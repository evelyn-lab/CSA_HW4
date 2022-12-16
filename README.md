# Лабуть Эвелина Юрьевна, БПИ218
# Вариант 26
## Условие 
Задача про экзамен. Преподаватель проводит экзамен у группы студентов. Каждый студент получает свой билет, сообщает его номер и готовит
письменный ответ. Подготовив ответ, он передает его преподавателю. Преподаватель просматривает ответ и сообщает студенту оценку. Требуется создать многопоточное приложение, моделирующее действия преподавателя и студентов. При решении использовать парадигму «клиент-сервер».
## 4 балла
### Модель параллельных вычислений
При решении была использована парадигма «клиент-сервер». 
<img width="401" alt="Снимок экрана 2022-12-12 в 18 30 46" src="https://user-images.githubusercontent.com/86932751/207085672-4b4a0dc3-97c6-4a82-9870-1195d3cb8441.png">
### Код (файл solution.cpp) с комментариями
```cpp
#include <iostream>
#include <mutex>
#include <vector>
#include <pthread.h>

std::mutex mtx; // синхропримитив
static bool is_file_input = false; // должен ли ввод осуществляться из файла
static std::vector<int> nums_taken; // "занятые" номера билетов
static int counter = 0; // для вывода порядкового номера студента  
static FILE *file_out = fopen("output.txt", "w");
static int count_of_students = 0; // количество потоков (студентов)

void getMark(int id) {
    std::string marks[] = {"A","A+","A-","B","B+", "B-","C",
                           "C+", "C-","D", "D+", "D-", "F", "F+", "F-"}; // массив оценок
    int random = rand() % 15; // рандом на элемент массива 
    std::string mark = marks[random]; // выбор оценки с учетом рандома 
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
    std::cout << "student #" << id << " finished the question paper "<< std::endl;
    std::cout << std::endl;
    std::string outline = "student #"  + std::to_string(id) + " finished the question paper " +'\n';
    fprintf(file_out, "%s", outline.c_str());
    getMark(id);
}

void *studentWorks(void *vargp) {
    mtx.lock();
    int id = ++counter;
    int number; // номер билета 
    do {
        number = 1 + (rand() % count_of_students); // генератор номера билета  
    } while (std::count(nums_taken.begin(), nums_taken.end(), number)); // проверка номера
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
    pthread_t students[count_of_students]; // массив потоков
    for(size_t i = 0; i < count_of_students; i++) {
        pthread_create(&students[i], NULL, studentWorks, (void *)&students[i]); // вызов функции для каждого потока
    }
   for (size_t i = 0; i < count_of_students; i++) {
        pthread_join(students[i], NULL); 
    }
    return 0;
}
```
### Входные данные 
Одно число в диапазоне [2;15] - количество студентов на экзамене.
### Консольный ввод данных
В начале программы пользователь может выбрать ввод с консоли или из файла. 
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
Как объекты, студенты одновремнно выполняют одно и то же действие, т.е. решают экзамен. Выдача билетов ( то есть по сути распределение вариантов между студентами ) в моей реализации также происходит одновременно, а вот проверка работы преподавателем после сдачи и выставление оценки уже отдельно для каждого.  
## 6 баллов
### Обобщение алгоритма решения словесного сценария
Алгоритм решения заключается в создании заданного пользователем числа потоков (количество студентов). Далее происходит распределение вариантов с помощью функции *rand()* и проверка нахождения номера билета в векторе *nums_taken* (т.е. занят ли билет другим студентом). Чтобы не присходило коллизии номеров (один и тот же билет не может быть занят несколькими студентами) эта операция блокируется мьютексом. Затем идет функция завершения работы (т.е. просто выводиться сообщение о завершении студентом написания билета) и билет сдаётся преподавателю (происходит обращение к серверу). Процесс сдачи также блокируется мьютексом т.к. преподавателю нужно время на проверку работы студента. После выводится оценка студента и поток завершает работу (ответ сервера). 
### Ввод данных из командной строки
Ввод данных из командной строки реализован. 
## 7 баллов 
### Ввод данных из файла и вывод в файл 
Ввод данных из файла реализован. Результаты работы программы выводятся на экран и записываются в файл. 
### Входные и выходные файлы
входные файлы находятся вот тут: https://github.com/evelyn-lab/CSA_HW4/tree/main/input_files
соответствующие им выходные файлы находятся вот тут: https://github.com/evelyn-lab/CSA_HW4/tree/main/output_files
Учтите, что для корректной работы программы файлы должны называться input.txt и output.txt.
