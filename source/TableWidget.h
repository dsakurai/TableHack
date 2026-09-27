#pragma once

#include "MinimalTableWidget.h"

class TableWidget : public MinimalTableWidget {
    Q_OBJECT
public:
    explicit TableWidget(QWidget *parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent *event) override;
};
