#include <iostream>
#include <memory>
#include <utility>
#include <type_traits>
#include <string>
#include <vector>

// ============================================================================
// 1. Generic Factory Function with Perfect Forwarding
// ============================================================================

template<typename T, typename... Args>
std::unique_ptr<T> make_resource(Args&&... args) {
    // Perfect forwarding: std::forward preserves the value category
    // of each argument, so lvalues are passed as lvalues and rvalues as rvalues
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// ============================================================================
// 2. Value Category Preserving Wrapper
// ============================================================================

template<typename T>
class Wrapper {
private:
    T value_;

public:
    // Perfect forwarding constructor
    // This is a forwarding reference (universal reference) when T is a template parameter
    template<typename U>
    explicit Wrapper(U&& val) 
        : value_(std::forward<U>(val)) {
        std::cout << "Wrapper constructed (forwarding)" << std::endl;
    }

    // Move constructor
    Wrapper(Wrapper&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : value_(std::move(other.value_)) {
        std::cout << "Wrapper move constructed" << std::endl;
    }

    // Move assignment operator
    Wrapper& operator=(Wrapper&& other) noexcept(std::is_nothrow_move_assignable_v<T>) {
        if (this != &other) {
            value_ = std::move(other.value_);
            std::cout << "Wrapper move assigned" << std::endl;
        }
        return *this;
    }

    // Delete copy constructor and copy assignment
    Wrapper(const Wrapper&) = delete;
    Wrapper& operator=(const Wrapper&) = delete;

    // Accessor
    const T& get() const { return value_; }
    T& get() { return value_; }

    T* operator->() { return &value_; }
    const T* operator->() const { return &value_; }
};

// ============================================================================
// 3. Conditional Move Function
// ============================================================================

// Helper type trait to check if type is moveable
template<typename T>
struct is_moveable {
    static constexpr bool value = std::is_move_constructible_v<T> && 
                                  std::is_move_assignable_v<T> &&
                                  !std::is_reference_v<T>;
};

template<typename T>
constexpr bool is_moveable_v = is_moveable<T>::value;

// Conditional move: only moves if type is moveable
template<typename T>
std::conditional_t<is_moveable_v<std::remove_reference_t<T>>, 
                   std::remove_reference_t<T>, 
                   T>
conditional_move(T&& val) {
    if constexpr (is_moveable_v<std::remove_reference_t<T>>) {
        return std::move(val);
    } else {
        return val;
    }
}

// ============================================================================
// 4. Resource Manager with Move Semantics
// ============================================================================

class ResourceManager {
private:
    size_t size_;
    int* buffer_;  // Simulated resource (could be file handle, socket, etc.)

public:
    // Constructor
    explicit ResourceManager(size_t size) 
        : size_(size), buffer_(new int[size]()) {
        std::cout << "ResourceManager: Allocated " << size_ << " elements" << std::endl;
    }

    // Move constructor
    ResourceManager(ResourceManager&& other) noexcept
        : size_(other.size_), buffer_(other.buffer_) {
        other.size_ = 0;
        other.buffer_ = nullptr;
        std::cout << "ResourceManager: Move constructed" << std::endl;
    }

    // Move assignment operator
    ResourceManager& operator=(ResourceManager&& other) noexcept {
        if (this != &other) {
            // Clean up existing resource
            delete[] buffer_;
            
            // Transfer ownership
            size_ = other.size_;
            buffer_ = other.buffer_;
            
            // Reset source
            other.size_ = 0;
            other.buffer_ = nullptr;
            
            std::cout << "ResourceManager: Move assigned" << std::endl;
        }
        return *this;
    }

    // Delete copy operations
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Destructor
    ~ResourceManager() {
        if (buffer_) {
            delete[] buffer_;
            std::cout << "ResourceManager: Released " << size_ << " elements" << std::endl;
        }
    }

    // Accessors
    size_t size() const { return size_; }
    bool valid() const { return buffer_ != nullptr; }
    int& operator[](size_t index) { return buffer_[index]; }
    const int& operator[](size_t index) const { return buffer_[index]; }
};

// ============================================================================
// 5. Forwarding Reference Container (Advanced)
// ============================================================================

// For storing lvalue references
template<typename T>
class ReferenceWrapper {
private:
    T* ptr_;

public:
    explicit ReferenceWrapper(T& ref) noexcept : ptr_(&ref) {}
    
    T& get() const noexcept { return *ptr_; }
    operator T&() const noexcept { return *ptr_; }
    T& operator*() const noexcept { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }
};

// Container that can hold either value or reference
template<typename T>
class OptionalReference {
private:
    std::aligned_storage_t<sizeof(T), alignof(T)> storage_;
    bool is_value_;

    T* ptr() {
        return reinterpret_cast<T*>(&storage_);
    }
    
    const T* ptr() const {
        return reinterpret_cast<const T*>(&storage_);
    }

public:
    // Store by value (for rvalues)
    template<typename U>
    OptionalReference(U&& val) 
        : is_value_(true) {
        new (ptr()) T(std::forward<U>(val));
    }

    // Store reference (for lvalues)
    OptionalReference(ReferenceWrapper<T> ref)
        : is_value_(false) {
        new (ptr()) T*(ref.get());
    }

    ~OptionalReference() {
        if (is_value_) {
            ptr()->~T();
        } else {
            // Don't delete reference
        }
    }

    // Move constructor
    OptionalReference(OptionalReference&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : is_value_(other.is_value_) {
        if (is_value_) {
            new (ptr()) T(std::move(*other.ptr()));
            other.ptr()->~T();
        } else {
            new (ptr()) T*(*reinterpret_cast<T**>(other.ptr()));
        }
        other.is_value_ = false;
    }

    // Delete copy operations
    OptionalReference(const OptionalReference&) = delete;
    OptionalReference& operator=(const OptionalReference&) = delete;

    T& get() {
        if (is_value_) {
            return *ptr();
        } else {
            return **reinterpret_cast<T**>(ptr());
        }
    }

    const T& get() const {
        if (is_value_) {
            return *ptr();
        } else {
            return **reinterpret_cast<T* const*>(ptr());
        }
    }
};

// ============================================================================
// Test Classes
// ============================================================================

class TestClass {
private:
    std::string name_;
    static int copy_count_;
    static int move_count_;

public:
    explicit TestClass(const std::string& name) : name_(name) {
        std::cout << "  TestClass(" << name_ << ") constructed" << std::endl;
    }

    TestClass(const TestClass& other) : name_(other.name_) {
        ++copy_count_;
        std::cout << "  TestClass(" << name_ << ") copied (total copies: " << copy_count_ << ")" << std::endl;
    }

    TestClass(TestClass&& other) noexcept : name_(std::move(other.name_)) {
        ++move_count_;
        std::cout << "  TestClass(" << name_ << ") moved (total moves: " << move_count_ << ")" << std::endl;
    }

    TestClass& operator=(const TestClass& other) {
        if (this != &other) {
            name_ = other.name_;
            ++copy_count_;
            std::cout << "  TestClass(" << name_ << ") copy assigned" << std::endl;
        }
        return *this;
    }

    TestClass& operator=(TestClass&& other) noexcept {
        if (this != &other) {
            name_ = std::move(other.name_);
            ++move_count_;
            std::cout << "  TestClass(" << name_ << ") move assigned" << std::endl;
        }
        return *this;
    }

    const std::string& getName() const { return name_; }

    static void resetCounters() {
        copy_count_ = 0;
        move_count_ = 0;
    }

    static void printCounters() {
        std::cout << "  Counters - Copies: " << copy_count_ << ", Moves: " << move_count_ << std::endl;
    }
};

int TestClass::copy_count_ = 0;
int TestClass::move_count_ = 0;

// ============================================================================
// Main Test Program
// ============================================================================

int main() {
    std::cout << "=== Exercise 2: Move Semantics & Perfect Forwarding ===\n\n";

    // Test 1: Perfect forwarding factory
    std::cout << "Test 1: Perfect Forwarding Factory\n";
    {
        auto obj = make_resource<TestClass>(std::string("FactoryObject"));
        std::cout << "Created: " << obj->getName() << std::endl;
    }
    std::cout << std::endl;

    // Test 2: Value category preserving wrapper
    std::cout << "Test 2: Value Category Preserving Wrapper\n";
    {
        TestClass::resetCounters();
        
        TestClass temp("WrapperTest");
        std::cout << "Creating wrapper with lvalue:\n";
        Wrapper<TestClass> w1(temp);  // Should copy or reference
        
        std::cout << "Creating wrapper with rvalue:\n";
        Wrapper<TestClass> w2(TestClass("Temporary"));  // Should move
        
        std::cout << "Move constructing wrapper:\n";
        Wrapper<TestClass> w3 = std::move(w2);
        
        TestClass::printCounters();
    }
    std::cout << std::endl;

    // Test 3: Conditional move
    std::cout << "Test 3: Conditional Move\n";
    {
        std::string str = "Hello, World!";
        std::cout << "Original: " << str << std::endl;
        
        auto moved_str = conditional_move(str);
        std::cout << "After conditional_move: " << str << " (may be empty if moved)" << std::endl;
        std::cout << "Moved result: " << moved_str << std::endl;
        
        const std::string const_str = "Constant";
        auto const_result = conditional_move(const_str);
        std::cout << "Constant string preserved: " << const_str << std::endl;
    }
    std::cout << std::endl;

    // Test 4: Resource manager
    std::cout << "Test 4: Resource Manager with Move Semantics\n";
    {
        ResourceManager rm1(10);
        std::cout << "rm1 valid: " << (rm1.valid() ? "yes" : "no") << std::endl;
        
        std::cout << "Moving rm1 to rm2:\n";
        ResourceManager rm2 = std::move(rm1);
        std::cout << "rm1 valid after move: " << (rm1.valid() ? "yes" : "no") << std::endl;
        std::cout << "rm2 valid: " << (rm2.valid() ? "yes" : "no") << std::endl;
        
        std::cout << "Move assignment:\n";
        ResourceManager rm3(5);
        rm3 = std::move(rm2);
        std::cout << "rm2 valid after move assignment: " << (rm2.valid() ? "yes" : "no") << std::endl;
        std::cout << "rm3 valid: " << (rm3.valid() ? "yes" : "no") << std::endl;
        
        // Test self-assignment
        std::cout << "Self move assignment (should be safe):\n";
        rm3 = std::move(rm3);
        std::cout << "rm3 still valid: " << (rm3.valid() ? "yes" : "no") << std::endl;
    }
    std::cout << std::endl;

    // Test 5: Comparison of move vs copy
    std::cout << "Test 5: Performance Comparison (Move vs Copy)\n";
    {
        TestClass::resetCounters();
        
        std::vector<TestClass> vec;
        vec.reserve(3);
        
        std::cout << "Adding with push_back (rvalue - should move):\n";
        vec.push_back(TestClass("Element1"));
        vec.push_back(TestClass("Element2"));
        vec.push_back(TestClass("Element3"));
        
        TestClass::printCounters();
    }
    std::cout << std::endl;

    std::cout << "All tests completed!\n";
    return 0;
}

