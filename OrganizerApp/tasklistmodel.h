#pragma once

#include <QAbstractListModel>

#include "task_list.h"

// Модель одного списка задач для QML.
// Не хранит данные сама - "смотрит" в Organizer через id списка,
// каждый раз заново находя актуальный TaskList (см. findList()).
// Так модель не может остаться с "битым" указателем, если Organizer
// изменится где-то ещё (например, список удалят, вектор переедет
// в памяти при росте).
class TaskListModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        ItemIdRole = Qt::UserRole + 1,
        PriorityRole,
        TextRole,
        ListColorRole,
    };

    explicit TaskListModel(Organizer& organizer, int listId,
                           QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Методы, вызываемые из QML (например, по нажатию кнопки).
    Q_INVOKABLE void addItem(int priority, const QString& text);
    Q_INVOKABLE void removeItem(int itemId);
    Q_INVOKABLE void changePriority(int itemId, int newPriority);

private:
    // Находит актуальный TaskList в organizer_ по list_id_.
    // Возвращает nullptr, если список уже удалён (на этот случай тоже
    // нужно быть готовым реагировать - см. .cpp).
    TaskList* findList() const;

    Organizer& organizer_;
    int list_id_;
};