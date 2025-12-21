# Exercise 4: RAII & Exception Safety

## Problem Statement

Implement a resource wrapper class that follows RAII (Resource Acquisition Is Initialization) principles and provides strong exception safety guarantees. The class should manage a resource (such as a file handle, memory buffer, or network connection) and ensure proper cleanup even in the presence of exceptions.

## Requirements

1. **RAII Resource Wrapper:**
   - Acquire resource in constructor
   - Release resource in destructor (even if exceptions occur)
   - Support both move and copy semantics appropriately
   - Handle resource acquisition failures

2. **Exception Safety Guarantees:**
   - **Basic Guarantee**: No resource leaks, object remains in valid state
   - **Strong Guarantee**: Operations are atomic - either complete successfully or leave state unchanged
   - **No-throw Guarantee**: Certain operations should never throw (e.g., destructor, move operations)

3. **Resource Management:**
   - Implement proper copy semantics (deep copy, reference counting, or disable copying)
   - Implement proper move semantics (transfer ownership)
   - Handle self-assignment safely
   - Provide a way to release resource without destruction
   - Provide a way to reset/reassign the resource

4. **Specific Requirements:**
   - Destructor must be `noexcept`
   - Move constructor and move assignment should be `noexcept` where possible
   - Handle exceptions during resource acquisition
   - Handle exceptions during resource operations
   - Provide query methods (is_valid, get_resource, etc.)

5. **Example Resource Types to Consider:**
   - Memory buffer (dynamically allocated array)
   - File handle (simulated)
   - Network connection (simulated)
   - Lock (mutex wrapper)

## Example Usage

```cpp
// Basic usage
{
    ResourceWrapper<FileHandle> file(open_file("test.txt"));
    if (file.is_valid()) {
        file.write("Hello");
        // Resource automatically released when file goes out of scope
    }
}

// Move semantics
ResourceWrapper<Buffer> buf1(allocate_buffer(1024));
ResourceWrapper<Buffer> buf2 = std::move(buf1);  // Transfer ownership
// buf1 is now invalid

// Exception safety
try {
    ResourceWrapper<Connection> conn(establish_connection());
    conn.send_data(data);  // May throw
    // Resource still properly cleaned up even if exception thrown
} catch (...) {
    // Resource already cleaned up
}
```

## Constraints

- Must use C++17 or later
- No resource leaks under any circumstances (including exceptions)
- Destructor must never throw exceptions
- Follow the Rule of Five (or Rule of Zero if using smart pointers internally)
- Provide meaningful error handling

## Hints

- Use RAII: acquire in constructor, release in destructor
- Use `std::exchange` or similar for move operations
- Use `noexcept` specifiers appropriately
- Consider using smart pointers internally for automatic cleanup
- For copy semantics, decide: deep copy, reference counting, or non-copyable
- Use `std::optional` or nullable pointer to track validity
- Test exception paths thoroughly

## Key Concepts to Demonstrate

- RAII principle
- Exception safety guarantees (basic, strong, no-throw)
- Resource lifetime management
- Move vs copy semantics for resources
- Exception-safe code design
- The Rule of Five

## Testing Considerations

- Test normal resource acquisition and release
- Test move semantics
- Test copy semantics (if supported)
- Test exception during resource acquisition
- Test exception during resource operations
- Test resource cleanup when exceptions are thrown
- Test self-assignment
- Verify no resource leaks (use tools like valgrind if available)

## Exception Safety Levels

1. **No-throw guarantee**: Operation never throws exceptions
2. **Strong guarantee**: Operation either completes successfully or leaves state unchanged
3. **Basic guarantee**: Operation may change state, but object remains in valid state, no resource leaks

## Safety Notes

- Destructors should never throw exceptions
- Move operations should be `noexcept` when possible
- Always test exception paths
- Use RAII to ensure cleanup even with exceptions

