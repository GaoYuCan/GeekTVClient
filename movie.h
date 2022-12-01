#ifndef MOVIE_H
#define MOVIE_H
#include<QString>

class Movie
{
public:

    Movie();
    Movie(const QString &title, const QString &coverURL, const QString &key, const QString &category, const QString &actor, const QString &intro);

    QString title;
    QString coverURL;
    QString key;
    QString category;
    QString actor;
    QString intro;
};

#endif // MOVIE_H
