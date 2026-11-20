#ifndef BAND_BLOCKS_WIDGET_H
#define BAND_BLOCKS_WIDGET_H

#include <QWidget>
#include <QString>
#include <QPixmap>

class BandBlocksWidget : public QWidget {
    Q_OBJECT
public:
    explicit BandBlocksWidget(QWidget *parent = nullptr);

    void setMaterials(const QString& name1, double Eg1, double chi1, bool isMetal1,
                      const QString& name2, double Eg2, double chi2, bool isMetal2);
    void saveImage(const QString& filePath);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_name1 = "Материал 1";
    double m_Eg1 = 1.35;
    double m_chi1 = 5.34;
    bool m_isMetal1 = false;

    QString m_name2 = "Материал 2";
    double m_Eg2 = 2.17;
    double m_chi2 = 3.20;
    bool m_isMetal2 = false;
};

#endif // BAND_BLOCKS_WIDGET_H
