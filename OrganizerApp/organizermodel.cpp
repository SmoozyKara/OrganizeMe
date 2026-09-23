#include "organizermodel.h"

OrganizerModel::OrganizerModel(Organizer& organizer, QObject* parent)
    : QAbstractListModel(parent), organizer_(organizer) {}

int OrganizerModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(organizer_.lists.size());
}

QVariant OrganizerModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() ||
        index.row() >= static_cast<int>(organizer_.lists.size())) {
        return QVariant();
    }

    const TaskList& list = organizer_.lists[index.row()];
    switch (role) {
    case ListIdRole:
        return list.id;
    case NameRole:
        return QString::fromStdString(list.name);
    case ColorRole:
        return QString::fromStdString(list.color);
    case RankRole:
        return list.rank;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> OrganizerModel::roleNames() const {
    return {
            {ListIdRole, "listId"},
            {NameRole, "name"},
            {ColorRole, "color"},
            {RankRole, "rank"},
            };
}

void OrganizerModel::createList(const QString& name, const QString& color) {
    beginResetModel();
    CreateTaskList(organizer_, name.toStdString(), color.toStdString());
    endResetModel();
}

void OrganizerModel::deleteList(int listId) {
    beginResetModel();
    DeleteTaskList(organizer_, listId);
    endResetModel();
}