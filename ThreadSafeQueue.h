//
// Created by Cheng MingBo on 2023/4/27.
//

#ifndef UNTITLED5_THREADSAFEQUEUE_H
#define UNTITLED5_THREADSAFEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

template<typename T>
class ThreadSafeQueue {
  public:
    ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue&) = delete; // 禁用拷贝构造函数
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete; // 禁用赋值操作符
  
    void push(const T& e) {
        {
            std::lock_guard<std::mutex> lock(que_mtx);
            que.push(e);
        }
        que_cv.notify_one();
    }
    bool try_pop(T& e) {
        std::lock_guard<std::mutex> lock(que_mtx);
        if (que.empty()) {
            return false;
        }
        e = que.front();
        que.pop();
        return true;
    }
    
    bool wait_and_pop(T& e) {
        std::unique_lock<std::mutex> lock(que_mtx);
        que_cv.wait(lock, [this] { return !que.empty() || stop_; });
        if (!stop_) {
            e = que.front();
            que.pop();
            return true;
        }
        return false;
    }
    
    
    std::size_t size() {
        std::lock_guard<std::mutex> lock(que_mtx);
        return que.size();
    }
    
    bool empty() {
        std::lock_guard<std::mutex> lock(que_mtx);
        return que.empty();
    }
    
    void set_stop(bool stop) {
        stop_ = stop;
    }
    
    void stop() {
        stop_ = true;
        que_cv.notify_all();
    }
    
  private:
    std::queue<T> que;
    std::mutex que_mtx;
    std::atomic<bool> stop_ = true;
    std::condition_variable que_cv;
};

#endif //UNTITLED5_THREADSAFEQUEUE_H
