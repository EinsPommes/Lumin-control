#include "ui/mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QRandomGenerator>
#include <QDateTime>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QFileDialog>
#include <QInputDialog>

// Entfernen der Namespace-Direktive, da wir QT_CHARTS_USE_NAMESPACE verwenden
// using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(nullptr)
    , tabWidget(new QTabWidget(this))
    , currentColor(Qt::white)
    , temperatureUpdateTimer(new QTimer(this))
    , deviceManager(new DeviceManager(this))
    , rgbController(new RGBController(this))
    , sensorMonitor(new SensorMonitor(2000, this))
    , profileManager(new ProfileManager(rgbController, this))
{
    setupUi();
    createDevicesTab();
    createRGBControlTab();
    createMonitoringTab();
    createProfilesTab();
    createSettingsTab();
    setupCharts();
    connectSignals();
    
    initializePluginSystem();
    
    // Temperatur-Update-Timer starten
    temperatureUpdateTimer->setInterval(2000); // Alle 2 Sekunden aktualisieren
    temperatureUpdateTimer->start();
    
    // Sensor-Monitoring starten
    sensorMonitor->startMonitoring();
    
    // Standardprofil laden, falls vorhanden
    if (!profileManager->getDefaultProfile().isEmpty()) {
        profileManager->loadDefaultProfile();
    }
    
    statusBar()->showMessage("Ready");
    setWindowTitle("LuminControl");
    resize(1000, 800);
}

MainWindow::~MainWindow()
{
    // Sensor-Monitoring stoppen
    sensorMonitor->stopMonitoring();
    
    // Qt will handle deleting the UI elements
}

void MainWindow::setupUi()
{
    // Create central widget and layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Add tab widget to layout
    mainLayout->addWidget(tabWidget);
    
    // Set central widget
    setCentralWidget(centralWidget);
    
    // Create menu bar
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    // File menu
    QMenu *fileMenu = menuBar->addMenu("&File");
    
    QAction *exitAction = new QAction("E&xit", this);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    fileMenu->addAction(exitAction);
    
    // Help menu
    QMenu *helpMenu = menuBar->addMenu("&Help");
    
    QAction *aboutAction = new QAction("&About", this);
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "About LuminControl",
                          "LuminControl v0.1.0\n\n"
                          "A central application for controlling RGB devices, "
                          "effects, fans, and system monitoring.");
    });
    helpMenu->addAction(aboutAction);
}

void MainWindow::createDevicesTab()
{
    QWidget *devicesTab = new QWidget();
    QVBoxLayout *devicesLayout = new QVBoxLayout(devicesTab);
    
    // Create table view for devices
    devicesTableView = new QTableView(devicesTab);
    devicesModel = new QStandardItemModel(0, 3, this);
    devicesModel->setHorizontalHeaderLabels({"Gerätename", "Status", "Typ"});
    devicesTableView->setModel(devicesModel);
    devicesTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    devicesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    devicesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    devicesTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    // Create configure button
    rgbConfigureButton = new QPushButton("RGB konfigurieren", devicesTab);
    rgbConfigureButton->setEnabled(false); // Erst aktivieren, wenn Geräte ausgewählt sind
    
    // Add widgets to layout
    devicesLayout->addWidget(new QLabel("<h2>Geräteübersicht</h2>"));
    devicesLayout->addWidget(devicesTableView);
    devicesLayout->addWidget(rgbConfigureButton);
    
    // Add tab to tab widget
    tabWidget->addTab(devicesTab, "Geräte");
}

void MainWindow::createRGBControlTab()
{
    QWidget *rgbTab = new QWidget();
    QGridLayout *rgbLayout = new QGridLayout(rgbTab);
    
    // Color picker section
    QGroupBox *colorGroup = new QGroupBox("Farbauswahl", rgbTab);
    QVBoxLayout *colorLayout = new QVBoxLayout(colorGroup);
    
    colorPicker = new QColorDialog(rgbTab);
    colorPicker->setOption(QColorDialog::NoButtons, true);
    colorPicker->setOption(QColorDialog::DontUseNativeDialog, true);
    colorPicker->setCurrentColor(currentColor);
    
    // RGB sliders
    QGroupBox *slidersGroup = new QGroupBox("RGB-Werte", rgbTab);
    QGridLayout *slidersLayout = new QGridLayout(slidersGroup);
    
    redSlider = new QSlider(Qt::Horizontal, rgbTab);
    redSlider->setRange(0, 255);
    redSlider->setValue(currentColor.red());
    
    greenSlider = new QSlider(Qt::Horizontal, rgbTab);
    greenSlider->setRange(0, 255);
    greenSlider->setValue(currentColor.green());
    
    blueSlider = new QSlider(Qt::Horizontal, rgbTab);
    blueSlider->setRange(0, 255);
    blueSlider->setValue(currentColor.blue());
    
    slidersLayout->addWidget(new QLabel("Rot:"), 0, 0);
    slidersLayout->addWidget(redSlider, 0, 1);
    slidersLayout->addWidget(new QLabel("Grün:"), 1, 0);
    slidersLayout->addWidget(greenSlider, 1, 1);
    slidersLayout->addWidget(new QLabel("Blau:"), 2, 0);
    slidersLayout->addWidget(blueSlider, 2, 1);
    
    // Effect selection
    QGroupBox *effectGroup = new QGroupBox("Effekte", rgbTab);
    QVBoxLayout *effectLayout = new QVBoxLayout(effectGroup);
    
    effectComboBox = new QComboBox(rgbTab);
    effectComboBox->addItems(rgbController->getAvailableEffects());
    
    effectLayout->addWidget(effectComboBox);
    
    // Preview section
    QGroupBox *previewGroup = new QGroupBox("Vorschau", rgbTab);
    QVBoxLayout *previewLayout = new QVBoxLayout(previewGroup);
    
    previewLabel = new QLabel(rgbTab);
    previewLabel->setMinimumSize(200, 100);
    previewLabel->setFrameShape(QFrame::Box);
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setStyleSheet(QString("background-color: %1;").arg(currentColor.name()));
    
    applyButton = new QPushButton("Anwenden", rgbTab);
    
    previewLayout->addWidget(previewLabel);
    previewLayout->addWidget(applyButton);
    
    // Add all sections to main layout
    colorLayout->addWidget(colorPicker);
    
    rgbLayout->addWidget(colorGroup, 0, 0, 1, 1);
    rgbLayout->addWidget(slidersGroup, 1, 0, 1, 1);
    rgbLayout->addWidget(effectGroup, 0, 1, 1, 1);
    rgbLayout->addWidget(previewGroup, 1, 1, 1, 1);
    
    tabWidget->addTab(rgbTab, "RGB-Steuerung");
}

void MainWindow::createMonitoringTab()
{
    QWidget *monitoringTab = new QWidget();
    QVBoxLayout *monitoringLayout = new QVBoxLayout(monitoringTab);
    
    // CPU temperature
    QGroupBox *cpuGroup = new QGroupBox("CPU", monitoringTab);
    QGridLayout *cpuLayout = new QGridLayout(cpuGroup);
    
    QLabel *cpuTempLabel = new QLabel("Temperatur:", monitoringTab);
    cpuTempBar = new QProgressBar(monitoringTab);
    cpuTempBar->setRange(0, 100);
    cpuTempBar->setValue(45); // Demo value
    cpuTempBar->setFormat("%v°C");
    cpuTempBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #5DADE2, stop:0.5 #F4D03F, stop:1 #E74C3C); }");
    
    QLabel *cpuUsageLabel = new QLabel("Auslastung:", monitoringTab);
    cpuUsageBar = new QProgressBar(monitoringTab);
    cpuUsageBar->setRange(0, 100);
    cpuUsageBar->setValue(30); // Demo value
    cpuUsageBar->setFormat("%v%");
    cpuUsageBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2ECC71, stop:1 #E74C3C); }");
    
    cpuLayout->addWidget(cpuTempLabel, 0, 0);
    cpuLayout->addWidget(cpuTempBar, 0, 1);
    cpuLayout->addWidget(cpuUsageLabel, 1, 0);
    cpuLayout->addWidget(cpuUsageBar, 1, 1);
    
    // GPU temperature
    QGroupBox *gpuGroup = new QGroupBox("GPU", monitoringTab);
    QGridLayout *gpuLayout = new QGridLayout(gpuGroup);
    
    QLabel *gpuTempLabel = new QLabel("Temperatur:", monitoringTab);
    gpuTempBar = new QProgressBar(monitoringTab);
    gpuTempBar->setRange(0, 100);
    gpuTempBar->setValue(65); // Demo value
    gpuTempBar->setFormat("%v°C");
    gpuTempBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #5DADE2, stop:0.5 #F4D03F, stop:1 #E74C3C); }");
    
    QLabel *gpuUsageLabel = new QLabel("Auslastung:", monitoringTab);
    gpuUsageBar = new QProgressBar(monitoringTab);
    gpuUsageBar->setRange(0, 100);
    gpuUsageBar->setValue(40); // Demo value
    gpuUsageBar->setFormat("%v%");
    gpuUsageBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2ECC71, stop:1 #E74C3C); }");
    
    gpuLayout->addWidget(gpuTempLabel, 0, 0);
    gpuLayout->addWidget(gpuTempBar, 0, 1);
    gpuLayout->addWidget(gpuUsageLabel, 1, 0);
    gpuLayout->addWidget(gpuUsageBar, 1, 1);
    
    // Charts Container
    QWidget *chartsContainer = new QWidget(monitoringTab);
    QGridLayout *chartsLayout = new QGridLayout(chartsContainer);
    
    // Platzhalter für Charts (werden in setupCharts() initialisiert)
    cpuTempChartView = new QChartView(monitoringTab);
    cpuUsageChartView = new QChartView(monitoringTab);
    gpuTempChartView = new QChartView(monitoringTab);
    gpuUsageChartView = new QChartView(monitoringTab);
    
    cpuTempChartView->setMinimumSize(300, 150);
    cpuUsageChartView->setMinimumSize(300, 150);
    gpuTempChartView->setMinimumSize(300, 150);
    gpuUsageChartView->setMinimumSize(300, 150);
    
    chartsLayout->addWidget(cpuTempChartView, 0, 0);
    chartsLayout->addWidget(cpuUsageChartView, 0, 1);
    chartsLayout->addWidget(gpuTempChartView, 1, 0);
    chartsLayout->addWidget(gpuUsageChartView, 1, 1);
    
    // RGB temperature link
    linkRgbToTempCheckBox = new QCheckBox("RGB an Temperatur koppeln", monitoringTab);
    
    // Add all sections to main layout
    monitoringLayout->addWidget(new QLabel("<h2>System-Monitoring</h2>"));
    monitoringLayout->addWidget(cpuGroup);
    monitoringLayout->addWidget(gpuGroup);
    monitoringLayout->addWidget(chartsContainer);
    monitoringLayout->addWidget(linkRgbToTempCheckBox);
    monitoringLayout->addStretch();
    
    tabWidget->addTab(monitoringTab, "Monitoring");
}

void MainWindow::createProfilesTab()
{
    QWidget *profilesTab = new QWidget();
    QVBoxLayout *profilesLayout = new QVBoxLayout(profilesTab);
    
    // Überschrift
    profilesLayout->addWidget(new QLabel("<h2>RGB-Profile</h2>"));
    
    // Profilliste
    QGroupBox *profilesGroup = new QGroupBox("Verfügbare Profile", profilesTab);
    QVBoxLayout *profilesGroupLayout = new QVBoxLayout(profilesGroup);
    
    profilesListView = new QListView(profilesTab);
    profilesModel = new QStandardItemModel(this);
    profilesListView->setModel(profilesModel);
    profilesListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    profilesListView->setSelectionMode(QAbstractItemView::SingleSelection);
    
    profilesGroupLayout->addWidget(profilesListView);
    
    // Standardprofil-Anzeige
    QHBoxLayout *defaultProfileLayout = new QHBoxLayout();
    QLabel *defaultProfileTextLabel = new QLabel("Standardprofil:", profilesTab);
    defaultProfileLabel = new QLabel("Keines", profilesTab);
    defaultProfileLabel->setStyleSheet("font-weight: bold;");
    defaultProfileLayout->addWidget(defaultProfileTextLabel);
    defaultProfileLayout->addWidget(defaultProfileLabel);
    defaultProfileLayout->addStretch();
    
    profilesGroupLayout->addLayout(defaultProfileLayout);
    
    // Profil-Aktionen
    QGroupBox *profileActionsGroup = new QGroupBox("Profil-Aktionen", profilesTab);
    QGridLayout *profileActionsLayout = new QGridLayout(profileActionsGroup);
    
    // Profilname-Eingabe
    QLabel *profileNameLabel = new QLabel("Profilname:", profilesTab);
    profileNameEdit = new QLineEdit(profilesTab);
    
    // Checkbox für Temperaturregeln
    includeTempRulesCheckBox = new QCheckBox("Temperaturregeln einbeziehen", profilesTab);
    includeTempRulesCheckBox->setChecked(true);
    
    // Buttons
    loadProfileButton = new QPushButton("Laden", profilesTab);
    saveProfileButton = new QPushButton("Speichern", profilesTab);
    newProfileButton = new QPushButton("Neu", profilesTab);
    deleteProfileButton = new QPushButton("Löschen", profilesTab);
    setDefaultProfileButton = new QPushButton("Als Standard setzen", profilesTab);
    
    // Buttons initial deaktivieren
    loadProfileButton->setEnabled(false);
    deleteProfileButton->setEnabled(false);
    setDefaultProfileButton->setEnabled(false);
    
    // Layout für Profil-Aktionen
    profileActionsLayout->addWidget(profileNameLabel, 0, 0);
    profileActionsLayout->addWidget(profileNameEdit, 0, 1, 1, 2);
    profileActionsLayout->addWidget(includeTempRulesCheckBox, 1, 0, 1, 3);
    profileActionsLayout->addWidget(loadProfileButton, 2, 0);
    profileActionsLayout->addWidget(saveProfileButton, 2, 1);
    profileActionsLayout->addWidget(newProfileButton, 2, 2);
    profileActionsLayout->addWidget(deleteProfileButton, 3, 0);
    profileActionsLayout->addWidget(setDefaultProfileButton, 3, 1, 1, 2);
    
    // Hauptlayout
    profilesLayout->addWidget(profilesGroup);
    profilesLayout->addWidget(profileActionsGroup);
    profilesLayout->addStretch();
    
    // Tab hinzufügen
    tabWidget->addTab(profilesTab, "Profile");
    
    // Profilliste aktualisieren
    updateProfileList();
}

void MainWindow::createSettingsTab()
{
    QWidget *settingsTab = new QWidget();
    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsTab);
    
    // Theme selection
    QGroupBox *themeGroup = new QGroupBox("Theme", settingsTab);
    QVBoxLayout *themeLayout = new QVBoxLayout(themeGroup);
    
    themeComboBox = new QComboBox(settingsTab);
    themeComboBox->addItem("System");
    themeComboBox->addItem("Hell");
    themeComboBox->addItem("Dunkel");
    themeComboBox->addItem("Blau");
    
    themeLayout->addWidget(themeComboBox);
    
    // Plugins
    QGroupBox *pluginsGroup = new QGroupBox("Plugins", settingsTab);
    QVBoxLayout *pluginsLayout = new QVBoxLayout(pluginsGroup);
    
    managePluginsButton = new QPushButton("Plugins verwalten", settingsTab);
    
    pluginsLayout->addWidget(managePluginsButton);
    
    // Startup behavior
    QGroupBox *startupGroup = new QGroupBox("Startverhalten", settingsTab);
    QVBoxLayout *startupLayout = new QVBoxLayout(startupGroup);
    
    startWithSystemCheckBox = new QCheckBox("Mit System starten", settingsTab);
    
    startupLayout->addWidget(startWithSystemCheckBox);
    
    // Add all sections to main layout
    settingsLayout->addWidget(new QLabel("<h2>Einstellungen</h2>"));
    settingsLayout->addWidget(themeGroup);
    settingsLayout->addWidget(pluginsGroup);
    settingsLayout->addWidget(startupGroup);
    settingsLayout->addStretch();
    
    tabWidget->addTab(settingsTab, "Einstellungen");
}

void MainWindow::connectSignals()
{
    // Tab change signal
    connect(tabWidget, &QTabWidget::currentChanged, [this](int index) {
        QString tabName = tabWidget->tabText(index);
        statusBar()->showMessage("Tab gewechselt: " + tabName);
    });
    
    // RGB Configure button
    connect(rgbConfigureButton, &QPushButton::clicked, this, &MainWindow::onRGBConfigureClicked);
    
    // Color picker
    connect(colorPicker, &QColorDialog::currentColorChanged, this, &MainWindow::onColorSelected);
    
    // RGB sliders
    connect(redSlider, &QSlider::valueChanged, [this](int value) {
        currentColor.setRed(value);
        updateColorPreview();
    });
    
    connect(greenSlider, &QSlider::valueChanged, [this](int value) {
        currentColor.setGreen(value);
        updateColorPreview();
    });
    
    connect(blueSlider, &QSlider::valueChanged, [this](int value) {
        currentColor.setBlue(value);
        updateColorPreview();
    });
    
    // Effect selection
    connect(effectComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onEffectSelected);
    
    // Apply button
    connect(applyButton, &QPushButton::clicked, this, &MainWindow::onApplyClicked);
    
    // Profile buttons
    connect(loadProfileButton, &QPushButton::clicked, this, &MainWindow::onProfileLoad);
    connect(saveProfileButton, &QPushButton::clicked, this, &MainWindow::onProfileSave);
    connect(newProfileButton, &QPushButton::clicked, this, &MainWindow::onProfileNew);
    connect(deleteProfileButton, &QPushButton::clicked, this, &MainWindow::onProfileDelete);
    connect(setDefaultProfileButton, &QPushButton::clicked, this, &MainWindow::onProfileSetDefault);
    connect(profilesListView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::onProfileSelectionChanged);
    
    // ProfileManager connections
    connect(profileManager, &ProfileManager::profileLoaded, this, &MainWindow::onProfileLoaded);
    connect(profileManager, &ProfileManager::profileSaved, this, &MainWindow::onProfileSaved);
    connect(profileManager, &ProfileManager::profileDeleted, this, &MainWindow::onProfileDeleted);
    connect(profileManager, &ProfileManager::error, this, &MainWindow::onProfileError);
    
    // Theme selection
    connect(themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onThemeChanged);
    
    // Device selection
    connect(devicesTableView->selectionModel(), &QItemSelectionModel::selectionChanged, 
            this, &MainWindow::onDeviceSelectionChanged);
    
    // Device Manager signals
    connect(deviceManager, &DeviceManager::deviceDiscovered, this, &MainWindow::onDeviceDiscovered);
    
    // RGB Controller signals
    connect(rgbController, &RGBController::colorChanged, this, &MainWindow::onRGBColorChanged);
    connect(rgbController, &RGBController::effectChanged, this, &MainWindow::onRGBEffectChanged);
    connect(rgbController, &RGBController::actionSuccess, this, &MainWindow::onRGBActionSuccess);
    connect(rgbController, &RGBController::actionError, this, &MainWindow::onRGBActionError);
    
    // Temperature linking
    connect(linkRgbToTempCheckBox, &QCheckBox::checkStateChanged, this, &MainWindow::onLinkRgbToTempChanged);
    
    // Temperature update timer
    connect(temperatureUpdateTimer, &QTimer::timeout, this, &MainWindow::updateTemperatures);
    
    // Sensor Monitor signals
    connect(sensorMonitor, &SensorMonitor::cpuTemperatureChanged, this, &MainWindow::onCpuTemperatureChanged);
    connect(sensorMonitor, &SensorMonitor::cpuUsageChanged, this, &MainWindow::onCpuUsageChanged);
    connect(sensorMonitor, &SensorMonitor::gpuTemperatureChanged, this, &MainWindow::onGpuTemperatureChanged);
    connect(sensorMonitor, &SensorMonitor::gpuUsageChanged, this, &MainWindow::onGpuUsageChanged);
    connect(sensorMonitor, &SensorMonitor::sensorsUpdated, this, &MainWindow::onSensorsUpdated);
}

void MainWindow::onRGBConfigureClicked()
{
    // Überprüfen, ob Geräte ausgewählt sind
    QModelIndexList selectedIndexes = devicesTableView->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "Keine Geräte ausgewählt", 
                            "Bitte wähle mindestens ein Gerät aus der Liste aus.");
        return;
    }
    
    // Switch to RGB Control tab
    tabWidget->setCurrentIndex(1);
    statusBar()->showMessage("RGB-Konfiguration geöffnet");
}

void MainWindow::onColorSelected(const QColor &color)
{
    currentColor = color;
    
    // Update sliders
    redSlider->setValue(color.red());
    greenSlider->setValue(color.green());
    blueSlider->setValue(color.blue());
    
    updateColorPreview();
}

void MainWindow::onEffectSelected(int index)
{
    QString effectName = effectComboBox->itemText(index);
    statusBar()->showMessage("Effekt ausgewählt: " + effectName);
}

void MainWindow::onApplyClicked()
{
    if (selectedDevices.isEmpty()) {
        QMessageBox::warning(this, "Keine Geräte ausgewählt", 
                            "Bitte wähle mindestens ein Gerät in der Geräteübersicht aus.");
        return;
    }
    
    // Farbe und Effekt auf ausgewählte Geräte anwenden
    applyColorToSelectedDevices();
    applyEffectToSelectedDevices();
}

void MainWindow::onProfileLoad()
{
    QString profileName = profileNameEdit->text().trimmed();
    
    // Wenn kein Name eingegeben wurde, aber ein Profil ausgewählt ist
    if (profileName.isEmpty() && profilesListView->selectionModel()->hasSelection()) {
        QModelIndex index = profilesListView->selectionModel()->selectedIndexes().first();
        profileName = profilesModel->data(index).toString();
    }
    
    if (profileName.isEmpty()) {
        QMessageBox::warning(this, "Profil laden", "Bitte geben Sie einen Profilnamen ein oder wählen Sie ein Profil aus der Liste.");
        return;
    }
    
    profileManager->loadProfile(profileName);
}

void MainWindow::onProfileSave()
{
    QString profileName = profileNameEdit->text().trimmed();
    
    if (profileName.isEmpty()) {
        QMessageBox::warning(this, "Profil speichern", "Bitte geben Sie einen Profilnamen ein.");
        return;
    }
    
    // Bestätigung anfordern, wenn das Profil bereits existiert
    if (profileManager->profileExists(profileName)) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, 
            "Profil überschreiben", 
            QString("Das Profil '%1' existiert bereits. Möchten Sie es überschreiben?").arg(profileName),
            QMessageBox::Yes | QMessageBox::No
        );
        
        if (reply == QMessageBox::No) {
            return;
        }
    }
    
    bool includeTemperatureRules = includeTempRulesCheckBox->isChecked();
    profileManager->saveProfile(profileName, includeTemperatureRules);
}

void MainWindow::onProfileNew()
{
    profileNameEdit->clear();
    profileNameEdit->setFocus();
}

void MainWindow::onProfileDelete()
{
    QString profileName;
    
    // Wenn ein Profil ausgewählt ist
    if (profilesListView->selectionModel()->hasSelection()) {
        QModelIndex index = profilesListView->selectionModel()->selectedIndexes().first();
        profileName = profilesModel->data(index).toString();
    } else {
        profileName = profileNameEdit->text().trimmed();
    }
    
    if (profileName.isEmpty()) {
        QMessageBox::warning(this, "Profil löschen", "Bitte wählen Sie ein Profil aus der Liste oder geben Sie einen Profilnamen ein.");
        return;
    }
    
    // Bestätigung anfordern
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, 
        "Profil löschen", 
        QString("Möchten Sie das Profil '%1' wirklich löschen?").arg(profileName),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        profileManager->deleteProfile(profileName);
    }
}

void MainWindow::onProfileSetDefault()
{
    QString profileName;
    
    // Wenn ein Profil ausgewählt ist
    if (profilesListView->selectionModel()->hasSelection()) {
        QModelIndex index = profilesListView->selectionModel()->selectedIndexes().first();
        profileName = profilesModel->data(index).toString();
    } else {
        profileName = profileNameEdit->text().trimmed();
    }
    
    if (profileName.isEmpty()) {
        QMessageBox::warning(this, "Standardprofil", "Bitte wählen Sie ein Profil aus der Liste oder geben Sie einen Profilnamen ein.");
        return;
    }
    
    if (!profileManager->profileExists(profileName)) {
        QMessageBox::warning(this, "Standardprofil", QString("Das Profil '%1' existiert nicht.").arg(profileName));
        return;
    }
    
    if (profileManager->setDefaultProfile(profileName)) {
        defaultProfileLabel->setText(profileName);
        showStatusMessage(QString("Profil '%1' wurde als Standard festgelegt.").arg(profileName));
    }
}

void MainWindow::onProfileSelectionChanged()
{
    bool hasSelection = profilesListView->selectionModel()->hasSelection();
    
    // Buttons aktivieren/deaktivieren
    loadProfileButton->setEnabled(hasSelection);
    deleteProfileButton->setEnabled(hasSelection);
    setDefaultProfileButton->setEnabled(hasSelection);
    
    // Profilnamen in das Eingabefeld übernehmen
    if (hasSelection) {
        QModelIndex index = profilesListView->selectionModel()->selectedIndexes().first();
        profileNameEdit->setText(profilesModel->data(index).toString());
    }
}

void MainWindow::onProfileLoaded(const QString &profileName)
{
    showStatusMessage(QString("Profil '%1' wurde geladen.").arg(profileName));
}

void MainWindow::onProfileSaved(const QString &profileName)
{
    showStatusMessage(QString("Profil '%1' wurde gespeichert.").arg(profileName));
    updateProfileList();
}

void MainWindow::onProfileDeleted(const QString &profileName)
{
    showStatusMessage(QString("Profil '%1' wurde gelöscht.").arg(profileName));
    updateProfileList();
    
    // Wenn das gelöschte Profil das aktuelle Standardprofil war, Anzeige aktualisieren
    if (defaultProfileLabel->text() == profileName) {
        defaultProfileLabel->setText("Keines");
    }
}

void MainWindow::onProfileError(const QString &errorMessage)
{
    showStatusMessage(errorMessage, 5000);
    QMessageBox::warning(this, "Profilfehler", errorMessage);
}

void MainWindow::updateProfileList()
{
    // Profilliste leeren
    profilesModel->clear();
    
    // Verfügbare Profile abrufen und in die Liste einfügen
    QStringList profiles = profileManager->getAvailableProfiles();
    for (const QString &profileName : profiles) {
        profilesModel->appendRow(new QStandardItem(profileName));
    }
    
    // Standardprofil anzeigen
    QString defaultProfile = profileManager->getDefaultProfile();
    if (!defaultProfile.isEmpty()) {
        defaultProfileLabel->setText(defaultProfile);
    } else {
        defaultProfileLabel->setText("Keines");
    }
}

void MainWindow::showStatusMessage(const QString &message, int timeout)
{
    statusBar()->showMessage(message, timeout);
}

void MainWindow::onThemeChanged(int index)
{
    QString themeName = themeComboBox->itemText(index);
    statusBar()->showMessage("Theme geändert: " + themeName);
    
    // Here you would apply the actual theme changes
}

void MainWindow::updateColorPreview()
{
    // Update color preview
    previewLabel->setStyleSheet(QString("background-color: %1;").arg(currentColor.name()));
}

void MainWindow::initializePluginSystem()
{
    // System initialisieren
    deviceManager->loadPlugins();
    
    // Statusmeldung anzeigen
    statusBar()->showMessage("LuminControl bereit");
    
    // Hinweis anzeigen, dass die Anwendung bereit für Plugins ist
    QMessageBox::information(this, "LuminControl", 
                            "LuminControl wurde erfolgreich gestartet.\n\n"
                            "Die Profilverwaltung und Benutzeroberfläche sind voll funktionsfähig. "
                            "Legen Sie RGB-Geräte-Plugins im 'plugins'-Verzeichnis ab, um "
                            "Ihre Hardware zu steuern.");
}

void MainWindow::onDeviceDiscovered(IRGBDevice *device)
{
    if (!device) return;
    
    // Gerät zur Liste hinzufügen
    int row = devicesModel->rowCount();
    devicesModel->insertRow(row);
    
    // Gerätename
    QStandardItem *nameItem = new QStandardItem(device->getDisplayName());
    devicesModel->setItem(row, 0, nameItem);
    
    // Gerätestatus
    QString status = device->isConnected() ? "Verbunden" : "Nicht verbunden";
    QStandardItem *statusItem = new QStandardItem(status);
    devicesModel->setItem(row, 1, statusItem);
    
    // Gerätetyp
    QStandardItem *typeItem = new QStandardItem(device->getType());
    devicesModel->setItem(row, 2, typeItem);
    
    // Statusmeldung anzeigen
    showStatusMessage(QString("Neues Gerät erkannt: %1").arg(device->getDisplayName()));
}

void MainWindow::onDeviceSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    // Liste der ausgewählten Geräte aktualisieren
    selectedDevices.clear();
    
    QModelIndexList selectedRows = devicesTableView->selectionModel()->selectedRows();
    for (const QModelIndex &index : selectedRows) {
        QString deviceId = devicesModel->data(devicesModel->index(index.row(), 0), Qt::UserRole).toString();
        IRGBDevice *device = deviceManager->getDeviceById(deviceId);
        if (device) {
            selectedDevices.append(device);
        }
    }
    
    // RGB-Konfigurationsbutton aktivieren/deaktivieren
    rgbConfigureButton->setEnabled(!selectedDevices.isEmpty());
}

void MainWindow::applyColorToSelectedDevices()
{
    // RGBController verwenden, um die Farbe auf die ausgewählten Geräte anzuwenden
    rgbController->setColorForDevices(selectedDevices, currentColor);
}

void MainWindow::applyEffectToSelectedDevices()
{
    // Effekt-Parameter festlegen
    QVariantMap parameters;
    parameters["color"] = currentColor;
    
    // RGBController verwenden, um den Effekt auf die ausgewählten Geräte anzuwenden
    QString effectName = effectComboBox->currentText();
    rgbController->setEffectForDevices(selectedDevices, effectName, parameters);
}

void MainWindow::onRGBColorChanged(const QString &deviceId, const QColor &color)
{
    // Hier könnte man die UI aktualisieren, wenn sich die Farbe eines Geräts ändert
    // z.B. die Vorschau aktualisieren, wenn das aktuell ausgewählte Gerät betroffen ist
    
    // Gerät in der Tabelle finden und Status aktualisieren
    for (int row = 0; row < devicesModel->rowCount(); ++row) {
        QString id = devicesModel->data(devicesModel->index(row, 0), Qt::UserRole).toString();
        if (id == deviceId) {
            // Status aktualisieren (optional)
            devicesModel->setData(devicesModel->index(row, 1), "Verbunden");
            break;
        }
    }
}

void MainWindow::onRGBEffectChanged(const QString &deviceId, const QString &effectName)
{
    // Hier könnte man die UI aktualisieren, wenn sich der Effekt eines Geräts ändert
    // z.B. den ausgewählten Effekt im ComboBox aktualisieren
    
    // Wenn das aktuelle Gerät betroffen ist, Effekt im ComboBox auswählen
    IRGBDevice *device = deviceManager->getDeviceById(deviceId);
    if (device && selectedDevices.contains(device)) {
        int index = effectComboBox->findText(effectName);
        if (index >= 0) {
            effectComboBox->setCurrentIndex(index);
        }
    }
}

void MainWindow::onRGBActionSuccess(const QString &message)
{
    // Erfolgsmeldung in der Statusleiste anzeigen
    statusBar()->showMessage(message, 3000); // 3 Sekunden anzeigen
}

void MainWindow::onRGBActionError(const QString &message)
{
    // Fehlermeldung in einem Dialog anzeigen
    QMessageBox::warning(this, "RGB-Fehler", message);
}

void MainWindow::onLinkRgbToTempChanged(int state)
{
    bool enabled = (state == Qt::Checked);
    rgbController->setTemperatureLinking(enabled);
    
    if (enabled) {
        statusBar()->showMessage("RGB-Farben werden jetzt an Temperaturen gekoppelt");
        
        // Sofort Temperaturen aktualisieren
        updateTemperatures();
    } else {
        statusBar()->showMessage("RGB-Farben werden nicht mehr an Temperaturen gekoppelt");
    }
}

void MainWindow::updateTemperatures()
{
    // In einer realen Anwendung würden hier die tatsächlichen Temperaturen abgefragt werden
    // Für Demo-Zwecke verwenden wir zufällige Werte
    
    // Zufällige Temperaturwerte generieren (mit leichten Schwankungen)
    static int lastCpuTemp = 45;
    static int lastGpuTemp = 65;
    
    int cpuTemp = qBound(30, lastCpuTemp + QRandomGenerator::global()->bounded(-3, 4), 90);
    int gpuTemp = qBound(30, lastGpuTemp + QRandomGenerator::global()->bounded(-3, 4), 90);
    
    lastCpuTemp = cpuTemp;
    lastGpuTemp = gpuTemp;
    
    // Temperaturanzeigen aktualisieren
    cpuTempBar->setValue(cpuTemp);
    gpuTempBar->setValue(gpuTemp);
    
    // Temperaturwerte an den RGBController weitergeben
    rgbController->updateTemperatures(cpuTemp, gpuTemp);
}

void MainWindow::setupCharts()
{
    // CPU Temperatur Chart
    cpuTempChart = new QChart();
    cpuTempChart->setTitle("CPU Temperatur");
    cpuTempChart->legend()->hide();
    
    cpuTempSeries = new QLineSeries();
    cpuTempChart->addSeries(cpuTempSeries);
    
    QValueAxis *cpuTempAxisX = new QValueAxis();
    cpuTempAxisX->setRange(0, 60);
    cpuTempAxisX->setLabelFormat("%g");
    cpuTempAxisX->setTitleText("Zeit (s)");
    
    QValueAxis *cpuTempAxisY = new QValueAxis();
    cpuTempAxisY->setRange(0, 100);
    cpuTempAxisY->setLabelFormat("%g");
    cpuTempAxisY->setTitleText("Temperatur (°C)");
    
    cpuTempChart->addAxis(cpuTempAxisX, Qt::AlignBottom);
    cpuTempChart->addAxis(cpuTempAxisY, Qt::AlignLeft);
    cpuTempSeries->attachAxis(cpuTempAxisX);
    cpuTempSeries->attachAxis(cpuTempAxisY);
    
    cpuTempChartView = new QChartView(cpuTempChart);
    cpuTempChartView->setRenderHint(QPainter::Antialiasing);
    
    // CPU Auslastung Chart
    cpuUsageChart = new QChart();
    cpuUsageChart->setTitle("CPU Auslastung");
    cpuUsageChart->legend()->hide();
    
    cpuUsageSeries = new QLineSeries();
    cpuUsageChart->addSeries(cpuUsageSeries);
    
    QValueAxis *cpuUsageAxisX = new QValueAxis();
    cpuUsageAxisX->setRange(0, 60);
    cpuUsageAxisX->setLabelFormat("%g");
    cpuUsageAxisX->setTitleText("Zeit (s)");
    
    QValueAxis *cpuUsageAxisY = new QValueAxis();
    cpuUsageAxisY->setRange(0, 100);
    cpuUsageAxisY->setLabelFormat("%g");
    cpuUsageAxisY->setTitleText("Auslastung (%)");
    
    cpuUsageChart->addAxis(cpuUsageAxisX, Qt::AlignBottom);
    cpuUsageChart->addAxis(cpuUsageAxisY, Qt::AlignLeft);
    cpuUsageSeries->attachAxis(cpuUsageAxisX);
    cpuUsageSeries->attachAxis(cpuUsageAxisY);
    
    cpuUsageChartView = new QChartView(cpuUsageChart);
    cpuUsageChartView->setRenderHint(QPainter::Antialiasing);
    
    // GPU Temperatur Chart
    gpuTempChart = new QChart();
    gpuTempChart->setTitle("GPU Temperatur");
    gpuTempChart->legend()->hide();
    
    gpuTempSeries = new QLineSeries();
    gpuTempChart->addSeries(gpuTempSeries);
    
    QValueAxis *gpuTempAxisX = new QValueAxis();
    gpuTempAxisX->setRange(0, 60);
    gpuTempAxisX->setLabelFormat("%g");
    gpuTempAxisX->setTitleText("Zeit (s)");
    
    QValueAxis *gpuTempAxisY = new QValueAxis();
    gpuTempAxisY->setRange(0, 100);
    gpuTempAxisY->setLabelFormat("%g");
    gpuTempAxisY->setTitleText("Temperatur (°C)");
    
    gpuTempChart->addAxis(gpuTempAxisX, Qt::AlignBottom);
    gpuTempChart->addAxis(gpuTempAxisY, Qt::AlignLeft);
    gpuTempSeries->attachAxis(gpuTempAxisX);
    gpuTempSeries->attachAxis(gpuTempAxisY);
    
    gpuTempChartView = new QChartView(gpuTempChart);
    gpuTempChartView->setRenderHint(QPainter::Antialiasing);
    
    // GPU Auslastung Chart
    gpuUsageChart = new QChart();
    gpuUsageChart->setTitle("GPU Auslastung");
    gpuUsageChart->legend()->hide();
    
    gpuUsageSeries = new QLineSeries();
    gpuUsageChart->addSeries(gpuUsageSeries);
    
    QValueAxis *gpuUsageAxisX = new QValueAxis();
    gpuUsageAxisX->setRange(0, 60);
    gpuUsageAxisX->setLabelFormat("%g");
    gpuUsageAxisX->setTitleText("Zeit (s)");
    
    QValueAxis *gpuUsageAxisY = new QValueAxis();
    gpuUsageAxisY->setRange(0, 100);
    gpuUsageAxisY->setLabelFormat("%g");
    gpuUsageAxisY->setTitleText("Auslastung (%)");
    
    gpuUsageChart->addAxis(gpuUsageAxisX, Qt::AlignBottom);
    gpuUsageChart->addAxis(gpuUsageAxisY, Qt::AlignLeft);
    gpuUsageSeries->attachAxis(gpuUsageAxisX);
    gpuUsageSeries->attachAxis(gpuUsageAxisY);
    
    gpuUsageChartView = new QChartView(gpuUsageChart);
    gpuUsageChartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::updateCharts()
{
    // Temporäre leere Implementierung, bis die QtCharts-Probleme gelöst sind
    qDebug() << "Chart-Update übersprungen";
}

void MainWindow::onCpuTemperatureChanged(int temperature)
{
    cpuTempBar->setValue(temperature);
    
    // Farbe des Balkens basierend auf Temperatur anpassen
    if (temperature < 50) {
        cpuTempBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: #5DADE2; }");
    } else if (temperature < 70) {
        cpuTempBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: #F4D03F; }");
    } else {
        cpuTempBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: #E74C3C; }");
    }
    
    // Aktualisiere Charts
    updateCharts();
}

void MainWindow::onCpuUsageChanged(int usage)
{
    cpuUsageBar->setValue(usage);
    
    // Farbe des Balkens basierend auf Auslastung anpassen
    if (usage < 50) {
        cpuUsageBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: #2ECC71; }");
    } else if (usage < 80) {
        cpuUsageBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: #F4D03F; }");
    } else {
        cpuUsageBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: #E74C3C; }");
    }
    
    // Aktualisiere Charts
    updateCharts();
}

void MainWindow::onGpuTemperatureChanged(int temperature)
{
    gpuTempBar->setValue(temperature);
    
    // Farbe des Balkens basierend auf Temperatur anpassen
    if (temperature < 60) {
        gpuTempBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: #5DADE2; }");
    } else if (temperature < 80) {
        gpuTempBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: #F4D03F; }");
    } else {
        gpuTempBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: #E74C3C; }");
    }
    
    // Aktualisiere Charts
    updateCharts();
}

void MainWindow::onGpuUsageChanged(int usage)
{
    gpuUsageBar->setValue(usage);
    
    // Farbe des Balkens basierend auf Auslastung anpassen
    if (usage < 50) {
        gpuUsageBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: #2ECC71; }");
    } else if (usage < 80) {
        gpuUsageBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: #F4D03F; }");
    } else {
        gpuUsageBar->setStyleSheet("QProgressBar { text-align: center; } QProgressBar::chunk { background-color: #E74C3C; }");
    }
    
    // Aktualisiere Charts
    updateCharts();
}

void MainWindow::onSensorsUpdated()
{
    // Diese Methode wird aufgerufen, wenn alle Sensoren aktualisiert wurden
    // Hier können wir zusätzliche Aktualisierungen vornehmen, die von mehreren Sensoren abhängen
    
    // Temperaturabhängige RGB-Farbe aktualisieren, falls aktiviert
    if (linkRgbToTempCheckBox && linkRgbToTempCheckBox->isChecked()) {
        updateTemperatures();
    }
}

// Methode onSensorsUpdated wurde temporär entfernt wegen Kompilierungsproblemen
