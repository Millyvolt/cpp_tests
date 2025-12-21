#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <iterator>
#include <type_traits>

// ============================================================================
// Custom Fixed-Size Array Container with Random Access Iterator
// ============================================================================

template<typename T, size_t N>
class FixedArray {
private:
    T data_[N];

public:
    // Iterator class (nested)
    class iterator {
    private:
        T* ptr_;

    public:
        // Iterator traits (C++17 approach)
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        // Constructor
        explicit iterator(T* ptr) : ptr_(ptr) {}

        // Dereference operators
        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }
        reference operator[](difference_type n) const { return ptr_[n]; }

        // Increment operators
        iterator& operator++() {
            ++ptr_;
            return *this;
        }

        iterator operator++(int) {
            iterator temp = *this;
            ++ptr_;
            return temp;
        }

        // Decrement operators
        iterator& operator--() {
            --ptr_;
            return *this;
        }

        iterator operator--(int) {
            iterator temp = *this;
            --ptr_;
            return temp;
        }

        // Arithmetic operators
        iterator& operator+=(difference_type n) {
            ptr_ += n;
            return *this;
        }

        iterator& operator-=(difference_type n) {
            ptr_ -= n;
            return *this;
        }

        iterator operator+(difference_type n) const {
            return iterator(ptr_ + n);
        }

        iterator operator-(difference_type n) const {
            return iterator(ptr_ - n);
        }

        difference_type operator-(const iterator& other) const {
            return ptr_ - other.ptr_;
        }

        // Comparison operators
        bool operator==(const iterator& other) const {
            return ptr_ == other.ptr_;
        }

        bool operator!=(const iterator& other) const {
            return ptr_ != other.ptr_;
        }

        bool operator<(const iterator& other) const {
            return ptr_ < other.ptr_;
        }

        bool operator>(const iterator& other) const {
            return ptr_ > other.ptr_;
        }

        bool operator<=(const iterator& other) const {
            return ptr_ <= other.ptr_;
        }

        bool operator>=(const iterator& other) const {
            return ptr_ >= other.ptr_;
        }
    };

    // Const iterator class
    class const_iterator {
    private:
        const T* ptr_;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        explicit const_iterator(const T* ptr) : ptr_(ptr) {}

        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }
        reference operator[](difference_type n) const { return ptr_[n]; }

        const_iterator& operator++() {
            ++ptr_;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator temp = *this;
            ++ptr_;
            return temp;
        }

        const_iterator& operator--() {
            --ptr_;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator temp = *this;
            --ptr_;
            return temp;
        }

        const_iterator& operator+=(difference_type n) {
            ptr_ += n;
            return *this;
        }

        const_iterator& operator-=(difference_type n) {
            ptr_ -= n;
            return *this;
        }

        const_iterator operator+(difference_type n) const {
            return const_iterator(ptr_ + n);
        }

        const_iterator operator-(difference_type n) const {
            return const_iterator(ptr_ - n);
        }

        difference_type operator-(const const_iterator& other) const {
            return ptr_ - other.ptr_;
        }

        bool operator==(const const_iterator& other) const {
            return ptr_ == other.ptr_;
        }

        bool operator!=(const const_iterator& other) const {
            return ptr_ != other.ptr_;
        }

        bool operator<(const const_iterator& other) const {
            return ptr_ < other.ptr_;
        }

        bool operator>(const const_iterator& other) const {
            return ptr_ > other.ptr_;
        }

        bool operator<=(const const_iterator& other) const {
            return ptr_ <= other.ptr_;
        }

        bool operator>=(const const_iterator& other) const {
            return ptr_ >= other.ptr_;
        }
    };

    // Type aliases
    using value_type = T;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    // Constructors
    FixedArray() {
        std::fill(data_, data_ + N, T{});
    }

    FixedArray(std::initializer_list<T> init) {
        size_t i = 0;
        for (const auto& val : init) {
            if (i < N) {
                data_[i++] = val;
            } else {
                break;
            }
        }
        // Fill remaining with default value
        while (i < N) {
            data_[i++] = T{};
        }
    }

    // Accessors
    reference operator[](size_type index) { return data_[index]; }
    const_reference operator[](size_type index) const { return data_[index]; }
    
    reference at(size_type index) {
        if (index >= N) {
            throw std::out_of_range("FixedArray::at: index out of range");
        }
        return data_[index];
    }

    const_reference at(size_type index) const {
        if (index >= N) {
            throw std::out_of_range("FixedArray::at: index out of range");
        }
        return data_[index];
    }

    // Iterators
    iterator begin() { return iterator(data_); }
    iterator end() { return iterator(data_ + N); }
    const_iterator begin() const { return const_iterator(data_); }
    const_iterator end() const { return const_iterator(data_ + N); }
    const_iterator cbegin() const { return const_iterator(data_); }
    const_iterator cend() const { return const_iterator(data_ + N); }

    // Capacity
    constexpr size_type size() const { return N; }
    constexpr bool empty() const { return N == 0; }
    constexpr size_type max_size() const { return N; }

    // Operations
    void fill(const T& value) {
        std::fill(data_, data_ + N, value);
    }
};

// Non-member arithmetic operators for iterators
template<typename T, size_t N>
typename FixedArray<T, N>::iterator operator+(
    typename FixedArray<T, N>::iterator::difference_type n,
    const typename FixedArray<T, N>::iterator& it) {
    return it + n;
}

template<typename T, size_t N>
typename FixedArray<T, N>::const_iterator operator+(
    typename FixedArray<T, N>::const_iterator::difference_type n,
    const typename FixedArray<T, N>::const_iterator& it) {
    return it + n;
}

// ============================================================================
// Test Functions
// ============================================================================

void test_basic_operations() {
    std::cout << "=== Test 1: Basic Iterator Operations ===\n";
    
    FixedArray<int, 5> arr{10, 20, 30, 40, 50};

    std::cout << "  Using range-based for loop:\n  ";
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << "\n";

    std::cout << "  Using iterator manually:\n  ";
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n\n";
}

void test_stl_algorithms() {
    std::cout << "=== Test 2: STL Algorithm Compatibility ===\n";
    
    FixedArray<int, 7> arr{3, 1, 4, 1, 5, 9, 2};

    std::cout << "  Original array: ";
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << "\n";

    // Find
    auto it = std::find(arr.begin(), arr.end(), 5);
    if (it != arr.end()) {
        std::cout << "  Found 5 at index: " << (it - arr.begin()) << "\n";
    }

    // Count
    size_t count = std::count(arr.begin(), arr.end(), 1);
    std::cout << "  Count of 1: " << count << "\n";

    // Accumulate
    int sum = std::accumulate(arr.begin(), arr.end(), 0);
    std::cout << "  Sum: " << sum << "\n";

    // For each
    std::cout << "  Doubled values: ";
    std::for_each(arr.begin(), arr.end(), [](int& x) { x *= 2; });
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << "\n\n";
}

void test_iterator_arithmetic() {
    std::cout << "=== Test 3: Iterator Arithmetic ===\n";
    
    FixedArray<char, 6> arr{'a', 'b', 'c', 'd', 'e', 'f'};

    auto begin = arr.begin();
    auto end = arr.end();

    std::cout << "  Distance: " << (end - begin) << "\n";
    std::cout << "  First element: " << *begin << "\n";
    std::cout << "  Third element: " << begin[2] << "\n";
    std::cout << "  Last element: " << *(end - 1) << "\n";

    auto it = begin + 3;
    std::cout << "  begin + 3: " << *it << "\n";

    it -= 1;
    std::cout << "  after -= 1: " << *it << "\n";

    std::cout << "  Comparison (begin < end): " << (begin < end ? "true" : "false") << "\n";
    std::cout << "  Comparison (begin >= end): " << (begin >= end ? "true" : "false") << "\n\n";
}

void test_transform_algorithm() {
    std::cout << "=== Test 4: Transform Algorithm ===\n";
    
    FixedArray<int, 5> arr{1, 2, 3, 4, 5};

    std::cout << "  Original: ";
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << "\n";

    // Transform in-place
    std::transform(arr.begin(), arr.end(), arr.begin(),
                   [](int x) { return x * x; });

    std::cout << "  Squared: ";
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << "\n";

    // Transform to another container
    std::vector<int> doubled;
    std::transform(arr.begin(), arr.end(), std::back_inserter(doubled),
                   [](int x) { return x * 2; });

    std::cout << "  Doubled (in vector): ";
    for (const auto& elem : doubled) {
        std::cout << elem << " ";
    }
    std::cout << "\n\n";
}

void test_const_iterators() {
    std::cout << "=== Test 5: Const Iterators ===\n";
    
    const FixedArray<std::string, 3> arr{"apple", "banana", "cherry"};

    std::cout << "  Using const_iterator:\n  ";
    for (auto it = arr.cbegin(); it != arr.cend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";

    std::cout << "  Using range-based for (const):\n  ";
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << "\n\n";
}

void test_sort_algorithm() {
    std::cout << "=== Test 6: Sort Algorithm ===\n";
    
    FixedArray<int, 8> arr{64, 34, 25, 12, 22, 11, 90, 5};

    std::cout << "  Before sort: ";
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << "\n";

    std::sort(arr.begin(), arr.end());

    std::cout << "  After sort: ";
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << "\n\n";
}

void test_reverse_iterator() {
    std::cout << "=== Test 7: Reverse Iteration ===\n";
    
    FixedArray<int, 5> arr{1, 2, 3, 4, 5};

    std::cout << "  Forward: ";
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << "\n";

    std::cout << "  Reverse: ";
    std::reverse(arr.begin(), arr.end());
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << "\n\n";
}

void test_iterator_traits() {
    std::cout << "=== Test 8: Iterator Traits ===\n";
    
    FixedArray<double, 4> arr{1.1, 2.2, 3.3, 4.4};
    auto it = arr.begin();

    // Verify iterator traits
    using traits = std::iterator_traits<decltype(it)>;
    std::cout << "  Iterator category: Random Access Iterator\n";
    std::cout << "  Value type: " << typeid(typename traits::value_type).name() << "\n";
    std::cout << "  Difference type: " << typeid(typename traits::difference_type).name() << "\n";
    std::cout << "  Pointer type: " << typeid(typename traits::pointer).name() << "\n";
    std::cout << "  Reference type: " << typeid(typename traits::reference).name() << "\n\n";
}

int main() {
    std::cout << "=== Exercise 5: Advanced STL Usage - Custom Iterator ===\n\n";

    test_basic_operations();
    test_stl_algorithms();
    test_iterator_arithmetic();
    test_transform_algorithm();
    test_const_iterators();
    test_sort_algorithm();
    test_reverse_iterator();
    test_iterator_traits();

    std::cout << "All tests completed!\n";
    return 0;
}

