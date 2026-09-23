#pragma once

#include <QAbstractListModel>
#include <vector>

#include "task_list.h"

// Модель сводного экрана - показывает результат MergeAllLists.
// В отличие от TaskListModel/OrganizerModel, не "смотрит" напрямую
// в Organizer при каждом обращении - MergeAllLists каждый раз заново
// вычисляет результат (это не бесплатно), поэтому модель хранит
// закэшированный результат и обновляет его только по явному вызову
// refresh() - не при любом изменении данных где-то ещё.
class SummaryModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        ItemIdRole = Qt::UserRole + 1,
        PriorityRole,
        TextRole,
        ListColorRole,
    };

    explicit SummaryModel(Organizer& organizer, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Пересчитывает сводный список заново из текущего состояния
    // organizer_. Вызывается из QML каждый раз при открытии сводного
    // экрана.
    Q_INVOKABLE void refresh();

private:
    void RebuildCache();

    Organizer& organizer_;
    std::vector<TaskItem> cached_items_;
    // cached_colors_[i] - цвет списка, из которого пришёл cached_items_[i].
    // Параллельный массив (тот же размер и порядок, что cached_items_) -
    // проще, чем заводить отдельную структуру-пару, раз оба массива
    // всегда пересобираются и используются вместе.
    std::vector<std::string> cached_colors_;
};