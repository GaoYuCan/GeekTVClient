#include "main_window.h"
#include <QDebug>
#include <QApplication>

extern "C"{
    #include <libavutil/avutil.h>
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    qDebug() << av_version_info() << Qt::endl;
    return a.exec();
}
