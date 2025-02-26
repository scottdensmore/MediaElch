#include "test/test_helpers.h"

#include "scrapers/movie/tmdb/TmdbMovie.h"
#include "scrapers/movie/tmdb/TmdbMovieSearchJob.h"
#include "settings/Settings.h"
#include "test/scrapers/testScraperHelpers.h"

#include <chrono>

using namespace std::chrono_literals;
using namespace mediaelch::scraper;

static TmdbApi& getTmdbApi()
{
    static auto api = std::make_unique<TmdbApi>();
    if (!api->isInitialized()) {
        QEventLoop loop;
        QEventLoop::connect(api.get(), &TmdbApi::initialized, &loop, &QEventLoop::quit);
        api->initialize();
        loop.exec();
    }
    return *api;
}

TEST_CASE("TmdbMovie returns valid search results", "[TmdbMovie][search]")
{
    TmdbMovie tmdb;

    SECTION("Search by movie name returns correct results")
    {
        MovieSearchJob::Config config{"Finding Dory", mediaelch::Locale::English};
        auto* searchJob = new TmdbMovieSearchJob(getTmdbApi(), config);
        const auto scraperResults = searchMovieScraperSync(searchJob).first;

        REQUIRE(scraperResults.length() >= 2);
        CHECK(scraperResults[0].title == "Finding Dory");
        CHECK(scraperResults[1].title == "Marine Life Interviews");
    }
}

TEST_CASE("TmdbMovie scrapes correct movie details", "[TmdbMovie][load_data]")
{
    TmdbMovie tmdb;
    Settings::instance()->setUsePlotForOutline(true);

    SECTION("'Normal' movie loaded by using IMDb id")
    {
        Movie m(QStringList{}); // Movie without files
        loadDataSync(tmdb, {{nullptr, MovieIdentifier("tt2277860")}}, m, tmdb.scraperNativelySupports());

        REQUIRE(m.imdbId() == ImdbId("tt2277860"));
        REQUIRE(m.tmdbId() == TmdbId("127380"));

        test::scraper::compareAgainstReference(m, "scrapers/tmdb/Finding_Dory_tt2277860");
    }

    SECTION("'Normal' movie loaded by using TmdbMovie id")
    {
        Movie m(QStringList{}); // Movie without files
        loadDataSync(tmdb, {{nullptr, MovieIdentifier("127380")}}, m, tmdb.scraperNativelySupports());

        REQUIRE(m.imdbId() == ImdbId("tt2277860"));
        REQUIRE(m.tmdbId() == TmdbId("127380"));

        test::scraper::compareAgainstReference(m, "scrapers/tmdb/Finding_Dory_tmdb127380");
    }

    SECTION("Scraping movie two times does not increase actor count")
    {
        Movie m(QStringList{}); // Movie without files

        // load first time
        loadDataSync(tmdb, {{nullptr, MovieIdentifier("tt2277860")}}, m, tmdb.scraperNativelySupports());
        REQUIRE(m.imdbId() == ImdbId("tt2277860"));
        REQUIRE(m.actors().size() == 32);

        // load second time
        loadDataSync(tmdb, {{nullptr, MovieIdentifier("tt2277860")}}, m, tmdb.scraperNativelySupports());
        REQUIRE(m.imdbId() == ImdbId("tt2277860"));
        REQUIRE(m.actors().size() == 32);
    }
}
