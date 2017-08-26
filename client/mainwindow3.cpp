#include "mainwindow3.h"
#include "ui_mainwindow3.h"

#include <QDebug>
#include <QFontDatabase>

MainWindow3::MainWindow3(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow3)
{
    ui->setupUi(this);
    qDebug() << "Look up fonts";
    int idRegular = QFontDatabase::addApplicationFont(":/fonts/OpenSans-Regular.ttf");
    int idSemiBold = QFontDatabase::addApplicationFont(":/fonts/OpenSans-SemiBold.ttf");

    qDebug() << "Create fonts";
    QFont regular(QFontDatabase::applicationFontFamilies(idRegular).at(0));
    QFont semiBold(QFontDatabase::applicationFontFamilies(idSemiBold).at(0));
    qDebug() << "Set fonts";
    ui->cryptoLabel->setFont(regular);
    ui->signatureLabel->setFont(semiBold);
    ui->myEidLabel->setFont(regular);
}

MainWindow3::~MainWindow3()
{
    delete ui;
}