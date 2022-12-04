#include "player_window.h"
#include "ui_player_window.h"

PlayerWindow::PlayerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlayerWindow)
{
    ui->setupUi(this);
}

PlayerWindow::~PlayerWindow()
{
    delete ui;
}
