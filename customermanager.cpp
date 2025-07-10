#include "customermanager.h"
#include "./ui_customermanager.h"
#include <iostream>
#include <QMessageBox>
#include <string>
#include <filesystem>
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
    connect(ui->C_calculateButton, &QPushButton::clicked, this, &customerManager::calculateButtonClicked);
    connect(ui->MM_calculatorButton, &QPushButton::clicked, this, &customerManager::calculatorButtonClicked);
    connect(ui->C_backButton, &QPushButton::clicked, this, &customerManager::C_backButtonClicked);





    // Setting the tab orders
    setTabOrder(ui->AC_nameInput, ui->AC_phoneInput);
    setTabOrder(ui->AC_phoneInput, ui->AC_balanceInput);
    setTabOrder(ui->AC_balanceInput, ui->AC_saveCustomerButton);


    // Switching to Main Menu Frame
    switchFrame(ui->MM_frame);

}







// Frame Control: ========================================================================================================================
void customerManager::switchFrame(QFrame* targetFrame){
    // Hiding all frames
    ui->MM_frame->hide();
    ui->AC_frame->hide();
    ui->C_frame->hide();


    // Resetting the frame that we are going into
    if(targetFrame == ui->MM_frame){
        ui->MM_customerDisplay->clear();
        ui->MM_searchBar->clear();
        populateCustomerDisplay();
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
        ui->C_weightInput->setValue(0);
        ui->C_priceInput->setValue(0);
        calculateButtonClicked();
    }

    // Show the frame
    targetFrame->show();
}





// Main Menu Functions: ========================================================================================================================
void customerManager::MM_addCustomerClicked()  { switchFrame(ui->AC_frame); }
void customerManager::calculatorButtonClicked(){ switchFrame(ui->C_frame) ; }



void customerManager::populateCustomerDisplay(){
    // Loading in the customers into the customerDispaly by only the names, no other information is loaded
    namespace fs = std::filesystem;
    QString customerPath = QString::fromStdString(filePath);


    try {
        for (const auto& entry : fs::directory_iterator(customerPath.toStdString())) {
            if (entry.is_regular_file()) {
                QString fileName = QString::fromStdString(entry.path().filename().string());
                ui->MM_customerDisplay->addItem(fileName);
            }
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "ERORR", "Could not open a file");
    }
}



// Adding Customer Functions ============================================================================================================
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
    if(testingNameFile){
        validName = false;
        testingNameFile.close();
        QMessageBox::critical(this, "Error", "Name is already used, please change.");
    }



    // Setting the colors if invalid and resetting if IS valid
    ui->AC_nameInput->setStyleSheet(validName ? normalStyle : errorStyle);
    ui->AC_phoneInput->setStyleSheet(validPhone ? normalStyle : errorStyle);


    // Blocking if this is bad input
    if(!validName || !validPhone)
        return;



    // [ INFORMATION IS VALID FROM HERE FORTH ]

    // Setting up the customer data
    current_customer.setData(ui->AC_nameInput->text().toStdString(),ui->AC_phoneInput->text().toStdString(), ui->AC_balanceInput->value());

    // Saving the customer
    current_customer.saveFile();

    // Sending sucess message
    QMessageBox::information(this,"Success","Added new customer!");

    // Clearing the inputs
    switchFrame(ui->AC_frame);
}





// Calculator Functions: ============================================================================================================


void customerManager::C_backButtonClicked(){
    switchFrame(ui->MM_frame);
}



void customerManager::calculateButtonClicked(){
    // Multiplying and showing the results in the output box
    QString product = QString::number((ui->C_priceInput->value() * ui->C_weightInput->value()), 'f', 2);
    ui->C_output->setText(product);
}



















