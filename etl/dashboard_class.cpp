#ifndef DASHBOARD_CLASS_CPP
#define DASHBOARD_CLASS_CPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "car_classes.cpp"

using namespace std;

class dashboard{
    int number_roads;
    int number_cars;
    int number_cars_over_speed_limit;
    int number_cars_risk_collision;
    int plate;
    int GPS_position;
    int speed;
    int acceleration;
    int risk_collision;

    public:

        dashboard(roads *roads_obj) {
            this->number_roads = roads_obj->get_roads_count();
            this->number_cars = roads_obj->get_cars_count();
            this->number_cars_over_speed_limit = roads_obj->get_speeding_cars_count();
            this->number_cars_risk_collision = roads_obj->get_collision_risk_cars_count();
            // this->plate = plate;
            // this->GPS_position = GPS_position;
            // this->speed = speed;
            // this->acceleration = acceleration;
            // this->risk_collision = risk_collision;
        };

        void print(){
            std::cout << "\033[A\033[J\033[A\033[J\033[A\033[J";
            // if (*find(cars_over_speed_limit.begin(), cars_over_speed_limit.end(), plate) == plate){
            //     std::cout << "\033[31m";
            //     cout << plate << GPS_position << speed << acceleration << risk_collision << endl;
            //     std::cout << "\033[0m";
            // }
            // else{
            //     cout << plate << GPS_position << speed << acceleration << risk_collision << endl;
            // }
            cout << "Número de rodovias presentes na simulação: " << number_roads << endl;
            cout << "Número total de veículos presentes na simulação: " << number_cars << endl;
            cout << "Veículos acima do limite de velocidade: " << number_cars_over_speed_limit << endl;
            cout << "Veículos em risco de colisão: " << number_cars_risk_collision << endl;
        };

        // void update(int number_roads, int number_cars, int number_cars_over_speed_limit){
        //     this->number_roads = number_roads;
        //     this->number_cars = number_cars;
        //     this->number_cars_over_speed_limit = number_cars_over_speed_limit;
        // };
};

#endif // DASHBOARD_CLASS_CPP