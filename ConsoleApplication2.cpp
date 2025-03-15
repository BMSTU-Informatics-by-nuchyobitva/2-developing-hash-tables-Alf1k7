#include <iostream>  
#include <vector>    
#include <string>    
// Класс для представления банковского счета
class BankAccount {
public:
    std::string name;  // Поле для хранения имени владельца счета — это ключ в хеш-таблице, уникальный идентификатор
    int balance;       // Поле для хранения баланса счета — это значение, связанное с ключом


    BankAccount(std::string n, int b) : name(n), balance(b) {// Конструктор для создания объекта счета с заданным именем и балансом
    }  

};
class HashTable {
private:
    // Внутренняя структура для представления одной ячейки хеш-таблицы
    struct HashEntry {
        std::string key;    // Ключ — имя владельца счета, используется для поиска и идентификации записи
        int value;          // Значение — баланс счета, связанный с ключом
        bool occupied;      // Флаг, показывающий, занята ли ячейка (true — есть данные, false — свободна)
        bool deleted;       // Флаг, показывающий, был ли элемент логически удален (нужен для корректной работы при удалении)

        // Конструктор по умолчанию для создания пустой ячейки
        HashEntry() : occupied(false), deleted(false) {}  // Инициализируем ячейку как свободную (occupied = false) и не удаленную (deleted = false)
    };

    std::vector<HashEntry> table;  // Вектор для хранения всех ячеек хеш-таблицы, его размер определяет вместимость таблицы
    size_t capacity;               // Переменная, хранящая текущий размер таблицы (количество доступных ячеек)
    size_t size;                   // Переменная, хранящая текущее количество активных (не удаленных) элементов в таблице

    // Хеш-функция для вычисления индекса ячейки на основе ключа
    size_t getIndex(const std::string& key) const {
        size_t hash = 0;  
        for (size_t i = 0; i < key.length(); ++i) {  
            hash += (i + 1) * static_cast<size_t>(key[i]);  
        }
        return hash % capacity;  // Делим хеш на размер таблицы и возвращаем остаток — это индекс в пределах таблицы
    }

    // Метод перехеширования для увеличения размера таблицы при заполнении
    void rehash() {
        size_t newCapacity = capacity * 2;  // Удваиваем текущий размер таблицы, чтобы уменьшить вероятность коллизий
        std::vector<HashEntry> newTable(newCapacity);  // Создаем новый вектор с увеличенной емкостью для хранения элементов

        // Переносим все существующие элементы из старой таблицы в новую
        for (const auto& entry : table) { 
            if (entry.occupied && !entry.deleted) {  
                size_t index = getIndex(entry.key);  
                size_t i = 0;  // Счетчик для квадратичного пробирования, начинаем с 0
                
                while (newTable[(index + i * i) % newCapacity].occupied) {i++;  
                }
                size_t pos = (index + i * i) % newCapacity; 
                newTable[pos] = entry;  
            }
        }
        table = std::move(newTable);  // Заменяем старую таблицу новой с помощью move для эффективности (избегаем копирования)
        capacity = newCapacity;       // Обновляем переменную capacity, чтобы отражать новый размер таблицы
    }

public:
    // Конструктор хеш-таблицы с начальной емкостью (по умолчанию 10)
    HashTable(size_t initialCapacity = 10) : capacity(initialCapacity), size(0) {  // Устанавливаем начальные значения через список инициализации
        table.resize(capacity);  
    }

    // Метод вставки нового элемента (ключа и значения) в таблицу
    void insert(const std::string& key, int value) {
        if (size >= capacity / 2) rehash();  // Если таблица заполнена более чем на 50%, увеличиваем ее размер для предотвращения частых коллизий
        size_t index = getIndex(key);  
        size_t i = 0;  
       
        while (table[(index + i * i) % capacity].occupied && table[(index + i * i) % capacity].key != key) {i++; 
            if (i * i >= capacity) {  // Если шаг стал слишком большим (вышел за пределы таблицы), нужно перехешировать
                rehash();  
                insert(key, value);  
                return;  
            }
        }
        size_t pos = (index + i * i) % capacity;  // Вычисляем итоговую позицию ячейки после пробирования
        
        if (!table[pos].occupied || table[pos].deleted) {  
            table[pos].key = key;         
            table[pos].value = value;     
            table[pos].occupied = true;  
            table[pos].deleted = false;   
            size++; 
        }
        else {  // Если ячейка уже занята и ключ совпадает
            table[pos].value = value;  // Обновляем значение для существующего ключа
        }
    }

    // Метод поиска элемента по ключу, возвращает true, если найден, и записывает значение в переменную
    bool find(const std::string& key, int& value) const {
        size_t index = getIndex(key);  // Вычисляем начальный индекс для ключа
        size_t i = 0;  // Счетчик для квадратичного пробирования
        // Ищем ячейку, пока она занята
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

    // Метод удаления элемента по ключу, возвращает true, если удаление прошло успешно
    bool remove(const std::string& key) {
        size_t index = getIndex(key);  // Вычисляем начальный индекс для ключа
        size_t i = 0;  
        
        while (table[(index + i * i) % capacity].occupied) { 
            size_t pos = (index + i * i) % capacity;  // Вычисляем текущую позицию
            if (table[pos].key == key && !table[pos].deleted) {  // Если ключ совпадает и элемент не удален
                table[pos].deleted = true;  // Помечаем элемент как удаленный (логическое удаление)
                size--; 
                return true;  // Возвращаем true, указывая успешное удаление
            }
            i++;  
            if (i * i >= capacity) break;  // Если шаг стал слишком большим, прекращаем поиск
        }
        return false; 
    }

    // Вывод
    void print() const {
        for (size_t i = 0; i < capacity; ++i) {  
            if (table[i].occupied && !table[i].deleted) {  
                std::cout << "Index " << i << ": " << table[i].key << " -> " << table[i].value << std::endl;  // Форматированный вывод: индекс, ключ, значение
            }
        }
    }
};

//Пример
int main() {
    HashTable ht;  
    ht.insert("Alfred", 1000);  
    ht.insert("Rockefeller", 5000);
    ht.print();  

    int balance;  
    if (ht.find("Alfred", balance)) {  
        std::cout << "Alfred's balance: " << balance << std::endl;  
    }
    ht.remove("Rockefeller"); // Удаляем запись с ключом Rockefeller
    ht.print();  

    return 0;  
}