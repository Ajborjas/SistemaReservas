#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateEdit>
#include <QTimeEdit>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDebug>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "Directorio de trabajo actual: " << QDir::currentPath();

    connect(ui->btnCrearReserva, &QPushButton::clicked, this, &MainWindow::crearReserva);
    connect(ui->btnConsultarDisponibilidad, &QPushButton::clicked, this, &MainWindow::consultarDisponibilidad);
    connect(ui->btnModificarReserva, &QPushButton::clicked, this, &MainWindow::modificarReserva);
    connect(ui->btnCancelarReserva, &QPushButton::clicked, this, &MainWindow::cancelarReserva);

    cargarReservasDesdeArchivo();
}
MainWindow::~MainWindow() {
    guardarReservasEnArchivo();
    delete ui;
}

void MainWindow::crearReserva() {

    bool ok;
    QString nombre = QInputDialog::getText(this, "Crear Reserva", "Nombre del Cliente:", QLineEdit::Normal, "", &ok);
    if (!ok || nombre.isEmpty()) return;

    QString contacto = QInputDialog::getText(this, "Crear Reserva", "Número de Contacto:", QLineEdit::Normal, "", &ok);
    if (!ok || contacto.isEmpty()) return;

    int numComensales = QInputDialog::getInt(this, "Crear Reserva", "Número de Comensales:", 1, 1, 20, 1, &ok);
    if (!ok) return;

    QDialog dialog(this);
    dialog.setWindowTitle("Seleccionar Fecha y Hora");

    QVBoxLayout layout(&dialog);

    QDateEdit dateEdit(QDate::currentDate(), &dialog);
    dateEdit.setCalendarPopup(true);
    layout.addWidget(&dateEdit);

    QTimeEdit timeEdit(QTime::currentTime(), &dialog);
    layout.addWidget(&timeEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout.addWidget(&buttonBox);

    if (dialog.exec() != QDialog::Accepted) return;

    QDate fecha = dateEdit.date();
    QTime hora = timeEdit.time();

    QString numeroReferencia = "REF" + QString::number(reservas.size() + 1);

    Reserva nuevaReserva = {nombre, contacto, numComensales, fecha, hora, numeroReferencia};

    if (!verificarConflictoReserva(nuevaReserva)) {
        reservas.append(nuevaReserva);
        guardarReservasEnArchivo();
        QMessageBox::information(this, "Reserva Creada", "La reserva ha sido creada exitosamente.");
    } else {
        mostrarAlternativasReserva(nuevaReserva);
    }
}

void MainWindow::consultarDisponibilidad() {
    bool ok;


    QDialog dialog(this);
    dialog.setWindowTitle("Consultar Disponibilidad");

    QVBoxLayout layout(&dialog);

    QDateEdit dateEdit(QDate::currentDate(), &dialog);
    dateEdit.setCalendarPopup(true);
    layout.addWidget(&dateEdit);

    QTimeEdit timeEdit(QTime::currentTime(), &dialog);
    layout.addWidget(&timeEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout.addWidget(&buttonBox);

    if (dialog.exec() != QDialog::Accepted) return;

    QDate fecha = dateEdit.date();
    QTime hora = timeEdit.time();

    QString disponibilidad = "Mesas disponibles:\n";
    bool hayDisponibilidad = false;

    for (const Reserva& reserva : reservas) {
        if (reserva.fecha == fecha && reserva.hora == hora) {
            disponibilidad += "Mesa ocupada\n";
        } else {
            disponibilidad += "Mesa disponible\n";
            hayDisponibilidad = true;
        }
    }

    if (!hayDisponibilidad) {
        QMessageBox::information(this, "Disponibilidad", "No hay mesas disponibles para el horario seleccionado.");
    } else {
        QMessageBox::information(this, "Disponibilidad", disponibilidad);
    }
}

void MainWindow::modificarReserva() {
    bool ok;
    QString numeroReferencia = QInputDialog::getText(this, "Modificar Reserva", "Número de Referencia de la Reserva:", QLineEdit::Normal, "", &ok);
    if (!ok || numeroReferencia.isEmpty()) return;

    bool reservaEncontrada = false;

    for (Reserva& reserva : reservas) {
        if (reserva.numeroReferencia == numeroReferencia) {
            reserva.nombre = QInputDialog::getText(this, "Modificar Reserva", "Nombre del Cliente:", QLineEdit::Normal, reserva.nombre, &ok);
            if (!ok) return;

            reserva.contacto = QInputDialog::getText(this, "Modificar Reserva", "Número de Contacto:", QLineEdit::Normal, reserva.contacto, &ok);
            if (!ok) return;

            reserva.numComensales = QInputDialog::getInt(this, "Modificar Reserva", "Número de Comensales:", reserva.numComensales, 1, 20, 1, &ok);
            if (!ok) return;


            QDialog dialog(this);
            dialog.setWindowTitle("Modificar Fecha y Hora");

            QVBoxLayout layout(&dialog);

            QDateEdit dateEdit(reserva.fecha, &dialog);
            dateEdit.setCalendarPopup(true);
            layout.addWidget(&dateEdit);

            QTimeEdit timeEdit(reserva.hora, &dialog);
            layout.addWidget(&timeEdit);

            QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
            connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
            connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
            layout.addWidget(&buttonBox);

            if (dialog.exec() != QDialog::Accepted) return;

            reserva.fecha = dateEdit.date();
            reserva.hora = timeEdit.time();

            guardarReservasEnArchivo();
            reservaEncontrada = true;
            QMessageBox::information(this, "Reserva Modificada", "La reserva ha sido modificada exitosamente.");
            break;
        }
    }

    if (!reservaEncontrada) {
        QMessageBox::warning(this, "Modificar Reserva", "No se encontró ninguna reserva con ese número de referencia.");
    }
}

void MainWindow::cancelarReserva() {
    bool ok;
    QString numeroReferencia = QInputDialog::getText(this, "Cancelar Reserva", "Número de Referencia de la Reserva:", QLineEdit::Normal, "", &ok);
    if (!ok || numeroReferencia.isEmpty()) return;

    bool reservaEliminada = false;

    for (int i = 0; i < reservas.size(); ++i) {
        if (reservas[i].numeroReferencia == numeroReferencia) {
            reservas.removeAt(i);
            guardarReservasEnArchivo();
            reservaEliminada = true;
            QMessageBox::information(this, "Reserva Cancelada", "La reserva ha sido cancelada exitosamente.");
            break;
        }
    }

    if (!reservaEliminada) {
        QMessageBox::warning(this, "Cancelar Reserva", "No se encontró ninguna reserva con ese número de referencia.");
    }
}

void MainWindow::guardarReservasEnArchivo() {
    QFile file("reservas.txt");
    qDebug() << "Intentando abrir el archivo 'reservas.txt' para escritura...";
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const Reserva &r : reservas) {
            out << r.nombre << "," << r.contacto << "," << r.numComensales << ","
                << r.fecha.toString(Qt::ISODate) << "," << r.hora.toString(Qt::ISODate)
                << "," << r.numeroReferencia << "\n";
        }
        file.close();
        qDebug() << "Archivo 'reservas.txt' guardado correctamente.";
    } else {
        QMessageBox::warning(this, "Error", "No se pudo abrir el archivo para guardar las reservas.");
        qDebug() << "Error: No se pudo abrir el archivo 'reservas.txt' para escritura.";
    }
}


void MainWindow::cargarReservasDesdeArchivo() {
    QFile file("reservas.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        reservas.clear();
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(",");
            if (fields.size() == 6) {
                Reserva r;
                r.nombre = fields[0];
                r.contacto = fields[1];
                r.numComensales = fields[2].toInt();
                r.fecha = QDate::fromString(fields[3], Qt::ISODate);
                r.hora = QTime::fromString(fields[4], Qt::ISODate);
                r.numeroReferencia = fields[5];
                reservas.append(r);
            }
        }
        file.close();
    } else {
        QMessageBox::information(this, "Información", "No se encontró el archivo de reservas. Se creará uno nuevo al guardar.");
    }
}


bool MainWindow::verificarConflictoReserva(const Reserva& nuevaReserva) {
    for (const Reserva& reserva : reservas) {
        if (reserva.fecha == nuevaReserva.fecha && reserva.hora == nuevaReserva.hora) {
            return true;
        }
    }
    return false;
}

void MainWindow::mostrarAlternativasReserva(const Reserva& nuevaReserva) {
    QString alternativas = "No hay disponibilidad para la fecha y hora seleccionadas. Intente con estas alternativas:\n";
    QTime horaAlternativa = nuevaReserva.hora.addSecs(3600);

    for (int i = 0; i < 3; ++i) {
        if (!verificarConflictoReserva({nuevaReserva.nombre, nuevaReserva.contacto, nuevaReserva.numComensales, nuevaReserva.fecha, horaAlternativa, ""})) {
            alternativas += "Fecha: " + nuevaReserva.fecha.toString("dd/MM/yyyy") + " Hora: " + horaAlternativa.toString("HH:mm") + "\n";
        }
        horaAlternativa = horaAlternativa.addSecs(3600);
    }

    QMessageBox::information(this, "Alternativas de Reserva", alternativas);
}
