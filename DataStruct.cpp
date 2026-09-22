#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

/* Элемент списка:
id - уникальный id,
priority - приоритет(1 - наивысший)
text - текст
*/
struct TaskItem {
  int id;
  int priority;
  std::string text;
};

/* Список со списками и их рангами
rank - ранг списка
items - список элементов
*/
struct TaskList {
  int rank;
  std::vector<TaskItem> items;
};

/* Генератор уникальных id, общий на все списки
необходим для однозначной идентификация каждого списка элементов
*/
int GenerateNextId() {
  static int next_id = 1;
  return next_id++;
}

// Добавляет элемент в список, сохраняя сортировку по priority.
// id элементу присваивается здесь - вызывающий код не должен
void AddElementToList(TaskList& list, TaskItem elem) {
  elem.id = GenerateNextId();

  auto pos = std::upper_bound(list.items.begin(), list.items.end(), elem,
                              [](const TaskItem& a, const TaskItem& b) {
                                return a.priority < b.priority;
                              });
  list.items.insert(pos, elem);
}

// Удаляет элемент по id. Если id не найден - сообщает ошибку
void DeleteElementFromList(TaskList& list, int id) {
  auto it = std::find_if(list.items.begin(), list.items.end(),
                         [id](const TaskItem& elem) { return elem.id == id; });

  if (it != list.items.end()) {
    list.items.erase(it);
  } else {
    std::cerr << "DeleteElementFromList: id " << id << " not found\n";
  }
}

// Кандидат в heap для k-way merge: указатель на элемент из исходного
// списка + откуда он пришёл (rank списка и позиция внутри него),
// чтобы после извлечения знать, каким элементом его заменить в heap.
struct MergeCandidate {
  const TaskItem* item;
  int list_rank;
  size_t list_index;  // индекс в `lists` (не rank - индекс массива)
  size_t item_index;  // позиция внутри lists[list_index].items
};

// Comparator для priority_queue
// priority_queue по умолчанию - max-heap. Чтобы получить min-heap
// компаратор строится "инвертированно": true, если `a` "хуже" `b`.
struct CandidateComparator {
  bool operator()(const MergeCandidate& a, const MergeCandidate& b) const {
    if (a.item->priority != b.item->priority) {
      return a.item->priority > b.item->priority;
    }
    return a.list_rank > b.list_rank;
  }
};

// Сливает произвольное число списков в один, отсортированный по
// priority элементов или по rank самого списка, если priority равный
std::vector<TaskItem> MergeAllLists(const std::vector<TaskList>& lists) {
  std::vector<TaskItem> result;

  std::priority_queue<MergeCandidate, std::vector<MergeCandidate>,
                      CandidateComparator>
      heap;

  for (size_t i = 0; i < lists.size(); ++i) {
    if (!lists[i].items.empty()) {
      heap.push({&lists[i].items[0], lists[i].rank, i, 0});
    }
  }

  while (!heap.empty()) {
    MergeCandidate top = heap.top();
    heap.pop();

    result.push_back(*top.item);

    size_t next_index = top.item_index + 1;
    if (next_index < lists[top.list_index].items.size()) {
      heap.push({&lists[top.list_index].items[next_index], top.list_rank,
                 top.list_index, next_index});
    }
  }

  return result;
}

void PrintList(const std::vector<TaskItem>& list) {
  for (const auto& item : list) {
    std::cout << "[priority=" << item.priority << "] " << item.text << '\n';
  }
}

int main() {
  TaskList work{1, {}};
  AddElementToList(work, {0, 1, "Утренний дейлик"});
  AddElementToList(work, {0, 1, "Собеседование с кандидатом"});
  AddElementToList(work, {0, 2, "Обед с коллегой"});

  TaskList home{2, {}};
  AddElementToList(home, {0, 1, "Генеральная уборка"});
  AddElementToList(home, {0, 2, "Забрать заказ с ПВЗ"});

  std::cout << "Список 'Работа':\n";
  PrintList(work.items);

  DeleteElementFromList(work, work.items[2].id);  // удаляем "Обед с коллегой"
  std::cout << "\nПосле удаления из 'Работа':\n";
  PrintList(work.items);

  auto merged = MergeAllLists({work, home});
  std::cout << "\nСводный список:\n";
  PrintList(merged);

  return 0;
}