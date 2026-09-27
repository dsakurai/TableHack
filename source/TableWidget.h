#pragma once

#include "VisibleIndexTableWidget.h"

class TableWidget : public VisibleIndexTableWidget {
    Q_OBJECT
public:
    explicit TableWidget(QWidget *parent = nullptr);
};