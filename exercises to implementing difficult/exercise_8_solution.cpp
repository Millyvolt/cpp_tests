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
    return temp;  // RVO may apply here, or move if RVO doesn't apply
}

int main() {
    TrackedResource::resetCounters();
    
    std::cout << "=== Test 1: Direct construction ===" << std::endl;
    TrackedResource obj1(10);
    
    std::cout << "\n=== Test 2: Copy construction ===" << std::endl;
    TrackedResource obj2 = obj1;
    
    std::cout << "\n=== Test 3: Move construction ===" << std::endl;
    TrackedResource obj3 = std::move(obj1);
    std::cout << "  Note: obj1 is now in moved-from state (id = -1)" << std::endl;
    
    std::cout << "\n=== Test 4: Function by value (lvalue) ===" << std::endl;
    TrackedResource::resetCounters();
    TrackedResource obj4(20);
    funcByValue(obj4);
    TrackedResource::printCounters();
    
    std::cout << "\n=== Test 5: Function by value (rvalue) ===" << std::endl;
    TrackedResource::resetCounters();
    funcByValue(TrackedResource(30));
    TrackedResource::printCounters();
    
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
    std::cout << "  Note: RVO may eliminate the copy/move, compiler dependent" << std::endl;
    
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
    std::cout << "  Note: Moving from moved-from object is safe, but object is in valid but unspecified state" << std::endl;
    
    std::cout << "\n=== Test 16: Const object ===" << std::endl;
    TrackedResource::resetCounters();
    const TrackedResource obj15(600);
    TrackedResource obj16 = std::move(obj15);
    TrackedResource::printCounters();
    

    
    return 0;
}

