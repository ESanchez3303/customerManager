#include "customermanager.h"
#include "./ui_customermanager.h"
#include <iostream>
#include <QMessageBox>
#include <string>
#include <filesystem>
#include <QDate>
#include <QLayoutItem>
#include <QVBoxLayout>


using namespace std;

// Destructor: ==============================================================================================================================
customerManager::~customerManager(){delete ui;}


// Constructor: =============================================================================================================================
customerManager::customerManager(QWidget *parent): QMainWindow(parent), ui(new Ui::customerManager){
    ui->setupUi(this);

    // Setting up the file path for the customers
    current_customer.setFilePath(filePath);

    // Connecting the buttons to functions
    connect(ui->MM_addCustomerButton, &QPushButton::clicked, this, &customerManager::MM_addCustomerClicked);
    connect(ui->AC_backButton, &QPushButton::clicked, this, &customerManager::AC_backButtonClicked);
    connect(ui->AC_saveCustomerButton, &QPushButton::clicked, this, &customerManager::AC_savedCustomerButtonClicked);
    connect(ui->MM_statsButton, &QPushButton::clicked, this, &customerManager::MM_statsButtonClicked);
    connect(ui->S_backButton, &QPushButton::clicked, this, &customerManager::S_backButtonClicked);
    connect(ui->MM_searchButton, &QPushButton::clicked, this, &customerManager::MM_searchButtonClicked);
    connect(ui->MM_openCustomerButton, &QPushButton::clicked, this, &customerManager::MM_openCustomerButtonClicked);
    connect(ui->OC_backButton, &QPushButton::clicked, this, &customerManager::OC_backButtonClicked);
    connect(ui->OC_increaseBalanceButton, &QPushButton::clicked, this, &customerManager::OC_increaseButtonClicked);
    connect(ui->OC_decreaseBalanceButton, &QPushButton::clicked, this, &customerManager::OC_descreaseButtonClicked);
    connect(ui->OC_editButton, &QPushButton::clicked, this, &customerManager::OC_editButtonClicked);
    connect(ui->OC_edittingCloseButton, &QPushButton::clicked, this, &customerManager::OC_edittingCancelButtonClicked);
    connect(ui->OC_edittingCancelButton, &QPushButton::clicked, this, &customerManager::OC_edittingCancelButtonClicked);
    connect(ui->OC_edittingSaveButton, &QPushButton::clicked, this, &customerManager::OC_edittingSaveButtonClicked);
    connect(ui->OC_pastTransactionsButton, &QPushButton::clicked, this, &customerManager::OC_pastTransactionsButtonClicked);
    connect(ui->MM_english, &QRadioButton::toggled, this, &customerManager::enlishLanguageToggled);


    // Seting up the csutomer graph
    customerGraph = new BalanceGraph();
    QVBoxLayout* layout = new QVBoxLayout(ui->OC_graphFrame);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(customerGraph);

    // Setting up the statistics graph
    statsGraph = new BalanceGraph();
    layout = new QVBoxLayout(ui->S_owedGraph);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(statsGraph);

    // Resetting layout pointer
    layout = nullptr;


    // Setting the tab orders
    setTabOrder(ui->AC_nameInput, ui->AC_phoneInput);
    setTabOrder(ui->AC_phoneInput, ui->AC_balanceInput);
    setTabOrder(ui->AC_balanceInput, ui->AC_saveCustomerButton);
    setTabOrder(ui->AC_addressInput, ui->AC_saveCustomerButton);
    setTabOrder(ui->OC_edittingNameInput, ui->OC_edittingPhoneInput);
    setTabOrder(ui->OC_edittingPhoneInput, ui->OC_edittingAddressInput);
    setTabOrder(ui->OC_edittingAddressInput, ui->OC_edittingSaveButton);
    setTabOrder(ui->OC_edittingSaveButton, ui->OC_edittingCancelButton);


    // Setting application settings
    QApplication::setStyle("Fusion");                               // Setting the style be portable
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);  // Removing the sizing gadget
    ui->MM_customerDisplay->setStyleSheet(R"(
        QListWidget {
            color: black;
            background: rgb(209, 209, 209);
        }
        QListWidget::item:selected {
            color:white;
            background:rgb(138, 195, 171)
        }
        )");

    // Changing language depending on the toggle of the english trans. button
    enlishLanguageToggled();

    // Switching to Main Menu Frame
    switchFrame(ui->MM_frame);

    // Loading all stats to also save the current balance history
    setUpAllStats();

    // Making backup on the customers and the balance history
    backUpData();

}


// Helper Functions: ========================================================================================================================
bool customerManager::loadCustomerFromDisplay(){
    // Checking that there is only one file selected
    if(ui->MM_customerDisplay->selectedItems().count() != 1){
        QMessageBox::critical(this, errorTitle , noCustomerChosenMessage);
        return false;
    }

    // Attempting to open the file and throwing an error if it does nto open
    string customerName = ui->MM_customerDisplay->currentItem()->text().toStdString();
    string customerFullPath = filePath + "/" + customerName + ".txt";
    ifstream customerFile(customerFullPath);

    if(!customerFile){
        QMessageBox::critical(this,errorTitle ,"Could not open file");
        return false;
    }


    // Clearing the customer
    current_customer.clear();

    // File was opened, load everything from the file name
    if(!current_customer.loadFromFile(customerName)){
        string errorMessage = "Could not open the customer: " + customerName;
        QMessageBox::critical(this,"ERROR", QString::fromStdString(errorMessage));
        return false;
    }

    return true;
}

void customerManager::backUpData() {
    namespace fs = filesystem;

    QString backUpPath = QString::fromStdString(backUpPath_str);
    QString currentDate = QDate::currentDate().addDays(-1).toString("yyyy-MM-dd");
    QString datedFolder = backUpPath + "/" + currentDate;


    // If backup for today already exists, do nothing since we want yesterdays backup
    if (fs::exists(datedFolder.toStdString()))
        return;


    try {
        // Create the dated backup folder
        fs::create_directories(datedFolder.toStdString());

        // Backup the "customers" folder
        QString sourceCustomerFolder = QString::fromStdString(filePath);
        QString destCustomerFolder = datedFolder + "/customers";
        fs::create_directories(destCustomerFolder.toStdString());

        for (const auto& entry : fs::recursive_directory_iterator(sourceCustomerFolder.toStdString())) {
            const auto& path = entry.path();
            auto relativePath = fs::relative(path, sourceCustomerFolder.toStdString());
            fs::copy(path, fs::path(destCustomerFolder.toStdString()) / relativePath,
                     fs::copy_options::recursive | fs::copy_options::overwrite_existing);
        }

        // Backup the totalBalanceHistory.txt file
        QString sourceFile = "totalBalanceHistory.txt";
        QString destFile = datedFolder + "/totalBalanceHistory.txt";

        if (fs::exists(sourceFile.toStdString())) {
            fs::copy_file(sourceFile.toStdString(), destFile.toStdString(),
                          fs::copy_options::overwrite_existing);
        } else {
            QMessageBox::critical(this,"ERROR", "Error in creating backup. Location: 01");
        }



    } catch (const exception& e) {
        QMessageBox::critical(this, "ERORR", "Error in creating backup. Location: 02.");
        return;
    }
}




// Frame Control: ==========================================================================================================================
void customerManager::switchFrame(QFrame* targetFrame){
    // Hiding all frames
    ui->MM_frame->hide();
    ui->AC_frame->hide();
    ui->S_frame->hide();
    ui->OC_frame->hide();


    // Resetting the frame that we are going into
    if(targetFrame == ui->MM_frame){
        ui->MM_searchBar->clear();
        populateCustomerDisplay("");
    }

    else if(targetFrame == ui->AC_frame){
        current_customer.clear();
        // Resetting the values
        ui->AC_nameInput->setText("");
        ui->AC_phoneInput->setText("");
        ui->AC_balanceInput->setValue(0);
        ui->AC_addressInput->setText("");
        // Resetting the colors
        ui->AC_nameInput->setStyleSheet("color:black;background:white;");
        ui->AC_phoneInput->setStyleSheet("color:black;background:white;");
    }

    else if(targetFrame == ui->S_frame){
        setUpAllStats();
    }

    else if(targetFrame == ui->OC_frame){
        // Setting text displays
        ui->OC_name->setText(QString::fromStdString(current_customer.name));
        ui->OC_balance->setText(QString::number(current_customer.balance,'f',2));
        ui->OC_phone->setText(QString::fromStdString(current_customer.phoneNumber));
        ui->OC_address->setText(QString::fromStdString(current_customer.address));

        // Moving and hiding extra windows inside
        ui->OC_edittingFrame->move(130,150);
        ui->OC_edittingFrame->hide();
        ui->OC_transactionsDisplay->move(39,162);
        ui->OC_transactionsDisplay->hide();

        // Setting transaction button
        ui->OC_pastTransactionsButton->setText(showTransactionText);
        ui->OC_pastTransactionsButton->setStyleSheet("color:white; background: rgb(99, 140, 122)");
        transactionsShowing = false;

        // Setting up the graph
        customerGraph->setTransactions(current_customer.transactions);

        // Refreshing transaction window
        refreshTransactionDisplay();

    }
    // Show the frame
    targetFrame->show();
}







// Main Menu Functions: ====================================================================================================================
void customerManager::MM_addCustomerClicked()  { switchFrame(ui->AC_frame); }

void customerManager::MM_statsButtonClicked(){ switchFrame(ui->S_frame) ; }

void customerManager::MM_searchButtonClicked(){
    populateCustomerDisplay(ui->MM_searchBar->text());
}

void customerManager::populateCustomerDisplay(const QString& filter) {
    // Clear the display
    ui->MM_customerDisplay->clear();

    namespace fs = filesystem;
    QString customerPath = QString::fromStdString(filePath);

    try {
        for (const auto& entry : fs::directory_iterator(customerPath.toStdString())) {
            if (entry.is_regular_file()) {
                string fullName = entry.path().filename().string();  // e.g., "JohnDoe.txt"
                string nameOnly = fullName.substr(0, fullName.length() - 4); // Remove ".txt"

                QString qName = QString::fromStdString(nameOnly);

                // If filter is empty or qName contains the filter, add it
                if (filter.isEmpty() || qName.contains(filter, Qt::CaseInsensitive)) {
                    ui->MM_customerDisplay->addItem(qName);
                }
            }
        }
    } catch (const exception& e) {
        QMessageBox::critical(this, "ERROR", "Could not open the folder to load customers.");
    }
}

void customerManager::MM_openCustomerButtonClicked(){
    if(!loadCustomerFromDisplay()){
        return;
    }

    switchFrame(ui->OC_frame);

}

void customerManager::enlishLanguageToggled(){
    // Changing lanauge to English
    if(ui->MM_english->isChecked()){
        // Main Menu:
        ui->MM_label->setText("Customer Search");
        ui->MM_searchBar->setPlaceholderText("Enter Name To Search");
        ui->MM_searchButton->setText("Search");
        ui->MM_openCustomerButton->setText("Open Customer");
        errorTitle = "ERORR";
        noCustomerChosenMessage = "Please choose a customer";


        // Add Customer:
        ui->AC_label->setText("Add Customer");
        ui->AC_label_5->setText("Name:");
        ui->AC_nameInput->setPlaceholderText("Enter Name");
        ui->AC_label_3->setText("Phone Number:");
        ui->AC_phoneInput->setPlaceholderText("Enter Phone Number");
        ui->AC_label_4->setText("Balance:");
        ui->AC_saveCustomerButton->setText("Save Customer");
        ui->AC_label_6->setText("Address:");
        ui->AC_addressInput->setPlaceholderText("Enter Address");
        nameIsAlreadyUsedMessage = "Name is already used, please change.";
        sucessTitle = "Successfully Added";
        successAddedMessage = "Customer has been added";

        // Open Customer:
        ui->OC_editButton->setText("Edit");
        ui->OC_label_3->setText("Balance:");
        ui->OC_label_5->setText("Edit Customer");
        ui->OC_label_6->setText("Name:");
        ui->OC_label_7->setText("Phone:");
        ui->OC_edittingSaveButton->setText("Save");
        ui->OC_edittingCancelButton->setText("Cancel");
        ui->OC_label_8->setText("Addres:");
        hideTransactionText = "X";
        showTransactionText = "Past Transactions";
        enterBalanceMessage = "Please input an amount for the transaction.";
        negativeBalanceTitle = "Negative Balance Warning";
        negativeBalanceMessage = "This transaction will make the balance negative.";
        confirmTransactionTitle = "Confirm Transaction";
        confirmTransactionMessage = "Please confirm this action";

        // Statistics:
        ui->S_title->setText("Statistics");
        ui->S_owedLabel->setText("Total Owed:");
        ui->S_gainedLabel->setText("Total Gained:");
        ui->S_owedHistoryLabel->setText("Total Owed History");
        ui->S_mostOwedLabel->setText("Most Debeted Customers");

    }
    // Changing langauge to spanish
    else{
        // Main Menu:
        ui->MM_label->setText("Búsqueda de Cliente");
        ui->MM_searchBar->setPlaceholderText("Ingrese el Nombre para Buscar");
        ui->MM_searchButton->setText("Buscar");
        ui->MM_openCustomerButton->setText("Abrir Cliente");
        errorTitle = "Fallo";
        noCustomerChosenMessage = "Por favor seleccione un cliente";

        // Add Customer:
        ui->AC_label->setText("Agregar Cliente");
        ui->AC_label_5->setText("Nombre:");
        ui->AC_nameInput->setPlaceholderText("Ingrese el Nombre");
        ui->AC_label_3->setText("Teléfono:");
        ui->AC_phoneInput->setPlaceholderText("Ingrese el Número de Teléfono");
        ui->AC_label_4->setText("Saldo:");
        ui->AC_saveCustomerButton->setText("Guardar Cliente");
        ui->AC_label_6->setText("Ubicacion:");
        ui->AC_addressInput->setPlaceholderText("Ingrese la ubicacion");
        nameIsAlreadyUsedMessage = "El nombre ya está en uso, por favor cámbielo.";
        sucessTitle = "Agregado Exitosamente";
        successAddedMessage = "El cliente ha sido agregado";

        // Open Customer:
        ui->OC_editButton->setText("Editar");
        ui->OC_label_3->setText("Saldo:");
        ui->OC_label_5->setText("Editar Cliente");
        ui->OC_label_6->setText("Nombre:");
        ui->OC_label_7->setText("Teléfono:");
        ui->OC_edittingSaveButton->setText("Guardar");
        ui->OC_edittingCancelButton->setText("Cancelar");
        ui->OC_label_8->setText("Ubicacion:");
        hideTransactionText = "X";
        showTransactionText = "Transacciones Pasadas";
        enterBalanceMessage = "Por favor ingrese una cantidad para la transacción.";
        negativeBalanceTitle = "Advertencia de Saldo Negativo";
        negativeBalanceMessage = "Esta transacción hará que el saldo sea negativo.";
        confirmTransactionTitle = "Confirmar Transacción";
        confirmTransactionMessage = "Por favor confirme esta acción";

        // Statistics:
        ui->S_title->setText("Estadísticas");
        ui->S_owedLabel->setText("Total Adeudado:");
        ui->S_gainedLabel->setText("Total Ganado:");
        ui->S_owedHistoryLabel->setText("Historial de Deudas Totales:");
        ui->S_mostOwedLabel->setText("Clientes Más Endeudados:");
    }
}



// Adding Customer Functions ===============================================================================================================
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
        QMessageBox::critical(this, errorTitle, nameIsAlreadyUsedMessage);
    }



    // Setting the colors if invalid and resetting if IS valid
    ui->AC_nameInput->setStyleSheet(validName ? normalStyle_textInput : errorStyle_textInput);
    ui->AC_phoneInput->setStyleSheet(validPhone ? normalStyle_textInput : errorStyle_textInput);


    // Blocking if this is bad input
    if(!validName || !validPhone)
        return;



    // [ INFORMATION IS VALID FROM HERE FORTH ]

    // Setting up the customer data and adding the new balance transaction
    current_customer.setData(ui->AC_nameInput->text().toStdString(),ui->AC_phoneInput->text().toStdString(), ui->AC_balanceInput->value(), ui->AC_addressInput->text().toStdString());

    // Adding first transaction
    QString date_str = QDate::currentDate().toString("MM/dd/yyyy");
    string newTransaction = date_str.toStdString() + "|+|" + QString::number(ui->AC_balanceInput->value(),'f',2).toStdString() + "|New customer balance";
    current_customer.transactions.push_back(newTransaction);

    // Saving the customer
    current_customer.saveFile();

    // Sending sucess message
    QMessageBox::information(this, sucessTitle, successAddedMessage);

    // Clearing the inputs
    switchFrame(ui->AC_frame);

    // Updating the stats
    setUpAllStats();
}



// Open Customer Functions: ================================================================================================================
void customerManager::OC_backButtonClicked(){switchFrame(ui->MM_frame);}

void customerManager::refreshTransactionDisplay() {
    // Clear the contents and reset rows
    ui->OC_transactionsDisplay->clearContents();
    ui->OC_transactionsDisplay->setRowCount(0);

    // Helper to create a styled table item
    auto makeItem = [](const string& value, bool bold = false, int fontSize = -1, QColor color = Qt::black) {
        QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(value));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);  // Make uneditable
        item->setTextAlignment(Qt::AlignCenter);              // Centered
        item->setForeground(color);                           // Set text color

        if (bold || fontSize > 0) {
            QFont font = item->font();
            if (bold) font.setBold(true);
            if (fontSize > 0) font.setPointSize(fontSize);
            item->setFont(font);
        }

        return item;
    };

    for (const string& transactionStr : current_customer.transactions) {
        stringstream ss(transactionStr);
        string date, type, amount, comment;

        getline(ss, date, '|');
        getline(ss, type, '|');
        getline(ss, amount, '|');
        getline(ss, comment, '|'); // May be empty

        int currentRow = ui->OC_transactionsDisplay->rowCount();
        ui->OC_transactionsDisplay->insertRow(currentRow);

        // Decide color: green for +, red for -
        QColor color = (type == "+") ? QColor(0, 160, 0) : QColor(200, 0, 0);

        // Set cells
        ui->OC_transactionsDisplay->setItem(currentRow, 0, makeItem(type, true, 14, color));
        ui->OC_transactionsDisplay->setItem(currentRow, 1, makeItem(amount, false, 14, color));
        ui->OC_transactionsDisplay->setItem(currentRow, 2, makeItem(comment.empty() ? "(No comment)" : comment));
        ui->OC_transactionsDisplay->setVerticalHeaderItem(currentRow, makeItem(date));
    }

    // Lock layout and adjust column widths
    ui->OC_transactionsDisplay->horizontalHeader()->setSectionsMovable(false);
    ui->OC_transactionsDisplay->verticalHeader()->setSectionsMovable(false);
    ui->OC_transactionsDisplay->setColumnWidth(0, 40);  // Narrow for +/−
    ui->OC_transactionsDisplay->setColumnWidth(1, 100);
    ui->OC_transactionsDisplay->setColumnWidth(2, 702);
}

void customerManager::OC_descreaseButtonClicked(){ OC_changeBalance("|-|"); }

void customerManager::OC_increaseButtonClicked(){ OC_changeBalance("|+|"); }

void customerManager::OC_changeBalance(QString type){
    // Catching when user did not put in an amount
    if(ui->OC_amountInput->value() <= 0){
        QMessageBox::critical(this, errorTitle, enterBalanceMessage);
        return;
    }

    if(type == "|-|"){
        if((current_customer.balance - ui->OC_amountInput->value()) <= 0){
            QMessageBox::warning(this, negativeBalanceTitle, negativeBalanceMessage);
            return;
        }
    }

    // Confirming with user if this is what they wish to do
    QString sendingMessage = QString::fromStdString(type.toStdString().substr(1,1)) + QString::number(ui->OC_amountInput->value(),'f',2);
    sendingMessage = confirmTransactionMessage + ":\n" + sendingMessage;
    QMessageBox::StandardButton reply = QMessageBox::question(this, confirmTransactionTitle, sendingMessage,
                                                     QMessageBox::Ok | QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel) {
        // Clearing the inputs and returning if they regret it
        ui->OC_amountInput->setValue(0);
        ui->OC_commentInput->clear();
        return;
    }


    // Creating new transaction string
    QString date_str = QDate::currentDate().toString("MM/dd/yyyy");
    QString comment = ui->OC_commentInput->toPlainText();
    double amount = ui->OC_amountInput->value();
    QString new_transaction = date_str + type +
                            QString::number(amount,'f',2) + "|" +
                            comment;

    // Increasing/Decreasing the total balance of customer
    if(type == "|+|")
        current_customer.balance += amount;
    else
        current_customer.balance -= amount;


    // Putting string at the beginning of the transaction list
    current_customer.transactions.insert(current_customer.transactions.begin(), new_transaction.toStdString());

    // Saving customer file in case user closes at any moment
    current_customer.saveFile();

    // Refreshing the transaction display to show new information
    refreshTransactionDisplay();

    // Clearing the inputs
    ui->OC_amountInput->setValue(0);
    ui->OC_commentInput->clear();

    // Setting the new balance
    ui->OC_balance->setText(QString::number(current_customer.balance,'f',2));

    // Refreshing the graph
    customerGraph->setTransactions(current_customer.transactions);

    // Updating the stats page
    setUpAllStats();
}

void customerManager::OC_editButtonClicked(){
    ui->OC_edittingFrame->raise();
    ui->OC_edittingFrame->show();
    ui->OC_edittingNameInput->setText(QString::fromStdString(current_customer.name));
    ui->OC_edittingPhoneInput->setText(QString::fromStdString(current_customer.phoneNumber));
    ui->OC_edittingAddressInput->setText(QString::fromStdString(current_customer.address));
}

void customerManager::OC_edittingCancelButtonClicked(){
    ui->OC_edittingFrame->hide();
}

void customerManager::OC_edittingSaveButtonClicked(){
    // Checking if Name or Phone has changed
    bool changedName = false;
    bool changedPhone = false;
    bool changedAddress = false;

    if(ui->OC_edittingNameInput->text() != QString::fromStdString(current_customer.name))
        changedName = true;
    if(ui->OC_edittingPhoneInput->text() != QString::fromStdString(current_customer.phoneNumber))
        changedPhone = true;
    if(ui->OC_edittingAddressInput->text() != QString::fromStdString(current_customer.address))
        changedAddress = true;

    // If changedName is still false AND changedPhone are still false -> just close the window
    if((changedName == false) && (changedPhone == false) && (changedAddress == false)){
        ui->OC_edittingFrame->hide();
        return;
    }




    bool validName = true, validPhone = true;
    if(changedName){
        static QRegularExpression invalidChars(R"([\\\/:*?"<>|])");
        validName = (!ui->OC_edittingNameInput->text().isEmpty() && !ui->OC_edittingNameInput->text().contains(invalidChars));

        // If we can open the file, then make the name invalid
        string fullPath = filePath + "/" + ui->OC_edittingNameInput->text().toStdString() + ".txt";
        ifstream testingNameFile(fullPath);
        if(testingNameFile){
            validName = false;
            testingNameFile.close();
            QMessageBox::critical(this, errorTitle, nameIsAlreadyUsedMessage);
            return;
        }
    }



    if(changedPhone){
        static QRegularExpression phoneRegex(R"(^\+?\d{7,15}$)");
        validPhone = (phoneRegex.match(ui->OC_edittingPhoneInput->text()).hasMatch());
    }


    // Setting the colors if invalid and resetting if IS valid
    ui->OC_edittingNameInput->setStyleSheet(validName ? normalStyle_textInput : errorStyle_textInput);
    ui->OC_edittingPhoneInput->setStyleSheet(validPhone ? normalStyle_textInput : errorStyle_textInput);


    // Not continuing if the name or phone number inputs are invalid
    if(!validName || !validPhone){
        return;
    }



    // [ Saving the information since everything is valid from here forth ]

    // If name changed, remove the previous file using the current name before changing
    if(changedName){
        string oldFilePath = filePath + "/" + current_customer.name + ".txt";
        try {
            filesystem::remove(oldFilePath);
        } catch (const filesystem::filesystem_error& e) {
            QMessageBox::critical(this, errorTitle, "Failed to remove old file: " + QString::fromStdString(e.what()));
        }
    }

    // Setting the object with new information and the OLD balance since we aren't changing that
    string newName = ui->OC_edittingNameInput->text().toStdString();
    string newPhone = ui->OC_edittingPhoneInput->text().toStdString();
    string newAddress = ui->OC_edittingAddressInput->text().toStdString();
    current_customer.setData(newName,newPhone,current_customer.balance,newAddress);


    // Saving the file
    current_customer.saveFile();

    // Closing the window
    ui->OC_edittingFrame->hide();

    // Resetting up the information in the top section
    ui->OC_name->setText(QString::fromStdString(current_customer.name));
    ui->OC_phone->setText(QString::fromStdString(current_customer.phoneNumber));
    ui->OC_address->setText(QString::fromStdString(current_customer.address));
}

void customerManager::OC_pastTransactionsButtonClicked(){
    // Catching if the edit customer window is open
    if(ui->OC_edittingFrame->isVisible())
        return;

    transactionsShowing = !transactionsShowing;
    // Show the transactions window
    if(transactionsShowing){
        ui->OC_pastTransactionsButton->setStyleSheet("color:white; background: rgb(250, 0, 0)");
        ui->OC_pastTransactionsButton->setText(hideTransactionText);
        ui->OC_transactionsDisplay->raise();
        ui->OC_transactionsDisplay->show();
    }
    // Hide the transactions window
    else{
        ui->OC_pastTransactionsButton->setText(showTransactionText);
        ui->OC_pastTransactionsButton->setStyleSheet("color:white; background: rgb(99, 140, 122)");
        ui->OC_transactionsDisplay->hide();
    }
}



// Statistics Functions: ===================================================================================================================


void customerManager::S_backButtonClicked(){
    switchFrame(ui->MM_frame);
}

void customerManager::setUpAllStats() {

    double runningTotalOwed = 0;
    double runningTotalGained = 0;

    vector<pair<string, double>> customerBalances;

    namespace fs = filesystem;
    for (const auto& entry : fs::directory_iterator(filePath)) {
        if (entry.is_regular_file()) {
            string fullName = entry.path().filename().string();
            fullName = fullName.substr(0, fullName.length() - 4);  // remove ".txt"

            ifstream file(entry.path());
            string line;

            // Skip the phone number
            getline(file, line);

            // Read the second line for the current balance
            if (getline(file, line)) {
                try {
                    double balance = stod(line);
                    runningTotalOwed += balance;

                    // Store name and balance
                    customerBalances.push_back({ fullName, balance });

                } catch (const invalid_argument&) {
                    QString errorMessage = "One of the files is corrupted. Please check the file named: " + QString::fromStdString(entry.path().filename().string());
                    QMessageBox::critical(this, "ERROR", errorMessage);
                    switchFrame(ui->MM_frame);
                    return;
                }
            }

            // Skip address line
            getline(file, line);
            double lastValue;

            while (getline(file, line)) {
                if (line.empty()) continue;
                line = line.substr(line.find("|") + 1);
                if (line[1] == '-') continue;  // skip negatives
                line = line.substr(2);
                line = line.substr(0, line.find("|"));

                try {
                    double balance = stod(line);
                    runningTotalGained += balance;
                    lastValue = balance;
                    if (balance == 380)
                        cout << entry.path().filename().string() << endl;
                } catch (const invalid_argument&) {
                    QString errorMessage = "One of the files is corrupted. Please check the file named: " + QString::fromStdString(entry.path().filename().string());
                    QMessageBox::critical(this, "ERROR", errorMessage);
                    switchFrame(ui->MM_frame);
                    return;
                }
            }

            runningTotalGained -= lastValue;
            file.close();
        }
    }

    // Sorting the customer balances
    sort(customerBalances.begin(), customerBalances.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    // Display totals
    ui->S_totalOwed->setText(QString::number(runningTotalOwed, 'f', 2));
    ui->S_totalGained->setText(QString::number(runningTotalGained, 'f', 2));


    // Displaying the most owed list
    ui->S_owedMostList->clear();
    int maxDollarWidth = 0;
    if (!customerBalances.empty() && customerBalances.front().second > 0) {
        QString largestAmount = QString("$%1").arg(customerBalances.front().second, 0, 'f', 2);
        maxDollarWidth = largestAmount.length();
    }

    for (const auto& [name, balance] : customerBalances) {
        if (balance > 0) {
            QString amountStr = QString("$%1").arg(balance, maxDollarWidth, 'f', 2);  // right-align
            QString entry = QString("%1: %2").arg(amountStr).arg(QString::fromStdString(name));
            ui->S_owedMostList->addItem(entry);
        }
    }

    // Update the balance history file and the graph
    updateBalanceHistory(runningTotalOwed);
}



void customerManager::updateBalanceHistory(double curr_totalBalance){

    // Checking if the totalOwedFile exists, if not then create it
    ifstream balanceHistoryFile(totalBalanceHistory_file);
    if(!balanceHistoryFile){                          // If file has not been created yet
        ofstream temp_file(totalBalanceHistory_file); // Create file and write current total balance as first entry
        temp_file << curr_totalBalance << endl;
        temp_file.close();
        return;                                       // Do not update the graph since there is only one
    }


    // Grab all history from file and put into the vector
    vector<double> allBalances;
    string balance_string;
    double balance_double;
    while(getline(balanceHistoryFile, balance_string)){
        try {
            balance_double = stod(balance_string);
        } catch (const invalid_argument&) {
            QMessageBox::critical(this, "ERROR", "Balance file is corrupted. Please check the BALANCE FILE.");
            switchFrame(ui->MM_frame);
            balanceHistoryFile.close();
            return;
        }
        allBalances.push_back(balance_double);
    }
    balanceHistoryFile.close();

    // Adding to the end of the vector today's balance if its not the same as the last one in the vector
    if(allBalances[allBalances.size()-1] != curr_totalBalance){
        allBalances.push_back(curr_totalBalance);
        ofstream writting_file(totalBalanceHistory_file, ios::app);
        writting_file << curr_totalBalance << endl;
        writting_file.close();
    }


    // Using the object graph we have saved for showing the balances, update the graph
    statsGraph->setBalanceHistory(allBalances);

}




















