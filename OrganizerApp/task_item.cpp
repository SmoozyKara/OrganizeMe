#include "task_item.h"

#include <algorithm>
#include <iostream>

namespace {
int g_next_id = 1;
}  // namespace

int GenerateNextId() {
    return g_next_id++;
}

// Вызывается при загрузке сохранённых данных: гарантирует, что
// следующий сгенерированный id будет больше любого уже загруженного,
// чтобы не столкнуться с уже занятым id.
void EnsureNextIdAtLeast(int min_id) {
    if (g_next_id < min_id) {
        g_next_id = min_id;
    }
}

void InsertSorted(std::vector<TaskItem>& items, TaskItem elem) {
    auto pos = std::upper_bound(items.begin(), items.end(), elem,
                                [](const TaskItem& a, const TaskItem& b) {
                                    return a.priority < b.priority;
                                });
    items.insert(pos, elem);
}

void AddElementToList(std::vector<TaskItem>& items, TaskItem elem) {
    elem.id = GenerateNextId();
    InsertSorted(items, elem);
}

void DeleteElementFromList(std::vector<TaskItem>& items, int id) {
    auto it = std::find_if(items.begin(), items.end(),
                           [id](const TaskItem& elem) { return elem.id == id; });

    if (it != items.end()) {
        items.erase(it);
    } else {
        std::cerr << "DeleteElementFromList: id " << id << " not found\n";
    }
}

void ChangeElementPriority(std::vector<TaskItem>& items, int id,
                           int new_priority) {
    auto it = std::find_if(items.begin(), items.end(),
                           [id](const TaskItem& elem) { return elem.id == id; });

    if (it == items.end()) {
        std::cerr << "ChangeElementPriority: id " << id << " not found\n";
        return;
    }

    // Если приоритет не поменялся - список уже отсортирован верно,
    // лишняя работа (удаление + вставка) не нужна.
    if (it->priority == new_priority) {
        return;
    }

    // Сначала копируем данные элемента - после erase(it) сам итератор
    // и всё, на что он указывал, становятся недействительными.
    TaskItem elem = *it;
    items.erase(it);
    elem.priority = new_priority;
    InsertSorted(items, elem);
}