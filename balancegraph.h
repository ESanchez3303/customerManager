#ifndef BALANCEGRAPH_H
#define BALANCEGRAPH_H

#include <QWidget>
#include <vector>
#include <string>

class BalanceGraph : public QWidget {
    Q_OBJECT

public:
    explicit BalanceGraph(QWidget *parent = nullptr);
    void setTransactions(const std::vector<std::string>& transactions);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::vector<double> balanceHistory;
};

#endif // BALANCEGRAPH_H
