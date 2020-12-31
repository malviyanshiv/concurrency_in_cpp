#include "threadsafe_queue.h"
#include <thread>
#include <iostream>

threadsafe_queue<int> q;

void process(){
    int result = 0;
    std::cout << "requesting for data\n";
    q.wait_and_pop(result);
    std::cout << "processing " << result << std::endl;
    q.wait_and_pop(result);
    std::cout << "processing again " << result << std::endl;
}

void generatingData(){
    std::cout << "generating data" << std::endl;
    q.push(50);
    std::cout << "Data generated, leaving\n";
    q.push(55);
}
int main(){

    std::thread t1(process);
    std::thread t2(generatingData);
    t1.join();
    t2.join();
    return 0;
}