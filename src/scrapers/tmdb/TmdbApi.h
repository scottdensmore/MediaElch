#pragma once

#include "data/Locale.h"
#include "data/TmdbId.h"
#include "data/tv_show/EpisodeNumber.h"
#include "data/tv_show/SeasonNumber.h"
#include "data/tv_show/SeasonOrder.h"
#include "network/NetworkManager.h"
#include "network/NetworkRequest.h"
#include "network/WebsiteCache.h"
#include "scrapers/ScraperError.h"
#include "scrapers/ScraperInfos.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QObject>
#include <QString>
#include <QUrl>
#include <functional>

namespace mediaelch {
namespace scraper {

struct TmdbApiConfiguration
{
    QString imageBaseUrl = "http://image.tmdb.org/t/p/";
    QString imageSecureBaseUrl = "https://image.tmdb.org/t/p/";
    QStringList backdropSizes;
    QStringList logoSizes;
    QStringList posterSizes;
    QStringList profileSizes;
    QStringList stillSizes;

    static TmdbApiConfiguration from(QJsonDocument doc);
};

/// \brief API interface for TheTvDb
class TmdbApi : public QObject
{
    Q_OBJECT

public:
    explicit TmdbApi(QObject* parent = nullptr);
    ~TmdbApi() override = default;

    void initialize();
    bool isInitialized() const;

public:
    const TmdbApiConfiguration& config() const;

public:
    using ApiCallback = std::function<void(QJsonDocument, ScraperError)>;

    enum class ApiMovieDetails
    {
        INFOS,
        IMAGES,
        CASTS,
        TRAILERS,
        RELEASES
    };
    enum class ApiUrlParameter
    {
        YEAR,
        PAGE,
        INCLUDE_ADULT
    };
    using UrlParameterMap = QMap<ApiUrlParameter, QString>;

public:
    static QString apiKey();

public:
    void sendGetRequest(const Locale& locale, const QUrl& url, ApiCallback callback);

    void searchForShow(const Locale& locale, const QString& query, bool includeAdult, ApiCallback callback);
    void loadShowInfos(const Locale& locale, const TmdbId& id, ApiCallback callback);
    void loadMinimalInfos(const Locale& locale, const TmdbId& id, ApiCallback callback);

    void loadEpisode(const Locale& locale,
        const TmdbId& showId,
        SeasonNumber season,
        EpisodeNumber episode,
        ApiCallback callback);
    void loadSeason(const Locale& locale,
        const TmdbId& showId,
        SeasonNumber season,
        SeasonOrder order,
        ApiCallback callback);

    // Concerts

    void searchForConcert(const Locale& locale, const QString& query, ApiCallback callback);

signals:
    void initialized(bool wasSuccessful);

public:
    QUrl makeImageUrl(const QString& suffix) const;
    QUrl makeApiUrl(const QString& suffix, const Locale& locale, QUrlQuery query) const;

private:
    // TV shows
    QUrl getShowUrl(const TmdbId& id, const Locale& locale, bool onlyBasicDetails = false) const;
    QUrl getShowSearchUrl(const QString& searchStr, const Locale& locale, bool includeAdult) const;
    QUrl getEpisodeUrl(const TmdbId& showId, SeasonNumber season, EpisodeNumber episode, const Locale& locale) const;
    QUrl getSeasonUrl(const TmdbId& showId, SeasonNumber season, const Locale& locale) const;

public:
    // TODO: Make these private when the TMDb movie scraper has switched to the job-based model.

    // Movies
    QUrl getMovieSearchUrl(const QString& searchStr,
        const Locale& locale,
        bool includeAdult,
        const UrlParameterMap& parameters) const;

    QString apiUrlParameterString(ApiUrlParameter parameter) const;
    QUrl getMovieUrl(QString movieId,
        const Locale& locale,
        ApiMovieDetails type,
        const UrlParameterMap& parameters = UrlParameterMap{}) const;
    QUrl getCollectionUrl(QString collectionId, const Locale& locale) const;

private:
    const QString m_language;
    network::NetworkManager m_network;
    WebsiteCache m_cache;
    TmdbApiConfiguration m_config;
    bool m_isInitialized = false;
};

} // namespace scraper
} // namespace mediaelch
