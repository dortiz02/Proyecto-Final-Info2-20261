#include "NivelWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QTransform>
#include <stdexcept>
#include <cmath>
#include <algorithm>

NivelWidget::NivelWidget(Juego* juego, QWidget* parent)
    : QWidget(parent), juego(juego),
      timerLoop(new QTimer(this)),
      frameTotodile(0), framePsyduck(0), frameMagikarp(0),
      timerAnimacion(0.0f), scrollX(0.0f),
      timerHit(0.0f), mostrandoHit(false),
      keyA(false), keyD(false), keyW(false), keyS(false), keyE(false),
      ultimaTeclaFueA(false), nadando(false),
      jugadorCarrilY(0.5f), jugadorVY(0.0f)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(800, 500);
    connect(timerLoop, &QTimer::timeout, this, &NivelWidget::tick);
    cargarSprites();
}

NivelWidget::~NivelWidget() {}

void NivelWidget::cargarSprites() {
    auto cargar = [](QPixmap& pm, const QString& path) {
        if (!pm.load(path))
            throw std::runtime_error("No se pudo cargar: " + path.toStdString());
    };
    auto cargarOpcional = [](QPixmap& pm, const QString& path) {
        pm.load(path); // no lanza excepcion si falta
    };

    cargar(totIdle,     ":/sprites/tot_idle.png");
    cargar(totSwim[0],  ":/sprites/tot_swim_0.png");
    cargar(totSwim[1],  ":/sprites/tot_swim_up.png");
    cargar(totSwim[2],  ":/sprites/tot_swim_down.png");
    cargar(totAtk[0],   ":/sprites/tot_atk_0.png");
    cargar(totAtk[1],   ":/sprites/tot_atk_1.png");
    cargar(totHit,      ":/sprites/tot_hit.png");
    cargar(totTop[0],   ":/sprites/tot_top_0.png");
    cargar(totTop[1],   ":/sprites/tot_top_1.png");
    cargar(totTop[2],   ":/sprites/tot_top_2.png");
    cargar(psySwim[0],  ":/sprites/psy_swim_0.png");
    cargar(psySwim[1],  ":/sprites/psy_swim_1.png");
    cargar(psySwim[2],  ":/sprites/psy_swim_2.png");
    cargar(mag[0],      ":/sprites/mag_0.png");
    cargar(mag[1],      ":/sprites/mag_1.png");
    cargar(mag[2],      ":/sprites/mag_2.png");
    cargar(mag[3],      ":/sprites/mag_3.png");
    cargar(magJump,     ":/sprites/mag_jump.png");
    cargar(magHit,      ":/sprites/mag_hit.png");
    cargar(magFall,     ":/sprites/mag_fall.png");
    cargar(remolino[0], ":/sprites/remolino_0.png");
    cargar(remolino[1], ":/sprites/remolino_1.png");
    cargar(remolino[2], ":/sprites/remolino_2.png");
    cargar(remolino[3], ":/sprites/remolino_3.png");
    cargar(rioAgua[0],  ":/sprites/rio_agua_0.png");
    cargar(rioAgua[1],  ":/sprites/rio_agua_1.png");
    cargar(rioAgua[2],  ":/sprites/rio_agua_2.png");
    cargar(rioAgua[3],  ":/sprites/rio_agua_3.png");
    cargar(rioBorderL,  ":/sprites/rio_border_left.png");
    cargar(rioBorderR,  ":/sprites/rio_border_right.png");

    // Agua animada (aún disponibles si se quieren usar en nivel 2, etc.)
    cargarOpcional(poolAgua[0], ":/sprites/pool_agua_1.png");
    cargarOpcional(poolAgua[1], ":/sprites/pool_agua_2.png");
    cargarOpcional(poolAgua[2], ":/sprites/pool_agua_3.png");

    // Bordes de piscina nuevos (franjas horizontales completas)
    cargar(poolBorderTop,    ":/sprites/pool_border_top.png");
    cargar(poolBorderBottom, ":/sprites/pool_border_bottom.png");

    // Sprites legados (opcionales, ya no se usan en el render principal)
    cargarOpcional(poolBorderTopL, ":/sprites/border_top_l.png");
    cargarOpcional(poolBorderTopM, ":/sprites/border_top_mid.png");
    cargarOpcional(poolBorderTopR, ":/sprites/border_top_r.png");
    cargarOpcional(poolBorderL,    ":/sprites/border_left.png");
    cargarOpcional(poolBorderR,    ":/sprites/border_right.png");
    cargarOpcional(poolCornerBL,   ":/sprites/border_corner_bl.png");
    cargarOpcional(poolCornerBR,   ":/sprites/border_corner_br.png");
    cargarOpcional(poolLadder,     ":/sprites/border_ladder.png");
    cargarOpcional(poolLaneRope,   ":/sprites/border_lane_rope.png");
}

void NivelWidget::dibujarSprite(QPainter& p, const QPixmap& pm, int cx, int cy, int tamano) {
    if (pm.isNull()) return;
    QPixmap scaled = pm.scaled(tamano, tamano, Qt::KeepAspectRatio, Qt::FastTransformation);
    p.drawPixmap(cx - scaled.width()/2, cy - scaled.height()/2, scaled);
}

void NivelWidget::iniciar() {
    frameTotodile = framePsyduck = frameMagikarp = 0;
    timerAnimacion = scrollX = timerHit = 0.0f;
    mostrandoHit = nadando = false;
    keyA = keyD = keyW = keyS = keyE = false;
    ultimaTeclaFueA = false;
    jugadorCarrilY = 0.5f;
    jugadorVY = 0.0f;
    timerLoop->start(16);
    setFocus();
}

void NivelWidget::detener() { timerLoop->stop(); }

void NivelWidget::tick() {
    const float dt = 0.016f;
    Nivel* nivel = juego->getNivel();
    if (!nivel) return;

    float velAntes = nivel->getJugador()->getVelocidad();
    nadando = keyA || keyD;

    if (juego->getNivelActualNum() == 1) {
        // Brazada horizontal
        if (keyA && !ultimaTeclaFueA) { nivel->procesarBrazada(); ultimaTeclaFueA = true; }
        if (keyD &&  ultimaTeclaFueA) { nivel->procesarBrazada(); ultimaTeclaFueA = false; }
        if (keyE) nivel->procesarMordedor();

        // Movimiento vertical del jugador en el carril (W/S)
        float accelY = 0.0f;
        if (keyW) accelY -= 3.5f;
        if (keyS) accelY += 3.5f;
        jugadorVY += accelY * dt;
        jugadorVY *= 0.85f; // fricción del agua
        jugadorCarrilY += jugadorVY * dt;
        jugadorCarrilY = std::clamp(jugadorCarrilY, 0.05f, 0.95f);

        scrollX = nivel->getJugador()->getProgreso() * 0.4f;
        nivel->setJugadorCarrilY(jugadorCarrilY);
    }

    if (juego->getNivelActualNum() == 2) {
        float vel = 130.0f, dx = 0, dy = 0;
        if (keyA) dx -= vel * dt;
        if (keyD) dx += vel * dt;
        if (keyW) dy -= vel * dt;
        if (keyS) dy += vel * dt;
        nivel->moverJugador(dx, dy);
        if (keyE) nivel->procesarMordedor();
    }

    juego->actualizar(dt);

    // Detectar golpe por Magikarp
    float velDespues = nivel->getJugador()->getVelocidad();
    if (velAntes > 20.0f && velDespues < velAntes * 0.5f && !nivel->getJugador()->esMordedorActivo()) {
        mostrandoHit = true;
        timerHit = 0.5f;
    }
    if (mostrandoHit) {
        timerHit -= dt;
        if (timerHit <= 0.0f) mostrandoHit = false;
    }

    // Animación
    timerAnimacion += dt;
    if (timerAnimacion >= 0.22f) {
        timerAnimacion = 0.0f;
        frameTotodile  = (frameTotodile + 1) % 4;
        framePsyduck   = (framePsyduck  + 1) % 3;
        frameMagikarp  = (frameMagikarp + 1) % 4;
    }

    update();
    if (nivel->estaTerminado()) emit nivelTerminado(nivel->getGanador());
}

void NivelWidget::dibujarAgua(QPainter& p, int x, int y, int w, int h,
                               QColor base, float offset) {
    p.fillRect(x, y, w, h, base);
    p.setPen(QPen(base.lighter(115), 1));
    for (int oy = y + 10; oy < y + h; oy += 16) {
        QPainterPath path;
        path.moveTo(x, oy);
        for (int ox = x; ox <= x + w; ox += 3) {
            float wave = 2.5f * std::sin((ox + offset) * 0.045f);
            path.lineTo(ox, oy + wave);
        }
        p.drawPath(path);
    }
}

void NivelWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform, false);
    if (juego->getNivelActualNum() == 1) dibujarNivel1(p);
    else                                  dibujarNivel2(p);
    dibujarHUD(p);
}

void NivelWidget::dibujarNivel1(QPainter& p) {
    int W = width(), H = height();

    // -----------------------------------------------------------------------
    // LAYOUT DE LA PISCINA
    // La pantalla se divide en DOS carriles (rival arriba, jugador abajo).
    // Cada carril tiene:
    //   - Franja de borde superior  (poolBorderTop,    altura = BORDER_H)
    //   - Agua generada por código  (zona central)
    //   - Franja de borde inferior  (poolBorderBottom, altura = BORDER_B)
    //
    // Los bordes son imágenes de ancho fijo que se repiten en X (tile horizontal).
    // NO se dibujan bordes laterales: dan sensación de avance infinito.
    // -----------------------------------------------------------------------

    const int BORDER_H = 26;   // alto borde superior (pool_border_top)
    const int BORDER_B = 24;   // alto borde inferior (pool_border_bottom)

    int mitad    = H / 2;
    int frameAgua = frameMagikarp % 3;  // cicla entre los 3 frames de pool_agua

    // ---- Helper: dibujar una franja de imagen repetida horizontalmente ----
    // La imagen se escala a (W, targetH) en una sola pasada (más eficiente).
    auto dibujarFranja = [&](const QPixmap& pm, int y, int targetH) {
        if (pm.isNull()) return;
        // Escalamos a la altura target manteniendo proporción de ancho/alto
        // original para no distorsionar el tile, luego lo repetimos.
        float escala  = (float)targetH / pm.height();
        int   tileW   = std::max(1, (int)(pm.width() * escala));
        QPixmap tile  = pm.scaled(tileW, targetH,
                                  Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        // Offset de scroll para que la imagen "se mueva" suavemente con el jugador
        int offset = (int)(scrollX * 0.25f) % tileW;
        for (int x = -offset; x < W; x += tileW)
            p.drawPixmap(x, y, tile);
    };

    // -----------------------------------------------------------------------
    // CARRIL RIVAL  (y: 0 → mitad-1)
    // -----------------------------------------------------------------------
    int rivalAguaTop    = BORDER_H;
    int rivalAguaBot    = mitad - BORDER_B;
    int rivalAguaH      = rivalAguaBot - rivalAguaTop;

    // Agua central carril rival: tile sprite animado repetido en X
    {
        const QPixmap& tileRef = poolAgua[frameAgua];
        p.fillRect(0, rivalAguaTop, W, rivalAguaH, QColor(29, 140, 211));
        if (!tileRef.isNull()) {
            float esc   = (float)rivalAguaH / tileRef.height();
            int   tileW = std::max(1, (int)(tileRef.width() * esc));
            // +1 al dibujar para solapar la costura entre tiles
            QPixmap tile = tileRef.scaled(tileW + 1, rivalAguaH,
                                          Qt::IgnoreAspectRatio, Qt::FastTransformation);
            int offset = (int)(scrollX * 0.4f) % tileW;
            for (int x = -offset; x < W; x += tileW)
                p.drawPixmap(x, rivalAguaTop, tile);
        }
    }

    // Borde superior carril rival
    dibujarFranja(poolBorderTop,    0,            BORDER_H);
    // Borde inferior carril rival (= divisor entre carriles)
    dibujarFranja(poolBorderBottom, mitad - BORDER_B, BORDER_B);

    // -----------------------------------------------------------------------
    // CARRIL JUGADOR  (y: mitad → H-1)
    // -----------------------------------------------------------------------
    int jugAguaTop  = mitad + BORDER_H;
    int jugAguaBot  = H    - BORDER_B;
    int jugAguaH    = jugAguaBot - jugAguaTop;

    // Agua central carril jugador: mismo tile, mismo tratamiento
    {
        const QPixmap& tileRef = poolAgua[frameAgua];
        p.fillRect(0, jugAguaTop, W, jugAguaH, QColor(25, 128, 200));
        if (!tileRef.isNull()) {
            float esc   = (float)jugAguaH / tileRef.height();
            int   tileW = std::max(1, (int)(tileRef.width() * esc));
            QPixmap tile = tileRef.scaled(tileW + 1, jugAguaH,
                                          Qt::IgnoreAspectRatio, Qt::FastTransformation);
            int offset = (int)(scrollX * 0.4f) % tileW;
            for (int x = -offset; x < W; x += tileW)
                p.drawPixmap(x, jugAguaTop, tile);
        }
    }

    // Borde superior carril jugador
    dibujarFranja(poolBorderTop,    mitad, BORDER_H);
    // Borde inferior carril jugador
    dibujarFranja(poolBorderBottom, H - BORDER_B, BORDER_B);

    // -----------------------------------------------------------------------
    // ETIQUETAS DE CARRIL (pequeñas, discretas)
    // -----------------------------------------------------------------------
    p.setPen(QColor(220, 240, 255, 170));
    p.setFont(QFont("Arial", 9, QFont::Bold));
    p.drawText(14, BORDER_H + 18, "PSYDUCK");
    p.drawText(14, mitad + BORDER_H + 18, "TOTODILE");

    // -----------------------------------------------------------------------
    // PERSONAJES Y OBSTÁCULOS
    // -----------------------------------------------------------------------
    Nivel* nivel = juego->getNivel();

    // Posición Y del jugador dentro del carril inferior
    int carrilTop    = jugAguaTop;
    int carrilBottom = jugAguaBot - 4;
    int carrilH      = std::max(1, carrilBottom - carrilTop);
    int totX = W / 3;
    int totY = carrilTop + (int)(jugadorCarrilY * carrilH);

    // RIVAL PSYDUCK (carril superior, centrado verticalmente)
    int psyX  = W * 2 / 3;
    int psyY  = rivalAguaTop + (rivalAguaBot - rivalAguaTop) / 2;
    float velRival = nivel->getRival()->getVelocidad();
    float oscPsy   = velRival > 5.0f ? std::sin(scrollX * 0.07f) * 4.0f : 0.0f;
    dibujarSprite(p, psySwim[framePsyduck % 3], psyX, psyY + (int)oscPsy, 96);

    // TOTODILE
    if (mostrandoHit) {
        p.fillRect(0, mitad, W, H - mitad, QColor(255, 0, 0, 50));
        dibujarSprite(p, totHit, totX, totY, 68);
    } else if (nivel->getJugador()->esMordedorActivo()) {
        dibujarSprite(p, totAtk[frameTotodile % 2], totX, totY, 68);
    } else if (keyW && nadando) {
        dibujarSprite(p, totSwim[1], totX, totY, 68);
    } else if (keyS && nadando) {
        dibujarSprite(p, totSwim[2], totX, totY, 68);
    } else if (nadando) {
        dibujarSprite(p, totSwim[0], totX, totY, 68);
    } else {
        dibujarSprite(p, totIdle, totX, totY, 68);
    }

    // MAGIKARPS
    for (Obstaculo* o : nivel->getObstaculos()) {
        if (o->getTipo() != 0) continue;

        // Mostrar mientras esté activo O mientras tenga timer de muerte corriendo
        bool visible = o->estaActivo() || !o->listoPaLimpiar();
        if (!visible) continue;

        float mundoX = o->getX();
        float mundoY = o->getY();
        int   screenX = totX + (int)((mundoX - nivel->getJugador()->getProgreso()) * 0.8f);
        if (screenX < -80 || screenX > W + 80) continue;

        float t_norm  = mundoY / 550.0f;
        int   screenY = carrilTop + (int)(t_norm * carrilH);

        // Selección de sprite:
        // - mordido:                  magHit  (prioridad máxima)
        // - bajando sin ser mordido:  magFall
        // - subiendo normal:          magJump
        const QPixmap* sprite = &magJump;
        if (o->fueMordido())     sprite = &magHit;
        else if (o->esBajando()) sprite = &magFall;

        dibujarSprite(p, *sprite, screenX, screenY, 52);
    }

    // Indicadores de habilidades
    if (nivel->getJugador()->esMordedorActivo()) {
        p.setPen(QColor(255, 220, 50));
        p.setFont(QFont("Arial", 11, QFont::Bold));
        p.drawText(totX - 40, totY - 44, "MORDEDOR!");
    }
    if (nivel->getJugador()->esHiperactivoActivo()) {
        p.setPen(QColor(100, 230, 255));
        p.setFont(QFont("Arial", 10, QFont::Bold));
        p.drawText(totX - 45, totY - 60, "HIPERACTIVO!");
    }
}

void NivelWidget::dibujarNivel2(QPainter& p) {
    int W = width(), H = height();
    Nivel* nivel = juego->getNivel();
    float t = nivel->getTiempoTranscurrido();

    // -----------------------------------------------------------------------
    // BORDES LATERALES DEL RÍO
    // rioBorderL y rioBorderR son tiras verticales que se repiten en Y
    // para dar efecto de orilla infinita. El ancho se fija proporcional
    // a la altura escalada del sprite.
    // -----------------------------------------------------------------------
    int frameAgua = frameMagikarp % 4;  // mismo timer que remolinos/magikarps

    // Ancho de cada borde: escalar el sprite al alto de pantalla manteniendo proporción
    int borderLW = rioBorderL.isNull() ? 40
                 : (int)(rioBorderL.width() * ((float)H / rioBorderL.height()));
    int borderRW = rioBorderR.isNull() ? 40
                 : (int)(rioBorderR.width() * ((float)H / rioBorderR.height()));

    // Zona de agua: entre los dos bordes
    int aguaX = borderLW;
    int aguaW = W - borderLW - borderRW;

    // -----------------------------------------------------------------------
    // FONDO DE AGUA ANIMADA (tiles repetidos en X e Y con scroll vertical)
    // El río fluye hacia arriba: el jugador avanza hacia la meta subiendo en Y.
    // El scroll de los tiles va hacia abajo para dar sensación de avance.
    // -----------------------------------------------------------------------
    if (!rioAgua[frameAgua].isNull()) {
        const QPixmap& tileRef = rioAgua[frameAgua];
        // Escalar tile para que llene el ancho del río manteniendo proporción
        float escala = (float)aguaW / tileRef.width();
        int tileW    = aguaW;
        int tileH    = std::max(1, (int)(tileRef.height() * escala));
        QPixmap tile = tileRef.scaled(tileW, tileH,
                                      Qt::IgnoreAspectRatio, Qt::FastTransformation);
        // Scroll vertical descendente: simula avance del jugador río arriba
        int scrollY = (int)(t * 60.0f) % tileH;
        for (int y = -tileH + scrollY; y < H; y += tileH)
            p.drawPixmap(aguaX, y, tile);
    } else {
        // Fallback color si no hay sprite
        p.fillRect(aguaX, 0, aguaW, H, QColor(42, 105, 175));
    }

    // -----------------------------------------------------------------------
    // BORDES LATERALES: repetir verticalmente con offset para scroll
    // -----------------------------------------------------------------------
    auto dibujarBordeV = [&](const QPixmap& pm, int x, int bw) {
        if (pm.isNull()) { p.fillRect(x, 0, bw, H, QColor(38, 88, 38)); return; }
        float esc  = (float)bw / pm.width();
        int   bh   = std::max(1, (int)(pm.height() * esc));
        QPixmap tile = pm.scaled(bw, bh, Qt::IgnoreAspectRatio, Qt::FastTransformation);
        int scrollY  = (int)(t * 60.0f) % bh;
        for (int y = -bh + scrollY; y < H; y += bh)
            p.drawPixmap(x, y, tile);
    };
    dibujarBordeV(rioBorderL, 0,       borderLW);
    dibujarBordeV(rioBorderR, W - borderRW, borderRW);

    // -----------------------------------------------------------------------
    // FLECHAS DE CORRIENTE (discretas, encima del agua)
    // -----------------------------------------------------------------------
    p.setPen(QPen(QColor(255, 255, 255, 55), 1));
    int flechaScroll = (int)(t * 50) % 56;
    for (int oy = -flechaScroll; oy < H; oy += 56) {
        int ox = aguaX + aguaW / 2;
        p.drawLine(ox, oy + 8,  ox,    oy + 36);
        p.drawLine(ox, oy + 8,  ox - 5, oy + 20);
        p.drawLine(ox, oy + 8,  ox + 5, oy + 20);
    }

    // -----------------------------------------------------------------------
    // REMOLINOS CON SPRITE ANIMADO
    // -----------------------------------------------------------------------
    for (Obstaculo* o : nivel->getObstaculos()) {
        if (o->getTipo() != 1) continue;
        int rx = (int)o->getX();
        int ry = H/2 + (int)(o->getY() - nivel->getJugadorMundoY());
        dibujarSprite(p, remolino[frameMagikarp % 4], rx, ry, 120);
    }

    // -----------------------------------------------------------------------
    // JUGADOR CENITAL
    // -----------------------------------------------------------------------
    int screenX = (int)nivel->getJugadorMundoX();
    dibujarSprite(p, totTop[frameTotodile % 3], screenX, H/2, 72);
}

void NivelWidget::dibujarHUD(QPainter& p) {
    Nivel* nivel = juego->getNivel();
    if (!nivel) return;
    int W = width();

    if (juego->getNivelActualNum() == 1) {
        float pJ = std::clamp(nivel->getJugador()->getProgreso() / 3000.0f, 0.0f, 1.0f);
        float pR = std::clamp(nivel->getRival()->getProgreso()   / 3000.0f, 0.0f, 1.0f);

        p.fillRect(40, 5, 345, 42, QColor(0, 0, 0, 130));
        p.fillRect(50, 10, 280, 11, QColor(25,25,25));
        p.fillRect(50, 10, (int)(280*pJ), 11, QColor(50, 215, 115));
        p.fillRect(50, 25, 280, 11, QColor(25,25,25));
        p.fillRect(50, 25, (int)(280*pR), 11, QColor(225, 175, 50));
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 9));
        p.drawText(338, 20, "Totodile");
        p.drawText(338, 35, "Psyduck");

        float stam = std::clamp(nivel->getJugador()->getStamina() / 100.0f, 0.0f, 1.0f);
        p.fillRect(W-168, 7, 144, 12, QColor(25,25,25));
        p.fillRect(W-168, 7, (int)(144*stam), 12,
                   stam > 0.3f ? QColor(255,210,40) : QColor(220,55,55));
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 9));
        p.drawText(W-172, 30, "Stamina");

        // Controles
        p.setPen(QColor(200,235,255,180));
        p.setFont(QFont("Arial", 8));
        p.drawText(8, height()-8, "A/D: bracear   W/S: subir/bajar   E: mordedor");
    }

    if (juego->getNivelActualNum() == 2) {
        p.fillRect(W/2-42, 4, 84, 38, QColor(0,0,0,140));
        int seg = (int)std::max(0.0f, nivel->getTimer());
        p.setPen(seg > 15 ? Qt::white : QColor(255,75,75));
        p.setFont(QFont("Arial", 20, QFont::Bold));
        p.drawText(W/2-32, 34, QString("%1s").arg(seg));

        float progY = std::clamp(1.0f - nivel->getJugadorMundoY() / 900.0f, 0.0f, 1.0f);
        p.fillRect(W-30, 7, 18, height()-14, QColor(25,25,25));
        int barH = (int)((height()-14) * progY);
        p.fillRect(W-30, height()-7-barH, 18, barH, QColor(50, 205, 105));
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 8));
        p.drawText(W-32, height()-9, "META");
    }
}

void NivelWidget::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_A: keyA = true;  break;
        case Qt::Key_D: keyD = true;  break;
        case Qt::Key_W: keyW = true;  break;
        case Qt::Key_S: keyS = true;  break;
        case Qt::Key_E: keyE = true;  break;
        default: QWidget::keyPressEvent(event);
    }
}

void NivelWidget::keyReleaseEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_A: keyA = false; break;
        case Qt::Key_D: keyD = false; break;
        case Qt::Key_W: keyW = false; break;
        case Qt::Key_S: keyS = false; break;
        case Qt::Key_E: keyE = false; break;
        default: QWidget::keyReleaseEvent(event);
    }
}
