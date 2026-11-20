#include "potential_plot_widget.h"
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <cmath>
#include <algorithm>

PotentialPlotWidget::PotentialPlotWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(540, 400);
    setStyleSheet("background-color: #1E1F26;");
    recalculate();
}

void PotentialPlotWidget::setParams(double xd1, double xd2, double qphi01, double qphi02, double phi0) {
    m_xd1 = xd1;
    m_xd2 = xd2;
    m_qphi01 = qphi01;
    m_qphi02 = qphi02;
    m_phi0 = phi0;
    recalculate();
    update();
}

void PotentialPlotWidget::recalculate() {
    m_potentialPoints.clear();
    m_fieldPoints.clear();

    double totalW = m_xd1 + m_xd2;
    if (totalW <= 0.0) totalW = 20.0;

    double xmin = -1.5 * totalW;
    double xmax = 1.5 * totalW;

    int steps = 240;
    double dx = (xmax - xmin) / steps;

    double emax = (m_xd1 > 0.0 && m_xd2 > 0.0) ? (2.0 * m_phi0 / (totalW * 1e-7)) : 1.0;

    for (int i = 0; i <= steps; ++i) {
        double x = xmin + i * dx;
        double phi = 0.0;
        double field = 0.0;

        if (x <= -m_xd1) {
            phi = 0.0;
            field = 0.0;
        } else if (x <= 0.0) {
            double frac = (x + m_xd1) / m_xd1;
            phi = m_qphi01 * frac * frac;
            field = (x + m_xd1) / m_xd1;
        } else if (x <= m_xd2) {
            double frac = (m_xd2 - x) / m_xd2;
            phi = m_phi0 - m_qphi02 * frac * frac;
            field = (m_xd2 - x) / m_xd2;
        } else {
            phi = m_phi0;
            field = 0.0;
        }

        m_potentialPoints.append(QPointF(x, phi));
        m_fieldPoints.append(QPointF(x, field));
    }
}

void PotentialPlotWidget::saveImage(const QString& filePath) {
    QPixmap pixmap(size() * 2);
    pixmap.setDevicePixelRatio(2.0);
    render(&pixmap);
    pixmap.save(filePath);
}

void PotentialPlotWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    painter.fillRect(rect(), QColor("#1E1F26"));

    double marginLeft = 70.0;
    double marginRight = 35.0;
    double marginTop = 45.0;
    double marginBottom = 50.0;

    QRectF plotRect(marginLeft, marginTop, width() - marginLeft - marginRight, height() - marginTop - marginBottom);
    if (plotRect.width() <= 10 || plotRect.height() <= 10) return;

    QFont titleFont("Montserrat");
    titleFont.setPointSize(11);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(QColor("#EDEDF4"));
    painter.drawText(QRectF(0, 10, width(), 25), Qt::AlignCenter, "Распределение электростатического потенциала φ(x) и поля E(x) в ОПЗ");

    double totalW = m_xd1 + m_xd2;
    double xMin = -1.5 * totalW;
    double xMax = 1.5 * totalW;

    QLinearGradient bgGrad(plotRect.topLeft(), plotRect.bottomLeft());
    bgGrad.setColorAt(0.0, QColor("#1C1B2E"));
    bgGrad.setColorAt(1.0, QColor("#161524"));
    painter.fillRect(plotRect, bgGrad);

    auto toWidgetX = [&](double x) {
        return plotRect.left() + (x - xMin) / (xMax - xMin) * plotRect.width();
    };

    double px_d1 = toWidgetX(-m_xd1);
    double px_d2 = toWidgetX(m_xd2);
    double px0 = toWidgetX(0.0);

    QRectF scrRect(px_d1, plotRect.top(), px_d2 - px_d1, plotRect.height());
    painter.fillRect(scrRect, QColor(122, 168, 153, 30));

    painter.setPen(QPen(QColor("#729B8C"), 1.2, Qt::DashLine));
    painter.drawLine(QPointF(px_d1, plotRect.top()), QPointF(px_d1, plotRect.bottom()));
    painter.setPen(QPen(QColor("#7E749B"), 1.2, Qt::DashLine));
    painter.drawLine(QPointF(px_d2, plotRect.top()), QPointF(px_d2, plotRect.bottom()));
    painter.setPen(QPen(QColor("#EDEDF4"), 1.5, Qt::DashDotLine));
    painter.drawLine(QPointF(px0, plotRect.top()), QPointF(px0, plotRect.bottom()));

    QFont markFont("Montserrat");
    markFont.setPointSize(8);
    markFont.setBold(true);
    painter.setFont(markFont);
    painter.setPen(QColor("#729B8C"));
    painter.drawText(QPointF(px_d1 - 42, plotRect.top() + 24), "-x_d1");
    painter.setPen(QColor("#7E749B"));
    painter.drawText(QPointF(px_d2 + 6, plotRect.top() + 24), "+x_d2");
    painter.setPen(QColor("#EDEDF4"));
    painter.drawText(QPointF(px0 + 5, plotRect.top() + 15), "x = 0");

    double yMax = m_phi0 * 1.15;
    if (yMax <= 0.1) yMax = 1.5;

    auto toWidgetY_phi = [&](double phi) {
        return plotRect.bottom() - (phi / yMax) * plotRect.height();
    };

    auto toWidgetY_field = [&](double field) {
        return plotRect.bottom() - (field / 1.15) * plotRect.height();
    };

    QPainterPath pathPhi;
    bool sPhi = false;
    for (const auto& pt : m_potentialPoints) {
        QPointF p(toWidgetX(pt.x()), toWidgetY_phi(pt.y()));
        if (!sPhi) { pathPhi.moveTo(p); sPhi = true; } else { pathPhi.lineTo(p); }
    }
    painter.setPen(QPen(QColor("#729B8C"), 2.6));
    painter.drawPath(pathPhi);

    QPainterPath pathField;
    bool sF = false;
    for (const auto& pt : m_fieldPoints) {
        QPointF p(toWidgetX(pt.x()), toWidgetY_field(pt.y()));
        if (!sF) { pathField.moveTo(p); sF = true; } else { pathField.lineTo(p); }
    }
    painter.setPen(QPen(QColor("#F4D03F"), 2.2, Qt::DashLine));
    painter.drawPath(pathField);

    painter.setPen(QPen(QColor("#454359"), 1.2));
    painter.drawRect(plotRect);

    QFont labelFont("Montserrat");
    labelFont.setPointSize(9);
    labelFont.setBold(true);
    painter.setFont(labelFont);
    painter.setPen(QColor("#C7C5D8"));
    painter.drawText(QRectF(plotRect.left(), height() - marginBottom + 24, plotRect.width(), 20), Qt::AlignCenter, "Координата x, нм");

    painter.save();
    painter.translate(16, plotRect.top() + plotRect.height() / 2);
    painter.rotate(-90);
    painter.drawText(QRectF(-110, -10, 220, 20), Qt::AlignCenter, "Потенциал φ(x), В / Поле (отн. ед.)");
    painter.restore();

    QRectF legRect(plotRect.right() - 200, plotRect.top() + 15, 185, 60);
    painter.fillRect(legRect, QColor(30, 31, 38, 235));
    painter.setPen(QPen(QColor("#3B3C48"), 1));
    painter.drawRoundedRect(legRect, 6, 6);

    painter.setPen(QPen(QColor("#729B8C"), 2.5));
    painter.drawLine(QPointF(legRect.left() + 10, legRect.top() + 18), QPointF(legRect.left() + 28, legRect.top() + 18));
    painter.setPen(QColor("#EDEDF4"));
    QFont f("Montserrat");
    f.setPointSize(8);
    painter.setFont(f);
    painter.drawText(QPointF(legRect.left() + 34, legRect.top() + 22), "Потенциал φ(x)");

    painter.setPen(QPen(QColor("#F4D03F"), 2.0, Qt::DashLine));
    painter.drawLine(QPointF(legRect.left() + 10, legRect.top() + 40), QPointF(legRect.left() + 28, legRect.top() + 40));
    painter.setPen(QColor("#EDEDF4"));
    painter.drawText(QPointF(legRect.left() + 34, legRect.top() + 44), "Напряженность поля E(x)");
}
