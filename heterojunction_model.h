#ifndef HETEROJUNCTION_MODEL_H
#define HETEROJUNCTION_MODEL_H

#include <QString>
#include <QVector>
#include <QPointF>
#include <cmath>

struct MaterialParams {
    QString name;
    double Eg;
    double chi;
    double Na;
    double mh;
    double me;
    double eps;
    bool isMetal = false;
};

struct CalculationResults {
    double Delta_Ec;
    double Delta_Eg;
    double Delta_Ev;

    double NV1;
    double Nc1;
    double NV2;
    double Nc2;

    double qphi1;
    double qphi2;

    double phi0;

    double Xd;
    double xd1;
    double xd2;

    double qphi01;
    double qphi02;
    double sum_qphi0;

    double xmin;
    double xmax;

    QVector<QPointF> contact_Ec;
    QVector<QPointF> contact_Ev;
    QVector<QPointF> contact_E0;
    QVector<QPointF> contact_Ef;

    QVector<QPointF> isolated_Ec;
    QVector<QPointF> isolated_Ev;
    QVector<QPointF> isolated_E0;
    QVector<QPointF> isolated_Ef;
};

class HeterojunctionModel {
public:
    static constexpr double k_B = 1.38e-23;
    static constexpr double k_ev = 8.6e-5;
    static constexpr double m_e = 9.1e-31;
    static constexpr double q = 1.6e-19;
    static constexpr double h = 6.63e-34;
    static constexpr double eps_0 = 8.85e-12;
    static constexpr double Pi = 3.14159265358979323846;

    static CalculationResults calculate(const MaterialParams& m1, const MaterialParams& m2, double T = 300.0, int pointCount = 300);

    static QVector<MaterialParams> getPresetMaterials();
};

#endif // HETEROJUNCTION_MODEL_H
