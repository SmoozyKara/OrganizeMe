#pragma once

#include <QString>

#include "task_list.h"

// Сохраняет все списки и задачи в JSON-файл по указанному пути.
// Возвращает false, если не удалось открыть файл для записи.
bool SaveOrganizerToFile(const Organizer& organizer, const QString& filePath);

// Загружает списки и задачи из JSON-файла, заменяя текущее
// содержимое organizer.lists. Также подтягивает счётчики id
// (EnsureNextIdAtLeast/EnsureNextListIdAtLeast), чтобы новые элементы
// не получили уже занятый id.
// Возвращает false, если файл не найден или повреждён - в этом
// случае organizer не трогается (остаётся как был до вызова).
bool LoadOrganizerFromFile(Organizer& organizer, const QString& filePath);