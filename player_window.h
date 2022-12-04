#ifndef PLAYER_WINDOW_H
#define PLAYER_WINDOW_H

#include <QMainWindow>

namespace Ui {
class PlayerWindow;
}

class PlayerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlayerWindow(QWidget *parent = nullptr);
    ~PlayerWindow();

private:
    Ui::PlayerWindow *ui;

private slots:
    void enterFullscreen();
    void exitFullscreen();

};

#endif // PLAYER_WINDOW_H
