# Exercise 1: Custom Smart Pointer with Template Specialization

## Problem Statement

Implement a custom smart pointer class template called `UniquePtr` that manages a single dynamically allocated resource. The implementation should use template specialization to handle arrays differently from single objects, and include type traits to determine the appropriate behavior at compile time.

## Requirements

1. **Basic Smart Pointer Interface:**
   - `UniquePtr` should manage a single resource (single object or array)
   - Automatic resource cleanup when the pointer goes out of scope
   - Support for move semantics (non-copyable)
   - Overload `operator*` and `operator->` for single objects
   - Overload `operator[]` for arrays

2. **Template Specialization:**
   - Primary template handles single objects
   - Partial specialization handles arrays `T[]`
   - Use type traits to distinguish between single objects and arrays at compile time

3. **Type Traits:**
   - Implement a helper type trait `is_array` to detect array types
   - Use SFINAE or `std::is_array` for type checking

4. **Resource Management:**
   - Constructor should accept a raw pointer or use default construction
   - Destructor should properly delete the resource (use `delete` for objects, `delete[]` for arrays)
   - Reset functionality to replace the managed resource
   - Release functionality to release ownership without deleting

5. **Additional Features:**
   - `get()` method to return raw pointer without transferring ownership
   - `operator bool()` for checking if pointer is valid
   - Default constructor (nullptr)
   - Move constructor and move assignment operator

## Example Usage

```cpp
// Single object
UniquePtr<int> ptr1(new int(42));
std::cout << *ptr1 << std::endl;  // Output: 42

// Array
UniquePtr<int[]> ptr2(new int[5]{1, 2, 3, 4, 5});
std::cout << ptr2[0] << std::endl;  // Output: 1

// Move semantics
UniquePtr<int> ptr3 = std::move(ptr1);
// ptr1 is now nullptr

// Reset
ptr3.reset(new int(100));
std::cout << *ptr3 << std::endl;  // Output: 100

// Release
int* raw = ptr3.release();
// ptr3 is now nullptr, caller owns raw
delete raw;
```

## Constraints

- Do not use `std::unique_ptr` in your implementation (you're creating your own!)
- Must compile with C++17 or later
- Handle edge cases: nullptr, self-assignment, empty arrays
- All methods should be `noexcept` where appropriate

## Hints

- Use `std::remove_extent` to get the element type from array types
- Consider using `std::conditional` for compile-time type selection
- Template specialization allows different deletion logic for arrays vs single objects
- Use `= delete` to prevent copying
- Consider using `std::exchange` for move operations

## Testing Considerations

- Test with single objects (int, custom classes)
- Test with arrays of different types
- Test move semantics
- Test reset and release
- Test with nullptr
- Verify proper cleanup (use a class with destructor that prints)

