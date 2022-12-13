# Лабуть Эвелина Юрьевна, БПИ218
# Вариант 26
## Условие 
Задача про экзамен. Преподаватель проводит экзамен у группы студентов. Каждый студент получает свой билет, сообщает его номер и готовит
письменный ответ. Подготовив ответ, он передает его преподавателю. Преподаватель просматривает ответ и сообщает студенту оценку. Требуется создать многопоточное приложение, моделирующее действия преподавателя и студентов. При решении использовать парадигму «клиент-сервер».
## 4 балла
### Модель параллельных вычислений
При решении была использована парадигма «клиент-сервер». 
<img width="401" alt="Снимок экрана 2022-12-12 в 18 30 46" src="https://user-images.githubusercontent.com/86932751/207085672-4b4a0dc3-97c6-4a82-9870-1195d3cb8441.png">
### Код ( файл solution.cpp)
```cpp
#include <iostream>
#include <mutex>
#include <vector>
#include <pthread.h>

std::mutex mtx;
static bool is_file_input = false;
static std::vector<int> nums_taken;
static int counter = 0;
static FILE *file_out = fopen("output.txt", "w");
static int count_of_students = 0;

void getMark(int id) {
    std::string marks[] = {"A","A+","A-","B","B+", "B-","C",
                           "C+", "C-","D", "D+", "D-", "F", "F+", "F-"};
    int random = rand() % 15;
    std::string mark = marks[random];
    mtx.lock();
    std::cout << "student #"  << id << " turned in the question paper "<< std::endl;
    std::string outline = "student #"  + std::to_string(id) + " turned in the question paper " +'\n';
    fprintf(file_out, "%s", outline.c_str());
    mtx.unlock();
    mtx.lock();
    std::cout << "student #"  << id << " got the "<< mark << " mark" << std::endl;
    outline = "student #"  + std::to_string(id) + " got the " + mark + " mark" + '\n';
    fprintf(file_out, "%s", outline.c_str());
    mtx.unlock();
}
void finishWork(int id) {
    mtx.lock();
    std::cout << "student #" << id << " finished the question paper "<< std::endl;
    std::cout << std::endl;
    std::string outline = "student #"  + std::to_string(id) + " finished the question paper " +'\n';
    fprintf(file_out, "%s", outline.c_str());
    mtx.unlock();
    getMark(id);
}

void *studentWorks(void *vargp) {
    mtx.lock();
    int id = ++counter;
    int number;
    do {
        number = 1 + (rand() % count_of_students);
    } while (std::count(nums_taken.begin(), nums_taken.end(), number));
    nums_taken.push_back(number);
    std::cout << "student #"  << id << " got question paper "<< number << std::endl;
    std::string outline = "student #" + std::to_string(id) + " got question paper " + std::to_string(number) + '\n';
    fprintf(file_out, "%s", outline.c_str());
    std::cout << "student #" << id <<" started writing down his answers" << std::endl;
    outline = "student #" + std::to_string(id) + " started writing down his answers" + '\n';
    fprintf(file_out, "%s", outline.c_str());
    mtx.unlock();
    finishWork(id);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "for console input press 0, for file input press 1" << std::endl << "> ";
        std::cin >> is_file_input;
        if (is_file_input) {
            FILE *file_in = fopen("input.txt", "r");
            fscanf(file_in, "%d", &count_of_students);
            if (count_of_students < 2 || count_of_students > 15) {
                std::cout << "incorrect count of students" << std::endl;
                fclose(file_in);
                return 0;
            }
            fclose(file_in);
        } else {
            do {
                std::cout << "please enter count of students in range [2;15]" << std::endl << "> ";
                std::cin >> count_of_students;                                                   // Count of bees.
            } while (count_of_students < 2 || count_of_students > 15);
            std::cout << std::endl;
        }
    } else if (argc == 3) {
        count_of_students  = atoi(argv[1]);
        if (count_of_students  < 2 || count_of_students > 15) {
            std::cout << "incorrect count of students" << std::endl;
            return 0;
        }
    } else {
        std::cout << "incorrect argument line format" << std::endl;
        return 0;
    }
    std::cout << std::endl;
    pthread_t students[count_of_students];
    for(size_t i = 0; i < count_of_students; i++) {
        pthread_create(&students[i], NULL, studentWorks, (void *)&students[i]);
    }
   for (size_t i = 0; i < count_of_students; i++) {
        pthread_join(students[i], NULL);
    }
    return 0;
}
```
### Входные данные 
Одно число в диапазоне [2;15] - количество студентов на экзамене.
### Сихнропримитив
В ходе решения используется мьютекс
```cpp
    mtx.lock();
    std::cout << "student #" << id << " finished the question paper "<< std::endl;
    std::cout << std::endl;
    std::string outline = "student #"  + std::to_string(id) + " finished the question paper " +'\n';
    fprintf(file_out, "%s", outline.c_str());
    mtx.unlock();
```
## 5 баллов 
### Сценарий работы программы 
## 6 баллов
### Обобщение алгоритма решения словесного сценария
### Ввод из командной строки
