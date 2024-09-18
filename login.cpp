#include "login.h"
#include "ui_login.h"
#include "mainwindow.h"
#include<QMessageBox>


Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
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
