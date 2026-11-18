#include <QApplication>
#include <QStyleFactory>
#include "mainwindow.h"

int main(int argc, char *argv[]) {

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app(argc, argv);
    app.setApplicationName("HeterojunctionLab");
    app.setApplicationDisplayName("Моделирование энергетических зонных диаграмм гетеропереходов");

    app.setStyle(QStyleFactory::create("Fusion"));

    MainWindow window;
    window.show();

    return app.exec();
}
