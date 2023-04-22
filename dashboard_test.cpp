#ifndef DASHBOARD_TEST_CPP
#define DASHBOARD_TEST_CPP

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace std;



int main() {
    for (int i = 0; i <300; i++) {
        std::cout << "\033[A\033[J\033[A\033[J\033[A\033[J";
        if (i % 3 == 0) {
            std::cout << "\033[31m";
            cout << 1 << 1 << 1 << 1 << 1 << endl;
            std::cout << "\033[0m";
        }
        else {
            cout << 1 << 1 << 1 << 1 << 1 << endl;
        }
        std::cout << "Número de rodovias presentes na simulação: " << 10 - i << std::endl;
        std::cout << "Número total de veículos presentes na simulação: " << 10 - i << std::endl;
        std::cout << "Veículos acima do limite de velocidade: " << 10 - i << std::endl;
        
        // std::cout << "Count: " << 10 - i << "\r";
        // std::cout.flush();
        sleep(0.1);
        // system("cls");
    }
    return 0;
};

#endif // DASHBOARD_TEST_CPP
