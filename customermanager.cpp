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
    connect(ui->MM_changeBalanceButton, &QPushButton::clicked, this, &customerManager::MM_changeBalanceButtonClicked);
    connect(ui->MM_editCustomerButton, &QPushButton::clicked, this, &customerManager::MM_editCustomerButtonClicked);






    // Setting the tab orders
    setTabOrder(ui->AC_nameInput, ui->AC_phoneInput);
    setTabOrder(ui->AC_phoneInput, ui->AC_balanceInput);
    setTabOrder(ui->AC_balanceInput, ui->AC_saveCustomerButton);


    // Switching to Main Menu Frame
    switchFrame(ui->MM_frame);

}

// Helper Functions: ========================================================================================================================
bool customerManager::loadCustomerFromDisplay(){
    // Checking that there is only one file selected
    if(ui->MM_customerDisplay->selectedItems().count() != 1){
        QMessageBox::critical(this,"ERROR","Please choose a customer");
        return false;
    }

    // Attempting to open the file and throwing an error if it does nto open
    string customerName = ui->MM_customerDisplay->currentItem()->text().toStdString();
    string customerFullPath = filePath + "/" + customerName + ".txt";
    ifstream customerFile(customerFullPath);

    if(!customerFile){
        QMessageBox::critical(this,"ERROR","Could not open file");
        return false;
    }


    // File was opened, load everything from the file name
    if(!current_customer.loadFromFile(customerName)){
        string errorMessage = "Could not open the customer: " + customerName;
        QMessageBox::critical(this,"ERROR", QString::fromStdString(errorMessage));
        return false;
    }

    return true;
}





// Frame Control: ========================================================================================================================
void customerManager::switchFrame(QFrame* targetFrame){
    // Hiding all frames
    ui->MM_frame->hide();
    ui->AC_frame->hide();
    ui->C_frame->hide();
    ui->CB_frame->hide();


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

    else if(targetFrame == ui->CB_frame){

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
        for (const auto& entry : fs::directory_iterator(customerPath.toStdString())) {   // Looking through all of the files in the path
            if (entry.is_regular_file()) {                                               // If the file is a text file
                string fileName = entry.path().filename().string();                      // Grab the full name of the file
                fileName = fileName.substr(0,fileName.length()-4);                       // Chop off the last 4 characters being ".txt"
                ui->MM_customerDisplay->addItem(QString::fromStdString(fileName));       // Adding item to the display
            }
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "ERORR", "Could not open a file");                   // Catching when the file could not be opened
    }
}

void customerManager::MM_changeBalanceButtonClicked(){
    // Loading customer from display, if it does not work, let it show error and return
    if(!loadCustomerFromDisplay()){
        return;
    }

    // If customer was loaded, change to the CB_frame
    switchFrame(ui->CB_frame);



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
    ui->AC_nameInput->setStyleSheet(validName ? normalStyle_textInput : errorStyle_textInput);
    ui->AC_phoneInput->setStyleSheet(validPhone ? normalStyle_textInput : errorStyle_textInput);


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



















