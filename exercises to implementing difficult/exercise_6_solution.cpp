#include <iostream>
#include <string>
#include <vector>

// ============================================================================
// Base Class: Animal
// ============================================================================

class Animal {
protected:
    std::string name_;
    static int instance_count_;

public:
    explicit Animal(const std::string& name) : name_(name) {
        ++instance_count_;
        std::cout << "    Animal(" << name_ << ") constructed (total instances: " 
                  << instance_count_ << ")\n";
    }

    virtual ~Animal() {
        --instance_count_;
        std::cout << "    Animal(" << name_ << ") destructed (remaining instances: " 
                  << instance_count_ << ")\n";
    }

    virtual void makeSound() const {
        std::cout << "    " << name_ << " makes a generic animal sound\n";
    }

    std::string getName() const {
        return name_;
    }

    static int getInstanceCount() {
        return instance_count_;
    }

    static void resetInstanceCount() {
        instance_count_ = 0;
    }
};

int Animal::instance_count_ = 0;

// ============================================================================
// Intermediate Classes WITHOUT Virtual Inheritance (Demonstrates Problem)
// ============================================================================

class MammalBad : public Animal {
protected:
    int bodyTemperature_;

public:
    MammalBad(const std::string& name, int temp = 37) 
        : Animal(name), bodyTemperature_(temp) {
        std::cout << "    MammalBad(" << name_ << ") constructed\n";
    }

    ~MammalBad() override {
        std::cout << "    MammalBad(" << name_ << ") destructed\n";
    }

    void makeSound() const override {
        std::cout << "    " << name_ << " makes a mammal sound (temperature: " 
                  << bodyTemperature_ << "°C)\n";
    }

    int getBodyTemperature() const {
        return bodyTemperature_;
    }
};

class WingedBad : public Animal {
protected:
    int wingSpan_;

public:
    WingedBad(const std::string& name, int span = 100) 
        : Animal(name), wingSpan_(span) {
        std::cout << "    WingedBad(" << name_ << ") constructed\n";
    }

    ~WingedBad() override {
        std::cout << "    WingedBad(" << name_ << ") destructed\n";
    }

    void makeSound() const override {
        std::cout << "    " << name_ << " makes a winged creature sound (wingspan: " 
                  << wingSpan_ << "cm)\n";
    }

    int getWingSpan() const {
        return wingSpan_;
    }
};

// Problematic class - demonstrates diamond problem
class BatBad : public MammalBad, public WingedBad {
public:
    BatBad(const std::string& name, int temp = 35, int span = 50) 
        : MammalBad(name, temp), WingedBad(name, span) {
        std::cout << "    BatBad(" << name << ") constructed\n";
        std::cout << "    PROBLEM: Two Animal subobjects exist!\n";
        std::cout << "    Animal instances: " << Animal::getInstanceCount() << "\n";
    }

    ~BatBad() override {
        std::cout << "    BatBad destructed\n";
    }

    void makeSound() const override {
        // Ambiguous: which Animal::name_ to use?
        // Must use scope resolution
        std::cout << "    " << MammalBad::name_ << " (or " << WingedBad::name_ 
                  << ") makes a bat sound\n";
    }
};

// ============================================================================
// Intermediate Classes WITH Virtual Inheritance (Correct Solution)
// ============================================================================

class Mammal : public virtual Animal {
protected:
    int bodyTemperature_;

public:
    Mammal(const std::string& name, int temp = 37) 
        : Animal(name), bodyTemperature_(temp) {
        std::cout << "    Mammal(" << name << ") constructed\n";
    }

    ~Mammal() override {
        std::cout << "    Mammal destructed\n";
    }

    void makeSound() const override {
        std::cout << "    " << name_ << " makes a mammal sound (temperature: " 
                  << bodyTemperature_ << "°C)\n";
    }

    int getBodyTemperature() const {
        return bodyTemperature_;
    }
};

class Winged : public virtual Animal {
protected:
    int wingSpan_;

public:
    Winged(const std::string& name, int span = 100) 
        : Animal(name), wingSpan_(span) {
        std::cout << "    Winged(" << name << ") constructed\n";
    }

    ~Winged() override {
        std::cout << "    Winged destructed\n";
    }

    void makeSound() const override {
        std::cout << "    " << name_ << " makes a winged creature sound (wingspan: " 
                  << wingSpan_ << "cm)\n";
    }

    int getWingSpan() const {
        return wingSpan_;
    }
};

// ============================================================================
// Derived Class WITH Virtual Inheritance (Correct Solution)
// ============================================================================

class Bat : public Mammal, public Winged {
private:
    std::string echolocationFrequency_;

public:
    // IMPORTANT: Virtual base class must be initialized by the most derived class
    // Constructor initialization order: virtual bases → direct bases → members
    Bat(const std::string& name, int temp = 35, int span = 50, 
        const std::string& freq = "40kHz")
        : Animal(name),           // Initialize virtual base directly
          Mammal(name, temp),     // name passed but Animal already initialized
          Winged(name, span),     // name passed but Animal already initialized
          echolocationFrequency_(freq) {
        std::cout << "    Bat(" << name_ << ") constructed\n";
        std::cout << "    SUCCESS: Only one Animal subobject exists!\n";
        std::cout << "    Animal instances: " << Animal::getInstanceCount() << "\n";
    }

    ~Bat() override {
        std::cout << "    Bat destructed\n";
    }

    // Override makeSound() - no ambiguity!
    void makeSound() const override {
        std::cout << "    " << name_ << " squeaks! Uses echolocation at " 
                  << echolocationFrequency_ << "\n";
    }

    // Access members from both intermediate classes - no ambiguity
    void displayInfo() const {
        std::cout << "    Bat Info:\n";
        std::cout << "      Name: " << name_ << "\n";  // Direct access, no ambiguity
        std::cout << "      Body Temperature: " << bodyTemperature_ << "°C\n";
        std::cout << "      Wing Span: " << wingSpan_ << "cm\n";
        std::cout << "      Echolocation: " << echolocationFrequency_ << "\n";
    }

    const std::string& getEcholocationFrequency() const {
        return echolocationFrequency_;
    }
};

// ============================================================================
// Additional Interface-like Class (Mix-in pattern)
// ============================================================================

class Flyable {
public:
    virtual ~Flyable() = default;
    virtual void fly() const = 0;
    virtual int getMaxAltitude() const = 0;
};

class Nocturnal {
public:
    virtual ~Nocturnal() = default;
    virtual void huntAtNight() const = 0;
};

// Bat can implement multiple interfaces without virtual inheritance issues
class AdvancedBat : public Bat, public Flyable, public Nocturnal {
public:
    AdvancedBat(const std::string& name, int temp = 35, int span = 50, 
                const std::string& freq = "40kHz")
        : Bat(name, temp, span, freq) {
        std::cout << "    AdvancedBat(" << name << ") constructed\n";
    }

    ~AdvancedBat() override {
        std::cout << "    AdvancedBat destructed\n";
    }

    // Implement Flyable interface
    void fly() const override {
        std::cout << "    " << getName() << " is flying at night using echolocation\n";
    }

    int getMaxAltitude() const override {
        return 3000;  // meters
    }

    // Implement Nocturnal interface
    void huntAtNight() const override {
        std::cout << "    " << getName() << " is hunting insects at night\n";
    }
};

// ============================================================================
// Test Functions
// ============================================================================

void test_diamond_problem() {
    std::cout << "=== Test 1: Diamond Problem (Without Virtual Inheritance) ===\n";
    
    Animal::resetInstanceCount();
    
    {
        std::cout << "  Creating BatBad (NO virtual inheritance):\n";
        BatBad badBat("Problematic Bat", 35, 50);
        
        std::cout << "  Object size: " << sizeof(badBat) << " bytes\n";
        std::cout << "  Animal instances: " << Animal::getInstanceCount() << " (should be 2 - PROBLEM!)\n";
        
        // Ambiguous access - need scope resolution
        std::cout << "  Accessing name via MammalBad: " << badBat.MammalBad::getName() << "\n";
        std::cout << "  Accessing name via WingedBad: " << badBat.WingedBad::getName() << "\n";
        
        badBat.makeSound();
        std::cout << "  Body temp: " << badBat.getBodyTemperature() << "°C\n";
        std::cout << "  Wing span: " << badBat.getWingSpan() << "cm\n";
    }
    
    std::cout << "  Animal instances after destruction: " << Animal::getInstanceCount() << "\n\n";
}

void test_virtual_inheritance() {
    std::cout << "=== Test 2: Virtual Inheritance Solution ===\n";
    
    Animal::resetInstanceCount();
    
    {
        std::cout << "  Creating Bat (WITH virtual inheritance):\n";
        Bat bat("Vampire Bat", 35, 50, "45kHz");
        
        std::cout << "  Object size: " << sizeof(bat) << " bytes (larger due to virtual base pointers)\n";
        std::cout << "  Animal instances: " << Animal::getInstanceCount() << " (should be 1 - CORRECT!)\n";
        
        // No ambiguity - direct access works
        std::cout << "  Name: " << bat.getName() << "\n";
        bat.displayInfo();
        bat.makeSound();
    }
    
    std::cout << "  Animal instances after destruction: " << Animal::getInstanceCount() << "\n\n";
}

void test_polymorphism() {
    std::cout << "=== Test 3: Polymorphism with Virtual Inheritance ===\n";
    
    Bat bat("Brown Bat");
    
    // Polymorphic calls through different base class pointers
    Animal* animalPtr = &bat;
    Mammal* mammalPtr = &bat;
    Winged* wingedPtr = &bat;
    
    std::cout << "  Calling makeSound() through Animal*:\n  ";
    animalPtr->makeSound();
    
    std::cout << "  Calling makeSound() through Mammal*:\n  ";
    mammalPtr->makeSound();
    
    std::cout << "  Calling makeSound() through Winged*:\n  ";
    wingedPtr->makeSound();
    
    std::cout << "  All pointers point to the same object - virtual function resolution works!\n\n";
}

void test_constructor_order() {
    std::cout << "=== Test 4: Constructor Initialization Order ===\n";
    std::cout << "  Creating Bat to observe construction order:\n";
    std::cout << "  Expected order: Animal → Mammal → Winged → Bat\n";
    
    Animal::resetInstanceCount();
    Bat bat("Test Bat");
    
    std::cout << "  Construction complete!\n\n";
}

void test_mixed_inheritance() {
    std::cout << "=== Test 5: Multiple Inheritance with Interfaces ===\n";
    
    Animal::resetInstanceCount();
    
    {
        AdvancedBat advancedBat("Advanced Bat");
        std::cout << "  Animal instances: " << Animal::getInstanceCount() << "\n";
        
        // Use Bat interface
        advancedBat.makeSound();
        advancedBat.displayInfo();
        
        // Use Flyable interface
        Flyable* flyable = &advancedBat;
        flyable->fly();
        std::cout << "  Max altitude: " << flyable->getMaxAltitude() << "m\n";
        
        // Use Nocturnal interface
        Nocturnal* nocturnal = &advancedBat;
        nocturnal->huntAtNight();
    }
    
    std::cout << "  Animal instances after destruction: " << Animal::getInstanceCount() << "\n\n";
}

void test_object_layout() {
    std::cout << "=== Test 6: Object Size Comparison ===\n";
    
    std::cout << "  sizeof(Animal): " << sizeof(Animal) << " bytes\n";
    std::cout << "  sizeof(Mammal): " << sizeof(Mammal) << " bytes\n";
    std::cout << "  sizeof(Winged): " << sizeof(Winged) << " bytes\n";
    std::cout << "  sizeof(BatBad): " << sizeof(BatBad) << " bytes (no virtual inheritance)\n";
    std::cout << "  sizeof(Bat): " << sizeof(Bat) << " bytes (with virtual inheritance)\n";
    std::cout << "  Note: Virtual inheritance adds overhead (virtual base pointers)\n\n";
}

void test_destructor_order() {
    std::cout << "=== Test 7: Destructor Order (Reverse of Construction) ===\n";
    
    Animal::resetInstanceCount();
    std::cout << "  Creating and destroying Bat:\n";
    
    {
        Bat bat("Destructor Test Bat");
    }
    
    std::cout << "  Destruction complete!\n";
    std::cout << "  Animal instances: " << Animal::getInstanceCount() << "\n\n";
}

int main() {
    std::cout << "=== Exercise 6: Virtual Inheritance & Multiple Inheritance ===\n\n";

    test_diamond_problem();
    test_virtual_inheritance();
    test_polymorphism();
    test_constructor_order();
    test_mixed_inheritance();
    test_object_layout();
    test_destructor_order();

    std::cout << "All tests completed!\n";
    return 0;
}

