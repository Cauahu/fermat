#ifndef FERMAT_COMMON_STACK_BUFFER_H_
#define FERMAT_COMMON_STACK_BUFFER_H_
#include <fermat/common/basic_buffer.h>
#include <memory>
#include <utility>

namespace fermat {

template <typename T, std::size_t SIZE, typename Allocator = std::allocator<T> >
class StackBuffer: private Allocator, public BasicBuffer<T> {
private:
    T _data[SIZE];

    // Deallocate memory allocated by the buffer.
    void deallocate()
    {
        if (this->_ptr != _data) Allocator::deallocate(this->_ptr, this->_capacity);
    }

protected:
    void grow(std::size_t size);

public:
    explicit StackBuffer(const Allocator &alloc = Allocator())
        : Allocator(alloc), BasicBuffer<T>(_data, SIZE)
    {}
    ~StackBuffer()
    {
        deallocate();
    }

private:
    // Move data from other to this buffer.
    void move(StackBuffer &other)
    {
        Allocator &this_alloc = *this, &other_alloc = other;
        this_alloc = std::move(other_alloc);
        this->_size = other._size;
        this->_capacity = other._capacity;
        if (other._ptr == other._data) {
            this->_ptr = _data;
            std::uninitialized_copy(other._data, other._data + this->_size, _data);
        } else {
            this->_ptr = other._ptr;
            // Set pointer to the inline array so that delete is not called
            // when deallocating.
            other._ptr = other._data;
        }
    }

public:
    StackBuffer(StackBuffer &&other)
    {
        move(other);
    }

    StackBuffer &operator=(StackBuffer &&other)
    {
        assert(this != &other);
        deallocate();
        move(other);
        return *this;
    }

    // Returns a copy of the allocator associated with this buffer.
    Allocator get_allocator() const
    {
        return *this;
    }

    size_t stack_size() const
    {
        return SIZE;
    }

    bool is_stack() const 
    {
        return this->_ptr == _data;
    }

    void drain(size_t len)
    {
        if(this->_size + len > this->_capacity) {
            this->_size = this->_capacity;
            return ;
        }
        this->_size += len;
    }

};

template <typename T, std::size_t SIZE, typename Allocator>
void StackBuffer<T, SIZE, Allocator>::grow(std::size_t len)
{
    std::size_t new_capacity = this->_capacity + this->_capacity / 2;
    if (len > new_capacity)
        new_capacity = len;
    T *new_ptr = this->allocate(new_capacity);
    // The following code doesn't throw, so the raw pointer above doesn't leak.
    std::uninitialized_copy(this->_ptr, this->_ptr + this->_size, new_ptr);
    std::size_t old_capacity = this->_capacity;
    T *old_ptr = this->_ptr;
    this->_capacity = new_capacity;
    this->_ptr = new_ptr;
    // deallocate may throw (at least in principle), but it doesn't matter since
    // the buffer already uses the new storage and will deallocate it in case
    // of exception.
    if (old_ptr != _data)
        Allocator::deallocate(old_ptr, old_capacity);
}

} //namespace fermat
#endif
