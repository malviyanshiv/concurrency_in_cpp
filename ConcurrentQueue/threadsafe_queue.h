#ifndef THREADSAFE_QUEUE
#define THREADSAFE_QUEUE
#include <mutex>
#include <queue>
#include <condition_variable>

template<typename T>
class threadsafe_queue{
    public:
        explicit threadsafe_queue(){}
        explicit threadsafe_queue(const threadsafe_queue& other){
            std::lock_guard<std::mutex> lk(mut);
            data = other.data;
        }

        void push(const T& value){
            {
                std::lock_guard<std::mutex> lk(mut);
                data.push(value);
            }
            cv.notify_one();
        }

        void push(T&& value){
            {
                std::lock_guard<std::mutex> lk(mut);
                data.push(value);
            }
            cv.notify_one();
        }

        bool try_pop(T& value){
            std::lock_guard<std::mutex> lk(mut);
            if( data.empty() ){
                return false;
            }
            value = data.front();
            data.pop();

            return true;
        }

        std::shared_ptr<T> try_pop(){
            std::lock_guard<std::mutex> lk(mut);
            if( data.empty() ){
                return nullptr;
            }
            std::shared_ptr<T> sp = std::make_shared<T>(data.front());
            data.pop();

            return sp;
        }

        void wait_and_pop(T& value){
            std::unique_lock<std::mutex> lk(mut);
            cv.wait(lk, [this]{ return !data.empty(); });
            value = data.front();
            data.pop();
        }

        std::shared_ptr<T> wait_and_pop(){
            std::unique_lock<std::mutex> lk(mut);
            cv.wait(lk, [this]{ return !data.empty(); });
            std::shared_ptr<T> sp = std::make_shared<T>(data.front());
            data.pop();

            return sp;
        }

        size_t size() const{
            std::lock_guard<std::mutex> lk(mut);
            return data.size();
        }

        bool empty() const{
            std::lock_guard<std::mutex> lk(mut);
            return data.empty();
        }
    private:
        std::queue<T> data;
        mutable std::mutex mut;
        std::condition_variable cv;
};
#endif