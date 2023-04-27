//
// Created by Cheng MingBo on 2023/4/27.
//

#ifndef UNTITLED5_THREADSAFESET_H
#define UNTITLED5_THREADSAFESET_H
#include <iostream>
#include <set>
#include <mutex>
#include <shared_mutex>

template <typename T>
class ThreadSafeSet {
  public:
    // 添加元素
    void insert(const T& value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        data_.insert(value);
    }
    
    // 删除元素
    void erase(const T& value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        data_.erase(value);
    }
    
    // 查找元素
    bool find(const T& value) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return data_.find(value) != data_.end();
    }
    
    // 检查集合是否为空
    bool empty() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return data_.empty();
    }
    
    // 获取集合大小
    std::size_t size() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return data_.size();
    }
    
    // 获取集合的开始迭代器
    typename std::set<T>::iterator begin() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        return data_.begin();
    }
    
    // 获取集合的结束迭代器
    typename std::set<T>::iterator end() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        return data_.end();
    }
    
    // 获取集合的第一个元素
    T first() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (!data_.empty()) {
            return *data_.begin();
        }
        throw std::runtime_error("The set is empty.");
    }
    
    void update(std::size_t new_val, const T& value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        data_.erase(value);
        data_.insert({new_val, value.second});
    }
    
    // 清空集合
    void clear() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        data_.clear();
    }
  
  private:
    std::set<T> data_;
    mutable std::shared_mutex mutex_;
};

#endif //UNTITLED5_THREADSAFESET_H
