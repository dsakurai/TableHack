#pragma once

#include <QTableView>

class QStandardItemModel;

class MyTableView : public QTableView {
    Q_OBJECT
public:
    explicit MyTableView(QWidget *parent = nullptr);
};
