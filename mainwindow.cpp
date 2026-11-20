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
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("HeterojunctionLab — Аналитическое моделирование зонных диаграмм гетероструктур");

    QIcon appIcon(":/app_icon.png");
    if (appIcon.isNull()) appIcon = QIcon("app_icon.png");
    if (appIcon.isNull()) appIcon = QIcon("app_icon.ico");
    setWindowIcon(appIcon);

    setupUI();
    populatePresets();
    onResetToLab1Clicked();

    showMaximized();
}

QFrame* MainWindow::createKpiCard(const QString& title, QLabel*& valLabel, const QString& unit, const QString& accentColor) {
    QFrame *card = new QFrame(this);
    card->setStyleSheet(QString(
        "QFrame {"
        "  background-color: #262732;"
        "  border: 1px solid #3B3C48;"
        "  border-top: 3px solid %1;"
        "  border-radius: 6px;"
        "}"
    ).arg(accentColor));

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(2);

    QLabel *lblTitle = new QLabel(title, card);
    lblTitle->setStyleSheet("color: #9A99A8; font-size: 11px; font-weight: bold;");

    QHBoxLayout *valLayout = new QHBoxLayout();
    valLayout->setSpacing(6);

    valLabel = new QLabel("0.00", card);
    valLabel->setStyleSheet(QString("color: %1; font-size: 20px; font-weight: bold;").arg(accentColor));

    QLabel *lblUnit = new QLabel(unit, card);
    lblUnit->setStyleSheet("color: #727180; font-size: 11px; font-weight: bold; padding-top: 4px;");

    valLayout->addWidget(valLabel);
    valLayout->addWidget(lblUnit);
    valLayout->addStretch();

    layout->addWidget(lblTitle);
    layout->addLayout(valLayout);

    return card;
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    QWidget *leftPanel = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(8);

    QFrame *actionsCard = new QFrame(leftPanel);
    actionsCard->setStyleSheet(
        "QFrame {"
        "  background-color: #262732;"
        "  border: 1px solid #3B3C48;"
        "  border-radius: 6px;"
        "  padding: 8px;"
        "}"
    );
    QVBoxLayout *actionsLayout = new QVBoxLayout(actionsCard);
    actionsLayout->setContentsMargins(6, 6, 6, 6);
    actionsLayout->setSpacing(6);

    m_btnCalculate = new QPushButton("Рассчитать и построить диаграммы");
    m_btnCalculate->setCursor(Qt::PointingHandCursor);
    m_btnCalculate->setStyleSheet(
        "QPushButton {"
        "  background: #689F8F;"
        "  color: #16171D;"
        "  border: none;"
        "  font-weight: bold;"
        "  font-size: 13px;"
        "  padding: 10px;"
        "  border-radius: 4px;"
        "}"
        "QPushButton:hover { background: #7BB5A4; }"
        "QPushButton:pressed { background: #558778; }"
    );

    QHBoxLayout *subActions = new QHBoxLayout();
    subActions->setSpacing(6);

    m_btnReset = new QPushButton("Сброс (InP / Cu2O)");
    m_btnReset->setCursor(Qt::PointingHandCursor);
    m_btnReset->setStyleSheet(
        "QPushButton {"
        "  background-color: #31323D;"
        "  color: #E8E8EE;"
        "  border: 1px solid #7E749B;"
        "  border-radius: 4px;"
        "  padding: 6px 10px;"
        "  font-size: 11px;"
        "}"
        "QPushButton:hover { background-color: #3F3E50; }"
    );

    m_btnExport = new QPushButton("Экспорт в PNG");
    m_btnExport->setCursor(Qt::PointingHandCursor);
    m_btnExport->setStyleSheet(
        "QPushButton {"
        "  background-color: #31323D;"
        "  color: #E8E8EE;"
        "  border: 1px solid #689F8F;"
        "  border-radius: 4px;"
        "  padding: 6px 10px;"
        "  font-size: 11px;"
        "}"
        "QPushButton:hover { background-color: #35423E; }"
    );

    subActions->addWidget(m_btnReset);
    subActions->addWidget(m_btnExport);

    actionsLayout->addWidget(m_btnCalculate);
    actionsLayout->addLayout(subActions);

    leftLayout->addWidget(actionsCard);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    QWidget *scrollContent = new QWidget(scrollArea);
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(0, 0, 8, 0);
    scrollLayout->setSpacing(10);

    auto createDoubleSpin = [](double min, double max, double val, int decimals, double step) {
        QDoubleSpinBox *sb = new QDoubleSpinBox();
        sb->setRange(min, max);
        sb->setValue(val);
        sb->setDecimals(decimals);
        sb->setSingleStep(step);
        return sb;
    };

    QGroupBox *groupMat1 = new QGroupBox("Материал 1 (слева, x < 0)", scrollContent);
    groupMat1->setStyleSheet(
        "QGroupBox {"
        "  background-color: #262732;"
        "  border: 1px solid #3B3C48;"
        "  border-top: 3px solid #689F8F;"
        "  border-radius: 6px;"
        "  margin-top: 10px;"
        "  padding-top: 10px;"
        "}"
        "QGroupBox::title { color: #689F8F; font-weight: bold; }"
    );
    QGridLayout *gridMat1 = new QGridLayout(groupMat1);
    gridMat1->setHorizontalSpacing(8);
    gridMat1->setVerticalSpacing(5);

    m_comboType1 = new QComboBox();
    m_comboType1->addItem("Полупроводник");
    m_comboType1->addItem("Металл / Проводник");

    m_comboPreset1 = new QComboBox();
    m_editName1 = new QLineEdit("InP");
    m_spinEg1 = createDoubleSpin(0.0, 10.0, 1.35, 3, 0.05);
    m_spinChi1 = createDoubleSpin(0.1, 10.0, 5.34, 3, 0.05);
    m_editNa1 = new QLineEdit("5.2e19");
    m_spinMh1 = createDoubleSpin(0.01, 10.0, 0.64, 3, 0.01);
    m_spinMe1 = createDoubleSpin(0.01, 10.0, 0.078, 3, 0.01);
    m_spinEps1 = createDoubleSpin(1.0, 50.0, 12.5, 2, 0.5);

    int r = 0;
    gridMat1->addWidget(new QLabel("Тип:"), r, 0);
    gridMat1->addWidget(m_comboType1, r++, 1, 1, 3);
    gridMat1->addWidget(new QLabel("Пресет:"), r, 0);
    gridMat1->addWidget(m_comboPreset1, r++, 1, 1, 3);
    gridMat1->addWidget(new QLabel("Имя:"), r, 0);
    gridMat1->addWidget(m_editName1, r++, 1, 1, 3);
    gridMat1->addWidget(new QLabel("Eg (эВ):"), r, 0);
    gridMat1->addWidget(m_spinEg1, r, 1);
    gridMat1->addWidget(new QLabel("χ (эВ):"), r, 2);
    gridMat1->addWidget(m_spinChi1, r++, 3);
    gridMat1->addWidget(new QLabel("Концентр. (см⁻³):"), r, 0);
    gridMat1->addWidget(m_editNa1, r++, 1, 1, 3);
    gridMat1->addWidget(new QLabel("mh (m₀):"), r, 0);
    gridMat1->addWidget(m_spinMh1, r, 1);
    gridMat1->addWidget(new QLabel("me (m₀):"), r, 2);
    gridMat1->addWidget(m_spinMe1, r++, 3);
    gridMat1->addWidget(new QLabel("Диэлектр. ε:"), r, 0);
    gridMat1->addWidget(m_spinEps1, r++, 1, 1, 3);

    scrollLayout->addWidget(groupMat1);

    QGroupBox *groupMat2 = new QGroupBox("Материал 2 (справа, x > 0)", scrollContent);
    groupMat2->setStyleSheet(
        "QGroupBox {"
        "  background-color: #262732;"
        "  border: 1px solid #3B3C48;"
        "  border-top: 3px solid #7E749B;"
        "  border-radius: 6px;"
        "  margin-top: 10px;"
        "  padding-top: 10px;"
        "}"
        "QGroupBox::title { color: #7E749B; font-weight: bold; }"
    );
    QGridLayout *gridMat2 = new QGridLayout(groupMat2);
    gridMat2->setHorizontalSpacing(8);
    gridMat2->setVerticalSpacing(5);

    m_comboType2 = new QComboBox();
    m_comboType2->addItem("Полупроводник");
    m_comboType2->addItem("Металл / Проводник");

    m_comboPreset2 = new QComboBox();
    m_editName2 = new QLineEdit("Cu2O");
    m_spinEg2 = createDoubleSpin(0.0, 10.0, 2.17, 3, 0.05);
    m_spinChi2 = createDoubleSpin(0.1, 10.0, 3.20, 3, 0.05);
    m_editNa2 = new QLineEdit("3.0e18");
    m_spinMh2 = createDoubleSpin(0.01, 10.0, 0.58, 3, 0.01);
    m_spinMe2 = createDoubleSpin(0.01, 10.0, 0.99, 3, 0.01);
    m_spinEps2 = createDoubleSpin(1.0, 50.0, 7.5, 2, 0.5);

    r = 0;
    gridMat2->addWidget(new QLabel("Тип:"), r, 0);
    gridMat2->addWidget(m_comboType2, r++, 1, 1, 3);
    gridMat2->addWidget(new QLabel("Пресет:"), r, 0);
    gridMat2->addWidget(m_comboPreset2, r++, 1, 1, 3);
    gridMat2->addWidget(new QLabel("Имя:"), r, 0);
    gridMat2->addWidget(m_editName2, r++, 1, 1, 3);
    gridMat2->addWidget(new QLabel("Eg (эВ):"), r, 0);
    gridMat2->addWidget(m_spinEg2, r, 1);
    gridMat2->addWidget(new QLabel("χ (эВ):"), r, 2);
    gridMat2->addWidget(m_spinChi2, r++, 3);
    gridMat2->addWidget(new QLabel("Концентр. (см⁻³):"), r, 0);
    gridMat2->addWidget(m_editNa2, r++, 1, 1, 3);
    gridMat2->addWidget(new QLabel("mh (m₀):"), r, 0);
    gridMat2->addWidget(m_spinMh2, r, 1);
    gridMat2->addWidget(new QLabel("me (m₀):"), r, 2);
    gridMat2->addWidget(m_spinMe2, r++, 3);
    gridMat2->addWidget(new QLabel("Диэлектр. ε:"), r, 0);
    gridMat2->addWidget(m_spinEps2, r++, 1, 1, 3);

    scrollLayout->addWidget(groupMat2);

    QGroupBox *groupEnv = new QGroupBox("Условия эксперимента", scrollContent);
    groupEnv->setStyleSheet("QGroupBox { background-color: #262732; border: 1px solid #3B3C48; border-radius: 6px; margin-top: 8px; padding-top: 8px; } QGroupBox::title { color: #A8A7B6; }");
    QGridLayout *gridEnv = new QGridLayout(groupEnv);
    m_spinTemp = createDoubleSpin(10.0, 1000.0, 300.0, 1, 10.0);
    gridEnv->addWidget(new QLabel("Температура T (К):"), 0, 0);
    gridEnv->addWidget(m_spinTemp, 0, 1);
    scrollLayout->addWidget(groupEnv);

    scrollArea->setWidget(scrollContent);
    leftLayout->addWidget(scrollArea);
    leftPanel->setFixedWidth(360);

    QWidget *rightContainer = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(8);

    QHBoxLayout *kpiLayout = new QHBoxLayout();
    kpiLayout->setSpacing(8);

    kpiLayout->addWidget(createKpiCard("Разрыв зоны Ec (ΔEc)", m_kpiEcVal, "эВ", "#E67E22"));
    kpiLayout->addWidget(createKpiCard("Разрыв зоны Ev (ΔEv)", m_kpiEvVal, "эВ", "#689F8F"));
    kpiLayout->addWidget(createKpiCard("Контактный потенциал (φ₀)", m_kpiPhi0Val, "эВ", "#7E749B"));
    kpiLayout->addWidget(createKpiCard("Ширина ОПЗ (Xd)", m_kpiXdVal, "нм", "#F1C40F"));

    rightLayout->addLayout(kpiLayout);

    m_tabPlots = new QTabWidget(this);
    m_plotContact = new DiagramPlotWidget(this);
    m_plotContact->setTitle("Энергетическая диаграмма гетероперехода в контакте");

    m_plotIsolated = new DiagramPlotWidget(this);
    m_plotIsolated->setTitle("Энергетическая диаграмма изолированных материалов до контакта");

    m_plotBlocks = new BandBlocksWidget(this);

    m_plotPotential = new PotentialPlotWidget(this);

    m_tableResults = new QTableWidget(this);
    m_tableResults->setColumnCount(4);
    m_tableResults->setHorizontalHeaderLabels({"Параметр", "Обозначение", "Значение", "Единицы измерения"});
    m_tableResults->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tableResults->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tableResults->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_tableResults->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_tableResults->verticalHeader()->setVisible(false);
    m_tableResults->setAlternatingRowColors(true);
    m_tableResults->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_tabPlots->addTab(m_plotContact, "1. Зонная диаграмма в контакте");
    m_tabPlots->addTab(m_plotIsolated, "2. Диаграмма зон до контакта");
    m_tabPlots->addTab(m_plotBlocks, "3. Классическая зонная структура (Блоки)");
    m_tabPlots->addTab(m_plotPotential, "4. Распределение потенциала и поля в ОПЗ");
    m_tabPlots->addTab(m_tableResults, "5. Сводная таблица расчетов");

    rightLayout->addWidget(m_tabPlots);

    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(rightContainer);

    connect(m_comboType1, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onType1Changed);
    connect(m_comboType2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onType2Changed);
    connect(m_comboPreset1, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onPreset1Changed);
    connect(m_comboPreset2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onPreset2Changed);
    connect(m_btnCalculate, &QPushButton::clicked, this, &MainWindow::onCalculateClicked);
    connect(m_btnReset, &QPushButton::clicked, this, &MainWindow::onResetToLab1Clicked);
    connect(m_btnExport, &QPushButton::clicked, this, &MainWindow::onExportPlotClicked);
}

void MainWindow::onType1Changed(int index) {
    bool isMetal = (index == 1);
    m_spinEg1->setValue(isMetal ? 0.0 : 1.35);
    m_spinEg1->setEnabled(!isMetal);
    if (isMetal) {
        m_editNa1->setText("5.00e+22");
    }
}

void MainWindow::onType2Changed(int index) {
    bool isMetal = (index == 1);
    m_spinEg2->setValue(isMetal ? 0.0 : 2.17);
    m_spinEg2->setEnabled(!isMetal);
    if (isMetal) {
        m_editNa2->setText("5.00e+22");
    }
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
        m_comboType1->setCurrentIndex(mat.isMetal ? 1 : 0);
        m_editName1->setText(mat.name.split(" ").first());
        m_spinEg1->setValue(mat.Eg);
        m_spinChi1->setValue(mat.chi);
        m_editNa1->setText(QString::number(mat.Na, 'e', 2));
        m_spinMh1->setValue(mat.mh);
        m_spinMe1->setValue(mat.me);
        m_spinEps1->setValue(mat.eps);
    } else {
        m_comboType2->setCurrentIndex(mat.isMetal ? 1 : 0);
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
        mat.isMetal = (m_comboType1->currentIndex() == 1);
        mat.name = m_editName1->text().trimmed();
        mat.Eg = mat.isMetal ? 0.0 : m_spinEg1->value();
        mat.chi = m_spinChi1->value();
        mat.Na = m_editNa1->text().trimmed().toDouble();
        mat.mh = m_spinMh1->value();
        mat.me = m_spinMe1->value();
        mat.eps = m_spinEps1->value();
    } else {
        mat.isMetal = (m_comboType2->currentIndex() == 1);
        mat.name = m_editName2->text().trimmed();
        mat.Eg = mat.isMetal ? 0.0 : m_spinEg2->value();
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
        QMessageBox::warning(this, "Ошибка ввода", "Концентрация примесей должна быть больше нуля.");
        return;
    }

    CalculationResults res = HeterojunctionModel::calculate(m1, m2, T);

    m_kpiEcVal->setText(QString::number(res.Delta_Ec, 'f', 3));
    m_kpiEvVal->setText(QString::number(res.Delta_Ev, 'f', 3));
    m_kpiPhi0Val->setText(QString::number(res.phi0, 'f', 3));
    m_kpiXdVal->setText(QString::number(res.Xd, 'f', 2));

    m_plotContact->setMaterialNames(m1.name + " (слева)", m2.name + " (справа)");
    m_plotContact->setBoundaryPositions(res.xd1, res.xd2);
    m_plotContact->setData(res.contact_Ec, res.contact_Ev, res.contact_E0, res.contact_Ef);

    m_plotIsolated->setMaterialNames(m1.name + " (слева)", m2.name + " (справа)");
    m_plotIsolated->setBoundaryPositions(0, 0);
    m_plotIsolated->setData(res.isolated_Ec, res.isolated_Ev, res.isolated_E0, res.isolated_Ef);

    m_plotBlocks->setMaterials(m1.name, m1.Eg, m1.chi, m1.isMetal,
                               m2.name, m2.Eg, m2.chi, m2.isMetal);

    m_plotPotential->setParams(res.xd1, res.xd2, res.qphi01, res.qphi02, res.phi0);

    updateResultsUI(res);
}

void MainWindow::updateResultsUI(const CalculationResults& res) {
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
        QTableWidgetItem *itemDesc = new QTableWidgetItem(rows[i].desc);
        QTableWidgetItem *itemSym = new QTableWidgetItem(rows[i].sym);
        itemSym->setForeground(QColor("#7E749B"));
        itemSym->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *itemVal = new QTableWidgetItem(rows[i].val);
        itemVal->setForeground(QColor("#689F8F"));
        itemVal->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QFont valFont = itemVal->font();
        valFont.setBold(true);
        itemVal->setFont(valFont);

        QTableWidgetItem *itemUnit = new QTableWidgetItem(rows[i].unit);
        itemUnit->setForeground(QColor("#9A99A8"));

        m_tableResults->setItem(i, 0, itemDesc);
        m_tableResults->setItem(i, 1, itemSym);
        m_tableResults->setItem(i, 2, itemVal);
        m_tableResults->setItem(i, 3, itemUnit);
    }
}

void MainWindow::onExportPlotClicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить изображение", "heterojunction_plot.png", "Изображения (*.png *.jpg *.bmp)");
    if (!fileName.isEmpty()) {
        int currentTab = m_tabPlots->currentIndex();
        if (currentTab == 0) {
            m_plotContact->saveImage(fileName);
        } else if (currentTab == 1) {
            m_plotIsolated->saveImage(fileName);
        } else if (currentTab == 2) {
            m_plotBlocks->saveImage(fileName);
        } else if (currentTab == 3) {
            m_plotPotential->saveImage(fileName);
        }
        QMessageBox::information(this, "Экспорт", "Изображение успешно сохранено:\n" + fileName);
    }
}
