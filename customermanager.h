#ifndef CUSTOMERMANAGER_H
#define CUSTOMERMANAGER_H

#include "customer.h"
#include <QMainWindow>
#include "balancegraph.h"
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
    QString errorTitle;
    QString noCustomerChosenMessage;
    bool loadCustomerFromDisplay();

// Frame Control
    void switchFrame(QFrame* targetFrame);


// Main Menu Functions
    void MM_addCustomerClicked();
    void populateCustomerDisplay(const QString& filter);
    void MM_openCustomerButtonClicked();
    void MM_searchButtonClicked();
    void enlishLanguageToggled();


// Adding Customer Functions
    QString nameIsAlreadyUsedMessage;
    QString sucessTitle;
    QString successAddedMessage;
    void AC_backButtonClicked();
    void AC_markError(QLineEdit* target);
    void AC_savedCustomerButtonClicked();


// Open Customer Functions:
    BalanceGraph* graph = nullptr;
    bool transactionsShowing = false;
    QString hideTransactionText;
    QString showTransactionText;
    QString enterBalanceMessage;
    QString negativeBalanceTitle;
    QString negativeBalanceMessage;
    QString confirmTransactionTitle;
    QString confirmTransactionMessage;
    void refreshTransactionDisplay();
    void OC_backButtonClicked();
    void OC_changeBalance(QString type);
    void OC_increaseButtonClicked();
    void OC_descreaseButtonClicked();
    void OC_editButtonClicked();
    void OC_edittingCancelButtonClicked();
    void OC_edittingSaveButtonClicked();
    void OC_pastTransactionsButtonClicked();


// Calculator Function
    void calculatorButtonClicked();
    void calculateButtonClicked();
    void C_backButtonClicked();
};
#endif // CUSTOMERMANAGER_H
