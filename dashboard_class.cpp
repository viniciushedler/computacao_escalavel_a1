#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;


class dashboard{
    int number_roads;
    int number_cars;
    int cars_over_speed_limit;
    int plate;
    int GPS_position;
    int speed;
    int acceleration;
    int risk_collision;

    public:
        dashboard(int number_roads, int number_cars, int number_cars_over_speed_limit){
            this->number_roads = get_roads_count();
            this->number_cars = number_cars;
            this->cars_over_speed_limit = cars_over_speed_limit;
            this->plate = plate;
            this->GPS_position = GPS_position;
            this->speed = speed;
            this->acceleration = acceleration;
            this->risk_collision = risk_collision;
        };

        void print(){
            std::cout << "\033[A\033[J\033[A\033[J\033[A\033[J";
            if (*find(cars_over_speed_limit.begin(), cars_over_speed_limit.end(), plate) == plate){
                std::cout << "\033[31m";
                cout << plate << GPS_position << speed << acceleration << risk_collision << endl;
                std::cout << "\033[0m";
            }
            else{
                cout << plate << GPS_position << speed << acceleration << risk_collision << endl;
            }
            cout << "Número de rodovias presentes na simulação: " << number_roads << endl;
            cout << "Número total de veículos presentes na simulação: " << number_cars << endl;
            cout << "Veículos acima do limite de velocidade: " << cars_over_speed_limit << endl;
        };

        // void update(int number_roads, int number_cars, int number_cars_over_speed_limit){
        //     this->number_roads = number_roads;
        //     this->number_cars = number_cars;
        //     this->number_cars_over_speed_limit = number_cars_over_speed_limit;
        // };
};