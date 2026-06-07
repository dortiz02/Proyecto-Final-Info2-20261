#include <QApplication>
#include <QMessageBox>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Pokemon Swim Championship");
    app.setApplicationVersion("1.0");

    try {
        MainWindow ventana;
        ventana.show();
        return app.exec();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Error al iniciar", e.what());
        return 1;
    }
}
