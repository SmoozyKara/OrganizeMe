#pragma once

#include <QAbstractListModel>

#include "task_list.h"

// Модель для экрана "все мои списки" - показывает метаданные каждого
// TaskList (не его элементы). За элементами внутри конкретного списка
// - отдельная TaskListModel, по одной на список.
class OrganizerModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        ListIdRole = Qt::UserRole + 1,
        NameRole,
        ColorRole,
        RankRole,
    };

    explicit OrganizerModel(Organizer& organizer, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void createList(const QString& name);
    Q_INVOKABLE void deleteList(int listId);
    Q_INVOKABLE void changeColor(int listId, const QString& color);

private:
    Organizer& organizer_;
};