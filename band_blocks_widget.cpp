#include "band_blocks_widget.h"
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QFontMetrics>
#include <algorithm>

BandBlocksWidget::BandBlocksWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(600, 420);
    setStyleSheet("background-color: #1E1F26;");
}

void BandBlocksWidget::setMaterials(const QString& name1, double Eg1, double chi1, bool isMetal1,
                                   const QString& name2, double Eg2, double chi2, bool isMetal2)
{
    m_name1 = name1;
    m_Eg1 = Eg1;
    m_chi1 = chi1;
    m_isMetal1 = isMetal1;

    m_name2 = name2;
    m_Eg2 = Eg2;
    m_chi2 = chi2;
    m_isMetal2 = isMetal2;

    update();
}

void BandBlocksWidget::saveImage(const QString& filePath) {
    QPixmap pixmap(size() * 2);
    pixmap.setDevicePixelRatio(2.0);
    render(&pixmap);
    pixmap.save(filePath);
}

void BandBlocksWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    painter.fillRect(rect(), QColor("#1E1F26"));

    int w = width();
    int h = height();

    double axisLeft = 45.0;
    double axisTop = 30.0;
    double axisBottom = h - 60.0;

    painter.setPen(QPen(QColor("#B8B7C5"), 1.8));
    painter.drawLine(QPointF(axisLeft, axisBottom), QPointF(axisLeft, axisTop));

    painter.drawLine(QPointF(axisLeft, axisTop), QPointF(axisLeft - 4, axisTop + 8));
    painter.drawLine(QPointF(axisLeft, axisTop), QPointF(axisLeft + 4, axisTop + 8));

    painter.save();
    painter.translate(axisLeft - 18, (axisTop + axisBottom) / 2);
    painter.rotate(-90);
    QFont axisFont("Montserrat");
    axisFont.setPointSize(9);
    axisFont.setBold(true);
    painter.setFont(axisFont);
    painter.setPen(QColor("#A8A7B6"));
    painter.drawText(QRectF(-120, -10, 240, 20), Qt::AlignCenter, "энергия электронов");
    painter.restore();

    double contentLeft = axisLeft + 25.0;
    double contentWidth = w - contentLeft - 30.0;
    if (contentWidth < 200) return;

    int cols = 4;
    double colWidth = contentWidth / cols;
    double blockWidth = std::min(colWidth * 0.72, 140.0);

    double fermiY = (axisTop + axisBottom) * 0.48;

    painter.setPen(QPen(QColor("#7A7988"), 1.5, Qt::DashLine));
    painter.drawLine(QPointF(axisLeft - 5, fermiY), QPointF(w - 20, fermiY));

    QFont fermiFont("Montserrat");
    fermiFont.setPointSize(8);
    fermiFont.setBold(true);
    painter.setFont(fermiFont);
    painter.setPen(QColor("#D6D5E2"));
    painter.drawText(QPointF(w - 110, fermiY - 6), "уровень Ферми");

    QColor colConduction("#008CD2");
    QColor colValence("#D32F2F");
    QColor colText("#EDEDF4");

    QFont blockFont("Montserrat");
    blockFont.setPointSize(8);
    blockFont.setBold(true);

    QFont subFont("Montserrat");
    subFont.setPointSize(9);
    subFont.setBold(true);

    auto drawColumn = [&](int idx, const QString& title, double topY, double egHeight, bool isConductor, double customEg) {
        double cx = contentLeft + idx * colWidth + (colWidth - blockWidth) / 2;
        double cbTop = topY;
        double cbBottom = isConductor ? (fermiY + 22) : (fermiY - egHeight / 2);

        if (cbBottom < cbTop + 30) cbBottom = cbTop + 50;

        QRectF cbRect(cx, cbTop, blockWidth, cbBottom - cbTop);
        painter.fillRect(cbRect, colConduction);
        painter.setPen(QPen(QColor("#155A8A"), 1));
        painter.drawRect(cbRect);

        double vbTop = isConductor ? (fermiY - 22) : (fermiY + egHeight / 2);
        double vbBottom = axisBottom - 15;
        if (vbBottom < vbTop + 30) vbBottom = vbTop + 50;

        QRectF vbRect(cx, vbTop, blockWidth, vbBottom - vbTop);
        painter.fillRect(vbRect, colValence);
        painter.setPen(QPen(QColor("#8C1D1D"), 1));
        painter.drawRect(vbRect);

        if (isConductor) {
            double ovTop = fermiY - 22;
            double ovBottom = fermiY + 22;
            QRectF ovRect(cx, ovTop, blockWidth, ovBottom - ovTop);
            painter.fillRect(ovRect, colValence);

            QBrush hatch(colConduction, Qt::BDiagPattern);
            painter.fillRect(ovRect, hatch);
            painter.setPen(QPen(QColor("#EDEDF4"), 1));
            painter.drawRect(ovRect);

            painter.setFont(blockFont);
            painter.setPen(colText);
            painter.drawText(ovRect, Qt::AlignCenter, "зона\nперекрытия");
        } else {
            painter.setFont(blockFont);
            painter.setPen(colText);
            painter.drawText(cbRect, Qt::AlignCenter, "зона\nпроводимости");
            painter.drawText(vbRect, Qt::AlignCenter, "валентная\nзона");

            QRectF gapRect(cx, cbBottom, blockWidth, vbTop - cbBottom);
            if (gapRect.height() >= 14) {
                QFont egFont("Montserrat");
                egFont.setPointSize(8);
                egFont.setBold(true);
                painter.setFont(egFont);
                painter.setPen(QColor("#F4D03F"));
                QString egStr = QString("Eg = %1 эВ").arg(QString::number(customEg, 'f', 2));
                painter.drawText(gapRect, Qt::AlignCenter, egStr);
            }
        }

        painter.setFont(subFont);
        painter.setPen(QColor("#D6D5E2"));
        QRectF titleRect(cx - 15, axisBottom + 10, blockWidth + 30, 40);
        painter.drawText(titleRect, Qt::AlignHCenter | Qt::AlignTop, title);
    };

    drawColumn(0, "проводник\n(металл)", axisTop + 15, 0, true, 0.0);

    double egScale1 = std::clamp(m_Eg1 * 32.0, 20.0, 110.0);
    QString title1 = QString("%1\n(Материал 1)").arg(m_name1);
    drawColumn(1, title1, axisTop + 20, egScale1, m_isMetal1, m_Eg1);

    double egScale2 = std::clamp(m_Eg2 * 32.0, 20.0, 110.0);
    QString title2 = QString("%1\n(Материал 2)").arg(m_name2);
    drawColumn(2, title2, axisTop + 20, egScale2, m_isMetal2, m_Eg2);

    drawColumn(3, "диэлектрик\n(изолятор)", axisTop + 20, 125.0, false, 5.50);
}
