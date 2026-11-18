#include "diagram_plot_widget.h"
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QBrush>
#include <QPixmap>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QFontMetrics>
#include <cmath>
#include <algorithm>

DiagramPlotWidget::DiagramPlotWidget(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setMinimumSize(500, 380);
    setStyleSheet("background-color: #FFFFFF;");
}

void DiagramPlotWidget::setTitle(const QString& title) {
    m_title = title;
    update();
}

void DiagramPlotWidget::setMaterialNames(const QString& leftMat, const QString& rightMat) {
    m_leftMaterial = leftMat;
    m_rightMaterial = rightMat;
    update();
}

void DiagramPlotWidget::setBoundaryPositions(double xd1, double xd2) {
    m_xd1 = xd1;
    m_xd2 = xd2;
    update();
}

void DiagramPlotWidget::setData(const QVector<QPointF>& ec,
                               const QVector<QPointF>& ev,
                               const QVector<QPointF>& e0,
                               const QVector<QPointF>& ef)
{
    m_ec = ec;
    m_ev = ev;
    m_e0 = e0;
    m_ef = ef;
    update();
}

void DiagramPlotWidget::clear() {
    m_ec.clear();
    m_ev.clear();
    m_e0.clear();
    m_ef.clear();
    m_xd1 = 0.0;
    m_xd2 = 0.0;
    update();
}

void DiagramPlotWidget::saveImage(const QString& filePath) {
    QPixmap pixmap(size());
    render(&pixmap);
    pixmap.save(filePath);
}

void DiagramPlotWidget::mouseMoveEvent(QMouseEvent *event) {
    m_mousePos = event->pos();
    m_hasHover = true;
    update();
}

void DiagramPlotWidget::leaveEvent(QEvent *event) {
    Q_UNUSED(event);
    m_hasHover = false;
    update();
}

QPointF DiagramPlotWidget::toWidgetCoords(const QPointF& pt, const QRectF& plotRect, double xMin, double xMax, double yMin, double yMax) const {
    double px = plotRect.left() + (pt.x() - xMin) / (xMax - xMin) * plotRect.width();
    double py = plotRect.bottom() - (pt.y() - yMin) / (yMax - yMin) * plotRect.height();
    return QPointF(px, py);
}

QPointF DiagramPlotWidget::toDataCoords(const QPoint& widgetPos, const QRectF& plotRect, double xMin, double xMax, double yMin, double yMax) const {
    double x = xMin + (widgetPos.x() - plotRect.left()) / plotRect.width() * (xMax - xMin);
    double y = yMin + (plotRect.bottom() - widgetPos.y()) / plotRect.height() * (yMax - yMin);
    return QPointF(x, y);
}

void DiagramPlotWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    painter.fillRect(rect(), Qt::white);

    double marginLeft = 65.0;
    double marginRight = 35.0;
    double marginTop = 50.0;
    double marginBottom = 55.0;

    QRectF plotRect(marginLeft, marginTop, width() - marginLeft - marginRight, height() - marginTop - marginBottom);

    if (plotRect.width() <= 10 || plotRect.height() <= 10) return;

    QFont titleFont = painter.font();
    titleFont.setPointSize(11);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(QColor("#2C3E50"));
    painter.drawText(QRectF(0, 8, width(), 30), Qt::AlignCenter, m_title);

    if (m_ec.isEmpty() && m_ev.isEmpty()) {
        QFont emptyFont = painter.font();
        emptyFont.setPointSize(10);
        emptyFont.setItalic(true);
        painter.setFont(emptyFont);
        painter.setPen(Qt::gray);
        painter.drawText(plotRect, Qt::AlignCenter, "Нет данных для отображения.\nНажмите 'Рассчитать и построить'.");
        return;
    }

    double xMin = 1e9, xMax = -1e9;
    double yMin = 1e9, yMax = -1e9;

    auto updateBounds = [&](const QVector<QPointF>& pts) {
        for (const auto& p : pts) {
            xMin = std::min(xMin, p.x());
            xMax = std::max(xMax, p.x());
            yMin = std::min(yMin, p.y());
            yMax = std::max(yMax, p.y());
        }
    };

    updateBounds(m_ec);
    updateBounds(m_ev);
    updateBounds(m_e0);
    updateBounds(m_ef);

    if (xMin >= xMax) { xMin = -10.0; xMax = 10.0; }
    if (yMin >= yMax) { yMin = -5.0; yMax = 5.0; }

    double yRange = yMax - yMin;
    if (yRange < 0.1) yRange = 1.0;
    yMin -= yRange * 0.08;
    yMax += yRange * 0.12;

    painter.fillRect(plotRect, QColor("#FBFBFB"));

    if (m_xd1 > 0.0 || m_xd2 > 0.0) {
        double px_d1 = toWidgetCoords(QPointF(-m_xd1, 0), plotRect, xMin, xMax, yMin, yMax).x();
        double px_d2 = toWidgetCoords(QPointF(m_xd2, 0), plotRect, xMin, xMax, yMin, yMax).x();
        QRectF scrRect(px_d1, plotRect.top(), px_d2 - px_d1, plotRect.height());
        painter.fillRect(scrRect, QColor(255, 235, 205, 100)); 

        painter.setPen(QPen(QColor(230, 126, 34, 180), 1.5, Qt::DashLine));
        painter.drawLine(QPointF(px_d1, plotRect.top()), QPointF(px_d1, plotRect.bottom()));
        painter.drawLine(QPointF(px_d2, plotRect.top()), QPointF(px_d2, plotRect.bottom()));
    }

    QFont axisFont = painter.font();
    axisFont.setPointSize(8);
    axisFont.setBold(false);
    painter.setFont(axisFont);

    int xTicks = 8;
    for (int i = 0; i <= xTicks; ++i) {
        double xVal = xMin + i * (xMax - xMin) / xTicks;
        double px = toWidgetCoords(QPointF(xVal, 0), plotRect, xMin, xMax, yMin, yMax).x();

        painter.setPen(QPen(QColor("#E2E8F0"), 1, Qt::SolidLine));
        painter.drawLine(QPointF(px, plotRect.top()), QPointF(px, plotRect.bottom()));

        painter.setPen(QPen(QColor("#718096"), 1));
        painter.drawLine(QPointF(px, plotRect.bottom()), QPointF(px, plotRect.bottom() + 4));

        QString label = QString::number(xVal, 'f', 1);
        painter.drawText(QRectF(px - 25, plotRect.bottom() + 6, 50, 15), Qt::AlignCenter, label);
    }

    int yTicks = 7;
    for (int i = 0; i <= yTicks; ++i) {
        double yVal = yMin + i * (yMax - yMin) / yTicks;
        double py = toWidgetCoords(QPointF(0, yVal), plotRect, xMin, xMax, yMin, yMax).y();

        painter.setPen(QPen(QColor("#E2E8F0"), 1, Qt::SolidLine));
        painter.drawLine(QPointF(plotRect.left(), py), QPointF(plotRect.right(), py));

        painter.setPen(QPen(QColor("#718096"), 1));
        painter.drawLine(QPointF(plotRect.left() - 4, py), QPointF(plotRect.left(), py));

        QString label = QString::number(yVal, 'f', 2);
        painter.drawText(QRectF(0, py - 8, marginLeft - 8, 16), Qt::AlignRight | Qt::AlignVCenter, label);
    }

    if (xMin <= 0.0 && xMax >= 0.0) {
        double px0 = toWidgetCoords(QPointF(0, 0), plotRect, xMin, xMax, yMin, yMax).x();
        painter.setPen(QPen(QColor("#8E44AD"), 1.8, Qt::DashDotLine));
        painter.drawLine(QPointF(px0, plotRect.top()), QPointF(px0, plotRect.bottom()));

        painter.setPen(QColor("#8E44AD"));
        painter.drawText(QPointF(px0 + 4, plotRect.top() + 15), "x = 0");
    }

    painter.setPen(QPen(QColor("#A0AEC0"), 1.2));
    painter.drawRect(plotRect);

    QFont matFont = painter.font();
    matFont.setPointSize(10);
    matFont.setBold(true);
    painter.setFont(matFont);

    if (!m_leftMaterial.isEmpty()) {
        painter.setPen(QColor("#1F618D"));
        painter.drawText(QRectF(plotRect.left() + 10, plotRect.top() + 8, 200, 20), Qt::AlignLeft, m_leftMaterial);
    }
    if (!m_rightMaterial.isEmpty()) {
        painter.setPen(QColor("#922B21"));
        painter.drawText(QRectF(plotRect.right() - 210, plotRect.top() + 8, 200, 20), Qt::AlignRight, m_rightMaterial);
    }

    auto drawCurve = [&](const QVector<QPointF>& pts, const QPen& pen) {
        if (pts.size() < 2) return;
        painter.setPen(pen);
        QPainterPath path;
        bool started = false;
        for (const auto& pt : pts) {
            QPointF p = toWidgetCoords(pt, plotRect, xMin, xMax, yMin, yMax);
            if (!started) {
                path.moveTo(p);
                started = true;
            } else {
                path.lineTo(p);
            }
        }
        painter.drawPath(path);
    };

    drawCurve(m_e0, QPen(QColor("#27AE60"), 2.2, Qt::SolidLine));

    drawCurve(m_ec, QPen(QColor("#E74C3C"), 2.5, Qt::SolidLine));

    drawCurve(m_ev, QPen(QColor("#2980B9"), 2.5, Qt::SolidLine));

    drawCurve(m_ef, QPen(QColor("#2C3E50"), 2.0, Qt::DashLine));

    QFont labelFont = painter.font();
    labelFont.setPointSize(9);
    labelFont.setBold(true);
    painter.setFont(labelFont);
    painter.setPen(QColor("#2D3748"));

    painter.drawText(QRectF(plotRect.left(), height() - marginBottom + 26, plotRect.width(), 20), Qt::AlignCenter, "Координата x, нм");

    painter.save();
    painter.translate(14, plotRect.top() + plotRect.height() / 2);
    painter.rotate(-90);
    painter.drawText(QRectF(-100, -10, 200, 20), Qt::AlignCenter, "Энергия E, эВ");
    painter.restore();

    QRectF legRect(plotRect.right() - 170, plotRect.bottom() - 105, 160, 95);
    painter.fillRect(legRect, QColor(255, 255, 255, 220));
    painter.setPen(QPen(QColor("#CBD5E0"), 1));
    painter.drawRect(legRect);

    auto drawLegendItem = [&](double yOffset, const QColor& col, Qt::PenStyle style, const QString& text) {
        painter.setPen(QPen(col, 2.0, style));
        painter.drawLine(QPointF(legRect.left() + 8, legRect.top() + yOffset),
                         QPointF(legRect.left() + 28, legRect.top() + yOffset));
        painter.setPen(QColor("#2D3748"));
        QFont f = painter.font();
        f.setPointSize(8);
        f.setBold(false);
        painter.setFont(f);
        painter.drawText(QPointF(legRect.left() + 35, legRect.top() + yOffset + 4), text);
    };

    drawLegendItem(18, QColor("#E74C3C"), Qt::SolidLine, "E_c (зона провод.)");
    drawLegendItem(38, QColor("#2980B9"), Qt::SolidLine, "E_v (валентн. зона)");
    drawLegendItem(58, QColor("#27AE60"), Qt::SolidLine, "E_0 (уров. вакуума)");
    drawLegendItem(78, QColor("#2C3E50"), Qt::DashLine,  "E_F (уров. Ферми)");

    if (m_hasHover && plotRect.contains(m_mousePos)) {
        QPointF dataCoord = toDataCoords(m_mousePos, plotRect, xMin, xMax, yMin, yMax);

        painter.setPen(QPen(QColor(100, 100, 100, 140), 1, Qt::DotLine));
        painter.drawLine(QPointF(m_mousePos.x(), plotRect.top()), QPointF(m_mousePos.x(), plotRect.bottom()));
        painter.drawLine(QPointF(plotRect.left(), m_mousePos.y()), QPointF(plotRect.right(), m_mousePos.y()));

        QString coordText = QString("x: %1 нм, E: %2 эВ")
                                .arg(QString::number(dataCoord.x(), 'f', 2))
                                .arg(QString::number(dataCoord.y(), 'f', 3));

        QFont tipFont = painter.font();
        tipFont.setPointSize(8);
        painter.setFont(tipFont);
        QRect tipRect(m_mousePos.x() + 10, m_mousePos.y() - 25, 140, 20);
        if (tipRect.right() > plotRect.right()) tipRect.moveLeft(m_mousePos.x() - 150);
        if (tipRect.top() < plotRect.top()) tipRect.moveTop(m_mousePos.y() + 10);

        painter.fillRect(tipRect, QColor(40, 40, 40, 220));
        painter.setPen(Qt::white);
        painter.drawText(tipRect, Qt::AlignCenter, coordText);
    }
}
