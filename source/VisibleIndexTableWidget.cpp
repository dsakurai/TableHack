#include "VisibleIndexTableWidget.h"

#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>

VisibleIndexTableWidget::VisibleIndexTableWidget(QWidget *parent)
    : QWidget(parent) {
        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        qTableWidget_ = new QTableWidget(this);
        layout->addWidget(qTableWidget_);
        
        // Move rows and columns within the table.
        qTableWidget_->horizontalHeader()->setSectionsMovable(true);
        qTableWidget_->horizontalHeader()->setDragEnabled(true);
        //
        qTableWidget_->verticalHeader()->setSectionsMovable(true);
        qTableWidget_->verticalHeader()->setDragEnabled(true);
}

VisibleIndexTableWidget::~VisibleIndexTableWidget() = default;
