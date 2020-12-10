#include <chrono>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

//dummy workable class acting as Iterator
template<typename T>
class MyWrapper{
    public:
    explicit MyWrapper(T vl): vl_(vl){}
    explicit MyWrapper(): MyWrapper((T)0){}

    //toT converter
    operator T() const{
        return vl_;
    } 

    //addition operator a = 5 + b
    template<typename LHS>
    inline friend LHS operator+(LHS lhs, MyWrapper rhs){
        return lhs + rhs.vl_;
    }

    inline friend MyWrapper operator+(MyWrapper lhs, MyWrapper rhs){
        return MyWrapper<T>(lhs.vl_ + rhs.vl_);
    }

    inline friend void operator++(MyWrapper& MyWrapper){
        MyWrapper.vl_++;
    }

    inline friend bool operator==(MyWrapper lhs, MyWrapper rhs){
        return lhs.vl_ == rhs.vl_;
    }

    inline friend bool operator!=(MyWrapper lhs, MyWrapper rhs){
        return !(lhs == rhs);
    }

    MyWrapper operator*(){
        return *this;
    }

    private:
    T vl_;
};

template<typename T>
class MyRange{
    public:
    explicit MyRange(T lend, T rend): lend_(lend), rend_(rend){}

    MyWrapper<T> begin(){
        return MyWrapper<T>(lend_);
    }

    MyWrapper<T> end(){
        return MyWrapper<T>(rend_ + 1);
    }

    private:
    T lend_, rend_;
};

template<typename T>
class ParallelAccumulate{
    public:
    void operator()(MyRange<T> myRange, T& result){
        result = std::accumulate(myRange.begin(), myRange.end(), result);
    }
};


int main(){
    
    //variables declarations
    int64_t start = 1, end = 3000000000, tcount = std::thread::hardware_concurrency();
    int64_t left = start, right = end/tcount, size = end/tcount;
    std::vector<std::thread> threadPool(tcount-1); //current thread will act as 1 thread
    std::vector<int64_t> results(tcount, 0);
    ParallelAccumulate<int64_t> parallelAccumulate;

    auto parallelExecutionStartTime = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < tcount-1; i++){
        threadPool[i] = std::thread(parallelAccumulate, MyRange<int64_t>(left, right), std::ref(results[i]));
        left = right + 1;
        right = right + size;
    }

    MyRange<int64_t> myRange(left, end);
    results[tcount-1] = std::accumulate(myRange.begin(), myRange.end(), results[tcount-1]);

    for(int i = 0; i < tcount-1; i++)
        threadPool[i].join();
    
    auto parallelExecutionEndTime = std::chrono::high_resolution_clock::now();
    int64_t presult = 0;
    presult = std::accumulate(results.begin(), results.end(), presult);

    std::cout << "Parallel Execution took: " << std::chrono::duration_cast<std::chrono::milliseconds>
                                                (parallelExecutionEndTime - parallelExecutionStartTime).count()
              << " Milliseconds" << std::endl;
    std::cout << "Sum from " << start << " to " << end << " is: " << presult << std::endl;

    int64_t sresult = 0;
    MyRange<int64_t> sRange(start, end);
    auto serialExecutionStartTime = std::chrono::high_resolution_clock::now();
    sresult = std::accumulate(sRange.begin(), sRange.end(), sresult);
    auto serialExecutionEndTime = std::chrono::high_resolution_clock::now();

    std::cout << "Serial Execution took: " << std::chrono::duration_cast<std::chrono::milliseconds>
                                            (serialExecutionEndTime - serialExecutionStartTime).count()
                << " Milliseconds" << std::endl;
    std::cout << "Sum from " << start << " to " << end << " is: " << sresult << std::endl;

    return 0;
}