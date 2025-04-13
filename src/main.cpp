#include <QApplication>
#include <QMessageBox>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    // QApplication erstellen
    QApplication app(argc, argv);
    
    // Anwendungsinformationen setzen
    QApplication::setApplicationName("LuminControl");
    QApplication::setApplicationVersion("0.1.0");
    QApplication::setOrganizationName("LuminControl");
    QApplication::setOrganizationDomain("lumincontrol.org");
    
    try {
        // Hauptfenster erstellen und anzeigen
        MainWindow mainWindow;
        mainWindow.show();
        
        // Anwendung ausführen
        return app.exec();
    } catch (const std::exception& e) {
        // Fehlerbehandlung
        QMessageBox::critical(nullptr, "Fehler", 
            QString("Ein kritischer Fehler ist aufgetreten: %1").arg(e.what()));
        return 1;
    } catch (...) {
        // Unbekannte Fehlerbehandlung
        QMessageBox::critical(nullptr, "Fehler", 
            "Ein unbekannter kritischer Fehler ist aufgetreten.");
        return 1;
    }
}
