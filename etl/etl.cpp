#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <sw/redis++/redis++.h>

#include "car_classes.cpp"
#include "dashboard_class.cpp"

using namespace std;
using namespace sw::redis;

const int N_THREADS_ROADS = 20;
const int N_THREADS_CARS_IN_ROAD = 20;

// objetos globais de rodovias e dashboard
roads roads_obj;  
dashboard dashboard_obj(&roads_obj);

// processando os dados
mutex lines_queue_mutex;
vector<string> lines_queue;
vector<string> roads_names; // Lista com as rodovias
int curr_road = 0;
mutex curr_road_mutex;
int doc_line = 0;

// Banco de dados
auto bd = Redis("tcp://127.0.0.1:6379");

void process_file_line(string road_name, vector<string>* cars_data, mutex* cars_data_mutex, int* data_line, int new_date) {
    // variáveis que ficarão na operação depois de abrir o mutex
    string process_line;
    string process_road;

    {  // cria um escopo para mexer nas variáveis globais
        lock_guard<mutex> guard(*cars_data_mutex);
        // se acabou o arquivo
        if (*data_line >= cars_data->size()) {
            return;
        }
        process_line = (*cars_data)[*data_line];
        *data_line++;
    }

    // faz o processamento aqui
    string car_plate = process_line.substr(0, 7);
    coords car_pos = coords(process_line.substr(9, 15));
    car* car_obj = roads_obj.update_car(car_plate, car_pos, process_road, new_date);
    
    // calcula o nível de risco
    int risk_level = 0;
    if (car_obj->is_over_speed_limit) {
        risk_level += 2;
    }
    if (car_obj->collision_status == 1) {
        risk_level += 4;
    }

    // acessa o serviço externo
    roads_obj.access_external_service(car_plate, process_road, risk_level+1);
}

void cars_work(string road_name, vector<string>* cars_data, int new_date) {
    mutex cars_data_mutex;
    int data_line = 0;
    
    while (data_line < cars_data->size()) {
        process_file_line(road_name, cars_data, &cars_data_mutex, &data_line, new_date);
    }
}

void road_work() {
    int cycle = 0;
    string road_name;
    while (true) {
        {
            lock_guard<mutex> guard(curr_road_mutex);
            road_name = roads_names[curr_road];
            curr_road++;
            curr_road %= roads_names.size();
        }

        // lê o arquivo
        string file_name = "" + road_name + " " + to_string(cycle);

        // lê o arquivo
        vector<string>* cars_data;
        int new_date = read_query(file_name, cars_data);

        vector<thread> threads;
        // cria as threads para os carros
        for (int i = 0; i < N_THREADS_CARS_IN_ROAD; i++) {
            thread t(cars_work, road_name, cars_data, new_date);
            threads.push_back(move(t));
        }
        // espera as threads terminarem
        for (int i = 0; i < N_THREADS_CARS_IN_ROAD; i++) {
            threads[i].join();
        }
        
        // Remove os carros que não foram atualizados (na respectiva rodovia)
        roads_obj.remove_unupdated_cars(road_name);

        // Atualiza o número de carros em colisão
        roads_obj.update_collision_risk_cars_count(new_date);

        // Atualiza o número de carros acima do limite de velocidade
        roads_obj.update_speeding_cars_count(new_date);

        // Atualiza o número de carros na simulação
        roads_obj.update_cars_count(new_date);

        // Atualiza o número de rodovias na simulação
        roads_obj.update_roads_count(new_date);

        // Atualiza a variável com as informações dos carros
        roads_obj.update_all_cars_info(new_date);

        cycle++;
    }
}

int read_query(string key, vector<string>* values) {
    string line;
    stringstream file = bd.get(key);
    while (getline(file, line, '\n')) {
        values->push_back(line);
    }
    // Converte o unix timestamp em string para um inteiro
    string timestamp = (*values)[0];
    int new_date = stoi(timestamp);
    // Remove o timestamp da lista
    values->erase(values->begin());
    return new_date;
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

void start_dashboard() {
    thread t(update_dashboard, &dashboard_obj, 10);
    t.detach();
}

int main() {
    // instanciando as rodovias e o dashboard
    create_roads("world.txt");
    start_dashboard();
    dashboard_obj.is_mock_on = true;

    vector<thread> threads;
    // cria as threads para as rodovias
    for (int i = 0; i < N_THREADS_ROADS; i++) {
        thread t(road_work);
        threads.push_back(move(t));
    }

    // espera as threads terminarem
    for (int i = 0; i < N_THREADS_ROADS; i++) {
        threads[i].join();
    }



    dashboard_obj.is_mock_on = false;
}
