#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QListView>
#include <QTableView>
#include <QPushButton>
#include <QColorDialog>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QProgressBar>
#include <QStandardItemModel>
#include <QSettings>
#include <QTimer>
#include <QLineEdit>
#include <QMessageBox>
#include <QStatusBar>
#include <QDateTime>
#include <QDebug>

#include "core/devicemanager.h"
#include "core/rgbcontroller.h"
#include "core/profilemanager.h"
#include "devices/irgbdevice.h"
#include "monitoring/sensormonitor.h"

// QtCharts-Includes - mit vollständigen Namespaces verwenden
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>

// Definiere QT_CHARTS_USE_NAMESPACE, um die QtCharts-Klassen ohne Präfix verwenden zu können
#define QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Wir verwenden keine Namespace-Direktiven, sondern vollständige Namespaces
// using namespace QtCharts;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onRGBConfigureClicked();
    void onColorSelected(const QColor &color);
    void onEffectSelected(int index);
    void onApplyClicked();
    void onProfileLoad();
    void onProfileSave();
    void onProfileNew();
    void onProfileDelete();
    void onProfileSetDefault();
    void onProfileSelectionChanged();
    void onThemeChanged(int index);
    void onDeviceDiscovered(IRGBDevice *device);
    void onDeviceSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onRGBColorChanged(const QString &deviceId, const QColor &color);
    void onRGBEffectChanged(const QString &deviceId, const QString &effectName);
    void onRGBActionSuccess(const QString &message);
    void onRGBActionError(const QString &message);
    void onLinkRgbToTempChanged(int state);
    void updateTemperatures();
    
    // Slots für Sensorüberwachung
    void onCpuTemperatureChanged(int temperature);
    void onCpuUsageChanged(int usage);
    void onGpuTemperatureChanged(int temperature);
    void onGpuUsageChanged(int usage);
    void onSensorsUpdated();
    void updateCharts();
    
    // Slots für Profilmanager
    void onProfileLoaded(const QString &profileName);
    void onProfileSaved(const QString &profileName);
    void onProfileDeleted(const QString &profileName);
    void onProfileError(const QString &errorMessage);
    void updateProfileList();

private:
    void setupUi();
    void createDevicesTab();
    void createRGBControlTab();
    void createMonitoringTab();
    void createProfilesTab();
    void createSettingsTab();
    void connectSignals();
    void updateColorPreview();
    void initializePluginSystem();
    void updateDeviceList();
    void applyColorToSelectedDevices();
    void applyEffectToSelectedDevices();
    void setupCharts();
    void showStatusMessage(const QString &message, int timeout = 3000);

private:
    Ui::MainWindow *ui;
    QTabWidget *tabWidget;
    
    // Devices Tab
    QTableView *devicesTableView;
    QStandardItemModel *devicesModel;
    QPushButton *rgbConfigureButton;
    
    // RGB Control Tab
    QColorDialog *colorPicker;
    QSlider *redSlider;
    QSlider *greenSlider;
    QSlider *blueSlider;
    QComboBox *effectComboBox;
    QLabel *previewLabel;
    QPushButton *applyButton;
    QColor currentColor;
    
    // Monitoring Tab
    QProgressBar *cpuTempBar;
    QProgressBar *cpuUsageBar;
    QProgressBar *gpuTempBar;
    QProgressBar *gpuUsageBar;
    QCheckBox *linkRgbToTempCheckBox;
    QTimer *temperatureUpdateTimer;
    QChartView *cpuTempChartView;
    QChartView *cpuUsageChartView;
    QChartView *gpuTempChartView;
    QChartView *gpuUsageChartView;
    QChart *cpuTempChart;
    QChart *cpuUsageChart;
    QChart *gpuTempChart;
    QChart *gpuUsageChart;
    QLineSeries *cpuTempSeries;
    QLineSeries *cpuUsageSeries;
    QLineSeries *gpuTempSeries;
    QLineSeries *gpuUsageSeries;
    
    // Profiles Tab
    QListView *profilesListView;
    QStandardItemModel *profilesModel;
    QLabel *defaultProfileLabel;
    QLineEdit *profileNameEdit;
    QCheckBox *includeTempRulesCheckBox;
    QPushButton *loadProfileButton;
    QPushButton *saveProfileButton;
    QPushButton *newProfileButton;
    QPushButton *deleteProfileButton;
    QPushButton *setDefaultProfileButton;
    
    // Settings Tab
    QComboBox *themeComboBox;
    QCheckBox *startWithSystemCheckBox;
    QPushButton *managePluginsButton;
    
    // Plugin System
    DeviceManager *deviceManager;
    RGBController *rgbController;
    QList<IRGBDevice*> selectedDevices;
    
    // Sensor Monitoring
    SensorMonitor *sensorMonitor;
    
    // Profile Management
    ProfileManager *profileManager;
};
