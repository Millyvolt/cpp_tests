# Exercise 8: Move Semantics Code Analysis

## Problem Statement

Analyze the following C++ code and predict when copy vs move operations occur. This exercise tests your understanding of move semantics, rvalue references, `std::move()`, and when the compiler chooses to move vs copy objects.

## Starter Code

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <utility>

class TrackedResource {
private:
    int id_;
    static int copy_count_;
    static int move_count_;
    static int copy_assign_count_;
    static int move_assign_count_;
    static int next_id_;

public:
    explicit TrackedResource(int value = 0) : id_(next_id_++) {
        std::cout << "TrackedResource(" << id_ << ") constructed with value " << value << std::endl;
    }

    // Copy constructor
    TrackedResource(const TrackedResource& other) : id_(next_id_++) {
        ++copy_count_;
        std::cout << "TrackedResource(" << id_ << ") copy constructed from " << other.id_ << std::endl;
    }

    // Move constructor
    TrackedResource(TrackedResource&& other) noexcept : id_(other.id_) {
        ++move_count_;
        other.id_ = -1;  // Mark as moved-from
        std::cout << "TrackedResource(" << id_ << ") move constructed from " << other.id_ << std::endl;
    }

    // Copy assignment
    TrackedResource& operator=(const TrackedResource& other) {
        if (this != &other) {
            ++copy_assign_count_;
            id_ = next_id_++;
            std::cout << "TrackedResource(" << id_ << ") copy assigned from " << other.id_ << std::endl;
        }
        return *this;
    }

    // Move assignment
    TrackedResource& operator=(TrackedResource&& other) noexcept {
        if (this != &other) {
            ++move_assign_count_;
            id_ = other.id_;
            other.id_ = -1;
            std::cout << "TrackedResource(" << id_ << ") move assigned from " << other.id_ << std::endl;
        }
        return *this;
    }

    ~TrackedResource() {
        if (id_ >= 0) {
            std::cout << "TrackedResource(" << id_ << ") destructed" << std::endl;
        }
    }

    static void resetCounters() {
        copy_count_ = 0;
        move_count_ = 0;
        copy_assign_count_ = 0;
        move_assign_count_ = 0;
        next_id_ = 1;
    }

    static void printCounters() {
        std::cout << "  Copy constructions: " << copy_count_ << std::endl;
        std::cout << "  Move constructions: " << move_count_ << std::endl;
        std::cout << "  Copy assignments: " << copy_assign_count_ << std::endl;
        std::cout << "  Move assignments: " << move_assign_count_ << std::endl;
    }
};

int TrackedResource::copy_count_ = 0;
int TrackedResource::move_count_ = 0;
int TrackedResource::copy_assign_count_ = 0;
int TrackedResource::move_assign_count_ = 0;
int TrackedResource::next_id_ = 1;

// Function taking by value
void funcByValue(TrackedResource obj) {
    std::cout << "funcByValue called" << std::endl;
}

// Function taking by reference
void funcByRef(const TrackedResource& obj) {
    std::cout << "funcByRef called" << std::endl;
}

// Function taking by rvalue reference
void funcByRvalueRef(TrackedResource&& obj) {
    std::cout << "funcByRvalueRef called" << std::endl;
}

// Function returning by value
TrackedResource returnByValue() {
    TrackedResource temp(100);
    return temp;  // What happens here?
}

int main() {
    TrackedResource::resetCounters();
    
    std::cout << "=== Test 1: Direct construction ===" << std::endl;
    TrackedResource obj1(10);
    
    std::cout << "\n=== Test 2: Copy construction ===" << std::endl;
    TrackedResource obj2 = obj1;
    
    std::cout << "\n=== Test 3: Move construction ===" << std::endl;
    TrackedResource obj3 = std::move(obj1);
    
    std::cout << "\n=== Test 4: Function by value (lvalue) ===" << std::endl;
    TrackedResource::resetCounters();
    TrackedResource obj4(20);
    funcByValue(obj4);
    
    std::cout << "\n=== Test 5: Function by value (rvalue) ===" << std::endl;
    TrackedResource::resetCounters();
    funcByValue(TrackedResource(30));
    
    std::cout << "\n=== Test 6: Function by reference ===" << std::endl;
    TrackedResource::resetCounters();
    TrackedResource obj5(40);
    funcByRef(obj5);
    TrackedResource::printCounters();
    
    std::cout << "\n=== Test 7: Function by rvalue reference ===" << std::endl;
    TrackedResource::resetCounters();
    funcByRvalueRef(TrackedResource(50));
    TrackedResource::printCounters();
    
    std::cout << "\n=== Test 8: Return by value ===" << std::endl;
    TrackedResource::resetCounters();
    TrackedResource obj6 = returnByValue();
    TrackedResource::printCounters();
    
    std::cout << "\n=== Test 9: Assignment (lvalue) ===" << std::endl;
    TrackedResource::resetCounters();
    TrackedResource obj7(60);
    TrackedResource obj8(70);
    obj8 = obj7;
    TrackedResource::printCounters();
    
    std::cout << "\n=== Test 10: Assignment (rvalue) ===" << std::endl;
    TrackedResource::resetCounters();
    TrackedResource obj9(80);
    obj9 = TrackedResource(90);
    TrackedResource::printCounters();
    
    std::cout << "\n=== Test 11: std::vector push_back (lvalue) ===" << std::endl;
    TrackedResource::resetCounters();
    std::vector<TrackedResource> vec1;
    TrackedResource obj10(100);
    vec1.push_back(obj10);
    TrackedResource::printCounters();
    
    std::cout << "\n=== Test 12: std::vector push_back (rvalue) ===" << std::endl;
    TrackedResource::resetCounters();
    std::vector<TrackedResource> vec2;
    vec2.push_back(TrackedResource(200));
    TrackedResource::printCounters();
    
    std::cout << "\n=== Test 13: std::vector push_back with std::move ===" << std::endl;
    TrackedResource::resetCounters();
    std::vector<TrackedResource> vec3;
    TrackedResource obj11(300);
    vec3.push_back(std::move(obj11));
    TrackedResource::printCounters();
    
    std::cout << "\n=== Test 14: Container resize ===" << std::endl;
    TrackedResource::resetCounters();
    std::vector<TrackedResource> vec4;
    vec4.resize(3, TrackedResource(400));
    TrackedResource::printCounters();
    
    std::cout << "\n=== Test 15: Multiple moves ===" << std::endl;
    TrackedResource::resetCounters();
    TrackedResource obj12(500);
    TrackedResource obj13 = std::move(obj12);
    TrackedResource obj14 = std::move(obj13);
    TrackedResource::printCounters();
    
    std::cout << "\n=== Test 16: Const object ===" << std::endl;
    TrackedResource::resetCounters();
    const TrackedResource obj15(600);
    TrackedResource obj16 = std::move(obj15);  // What happens?
    TrackedResource::printCounters();
    
    std::cout << "\n=== Final counters ===" << std::endl;
    TrackedResource::printCounters();
    
    return 0;
}
```

## Questions

Answer the following questions for each test case. Predict what operations will occur:

### Test 1: Direct construction
- How many constructions occur?

### Test 2: Copy construction
- Is this a copy or move construction?

### Test 3: Move construction
- What happens to `obj1` after `std::move(obj1)`?
- Can `obj1` still be used?

### Test 4: Function by value (lvalue)
- How many copy constructions occur?
- How many move constructions occur?
- Why?

### Test 5: Function by value (rvalue)
- How many copy constructions occur?
- How many move constructions occur?
- Why is this different from Test 4?

### Test 6: Function by reference
- Why are there no copy or move operations?

### Test 7: Function by rvalue reference
- Why are there no copy or move operations in the counters?
- What happens to the temporary object?

### Test 8: Return by value
- How many constructions occur when returning?
- Does RVO (Return Value Optimization) apply here?
- If RVO doesn't apply, is it a copy or move?

### Test 9: Assignment (lvalue)
- What type of assignment occurs?

### Test 10: Assignment (rvalue)
- What type of assignment occurs?
- Why?

### Test 11: std::vector push_back (lvalue)
- How many copy constructions occur?
- How many move constructions occur?

### Test 12: std::vector push_back (rvalue)
- How many copy constructions occur?
- How many move constructions occur?
- Why is this more efficient than Test 11?

### Test 13: std::vector push_back with std::move
- How many copy constructions occur?
- How many move constructions occur?
- What's the difference between this and Test 11?

### Test 14: Container resize
- How many copy constructions occur?
- Explain why.

### Test 15: Multiple moves
- What happens when you move from an already-moved-from object?
- Is this safe?

### Test 16: Const object
- What happens when you try to move from a const object?
- Why can't it be moved?

## General Questions

1. **When does `std::move()` actually cause a move?**
   - Does `std::move()` itself perform a move operation?
   - What does `std::move()` actually do?

2. **When are moves preferred over copies?**
   - What conditions must be met for a move to occur?
   - When does the compiler fall back to copying?

3. **Rvalue reference binding:**
   - Can an rvalue reference bind to an lvalue?
   - Can an rvalue reference bind to an rvalue?

4. **Return Value Optimization (RVO):**
   - What is RVO?
   - When does the compiler apply RVO?
   - How does RVO relate to move semantics?

## Hints

- `std::move()` is just a cast - it doesn't move anything by itself
- Moving from a const object falls back to copying (const objects can't be moved)
- Functions taking by value will either copy or move depending on what's passed
- Return value optimization can eliminate moves/copies entirely
- Containers like `std::vector` use move semantics during reallocation

## Verification

After you've made your predictions, compile and run `exercise_8_solution.cpp` to verify your answers. Compare the output with your predictions and understand any differences.

