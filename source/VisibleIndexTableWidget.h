#pragma once

#include <QTableWidget>

class VisibleIndexTableWidget : public QTableWidget {
    Q_OBJECT
public:
    explicit VisibleIndexTableWidget(QWidget *parent = nullptr);
    ~VisibleIndexTableWidget() override;
};
