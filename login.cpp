#include "login.h"
#include "ui_login.h"
#include "mainwindow.h"
#include <QMessageBox>

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    // Aplicar hoja de estilo para cambiar el color de fondo del login
    this->setStyleSheet("background-color: #ADD8E6;");  // Cambia el color por el que prefieras

    // Hacer que el campo de la contraseña oculte los caracteres
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
}

Login::~Login() {
    delete ui;
}

void Login::on_loginButton_clicked() {
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if (username == "admin" && password == "password") {
        accept();
    } else {
        QMessageBox::warning(this, "Inicio de Sesión Fallido", "Usuario o contraseña incorrectos.");
    }
}
