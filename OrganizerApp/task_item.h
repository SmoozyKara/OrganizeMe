#pragma once

#include <string>
#include <vector>

// Элемент списка:
// id - уникальный id
// priority - приоритет (1 - наивысший, чем больше число - тем ниже)
// text - текст задачи
struct TaskItem {
  int id;
  int priority;
  std::string text;
};

// Возвращает очередной уникальный id для нового элемента.
// Счётчик общий на все списки, id никогда не переиспользуется.
int GenerateNextId();

// Подтягивает счётчик id вверх, если min_id больше текущего значения.
// Используется при загрузке сохранённых данных.
void EnsureNextIdAtLeast(int min_id);

// Вставляет уже готовый элемент (с уже выставленным id) в вектор,
// сохраняя сортировку по priority. Сам id не трогает и не генерирует -
// это общий "кирпичик" для AddElementToList и ChangeElementPriority,
// чтобы логика вставки не дублировалась в двух местах.
void InsertSorted(std::vector<TaskItem>& items, TaskItem elem);

// Добавляет новый элемент в список, сохраняя сортировку по priority.
// id элементу присваивается здесь - вызывающий код не должен сам
// придумывать id.
void AddElementToList(std::vector<TaskItem>& items, TaskItem elem);

// Удаляет элемент по id. Если id не найден - ничего не удаляет,
// сообщает об этом в консоль.
void DeleteElementFromList(std::vector<TaskItem>& items, int id);

// Меняет приоритет уже существующего элемента (по его id) и
// переставляет элемент на новое правильное место в списке.
// id элемента при этом не меняется - это тот же элемент.
void ChangeElementPriority(std::vector<TaskItem>& items, int id,
                           int new_priority);