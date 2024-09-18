#include <QApplication>
#include "mainwindow.h"
#include "login.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    Login loginDialog;
    if (loginDialog.exec() == QDialog::Accepted) {
        MainWindow w;
        w.show();
        return a.exec();
    }

    return 0;
}
