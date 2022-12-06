#ifndef MOVIE_SEARCH_WIDGET_H
#define MOVIE_SEARCH_WIDGET_H
#include "movie.h"
#include <QWidget>
#include <QtNetwork>
#include <QStandardPaths>
#include "geektv_constants.h"

namespace Ui {
class MovieSearchWidget;
}

class MovieSearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MovieSearchWidget(const Movie &movie, QWidget *parent = nullptr);
    ~MovieSearchWidget();

    const Movie* getMovieRef() const {
        return &movie;
    }
private:
    Ui::MovieSearchWidget *ui;
    Movie movie;
    QNetworkAccessManager *networkAccessManager;
};

#endif // MOVIE_SEARCH_WIDGET_H
