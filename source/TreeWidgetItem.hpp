#pragma once

#include <QTreeWidgetItem>

class TreeWidgetItem : public QTreeWidgetItem {
public:
    explicit TreeWidgetItem(QTreeWidgetItem* parent = nullptr)
        : QTreeWidgetItem(parent)
    {
        // Set default values for the columns
        this->setText(0, "Default Name");
        this->setText(1, "Default Value");
    }
};