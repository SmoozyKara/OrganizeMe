#include "summarymodel.h"

SummaryModel::SummaryModel(Organizer& organizer, QObject* parent)
    : QAbstractListModel(parent), organizer_(organizer) {
    cached_items_ = MergeAllLists(organizer_.lists);
}

int SummaryModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(cached_items_.size());
}

QVariant SummaryModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() ||
        index.row() >= static_cast<int>(cached_items_.size())) {
        return QVariant();
    }

    const TaskItem& item = cached_items_[index.row()];
    switch (role) {
    case ItemIdRole:
        return item.id;
    case PriorityRole:
        return item.priority;
    case TextRole:
        return QString::fromStdString(item.text);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> SummaryModel::roleNames() const {
    return {
            {ItemIdRole, "itemId"},
            {PriorityRole, "priority"},
            {TextRole, "text"},
            };
}

void SummaryModel::refresh() {
    beginResetModel();
    cached_items_ = MergeAllLists(organizer_.lists);
    endResetModel();
}