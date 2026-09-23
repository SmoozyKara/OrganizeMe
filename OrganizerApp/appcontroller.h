#pragma once

#include <QObject>
#include <QString>
#include <map>

#include "task_list.h"
#include "tasklistmodel.h"
#include "organizermodel.h"
#include "summarymodel.h"

// Единая точка, которую main.cpp создаёт и передаёт в QML.
// Владеет всеми данными приложения (Organizer) и всеми QML-моделями
// поверх этих данных.
class AppController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QObject* organizerModel READ organizerModel CONSTANT)
    Q_PROPERTY(QObject* summaryModel READ summaryModel CONSTANT)

public:
    explicit AppController(QObject* parent = nullptr);

    QObject* organizerModel() const;
    QObject* summaryModel() const;

    // Возвращает модель конкретного списка по его id. Если модель для
    // этого id уже создавалась раньше - переиспользует её, а не
    // создаёт заново (чтобы не плодить несколько разных TaskListModel
    // для одного и того же списка).
    Q_INVOKABLE QObject* getTaskListModel(int listId);

public slots:
    // Сохраняет текущее состояние organizer_ в файл. Вызывается при
    // закрытии приложения (см. main.cpp) - для MVP этого достаточно;
    // сохранение после каждого изменения (на случай краша) можно
    // добавить позже.
    void save();

private:
    Organizer organizer_;
    OrganizerModel* organizer_model_;
    SummaryModel* summary_model_;
    QString storage_file_path_;

    // Кэш уже созданных моделей отдельных списков: list id -> модель.
    std::map<int, TaskListModel*> task_list_models_;
};