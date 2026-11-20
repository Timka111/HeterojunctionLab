#ifndef POTENTIAL_PLOT_WIDGET_H
#define POTENTIAL_PLOT_WIDGET_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include <QString>

class PotentialPlotWidget : public QWidget {
    Q_OBJECT
public:
    explicit PotentialPlotWidget(QWidget *parent = nullptr);

    void setParams(double xd1, double xd2, double qphi01, double qphi02, double phi0);
    void saveImage(const QString& filePath);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double m_xd1 = 1.11;
    double m_xd2 = 19.28;
    double m_qphi01 = 0.046;
    double m_qphi02 = 1.343;
    double m_phi0 = 1.39;

    QVector<QPointF> m_potentialPoints;
    QVector<QPointF> m_fieldPoints;

    void recalculate();
};

#endif // POTENTIAL_PLOT_WIDGET_H
