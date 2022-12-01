#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>
#include <QJsonDocument>
#include "movie.h"
#include "geektv_constants.h"
#include "movie_search_widget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_searchButton_clicked();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkAccessManager;

    void updateSearchResult(const QVector<Movie> &moives);
};
#endif // MAINWINDOW_H
