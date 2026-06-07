#pragma once
#include <QWidget>
#include <QTimer>
#include <QPixmap>
#include <QKeyEvent>
#include <QPainter>
#include "Juego.h"

class NivelWidget : public QWidget {
    Q_OBJECT

public:
    explicit NivelWidget(Juego* juego, QWidget* parent = nullptr);
    ~NivelWidget();

    void iniciar();
    void detener();

signals:
    void nivelTerminado(int ganador);

protected:
    void paintEvent(QPaintEvent* event)    override;
    void keyPressEvent(QKeyEvent* event)   override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void tick();

private:
    Juego*  juego;
    QTimer* timerLoop;

    // Sprites Totodile nivel 1
    QPixmap totIdle;        // quieto
    QPixmap totSwim[3];     // nadar: [0]=horizontal [1]=arriba [2]=abajo
    QPixmap totAtk[2];      // mordedor
    QPixmap totHit;         // golpe recibido
    // Sprites Totodile nivel 2
    QPixmap totTop[3];      // cenital 3 frames (tot_top_0/1/2)
    // Sprites rival y obstáculos
    QPixmap psySwim[3];
    QPixmap mag[4];
    QPixmap magJump;   // subiendo (normal)
    QPixmap magHit;    // golpeado por mordedor (subiendo o bajando)
    QPixmap magFall;   // bajando sin ser mordido (llegó al tope)
    QPixmap remolino[4];    // 4 frames animados de remolino
    // Tiles nivel 2 (río cenital)
    QPixmap rioAgua[4];     // 4 frames de agua animada del río
    QPixmap rioBorderL;     // borde lateral izquierdo del río
    QPixmap rioBorderR;     // borde lateral derecho del río

    // Tiles de piscina nivel 1
    QPixmap poolAgua[3];        // 3 frames de agua animada (generada por código)
    QPixmap poolBorderTop;      // franja de borde superior (imagen completa, se repite en X)
    QPixmap poolBorderBottom;   // franja de borde inferior (imagen completa, se repite en X)
    QPixmap poolBorderTopL;     // borde superior izquierdo (legado, no usado)
    QPixmap poolBorderTopM;     // (legado, no usado)
    QPixmap poolBorderTopR;     // (legado, no usado)
    QPixmap poolBorderL;        // (legado, no usado)
    QPixmap poolBorderR;        // (legado, no usado)
    QPixmap poolCornerBL;       // (legado, no usado)
    QPixmap poolCornerBR;       // (legado, no usado)
    QPixmap poolLadder;         // (legado, no usado)
    QPixmap poolLaneRope;       // cuerda separadora de carriles (legado)

    int   frameTotodile;    // 0-1 para swim/atk, 0-3 para top
    int   framePsyduck;
    int   frameMagikarp;
    float timerAnimacion;
    float scrollX;
    float timerHit;
    bool  mostrandoHit;

    // Estado de teclas
    bool keyA, keyD, keyW, keyS, keyE;
    bool ultimaTeclaFueA;
    bool nadando;           // true mientras A o D están presionadas

    // Posición vertical del jugador en nivel 1 (en coords de pantalla, dentro del carril)
    float jugadorCarrilY;   // 0=tope carril, 1=fondo carril
    float jugadorVY;        // velocidad vertical normalizada

    void cargarSprites();
    void dibujarNivel1(QPainter& p);
    void dibujarNivel2(QPainter& p);
    void dibujarHUD(QPainter& p);
    void dibujarAgua(QPainter& p, int x, int y, int w, int h, QColor base, float offset);
    void dibujarSprite(QPainter& p, const QPixmap& pm, int cx, int cy, int tamano);
};
