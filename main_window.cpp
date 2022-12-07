#include "main_window.h"
#include "./ui_main_window.h"
#include "ui_movie_search_widget.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), networkAccessManager(new QNetworkAccessManager(this)) {
    ui->setupUi(this);
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, [](QNetworkReply *reply) {
        reply->deleteLater();
    });

}

MainWindow::~MainWindow() {
    delete ui;
    delete networkAccessManager;
}


void MainWindow::on_searchButton_clicked() {
    GeekTVConstants::cacheDir();
    auto inputText = ui->searchEdit->text();
    if(inputText.isEmpty()) {
        QMessageBox::information(nullptr, "温馨提示", "请不要输入空气哦！");
        return;
    }
    // 构造搜索URL
    QUrlQuery query;
    query.addQueryItem("keyword", inputText);
    QUrl url(QString(GeekTVConstants::LOCAL_SERVER_DOMAIN) + "/search");
    url.setQuery(query);
    // 构造请求对象，并发送
    QNetworkRequest request(url);
    QNetworkReply* reply = networkAccessManager->get(request);
    connect(reply, &QNetworkReply::readyRead, this, [this, reply](){
        auto bytes = reply->readAll();
        QJsonParseError error;
        auto doc = QJsonDocument::fromJson(bytes, &error);
        if(error.error != QJsonParseError::NoError) {
            qDebug() << "JSON解析失败，error = " << error.errorString() << Qt::endl;
            return;
        }
        // 是否成功
        auto respJson = doc.object();
        qDebug() << "code = " << respJson["code"] << ", msg = " << respJson["msg"]  << ", data = " << respJson["data"] << Qt::endl;
        if(respJson["code"].toInt() != 0) {
             QMessageBox::information(nullptr, "抱歉", "搜索失败，" + respJson["msg"].toString());
             return;
        }
        // 解析 JSON 获取列表
        auto moviesJsonArray = respJson["data"].toArray();
        QVector<Movie> moviesList;
        for(auto movieValRef : moviesJsonArray) {
            auto movieJsonObject = movieValRef.toObject();
            QString actors = movieJsonObject["actor"].toString();
            QString category = movieJsonObject["category"].toString();
            QString coverURL = movieJsonObject["coverURL"].toString();
            QString intro = movieJsonObject["intro"].toString();
            QString title = movieJsonObject["title"].toString();
            QString key = movieJsonObject["key"].toString();
            moviesList += Movie(title, coverURL, key, category, actors, intro);
        }
        // 刷新UI
        updateSearchResult(moviesList);
    });
}


void MainWindow::updateSearchResult(const QVector<Movie> &movies) {
    static QVector<QListWidgetItem*> previousSearchResult;
    for (QListWidgetItem *item : previousSearchResult) {
        auto widget = ui->mainList->itemWidget(item);
        ui->mainList->removeItemWidget(item);
        delete widget;
    }
    ui->mainList->clear();
    previousSearchResult.clear(); // 清空
    for(auto &movie : movies) {
       auto item =  new QListWidgetItem(ui->mainList, 0);
       item->setData(Qt::UserRole, movie.key);
       auto itemSize = QSize(ui->mainList->width() - 21, 170);
       item->setSizeHint(itemSize);
       auto searchMovieView = new MovieSearchWidget(movie, ui->mainList);
       searchMovieView->setFixedSize(itemSize);
       ui->mainList->setItemWidget(item, searchMovieView);
       previousSearchResult += item;
    }
    connect(ui->mainList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        MovieSearchWidget *widget = static_cast<MovieSearchWidget *>(ui->mainList->itemWidget(item));
        const Movie* m = widget->getMovieRef();
        PlayerWindow* w = PlayerWindow::getPlayerWindowInstance();
        w->open(m->title, m->key);
        w->show();
    });
}

