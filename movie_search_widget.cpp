#include "movie_search_widget.h"
#include "ui_movie_search_widget.h"

MovieSearchWidget::MovieSearchWidget(const Movie &movie, QWidget *parent) :
    QWidget(parent), movie(movie), ui(new Ui::MovieSearchWidget), networkAccessManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    // 设置 reply 的清理
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, [](QNetworkReply *reply) {
        reply->deleteLater();
    });
    // 设置数据
    ui->title->setText(this->movie.title);
    ui->category->setText(this->movie.category);
    ui->actor->setText(this->movie.actor);
    ui->intro->setText(this->movie.intro);
    ui->cover->setIcon(QIcon(":res/image/loading.png")); // 加载 “加载中”

    // 判断是否存在本地缓存
    auto coverPath = GeekTVConstants::coverCacheDir() + "/" + movie.key;
    QFileInfo coverFileInfo(coverPath);
    if (coverFileInfo.isFile()) {
        ui->cover->setIcon(QIcon(coverPath));
    }else {
        // 加载网络图片，并缓存
        QNetworkRequest request(QUrl(this->movie.coverURL));
        auto reply = networkAccessManager->get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply, coverPath]() {
            QFile coverFile(coverPath);
            auto content = reply->readAll();
            if(coverFile.open(QIODevice::WriteOnly)) {
                coverFile.write(content.constData(), content.size());
            }
            coverFile.close();
            ui->cover->setIcon(QIcon(coverPath));
        });
    }
}

MovieSearchWidget::~MovieSearchWidget() {
    delete ui;
    delete networkAccessManager;
}
