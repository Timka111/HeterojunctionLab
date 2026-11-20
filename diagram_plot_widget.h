#ifndef DIAGRAM_PLOT_WIDGET_H
#define DIAGRAM_PLOT_WIDGET_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include <QString>
#include <QColor>

class DiagramPlotWidget : public QWidget {
    Q_OBJECT
public:
    explicit DiagramPlotWidget(QWidget *parent = nullptr);

    void setTitle(const QString& title);
    void setMaterialNames(const QString& leftMat, const QString& rightMat);
    void setBoundaryPositions(double xd1, double xd2);
    void setData(const QVector<QPointF>& ec,
                 const QVector<QPointF>& ev,
                 const QVector<QPointF>& e0,
                 const QVector<QPointF>& ef);
    void clear();
    void saveImage(const QString& filePath);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QString m_title;
    QString m_leftMaterial;
    QString m_rightMaterial;
    double m_xd1 = 0.0;
    double m_xd2 = 0.0;

    QVector<QPointF> m_ec;
    QVector<QPointF> m_ev;
    QVector<QPointF> m_e0;
    QVector<QPointF> m_ef;

    bool m_hasHover = false;
    QPoint m_mousePos;

    QPointF toWidgetCoords(const QPointF& dataPoint, const QRectF& plotRect, double xMin, double xMax, double yMin, double yMax) const;
    QPointF toDataCoords(const QPoint& widgetPos, const QRectF& plotRect, double xMin, double xMax, double yMin, double yMax) const;
};

#endif // DIAGRAM_PLOT_WIDGET_H
