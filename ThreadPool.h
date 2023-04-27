//
// Created by Cheng MingBo on 2023/4/27.
//

#ifndef UNTITLED5_THREADPOOL_H
#define UNTITLED5_THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <set>
#include <iomanip>
#include <format>

#include "ThreadSafeQueue.h"
#include "ThreadSafeSet.h"

class ThreadPool {
  public:
    explicit ThreadPool(size_t num_threads)
            : stop(false), task_queues(num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([ this, i ] { worker_thread(i); });
            min_task_queues.insert({ 0, i });
            task_queues[ i ].set_stop(stop);
        }
    }
    
    ~ThreadPool() {
        stop = true;
        for (auto &que: task_queues) {
            que.stop();
        }
        for (auto &worker: workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
    
    void add_task(const std::function<void()> &task) {
        auto min_queue = min_task_queues.first();
        std::size_t min_queue_index = min_queue.second;
        
        std::size_t task_count = task_queues[ min_queue_index ].size();
        min_task_queues.update(task_count + 1, min_queue);
        task_queues[ min_queue_index ].push(task);
    }
  
  private:
    void worker_thread(size_t thread_index) {
        while (!stop.load()) {
            thread_local std::function<void()> task;
            
            if (task_queues[ thread_index ].wait_and_pop(task)) {
                if (min_task_queues.find({ task_queues[ thread_index ].size() + 1, thread_index })) {
                    min_task_queues.update(task_queues[ thread_index ].size(), { task_queues[ thread_index ].size() + 1, thread_index });
                }
            }
            else {
                continue;
            }
            
            task();
            steal_task(thread_index);
        }
    }
    
    void steal_task(size_t thread_index) {
        if (task_queues[ thread_index ].empty()) {
            for (size_t i = 1; i < task_queues.size(); ++i) {
                size_t target_index = (thread_index + i) % task_queues.size();
                if (!task_queues[ target_index ].empty()) {
                    size_t tasks_to_steal = task_queues[ target_index ].size() / 2;
                    for (size_t j = 0; j < tasks_to_steal; ++j) {
                        std::function<void()> task;
                        task_queues[ target_index ].try_pop(task);
                        if (task) {
                            task_queues[ thread_index ].push(task);
                        }
                    }
                    if (min_task_queues
                            .find({ task_queues[ target_index ].size() + tasks_to_steal, target_index })) {
                        min_task_queues.update(task_queues[ target_index ].size(), { task_queues[ target_index ].size() + tasks_to_steal, target_index });
                    }
                    
                    if (min_task_queues
                            .find({ task_queues[ thread_index ].size() - tasks_to_steal, thread_index })) {
                        min_task_queues.update(task_queues[ thread_index ].size(), { task_queues[ thread_index ].size() - tasks_to_steal, thread_index });
                    }
                    
                    break;
                }
            }
        }
    }
    
    std::vector<std::thread> workers;
    std::vector<ThreadSafeQueue<std::function<void()>>> task_queues;
    ThreadSafeSet<std::pair<std::size_t, std::size_t>> min_task_queues;
    std::atomic<bool> stop;
};

#endif //UNTITLED5_THREADPOOL_H
