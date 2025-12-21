#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include <cstring>

// ============================================================================
// Simulated Resource Types
// ============================================================================

// Simulated file handle
class FileHandle {
private:
    int fd_;
    static int next_fd_;

public:
    FileHandle(const char* filename) : fd_(-1) {
        if (!filename || strlen(filename) == 0) {
            throw std::invalid_argument("Invalid filename");
        }
        fd_ = next_fd_++;
        std::cout << "    FileHandle: Opened file '" << filename << "' (fd=" << fd_ << ")\n";
    }

    ~FileHandle() {
        if (fd_ >= 0) {
            std::cout << "    FileHandle: Closed file (fd=" << fd_ << ")\n";
            fd_ = -1;
        }
    }

    // Delete copy operations
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    // Move operations
    FileHandle(FileHandle&& other) noexcept : fd_(other.fd_) {
        other.fd_ = -1;
    }

    FileHandle& operator=(FileHandle&& other) noexcept {
        if (this != &other) {
            if (fd_ >= 0) {
                std::cout << "    FileHandle: Closed file during move (fd=" << fd_ << ")\n";
            }
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }

    bool is_valid() const { return fd_ >= 0; }
    int get_fd() const { return fd_; }

    void write(const char* data) {
        if (!is_valid()) {
            throw std::runtime_error("FileHandle: Cannot write to invalid handle");
        }
        std::cout << "    FileHandle: Writing '" << data << "' to fd=" << fd_ << "\n";
    }
};

int FileHandle::next_fd_ = 1;

// Simulated memory buffer
class Buffer {
private:
    size_t size_;
    char* data_;
    static int instance_count_;

public:
    explicit Buffer(size_t size) : size_(size), data_(nullptr) {
        if (size == 0) {
            throw std::invalid_argument("Buffer size must be > 0");
        }
        data_ = new char[size_];
        std::fill(data_, data_ + size_, 0);
        ++instance_count_;
        std::cout << "    Buffer: Allocated " << size_ << " bytes (instances: " << instance_count_ << ")\n";
    }

    ~Buffer() {
        if (data_) {
            delete[] data_;
            --instance_count_;
            std::cout << "    Buffer: Deallocated " << size_ << " bytes (instances: " << instance_count_ << ")\n";
        }
    }

    // Copy constructor (deep copy)
    Buffer(const Buffer& other) : size_(other.size_), data_(nullptr) {
        if (other.data_) {
            data_ = new char[size_];
            std::memcpy(data_, other.data_, size_);
            ++instance_count_;
            std::cout << "    Buffer: Copied " << size_ << " bytes (instances: " << instance_count_ << ")\n";
        }
    }

    // Copy assignment
    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            Buffer temp(other);  // Strong exception safety: use copy-then-swap
            swap(temp);
        }
        return *this;
    }

    // Move constructor
    Buffer(Buffer&& other) noexcept : size_(other.size_), data_(other.data_) {
        other.size_ = 0;
        other.data_ = nullptr;
        std::cout << "    Buffer: Moved " << size_ << " bytes\n";
    }

    // Move assignment
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            if (data_) {
                delete[] data_;
                --instance_count_;
            }
            size_ = other.size_;
            data_ = other.data_;
            other.size_ = 0;
            other.data_ = nullptr;
        }
        return *this;
    }

    void swap(Buffer& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(data_, other.data_);
    }

    bool is_valid() const { return data_ != nullptr && size_ > 0; }
    size_t size() const { return size_; }
    char* data() { return data_; }
    const char* data() const { return data_; }
};

int Buffer::instance_count_ = 0;

// ============================================================================
// RAII Resource Wrapper with Exception Safety
// ============================================================================

template<typename Resource>
class ResourceWrapper {
private:
    std::unique_ptr<Resource> resource_;

public:
    // Constructor with resource acquisition
    template<typename... Args>
    explicit ResourceWrapper(Args&&... args) 
        : resource_(nullptr) {
        try {
            resource_ = std::make_unique<Resource>(std::forward<Args>(args)...);
        } catch (...) {
            // Resource acquisition failed, resource_ remains nullptr
            // No cleanup needed since nothing was acquired
            throw;  // Re-throw the exception
        }
    }

    // Move constructor (noexcept)
    ResourceWrapper(ResourceWrapper&& other) noexcept 
        : resource_(std::move(other.resource_)) {
        // other.resource_ is now nullptr
    }

    // Move assignment (noexcept)
    ResourceWrapper& operator=(ResourceWrapper&& other) noexcept {
        if (this != &other) {
            // Release current resource
            resource_.reset();
            // Transfer ownership
            resource_ = std::move(other.resource_);
            // other.resource_ is now nullptr
        }
        return *this;
    }

    // Delete copy operations (you could implement deep copy if Resource is copyable)
    ResourceWrapper(const ResourceWrapper&) = delete;
    ResourceWrapper& operator=(const ResourceWrapper&) = delete;

    // Destructor (noexcept) - ensures cleanup even if exceptions are active
    ~ResourceWrapper() noexcept {
        // std::unique_ptr destructor is noexcept
        // Resource destructor should ideally be noexcept too
        resource_.reset();
    }

    // Check if resource is valid
    bool is_valid() const noexcept {
        return resource_ != nullptr && resource_->is_valid();
    }

    // Get raw pointer (non-owning)
    Resource* get() noexcept {
        return resource_.get();
    }

    const Resource* get() const noexcept {
        return resource_.get();
    }

    // Dereference operators
    Resource& operator*() {
        if (!resource_) {
            throw std::runtime_error("ResourceWrapper: Cannot dereference null resource");
        }
        return *resource_;
    }

    const Resource& operator*() const {
        if (!resource_) {
            throw std::runtime_error("ResourceWrapper: Cannot dereference null resource");
        }
        return *resource_;
    }

    // Arrow operator
    Resource* operator->() {
        if (!resource_) {
            throw std::runtime_error("ResourceWrapper: Cannot access null resource");
        }
        return resource_.get();
    }

    const Resource* operator->() const {
        if (!resource_) {
            throw std::runtime_error("ResourceWrapper: Cannot access null resource");
        }
        return resource_.get();
    }

    // Release resource ownership (returns raw pointer, caller responsible for cleanup)
    Resource* release() noexcept {
        return resource_.release();
    }

    // Reset resource
    template<typename... Args>
    void reset(Args&&... args) {
        resource_.reset();
        try {
            resource_ = std::make_unique<Resource>(std::forward<Args>(args)...);
        } catch (...) {
            // Reset failed, resource_ is nullptr
            throw;
        }
    }
};

// ============================================================================
// Test Functions
// ============================================================================

void test_basic_raii() {
    std::cout << "=== Test 1: Basic RAII ===\n";
    {
        std::cout << "  Creating ResourceWrapper with FileHandle:\n";
        ResourceWrapper<FileHandle> file("test.txt");
        std::cout << "  File is valid: " << (file.is_valid() ? "yes" : "no") << "\n";
        if (file.is_valid()) {
            file->write("Hello, World!");
        }
        std::cout << "  Leaving scope (resource should be cleaned up)...\n";
    }
    std::cout << "  Resource cleaned up.\n\n";
}

void test_move_semantics() {
    std::cout << "=== Test 2: Move Semantics ===\n";
    {
        std::cout << "  Creating ResourceWrapper:\n";
        ResourceWrapper<FileHandle> file1("move_test.txt");
        std::cout << "  Moving to file2:\n";
        ResourceWrapper<FileHandle> file2 = std::move(file1);
        std::cout << "  file1 is valid: " << (file1.is_valid() ? "yes" : "no") << "\n";
        std::cout << "  file2 is valid: " << (file2.is_valid() ? "yes" : "no") << "\n";
        
        if (file2.is_valid()) {
            file2->write("Moved data");
        }
    }
    std::cout << "  Resources cleaned up.\n\n";
}

void test_exception_safety() {
    std::cout << "=== Test 3: Exception Safety ===\n";
    {
        try {
            std::cout << "  Attempting to create resource with invalid arguments:\n";
            ResourceWrapper<FileHandle> file("");  // Should throw
        } catch (const std::exception& e) {
            std::cout << "  Caught exception: " << e.what() << "\n";
            std::cout << "  No resource leak (no resource was acquired)\n";
        }

        try {
            std::cout << "  Creating valid resource:\n";
            ResourceWrapper<FileHandle> file("exception_test.txt");
            std::cout << "  Simulating exception during operation:\n";
            throw std::runtime_error("Operation failed");
            // This line won't execute, but resource will still be cleaned up
        } catch (const std::exception& e) {
            std::cout << "  Caught exception: " << e.what() << "\n";
            std::cout << "  Resource was properly cleaned up despite exception\n";
        }
    }
    std::cout << "\n";
}

void test_copyable_resource() {
    std::cout << "=== Test 4: Copyable Resource (Buffer) ===\n";
    {
        std::cout << "  Creating buffer:\n";
        ResourceWrapper<Buffer> buf1(1024);
        std::cout << "  Buffer size: " << buf1->size() << "\n";
        
        // Note: ResourceWrapper itself is not copyable, but we can work with the buffer
        std::cout << "  Accessing buffer data:\n";
        std::strcpy(buf1->data(), "Hello");
        std::cout << "  Buffer contains: " << buf1->data() << "\n";
    }
    std::cout << "  Buffer cleaned up.\n\n";
}

void test_move_assignment() {
    std::cout << "=== Test 5: Move Assignment ===\n";
    {
        ResourceWrapper<FileHandle> file1("file1.txt");
        ResourceWrapper<FileHandle> file2("file2.txt");
        
        std::cout << "  Before move assignment:\n";
        std::cout << "    file1 valid: " << (file1.is_valid() ? "yes" : "no") << "\n";
        std::cout << "    file2 valid: " << (file2.is_valid() ? "yes" : "no") << "\n";
        
        file2 = std::move(file1);
        
        std::cout << "  After move assignment:\n";
        std::cout << "    file1 valid: " << (file1.is_valid() ? "yes" : "no") << "\n";
        std::cout << "    file2 valid: " << (file2.is_valid() ? "yes" : "no") << "\n";
        
        // Test self-assignment
        file2 = std::move(file2);
        std::cout << "  After self move assignment, file2 valid: " 
                  << (file2.is_valid() ? "yes" : "no") << "\n";
    }
    std::cout << "\n";
}

void test_reset() {
    std::cout << "=== Test 6: Reset Resource ===\n";
    {
        ResourceWrapper<FileHandle> file("old_file.txt");
        std::cout << "  Original file valid: " << (file.is_valid() ? "yes" : "no") << "\n";
        
        std::cout << "  Resetting to new file:\n";
        file.reset("new_file.txt");
        std::cout << "  New file valid: " << (file.is_valid() ? "yes" : "no") << "\n";
        
        if (file.is_valid()) {
            file->write("Reset data");
        }
    }
    std::cout << "\n";
}

void test_release() {
    std::cout << "=== Test 7: Release Resource ===\n";
    {
        ResourceWrapper<FileHandle> file("release_test.txt");
        std::cout << "  File valid before release: " << (file.is_valid() ? "yes" : "no") << "\n";
        
        FileHandle* raw = file.release();
        std::cout << "  File valid after release: " << (file.is_valid() ? "yes" : "no") << "\n";
        std::cout << "  Raw pointer valid: " << (raw && raw->is_valid() ? "yes" : "no") << "\n";
        
        // Manual cleanup of released resource
        delete raw;
        std::cout << "  Manually deleted released resource\n";
    }
    std::cout << "\n";
}

int main() {
    std::cout << "=== Exercise 4: RAII & Exception Safety ===\n\n";

    test_basic_raii();
    test_move_semantics();
    test_exception_safety();
    test_copyable_resource();
    test_move_assignment();
    test_reset();
    test_release();

    std::cout << "All tests completed!\n";
    return 0;
}

