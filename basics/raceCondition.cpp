#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

int main(){

	int counter = 0;

	thread tA = thread([&](){
		int lcounter = 0;
		while( lcounter < 100000 ){
			counter++, lcounter++;
		}
	});

	int lcounter = 0;
	while( lcounter < 100000 ){
		counter++, lcounter++;
	}
	
	tA.join();

	cout << "over " << counter << "\n";

	return 0;
}