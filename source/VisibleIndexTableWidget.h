#pragma once

#include <QWidget>
#include <QPointer>

#include <QTableWidget>

/**
 * This class implements the use of visible indices in the table widget.
 * This is useful to avoid confusion when coding with indices in the table widget.
 */
class VisibleIndexTableWidget: public QWidget {
    Q_OBJECT
public:
    explicit VisibleIndexTableWidget(QWidget *parent = nullptr);
    ~VisibleIndexTableWidget() override;

    void setRowCount(int rowCount) {
        if (qTableWidget_) qTableWidget_->setRowCount(rowCount);
    }
    
    void setColumnCount(int columnCount) {
        if (qTableWidget_) qTableWidget_->setColumnCount(columnCount);
    }

private:
    QPointer<QTableWidget> qTableWidget_;
};
