#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QResizeEvent>
#include <QPixmap>
#include "Juego.h"
#include "NivelWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onJugar();
    void onNivelTerminado(int ganador);
    void onSiguienteNivel();
    void onVolverMenu();

private:
    Juego*          juego;
    NivelWidget*    nivelWidget;
    QStackedWidget* stack;

    QWidget*     pantallaMenu;
    QComboBox*   comboDificultad;

    QWidget*     pantallaResultado;
    QLabel*      labelResultado;
    QPushButton* btnSiguiente;
    QPushButton* btnMenu;

    QPixmap      menuBgPixmap;     // imagen original sin escalar

    void construirMenu();
    void construirPantallaResultado();
    void mostrarMenu();
    void mostrarNivel();
    void mostrarResultado(int ganador);
};
