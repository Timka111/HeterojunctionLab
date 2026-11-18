#include "heterojunction_model.h"

CalculationResults HeterojunctionModel::calculate(const MaterialParams& m1, const MaterialParams& m2, double T, int pointCount) {
    CalculationResults res;

    double phi_T = (k_B * T) / q;

    auto calc_density = [T](double mass_fraction) {
        double m = mass_fraction * m_e;
        return (2.0 * std::pow(2.0 * Pi * m * k_B * T, 1.5)) / std::pow(h, 3);
    };

    double NV1_si = calc_density(m1.mh);
    double Nc1_si = calc_density(m1.me);
    double NV2_si = calc_density(m2.mh);
    double Nc2_si = calc_density(m2.me);

    res.NV1 = NV1_si * 1e-6;
    res.Nc1 = Nc1_si * 1e-6;
    res.NV2 = NV2_si * 1e-6;
    res.Nc2 = Nc2_si * 1e-6;

    res.qphi1 = phi_T * std::log(NV1_si / (m1.Na * 1e6));
    res.qphi2 = phi_T * std::log(NV2_si / (m2.Na * 1e6));

    res.Delta_Ec = m1.chi - m2.chi;
    res.Delta_Eg = std::abs(m1.Eg - m2.Eg);
    res.Delta_Ev = std::abs(res.Delta_Eg - res.Delta_Ec);

    res.phi0 = m1.Eg - m2.Eg + res.Delta_Ec - res.qphi1 + res.qphi2;

    double Nad1_m3 = m1.Na * 1e6;
    double Nad2_m3 = m2.Na * 1e6;

    double numerator = 2.0 * eps_0 * res.phi0 * std::pow(Nad1_m3 + Nad2_m3, 2);
    double denominator = q * Nad1_m3 * Nad2_m3 * ((Nad1_m3 / m2.eps) + (Nad2_m3 / m1.eps));

    double Xd_m = 0.0;
    if (denominator > 0.0 && numerator > 0.0) {
        Xd_m = std::sqrt(numerator / denominator);
    }
    res.Xd = Xd_m * 1e9; 

    double sumNa = m1.Na + m2.Na;
    if (sumNa > 0.0) {
        res.xd1 = res.Xd * (m2.Na / sumNa);
        res.xd2 = res.Xd * (m1.Na / sumNa);
    } else {
        res.xd1 = res.Xd * 0.5;
        res.xd2 = res.Xd * 0.5;
    }

    res.qphi01 = (q * Nad1_m3 * std::pow(res.xd1, 2) * 1e-18) / (2.0 * m1.eps * eps_0);
    res.qphi02 = (q * Nad2_m3 * std::pow(res.xd2, 2) * 1e-18) / (2.0 * m2.eps * eps_0);
    res.sum_qphi0 = res.qphi01 + res.qphi02;

    res.xmin = -1.5 * res.Xd;
    res.xmax = 1.5 * res.Xd;

    double Ef_contact = res.qphi2; 
    double Ec1_contact = Ef_contact + m1.Eg - res.qphi1;
    double Ev1_contact = Ef_contact - res.qphi1;
    double E01_contact = Ec1_contact + m1.chi;

    double Ec2_contact = Ef_contact + m2.Eg - res.qphi2;
    double Ev2_contact = Ef_contact - res.qphi2;
    double E02_contact = Ec2_contact + m2.chi;

    int steps = std::max(200, pointCount);
    double dx = (res.xmax - res.xmin) / steps;

    for (int i = 0; i <= steps; ++i) {
        double x = res.xmin + i * dx;

        double Ec = 0.0, Ev = 0.0, E0 = 0.0;

        if (x <= -res.xd1) {

            Ec = Ec1_contact;
            Ev = Ev1_contact;
            E0 = E01_contact;
        } else if (x <= 0.0) {

            double bend = (q * Nad1_m3 * std::pow(x + res.xd1, 2) * 1e-18) / (2.0 * m1.eps * eps_0);
            Ec = Ec1_contact - bend;
            Ev = Ev1_contact - bend;
            E0 = E01_contact - bend;
        } else if (x <= res.xd2) {

            double bend = (q * Nad2_m3 * std::pow(res.xd2 - x, 2) * 1e-18) / (2.0 * m2.eps * eps_0);
            Ec = Ec2_contact + bend;
            Ev = Ev2_contact + bend;
            E0 = E02_contact + bend;
        } else {

            Ec = Ec2_contact;
            Ev = Ev2_contact;
            E0 = E02_contact;
        }

        res.contact_Ec.append(QPointF(x, Ec));
        res.contact_Ev.append(QPointF(x, Ev));
        res.contact_E0.append(QPointF(x, E0));
        res.contact_Ef.append(QPointF(x, Ef_contact));
    }

    double E0_iso = 0.0; 
    double Ec1_iso = E0_iso - m1.chi;
    double Ev1_iso = Ec1_iso - m1.Eg;
    double Ef1_iso = Ev1_iso + res.qphi1;

    double Ec2_iso = E0_iso - m2.chi;
    double Ev2_iso = Ec2_iso - m2.Eg;
    double Ef2_iso = Ev2_iso + res.qphi2;

    for (int i = 0; i <= steps; ++i) {
        double x = res.xmin + i * dx;
        if (x < 0.0) {
            res.isolated_Ec.append(QPointF(x, Ec1_iso));
            res.isolated_Ev.append(QPointF(x, Ev1_iso));
            res.isolated_E0.append(QPointF(x, E0_iso));
            res.isolated_Ef.append(QPointF(x, Ef1_iso));
        } else {
            res.isolated_Ec.append(QPointF(x, Ec2_iso));
            res.isolated_Ev.append(QPointF(x, Ev2_iso));
            res.isolated_E0.append(QPointF(x, E0_iso));
            res.isolated_Ef.append(QPointF(x, Ef2_iso));
        }
    }

    return res;
}

QVector<MaterialParams> HeterojunctionModel::getPresetMaterials() {
    QVector<MaterialParams> presets;

    presets.append({"InP (фосфид индия)", 1.35, 5.34, 5.2e19, 0.64, 0.078, 12.5});

    presets.append({"Cu2O (закись меди)", 2.17, 3.20, 3.0e18, 0.58, 0.99, 7.5});

    presets.append({"GaAs (арсенид галлия)", 1.42, 4.07, 1.0e18, 0.51, 0.067, 12.9});
    presets.append({"Si (кремний)", 1.12, 4.05, 1.0e17, 0.49, 0.26, 11.7});
    presets.append({"Ge (германий)", 0.66, 4.00, 1.0e17, 0.28, 0.12, 16.0});
    presets.append({"GaN (нитрид галлия)", 3.40, 4.10, 5.0e17, 0.80, 0.20, 8.9});
    presets.append({"CdTe (теллурид кадмия)", 1.50, 4.28, 1.0e17, 0.35, 0.096, 10.2});
    presets.append({"ZnS (сульфид цинка)", 3.68, 3.90, 1.0e17, 0.60, 0.28, 8.3});
    presets.append({"InAs (арсенид индия)", 0.35, 4.90, 1.0e17, 0.41, 0.023, 15.1});
    presets.append({"AlAs (арсенид алюминия)", 2.16, 3.50, 1.0e17, 0.76, 0.15, 10.0});
    return presets;
}
