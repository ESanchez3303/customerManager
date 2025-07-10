#include "customermanager.h"
#include "./ui_customermanager.h"
#include <iostream>
#include <QMessageBox>
using namespace std;

// Destructor
customerManager::~customerManager(){delete ui;}


// Constructor
customerManager::customerManager(QWidget *parent): QMainWindow(parent), ui(new Ui::customerManager){
    ui->setupUi(this);

    // Setting up the file path for the customers
    current_customer.setFilePath(filePath);

    // Connecting the buttons to functions
    connect(ui->MM_addCustomerButton, &QPushButton::clicked, this, &customerManager::MM_addCustomerClicked);
    connect(ui->AC_backButton, &QPushButton::clicked, this, &customerManager::AC_backButtonClicked);
    connect(ui->AC_saveCustomerButton, &QPushButton::clicked, this, &customerManager::AC_savedCustomerButtonClicked);


    // Switching to Main Menu Frame
    switchFrame(ui->MM_frame);

}


// Frame Control:
void customerManager::switchFrame(QFrame* targetFrame){
    // Hiding all frames
    ui->MM_frame->hide();
    ui->AC_frame->hide();
    ui->C_frame->hide();


    // Resetting the frame that we are going into
    if(targetFrame == ui->MM_frame){

    }
    else if(targetFrame == ui->AC_frame){
        // Resetting the values
        ui->AC_nameInput->setText("");
        ui->AC_phoneInput->setText("");
        ui->AC_balanceInput->setValue(0);
        // Resetting the colors
        ui->AC_nameInput->setStyleSheet("color:black;background:white;");
        ui->AC_nameInput->setStyleSheet("color:black;background:white;");
        ui->AC_nameInput->setStyleSheet("color:black;background:white;");
    }

    else if(targetFrame == ui->C_frame){

    }

    // Show the frame
    targetFrame->show();
}

// Main Menu Functions:
void customerManager::MM_addCustomerClicked(){ switchFrame(ui->AC_frame); }



// Adding Customer Functions
void customerManager::AC_backButtonClicked(){ switchFrame(ui->MM_frame); }

void customerManager::AC_markError(QLineEdit* target){
    // Reseting all inputs


    // Setting the error input signal
    target->setStyleSheet("color:white;background:red;");
}


void customerManager::AC_savedCustomerButtonClicked(){

    // Validating information (name, phone number, balance)
    static QRegularExpression invalidChars(R"([\\\/:*?"<>|])");
    static QRegularExpression phoneRegex(R"(^\+?\d{7,15}$)");
    bool validName = (!ui->AC_nameInput->text().isEmpty() && !ui->AC_nameInput->text().contains(invalidChars));
    bool validPhone = (phoneRegex.match(ui->AC_phoneInput->text()).hasMatch());


    // If we can open the file, then make the name invalid
    string fullPath = filePath + "/" + ui->AC_nameInput->text().toStdString() + ".txt";
    ifstream testingNameFile(fullPath);
    cout << fullPath << endl;
    if(testingNameFile){
        validName = false;
        testingNameFile.close();
        QMessageBox::critical(this, "Error", "Name is already used, please change.");
    }else



    // Setting the colors if invalid and resetting if IS valid
    ui->AC_nameInput->setStyleSheet(validName ? normalStyle : errorStyle);
    ui->AC_phoneInput->setStyleSheet(validPhone ? normalStyle : errorStyle);


    // Blocking if this is bad input
    if(!validName || !validPhone)
        return;



    // [ INFORMATION IS VALID FROM HERE FORTH ]

    cout << "Name: " << ui->AC_nameInput->text().toStdString() << endl;
    cout << "phone: " << ui->AC_phoneInput->text().toStdString() << endl;

    // Setting up the customer data
    current_customer.setData(ui->AC_nameInput->text().toStdString(),ui->AC_phoneInput->text().toStdString(), ui->AC_balanceInput->value());

    // Saving the customer
    current_customer.saveFile();

    // Sending sucess message
    QMessageBox::information(this,"Success","Added new customer!");

    // Clearing the inputs
    switchFrame(ui->AC_frame);
}
























