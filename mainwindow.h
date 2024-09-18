#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QDate>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


struct Reserva {
    QString nombre;
    QString contacto;
    int numComensales;
    QDate fecha;
    QTime hora;
    QString numeroReferencia;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void crearReserva();
    void consultarDisponibilidad();
    void modificarReserva();
    void cancelarReserva();

private:
    Ui::MainWindow *ui;
    QVector<Reserva> reservas;

    void guardarReservasEnArchivo();
    void cargarReservasDesdeArchivo();
    bool verificarConflictoReserva(const Reserva& nuevaReserva);
    void mostrarAlternativasReserva(const Reserva& nuevaReserva);
};

#endif // MAINWINDOW_H
