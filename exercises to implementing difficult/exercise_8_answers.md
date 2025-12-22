# Exercise 8: Move Semantics Code Analysis - Answers

## Expected Output Summary

The exact output may vary slightly by compiler (especially for RVO), but the operation counts should follow these patterns:

## Detailed Answers

### Test 1: Direct construction
**Answer:** 1 construction (regular constructor)

**Explanation:**
- Regular constructor is called
- No copy or move operations occur
- This is straightforward object construction

### Test 2: Copy construction
**Answer:** 1 copy construction

**Explanation:**
- `obj2 = obj1` uses the copy constructor
- `obj1` is an lvalue, so the copy constructor is selected
- No move occurs because `obj1` is not an rvalue

### Test 3: Move construction
**Answer:** 1 move construction; `obj1` is left in moved-from state

**Explanation:**
- `std::move(obj1)` casts `obj1` to an rvalue reference
- The move constructor is called
- `obj1.id_` is set to -1 (moved-from state)
- `obj1` is still valid (won't crash), but its state is unspecified
- You should not use `obj1` after it's been moved from (unless you reassign it)

### Test 4: Function by value (lvalue)
**Answer:** 1 copy construction

**Explanation:**
- When you pass an lvalue to a function taking by value, it must be copied
- `funcByValue(obj4)` passes `obj4` (an lvalue) by value
- The copy constructor is called to create the parameter
- After the function returns, the parameter is destroyed

### Test 5: Function by value (rvalue)
**Answer:** 1 move construction (or possibly 0 with RVO)

**Explanation:**
- `funcByValue(TrackedResource(30))` passes a temporary (rvalue) by value
- The move constructor is preferred for rvalues
- The temporary is moved into the function parameter
- More efficient than copying because it transfers ownership

**Key Point:** This demonstrates the advantage of move semantics - passing rvalues by value is now efficient!

### Test 6: Function by reference
**Answer:** 0 copy/move operations

**Explanation:**
- `funcByRef(obj5)` passes by const reference
- No copy or move occurs - the function receives a reference to the original object
- This is the most efficient way to pass objects when you don't need to modify or take ownership
- No object construction/destruction overhead

### Test 7: Function by rvalue reference
**Answer:** 0 copy/move operations (in the counters)

**Explanation:**
- `funcByRvalueRef(TrackedResource(50))` takes an rvalue reference
- No copy or move occurs when binding the reference
- The temporary object's lifetime is extended for the function call
- However, if you were to store or move the parameter inside the function, that would count

**Key Point:** Taking by rvalue reference doesn't move anything - it just binds to an rvalue. The move happens when you actually use it (e.g., constructing another object from it).

### Test 8: Return by value
**Answer:** 0 or 1 move construction (depends on RVO)

**Explanation:**
- Modern compilers apply **Return Value Optimization (RVO)** in most cases
- RVO constructs the return value directly in the caller's location, eliminating the move/copy
- If RVO doesn't apply (rare with modern compilers), the move constructor is called
- The move constructor is preferred over copy for return values (since the local `temp` is about to be destroyed)

**Key Point:** RVO and move semantics make returning by value efficient - no need for output parameters or pointers!

### Test 9: Assignment (lvalue)
**Answer:** 1 copy assignment

**Explanation:**
- `obj8 = obj7` assigns from an lvalue
- The copy assignment operator is called
- `obj7` remains unchanged (copy, not move)

### Test 10: Assignment (rvalue)
**Answer:** 1 move assignment

**Explanation:**
- `obj9 = TrackedResource(90)` assigns from a temporary (rvalue)
- The move assignment operator is called
- More efficient than copy assignment
- The temporary is moved into `obj9`

### Test 11: std::vector push_back (lvalue)
**Answer:** 1 copy construction

**Explanation:**
- `vec1.push_back(obj10)` passes an lvalue to `push_back`
- `std::vector::push_back` has an overload: `void push_back(const T&)`
- This copies the object into the vector
- The original `obj10` remains unchanged

### Test 12: std::vector push_back (rvalue)
**Answer:** 1 move construction

**Explanation:**
- `vec2.push_back(TrackedResource(200))` passes an rvalue (temporary)
- `std::vector::push_back` has an overload: `void push_back(T&&)`
- This moves the temporary into the vector
- More efficient than copying - ownership is transferred
- The temporary is destroyed after the move

**Key Point:** This is why you should use `push_back(std::move(obj))` or `push_back(T{})` instead of `push_back(obj)` when possible!

### Test 13: std::vector push_back with std::move
**Answer:** 1 move construction

**Explanation:**
- `vec3.push_back(std::move(obj11))` explicitly moves the lvalue
- `std::move()` casts `obj11` to an rvalue reference
- The move overload of `push_back` is selected
- `obj11` is moved into the vector (becomes moved-from state)

**Key Point:** Use `std::move()` when you're done with an object and want to transfer it efficiently!

### Test 14: Container resize
**Answer:** 3 copy constructions (typically)

**Explanation:**
- `vec4.resize(3, TrackedResource(400))` creates 3 elements
- The second parameter is copied for each element
- Even though `TrackedResource(400)` is a temporary, `resize` takes it by const reference, so it's copied 3 times
- Each new element gets a copy of the value parameter

**Note:** Some implementations might optimize this, but the standard behavior is to copy.

### Test 15: Multiple moves
**Answer:** 2 move constructions

**Explanation:**
- Moving from a moved-from object is **safe** and valid
- Each `std::move()` creates a new moved-from object
- `obj12` → moved-from → `obj13` → moved-from → `obj14`
- The moved-from objects are in a valid but unspecified state
- They can be assigned to or destroyed, but their values are undefined

**Key Point:** Always implement move operations to leave objects in a valid (but unspecified) state!

### Test 16: Const object
**Answer:** 1 copy construction

**Explanation:**
- `const TrackedResource obj15(600)` cannot be moved
- Move operations require non-const rvalue references
- `std::move(obj15)` still creates an rvalue reference, but it's a `const&&`, not `&&`
- The copy constructor is selected because there's no move constructor that takes `const TrackedResource&&`
- **Const objects cannot be moved - they must be copied**

**Key Point:** This is why you can't move from const objects - the move constructor takes a non-const rvalue reference!

## General Questions Answers

### 1. When does `std::move()` actually cause a move?

**Answer:**
- `std::move()` itself **does NOT perform a move** - it's just a cast!
- `std::move(x)` is equivalent to `static_cast<T&&>(x)` - it casts to rvalue reference
- The actual move happens when:
  - An rvalue reference is used to construct/assign another object
  - The move constructor or move assignment operator is called
- `std::move()` doesn't guarantee a move will occur - if no move constructor exists, it falls back to copy

### 2. When are moves preferred over copies?

**Answer:**
Moves are preferred when:
- The source is an rvalue (temporary, result of `std::move()`, return value)
- A move constructor/assignment operator exists and is accessible
- The source is not const
- Overload resolution selects the move version

The compiler falls back to copying when:
- The source is an lvalue (and not explicitly moved)
- No move constructor/assignment exists
- The source is const (can't be moved)
- The move constructor is deleted or inaccessible

### 3. Rvalue reference binding

**Answer:**
- **Rvalue references can bind to rvalues** (temporaries, `std::move()` results)
- **Rvalue references CANNOT bind to lvalues** (named variables)
- Exception: You can use `std::move()` to convert an lvalue to an rvalue reference

Examples:
```cpp
int x = 10;
int&& r1 = 42;        // OK: binds to rvalue
int&& r2 = x;         // ERROR: cannot bind lvalue to rvalue reference
int&& r3 = std::move(x); // OK: std::move converts lvalue to rvalue reference
```

### 4. Return Value Optimization (RVO)

**Answer:**
- **RVO** is a compiler optimization that eliminates copies/moves when returning by value
- The compiler constructs the return value directly in the caller's location
- **Named RVO (NRVO)**: When returning a named local variable
- **Temporary RVO**: When returning a temporary directly
- RVO is **mandatory in C++17** for certain cases (prvalues)
- When RVO applies, neither copy nor move constructors are called
- RVO is preferred over move semantics when possible

## Key Takeaways

1. **`std::move()` is just a cast** - it doesn't move anything itself
2. **Moves are preferred for rvalues** - more efficient than copies
3. **Const objects cannot be moved** - they must be copied
4. **Passing by reference avoids copies/moves** - use when you don't need ownership
5. **Return by value is efficient** - RVO + move semantics make it cheap
6. **Containers use move semantics** - pass rvalues or use `std::move()` for efficiency
7. **Moved-from objects are valid but unspecified** - can be destroyed or reassigned, but values are undefined

## Common Mistakes

- Thinking `std::move()` performs the move (it's just a cast)
- Trying to use moved-from objects as if they still have their original values
- Not using `std::move()` when transferring ownership to containers
- Passing by value when you could pass by reference (unnecessary copies/moves)
- Expecting moves to work with const objects

