#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Моделирование энергетических диаграмм гетеропереходов (Модель Андерсона)");
    resize(1200, 800);

    setupUI();
    populatePresets();
    onResetToLab1Clicked(); 
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    QWidget *leftPanel = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    QWidget *scrollContent = new QWidget(scrollArea);
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setSpacing(12);

    auto createDoubleSpin = [](double min, double max, double val, int decimals, double step) {
        QDoubleSpinBox *sb = new QDoubleSpinBox();
        sb->setRange(min, max);
        sb->setValue(val);
        sb->setDecimals(decimals);
        sb->setSingleStep(step);
        return sb;
    };

    QGroupBox *groupMat1 = new QGroupBox("Материал 1 (слева, x < 0)", scrollContent);
    QGridLayout *gridMat1 = new QGridLayout(groupMat1);

    m_comboPreset1 = new QComboBox();
    m_editName1 = new QLineEdit("InP");
    m_spinEg1 = createDoubleSpin(0.1, 10.0, 1.35, 3, 0.05);
    m_spinChi1 = createDoubleSpin(0.1, 10.0, 5.34, 3, 0.05);
    m_editNa1 = new QLineEdit("5.2e19");
    m_spinMh1 = createDoubleSpin(0.01, 10.0, 0.64, 3, 0.01);
    m_spinMe1 = createDoubleSpin(0.01, 10.0, 0.078, 3, 0.01);
    m_spinEps1 = createDoubleSpin(1.0, 50.0, 12.5, 2, 0.5);

    int r = 0;
    gridMat1->addWidget(new QLabel("Быстрый выбор:"), r, 0);
    gridMat1->addWidget(m_comboPreset1, r++, 1);
    gridMat1->addWidget(new QLabel("Название:"), r, 0);
    gridMat1->addWidget(m_editName1, r++, 1);
    gridMat1->addWidget(new QLabel("Eg (ширина зоны), эВ:"), r, 0);
    gridMat1->addWidget(m_spinEg1, r++, 1);
    gridMat1->addWidget(new QLabel("χ (сродство κ), эВ:"), r, 0);
    gridMat1->addWidget(m_spinChi1, r++, 1);
    gridMat1->addWidget(new QLabel("Na (концентрация), см⁻³:"), r, 0);
    gridMat1->addWidget(m_editNa1, r++, 1);
    gridMat1->addWidget(new QLabel("mh (масса дырок, ·m_e):"), r, 0);
    gridMat1->addWidget(m_spinMh1, r++, 1);
    gridMat1->addWidget(new QLabel("me (масса электр., ·m_e):"), r, 0);
    gridMat1->addWidget(m_spinMe1, r++, 1);
    gridMat1->addWidget(new QLabel("ε (диэл. проницаемость):"), r, 0);
    gridMat1->addWidget(m_spinEps1, r++, 1);

    scrollLayout->addWidget(groupMat1);

    QGroupBox *groupMat2 = new QGroupBox("Материал 2 (справа, x > 0)", scrollContent);
    QGridLayout *gridMat2 = new QGridLayout(groupMat2);

    m_comboPreset2 = new QComboBox();
    m_editName2 = new QLineEdit("Cu2O");
    m_spinEg2 = createDoubleSpin(0.1, 10.0, 2.17, 3, 0.05);
    m_spinChi2 = createDoubleSpin(0.1, 10.0, 3.20, 3, 0.05);
    m_editNa2 = new QLineEdit("3.0e18");
    m_spinMh2 = createDoubleSpin(0.01, 10.0, 0.58, 3, 0.01);
    m_spinMe2 = createDoubleSpin(0.01, 10.0, 0.99, 3, 0.01);
    m_spinEps2 = createDoubleSpin(1.0, 50.0, 7.5, 2, 0.5);

    r = 0;
    gridMat2->addWidget(new QLabel("Быстрый выбор:"), r, 0);
    gridMat2->addWidget(m_comboPreset2, r++, 1);
    gridMat2->addWidget(new QLabel("Название:"), r, 0);
    gridMat2->addWidget(m_editName2, r++, 1);
    gridMat2->addWidget(new QLabel("Eg (ширина зоны), эВ:"), r, 0);
    gridMat2->addWidget(m_spinEg2, r++, 1);
    gridMat2->addWidget(new QLabel("χ (сродство κ), эВ:"), r, 0);
    gridMat2->addWidget(m_spinChi2, r++, 1);
    gridMat2->addWidget(new QLabel("Na (концентрация), см⁻³:"), r, 0);
    gridMat2->addWidget(m_editNa2, r++, 1);
    gridMat2->addWidget(new QLabel("mh (масса дырок, ·m_e):"), r, 0);
    gridMat2->addWidget(m_spinMh2, r++, 1);
    gridMat2->addWidget(new QLabel("me (масса электр., ·m_e):"), r, 0);
    gridMat2->addWidget(m_spinMe2, r++, 1);
    gridMat2->addWidget(new QLabel("ε (диэл. проницаемость):"), r, 0);
    gridMat2->addWidget(m_spinEps2, r++, 1);

    scrollLayout->addWidget(groupMat2);

    QGroupBox *groupEnv = new QGroupBox("Условия эксперимента", scrollContent);
    QGridLayout *gridEnv = new QGridLayout(groupEnv);
    m_spinTemp = createDoubleSpin(10.0, 1000.0, 300.0, 1, 10.0);
    gridEnv->addWidget(new QLabel("Температура T, К:"), 0, 0);
    gridEnv->addWidget(m_spinTemp, 0, 1);
    scrollLayout->addWidget(groupEnv);

    m_btnCalculate = new QPushButton("▶ Рассчитать и построить");
    m_btnCalculate->setStyleSheet("background-color: #2980B9; color: white; font-weight: bold; padding: 8px; font-size: 13px; border-radius: 4px;");

    m_btnReset = new QPushButton("↺ Вариант Лаб 1 (InP / Cu2O)");
    m_btnReset->setStyleSheet("padding: 5px;");

    m_btnExport = new QPushButton("💾 Экспорт графика в PNG");
    m_btnExport->setStyleSheet("padding: 5px;");

    scrollLayout->addWidget(m_btnCalculate);
    scrollLayout->addWidget(m_btnReset);
    scrollLayout->addWidget(m_btnExport);
    scrollLayout->addStretch();

    scrollArea->setWidget(scrollContent);
    leftLayout->addWidget(scrollArea);
    leftPanel->setFixedWidth(340);

    QSplitter *rightSplitter = new QSplitter(Qt::Vertical, this);

    m_tabPlots = new QTabWidget(this);
    m_plotContact = new DiagramPlotWidget(this);
    m_plotContact->setTitle("Энергетическая зонная диаграмма гетероперехода в контакте");

    m_plotIsolated = new DiagramPlotWidget(this);
    m_plotIsolated->setTitle("Энергетическая диаграмма изолированных полупроводников до контакта");

    m_tabPlots->addTab(m_plotContact, "1. В контакте (термодинамическое равновесие)");
    m_tabPlots->addTab(m_plotIsolated, "2. До контакта (относительно уровня вакуума E₀)");

    rightSplitter->addWidget(m_tabPlots);

    QGroupBox *groupResults = new QGroupBox("Результаты физических расчетов гетероструктуры", this);
    QVBoxLayout *resultsLayout = new QVBoxLayout(groupResults);
    resultsLayout->setContentsMargins(6, 6, 6, 6);

    m_tableResults = new QTableWidget(this);
    m_tableResults->setColumnCount(4);
    m_tableResults->setHorizontalHeaderLabels({"Параметр", "Обозначение", "Значение", "Ед. изм."});
    m_tableResults->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tableResults->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tableResults->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_tableResults->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_tableResults->verticalHeader()->setVisible(false);
    m_tableResults->setAlternatingRowColors(true);
    m_tableResults->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableResults->setFixedHeight(220);

    resultsLayout->addWidget(m_tableResults);
    rightSplitter->addWidget(groupResults);

    rightSplitter->setStretchFactor(0, 3);
    rightSplitter->setStretchFactor(1, 1);

    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(rightSplitter);

    connect(m_comboPreset1, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onPreset1Changed);
    connect(m_comboPreset2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onPreset2Changed);
    connect(m_btnCalculate, &QPushButton::clicked, this, &MainWindow::onCalculateClicked);
    connect(m_btnReset, &QPushButton::clicked, this, &MainWindow::onResetToLab1Clicked);
    connect(m_btnExport, &QPushButton::clicked, this, &MainWindow::onExportPlotClicked);
}

void MainWindow::populatePresets() {
    m_presets = HeterojunctionModel::getPresetMaterials();
    m_comboPreset1->addItem("-- Пользовательский --");
    m_comboPreset2->addItem("-- Пользовательский --");

    for (const auto& mat : m_presets) {
        m_comboPreset1->addItem(mat.name);
        m_comboPreset2->addItem(mat.name);
    }
}

void MainWindow::applyMaterialToInputs(int materialNum, const MaterialParams& mat) {
    if (materialNum == 1) {
        m_editName1->setText(mat.name.split(" ").first());
        m_spinEg1->setValue(mat.Eg);
        m_spinChi1->setValue(mat.chi);
        m_editNa1->setText(QString::number(mat.Na, 'e', 2));
        m_spinMh1->setValue(mat.mh);
        m_spinMe1->setValue(mat.me);
        m_spinEps1->setValue(mat.eps);
    } else {
        m_editName2->setText(mat.name.split(" ").first());
        m_spinEg2->setValue(mat.Eg);
        m_spinChi2->setValue(mat.chi);
        m_editNa2->setText(QString::number(mat.Na, 'e', 2));
        m_spinMh2->setValue(mat.mh);
        m_spinMe2->setValue(mat.me);
        m_spinEps2->setValue(mat.eps);
    }
}

MaterialParams MainWindow::readMaterialInputs(int materialNum) {
    MaterialParams mat;
    if (materialNum == 1) {
        mat.name = m_editName1->text().trimmed();
        mat.Eg = m_spinEg1->value();
        mat.chi = m_spinChi1->value();
        mat.Na = m_editNa1->text().trimmed().toDouble();
        mat.mh = m_spinMh1->value();
        mat.me = m_spinMe1->value();
        mat.eps = m_spinEps1->value();
    } else {
        mat.name = m_editName2->text().trimmed();
        mat.Eg = m_spinEg2->value();
        mat.chi = m_spinChi2->value();
        mat.Na = m_editNa2->text().trimmed().toDouble();
        mat.mh = m_spinMh2->value();
        mat.me = m_spinMe2->value();
        mat.eps = m_spinEps2->value();
    }
    return mat;
}

void MainWindow::onPreset1Changed(int index) {
    if (index > 0 && index - 1 < m_presets.size()) {
        applyMaterialToInputs(1, m_presets[index - 1]);
    }
}

void MainWindow::onPreset2Changed(int index) {
    if (index > 0 && index - 1 < m_presets.size()) {
        applyMaterialToInputs(2, m_presets[index - 1]);
    }
}

void MainWindow::onResetToLab1Clicked() {

    m_comboPreset1->setCurrentIndex(1); 
    m_comboPreset2->setCurrentIndex(2); 
    m_spinTemp->setValue(300.0);
    onCalculateClicked();
}

void MainWindow::onCalculateClicked() {
    MaterialParams m1 = readMaterialInputs(1);
    MaterialParams m2 = readMaterialInputs(2);
    double T = m_spinTemp->value();

    if (m1.Na <= 0.0 || m2.Na <= 0.0) {
        QMessageBox::warning(this, "Ошибка ввода", "Концентрация примесей Na должна быть больше нуля.");
        return;
    }

    CalculationResults res = HeterojunctionModel::calculate(m1, m2, T);

    m_plotContact->setMaterialNames(m1.name + " (слева)", m2.name + " (справа)");
    m_plotContact->setBoundaryPositions(res.xd1, res.xd2);
    m_plotContact->setData(res.contact_Ec, res.contact_Ev, res.contact_E0, res.contact_Ef);

    m_plotIsolated->setMaterialNames(m1.name + " (слева)", m2.name + " (справа)");
    m_plotIsolated->setBoundaryPositions(0, 0);
    m_plotIsolated->setData(res.isolated_Ec, res.isolated_Ev, res.isolated_E0, res.isolated_Ef);

    updateResultsTable(res);
}

void MainWindow::updateResultsTable(const CalculationResults& res) {
    struct RowData {
        QString desc;
        QString sym;
        QString val;
        QString unit;
    };

    QVector<RowData> rows = {
        {"Разрыв зоны проводимости", "ΔEc", QString::number(res.Delta_Ec, 'f', 4), "эВ"},
        {"Разность ширин запрещенных зон", "ΔEg", QString::number(res.Delta_Eg, 'f', 4), "эВ"},
        {"Разрыв валентной зоны", "ΔEv", QString::number(res.Delta_Ev, 'f', 4), "эВ"},
        {"Плотность состояний валентной зоны 1", "N_V1", QString::number(res.NV1, 'e', 5), "см⁻³"},
        {"Плотность состояний валентной зоны 2", "N_V2", QString::number(res.NV2, 'e', 5), "см⁻³"},
        {"Плотность состояний зоны проводимости 1", "N_c1", QString::number(res.Nc1, 'e', 5), "см⁻³"},
        {"Плотность состояний зоны проводимости 2", "N_c2", QString::number(res.Nc2, 'e', 5), "см⁻³"},
        {"Положение уровня Ферми 1", "q·φ₁", QString::number(res.qphi1, 'f', 6), "эВ"},
        {"Положение уровня Ферми 2", "q·φ₂", QString::number(res.qphi2, 'f', 6), "эВ"},
        {"Диффузионный (контактный) потенциал", "φ₀", QString::number(res.phi0, 'f', 6), "эВ (В)"},
        {"Полная ширина ОПЗ", "Xd", QString::number(res.Xd, 'f', 4), "нм"},
        {"Ширина ОПЗ в материале 1", "xd1", QString::number(res.xd1, 'f', 4), "нм"},
        {"Ширина ОПЗ в материале 2", "xd2", QString::number(res.xd2, 'f', 4), "нм"},
        {"Падение потенциала в материале 1", "q·φ₀₁", QString::number(res.qphi01, 'f', 6), "эВ"},
        {"Падение потенциала в материале 2", "q·φ₀₂", QString::number(res.qphi02, 'f', 6), "эВ"},
        {"Сумма падений (баланс qφ₀₁ + qφ₀₂)", "Σ(qφ₀)", QString::number(res.sum_qphi0, 'f', 6), "эВ (≡ φ₀)"}
    };

    m_tableResults->setRowCount(rows.size());
    for (int i = 0; i < rows.size(); ++i) {
        m_tableResults->setItem(i, 0, new QTableWidgetItem(rows[i].desc));
        m_tableResults->setItem(i, 1, new QTableWidgetItem(rows[i].sym));
        m_tableResults->setItem(i, 2, new QTableWidgetItem(rows[i].val));
        m_tableResults->setItem(i, 3, new QTableWidgetItem(rows[i].unit));
    }
}

void MainWindow::onExportPlotClicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить график как изображение", "heterojunction_diagram.png", "Изображения (*.png *.jpg *.bmp)");
    if (!fileName.isEmpty()) {
        int currentTab = m_tabPlots->currentIndex();
        if (currentTab == 0) {
            m_plotContact->saveImage(fileName);
        } else {
            m_plotIsolated->saveImage(fileName);
        }
        QMessageBox::information(this, "Успех", "График успешно сохранен:\n" + fileName);
    }
}
