#ifndef CUSTOMERMANAGER_H
#define CUSTOMERMANAGER_H

#include "customer.h"
#include <QMainWindow>
#include <QFrame>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
class customerManager;
}
QT_END_NAMESPACE

class customerManager : public QMainWindow
{
    Q_OBJECT

public:
    customerManager(QWidget *parent = nullptr);
    ~customerManager();
private:
    Ui::customerManager *ui;

// Customer object
    customer current_customer;
    string filePath = "customers";


// Constants for styles
    QString const errorStyle_textInput = "color:white;background:red;";
    QString const normalStyle_textInput = "color:black;background:white;";

// Helper Functions
    bool loadCustomerFromDisplay();

// Frame Control
    void switchFrame(QFrame* targetFrame);


// Main Menu Functions
    void MM_addCustomerClicked();
    void populateCustomerDisplay();
    void MM_changeBalanceButtonClicked();
    void MM_editCustomerButtonClicked();


// Adding Customer Functions
    void AC_backButtonClicked();
    void AC_markError(QLineEdit* target);
    void AC_savedCustomerButtonClicked();


// Change Balance Functions
    void CB_backButtonClicked();
    void refreshTransactionDisplay();


// Calculator Function
    void calculatorButtonClicked();
    void calculateButtonClicked();
    void C_backButtonClicked();
};
#endif // CUSTOMERMANAGER_H
