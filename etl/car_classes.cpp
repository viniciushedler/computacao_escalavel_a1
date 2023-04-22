#ifndef CLASSES_CPP
#define CLASSES_CPP

#include <array>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// define uma estrutura de coordenadas
struct coords {
    int x;
    int y;

    coords(){};

    coords(int x, int y) {
        this->x = x;
        this->y = y;
    };

    coords(string line) {
        this->x = stoi(line.substr(0, 3));
        this->y = stoi(line.substr(4, 6));
    }
};

class car {
   public:
    // Atributos
    string plate;
    coords position;
    float speed;
    float acceleration;
    bool updated;
    const float TIME_SLICE = 0.1;
    // Atributos do sistema externo
    boll with_external_service_info = false;
    string propietary;
    string model;
    int year;

    car(){};

    // Construtor
    car(string plate, coords position) {
        this->plate = plate;
        this->position = position;
        this->speed = 0.0;
        this->acceleration = 0.0;
        this->updated = true;
    };

    // // Destrutor
    // ~car(){};

    // Atualiza a posição do carro (e calcula a aceleração e velocidade)
    void update_position(coords new_position) {
        this->calculate_acceleration(new_position);
        this->calculate_speed(new_position);
        this->position = new_position;
        this->updated = true;
    };

    // Define o proprietário do carro
    void set_propietary(string propietary) {
        this->propietary = propietary;
    };

    // Define o modelo do carro
    void set_model(string model) { this->model = model; };

    // Define o ano do carro
    void set_year(int year) { this->year = year; };

   private:
    // Calcula a nova velocidade
    void calculate_speed(coords new_position) {
        // Calcula e atualiza a velocidade
        this->speed = (this->position.x - new_position.x) / this->TIME_SLICE;
    };

    // Calcula a nova aceleração
    void calculate_acceleration(coords new_position) {
        // Calcula e atualiza a aceleração
        this->acceleration =
            (this->speed - (position.x - new_position.x)) / this->TIME_SLICE;
    };
};

class road {
   public:
    string name;
    int length;
    int width;
    int speed_limit;
    unordered_map<string, car*> cars;
    unordered_map<int, unordered_map<int, unordered_map<string, car*>>>
        road_matrix;
    const float TIME_SLICE = 0.1;
    const int SECURE_TIME_DISTANCE =
        2;  // Distância segura: 2 segundos de "distância"
    // ... outros atributos

    road(){};

    // Construtor
    road(int length, int width, int speed_limit) {
        this->length = length;
        this->width = width;
        this->speed_limit = speed_limit;
    };

    // // Destrutor
    // ~road() {
    //     for (auto curr_car = cars.begin(); curr_car != cars.end(); ++curr_car) {
    //         delete curr_car->second;
    //     }
    // };

    // Verifica se o carro com a placa especificada existe
    bool has_car(string plate) {
        if (cars.find(plate) == cars.end()) {
            return false;
        }
        return true;
    };

    // Cria um carro com a placa especificada
    car* create_car(string plate, coords position) {
        cars.insert(pair<string, car*>(plate, new car(plate, position)));
        road_matrix[position.x][position.y][plate] = cars.at(plate);
        return cars.at(plate);
    };

    // Retorna o carro com a placa especificada
    car* find_car(string plate) { return cars.at(plate); };

    // Retorna a quantidade de carros
    int get_cars_count() { return cars.size(); };

    // Retorna a quantidade de carros acima do limite de velocidade
    int get_speeding_cars_count() {
        int speeding_cars_count = 0;
        for (auto curr_car = cars.begin(); curr_car != cars.end(); ++curr_car) {
            if (curr_car->second->speed > this->speed_limit) {
                speeding_cars_count++;
            }
        }
        return speeding_cars_count;
    };

    // Verifica se um carro está em risco de colisão
    bool is_car_in_collision_risk(string plate) {
        car* curr_car = cars.at(plate);
        int limit_of_range =
            curr_car->position.x + curr_car->speed * this->SECURE_TIME_DISTANCE;
        limit_of_range =
            limit_of_range > this->length ? this->length : limit_of_range;
        for (int x = curr_car->position.x - 1; x <= curr_car->position.x + 1;
             x++) {
            if (road_matrix[x][curr_car->position.y].size() > 1) {
                return true;
            }
        }
        return false;
    };

    // Retorna a quantidade de carros com risco de colisão
    int get_collision_risk_cars_count() {
        int collision_risk_cars_count = 0;
        for (auto curr_car = cars.begin(); curr_car != cars.end(); ++curr_car) {
            if (is_car_in_collision_risk(curr_car->first)) {
                collision_risk_cars_count++;
            }
        }
        return collision_risk_cars_count;
    };

    // Atualiza a posição do carro com a placa especificada
    void update_car(string plate, coords new_position) {
        car* curr_car = cars.at(plate);
        road_matrix[curr_car->position.x][curr_car->position.y].erase(
            plate);  // remove o carro da matriz
        curr_car->update_position(
            new_position);  // atualiza a posição do carro (no objeto do carro)
        road_matrix[new_position.x][new_position.y][plate] =
            curr_car;  // adiciona o carro na matriz
    };

    // Remove carros que não foram atualizados
    void remove_unupdated_cars() {
        for (auto curr_car = cars.begin(); curr_car != cars.end(); ++curr_car) {
            if (curr_car->second->updated) {
                curr_car->second->updated = false;
            } else {
                road_matrix[curr_car->second->position.x]
                           [curr_car->second->position.y]
                               .erase(curr_car->first);
                delete curr_car->second;
                cars.erase(curr_car->first);
            }
        }
    };
};

class roads {
   public:
    unordered_map<string, road*> roads_list;

    void update_car(string plate, coords position, string road_name) {
        // define a rodovia
        if (roads_list.find(road_name) ==
            roads_list.end()) {  // se a rodovia não existe
            roads_list.insert(pair<string, road*>(
                road_name,
                new road(10, 2, 60)  // <! Adicionar propriedades da rodovia !>
                ));
        }
        road* curr_road = roads_list.at(road_name);

        // define o carro
        if (!curr_road->has_car(plate)) {  // se o carro ainda não existe
            curr_road->create_car(plate, position);
            return;
        };

        // atualiza a posição do carro
        curr_road->update_car(plate, position);



        /* <! Fazer cálculos de atualização do carro !>
        aqui a gente vai mudar a posição dele,
        tentar calcular velocidade e aceleração,
        computar risco de colisão, essas coisas.
        */
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

    // Remove carros e rodovias que não foram atualizados
    void remove_unupdated_cars_and_roads() {
        for (auto curr_road = roads_list.begin(); curr_road != roads_list.end();
             ++curr_road) {
            curr_road->second->remove_unupdated_cars();
            if (curr_road->second->get_cars_count() == 0) {
                delete curr_road->second;
                roads_list.erase(curr_road->first);
            }
        }
    };

    // Retorna todas as informações dos carros de todas as rodovias
    vector<car*> get_all_cars_info() {
        vector<car*> all_cars_info;
        for (auto curr_road = roads_list.begin(); curr_road != roads_list.end();
             ++curr_road) {
            for (auto curr_car = curr_road->second->cars.begin();
                 curr_car != curr_road->second->cars.end(); ++curr_car) {
                all_cars_info.push_back(curr_car->second);
            }
        }
    };
};


#endif  // CLASSES_CPP