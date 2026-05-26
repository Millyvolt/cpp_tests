#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <optional>
#include <chrono>
#include <atomic>

// ============================================================================
// Thread-Safe Queue Implementation
// ============================================================================

template<typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex mutex_;
    std::queue<T> queue_;
    std::condition_variable condition_;

public:
    ThreadSafeQueue() = default;

    // Delete copy constructor and copy assignment
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    // Push item (copy)
    void push(const T& item) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(item);
        }
        condition_.notify_one();
    }

    // Push item (move)
    void push(T&& item) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(item));
        }
        condition_.notify_one();
    }

    // Pop with blocking wait
    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // Wait until queue is not empty
        // Using predicate to avoid spurious wakeups
        condition_.wait(lock, [this] { return !queue_.empty(); });
        
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    // Try pop without blocking
    bool try_pop(T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (queue_.empty()) {
            return false;
        }
        
        item = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    // Try pop with optional (C++17)
    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (queue_.empty()) {
            return std::nullopt;
        }
        
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    // Pop with timeout
    template<typename Rep, typename Period>
    std::optional<T> pop_timeout(const std::chrono::duration<Rep, Period>& timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (condition_.wait_for(lock, timeout, [this] { return !queue_.empty(); })) {
            T item = std::move(queue_.front());
            queue_.pop();
            return item;
        }
        
        return std::nullopt;
    }

    // Check if queue is empty
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    // Get size
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    // Notify all waiting threads (useful for shutdown)
    void notify_all() {
        condition_.notify_all();
    }
};

// ============================================================================
// Thread-Safe Counter for Statistics
// ============================================================================

class ThreadSafeCounter {
private:
    std::atomic<int> count_{0};

public:
    void increment() {
        count_.fetch_add(1, std::memory_order_relaxed);
    }

    int get() const {
        return count_.load(std::memory_order_relaxed);
    }

    void reset() {
        count_.store(0, std::memory_order_relaxed);
    }
};

// ============================================================================
// Test Functions
// ============================================================================

void test_single_producer_single_consumer() {
    std::cout << "=== Test 1: Single Producer, Single Consumer ===\n";
    
    ThreadSafeQueue<int> queue;
    std::vector<int> consumed;

    std::thread producer([&queue]() {
        for (int i = 0; i < 10; ++i) {
            queue.push(i);
            std::cout << "  Produced: " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    std::thread consumer([&queue, &consumed]() {
        for (int i = 0; i < 10; ++i) {
            int value = queue.pop();
            consumed.push_back(value);
            std::cout << "  Consumed: " << value << std::endl;
        }
    });

    producer.join();
    consumer.join();

    std::cout << "  Consumed items: ";
    for (int val : consumed) {
        std::cout << val << " ";
    }
    std::cout << std::endl << std::endl;
}

void test_multiple_producers_multiple_consumers() {
    std::cout << "=== Test 2: Multiple Producers, Multiple Consumers ===\n";
    
    ThreadSafeQueue<int> queue;
    ThreadSafeCounter produced_count;
    ThreadSafeCounter consumed_count;
    const int items_per_producer = 20;
    const int num_producers = 3;
    const int num_consumers = 4;

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    // Start producers
    for (int i = 0; i < num_producers; ++i) {
        producers.emplace_back([&queue, &produced_count, i, items_per_producer]() {
            for (int j = 0; j < items_per_producer; ++j) {
                int value = i * items_per_producer + j;
                queue.push(value);
                produced_count.increment();
            }
        });
    }

    // Start consumers
    for (int i = 0; i < num_consumers; ++i) {
        consumers.emplace_back([&queue, &consumed_count, i]() {
            int value;
            while (consumed_count.get() < num_producers * items_per_producer) {
                if (queue.try_pop(value)) {
                    consumed_count.increment();
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        });
    }

    // Wait for all producers
    for (auto& t : producers) {
        t.join();
    }

    // Wait for all consumers
    for (auto& t : consumers) {
        t.join();
    }

    std::cout << "  Produced: " << produced_count.get() << " items\n";
    std::cout << "  Consumed: " << consumed_count.get() << " items\n";
    std::cout << "  Queue size: " << queue.size() << std::endl << std::endl;
}

void test_non_blocking_operations() {
    std::cout << "=== Test 3: Non-Blocking Operations ===\n";
    
    ThreadSafeQueue<std::string> queue;

    // Try pop from empty queue
    std::optional<std::string> result = queue.try_pop();
    std::cout << "  Try pop from empty queue: " 
              << (result.has_value() ? result.value() : "empty") << std::endl;

    // Push some items
    queue.push("first");
    queue.push("second");
    queue.push("third");

    // Try pop with optional
    while (auto item = queue.try_pop()) {
        std::cout << "  Popped: " << item.value() << std::endl;
    }

    std::cout << "  Queue empty: " << (queue.empty() ? "yes" : "no") << std::endl << std::endl;
}

void test_timeout() {
    std::cout << "=== Test 4: Timeout Operations ===\n";
    
    ThreadSafeQueue<int> queue;

    // Try pop with timeout (should timeout)
    auto start = std::chrono::steady_clock::now();
    auto result = queue.pop_timeout(std::chrono::milliseconds(100));
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "  Timeout pop (empty queue): " 
              << (result.has_value() ? std::to_string(result.value()) : "timeout") 
              << " (elapsed: " << elapsed.count() << "ms)" << std::endl;

    // Push item and try again
    queue.push(42);
    result = queue.pop_timeout(std::chrono::milliseconds(100));
    std::cout << "  Timeout pop (with item): " 
              << (result.has_value() ? std::to_string(result.value()) : "timeout") 
              << std::endl << std::endl;
}

void test_move_semantics() {
    std::cout << "=== Test 5: Move Semantics ===\n";
    
    ThreadSafeQueue<std::vector<int>> queue;

    // Push with move
    std::vector<int> vec{1, 2, 3, 4, 5};
    std::cout << "  Original vector size: " << vec.size() << std::endl;
    
    queue.push(std::move(vec));
    std::cout << "  Vector size after move: " << vec.size() << " (should be 0)" << std::endl;

    // Pop with move
    std::vector<int> popped = queue.pop();
    std::cout << "  Popped vector size: " << popped.size() << std::endl;
    std::cout << "  Popped vector contents: ";
    for (int val : popped) {
        std::cout << val << " ";
    }
    std::cout << std::endl << std::endl;
}

void test_concurrent_stress() {
    std::cout << "=== Test 6: Concurrent Stress Test ===\n";
    
    ThreadSafeQueue<int> queue;
    const int num_threads = 10;
    const int items_per_thread = 100;
    std::atomic<int> total_produced{0};
    std::atomic<int> total_consumed{0};

    std::vector<std::thread> threads;

    auto start = std::chrono::steady_clock::now();

    // Create producer-consumer threads
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&queue, &total_produced, &total_consumed, i, items_per_thread]() {
            // Produce items
            for (int j = 0; j < items_per_thread; ++j) {
                queue.push(i * items_per_thread + j);
                total_produced.fetch_add(1);
            }

            // Consume items
            int consumed = 0;
            while (consumed < items_per_thread) {
                int value;
                if (queue.try_pop(value)) {
                    consumed++;
                    total_consumed.fetch_add(1);
                } else {
                    std::this_thread::yield();
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "  Threads: " << num_threads << "\n";
    std::cout << "  Items per thread: " << items_per_thread << "\n";
    std::cout << "  Total produced: " << total_produced.load() << "\n";
    std::cout << "  Total consumed: " << total_consumed.load() << "\n";
    std::cout << "  Final queue size: " << queue.size() << "\n";
    std::cout << "  Time elapsed: " << elapsed.count() << "ms\n";
    std::cout << "  All items processed: " << (total_produced == total_consumed ? "yes" : "no") << std::endl << std::endl;
}

int main() {
    std::cout << "=== Exercise 3: Thread-Safe Data Structure ===\n\n";

    test_single_producer_single_consumer();
    test_multiple_producers_multiple_consumers();
    test_non_blocking_operations();
    test_timeout();
    test_move_semantics();
    test_concurrent_stress();

    std::cout << "All tests completed!\n";
    return 0;
}

