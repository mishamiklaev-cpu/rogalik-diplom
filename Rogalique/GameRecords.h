#pragma once
#include <fstream>
#include <string>
#include <vector>

// одна строка таблицы рекордов
struct Record
{
    std::string nameRecord;
    int levelsRecord;
};

// таблица рекордов: лучшие результаты, сверху самый большой. Хранится в текстовом файле,
// по строке на рекорд: "имя уровни". Поэтому в имени нет пробелов
class GameRecords
{
  public:
    std::vector<Record> records;

    void LoadRecords()
    {
        records.clear();
        std::ifstream file(RECORDS_FILE);
        Record record;
        // чтение идёт, пока в файле есть пары; файла нет — таблица просто пустая
        while (file >> record.nameRecord >> record.levelsRecord)
        {
            records.push_back(record);
        }
    }

    void AddRecord(const std::string &name, int levels)
    {
        // ищем первое место, где результат хуже нового: так таблица остаётся отсортированной
        int place = 0;
        while (place < records.size() && records[place].levelsRecord >= levels)
        {
            place++;
        }
        Record record;
        record.nameRecord = name;
        record.levelsRecord = levels;
        records.insert(records.begin() + place, record);

        if (records.size() > MAX_RECORDS)
        {
            records.pop_back(); // худший выпадает
        }
        SaveRecords();
    }

  private:
    const std::string RECORDS_FILE = "records.txt";
    const int MAX_RECORDS = 10;

    void SaveRecords()
    {
        std::ofstream file(RECORDS_FILE); // файл перезаписывается целиком
        for (const Record &record : records)
        {
            file << record.nameRecord << " " << record.levelsRecord << "\n";
        }
    }
};
