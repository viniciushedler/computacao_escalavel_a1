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
#define ORANGE  "\033[38;2;255;165;0m"      /* Orange */
#define YELLOW  "\033[33m"      /* Yellow */
#define WHITE   "\033[37m"      /* White */
#define GREEN   "\033[32m"      /* Green */
#define BOLD    "\033[1m"       /* Bold */
#define ITALIC  "\033[3m"       /* Italic */
#define RETURN  "\x1B[2J\x1B[H"

#include "car_classes.cpp"

using namespace std;

// Funções de atualização das propriedades do dashboard
void update_number_roads(int *property, int *tm, int interval, roads *roads_obj) {
    while (true) {
        auto start = chrono::steady_clock::now();
        *property = roads_obj->get_roads_count();
        this_thread::sleep_for(chrono::milliseconds(interval));
        auto end = chrono::steady_clock::now();
        *tm = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    }
}
void update_number_cars(int *property, int *tm, int interval, roads *roads_obj) {
    while (true) {
        auto start = chrono::steady_clock::now();
        *property = roads_obj->get_cars_count();
        this_thread::sleep_for(chrono::milliseconds(interval));
        auto end = chrono::steady_clock::now();
        *tm = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    }
}
void update_number_cars_over_speed_limit(int *property, int *tm, int interval, roads *roads_obj) {
    while (true) {
        auto start = chrono::steady_clock::now();
        *property = roads_obj->get_speeding_cars_count();
        this_thread::sleep_for(chrono::milliseconds(interval));
        auto end = chrono::steady_clock::now();
        *tm = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    }
}
void update_number_cars_risk_collision(int *property, int *tm, int interval, roads *roads_obj) {
    while (true) {
        auto start = chrono::steady_clock::now();
        *property = roads_obj->get_collision_risk_cars_count();
        this_thread::sleep_for(chrono::milliseconds(interval));
        auto end = chrono::steady_clock::now();
        *tm = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    }
}
void update_all_cars_info(vector<car> *property, int *tm, int interval, roads *roads_obj) {
    while (true) {
        auto start = chrono::steady_clock::now();
        *property = roads_obj->get_all_cars_info();
        this_thread::sleep_for(chrono::milliseconds(interval));
        auto end = chrono::steady_clock::now();
        *tm = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    }
}

class dashboard{
    int number_roads;
    int number_cars;
    int number_cars_over_speed_limit;
    int number_cars_risk_collision;
    vector<car> all_cars_info;
    int tm_number_roads;
    int tm_number_cars;
    int tm_number_cars_over_speed_limit;
    int tm_number_cars_risk_collision;
    int tm_all_cars_info;

    public:
        dashboard(roads *roads_obj) {
            thread t1(update_number_roads, &this->number_roads, &this->tm_number_roads, 1'000, roads_obj);
            thread t2(update_number_cars, &this->number_cars, &this->tm_number_cars, 1'000, roads_obj);
            thread t3(update_number_cars_over_speed_limit, &this->number_cars_over_speed_limit, &this->tm_number_cars_over_speed_limit, 50, roads_obj);
            thread t4(update_number_cars_risk_collision, &this->number_cars_risk_collision, &this->tm_number_cars_risk_collision, 10, roads_obj);
            thread t5(update_all_cars_info, &this->all_cars_info, &this->tm_all_cars_info, 1'000, roads_obj);
            t1.detach();
            t2.detach();
            t3.detach();
            t4.detach();
            t5.detach();
        };

        // Função para printar os dados dos carros
        void data_car(int i, vector<car> all_cars_info){
            if (all_cars_info[i].with_external_service_info == true){
                cout << all_cars_info[i].plate << "\t\t"
                << all_cars_info[i].position << "  " << "\t" 
                << all_cars_info[i].speed  << "\t" 
                << all_cars_info[i].acceleration << "\t"   
                << all_cars_info[i].model << "\t"  
                << all_cars_info[i].year  << "\t" 
                << all_cars_info[i].propietary << endl;
            }
            else{
                cout << all_cars_info[i].plate << "\t\t"
                << all_cars_info[i].position << "  " << "\t"
                << all_cars_info[i].speed  << "\t" 
                << all_cars_info[i].acceleration << endl;
            }
        };

        // Função para printar os dados 
        void print(){
            cout << BOLD << WHITE << "Dashboard" << RESET << endl;

            // Número de estradas
            cout << "Number of roads: " << BOLD << number_roads << RESET 
            << "\t\t\t\t" << ITALIC << "Updated in " << tm_number_roads << "ms" << RESET << endl;

            // Número de carros
            cout << "Number of cars: " << BOLD << number_cars << RESET  
            << "\t\t\t\t" << ITALIC << "Updated in " << tm_number_cars << "ms" << RESET << endl;

            // Número de carros acima do limite de velocidade
            cout << "Number of cars over speed limit: " << BOLD << number_cars_over_speed_limit << RESET 
            << "\t\t" << ITALIC << "Updated in " << tm_number_cars_over_speed_limit << "ms" << RESET << endl;

            // Número de carros em risco de colisão
            cout << "Number of cars at risk of collision: " << BOLD << number_cars_risk_collision << RESET 
            << "\t\t" << ITALIC << "Updated in " << tm_number_cars_risk_collision << "ms" << RESET << endl;

            // Faz uma tabela com os dados dos 6 primeiros carros
            cout << "\n" << ITALIC << "Updated in " << tm_all_cars_info << "ms" << RESET << endl;
            cout << "Plate\t\tPosition\tSpeed\tAcc\tModel\tYear\tProprietary" << endl;
            for (int j = 0; (j < all_cars_info.size() && j < 6); j++){

                if (all_cars_info[j].collision_status == 2){
                    std::cout << RED;
                    data_car(j, all_cars_info);
                    std::cout << RESET;
                } else if (all_cars_info[j].collision_status == 1){
                    std::cout << ORANGE;
                    data_car(j, all_cars_info);
                    std::cout << RESET;
                } else if (all_cars_info[j].is_over_speed_limit == true){
                    std::cout << YELLOW;
                    data_car(j, all_cars_info);
                    std::cout << RESET;
                } else{
                    data_car(j, all_cars_info);
                }
            }
            // std::this_thread::sleep_for(std::chrono::milliseconds(80));
            cout << "\033[2J\033[1;1H";
        };

        // Função para printar os dados quando o mock estiver desligado
        void print_offline() { 
            cout << RETURN;
            cout << ITALIC << "- All data available were shown -" << RESET << endl;
            print();
        }

};

#endif // DASHBOARD_CLASS_CPP