#ifndef CLASSES_CPP
#define CLASSES_CPP

#include <array>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "external_service.cpp"

using namespace std;

const float TIME_SLICE = 0.1;
const int EXTERNAL_SERVICE_MAX_QUEUE_SIZE = 50;
#define SAFE 0
#define WARNING 1
#define COLIDED 2

// define uma estrutura de coordenadas
struct coords {
    int x;
    int y;

    coords(){};

    coords(int x, int y) {
        this->x = x;
        this->y = y;
    };

    friend ostream& operator<<(ostream& os, const coords& coords);

    coords(string line) {
        this->x = stoi(line.substr(0, 3));
        this->y = stoi(line.substr(4, 6));
    };
};

std::ostream& operator<<(std::ostream& os, const coords& coords) {
  return os << "(" << coords.x << ", " << coords.y << ")";
}

class car {
   public:
    // Atributos
    string plate;
    coords position;
    float speed;
    float acceleration;
    bool updated;
    int collision_status;
    bool is_over_speed_limit;
    // Atributos do sistema externo
    bool with_external_service_info = false;
    string propietary;
    string model;
    string year;

    car(){};

    // Construtor
    car(string plate, coords position) {
        this->plate = plate;
        this->position = position;
        this->speed = 0.0;
        this->acceleration = 0.0;
        this->updated = true;
    };

    // Atualiza a posição do carro (e calcula a aceleração e velocidade)
    void update_position(coords new_position) {
        this->calculate_acceleration(new_position);
        this->calculate_speed(new_position);
        this->position = new_position;
        this->updated = true;
    };

    // Define o proprietário do carro
    void set_propietary(string propietary) { this->propietary = propietary; };

    // Define o modelo do carro
    void set_model(string model) { this->model = model; };

    // Define o ano do carro
    void set_year(string year) { this->year = year; };

   private:
    // Calcula a nova velocidade
    void calculate_speed(coords new_position) {
        // Calcula e atualiza a velocidade
        this->speed = (this->position.y - new_position.y) / TIME_SLICE;
    };

    // Calcula a nova aceleração
    void calculate_acceleration(coords new_position) {
        // Calcula e atualiza a aceleração
        this->acceleration =
            (this->speed - (position.y - new_position.y)) / TIME_SLICE;
    };
};

class road {
   public:
    int length;
    int width;
    int speed_limit;
    mutex cars_mutex;
    unordered_map<string, car*> cars;
    unordered_map<int, unordered_map<int, unordered_map<string, car*>>>
        road_matrix;
    const int SECURE_TIME_DISTANCE =
        2;  // Distância segura: 2 segundos de "distância"

    road(){};

    // Construtor
    road(int length, int width, int speed_limit) {
        this->length = length;
        this->width = width;
        this->speed_limit = speed_limit;
    };

    // Verifica se o carro com a placa especificada existe
    bool has_car(string plate) {
        if (cars.find(plate) == cars.end()) {
            return false;
        }
        return true;
    };

    // Cria um carro com a placa especificada
    car* create_car(string plate, coords position) {
        car* new_car = new car(plate, position);
        cars.insert(pair<string, car*>(plate, new_car));
        road_matrix[position.x][position.y][plate] = new_car;
        return new_car;
    };

    // Retorna o carro com a placa especificada
    car* find_car(string plate) { return cars.at(plate); };

    // Retorna a quantidade de carros
    int get_cars_count() { return cars.size(); };

    // Retorna a quantidade de carros acima do limite de velocidade
    int get_speeding_cars_count() {
        int speeding_cars_count = 0;
        for (auto curr_car = cars.begin(); curr_car != cars.end(); ++curr_car) {
            if (abs(curr_car->second->speed) > this->speed_limit) {
                speeding_cars_count++;
            }
        }
        return speeding_cars_count;
    };

    // Verifica se um carro está em risco de colisão
    bool is_car_in_collision_risk(string plate) {
        car* curr_car = cars.at(plate);
        int limit_of_range =
            curr_car->position.y + curr_car->speed * this->SECURE_TIME_DISTANCE;
        limit_of_range =
            limit_of_range > this->length ? this->length : limit_of_range;
        for (int y = curr_car->position.y - 1; y <= curr_car->position.y + 1;
             y++) {
            if (road_matrix[curr_car->position.x][y].size() > 1) {
                return true;
            }
        }
        return false;
    };

    // Retorna a quantidade de carros com risco de colisão
    int get_collision_risk_cars_count() {
        int collision_risk_cars_count = 0;
        { lock_guard<mutex> lock(cars_mutex);
            for (auto curr_car = cars.begin(); curr_car != cars.end(); ++curr_car) {
                if (curr_car->second && is_car_in_collision_risk(curr_car->first)) {
                    collision_risk_cars_count++;
                }
            }
        }
        return collision_risk_cars_count;
    };

    // Atualiza a posição do carro com a placa especificada
    car* update_car(string plate, coords new_position, timed_mutex* external_mutex, external_service* external_service_obj) {
        car* curr_car = cars.at(plate);
        road_matrix[curr_car->position.x][curr_car->position.y].erase(
            plate);  // remove o carro da matriz
        curr_car->update_position(
            new_position);  // atualiza a posição do carro (no objeto do carro)
        road_matrix[new_position.x][new_position.y][plate] =
            curr_car;  // adiciona o carro na matriz
        curr_car->updated = true;
        curr_car->collision_status = get_car_status(plate, external_mutex, external_service_obj);
        if (abs(curr_car->speed) > this->speed_limit) {
            curr_car->is_over_speed_limit = true;
        } else {
            curr_car->is_over_speed_limit = false;
        }
        return curr_car;
    };

    // Remove carros que não foram atualizados
    void remove_unupdated_cars() {
        { lock_guard<mutex> lock(cars_mutex);
            for (auto curr_car = cars.begin(); curr_car != cars.end();) {
                if (curr_car->second->updated) {
                    curr_car->second->updated = false;
                    ++curr_car;
                } else {
                    road_matrix[curr_car->second->position.x]
                            [curr_car->second->position.y]
                                .extract(curr_car->first);
                    curr_car = cars.erase(curr_car);
                }
            }
        }
    };

    // Verifica o status de um carro
    int get_car_status(string plate, timed_mutex* external_mutex, external_service* external_service_obj) {
        car* curr_car = cars.at(plate);
        if (road_matrix[curr_car->position.x][curr_car->position.y].size() >
            1) {
            access_external_service(plate, external_mutex, external_service_obj);
            return COLIDED;
        } else if (is_car_in_collision_risk(plate)) {
            return WARNING;
        } else {
            return SAFE;
        }
    };

    // Função para passar para a thread acessar o serviço externo
    void thread_acess_external_service(string plate, timed_mutex* external_service_mutex, external_service* external_service_obj, int tries=1) {
        // Tenta acessar o serviço externo no máximo n vezes
        for (int i = 0; i < tries; i++) {
            // Tenta acessar o serviço externo por 1 segundo
            if (external_service_mutex->try_lock_for(chrono::seconds(1))) {
                // Verifica se o carro ainda está na rodovia
                auto it = cars.find(plate);
                if (it == cars.end()) {
                    external_service_mutex->unlock();
                    return;
                }
                // Verifica se o carro já tem as informações do serviço externo
                car* curr_car = it->second;
                if (curr_car->with_external_service_info) {
                    external_service_mutex->unlock();
                    return;
                }
                // Tenta acessar o serviço externo
                if (external_service_obj->query_vehicle(plate)) {
                    curr_car->with_external_service_info = true;
                    curr_car->set_propietary(external_service_obj->get_name());
                    curr_car->set_model(external_service_obj->get_model());
                    curr_car->set_year(external_service_obj->get_year());
                }

                // Desbloqueia o acesso ao serviço externo
                external_service_mutex->unlock();
                return;
            }   
        } 
    };

    // Acessa o serviço externo em uma thread separada
    void access_external_service(string plate,
                                 timed_mutex* external_service_mutex,
                                 external_service* external_service_obj, int tries=1) {
        // Cria a thread para acessar o serviço externo
        thread external_service_thread(&road::thread_acess_external_service, this, plate, external_service_mutex, external_service_obj, tries);
        
        // Desconecta a thread da main thread (já que o serviço não é crucial para o funcionamento do programa)
        external_service_thread.detach();
    };
};

class roads {
   public:
    unordered_map<string, road*> roads_list;
    external_service* external_service_obj =
        new external_service(EXTERNAL_SERVICE_MAX_QUEUE_SIZE);
    timed_mutex external_service_mutex;  // Mutex para acessar o serviço externo

    roads(){};

    car* update_car(string plate, coords position, string road_name) {
        // define a rodovia
        road* curr_road = roads_list.at(road_name);

        // define o carro
        if (!curr_road->has_car(plate)) {  // se o carro ainda não existe
            return curr_road->create_car(plate, position);
        };

        // atualiza a posição do carro
        return curr_road->update_car(plate, position, &external_service_mutex, external_service_obj);
    }

    // Retorna a quatidade de rodovias
    int get_roads_count() { return roads_list.size(); };

    // Retorna o número de carros das rodovias (soma de todos os carros)
    int get_cars_count() {
        int cars_count = 0;
        for (auto curr_road = roads_list.begin(); curr_road != roads_list.end();
             ++curr_road) {
            cars_count += curr_road->second->get_cars_count();
        }
        return cars_count;
    };

    // Retorna o número de veículos acima do limite de velocidade (soma de todos
    // os carros acima do limite)
    int get_speeding_cars_count() {
        int speeding_cars_count = 0;
        for (auto curr_road = roads_list.begin(); curr_road != roads_list.end();
             ++curr_road) {
            speeding_cars_count += curr_road->second->get_speeding_cars_count();
        }
        return speeding_cars_count;
    };

    // Retorna a quantidade de veículos em risco de colisão (soma de todos os
    // carros em risco de colisão)
    int get_collision_risk_cars_count() {
        int collision_risk_cars_count = 0;
        for (auto curr_road = roads_list.begin(); curr_road != roads_list.end();
             ++curr_road) {
            collision_risk_cars_count +=
                curr_road->second->get_collision_risk_cars_count();
        }
        return collision_risk_cars_count;
    };

    // Remove carros que não foram atualizados
    void remove_unupdated_cars() {
        for (auto curr_road = roads_list.begin(); curr_road != roads_list.end(); ++curr_road) {
            curr_road->second->remove_unupdated_cars();
        }
    };

    // Retorna todas as informações dos carros de todas as rodovias
    vector<car> get_all_cars_info() {
        vector<car> all_cars_info;
        for (auto curr_road = roads_list.begin(); curr_road != roads_list.end();
             ++curr_road) {
            for (auto curr_car = curr_road->second->cars.begin();
                 curr_car != curr_road->second->cars.end(); ++curr_car) {
                all_cars_info.push_back(*curr_car->second);
            }
        }
        return all_cars_info;
    };

    // Acessa o serviço externo
    void access_external_service(string plate, string road_name, int tries) {
        roads_list.at(road_name)->access_external_service(
            plate, &external_service_mutex, external_service_obj);
    };

    // Cria uma rodovia segundo as especificações
    void add_road(string road_name, int road_length, int road_width,
                  int max_speed) {
        roads_list.insert(pair<string, road*>(
            road_name, new road(road_length, road_width, max_speed)));
    };
};

#endif  // CLASSES_CPP