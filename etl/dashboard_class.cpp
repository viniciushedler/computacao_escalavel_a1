#ifndef DASHBOARD_CLASS_CPP
#define DASHBOARD_CLASS_CPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>


#include "car_classes.cpp"

using namespace std;

class dashboard{
    int number_roads;
    int number_cars;
    int number_cars_over_speed_limit;
    int number_cars_risk_collision;
    vector<car> all_cars_info;

    public:
        dashboard(roads *roads_obj) {
            this->number_roads = roads_obj->get_roads_count();
            this->number_cars = roads_obj->get_cars_count();
            this->number_cars_over_speed_limit = roads_obj->get_speeding_cars_count();
            this->number_cars_risk_collision = roads_obj->get_collision_risk_cars_count();
            this->all_cars_info = roads_obj->get_all_cars_info();
        };

        // função para pegar os dados que serão printados
        void get_data(int i, vector<car> all_cars_info){
            if (all_cars_info[i].with_external_service_info == false){
                cout << all_cars_info[i].plate << " " << "(" << all_cars_info[i].position.x << ", " 
                << all_cars_info[i].position.y << ")" << " " << all_cars_info[i].speed << " " 
                << all_cars_info[i].acceleration << " " << all_cars_info[i].propietary << " " 
                << all_cars_info[i].model << " " << all_cars_info[i].year << endl;
            }
            else{
                cout << all_cars_info[i].plate << " " << "(" << all_cars_info[i].position.x << ", " 
                << all_cars_info[i].position.y << ")" << " " << all_cars_info[i].speed << " " 
                << all_cars_info[i].acceleration << " " << endl;
            }
        };

        // função para printar os dados 
        void print(){
            std::cout << "\033[A\033[J\033[A\033[J\033[A\033[J\033[A\033[J";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            // informations about all cars
            for (int j = 0; j < all_cars_info.size(); j++){
                if (all_cars_info[j].collision_status == 2){
                    std::cout << "\033[31m";
                    get_data(j, all_cars_info);
                    std::cout << "\033[0m";
                } else if (all_cars_info[j].collision_status == 1){
                    std::cout << "\033[38;5;208m";
                    get_data(j, all_cars_info);
                    std::cout << "\033[0m";
                } else if (all_cars_info[j].is_over_speed_limit == true){
                    std::cout << "\033[33m";
                    get_data(j, all_cars_info);
                    std::cout << "\033[0m";
                } else{
                    get_data(j, all_cars_info);
                }
            }
            // informations about roads
            cout << "Number of roads: " << number_roads << endl;
            cout << "Number of cars: " << number_cars << endl;
            cout << "Number of cars over speed limit: " << number_cars_over_speed_limit << endl;
            cout << "Number of cars at risk of collision: " << number_cars_risk_collision << endl;
        };

        // void print(){
        //     dashboard dashboard_obj(&roads_obj);

        //     std::cout << "\033[A\033[J\033[A\033[J\033[A\033[J\033[A\033[J";
        //     std::this_thread::sleep_for(std::chrono::milliseconds(500));
        //     // informations about all cars
        //     for (int i = 0; i < all_cars_info.size(); i++){
        //         if (all_cars_info[i].is_over_speed_limit == true){
        //             std::cout << "\033[31m";
        //             dashboard_obj.get_data(i, all_cars_info);
        //             std::cout << "\033[0m";

        //             // if (all_cars_info[i].with_external_service_info == false){
        //             //     std::cout << "\033[31m";
        //             //     cout << all_cars_info[i].plate << " " << "(" << all_cars_info[i].position.x << ", " 
        //             //     << all_cars_info[i].position.y << ")" << " " << all_cars_info[i].speed << " " 
        //             //     << all_cars_info[i].acceleration << " " << all_cars_info[i].propietary << " " 
        //             //     << all_cars_info[i].model << " " << all_cars_info[i].year << endl;
        //             //     std::cout << "\033[0m";
        //             // }
        //             // else{
        //             //     cout << all_cars_info[i].plate << " " << "(" << all_cars_info[i].position.x << ", " 
        //             //     << all_cars_info[i].position.y << ")" << " " << all_cars_info[i].speed << " " 
        //             //     << all_cars_info[i].acceleration << " " << endl;
        //             // }
        //         }
                
        //         else{
        //             if (all_cars_info[i].propietary != ""){
        //                 cout << all_cars_info[i].plate << " " << "(" << all_cars_info[i].position.x << ", " 
        //                 << all_cars_info[i].position.y << ")" << " " << all_cars_info[i].speed << " " 
        //                 << all_cars_info[i].acceleration << " " << all_cars_info[i].propietary << " " 
        //                 << all_cars_info[i].model << " " << all_cars_info[i].year << endl;
        //             }
        //             else{
        //                 cout << all_cars_info[i].plate << " " << "(" << all_cars_info[i].position.x << ", " 
        //                 << all_cars_info[i].position.y << ")" << " " << all_cars_info[i].speed << " " 
        //                 << all_cars_info[i].acceleration << endl;
        //             }
        //         }
        //     };
        //     cout << "Número de rodovias presentes na simulação: " << number_roads << endl;
        //     cout << "Número total de veículos presentes na simulação: " << number_cars << endl;
        //     cout << "Veículos acima do limite de velocidade: " << number_cars_over_speed_limit << endl;
        //     cout << "Veículos em risco de colisão: " << number_cars_risk_collision << endl;
        //     };
};

#endif // DASHBOARD_CLASS_CPP