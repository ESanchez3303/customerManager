#ifndef CUSTOMERMANAGER_H
#define CUSTOMERMANAGER_H

#include <QMainWindow>

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
};
#endif // CUSTOMERMANAGER_H
