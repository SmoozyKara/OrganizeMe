#include <iostream>

#include "task_item.h"
#include "task_list.h"

// Отладочная функция вывода списка элементов в консоль.
// Не часть бизнес-логики - только для проверки в main().
void PrintList(const std::vector<TaskItem>& items) {
  for (const auto& item : items) {
    std::cout << "[priority=" << item.priority << "] " << item.text << '\n';
  }
}

int main() {
  TaskList work{GenerateNextListId(), 1, "Работа", "#FF5733", {}};
  AddElementToList(work.items, {0, 1, "Утренний дейлик"});
  AddElementToList(work.items, {0, 1, "Собеседование с кандидатом"});
  AddElementToList(work.items, {0, 2, "Обед с коллегой"});

  TaskList home{GenerateNextListId(), 2, "Домашние дела", "#33A1FF", {}};
  AddElementToList(home.items, {0, 1, "Генеральная уборка"});
  AddElementToList(home.items, {0, 2, "Забрать заказ с ПВЗ"});

  std::cout << "Список 'Работа':\n";
  PrintList(work.items);

  DeleteElementFromList(work.items, work.items[2].id);
  std::cout << "\nПосле удаления из 'Работа':\n";
  PrintList(work.items);

  auto merged = MergeAllLists({work, home});
  std::cout << "\nСводный список:\n";
  PrintList(merged);

  return 0;
}