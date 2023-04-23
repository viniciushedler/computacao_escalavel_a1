#ifndef DASHBOARD_CLASS_CPP
#define DASHBOARD_CLASS_CPP

#include <iostream>
#include <string>
#include <vector>
// #include <algorithm>
#include <chrono>
#include <iomanip>


#include "car_classes.cpp"

using namespace std;

class dashboard{
    int number_roads;
    int number_cars;
    int number_cars_over_speed_limit;
    int number_cars_risk_collision;
    vector<car> all_cars_info;
    string yellow;
    string orange;
    string red;
    string no_color;


    public:
        dashboard(roads *roads_obj) {
            this->number_roads = roads_obj->get_roads_count();
            this->number_cars = roads_obj->get_cars_count();
            this->number_cars_over_speed_limit = roads_obj->get_speeding_cars_count();
            this->number_cars_risk_collision = roads_obj->get_collision_risk_cars_count();
            this->all_cars_info = roads_obj->get_all_cars_info();
            this->orange = "\033[38;5;208m";
            this->yellow = "\033[33m";
            this->red = "\033[31m";
            this->no_color = "\033[0m";
        };

        // função para pegar os dados que serão printados
        void get_data(int i, vector<car> all_cars_info){
            if (all_cars_info[i].with_external_service_info == true){

                // Faz uma tabela com os dados                
                cout << all_cars_info[i].plate << setw(8)  << left << all_cars_info[i].position
                << setw(8)  << all_cars_info[i].speed  << setw(8) 
                << all_cars_info[i].acceleration << setw(8)  << all_cars_info[i].model 
                 << setw(8)  << all_cars_info[i].year  << setw(8) << all_cars_info[i].propietary  << endl;
            }
            else{
                cout << all_cars_info[i].plate  << setw(8) <<  << setw(8) << all_cars_info[i].speed  << setw(8) 
                << all_cars_info[i].acceleration << endl;
            }
        };

        // função para printar os dados 
        void print(){
            cout << "Number of roads: " << number_roads << endl;
            cout << "Number of cars: " << number_cars << endl;
            cout << "Number of cars over speed limit: " << number_cars_over_speed_limit << endl;
            cout << "Number of cars at risk of collision: " << number_cars_risk_collision << endl;
            // cout << "Plate" << setw(8)  <<  "Position" << setw(8)
            //  <<  "Speed" << setw(20) <<  "Acceleration" << setw(20) << 
            // "Propietary" << setw(8) <<  "Model" <<setw(8) <<  "Year" << endl;
            // informations about cars
            for (int j = 0; j < all_cars_info.size(); j++){
                if (all_cars_info[j].collision_status == 2){
                    std::cout << yellow;
                    get_data(j, all_cars_info);
                    std::cout << no_color;
                } else if (all_cars_info[j].collision_status == 1){
                    std::cout << orange;
                    get_data(j, all_cars_info);
                    std::cout << no_color;
                } else if (all_cars_info[j].is_over_speed_limit == true){
                    std::cout << red;
                    get_data(j, all_cars_info);
                    std::cout << no_color;
                } else{
                    get_data(j, all_cars_info);
                }

                if (j==6){
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(80));
            // cout << "\033[12A\033[12J";  
            system("CLS")          
        };

        void update(roads *roads_obj){
            this->number_roads = roads_obj->get_roads_count();
            this->number_cars = roads_obj->get_cars_count();
            this->number_cars_over_speed_limit = roads_obj->get_speeding_cars_count();
            this->number_cars_risk_collision = roads_obj->get_collision_risk_cars_count();
            this->all_cars_info = roads_obj->get_all_cars_info();
        };

};

#endif // DASHBOARD_CLASS_CPP