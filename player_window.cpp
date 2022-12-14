#include "player_window.h"
#include "ui_control_panel.h"
#include "ui_player_window.h"


PlayerWindow::PlayerWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::PlayerWindow), mPreview(new QtAV::VideoPreviewWidget), controler(new ControlPanel),
    player(new QtAV::AVPlayer(this)), videoItem(new QtAV::GraphicsItemRenderer), scene(new QGraphicsScene(this)), desktop(QApplication::desktop()),
    networkAccessManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    ui->verticalLayout->addWidget(controler);
    // 注册事件监听
    qApp->installEventFilter(this);
    // 设置选集树
    sourceTreeModel = new QStandardItemModel(ui->sourcesTree);
    ui->sourcesTree->setModel(sourceTreeModel);
    // 设置快捷键
    // 暂停播放
    connect(new QShortcut(QKeySequence(Qt::Key_Space), this), &QShortcut::activated, this, &PlayerWindow::playOrPause_triggered);
    // 进度调节
    connect(new QShortcut(QKeySequence(Qt::Key_Left), this), &QShortcut::activated, this, &PlayerWindow::seekBackword_triggered);
    connect(new QShortcut(QKeySequence(Qt::Key_Right), this), &QShortcut::activated, this, &PlayerWindow::seekForword_triggered);
    // 声音加减
    connect(new QShortcut(QKeySequence(Qt::Key_Up), this), &QShortcut::activated, this, &PlayerWindow::volumeUp_triggered);
    connect(new QShortcut(QKeySequence(Qt::Key_Down), this), &QShortcut::activated, this, &PlayerWindow::volumeDown_triggered);
    // 静音
    connect(new QShortcut(QKeySequence(Qt::Key_M), this), &QShortcut::activated, this, &PlayerWindow::toggleVolumeMute_triggered);
    // 全屏和关闭全屏
    connect(new QShortcut(QKeySequence(Qt::Key_F), this), &QShortcut::activated, this, &PlayerWindow::enterFullScreen_triggered);
    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated, this, &PlayerWindow::exitFullScreen_triggered);

    // 获取 controlerUI
    auto controlerUI = this->controler->getUI();
    // 配置播放器
    scene->addItem(videoItem);
    ui->graphicsView->setScene(scene);
    player->setRenderer(videoItem);
    connect(player->audio(), &QtAV::AudioOutput::volumeChanged, this, [controlerUI](qreal volume) {
        // 设置slider_volume
        controlerUI->slider_volume->setValue(volume * 100);
        controlerUI->slider_volume->setToolTip(QString::number(floor(volume * 100)));
    });
    connect(player->audio(),  &QtAV::AudioOutput::muteChanged, this, [controlerUI](bool isMute) {
        if(!isMute) {
            controlerUI->btn_volume->setIcon(QIcon(":/res/icon/resource/icon/volume_notice.png"));
        }else {
            controlerUI->btn_volume->setIcon(QIcon(":/res/icon/resource/icon/volume_mute.png"));
        }
    });

    connect(player, &QtAV::AVPlayer::positionChanged, this, [this, controlerUI](qint64 position) {
        // 判断是否正在拖动中，如果拖动中则不修改
        if(!controlerUI->slider_progress->isSliderDown()) {
            // 修改进度条
            controlerUI->slider_progress->setValue(position);
        }
        setCurTimeProgress(position);
        if(player->duration() - position < 100) {
            PlayerWindow::open(nullptr, nextKey);
        }
    });

    connect(player, &QtAV::AVPlayer::started, this, [this, controlerUI]() {
        // 设置进度条的最大值
        controlerUI->slider_progress->setRange(0, player->duration());
        // 根据视频的大小适配播放器界面大小
        if(player->statistics().video_only.width != 0 || player->statistics().video_only.height != 0) {
            // int w = player->statistics().video_only.width + (this->ui->sourcesTree->isHidden() ? 0 : this->ui->sourcesTree->width());
            // resize(w, player->statistics().video_only.height + controler->height());
            videoItem->resizeRenderer(ui->graphicsView->size());
        }
        // 获取音量
        controlerUI->slider_volume->setValue(player->audio()->volume() * 100.0);

    });
    connect(player, &QtAV::AVPlayer::stateChanged, this, [controlerUI](QtAV::AVPlayer::State state) {
        // 图标修改
        if(state == QtAV::AVPlayer::State::PlayingState) {
            controlerUI->btn_play->setIcon(QIcon(":/res/icon/resource/icon/pause.png"));
        }else {
            controlerUI->btn_play->setIcon(QIcon(":/res/icon/resource/icon/play.png"));
        }
    });
    // 绑定 controler 事件
    connect(controlerUI->btn_play, &QPushButton::pressed, this, &PlayerWindow::playOrPause_triggered);
    connect(controlerUI->btn_full, &QPushButton::pressed, this, &PlayerWindow::fullOrOffScreen_triggered);
    connect(controlerUI->btn_volume,&QPushButton::pressed, this, &PlayerWindow::toggleVolumeMute_triggered);
    connect(controlerUI->btn_list,&QPushButton::pressed, this, &PlayerWindow::showOrHideSourcesList_triggered);
    connect(controlerUI->slider_progress, &QSlider::sliderPressed, this, [this, controlerUI]() {});
    connect(controlerUI->slider_progress, &QSlider::sliderMoved, this, [this](int position) {
         this->player->setPosition(position);
    });
    connect(controlerUI->slider_progress, &QSlider::sliderReleased, this, [this, controlerUI]() {
        this->player->setPosition(controlerUI->slider_progress->value());
    });
    connect(controlerUI->slider_volume, &QSlider::sliderMoved, this, [this](int position) {
        this->player->audio()->setVolume(position / 100.0);
    });
    connect(controlerUI->slider_volume, &QSlider::sliderReleased, this, [this, controlerUI]() {
        this->player->audio()->setVolume(controlerUI->slider_volume->value() / 100.0);
    });

    // 点击切换剧集
    connect(ui->sourcesTree, &QTreeView::clicked, this, [this](const QModelIndex &index) {
        QVariant data = index.data(Qt::UserRole + 1);
        if(!data.isValid()) {
            return;
        }
        PlayerWindow::open(nullptr, data.toString());
    });

    // 设置 reply 的清理
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, [](QNetworkReply *reply) {
        reply->deleteLater();
    });
}

// 单例模式
PlayerWindow* PlayerWindow::playerWindow = nullptr;
PlayerWindow* PlayerWindow::getPlayerWindowInstance() {
    if(PlayerWindow::playerWindow == nullptr) {
        PlayerWindow::playerWindow = new PlayerWindow();
    }
    return PlayerWindow::playerWindow;
}

void PlayerWindow::open(QString title, QString key) {
    this->curKey = key;
    if(!title.isNull()) {
        setWindowTitle(title);
    }
    // 判断当前是否正在播放中
    if(player->isPlaying()) {
        // 停止播放
        player->stop();
    }
    // 构造解析链接URL
    QUrlQuery query;
    query.addQueryItem("key", key);
    QUrl url(QString(GeekTVConstants::LOCAL_SERVER_DOMAIN) + "/parse_url");
    url.setQuery(query);
    // 构造请求对象，并发送
    QNetworkRequest request(url);
    QNetworkReply* reply = networkAccessManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
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
             QMessageBox::information(nullptr, "抱歉", "解析视频失败，" + respJson["msg"].toString());
             return;
        }
        auto m3u8URL = respJson["data"].toString();
        qDebug() << "play url = " << m3u8URL << Qt::endl;
        player->play(m3u8URL);
    });

    // 获取选集列表
    fetchSourceList();
}

void PlayerWindow::fetchSourceList() {
    // 清除并设置头
    sourceTreeModel->clear();
    sourceTreeModel->setHorizontalHeaderLabels(QStringList() << "剧集");
    // 构造解析剧集URL
    QUrlQuery query;
    query.addQueryItem("key", this->curKey);
    QUrl url(QString(GeekTVConstants::LOCAL_SERVER_DOMAIN) + "/parse_series");
    url.setQuery(query);
    // 构造请求对象，并发送
    QNetworkRequest request(url);
    QNetworkReply* reply = networkAccessManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [reply, this]() {
        auto bytes = reply->readAll();
        QJsonParseError error;
        auto doc = QJsonDocument::fromJson(bytes, &error);
        if(error.error != QJsonParseError::NoError) {
            qDebug() << "JSON解析失败，error = " << error.errorString() << Qt::endl;
            return;
        }
        // 是否成功
        auto respJson = doc.object();
        // qDebug() << "code = " << respJson["code"] << ", msg = " << respJson["msg"]  << ", data = " << respJson["data"] << Qt::endl;
        if(respJson["code"].toInt() != 0) {
             QMessageBox::information(nullptr, "抱歉", "解析视频选集失败，" + respJson["msg"].toString());
             return;
        }
        // 解析剧集列表
        bool pre = false;
        auto sourceList = respJson["data"].toArray();
        for(auto sourceRef: sourceList) {
             QList<QStandardItem*> sourceItemList;
            auto source = sourceRef.toObject();
            QString sourceName = source["name"].toString();
            auto sourceItem = new QStandardItem(sourceName);
            sourceItem->setData(sourceName, Qt::UserRole);
            sourceItem->setEditable(false);
            sourceItemList << sourceItem;
            QList<QStandardItem *> seriesList;
            auto playList = source["playlist"].toArray();
            for(auto itemRef : playList) {
                auto item = itemRef.toObject();
                auto key = item["key"].toString();
                auto title = item["title"].toString();
                auto seriesItem = new QStandardItem(title);
                seriesItem->setData(key, Qt::UserRole + 1);
                seriesItem->setEditable(false);
                // 下一集
                if(pre) {
                    pre = false;
                    this->nextKey = key;
                }
                // 当前的
                if(key == this->curKey) {
                    seriesItem->setCheckState(Qt::Checked);
                    pre = true;
                }
                seriesList << seriesItem;
            }
            sourceItem->appendRows(seriesList);
            sourceTreeModel->appendRow(sourceItemList);
        }
    });
}

PlayerWindow::~PlayerWindow() {
    delete ui;
    delete player;
    delete mPreview;
    delete controler;
    delete videoItem;
    delete scene;
}

void PlayerWindow::closeEvent(QCloseEvent* event) {
    if(this->player->isPlaying()) {
        // 停止播放
        this->player->stop();
    }
}

void PlayerWindow::resizeEvent(QResizeEvent* event) {
    qDebug() << "event = " << event->size();
    videoItem->resizeRenderer(ui->graphicsView->size());
    if(ui->sourcesTree->isHidden()) {
        controler->resize(width(), controler->height());
    }else {
        controler->resize(width() - ui->sourcesTree->width(), controler->height());
    }

}

bool PlayerWindow::eventFilter(QObject* obj, QEvent* event) {
    Q_UNUSED(obj);
    if(event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        // 是否移动到了底部
        if(mouseEvent->pos().y() > (height() - controler->height())) {
            if(controler->isHidden()) {
                // 显示出来
                controler->show();
                // 3 秒后隐藏
                QTimer::singleShot(3000, this, [this](){
                    controler->hide();
                });
            }
        }
    }
    return false;
}

void PlayerWindow::setCurTimeProgress(qint64 cur) {
    QTime curTimeProgress(0, 0);
    curTimeProgress = curTimeProgress.addMSecs(static_cast<int>(cur));
    QTime totalTime(0, 0);
    totalTime = totalTime.addMSecs(player->duration());
    const QString curTimeProgressText = curTimeProgress.toString("hh:mm:ss");
    const QString totalTimeText = totalTime.toString("hh:mm:ss");
    controler->getUI()->vedio_time->setText(curTimeProgressText + "/" + totalTimeText);
    controler->getUI()->slider_progress->setToolTip(curTimeProgressText);
}

void PlayerWindow::enterFullScreen_triggered() {
    showFullScreen();
}

void PlayerWindow::exitFullScreen_triggered() {
    showNormal();
}

void PlayerWindow::fullOrOffScreen_triggered() {
    if(isFullScreen()) {
        exitFullScreen_triggered();
    }else {
        enterFullScreen_triggered();
    }
}

void PlayerWindow::volumeUp_triggered() {
    // 获取当前音量
    qreal vol = player->audio()->volume();
    // 最大是 2.0
    if (vol >= 2.0) {
        return;
    }
    vol += 0.01;
    player->audio()->setVolume(vol);
}

void PlayerWindow::volumeDown_triggered() {
    // 获取当前音量
    qreal vol = player->audio()->volume();
    // 最小音量 0
    if(vol <= 0) {
        return;
    }
    vol -= 0.01;
    player->audio()->setVolume(vol);
}

void PlayerWindow::toggleVolumeMute_triggered() {
    player->audio()->setMute(!player->audio()->isMute());
}

void PlayerWindow::playOrPause_triggered() {
    if(player->state() == player->PlayingState) {
        player->pause(true);
    }else if(player->state() == player->PausedState) {
        player->pause(false);
    } else if(player->state() == player->StoppedState) {
        player->play();
    }
}

void PlayerWindow::seekForword_triggered() {
        player->seekForward();
};
void PlayerWindow::seekBackword_triggered() {
        player->seekBackward();
};
void PlayerWindow::showOrHideSourcesList_triggered() {
    if(ui->sourcesTree->isHidden()) {
        ui->sourcesTree->show();
    }else {
        ui->sourcesTree->hide();
        ui->graphicsView->resize(ui->graphicsView->width() + ui->sourcesTree->width(), ui->graphicsView->height());
    }
    videoItem->resizeRenderer(ui->graphicsView->size());
};
