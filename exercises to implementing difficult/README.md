# Advanced C++ Programming Exercises

This repository contains 8 advanced-level C++ programming exercises designed to help you master modern C++ features and best practices.

## Exercises

### Exercise 1: Custom Smart Pointer with Template Specialization
**Topic:** Template Meta-Programming  
**Files:** `exercise_1_problem.md`, `exercise_1_solution.cpp`

Implement a custom smart pointer class that handles different types of resources with template specialization. Learn about template meta-programming, SFINAE, and resource management.

**Key Concepts:**
- Template specialization
- Type traits
- Resource management

### Exercise 2: Move Semantics & Perfect Forwarding
**Topic:** Move Semantics & Perfect Forwarding  
**Files:** `exercise_2_problem.md`, `exercise_2_solution.cpp`

Create utility functions and classes that efficiently handle resource transfer using move semantics and perfect forwarding. Understand rvalue references, std::move, and std::forward.

**Key Concepts:**
- Rvalue references
- Move semantics
- Perfect forwarding
- Universal references

### Exercise 3: Thread-Safe Data Structure
**Topic:** Concurrency & Thread Safety  
**Files:** `exercise_3_problem.md`, `exercise_3_solution.cpp`

Implement a thread-safe data structure (such as a thread-safe queue or cache) using modern C++ concurrency primitives. Practice with mutexes, condition variables, and atomic operations.

**Key Concepts:**
- std::mutex and std::lock_guard
- std::condition_variable
- Thread safety guarantees
- Lock-free programming concepts

### Exercise 4: RAII Resource Wrapper with Exception Safety
**Topic:** RAII & Exception Safety  
**Files:** `exercise_4_problem.md`, `exercise_4_solution.cpp`

Design a resource wrapper that follows RAII principles and provides strong exception safety guarantees. Handle resource acquisition, release, and exception scenarios.

**Key Concepts:**
- RAII (Resource Acquisition Is Initialization)
- Exception safety guarantees
- Copy/move semantics for resources
- Smart resource management

### Exercise 5: Custom STL Allocator or Iterator
**Topic:** Advanced STL Usage  
**Files:** `exercise_5_problem.md`, `exercise_5_solution.cpp`

Implement a custom allocator or iterator that integrates seamlessly with the STL containers. Understand the requirements and interfaces of STL components.

**Key Concepts:**
- STL allocator requirements
- Iterator categories and traits
- Template requirements
- Container compatibility

### Exercise 6: Virtual Inheritance & Multiple Inheritance
**Topic:** Virtual Inheritance & Multiple Inheritance  
**Files:** `exercise_6_problem.md`, `exercise_6_solution.cpp`

Implement a class hierarchy that demonstrates the diamond problem in multiple inheritance and solve it using virtual inheritance. Learn when and how to use virtual inheritance correctly.

**Key Concepts:**
- Virtual inheritance syntax and semantics
- Diamond problem and its solution
- Constructor initialization order with virtual bases
- Virtual function resolution in multiple inheritance
- Object layout with virtual inheritance
- Interface-based inheritance patterns

### Exercise 7: Virtual Inheritance Code Analysis
**Topic:** Virtual Inheritance Code Reading  
**Files:** `exercise_7_problem.md`, `exercise_7_solution.cpp`, `exercise_7_answers.md`

Analyze provided C++ code with virtual inheritance and predict the program output. Test your understanding of constructor/destructor order, virtual base class initialization, virtual function resolution, and object layout.

**Key Concepts:**
- Constructor initialization order with virtual bases
- Virtual base class instance counting
- Virtual function resolution through different pointer types
- Object size and memory layout with virtual inheritance
- Destructor order and cleanup sequence

**Type:** Code Reading / Output Prediction

### Exercise 8: Move Semantics Code Analysis
**Topic:** Move Semantics Code Reading  
**Files:** `exercise_8_problem.md`, `exercise_8_solution.cpp`, `exercise_8_answers.md`

Analyze provided C++ code demonstrating move semantics and predict when copy vs move operations occur. Test your understanding of rvalue references, `std::move()`, function parameter passing, return value optimization, and container operations with move semantics.

**Key Concepts:**
- Copy vs move constructors and assignments
- Rvalue reference binding rules
- `std::move()` and when moves actually occur
- Function parameter passing (by value, reference, rvalue reference)
- Return Value Optimization (RVO)
- Container operations with move semantics
- Const objects and move semantics

**Type:** Code Reading / Output Prediction  
**Difficulty:** Medium

## How to Use

1. Read the problem statement in `exercise_N_problem.md`
2. Try to implement your solution (or predict the output for code-reading exercises)
3. Compare with the solution in `exercise_N_solution.cpp`
4. For code-reading exercises, check `exercise_N_answers.md` for detailed explanations
5. Study the implementation and comments to understand advanced C++ concepts

## Requirements

- C++17 or later recommended
- A C++ compiler with good standard library support (GCC 7+, Clang 5+, MSVC 2017+)
- Optional: A testing framework for running the examples

## Compilation

Each solution file can be compiled independently:

```bash
g++ -std=c++17 -pthread exercise_N_solution.cpp -o exercise_N
./exercise_N
```

Note: Exercise 3 (Concurrency) requires the `-pthread` flag for threading support.

