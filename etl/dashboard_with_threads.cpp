#ifndef DASHBOARD_CLASS_2_CPP
#define DASHBOARD_CLASS_2_CPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>

// Define cores para o terminal
#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BOLD    "\033[1m"       /* Bold */
#define RETURN  "\x1B[2J\x1B[H"

using namespace std;

class dashboard{
    public:
    int a = 0;
    int b = 0;
    int c = 0;
    vector<int> all_cars_info;
};

dashboard dash;

// Função que atualiza a propridade de acordo com a prioridade
void update_dashboard(int *property, int interval) {
    while (true) {
        ++*property;
        this_thread::sleep_for(chrono::milliseconds(interval));
    }
}

int main () {
    thread t1(update_dashboard, &dash.a, 1000);
    thread t2(update_dashboard, &dash.b, 500);
    thread t3(update_dashboard, &dash.c, 10);
    t1.detach();
    t2.detach();
    t3.detach();

    for (int i = 0; i < 1000000000; i++){
        cout << RETURN;
        cout << BOLD << GREEN << "Dashboard" << RESET << endl;
        cout << BOLD;
        cout << "a: " << dash.a << endl;
        cout << "b: " << dash.b << endl;
        cout << "c: " << dash.c << endl;
        cout << RESET;
        cout << "i: " << i << endl;
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

#endif // DASHBOARD_CLASS_2_CPP