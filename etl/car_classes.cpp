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
    string plate;
    coords position;
    float speed;
    float acceleration;
    float time_slice = 0.1;
    // ... outros atributos

    car(){};

    // Construtor
    car(string plate, coords position) {
        this->plate = plate;
        this->position = position;
        this->speed = 0.0;
        this->acceleration = 0.0;
    };

    // Atualiza a posição do carro (e calcula a aceleração e velocidade)
    void update_position(coords new_position) {
        this->calculate_acceleration(new_position);
        this->calculate_speed(new_position);
        this->position = new_position;
    };

   private:
    // Calcula a nova velocidade
    void calculate_speed(coords new_position) {
        // Calcula e atualiza a velocidade
        this->speed = (this->position.x - new_position.x) / this->time_slice;
    };

    // Calcula a nova aceleração
    void calculate_acceleration(coords new_position) {
        // Calcula e atualiza a aceleração
        this->acceleration =
            (this->speed - (position.x - new_position.x)) / this->time_slice;
    };
};

class road {
   public:
    string name;
    int length;
    int width;
    int speed_limit;
    unordered_map<string, car*> cars;
    // ... outros atributos

    road(){};

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
        cars.insert(pair<string, car*>(plate, new car(plate, position)));
        return cars.at(plate);
    };

    // Retorna o carro com a placa especificada
    car* find_car(string plate) { return cars.at(plate); };
};

class roads {
   public:
    unordered_map<string, road*> roads_list;

    void update_car(string plate, coords position, string road_name) {
        // define a estrada
        if (roads_list.find(road_name) ==
            roads_list.end()) {  // se a estrada não existe
            roads_list.insert(pair<string, road*>(
                road_name,
                new road(10, 2, 60)  // <! Adicionar propriedades da estrada !>
                ));
        }
        road* this_road = roads_list.at(road_name);

        // define o carro
        if (!this_road->has_car(plate)) {  // se o carro ainda não existe
            this_road->create_car(plate, position);
            return;
        };

        car* this_car = this_road->find_car(plate);  // se o carro já existe

        // atualiza a posição do carro
        this_car->update_position(position);

        /* <! Fazer cálculos de atualização do carro !>
        aqui a gente vai mudar a posição dele,
        tentar calcular velocidade e aceleração,
        computar risco de colisão, essas coisas.
        */
    }
};

#endif  // CLASSES_CPP