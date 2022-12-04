#include "control_panel.h"
#include "ui_control_panel.h"

ControlPanel::ControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel)
{
    ui->setupUi(this);
    QPalette pal(palette());
    pal.setColor(QPalette::Window, QColor(0, 0, 0, 0));
    this->setPalette(pal);
}

ControlPanel::~ControlPanel()
{
    delete ui;
}
