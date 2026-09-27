#pragma once

#include <QTableWidget>

/**
 * This class bans the use of logical indices in the table widget.
 * This is useful to avoid confusion when coding with indices in the table widget.
 */
class VisibleIndexTableWidget : public QTableWidget {
    Q_OBJECT
public:
    explicit VisibleIndexTableWidget(QWidget *parent = nullptr);
    ~VisibleIndexTableWidget() override;
};
