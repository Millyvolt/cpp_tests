# Exercise 6: Virtual Inheritance & Multiple Inheritance

## Problem Statement

Implement a class hierarchy that demonstrates the diamond problem in multiple inheritance and solve it using virtual inheritance. Create a scenario where multiple inheritance naturally occurs and show how virtual inheritance prevents ambiguity and ensures proper object layout.

## Requirements

1. **Create Base Class:**
   - Implement an `Animal` base class with:
     - A data member (e.g., `name_` or `species_`)
     - A constructor that initializes the data member
     - A virtual function `makeSound()` that can be overridden
     - A virtual destructor (essential for polymorphism)
     - A non-virtual member function to demonstrate access

2. **Create Intermediate Classes (Diamond Problem):**
   - Implement `Mammal` class that inherits from `Animal`
   - Implement `Winged` class that inherits from `Animal`
   - Both should override `makeSound()` with their own implementations
   - Both should add their own member data/functions

3. **Demonstrate Diamond Problem:**
   - Create a class `Bat` that inherits from both `Mammal` and `Winged`
   - Show the ambiguity that occurs without virtual inheritance:
     - Ambiguous access to base class members
     - Multiple copies of base class subobject
     - Constructor initialization issues
   - Implement a version WITHOUT virtual inheritance to demonstrate the problem

4. **Solve with Virtual Inheritance:**
   - Modify `Mammal` and `Winged` to use virtual inheritance from `Animal`
   - Fix the `Bat` class to properly initialize the virtual base
   - Show that ambiguity is resolved and only one `Animal` subobject exists
   - Demonstrate proper constructor initialization order

5. **Constructor Initialization Order:**
   - Show that virtual base classes are initialized first
   - Initialize virtual base classes from the most derived class
   - Properly handle initialization lists in all classes

6. **Virtual Function Overriding:**
   - Override `makeSound()` in `Bat` class
   - Show virtual function resolution works correctly
   - Demonstrate polymorphic behavior with base class pointers

## Example Usage

```cpp
// Without virtual inheritance (problematic)
class BatBad : public Mammal, public Winged {
    // Ambiguous: which Animal::name_ ?
    // Two Animal subobjects exist
};

// With virtual inheritance (correct)
class Bat : public Mammal, public Winged {
public:
    Bat(const std::string& name) 
        : Animal(name),  // Initialize virtual base directly
          Mammal(name, ...),
          Winged(name, ...) {
    }
};

// Usage
Bat bat("Vampire Bat");
bat.makeSound();  // Should call Bat::makeSound()

Animal* animal = &bat;
animal->makeSound();  // Polymorphic call

// Verify only one Animal subobject
std::cout << "Animal instances: " << Animal::getInstanceCount() << std::endl;  // Should be 1
```

## Constraints

- Must use C++17 or later
- All classes must have virtual destructors
- Constructor initialization lists must be correct
- Must compile without warnings
- Handle access to base class members correctly
- Show both problematic and correct implementations

## Hints

- Virtual inheritance syntax: `class Derived : public virtual Base`
- Virtual base classes are initialized by the most derived class, not intermediate classes
- Constructor initialization order: virtual bases → direct bases → members
- Use scope resolution (`::`) to disambiguate when needed (though virtual inheritance should eliminate most ambiguity)
- Consider adding instance counters to verify only one base subobject exists
- Use `sizeof` to demonstrate different object sizes with/without virtual inheritance

## Key Concepts to Demonstrate

- Virtual inheritance syntax and semantics
- Diamond problem and why it occurs
- Constructor initialization order with virtual bases
- Virtual function resolution in multiple inheritance
- Object layout differences (with/without virtual inheritance)
- When to use virtual inheritance vs regular inheritance
- Access control in multiple inheritance scenarios

## Testing Considerations

- Test object creation and destruction order
- Verify only one base class subobject exists (use counters or sizeof)
- Test virtual function calls through different base class pointers
- Test member access (should be unambiguous with virtual inheritance)
- Compare object sizes with and without virtual inheritance
- Test constructor initialization with virtual bases
- Verify proper cleanup with virtual destructors

## Additional Notes

- Virtual inheritance has a performance cost (indirection through virtual base pointer)
- Only use virtual inheritance when the diamond problem actually occurs
- Regular inheritance is preferred when not needed
- Virtual bases change object layout and constructor initialization semantics

