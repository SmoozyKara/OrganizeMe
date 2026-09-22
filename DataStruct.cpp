#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

// Элемент списка: уникальный id, приоритет (1 - наивысший) и текст.
struct TaskItem {
  int id;
  int priority;
  std::string text;
};

// Именованный список задач со своим rank'ом (порядком списка среди
// других списков, а не id самого списка - см. обсуждение выше).
// items предполагается отсортированным по priority по возрастанию.
struct TaskList {
  int rank;
  std::vector<TaskItem> items;
};

// Кандидат в heap: указатель на элемент из исходного списка + откуда
// он пришёл (rank списка и позиция внутри этого списка), чтобы после
// извлечения знать, каким элементом его заменить в heap.
struct MergeCandidate {
  const TaskItem* item;
  int list_rank;
  size_t list_index;  // индекс в `lists` (не rank - именно индекс массива)
  size_t item_index;  // позиция внутри lists[list_index].items
};

// priority_queue по умолчанию - max-heap. Чтобы получить min-heap
// (наверху - наивысший приоритет, т.е. наименьшее число), компаратор
// строим так же, как для std::greater: true, если `a` "хуже" `b`.
struct CandidateComparator {
  bool operator()(const MergeCandidate& a, const MergeCandidate& b) const {
    if (a.item->priority != b.item->priority) {
      return a.item->priority > b.item->priority;
    }
    return a.list_rank > b.list_rank;
  }
};

std::vector<TaskItem> MergeAllLists(const std::vector<TaskList>& lists) {
  std::vector<TaskItem> result;

  std::priority_queue<MergeCandidate, std::vector<MergeCandidate>,
                      CandidateComparator>
      heap;

  // Затравка: по одному (первому, т.е. наивысшего приоритета)
  // элементу от каждого непустого списка.
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
  TaskList work{1,
                {
                    {1, 1, "Утренний дейлик"},
                    {2, 1, "Собеседование с кандидатом"},
                    {3, 2, "Обед с коллегой"},
                }};

  TaskList home{2,
                {
                    {4, 1, "Генеральная уборка"},
                    {5, 2, "Забрать заказ с ПВЗ"},
                }};

  auto merged = MergeAllLists({work, home});

  std::cout << "Сводный список:\n";
  PrintList(merged);

  return 0;
}