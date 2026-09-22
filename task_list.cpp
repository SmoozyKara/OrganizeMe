#include "task_list.h"

#include <queue>

int GenerateNextListId() {
  static int next_id = 1;
  return next_id++;
}

// Всё внутри namespace { ... } видно только в этом файле (task_list.cpp)
// и нигде больше - это способ "спрятать" детали реализации, которые не
// нужны никому снаружи. MergeCandidate и CandidateComparator нужны
// только внутри MergeAllLists, поэтому в task_list.h их нет вообще -
// незачем показывать их всем, кто подключает этот заголовок.
namespace {

// Кандидат в heap для k-way merge: указатель на элемент из исходного
// списка + откуда он пришёл (rank списка и позиция внутри него),
// чтобы после извлечения знать, каким элементом его заменить в heap.
struct MergeCandidate {
  const TaskItem* item;
  int list_rank;
  size_t list_index;  // индекс в `lists` (не rank - индекс массива)
  size_t item_index;  // позиция внутри lists[list_index].items
};

// Comparator для priority_queue.
// priority_queue по умолчанию - max-heap. Чтобы получить min-heap
// (наверху - наивысший приоритет, т.е. наименьшее число), компаратор
// строится "инвертированно": true, если `a` "хуже" `b`.
struct CandidateComparator {
  bool operator()(const MergeCandidate& a, const MergeCandidate& b) const {
    if (a.item->priority != b.item->priority) {
      return a.item->priority > b.item->priority;
    }
    return a.list_rank > b.list_rank;
  }
};

}  // namespace

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