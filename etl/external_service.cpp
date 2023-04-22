#ifndef EXTERNAL_SERVICE_CPP
#define EXTERNAL_SERVICE_CPP

#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

class external_service {
   private:
    int queue_check_time = 200;   // tempo em milisegundos para checar a fila
    float chance_of_error = 0.1;  // chance de dar erro no serviço
    float chance_of_delay = 0.1;  // chance de dar delay no serviço
    int max_delay_time = 2000;    // tempo máximo de delay em milisegundos
    int max_queue_size = 5;       // tamanho máximo da fila

    // resultado da query
    string name;
    string model;
    string year;

   public:
    mutex service_mutex;
    int serving_now = 0;
    int last_ticket = 0;
    int queue_size = 0;
    unordered_map<string, vector<string>> database;

    external_service(int max_queue_size) {
        this->max_queue_size = max_queue_size;
    }

    bool query_vehicle(string plate) {
        int service_ticket;
        {  // cria um escopo para mexer nas variáveis globais
            lock_guard<mutex> guard(service_mutex);
            service_ticket = this->last_ticket++;

            // verifica se a fila está cheia
            if (this->queue_size >= this->max_queue_size) {
                return false;  // retorna erro
            }
            this->queue_size++;
        }
        while (this->serving_now != service_ticket) {
            // espera o serviço ser liberado
            this_thread::sleep_for(chrono::milliseconds(queue_check_time));
        }
        // caso o serviço esteja liberado, faz o serviço

        // calcula a chance de delay
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0.0, 1.0);
        float chance = dis(gen);
        if (chance < this->chance_of_delay) {
            // calcula o tempo de delay
            uniform_real_distribution<> dis(0.0, this->max_delay_time);
            int delay_time = dis(gen);
            this_thread::sleep_for(chrono::milliseconds(delay_time));
        }

        // calcula a chance de erro
        chance = dis(gen);
        if (chance < this->chance_of_error) {
            {  // cria um escopo para mexer nas variáveis globais
                lock_guard<mutex> guard(service_mutex);
                this->serving_now++;
                this->queue_size--;
            }
            return false;  // retorna erro
        }

        // faz a query
        if (this->database.find(plate) != this->database.end()) {
            // salva o resultado
            this->name = this->database[plate][0];
            this->model = this->database[plate][1];
            this->year = this->database[plate][2];
        } else {
            // <! Obs.: a descrição do trabalho diz que deve sair de um arquivo
            // > cria novos dados para o veículo
            vector<string> first_names = {
                "João",  "Maria",  "José",    "Ana",    "Pedro",
                "Paulo", "Carlos", "Mariana", "Rafael", "Gabriel"};
            vector<string> last_names = {
                "Silva",     "Santos",  "Souza",      "Oliveira", "Pereira",
                "Rodrigues", "Almeida", "Nascimento", "Lima",     "Ferreira"};
            vector<string> models = {"Gol",   "Palio",  "Uno",     "Celta",
                                     "Corsa", "Civic",  "Corolla", "Fiesta",
                                     "Focus", "Fusion", "Accord",  "Golf"};
            vector<string> years = {"2000", "2001", "2002", "2003", "2004",
                                    "2005", "2006", "2007", "2008", "2009"};

            // sorteia os dados
            uniform_int_distribution<> dis(0, first_names.size() - 1);
            string first_name = first_names[dis(gen)];
            dis = uniform_int_distribution<>(0, last_names.size() - 1);
            string last_name = last_names[dis(gen)];
            dis = uniform_int_distribution<>(0, models.size() - 1);
            string model = models[dis(gen)];
            dis = uniform_int_distribution<>(0, years.size() - 1);
            string year = years[dis(gen)];

            // salva o resultado
            this->name = first_name + " " + last_name;
            this->model = model;
            this->year = year;
            database[plate] = {name, model, year};
        }
        {  // cria um escopo para mexer nas variáveis globais
            lock_guard<mutex> guard(service_mutex);
            this->serving_now++;
            this->queue_size--;
        }
        return true;
    }

    string get_name() { return this->name; }

    string get_model() { return this->model; }

    string get_year() { return this->year; }
};

int __main() {
    // testa a classe de external service
    external_service service(5);
    vector<string> plates = {"ABC1234", "DEF5678", "GHI9012", "JKL3456",
                             "MNO7890", "PQR1234", "STU5678", "VWX9012",
                             "YZA3456", "ABC1234"};
    for (string plate : plates) {
        cout << "Querying for plate " << plate << ":" << endl;
        service.query_vehicle(plate);
        cout << service.get_name() << endl;
        cout << service.get_model() << endl;
        cout << service.get_year() << endl;
        cout << "--------------------------" << endl;
    }
    return 0;
}

#endif  // EXTERNAL_SERVICE_CPP