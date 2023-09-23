#include "QtStableDiffusion.h"
#include <QtWidgets/QApplication>

#define title_color "#ff00d9"

void init_app()
{
    srand(time(0));

}



int main(int argc, char *argv[])
{
    init_app();

    QApplication a(argc, argv);

    QPixmap pixmap;

    switch (rand() % 5)
    {
        case 0:pixmap.load(":/start/splash/img/Rounded_splash_1.png"); break;      //绑定一个图片
        case 1:pixmap.load(":/start/splash/img/Rounded_splash_2.png"); break;
        case 2:pixmap.load(":/start/splash/img/Rounded_splash_3.png"); break;
        case 3:pixmap.load(":/start/splash/img/Rounded_splash_4.png"); break;
        case 4:pixmap.load(":/start/splash/img/Rounded_splash_5.png"); break;
        default:break;
    }

    QSplashScreen splash;
    splash.setPixmap(pixmap);
    splash.show();             //显示启动画面    
    QThread::msleep(200);
    splash.showMessage(QString::fromStdString("<font size=\"12\" face=\"Console\" color=\"") + title_color + "\">Qt Stable Diffusion</font>", Qt::AlignHCenter | Qt::AlignVCenter);

    a.processEvents();
    QThread::msleep(1300);

    splash.finish(nullptr);

    QtStableDiffusion w;
    w.show();
    return a.exec();
}
