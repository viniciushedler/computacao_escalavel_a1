#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
#include "car_classes.cpp"

using namespace std;

// processando os dados
mutex lines_queue_mutex;
vector<string> lines_queue;
string road_name;
int doc_line = 0;

void process_file_line() {
    string process_line;

    { // cria um escopo para mexer nas variáveis globais
    lock_guard<mutex> guard(lines_queue_mutex);
    process_line = lines_queue[doc_line];
    doc_line++;
    // se estivermos lendo uma linha de nova estrada
    if (process_line.substr(0, 1) == ">") { 
        road_name = process_line.substr(2, 8);
        return;
    }
    }
    cout << "Processing line: " << process_line << endl;
}

int read_f(string file_name){
    ifstream file(file_name);

    if (!file.is_open()) {  // Verifica se o arquivo existe
        cout << "File not found." << endl;
        exit(1);
    }

    string line;
    int lines_number = 0;
    while(getline(file, line)) {
        lines_queue.push_back(line);
        lines_number++;
    }
    return lines_number;
}

int main() {
    int lines_number = read_f("carros.txt");

    for (int i=0; i<lines_number; i++) {
        process_file_line();
    }
}
