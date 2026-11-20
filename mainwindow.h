#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLabel>
#include <QFrame>
#include "heterojunction_model.h"
#include "diagram_plot_widget.h"
#include "band_blocks_widget.h"
#include "potential_plot_widget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onPreset1Changed(int index);
    void onPreset2Changed(int index);
    void onType1Changed(int index);
    void onType2Changed(int index);
    void onCalculateClicked();
    void onResetToLab1Clicked();
    void onExportPlotClicked();

private:
    void setupUI();
    void populatePresets();
    void applyMaterialToInputs(int materialNum, const MaterialParams& mat);
    MaterialParams readMaterialInputs(int materialNum);
    void updateResultsUI(const CalculationResults& res);
    QFrame* createKpiCard(const QString& title, QLabel*& valLabel, const QString& unit, const QString& accentColor);

    QVector<MaterialParams> m_presets;

    QComboBox *m_comboType1;
    QComboBox *m_comboPreset1;
    QLineEdit *m_editName1;
    QDoubleSpinBox *m_spinEg1;
    QDoubleSpinBox *m_spinChi1;
    QLineEdit *m_editNa1;
    QDoubleSpinBox *m_spinMh1;
    QDoubleSpinBox *m_spinMe1;
    QDoubleSpinBox *m_spinEps1;

    QComboBox *m_comboType2;
    QComboBox *m_comboPreset2;
    QLineEdit *m_editName2;
    QDoubleSpinBox *m_spinEg2;
    QDoubleSpinBox *m_spinChi2;
    QLineEdit *m_editNa2;
    QDoubleSpinBox *m_spinMh2;
    QDoubleSpinBox *m_spinMe2;
    QDoubleSpinBox *m_spinEps2;

    QDoubleSpinBox *m_spinTemp;

    QPushButton *m_btnCalculate;
    QPushButton *m_btnReset;
    QPushButton *m_btnExport;

    QLabel *m_kpiEcVal;
    QLabel *m_kpiEvVal;
    QLabel *m_kpiPhi0Val;
    QLabel *m_kpiXdVal;

    QTabWidget *m_tabPlots;
    DiagramPlotWidget *m_plotContact;
    DiagramPlotWidget *m_plotIsolated;
    BandBlocksWidget *m_plotBlocks;
    PotentialPlotWidget *m_plotPotential;
    QTableWidget *m_tableResults;
};

#endif // MAINWINDOW_H
