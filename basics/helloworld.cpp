#include <iostream>
#include <thread>

int main(){

    std::thread t = std::thread([](){
        std::cout << "Hello World from thread" << std::endl;
    });

    t.join();

    std::cout << "Bye\n" << std::endl;

    return 0;
}