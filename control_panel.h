#ifndef CONTROL_PANEL_H
#define CONTROL_PANEL_H

#include <QWidget>

namespace Ui {
class ControlPanel;
}

class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPanel(QWidget *parent = nullptr);
    ~ControlPanel();

private:
    Ui::ControlPanel *ui;
};

#endif // CONTROL_PANEL_H
