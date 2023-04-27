#include <iostream>
#include <mutex>
#include <thread>
#include <iomanip>

#include "ThreadPool.h"

int main() {
    ThreadPool pool(4);
    std::mutex mtx;
    int task_nums = 10000;
    int count = 0;
    for (int i = 0; i < task_nums; ++i) {
        pool.add_task([i, &mtx, &count]() {
//            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::unique_lock<std::mutex> lock(mtx);
            count++;
            std::cout << "Task: " << std::setw(2) << i << " is done!" << "Thread ID: " << std::this_thread::get_id() << std::endl;
            });
    }
    while (count < task_nums) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::cout << count << std::endl;
    return 0;
}
