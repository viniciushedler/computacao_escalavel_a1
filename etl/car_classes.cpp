#ifndef CLASSES_CPP
#define CLASSES_CPP

#include <array>
#include <iostream>
#include <unordered_map>
#include <string>
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

    car(string plate, coords position) {
        this->plate = plate;
        this->position = position;
        this->speed = NULL;
        this->acceleration = NULL;
    };

    void update_position(coords new_position) {
        this->calculate_acceleration(new_position);
        this->calculate_speed(new_position);
        this->position = new_position;
    };

   private:
    void calculate_speed(coords new_position) {
        // Calcula e atualiza a velocidade
        this->speed = (this->position.x - new_position.x) / this->time_slice;
    };

    void calculate_acceleration(coords new_position) {
        // Calcula e atualiza a aceleração
        this->acceleration = (this->speed - (position.x - new_position.x)) / this->time_slice;
    };
};

class road {
   public:
    string name;
    int length;
    int width;
    int speed_limit;
    unordered_map<string, car> cars;
    // ... outros atributos

    road(){};

    road(int length, int width, int speed_limit) {
        this->length = length;
        this->width = width;
        this->speed_limit = speed_limit;
    };

    bool has_car(string plate) {
        if (cars.find(plate) == cars.end()) {
            return false;
        }
        return true;
    };

    car create_car(string plate, coords position) {
        cars.insert(pair<string, car>(plate, car(plate, position)));
        return cars.at(plate);
    };

    car find_car(string plate) { return cars.at(plate); };
};

class roads {
   public:
    unordered_map<string, road> roads_list;

    void update_car(string plate, coords position, string road_name) {
        // define a estrada
        if (roads_list.find(road_name) ==
            roads_list.end()) {  // se a estrada não existe
            roads_list.insert(pair<string, road>(
                road_name,
                road(10, 2, 60)  // <! Adicionar propriedades da estrada !>
                ));
        }
        road this_road = roads_list.at(road_name);

        // define o carro
        if (!this_road.has_car(plate)) {  // se o carro ainda não existe
            this_road.create_car(plate, position);
            return;
        };
        car this_car = this_road.find_car(plate);  // se o carro já existe

        /* <! Fazer cálculos de atualização do carro !>
        aqui a gente vai mudar a posição dele,
        tentar calcular velocidade e aceleração,
        computar risco de colisão, essas coisas.
        */
    }
};

#endif  // CLASSES_CPP