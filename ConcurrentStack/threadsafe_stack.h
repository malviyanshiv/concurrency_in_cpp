#ifndef THREADSAFE_STACK
#define THREADSAFE_STACK
#include <stack>
#include <memory>
#include <mutex>
#include <exception>

class threadsafe_stack_empty_exception : public std::exception {
    const char* what() const throw(){
        return "threadsafe_stack is empty";
    }
};

template<typename T, typename Container = std::deque<T> >
class threadsafe_stack{
    public:
        threadsafe_stack(){}
        threadsafe_stack(const threadsafe_stack& other){
            std::lock_guard<std::mutex> lc(m);
            data = other.data;
        }

        threadsafe_stack& operator=(const threadsafe_stack&) = delete;

        void push(T& value){
            std::lock_guard<std::mutex> lc(m);
            data.push(value);
        }

        void push(T&& value){
            std::lock_guard<std::mutex> lc(m);
            data.push(std::move(value));
        }

        bool empty() const{
            std::lock_guard<std::mutex> lc(m);
            return data.empty();
        }
        size_t size() const{
            std::lock_guard<std::mutex> lc(m);
            return data.size();
        }

        T top() const{
            std::lock_guard<std::mutex> lc(m);
            return data.top();
        }

        std::shared_ptr<T> pop(){
            std::lock_guard<std::mutex> lc(m);
            if( data.empty() ){
                throw threadsafe_stack_empty_exception();
            }
            std::shared_ptr<T> const tp = std::make_shared(data.top());
            data.pop();

            return tp;
        }
        void pop(T& value){
            std::lock_guard<std::mutex> lc(m);
            if( data.empty() ){
                throw threadsafe_stack_empty_exception();
            }
            value = data.top();
            data.pop();
        }

    private:
        std::stack<T> data;
        mutable std::mutex m;
};
#endif