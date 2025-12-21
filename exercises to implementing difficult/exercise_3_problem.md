# Exercise 3: Thread-Safe Data Structure

## Problem Statement

Implement a thread-safe queue data structure using modern C++ concurrency primitives. The queue should support multiple producers and multiple consumers, providing safe concurrent access without data races or deadlocks.

## Requirements

1. **Thread-Safe Queue Interface:**
   - `push(T item)` - Add an item to the queue (thread-safe)
   - `pop()` - Remove and return an item from the queue (thread-safe, blocking)
   - `try_pop()` - Try to remove an item, return false if queue is empty (non-blocking)
   - `empty()` - Check if queue is empty (thread-safe)
   - `size()` - Get current size of queue (thread-safe)

2. **Concurrency Requirements:**
   - Support multiple producer threads adding items concurrently
   - Support multiple consumer threads removing items concurrently
   - Use mutexes to protect shared data
   - Use condition variables for efficient blocking operations
   - Prevent data races and ensure memory safety

3. **Implementation Details:**
   - Use `std::mutex` for synchronization
   - Use `std::lock_guard` or `std::unique_lock` for RAII locking
   - Use `std::condition_variable` for blocking waits
   - Implement proper exception safety

4. **Advanced Features (Optional but Recommended):**
   - Support timeout for `pop()` operations
   - Support move semantics for efficiency (`push(T&& item)`)
   - Provide both blocking and non-blocking variants
   - Optional: Implement lock-free version using atomics (advanced)

## Example Usage

```cpp
ThreadSafeQueue<int> queue;

// Producer thread
std::thread producer([&queue]() {
    for (int i = 0; i < 10; ++i) {
        queue.push(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
});

// Consumer threads
std::vector<std::thread> consumers;
for (int i = 0; i < 3; ++i) {
    consumers.emplace_back([&queue, i]() {
        int value;
        while (queue.try_pop(value)) {
            std::cout << "Consumer " << i << " got: " << value << std::endl;
        }
    });
}

producer.join();
for (auto& t : consumers) t.join();
```

## Constraints

- Must use C++17 or later
- No data races or undefined behavior
- Proper exception safety (basic guarantee minimum)
- Efficient locking (minimize lock contention)
- Handle empty queue cases gracefully

## Hints

- Use `std::deque` or `std::queue` as underlying container
- Lock only when necessary (fine-grained locking)
- Use `condition_variable::wait` with predicate to avoid spurious wakeups
- Consider using `std::optional<T>` for return values
- Use `std::atomic` for simple counters if needed
- Remember: condition variables require a lock to be held

## Key Concepts to Demonstrate

- Mutex-based synchronization
- Condition variables for signaling
- RAII lock management
- Producer-consumer patterns
- Thread safety guarantees
- Avoiding deadlocks and race conditions

## Testing Considerations

- Test with single producer, single consumer
- Test with multiple producers and consumers
- Test blocking behavior (empty queue)
- Test concurrent access (stress test)
- Verify no data loss or corruption
- Measure performance under contention

## Safety Notes

- Always use RAII locks (`lock_guard`, `unique_lock`)
- Never hold locks longer than necessary
- Use condition variable predicates to prevent spurious wakeups
- Be careful with exception safety when holding locks

