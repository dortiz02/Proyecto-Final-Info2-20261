#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <stdexcept>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      juego(new Juego()),
      nivelWidget(nullptr)
{
    setWindowTitle("Pokémon Swim Championship");
    setMinimumSize(900, 600);

    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    construirMenu();
    construirPantallaResultado();

    nivelWidget = new NivelWidget(juego, this);
    connect(nivelWidget, &NivelWidget::nivelTerminado, this, &MainWindow::onNivelTerminado);

    stack->addWidget(pantallaMenu);       // 0
    stack->addWidget(nivelWidget);        // 1
    stack->addWidget(pantallaResultado);  // 2

    mostrarMenu();
}

MainWindow::~MainWindow() {
    delete juego;
}

void MainWindow::construirMenu() {
    pantallaMenu    = new QWidget();
    QVBoxLayout* l  = new QVBoxLayout(pantallaMenu);
    l->setAlignment(Qt::AlignCenter);

    QLabel* titulo = new QLabel("Pokémon Swim Championship");
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-size:28px; font-weight:bold; color:#1a3a6a; margin:20px;");

    QLabel* sub = new QLabel("Compite en las aguas del mundo Pokémon");
    sub->setAlignment(Qt::AlignCenter);
    sub->setStyleSheet("font-size:14px; color:#444; margin-bottom:20px;");

    QLabel* lblDif = new QLabel("Dificultad del Nivel 1:");
    lblDif->setAlignment(Qt::AlignCenter);
    lblDif->setStyleSheet("font-size:13px;");

    comboDificultad = new QComboBox();
    comboDificultad->addItem("Fácil",   1);
    comboDificultad->addItem("Normal",  2);
    comboDificultad->addItem("Difícil", 3);
    comboDificultad->setFixedWidth(200);
    comboDificultad->setStyleSheet("font-size:15px; padding:4px;");

    QPushButton* btnJugar = new QPushButton("¡Jugar!");
    btnJugar->setFixedSize(200, 50);
    btnJugar->setStyleSheet(
        "QPushButton{background:#1a7abf;color:white;font-size:18px;border-radius:8px;}"
        "QPushButton:hover{background:#1560a0;}"
    );
    connect(btnJugar, &QPushButton::clicked, this, &MainWindow::onJugar);

    QLabel* controles = new QLabel(
        "Nivel 1: A/D para bracear | E = Mordedor\n"
        "Nivel 2: W/A/S/D para moverse | E = Mordedor"
    );
    controles->setAlignment(Qt::AlignCenter);
    controles->setStyleSheet("font-size:12px; color:#666; margin-top:20px;");

    l->addWidget(titulo);
    l->addWidget(sub);
    l->addSpacing(10);
    l->addWidget(lblDif, 0, Qt::AlignCenter);
    l->addWidget(comboDificultad, 0, Qt::AlignCenter);
    l->addSpacing(20);
    l->addWidget(btnJugar, 0, Qt::AlignCenter);
    l->addWidget(controles);
}

void MainWindow::construirPantallaResultado() {
    pantallaResultado = new QWidget();
    QVBoxLayout* l    = new QVBoxLayout(pantallaResultado);
    l->setAlignment(Qt::AlignCenter);

    labelResultado = new QLabel();
    labelResultado->setAlignment(Qt::AlignCenter);
    labelResultado->setStyleSheet("font-size:22px; font-weight:bold; margin:20px;");

    btnSiguiente = new QPushButton("Siguiente nivel →");
    btnSiguiente->setFixedSize(220, 48);
    btnSiguiente->setStyleSheet(
        "QPushButton{background:#2a9a50;color:white;font-size:16px;border-radius:8px;}"
        "QPushButton:hover{background:#1a7a40;}"
    );
    connect(btnSiguiente, &QPushButton::clicked, this, &MainWindow::onSiguienteNivel);

    btnMenu = new QPushButton("Volver al menú");
    btnMenu->setFixedSize(220, 48);
    btnMenu->setStyleSheet(
        "QPushButton{background:#888;color:white;font-size:16px;border-radius:8px;}"
        "QPushButton:hover{background:#666;}"
    );
    connect(btnMenu, &QPushButton::clicked, this, &MainWindow::onVolverMenu);

    l->addWidget(labelResultado);
    l->addSpacing(20);
    l->addWidget(btnSiguiente, 0, Qt::AlignCenter);
    l->addSpacing(8);
    l->addWidget(btnMenu,      0, Qt::AlignCenter);
}

void MainWindow::onJugar() {
    try {
        int dif = comboDificultad->currentData().toInt();
        juego->setDificultad(dif);
        juego->iniciarNivel(1);
        mostrarNivel();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

void MainWindow::onNivelTerminado(int ganador) {
    nivelWidget->detener();
    mostrarResultado(ganador);
}

void MainWindow::onSiguienteNivel() {
    try {
        int siguiente = juego->getNivelActualNum() + 1;
        if (siguiente > 2) { mostrarMenu(); return; }
        juego->iniciarNivel(siguiente);
        mostrarNivel();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

void MainWindow::onVolverMenu() { mostrarMenu(); }

void MainWindow::mostrarMenu()  { stack->setCurrentIndex(0); }

void MainWindow::mostrarNivel() {
    stack->setCurrentIndex(1);
    nivelWidget->iniciar();
}

void MainWindow::mostrarResultado(int ganador) {
    int n = juego->getNivelActualNum();
    QString msg;
    if (ganador == 0)
        msg = (n == 1)
            ? "¡Ganaste la carrera!\nTotodile avanza al río Safari."
            : "¡Completaste el recorrido!\n¡Eres el Pokémon Swim Champion!";
    else
        msg = (n == 1)
            ? "Psyduck ganó esta carrera.\n¡Inténtalo de nuevo!"
            : "Se agotó el tiempo.\nPsyduck gana esta vez.";

    labelResultado->setText(msg);
    btnSiguiente->setVisible(n < 2 && ganador == 0);
    stack->setCurrentIndex(2);
}
