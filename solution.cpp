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
