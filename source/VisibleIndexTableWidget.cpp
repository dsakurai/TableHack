#include "VisibleIndexTableWidget.h"

#include <QHBoxLayout>
#include <QTableWidget>

VisibleIndexTableWidget::VisibleIndexTableWidget(QWidget *parent)
    : QWidget(parent) {
        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        qTableWidget_ = new QTableWidget(this);
        layout->addWidget(qTableWidget_);
}

VisibleIndexTableWidget::~VisibleIndexTableWidget() = default;
