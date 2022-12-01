#ifndef GEEKTVCONSTANTS_H
#define GEEKTVCONSTANTS_H

#include <QString>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>

class GeekTVConstants
{
public:
    GeekTVConstants() {}

    constexpr static const char* const LOCAL_SERVER_DOMAIN = "http://localhost:8080";

    static QString cacheDir() {
        return QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    }

    static QString coverCacheDir() {
        QString coverCachePath = cacheDir() + "/.cover";
        QDir coverCacheDir(coverCachePath);
        if(!coverCacheDir.exists()) {
            coverCacheDir.mkpath(coverCachePath);
        }
        return coverCachePath;
    }
};
#endif // GEEKTVCONSTANTS_H
