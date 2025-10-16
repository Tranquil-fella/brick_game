#pragma once

#include <memory>
#include <vector>

template <typename T>
class Observer;

template <typename T>
class Observable {
 public:
  virtual ~Observable() = default;

  void AddObserver(std::weak_ptr<Observer<T>> observer) {
    observers_.emplace_back(std::move(observer));
  }

  void RemoveObservers(std::shared_ptr<Observer<T>> observer) {
    observers_.clear();
  }

  void NotifyObservers() {
    cleanupExpired();
    for (auto& weak : observers_) {
      if (auto observer = weak.lock()) observer->Update(static_cast<T*>(this));
    }
  }

 private:
  void cleanupExpired() {
    std::erase_if(observers_, [](const auto& weak) { return weak.expired(); });
  }

  std::vector<std::weak_ptr<Observer<T>>> observers_;
};

template <typename T>
class Observer : public std::enable_shared_from_this<Observer<T>> {
 public:
  virtual ~Observer() = default;
  virtual void Update(T* observable) = 0;
};