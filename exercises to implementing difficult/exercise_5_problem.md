# Exercise 5: Advanced STL Usage - Custom Iterator

## Problem Statement

Implement a custom iterator that integrates seamlessly with STL algorithms and containers. Create an iterator for a custom container or data structure that supports standard iterator operations and works with STL algorithms like `std::find`, `std::sort`, `std::transform`, etc.

## Requirements

1. **Custom Container:**
   - Implement a simple custom container (e.g., a fixed-size array wrapper, or a linked list)
   - The container should work with standard iterators
   - Provide `begin()` and `end()` methods returning iterators

2. **Iterator Implementation:**
   - Implement iterator following STL iterator requirements
   - Support appropriate iterator category (Input, Forward, Bidirectional, or Random Access)
   - Implement required operations for the chosen category:
     - Dereference operators (`operator*`, `operator->`)
     - Increment operators (`operator++`, `operator++(int)`)
     - Decrement operators (for Bidirectional/Random Access: `operator--`, `operator--(int)`)
     - Comparison operators (`operator==`, `operator!=`, `operator<`, etc. for Random Access)
     - Arithmetic operators (for Random Access: `operator+`, `operator-`, `operator+=`, `operator-=`)
     - Subscript operator (for Random Access: `operator[]`)

3. **Iterator Traits:**
   - Provide proper iterator traits (can use `std::iterator_traits` or specialize it)
   - Define appropriate iterator category tag
   - Define value type, difference type, pointer type, reference type

4. **STL Compatibility:**
   - Iterator should work with STL algorithms (`std::find`, `std::count`, `std::for_each`, etc.)
   - Support range-based for loops
   - Support iterator-based container construction

5. **Const Iterator (Optional but Recommended):**
   - Implement const iterator variant
   - Provide `cbegin()` and `cend()` methods

## Example Usage

```cpp
// Custom container with iterator
FixedArray<int, 5> arr{1, 2, 3, 4, 5};

// Range-based for loop
for (const auto& elem : arr) {
    std::cout << elem << " ";
}

// STL algorithms
auto it = std::find(arr.begin(), arr.end(), 3);
if (it != arr.end()) {
    std::cout << "Found: " << *it << std::endl;
}

// Iterator arithmetic (if Random Access)
auto begin = arr.begin();
auto end = arr.end();
std::cout << "Distance: " << (end - begin) << std::endl;
std::cout << "Third element: " << arr.begin()[2] << std::endl;

// Transform
std::vector<int> doubled;
std::transform(arr.begin(), arr.end(), std::back_inserter(doubled),
               [](int x) { return x * 2; });
```

## Constraints

- Must use C++17 or later
- Iterator must satisfy at least Forward Iterator requirements
- Should work with all applicable STL algorithms
- Provide proper iterator traits
- Handle edge cases (empty container, end iterator, etc.)

## Hints

- Choose appropriate iterator category based on your container's capabilities
- Use iterator tags: `std::input_iterator_tag`, `std::forward_iterator_tag`, `std::bidirectional_iterator_tag`, `std::random_access_iterator_tag`
- Implement iterator traits properly (C++17 allows simpler approach with member typedefs)
- Consider using `std::iterator_traits<Iterator>` specialization
- For Random Access iterators, you need: `+`, `-`, `+=`, `-=`, `[]`, `<`, `>`, `<=`, `>=`
- Use `iterator` and `const_iterator` typedefs in container
- Consider reverse iterators (`rbegin()`, `rend()`) for extra credit

## Iterator Categories (from least to most capable)

1. **Input Iterator**: Read-only, forward-only, single-pass
2. **Forward Iterator**: Multi-pass, forward-only
3. **Bidirectional Iterator**: Can move backward (`--`)
4. **Random Access Iterator**: Can jump arbitrary distances (`+`, `-`, `[]`)

## Key Concepts to Demonstrate

- STL iterator requirements
- Iterator categories and traits
- Iterator operations and semantics
- STL algorithm compatibility
- Range-based for loop support
- Const-correctness for iterators

## Testing Considerations

- Test all iterator operations
- Test with various STL algorithms
- Test range-based for loops
- Test const iterators
- Test iterator comparisons
- Test edge cases (empty container, end iterator)
- Verify iterator category requirements are met

## Additional Challenge (Optional)

- Implement a custom allocator instead of (or in addition to) iterator
- Implement reverse iterators
- Implement iterator adapters (e.g., transform iterator)

