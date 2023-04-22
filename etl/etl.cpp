#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "car_classes.cpp"
#include "dashboard_class.cpp"

using namespace std;

const int MAX_THREADS = 20;

roads roads_obj;  // objeto que contém as rodovias
// processando os dados
mutex lines_queue_mutex;
vector<string> lines_queue;
string road_name;
int doc_line = 0;

void process_file_line() {
    // variáveis que ficarão na operação depois de abrir o mutex
    string process_line;
    string process_road;

    {  // cria um escopo para mexer nas variáveis globais
        lock_guard<mutex> guard(lines_queue_mutex);
        process_line = lines_queue[doc_line];
        doc_line++;
        // se estivermos lendo uma linha de nova rodovia
        if (process_line.substr(0, 1) == ">") {
            road_name = process_line.substr(2, 8);
            return;
        }
        process_road = road_name;
    }

    // faz o processamento aqui
    string car_plate = process_line.substr(0, 8);
    coords car_pos = coords(process_line.substr(9, 15));
    roads_obj.update_car(car_plate, car_pos, process_road); // atualiza o carro

    // Tenta obter as informações do serviço externo gerando uma thread detached com timeout
    roads_obj.access_external_service(car_plate, process_road);    
}

void thread_work() {
    while (doc_line < lines_queue.size()) {
        process_file_line();
    }
}

int read_f(string file_name) {
    ifstream file(file_name);

    if (!file.is_open()) {  // Verifica se o arquivo existe
        cout << "File not found." << endl;
        exit(1);
    }

    string line;
    int lines_number = 0;
    while (getline(file, line)) {
        lines_queue.push_back(line);
        lines_number++;
    }
    return lines_number;
}

int main() {
    int lines_number = read_f("carros.txt");

    vector<thread> threads;
    // cria as threads
    for (int i = 0; i < MAX_THREADS; i++) {
        thread t(thread_work);
        threads.push_back(move(t));
    }

    // espera as threads terminarem
    for (int i = 0; i < MAX_THREADS; i++) {
        threads[i].join();
    }

    // cria o objeto dashboard
    dashboard dashboard_obj(&roads_obj);
    dashboard_obj.print();
}
