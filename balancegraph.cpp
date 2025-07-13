#include "balancegraph.h"
#include <QPainter>
#include <QPen>
#include <QString>
#include <QPainterPath>
#include <sstream>

BalanceGraph::BalanceGraph(QWidget *parent) : QWidget(parent) {}

void BalanceGraph::setTransactions(const std::vector<std::string>& transactions) {
    balanceHistory.clear();
    double balance = 0;

    // Iterate in reverse to build history (oldest to newest)
    for (auto it = transactions.rbegin(); it != transactions.rend(); ++it) {
        std::stringstream ss(*it);
        std::string date, type, amountStr, comment;

        getline(ss, date, '|');
        getline(ss, type, '|');
        getline(ss, amountStr, '|');
        getline(ss, comment); // ignore for graph

        double amount = std::stod(amountStr);
        if (type == "+")
            balance += amount;
        else
            balance -= amount;

        balanceHistory.push_back(balance);
    }


    update(); // Trigger repaint
}


void BalanceGraph::setBalanceHistory(const std::vector<double>& balances){
    balanceHistory = balances;
    update();
}

void BalanceGraph::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // === Configurable variables at the top ===
    const QColor lineColor(0, 0, 0);         // Line color
    const QColor fillColor(99, 140, 122);    // Fill color under line (lighter greenish)

    const int leftMargin = 50;      // space for Y labels and ticks
    const int rightPadding = 20;    // right side padding
    const int topPadding = 30;      // top padding
    const int bottomPadding = 20;   // bottom padding for X-axis labels or line
    const int tickLength = 6;

    // === End of configurable variables ===

    if (balanceHistory.empty())
        return;

    // New: If there is only one point, do not show anything (skip drawing)
    if (balanceHistory.size() == 1) {
        return;
    }

    int usableWidth = width() - leftMargin - rightPadding;
    int usableHeight = height() - topPadding - bottomPadding;

    // Normal graph for 2+ points
    double minY = 0;

    // Fix here: ensure maxY includes the latest balance if it is greater than max in history
    double maxY_data = *std::max_element(balanceHistory.begin(), balanceHistory.end());
    double latestBalance = balanceHistory.front();
    double maxY = std::max(maxY_data, latestBalance);

    double yRange = maxY - minY;
    if (yRange == 0) yRange = 1;

    int pointCount = balanceHistory.size();
    double xStep = static_cast<double>(usableWidth) / (pointCount - 1);

    // Draw Y-axis line
    painter.setPen(QPen(Qt::black, 1));
    painter.drawLine(leftMargin, topPadding, leftMargin, topPadding + usableHeight);

    // Draw X-axis line
    int xAxisY = topPadding + usableHeight;
    painter.drawLine(leftMargin, xAxisY, leftMargin + usableWidth, xAxisY);

    // Draw vertical tick marks at every transaction point on X-axis
    painter.setPen(QPen(Qt::black, 1));
    const int xTickLength = 8;  // height of vertical tick marks
    for (int i = 0; i < pointCount; ++i) {
        int x = leftMargin + static_cast<int>(i * xStep);
        painter.drawLine(x, xAxisY, x, xAxisY + xTickLength);
    }

    // Draw Y-axis ticks and labels
    painter.setFont(QFont("Arial", 8));
    for (int i = 0; i <= 5; ++i) {
        double value = minY + i * (yRange / 5);
        int y = topPadding + usableHeight - static_cast<int>(((value - minY) / yRange) * usableHeight);

        // Tick
        painter.drawLine(leftMargin - tickLength, y, leftMargin, y);

        // Label (right aligned)
        QString label = QString::number(value, 'f', 0);
        QRect textRect(0, y - 10, leftMargin - tickLength - 2, 20);
        painter.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, label);
    }

    // Prepare path for filled area under the line
    QPainterPath fillPath;
    fillPath.moveTo(leftMargin, xAxisY); // Start at bottom left (on x-axis)

    // Trace the line points
    for (int i = 0; i < pointCount; ++i) {
        int x = leftMargin + static_cast<int>(i * xStep);
        int y = topPadding + usableHeight - static_cast<int>(((balanceHistory[i] - minY) / yRange) * usableHeight);
        fillPath.lineTo(x, y);
    }

    fillPath.lineTo(leftMargin + usableWidth, xAxisY); // bottom right corner
    fillPath.closeSubpath();

    // Fill with chosen fill color
    painter.fillPath(fillPath, fillColor);

    // Draw the balance progression line on top
    painter.setPen(QPen(lineColor, 2));
    for (int i = 1; i < pointCount; ++i) {
        double prevVal = balanceHistory[i - 1];
        double currVal = balanceHistory[i];

        int x1 = leftMargin + static_cast<int>((i - 1) * xStep);
        int x2 = leftMargin + static_cast<int>(i * xStep);

        int y1 = topPadding + usableHeight - static_cast<int>(((prevVal - minY) / yRange) * usableHeight);
        int y2 = topPadding + usableHeight - static_cast<int>(((currVal - minY) / yRange) * usableHeight);

        painter.drawLine(x1, y1, x2, y2);
    }
}
