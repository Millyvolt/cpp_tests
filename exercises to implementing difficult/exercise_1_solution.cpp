#include <iostream>
#include <type_traits>
#include <utility>

// Helper type trait to detect if T is an array type
template<typename T>
struct is_array_type : std::false_type {};

template<typename T>
struct is_array_type<T[]> : std::true_type {};

template<typename T, size_t N>
struct is_array_type<T[N]> : std::true_type {};

// Primary template for single objects
template<typename T>
class UniquePtr {
private:
    T* ptr_;

public:
    // Type aliases
    using element_type = T;
    using pointer = T*;

    // Default constructor (nullptr)
    explicit UniquePtr(pointer ptr = nullptr) noexcept : ptr_(ptr) {}

    // Move constructor
    UniquePtr(UniquePtr&& other) noexcept : ptr_(other.release()) {}

    // Move assignment operator
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    // Delete copy constructor and copy assignment
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // Destructor
    ~UniquePtr() {
        if (ptr_) {
            delete ptr_;
        }
    }

    // Dereference operators for single objects
    T& operator*() const noexcept {
        return *ptr_;
    }

    pointer operator->() const noexcept {
        return ptr_;
    }

    // Get raw pointer
    pointer get() const noexcept {
        return ptr_;
    }

    // Release ownership
    pointer release() noexcept {
        pointer old = ptr_;
        ptr_ = nullptr;
        return old;
    }

    // Reset pointer
    void reset(pointer ptr = nullptr) noexcept {
        if (ptr_ != ptr) {
            delete ptr_;
            ptr_ = ptr;
        }
    }

    // Boolean conversion
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }
};

// Partial specialization for arrays
template<typename T>
class UniquePtr<T[]> {
private:
    T* ptr_;

public:
    // Type aliases
    using element_type = T;
    using pointer = T*;

    // Default constructor (nullptr)
    explicit UniquePtr(pointer ptr = nullptr) noexcept : ptr_(ptr) {}

    // Move constructor
    UniquePtr(UniquePtr&& other) noexcept : ptr_(other.release()) {}

    // Move assignment operator
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    // Delete copy constructor and copy assignment
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // Destructor - uses delete[] for arrays
    ~UniquePtr() {
        if (ptr_) {
            delete[] ptr_;
        }
    }

    // Array subscript operator
    T& operator[](size_t index) const noexcept {
        return ptr_[index];
    }

    // Get raw pointer
    pointer get() const noexcept {
        return ptr_;
    }

    // Release ownership
    pointer release() noexcept {
        pointer old = ptr_;
        ptr_ = nullptr;
        return old;
    }

    // Reset pointer
    void reset(pointer ptr = nullptr) noexcept {
        if (ptr_ != ptr) {
            delete[] ptr_;
            ptr_ = ptr;
        }
    }

    // Boolean conversion
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }
};

// Helper function to create UniquePtr (similar to std::make_unique)
template<typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
UniquePtr<T[]> make_unique_array(size_t size) {
    return UniquePtr<T[]>(new T[size]());
}

// Test class with destructor to verify cleanup
class TestResource {
private:
    int value_;
    static int count_;

public:
    explicit TestResource(int v) : value_(v) {
        ++count_;
        std::cout << "TestResource(" << value_ << ") created. Count: " << count_ << std::endl;
    }

    ~TestResource() {
        --count_;
        std::cout << "TestResource(" << value_ << ") destroyed. Count: " << count_ << std::endl;
    }

    int getValue() const { return value_; }

    static int getCount() { return count_; }
};

int TestResource::count_ = 0;

int main() {
    std::cout << "=== Exercise 1: Custom Smart Pointer ===\n\n";

    // Test 1: Single object
    std::cout << "Test 1: Single object\n";
    {
        UniquePtr<int> ptr1(new int(42));
        std::cout << "Value: " << *ptr1 << std::endl;
        std::cout << "Valid: " << (ptr1 ? "yes" : "no") << std::endl;
    }
    std::cout << std::endl;

    // Test 2: Array
    std::cout << "Test 2: Array\n";
    {
        UniquePtr<int[]> ptr2(new int[5]{1, 2, 3, 4, 5});
        std::cout << "First element: " << ptr2[0] << std::endl;
        std::cout << "Third element: " << ptr2[2] << std::endl;
    }
    std::cout << std::endl;

    // Test 3: Move semantics
    std::cout << "Test 3: Move semantics\n";
    {
        UniquePtr<int> ptr3(new int(100));
        std::cout << "Before move: " << *ptr3 << std::endl;
        
        UniquePtr<int> ptr4 = std::move(ptr3);
        std::cout << "After move - ptr4: " << *ptr4 << std::endl;
        std::cout << "After move - ptr3 valid: " << (ptr3 ? "yes" : "no") << std::endl;
    }
    std::cout << std::endl;

    // Test 4: Reset
    std::cout << "Test 4: Reset\n";
    {
        UniquePtr<int> ptr5(new int(200));
        std::cout << "Before reset: " << *ptr5 << std::endl;
        ptr5.reset(new int(300));
        std::cout << "After reset: " << *ptr5 << std::endl;
    }
    std::cout << std::endl;

    // Test 5: Release
    std::cout << "Test 5: Release\n";
    {
        UniquePtr<int> ptr6(new int(400));
        int* raw = ptr6.release();
        std::cout << "Released value: " << *raw << std::endl;
        std::cout << "ptr6 valid after release: " << (ptr6 ? "yes" : "no") << std::endl;
        delete raw;  // Manual cleanup
    }
    std::cout << std::endl;

    // Test 6: Resource cleanup verification
    std::cout << "Test 6: Resource cleanup\n";
    {
        std::cout << "Initial count: " << TestResource::getCount() << std::endl;
        {
            UniquePtr<TestResource> ptr7(new TestResource(500));
            std::cout << "Count after creation: " << TestResource::getCount() << std::endl;
            std::cout << "Resource value: " << ptr7->getValue() << std::endl;
        }
        std::cout << "Count after destruction: " << TestResource::getCount() << std::endl;
    }
    std::cout << std::endl;

    // Test 7: Array of custom objects
    std::cout << "Test 7: Array of custom objects\n";
    {
        std::cout << "Initial count: " << TestResource::getCount() << std::endl;
        {
            UniquePtr<TestResource[]> ptr8(new TestResource[3]{TestResource(1), TestResource(2), TestResource(3)});
            std::cout << "Count after array creation: " << TestResource::getCount() << std::endl;
            std::cout << "First element value: " << ptr8[0].getValue() << std::endl;
        }
        std::cout << "Count after array destruction: " << TestResource::getCount() << std::endl;
    }
    std::cout << std::endl;

    // Test 8: Helper function
    std::cout << "Test 8: Helper function make_unique\n";
    {
        auto ptr9 = make_unique<int>(999);
        std::cout << "Value from make_unique: " << *ptr9 << std::endl;
    }
    std::cout << std::endl;

    std::cout << "All tests completed!\n";
    return 0;
}

