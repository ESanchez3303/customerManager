#include "customermanager.h"
#include "./ui_customermanager.h"

// Destructor
customerManager::~customerManager(){delete ui;}


// Constructor
customerManager::customerManager(QWidget *parent): QMainWindow(parent), ui(new Ui::customerManager){
    ui->setupUi(this);
    connect(ui->MM_addCustomer, &QPushButton::clicked, this, &customerManager::MM_addCustomerClicked);




}


void customerManager::MM_addCustomerClicked(){

}

