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

// TODO: CreateTaskList, DeleteTaskList - ещё не реализованы.

// Сливает все переданные списки в один, отсортированный по priority
// элементов, с тай-брейком по rank списка при равном priority.
std::vector<TaskItem> MergeAllLists(const std::vector<TaskList>& lists);