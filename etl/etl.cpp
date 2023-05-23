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

const long long int N_THREADS_ROADS = 20;
const long long int N_THREADS_CARS_IN_ROAD = 20;

// objetos globais de rodovias e dashboard
roads roads_obj;  
dashboard dashboard_obj(&roads_obj);

// processando os dados
mutex lines_queue_mutex;
vector<string> lines_queue;
vector<string> roads_names; // Lista com as rodovias
long long int curr_road = 0;
mutex curr_road_mutex;
long long int doc_line = 0;

// Banco de dados
auto bd = Redis("tcp://127.0.0.1:6379");

void add_to_log_file(long long int time_diff) {
    ofstream file;
    file.open("log.txt", ios::app);
    file << time_diff << endl;
    file.close();
}

void process_file_line(string road_name, vector<string>* cars_data, mutex* cars_data_mutex, long long int* data_line, long long int new_date) {
    // variáveis que ficarão na operação depois de abrir o mutex
    string process_line;

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
    car* car_obj = roads_obj.update_car(car_plate, car_pos, road_name, new_date);
    
    // calcula o nível de risco
    long long int risk_level = 0;
    if (car_obj->is_over_speed_limit) {
        risk_level += 2;
    }
    if (car_obj->collision_status == 1) {
        risk_level += 4;
    }

    // acessa o serviço externo
    roads_obj.access_external_service(car_plate, road_name, risk_level+1);
}

void cars_work(string road_name, vector<string>* cars_data, long long int new_date) {
    mutex cars_data_mutex;
    long long int data_line = 0;
    
    while (data_line < cars_data->size()) {
        process_file_line(road_name, cars_data, &cars_data_mutex, &data_line, new_date);
        data_line++;
    }
}


long long int read_query(string key, vector<string>* values) {
    string line;
    auto data = bd.get(key);

    stringstream file;
    // cout << "Leitura: " << *data << "<FIM DA LEITURA>" << endl; // APAGAR (mas esse aqui é legal)

    string data_str = data.value_or("");
    if (data_str == "") {
        return -1;
    }
    file = stringstream(data_str);
    // cout << "conseguiu converter o arquivo" << endl; // APAGAR
    while (getline(file, line, '\n')) {
        values->push_back(line);
    }
    // cout << "conseguiu criar o vetor com o arquivo" << endl; // APAGAR
    // cout << "tamanho do vetor: " << endl; //APAGAR
    // cout << values->size() << endl; // APAGAR

    if (values->size() == 0) {
        return -1;
    }

    // Converte o unix timestamp em string para um inteiro
    string timestamp = (*values)[0];
    long long int new_date = stoll(timestamp);
    // Remove o timestamp da lista
    values->erase(values->begin());


    return new_date;
}

void road_work() {
    long long int cycle = 0;
    string road_name;
    while (true) {
    // cout << "CHEGOU NA THREAD DA RODOVIA" << endl; // APAGAR
        {
            lock_guard<mutex> guard(curr_road_mutex);
            // cout << "CHEGOU DENTRO DO MUTEX" << endl; // APAGAR
            // cout << "curr_road: " << curr_road << endl; // APAGAR
            // cout << "road_name: " << roads_names[curr_road] << endl; // APAGAR
            road_name = roads_names[curr_road];
            // cout << "road_name: " << road_name << endl; // APAGAR
            curr_road++;
            // cout << "curr_road: " << curr_road << endl; // APAGAR
            curr_road %= roads_names.size();
            // cout << "curr_road: " << curr_road << endl; // APAGAR
        }
    // cout << "CHEGOU NA THREAD DA RODOVI2A" << endl; // APAGAR

        // lê o arquivo
        string file_name = "" + road_name + " " + to_string(cycle);

        // cout << "Lendo arquivo: " << file_name << endl; // APAGAR

        // lê o arquivo
        vector<string>* cars_data = new vector<string>();
        // cout << "cars data recém criado. Tamanho:" << cars_data->size() << "FIM" << endl; // APAGAR
        long long int new_date = read_query(file_name, cars_data);
        if (new_date == -1) {
            continue; // não tem arquivo para ler
        }
        if (cars_data->size() == 0) {
            cycle++;
            continue; // arquivo vazio
        }

        vector<thread> threads;
        // cria as threads para os carros
        for (long long int i = 0; i < N_THREADS_CARS_IN_ROAD; i++) {
            thread t(cars_work, road_name, cars_data, new_date);
            threads.push_back(move(t));
        }
        // espera as threads terminarem
        for (long long int i = 0; i < N_THREADS_CARS_IN_ROAD; i++) {
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

        // Pega o momento atual em unix time
        long long int curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        // Calcula a diferença entre o momento atual e o momento em que o arquivo foi criado
        long long int time_diff = curr_time - new_date;
        // Adiciona para o arquivo de log essa diferença
        add_to_log_file(time_diff);

        cycle++;
    }
}


long long int read_f(string file_name) {
    ifstream file(file_name);

    if (!file.is_open()) {  // Verifica se o arquivo existe
        cout << "File not found." << endl;
        exit(1);
    }

    string line;
    long long int lines_number = 0;
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
        long long int road_length = 100;
        long long int max_speed = stoi(line.substr(11, 13));
        long long int road_width = line[7]-'0'+line[9]-'0';
        roads_obj.add_road(road_name, road_length, road_width, max_speed);
        roads_names.push_back(road_name);
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
    for (long long int i = 0; i < N_THREADS_ROADS; i++) {
        thread t(road_work);
        threads.push_back(move(t));
    }

    // espera as threads terminarem
    for (long long int i = 0; i < N_THREADS_ROADS; i++) {
        threads[i].join();
    }



    dashboard_obj.is_mock_on = false;
}
