#ifndef DASHBOARD_CLASS_CPP
#define DASHBOARD_CLASS_CPP

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <thread>

// Define cores para o terminal
#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define ORANGE  "\033[33m"      /* Orange */
#define YELLOW  "\033[33m"      /* Yellow */
#define WHITE   "\033[37m"      /* White */
#define BOLD    "\033[1m"       /* Bold */
#define ITALIC  "\033[3m"       /* Italic */
#define RETURN  "\x1B[2J\x1B[H"

#include "car_classes.cpp"

using namespace std;

// Funções de atualização das propriedades do dashboard
void update_number_roads(int *property, int interval, roads *roads_obj) {
    while (true) {
        *property = roads_obj->get_roads_count();
        this_thread::sleep_for(chrono::milliseconds(interval));
    }
}
void update_number_cars(int *property, int interval, roads *roads_obj) {
    while (true) {
        *property = roads_obj->get_cars_count();
        this_thread::sleep_for(chrono::milliseconds(interval));
    }
}
void update_number_cars_over_speed_limit(int *property, int interval, roads *roads_obj) {
    while (true) {
        *property = roads_obj->get_speeding_cars_count();
        this_thread::sleep_for(chrono::milliseconds(interval));
    }
}
void update_number_cars_risk_collision(int *property, int interval, roads *roads_obj) {
    while (true) {
        *property = roads_obj->get_collision_risk_cars_count();
        this_thread::sleep_for(chrono::milliseconds(interval));
    }
}
void update_all_cars_info(vector<car> *property, int interval, roads *roads_obj) {
    while (true) {
        // *property = roads_obj->get_all_cars_info();
        *property = *new vector<car>;
        this_thread::sleep_for(chrono::milliseconds(interval));
    }
}

class dashboard{
    int number_roads;
    int number_cars;
    int number_cars_over_speed_limit;
    int number_cars_risk_collision;
    vector<car> all_cars_info;

    public:
        dashboard(roads *roads_obj) {
            thread t1(update_number_roads, &this->number_roads, 1'000, roads_obj);
            thread t2(update_number_cars, &this->number_cars, 1'000, roads_obj);
            thread t3(update_number_cars_over_speed_limit, &this->number_cars_over_speed_limit, 50, roads_obj);
            thread t4(update_number_cars_risk_collision, &this->number_cars_risk_collision, 10, roads_obj);
            thread t5(update_all_cars_info, &this->all_cars_info, 1'000, roads_obj);
            t1.detach();
            t2.detach();
            t3.detach();
            t4.detach();
            t5.detach();
        };

        // função para pegar os dados que serão printados
        void get_data(int i, vector<car> all_cars_info){
            if (all_cars_info[i].with_external_service_info == false){
                // Cabeçalho da tabela
                cout << setw(10) << "Plate" << setw(10) << "Position" << "Speed" << "Acceleration" << 
                "Propietary" << "Model" << "Year" << endl;
                cout << all_cars_info[i].plate << setw(10) << " " << "(" << all_cars_info[i].position.x << ", " 
                << all_cars_info[i].position.y << ")" << " " << all_cars_info[i].speed << setw(10) << " " 
                << all_cars_info[i].acceleration << setw(10) << " " << all_cars_info[i].propietary << setw(10) << " " 
                << all_cars_info[i].model << setw(10) << " " << all_cars_info[i].year << endl;
            }
            else{
                cout << setw(10) << "Plate" << setw(10) << "Position" << "Speed" << "Acceleration" << endl;
                cout << setw(10) << all_cars_info[i].plate << setw(10) << " " << "(" << all_cars_info[i].position.x << ", " 
                << all_cars_info[i].position.y << ")" << setw(10) << " " << all_cars_info[i].speed << setw(10) << " " 
                << all_cars_info[i].acceleration << setw(10) << " " << endl;
            }
        };

        // função para printar os dados 
        void print(){
            cout << "Number of roads: " << BOLD << number_roads << RESET << endl;
            cout << "Number of cars: " << BOLD << number_cars << RESET << endl;
            cout << "Number of cars over speed limit: " << BOLD << number_cars_over_speed_limit << RESET << endl;
            cout << "Number of cars at risk of collision: " << BOLD << number_cars_risk_collision << RESET << endl;
            // informations about all cars
            for (int j = 0; j < all_cars_info.size(); j++){
                if (all_cars_info[j].collision_status == 2){
                    std::cout << YELLOW;
                    get_data(j, all_cars_info);
                    std::cout << WHITE;
                } else if (all_cars_info[j].collision_status == 1){
                    std::cout << ORANGE;
                    get_data(j, all_cars_info);
                    std::cout << WHITE;
                } else if (all_cars_info[j].is_over_speed_limit == true){
                    std::cout << RED;
                    get_data(j, all_cars_info);
                    std::cout << WHITE;
                } else{
                    get_data(j, all_cars_info);
                }

                if (j==8){
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            cout << "\033[16A\033[16J";            
        };

        // função para printar os dados quando o mock estiver desligado
        void print_offline() { 
            cout << RETURN;
            cout << ITALIC << "- All data available were shown -" << RESET << endl;
            print();
        }

        void update(roads *roads_obj){
            this->number_roads = roads_obj->get_roads_count();
            this->number_cars = roads_obj->get_cars_count();
            this->number_cars_over_speed_limit = roads_obj->get_speeding_cars_count();
            this->number_cars_risk_collision = roads_obj->get_collision_risk_cars_count();
            this->all_cars_info = roads_obj->get_all_cars_info();
        };

};

#endif // DASHBOARD_CLASS_CPP