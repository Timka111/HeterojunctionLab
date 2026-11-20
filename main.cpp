#include <QApplication>
#include <QStyleFactory>
#include <QFont>
#include <QIcon>
#include <QFile>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app(argc, argv);
    app.setApplicationName("HeterojunctionLab");
    app.setApplicationDisplayName("HeterojunctionLab");

    QIcon appIcon(":/app_icon.png");
    if (appIcon.isNull()) appIcon = QIcon("app_icon.png");
    if (appIcon.isNull()) appIcon = QIcon("app_icon.ico");
    app.setWindowIcon(appIcon);

    QFont appFont("Montserrat");
    appFont.setStyleHint(QFont::SansSerif);
    appFont.setPointSize(9);
    app.setFont(appFont);

    app.setStyle(QStyleFactory::create("Fusion"));

    const char* qss = R"(
        QWidget {
            background-color: #1E1F26;
            color: #EDEDF4;
            font-family: "Montserrat", "Segoe UI", sans-serif;
            font-size: 12px;
        }

        QMainWindow {
            background-color: #1E1F26;
        }

        QScrollArea {
            border: none;
            background-color: transparent;
        }

        QScrollBar:vertical {
            background: #1E1F26;
            width: 7px;
            margin: 0;
            border-radius: 3px;
        }
        QScrollBar::handle:vertical {
            background: #3B3C48;
            min-height: 24px;
            border-radius: 3px;
        }
        QScrollBar::handle:vertical:hover {
            background: #689F8F;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }

        QGroupBox {
            background-color: #262732;
            border: 1px solid #3B3C48;
            border-radius: 6px;
            margin-top: 10px;
            padding-top: 10px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 10px;
            padding: 0 6px;
            font-size: 12px;
            font-weight: bold;
            background-color: #262732;
        }

        QLabel {
            color: #9A99A8;
            background: transparent;
            font-size: 11px;
        }

        QLineEdit, QDoubleSpinBox, QComboBox {
            background-color: #1E1F26;
            border: 1px solid #3B3C48;
            border-radius: 4px;
            padding: 4px 6px;
            color: #EDEDF4;
            selection-background-color: #689F8F;
            selection-color: #16171D;
        }
        QLineEdit:focus, QDoubleSpinBox:focus, QComboBox:focus {
            border: 1px solid #689F8F;
        }
        QLineEdit:hover, QDoubleSpinBox:hover, QComboBox:hover {
            border: 1px solid #5A5968;
        }

        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border-left: 1px solid #3B3C48;
        }
        QComboBox QAbstractItemView {
            background-color: #262732;
            border: 1px solid #3B3C48;
            border-radius: 4px;
            color: #EDEDF4;
            selection-background-color: #689F8F;
            selection-color: #16171D;
            padding: 4px;
            outline: none;
        }

        QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
            background: #262732;
            border: none;
            width: 14px;
        }
        QDoubleSpinBox::up-button:hover, QDoubleSpinBox::down-button:hover {
            background: #689F8F;
        }

        QTabWidget::pane {
            border: 1px solid #3B3C48;
            border-radius: 6px;
            background-color: #1E1F26;
            top: -1px;
        }
        QTabBar::tab {
            background-color: #262732;
            color: #9A99A8;
            padding: 8px 16px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            margin-right: 3px;
            border: 1px solid #3B3C48;
            border-bottom: none;
            font-weight: bold;
            font-size: 11px;
        }
        QTabBar::tab:selected {
            background-color: #1E1F26;
            color: #EDEDF4;
            border-top: 2px solid #689F8F;
        }
        QTabBar::tab:hover:!selected {
            background-color: #31323D;
            color: #D6D5E2;
        }

        QTableWidget {
            background-color: #1E1F26;
            alternate-background-color: #262732;
            color: #EDEDF4;
            gridline-color: #2C2D38;
            border: 1px solid #3B3C48;
            border-radius: 6px;
            selection-background-color: #689F8F;
            selection-color: #16171D;
        }
        QTableWidget::item {
            padding: 4px 6px;
        }
        QHeaderView::section {
            background-color: #262732;
            color: #9A99A8;
            padding: 6px;
            border: none;
            border-bottom: 2px solid #689F8F;
            font-weight: bold;
        }
    )";

    app.setStyleSheet(qss);

    MainWindow window;
    window.setWindowIcon(appIcon);
    window.showMaximized();

    return app.exec();
}
