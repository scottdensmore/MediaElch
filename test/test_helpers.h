#pragma once

#define CATCH_CONFIG_ENABLE_CHRONO_STRINGMAKER

// Catch2 is included in each test cpp file
#include "third_party/catch2/catch.hpp"

#include "test/helpers/debug_output.h"
#include "test/helpers/diff.h"
#include "test/helpers/matchers.h"

#include "data/movie/Movie.h"
#include "data/movie/MovieController.h"
#include "globals/Globals.h"
#include "scrapers/ScraperResult.h"

#include <QEventLoop>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVector>

/**
 * @brief Searches for searchStr and returns the results synchronously using the given Scraper.
 */
template<class ScraperInterfaceT>
QVector<ScraperSearchResult> searchScraperSync(ScraperInterfaceT& scraper, QString search)
{
    QVector<ScraperSearchResult> results;
    QEventLoop loop;
    loop.connect(&scraper, &ScraperInterfaceT::searchDone, [&](QVector<ScraperSearchResult> res) {
        results = res;
        loop.quit();
    });
    scraper.search(search);
    loop.exec();
    return results;
}

/**
 * @brief Loads movie data synchronously
 */
template<class ScraperInterfaceT>
void loadDataSync(ScraperInterfaceT& scraper,
    QHash<mediaelch::scraper::MovieScraper*, mediaelch::scraper::MovieIdentifier> ids,
    Movie& movie,
    QSet<MovieScraperInfo> infos)
{
    QEventLoop loop;
    loop.connect(movie.controller(), &MovieController::sigInfoLoadDone, &loop, &QEventLoop::quit);
    scraper.loadData(ids, &movie, infos);
    loop.exec();
}
