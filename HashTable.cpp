#include <iostream>
#include <vector>
#include <string>

// Класс для представления банковского счета
class BankAccount {
public:
    std::string name;  // Имя владельца счета — ключ в хеш-таблице
    int balance;       // Баланс на счете — значение, связанное с ключом

    BankAccount(std::string n, int b) : name(n), balance(b) {}  // Конструктор
};

// Класс хеш-таблицы
class HashTable {
private:
    // Внутренняя структура для ячейки хеш-таблицы
    struct HashEntry {
        std::string key;    // Ключ
        int value;          // Значение
        bool occupied;      // Флаг занятости
        bool deleted;       // Флаг удаления

        HashEntry() : occupied(false), deleted(false) {}  // Конструктор по умолчанию
    };

    std::vector<HashEntry> table;  // Вектор для хранения ячеек
    size_t capacity;               // Размер таблицы
    size_t size;                   // Количество активных элементов

    // Хеш-функция
    size_t getIndex(const std::string& key) const {
        size_t hash = 0;
        for (size_t i = 0; i < key.length(); ++i) {
            hash += (i + 1) * static_cast<size_t>(key[i]);
        }
        return hash % capacity;
    }

    // Перехеширование
    void rehash() {
        size_t newCapacity = capacity * 2;
        std::vector<HashEntry> newTable(newCapacity);
        for (const auto& entry : table) {
            if (entry.occupied && !entry.deleted) {
                size_t index = getIndex(entry.key);
                size_t i = 0;
                while (newTable[(index + i * i) % newCapacity].occupied) {
                    i++;
                }
                size_t pos = (index + i * i) % newCapacity;
                newTable[pos] = entry;
            }
        }
        table = std::move(newTable);
        capacity = newCapacity;
    }

public:
    // Конструктор
    HashTable(size_t initialCapacity = 10) : capacity(initialCapacity), size(0) {
        table.resize(capacity);
    }

    // Вставка элемента
    void insert(const std::string& key, int value) {
        if (size >= capacity / 2) rehash();
        size_t index = getIndex(key);
        size_t i = 0;
        while (table[(index + i * i) % capacity].occupied && table[(index + i * i) % capacity].key != key) {
            i++;
            if (i * i >= capacity) {
                rehash();
                insert(key, value);
                return;
            }
        }
        size_t pos = (index + i * i) % capacity;
        if (!table[pos].occupied || table[pos].deleted) {
            table[pos].key = key;
            table[pos].value = value;
            table[pos].occupied = true;
            table[pos].deleted = false;
            size++;
        }
        else {
            table[pos].value = value;  // Обновляем значение
        }
    }

    // Поиск элемента
    bool find(const std::string& key, int& value) const {
        size_t index = getIndex(key);
        size_t i = 0;
        while (table[(index + i * i) % capacity].occupied) {
            size_t pos = (index + i * i) % capacity;
            if (table[pos].key == key && !table[pos].deleted) {
                value = table[pos].value;
                return true;
            }
            i++;
            if (i * i >= capacity) break;
        }
        return false;
    }

    // Удаление элемента
    bool remove(const std::string& key) {
        size_t index = getIndex(key);
        size_t i = 0;
        while (table[(index + i * i) % capacity].occupied) {
            size_t pos = (index + i * i) % capacity;
            if (table[pos].key == key && !table[pos].deleted) {
                table[pos].deleted = true;
                size--;
                return true;
            }
            i++;
            if (i * i >= capacity) break;
        }
        return false;
    }

    // Вывод таблицы
    void print() const {
        for (size_t i = 0; i < capacity; ++i) {
            if (table[i].occupied && !table[i].deleted) {
                std::cout << "Index " << i << ": " << table[i].key << " -> " << table[i].value << std::endl;
            }
        }
    }
};

// Функция для вввода данных с клавиатуры
void userInteraction() {
    HashTable ht;
    std::string command;

    while (true) {
        std::cout << "\nВыберите действие:\n";
        std::cout << "1. Вставить новый счет\n";
        std::cout << "2. Найти баланс по имени\n";
        std::cout << "3. Удалить счет\n";
        std::cout << "4. Вывести все счета\n";
        std::cout << "5. Выйти\n";
        std::cout << "Ваш выбор: ";
        std::cin >> command;

        if (command == "1") {
            std::string name;
            int balance;
            std::cout << "Введите имя владельца: ";
            std::cin >> name;
            std::cout << "Введите баланс: ";
            std::cin >> balance;
            ht.insert(name, balance);
            std::cout << "Счет для " << name << " успешно добавлен.\n";
        }
        else if (command == "2") {
            std::string name;
            std::cout << "Введите имя владельца: ";
            std::cin >> name;
            int balance;
            if (ht.find(name, balance)) {
                std::cout << "Баланс " << name << ": " << balance << std::endl;
            }
            else {
                std::cout << "Счет для " << name << " не найден.\n";
            }
        }
        else if (command == "3") {
            std::string name;
            std::cout << "Введите имя владельца: ";
            std::cin >> name;
            if (ht.remove(name)) {
                std::cout << "Счет для " << name << " удален.\n";
            }
            else {
                std::cout << "Счет для " << name << " не найден.\n";
            }
        }
        else if (command == "4") {
            ht.print();
        }
        else if (command == "5") {
            std::cout << "Программа завершена.\n";
            break;
        }
        else {
            std::cout << "Неверная команда. Попробуйте еще раз.\n";
        }
    }
}

// Главная функция
int main() {
    userInteraction();
    return 0;
}
