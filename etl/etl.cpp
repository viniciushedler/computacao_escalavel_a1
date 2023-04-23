#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "car_classes.cpp"
#include "dashboard_class.cpp"

using namespace std;

const int MAX_THREADS = 1;

// objetos globais de rodovias e dashboard
roads roads_obj;  
dashboard dashboard_obj(&roads_obj);

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
        // se acabou o arquivo
        if (doc_line >= lines_queue.size()) {
            return;
        }
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
    car* car_obj = roads_obj.update_car(car_plate, car_pos, process_road);
    
    // calcula o nível de risco
    int risk_level = 0;
    if (car_obj->is_over_speed_limit) {
        risk_level += 2;
    }
    if (car_obj->collision_status == 1) {
        risk_level += 4;
    }
    
    roads_obj.access_external_service(car_plate, process_road, risk_level);
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

void create_roads(string file_specifics) {
    ifstream file(file_specifics);

    if (!file.is_open()) {  // Verifica se o arquivo existe
        cout << "File not found." << endl;
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        string road_name = line.substr(0, 6);
        int road_length = 100;
        int max_speed = stoi(line.substr(11, 13));
        int road_width = line[7]-'0'+line[9]-'0';
        roads_obj.add_road(road_name, road_length, road_width, max_speed);
    }
}

int main() {
    // instanciando as rodovias
    create_roads("world.txt");
    int cycle = 0;
    while (true) {
        // Lê o arquivo
        string file_name = "../roads/" + to_string(cycle) + ".txt";
        while(!ifstream(file_name)) {
            dashboard_obj.print_offline();
        }
        int lines_number = read_f(file_name);

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

        // Remove os carros que não foram atualizados
        roads_obj.remove_unupdated_cars();
        
        // Imprime o dashboard
        dashboard_obj.print();

        // Apaga o arquivo recém lido
        remove(file_name.c_str());

        // Incrementa o ciclo
        cycle++;
    }
}
