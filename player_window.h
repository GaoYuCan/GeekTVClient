#ifndef PLAYER_WINDOW_H
#define PLAYER_WINDOW_H

#include <QMainWindow>
#include <QtAV/QtAV>
#include <QtAVWidgets/QtAVWidgets>
#include <QtAV/AVPlayer.h>
#include <QtNetwork>
#include <QDebug>
#include <QTimer>
#include <QDesktopWidget>
#include <QShortcut>
#include <QtMath>
#include <QMessageBox>
#include "control_panel.h"
#include "geektv_constants.h"

namespace Ui {
class PlayerWindow;
}

class PlayerWindow : public QMainWindow
{
    Q_OBJECT

public:
    ~PlayerWindow();
    void open(QString title, QString key);

    // 静态函数
    static PlayerWindow* getPlayerWindowInstance();
protected:
    void resizeEvent(QResizeEvent* event);
private:
    // 属性
    Ui::PlayerWindow *ui;
    bool isShowingSources;
    bool isPlaying;
    int volume;
    QtAV::AVPlayer* player;
    QGraphicsScene* scene;
    QtAV::GraphicsItemRenderer* videoItem;
    QtAV::VideoPreviewWidget* mPreview;
    ControlPanel* controler;
    QDesktopWidget* desktop;
    QNetworkAccessManager *networkAccessManager;

    // 构造函数
    explicit PlayerWindow(QWidget *parent = nullptr);
    // 普通函数
    void setCurTimeProgress(qint64 cur);
    bool eventFilter(QObject* obj, QEvent* event);
    // 静态变量
    static PlayerWindow* playerWindow;

private slots:
    void enterFullScreen_triggered();
    void exitFullScreen_triggered();
    void fullOrOffScreen_triggered();
    void showOrHideSourcesList_triggered();
    void volumeUp_triggered();
    void volumeDown_triggered();
    void toggleVolumeMute_triggered();
    void playOrPause_triggered();
    void seekForword_triggered();
    void seekBackword_triggered();

};

#endif // PLAYER_WINDOW_H
