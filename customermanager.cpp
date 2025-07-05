#include "customermanager.h"
#include "./ui_customermanager.h"

customerManager::customerManager(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::customerManager)
{
    ui->setupUi(this);
}

customerManager::~customerManager()
{
    delete ui;
}
