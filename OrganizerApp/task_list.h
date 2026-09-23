#pragma once

#include <string>
#include <vector>

#include "task_item.h"

// Список со своими метаданными:
// id - уникальный id списка (не меняется никогда)
// rank - порядок списка среди других списков (может пересчитываться,
//        например, после удаления другого списка)
// name - имя списка, например "Работа"
// color - цвет списка, hex-код, например "#FF5733"
// items - элементы списка, всегда отсортированы по priority
struct TaskList {
    int id;
    int rank;
    std::string name;
    std::string color;
    std::vector<TaskItem> items;
};

// Хранилище всех списков приложения.
struct Organizer {
    std::vector<TaskList> lists;
};

// Возвращает очередной уникальный id для нового списка.
// Отдельный счётчик от GenerateNextId (тот - для элементов внутри списка).
int GenerateNextListId();

// Подтягивает счётчик id списков вверх, если min_id больше текущего
// значения. Используется при загрузке сохранённых данных.
void EnsureNextListIdAtLeast(int min_id);

// Создаёт новый список и добавляет его в конец organizer.lists.
// rank нового списка = rank последнего списка + 1 (либо 0, если
// списков пока нет вообще). id - через GenerateNextListId().
void CreateTaskList(Organizer& organizer, std::string name,
                    std::string color);

// Удаляет список по id и пересчитывает rank оставшихся списков,
// чтобы rank всегда шёл подряд без дырок. Если список с таким id
// не найден - ничего не делает, сообщает об этом в консоль.
void DeleteTaskList(Organizer& organizer, int id);

// Сливает все переданные списки в один, отсортированный по priority
// элементов, с тай-брейком по rank списка при равном priority.
std::vector<TaskItem> MergeAllLists(const std::vector<TaskList>& lists);