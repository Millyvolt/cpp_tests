# Exercise 2: Move Semantics & Perfect Forwarding

## Problem Statement

Implement utility functions and classes that demonstrate efficient resource transfer using move semantics and perfect forwarding. Create a generic factory function, a wrapper class that preserves value categories, and a function that conditionally moves resources based on their value category.

## Requirements

1. **Generic Factory Function with Perfect Forwarding:**
   - Create a `make_shared`-like factory function that accepts any number of arguments
   - Use perfect forwarding to preserve value categories (lvalues stay lvalues, rvalues stay rvalues)
   - Handle both lvalue and rvalue references correctly

2. **Value Category Preserving Wrapper:**
   - Create a `Wrapper` class template that stores a value
   - Constructor should use perfect forwarding to preserve value categories
   - Implement move constructor and move assignment operator
   - Prevent unnecessary copies when possible

3. **Conditional Move Function:**
   - Create a function `conditional_move` that moves lvalues to rvalues only when appropriate
   - Use type traits to determine if a type is moveable
   - Handle both moveable and non-moveable types

4. **Resource Manager with Move Semantics:**
   - Implement a `ResourceManager` class that manages a resource (like a buffer or file handle)
   - Support move construction and move assignment
   - Ensure proper resource transfer without copying
   - Handle self-assignment in move operations

5. **Advanced: Forwarding Reference Container:**
   - Create a container-like class that can store either lvalue or rvalue references
   - Use template specialization to handle references correctly
   - Prevent dangling references

## Example Usage

```cpp
// Perfect forwarding factory
auto obj1 = make_resource<MyClass>(arg1, arg2, arg3);  // Forward all args

// Value category preserving wrapper
int x = 42;
Wrapper w1(x);                    // Stores reference or copy?
Wrapper w2(100);                  // Stores rvalue
Wrapper w3 = std::move(w2);       // Move construction

// Conditional move
std::string str = "hello";
auto moved = conditional_move(str);  // Returns moved string if moveable

// Resource manager
ResourceManager rm1(1024);        // Allocates resource
ResourceManager rm2 = std::move(rm1);  // Transfers ownership
// rm1 is now empty/invalid
```

## Constraints

- Must use C++17 or later
- No unnecessary copies should occur
- All move operations should be `noexcept` where possible
- Handle edge cases: self-assignment, nullptr, empty states
- Use `std::forward` and `std::move` appropriately

## Hints

- Remember: `std::move` casts to rvalue, `std::forward` preserves value category
- Use `std::is_move_constructible` and `std::is_move_assignable` for type traits
- Perfect forwarding requires forwarding references (`T&&` where T is a template parameter)
- Use `std::enable_if` or `if constexpr` for conditional compilation
- Consider using `std::exchange` for move assignments

## Key Concepts to Demonstrate

- Difference between `std::move` and `std::forward`
- Universal/forwarding references (`T&&`)
- Value category preservation
- Move semantics efficiency
- Rvalue references vs lvalue references

## Testing Considerations

- Test with moveable types (std::string, std::vector)
- Test with non-moveable types (const objects, arrays)
- Verify no unnecessary copies occur
- Test self-assignment scenarios
- Measure performance differences (if possible)

