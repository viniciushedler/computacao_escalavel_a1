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
void update_number_roads(long long int *property, long long int *tm, long long int interval, roads *roads_obj) {
    while (true) {
        *property = roads_obj->number_of_roads;
        *tm = roads_obj->time_to_number_of_roads;
        this_thread::sleep_for(chrono::milliseconds(interval));
    }
}
void update_number_cars(long long int *property, long long int *tm, long long int interval, roads *roads_obj) {
    while (true) {
        *property = roads_obj->number_of_cars;
        *tm = roads_obj->time_to_number_of_cars;
        this_thread::sleep_for(chrono::milliseconds(interval));
    }
}
void update_number_cars_over_speed_limit(long long int *property, long long int *tm, long long int interval, roads *roads_obj) {
    while (true) {
        *property = roads_obj->number_of_cars_over_speed_limit;
        *tm = roads_obj->time_to_number_of_cars_over_speed_limit;
        this_thread::sleep_for(chrono::milliseconds(interval));
    }
}
void update_number_cars_risk_collision(long long int *property, long long int *tm, long long int interval, roads *roads_obj) {
    while (true) {
        *property = roads_obj->number_of_cars_with_collision_risk;
        *tm = roads_obj->time_to_number_of_cars_with_collision_risk;
        this_thread::sleep_for(chrono::milliseconds(interval));
    }
}
void update_all_cars_info(vector<car> *property, long long int *tm, long long int interval, roads *roads_obj) {
    while (true) {
        *property = roads_obj->all_cars_info;
        *tm = roads_obj->time_to_all_cars_info;
        this_thread::sleep_for(chrono::milliseconds(interval));
    }
}

class dashboard{
    long long int number_roads;
    long long int number_cars;
    long long int number_cars_over_speed_limit;
    long long int number_cars_risk_collision;
    vector<car> all_cars_info;
    long long int tm_number_roads;
    long long int tm_number_cars;
    long long int tm_number_cars_over_speed_limit;
    long long int tm_number_cars_risk_collision;
    long long int tm_all_cars_info;

    // Variáveis para o tempo de execução
    long long int time1 = 0;
    long long int time2 = 0;
    long long int time3 = 0;
    long long int n_lines = 0;

    public:
        bool is_mock_on = false;

        dashboard(roads *roads_obj) {
            // Inicia as threads para atualizar as propriedades do dashboard
            thread t1(update_number_roads, &this->number_roads, &this->tm_number_roads, 1'000, roads_obj);
            thread t2(update_number_cars, &this->number_cars, &this->tm_number_cars, 1'000, roads_obj);
            thread t3(update_number_cars_over_speed_limit, &this->number_cars_over_speed_limit, &this->tm_number_cars_over_speed_limit, 20, roads_obj);
            thread t4(update_number_cars_risk_collision, &this->number_cars_risk_collision, &this->tm_number_cars_risk_collision, 10, roads_obj);
            thread t5(update_all_cars_info, &this->all_cars_info, &this->tm_all_cars_info, 500, roads_obj);
            t1.detach();
            t2.detach();
            t3.detach();
            t4.detach();
            t5.detach();
        };

        // Função para atualizar os tempos do dashboard
        void update_times(long long int time1, long long int time2, long long int n_lines) {
            this->time1 = time1;
            this->time2 = time2;
            this->time3 = time3;    
            this->n_lines = n_lines;
        }

        // Função para printar os dados dos carros
        void data_car(long long int i, vector<car> all_cars_info) {
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
        void print() {
            cout << BOLD << GREEN << "Dashboard" << RESET << endl;
            // cout << ITALIC << "Time to read file: " << this->time1 << 
            // "ms | Time to update data: " << this->time2 << 
            // "ms | Time to since emited data: " << this->time3 << 
            // "ms | N# of lines: " << this->n_lines << RESET << endl;
            cout << "N# of lines: " << this->n_lines << RESET << endl;

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

            // cout << "AQUI" << endl; // APAGAR

            // Faz uma tabela com os dados dos 6 primeiros carros
            cout << "\n" << ITALIC << "Updated in " << tm_all_cars_info << "ms" << RESET << endl;
            cout << BOLD << "Plate\t\tPosition\tSpeed\tAcc\tModel\tYear\tProprietary" << RESET << endl;
            // cout << "Tamanho da lista de carros:" << all_cars_info.size() << endl; // APAGAR
            // cout << "AQUI" << endl; // APAGAR
            for (long long int j = 0; (j < all_cars_info.size() && j < 6); j++){
                // cout << "AQUI" << endl; // APAGAR

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
            // cout << "AQUI" << endl; // APAGAR
            
            cout << "\033[2J\033[1;1H";
        };

        // Função para printar os dados quando o mock estiver desligado
        void print_offline() { 
            cout << RETURN;
            cout << ITALIC << "- All data available were shown -" << RESET << endl;
            print();
        }
};

void update_dashboard(dashboard* dashboard_obj, long long int interval) {
    while (true) {
        if (dashboard_obj->is_mock_on == true) {
            dashboard_obj->print();
        } else {
            dashboard_obj->print_offline();
        }
        this_thread::sleep_for(chrono::milliseconds(interval));
    }
}

#endif // DASHBOARD_CLASS_CPP