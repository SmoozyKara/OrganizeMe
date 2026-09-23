#include "organizermodel.h"

#include <QColor>
#include <QRandomGenerator>

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

void OrganizerModel::createList(const QString& name) {
    // Случайный приятный цвет: фиксированные насыщенность/яркость (HSL),
    // случайный только тон (hue) - так цвета получаются разнообразными,
    // но не "грязными" и не слишком тёмными/светлыми.
    int hue = QRandomGenerator::global()->bounded(360);
    QColor color = QColor::fromHsl(hue, 180, 130);

    beginResetModel();
    CreateTaskList(organizer_, name.toStdString(), color.name().toStdString());
    endResetModel();
}

void OrganizerModel::deleteList(int listId) {
    beginResetModel();
    DeleteTaskList(organizer_, listId);
    endResetModel();
}

void OrganizerModel::changeColor(int listId, const QString& color) {
    beginResetModel();
    ChangeListColor(organizer_, listId, color.toStdString());
    endResetModel();
}