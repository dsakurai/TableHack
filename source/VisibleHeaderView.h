#pragma once

#include <QHeaderView>
#include <QPointer>

class VisibleHeader : public QObject {
    Q_OBJECT
public:
    explicit VisibleHeader(QHeaderView *source, QObject *parent = nullptr);
    ~VisibleHeader() override;

    int visualIndex(int logical) const;

private:
    QPointer<QHeaderView> sourceHeader_;
};
