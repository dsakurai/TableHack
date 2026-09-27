#include "VisibleHeaderView.h"

#include <QHeaderView>

VisibleHeader::VisibleHeader(QHeaderView *source, QObject *parent)
    : QObject(parent),
      sourceHeader_(source) {
}

VisibleHeader::~VisibleHeader() = default;

int VisibleHeader::visualIndex(int logical) const {
    if (sourceHeader_)
        return sourceHeader_->visualIndex(logical);
    
    throw std::runtime_error("Source header is not available");
}
