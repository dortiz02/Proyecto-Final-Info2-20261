#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPainter>
#include <QEvent>
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
    menuBgPixmap.load(":/sprites/menu_bg.png");

    // Widget que pinta el fondo escalado en su paintEvent
    // Usamos una lambda-clase local via QWidget + installEventFilter
    pantallaMenu = new QWidget();
    pantallaMenu->setMinimumSize(900, 600);

    // Instalar filtro de eventos para pintar el fondo
    // (evita subclase extra; el fondo se pinta en el evento Paint del pantallaMenu)
    struct BgPainter : public QObject {
        QPixmap& bg;
        BgPainter(QPixmap& p, QObject* parent) : QObject(parent), bg(p) {}
        bool eventFilter(QObject* obj, QEvent* ev) override {
            if (ev->type() == QEvent::Paint) {
                QWidget* w = static_cast<QWidget*>(obj);
                QPainter painter(w);
                painter.drawPixmap(w->rect(),
                    bg.scaled(w->size(), Qt::KeepAspectRatioByExpanding,
                               Qt::SmoothTransformation));
                // NO llamar a QObject::eventFilter para que el widget
                // siga pintando sus hijos encima
            }
            return false; // siempre dejar que el evento continúe
        }
    };
    pantallaMenu->installEventFilter(new BgPainter(menuBgPixmap, pantallaMenu));

    // ── Panel central semitransparente ──────────────────────────────────────
    QWidget* panel = new QWidget(pantallaMenu);
    panel->setAttribute(Qt::WA_TranslucentBackground, false);
    panel->setStyleSheet(
        "QWidget#menuPanel { background: rgba(0,20,60,175); border-radius:16px; }"
    );
    panel->setObjectName("menuPanel");
    panel->setFixedSize(360, 300);

    QVBoxLayout* lp = new QVBoxLayout(panel);
    lp->setAlignment(Qt::AlignCenter);
    lp->setSpacing(12);
    lp->setContentsMargins(28, 24, 28, 24);

    QLabel* lblDif = new QLabel("Dificultad:");
    lblDif->setAlignment(Qt::AlignCenter);
    lblDif->setStyleSheet("font-size:15px; font-weight:bold; color:#cce8ff; background:transparent;");

    comboDificultad = new QComboBox();
    comboDificultad->addItem("Fácil",   1);
    comboDificultad->addItem("Normal",  2);
    comboDificultad->addItem("Difícil", 3);
    comboDificultad->setFixedWidth(220);
    comboDificultad->setStyleSheet(
        "QComboBox { background:#0a3060; color:white; font-size:14px;"
        "            padding:5px; border:2px solid #4aa8ff; border-radius:6px; }"
        "QComboBox QAbstractItemView { background:#0a3060; color:white;"
        "            selection-background-color:#1a6abf; }"
    );

    QPushButton* btnJugar = new QPushButton("¡JUGAR!");
    btnJugar->setFixedSize(220, 52);
    btnJugar->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "              stop:0 #2090e0, stop:1 #0a5fa0);"
        "              color:white; font-size:20px; font-weight:bold;"
        "              border-radius:10px; border:2px solid #60bfff; }"
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "              stop:0 #40b0ff, stop:1 #1070c0); }"
        "QPushButton:pressed { background:#0a4080; }"
    );
    connect(btnJugar, &QPushButton::clicked, this, &MainWindow::onJugar);

    QLabel* controles = new QLabel(
        "Nivel 1:  A/D bracear · W/S carril · E mordedor\n"
        "Nivel 2:  W/A/S/D moverse · E mordedor"
    );
    controles->setAlignment(Qt::AlignCenter);
    controles->setStyleSheet("font-size:11px; color:#99ccff; background:transparent;");

    lp->addWidget(lblDif,          0, Qt::AlignCenter);
    lp->addWidget(comboDificultad, 0, Qt::AlignCenter);
    lp->addSpacing(8);
    lp->addWidget(btnJugar,        0, Qt::AlignCenter);
    lp->addSpacing(4);
    lp->addWidget(controles,       0, Qt::AlignCenter);

    // Posición inicial centrada (se reajusta en resizeEvent)
    panel->move((900 - panel->width()) / 2, (600 - panel->height()) / 2 + 40);
    panel->raise();
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    // Recentrar el panel de controles cuando cambia el tamaño
    if (pantallaMenu) {
        QWidget* panel = pantallaMenu->findChild<QWidget*>("menuPanel");
        if (panel) {
            int W = event->size().width();
            int H = event->size().height();
            panel->move((W - panel->width()) / 2,
                        (H - panel->height()) / 2 + 40);
        }
        pantallaMenu->update(); // forzar repaint del fondo
    }
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
