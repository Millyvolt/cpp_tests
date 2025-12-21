#include <iostream>
#include <string>

class Base {
private:
    std::string name_;
    static int instance_count_;

public:
    Base(const std::string& name) : name_(name) {
        ++instance_count_;
        std::cout << "Base(" << name_ << ") constructed. Total instances: " 
                  << instance_count_ << std::endl;
    }

    virtual ~Base() {
        --instance_count_;
        std::cout << "Base(" << name_ << ") destructed. Remaining instances: " 
                  << instance_count_ << std::endl;
    }

    virtual void show() const {
        std::cout << "Base::show() - name: " << name_ << std::endl;
    }

    static int getInstanceCount() {
        return instance_count_;
    }
};

int Base::instance_count_ = 0;

class Derived1 : public virtual Base {
private:
    int value1_;

public:
    Derived1(const std::string& name, int v1) 
        : Base(name + "_D1"), value1_(v1) {
        std::cout << "Derived1(" << name << ", " << v1 << ") constructed" << std::endl;
    }

    ~Derived1() override {
        std::cout << "Derived1 destructed" << std::endl;
    }

    void show() const override {
        std::cout << "Derived1::show() - value1: " << value1_ << std::endl;
        Base::show();
    }
};

class Derived2 : public virtual Base {
private:
    int value2_;

public:
    Derived2(const std::string& name, int v2) 
        : Base(name + "_D2"), value2_(v2) {
        std::cout << "Derived2(" << name << ", " << v2 << ") constructed" << std::endl;
    }

    ~Derived2() override {
        std::cout << "Derived2 destructed" << std::endl;
    }

    void show() const override {
        std::cout << "Derived2::show() - value2: " << value2_ << std::endl;
        Base::show();
    }
};

class Final : public Derived1, public Derived2 {
private:
    int finalValue_;

public:
    Final(const std::string& name, int v1, int v2, int fv)
        : Base(name),
          Derived1(name, v1),
          Derived2(name, v2),
          finalValue_(fv) {
        std::cout << "Final(" << name << ", " << v1 << ", " << v2 << ", " << fv 
                  << ") constructed" << std::endl;
    }

    ~Final() override {
        std::cout << "Final destructed" << std::endl;
    }

    void show() const override {
        std::cout << "Final::show() - finalValue: " << finalValue_ << std::endl;
        Derived1::show();
        Derived2::show();
    }
};

int main() {
    std::cout << "=== Creating Final object ===" << std::endl;
    Final obj("Test", 10, 20, 30);
    
    std::cout << "\n=== Instance count ===" << std::endl;
    std::cout << "Base instances: " << Base::getInstanceCount() << std::endl;
    
    std::cout << "\n=== Object sizes ===" << std::endl;
    std::cout << "sizeof(Base): " << sizeof(Base) << std::endl;
    std::cout << "sizeof(Derived1): " << sizeof(Derived1) << std::endl;
    std::cout << "sizeof(Derived2): " << sizeof(Derived2) << std::endl;
    std::cout << "sizeof(Final): " << sizeof(Final) << std::endl;
    
    std::cout << "\n=== Virtual function calls ===" << std::endl;
    Base* basePtr = &obj;
    Derived1* d1Ptr = &obj;
    Derived2* d2Ptr = &obj;
    
    std::cout << "Calling through Base*:" << std::endl;
    basePtr->show();
    
    std::cout << "\nCalling through Derived1*:" << std::endl;
    d1Ptr->show();
    
    std::cout << "\nCalling through Derived2*:" << std::endl;
    d2Ptr->show();
    
    std::cout << "\n=== Destruction begins ===" << std::endl;
    return 0;
}

