#include <algorithm>
#include <iterator>
#include <initializer_list>
#include <memory>
#include <stdexcept>

template<class T, class Allocator = std::allocator<T>>
class CircularBufferExt {
private:
    Allocator _allocator;
    T* _start_memory;
    size_t _head;
    size_t _tail;
    size_t _size;
    size_t _count;

    void resize() {
        size_t new_capacity = _count * 2;
        resize(new_capacity);
    }

public:
    using value_type = T;
    using size_type = size_t;
    using reference = value_type&;
    using pointer = value_type*;
    using const_reference = const value_type&;
    using const_pointer = const value_type*;
    using allocator_type = Allocator;
    using traits = std::allocator_traits<allocator_type>;


    CircularBufferExt() : CircularBufferExt(0) {}

    CircularBufferExt(size_type count) {
        _start_memory = _allocator.allocate(count);
        for (size_type i = 0; i < count; ++i) {
            traits::construct(_allocator, &_start_memory[i]);
        }
        _head = 0;
        _tail = 0;
        _size = 0;
        _count = count;
    }

    constexpr CircularBufferExt(const CircularBufferExt& other)
            : _start_memory(other._start_memory), _size(other.size_), _head(other.head_) {}

    constexpr CircularBufferExt(CircularBufferExt&& other) noexcept {
        _start_memory = std::allocator_traits<allocator_type>::allocate(_allocator, other._count);
        for (size_type i = 0; i < other._size; ++i) {
            _start_memory + i = other._start_memory;
        }
        _size = other._size;
        _head = other._head;
        _tail = other._tail;

        other._size = 0;
        other.capacity = 0;
        other._head = 0;
        other._end = 0;
    }


    CircularBufferExt(std::initializer_list<T> init_list) : CircularBufferExt(init_list.size()) {
        for (const auto& element: init_list) {
            push_back(element);
        }
    }

    // random access iterator
    class IteratorExt {
        friend class CircularBufferExt;

    private:
        T* _pointer;
        size_t _index; // for indexing in buffer (when overflow too)
        size_t _size_container;
    public:
        explicit IteratorExt(T* ptr, size_t index, size_t size_container) {
            _pointer = ptr;
            _index = index;
            _size_container = size_container;
        }

        IteratorExt(const IteratorExt& other) {
            _pointer = other._pointer;
            _index = other._index;
            _size_container = other._size_container;
        }

        reference operator*() const noexcept {
            return *(_pointer + _index);
        }

        pointer operator->() const noexcept {
            return _pointer + _index;
        }

        IteratorExt& operator++() noexcept {
            if (_index + 1 == _size_container) {
                _index = 0;
            } else {
                ++_index;
            }
            return *this;
        }

        IteratorExt operator++(int) noexcept {
            IteratorExt iterator(*this);
            if (iterator._index + 1 == _size_container) {
                iterator._index = 0;
            } else {
                ++iterator._index;
            }
            return iterator;
        }

        IteratorExt& operator--() noexcept {
            if (_index - 1 < 0) {
                _index = _size_container - 1;
            } else {
                --_index;
            }
            return *this;
        }

        IteratorExt operator--(int) noexcept {
            IteratorExt iterator(*this);
            --iterator._index;
            if (iterator._index < 0) {
                iterator._index = _size_container - 1;
            }
            return iterator;
        }

        IteratorExt operator+=(size_type number) noexcept {
            _index += number;
            if (_index >= _size_container) {
                _index -= _size_container;
            }
            return *(this);
        }

        IteratorExt operator-=(size_type number) noexcept {
            if (number > _index) {
                number -= _index;
                _index = _size_container - number;
            } else {
                _index -= number;
            }
            return *(this);
        }

        IteratorExt operator+(size_type number) noexcept {
            _index += number;
            if (_index >= _size_container) {
                _index -= _size_container;
            }
            return (*this);
        }

        IteratorExt operator-(const IteratorExt& last) noexcept {
            size_t temp = last._index;
            if (temp > _index) {
                temp -= _index;
                _index = _size_container - temp;
            } else {
                _index -= last._index;
            }
            return (*this);
        }

        IteratorExt operator-(size_type number) noexcept {
            if (number > _index) {
                number -= _index;
                _index = _size_container - number;
            } else {
                _index -= number;
            }
            return *(this);
        }

        reference operator[](size_type index) const noexcept {
            return *(_pointer + index);
        }

        bool operator==(const IteratorExt& other) const noexcept {
            return (_index == other._index and this->_pointer == other._pointer);
        }

        bool operator!=(const IteratorExt& other) const noexcept {
            return (_index != other._index);
        }

        bool operator>(const IteratorExt& other) const noexcept {
            return (_index > other._index);
        }

        bool operator<(const IteratorExt& other) const noexcept {
            return (_index < other._index);
        }

        bool operator>=(const IteratorExt& other) const noexcept {
            return (_index >= other._index);
        }

        bool operator<=(const IteratorExt& other) const noexcept {
            return (_index <= other._index);
        }
    };

    // implementing const random access iterator
    class ConstIteratorExt {
    private:
        const_pointer _pointer;
        size_type _index; // for indexing in buffer (when overflow too)
        size_type _size_container;
    public:
        explicit ConstIteratorExt(T* ptr, size_t index, size_t size_container) {
            _pointer = ptr;
            _index = index;
            _size_container = size_container;
        }

        ConstIteratorExt(const ConstIteratorExt& other) {
            _pointer = other._pointer;
            _index = other._index;
            _size_container = other._size_container;
        }

        const_reference operator*() const noexcept {
            return *(_pointer + _index);
        }

        const_pointer operator->() const noexcept {
            return _pointer + _index;
        }

        constexpr ConstIteratorExt& operator++() noexcept {
            if (_index + 1 == _size_container) {
                _index = 0;
            } else {
                ++_index;
            }
            return *this;
        }

        constexpr ConstIteratorExt operator++(int) noexcept {
            ConstIteratorExt iterator(*this);
            if (iterator._index + 1 == _size_container) {
                iterator._index = 0;
            } else {
                ++iterator._index;
            }
            return iterator;
        }

        constexpr ConstIteratorExt& operator--() noexcept {
            if (_index - 1 < 0) {
                _index = _size_container - 1;
            } else {
                --_index;
            }
            return *this;
        }

        constexpr ConstIteratorExt operator--(int) noexcept {
            ConstIteratorExt iterator(*this);
            --iterator._index;
            if (iterator._index < 0) {
                iterator._index = _size_container - 1;
            }
            return iterator;
        }

        constexpr ConstIteratorExt operator+=(size_t number) noexcept {
            _index += number;
            if (_index >= _size_container) {
                _index -= _size_container;
            }
            return *(this);
        }

        constexpr ConstIteratorExt operator-=(size_t number) noexcept {
            if (number > _index) {
                number -= _index;
                _index = _size_container - number;
            } else {
                _index -= number;
            }
            return *(this);
        }

        constexpr ConstIteratorExt operator+(size_type number) noexcept {
            _index += number;
            if (_index >= _size_container) {
                _index -= _size_container;
            }
            return (*this);
        }

        constexpr ConstIteratorExt operator-(const ConstIteratorExt& last) noexcept {
            size_type temp = last._index;
            if (temp > _index) {
                temp -= _index;
                _index = _size_container - temp;
            } else {
                _index -= last._index;
            }
            return (*this);
        }

        constexpr ConstIteratorExt operator-(size_type number) noexcept {
            if (number > _index) {
                number -= _index;
                _index = _size_container - number;
            } else {
                _index -= number;
            }
            return *(this);
        }

        const_reference operator[](size_t index) const noexcept {
            return *(_pointer + index);
        }

        bool operator==(const ConstIteratorExt& other) const noexcept {
            return (_index == other._index and this->_pointer == other._pointer);
        }

        bool operator!=(const ConstIteratorExt& other) const noexcept {
            return (_index != other._index);
        }

        bool operator>(const ConstIteratorExt& other) const noexcept {
            return (_index > other._index);
        }

        bool operator<(const ConstIteratorExt& other) const noexcept {
            return (_index < other._index);
        }

        bool operator>=(const ConstIteratorExt& other) const noexcept {
            return (_index >= other._index);
        }

        bool operator<=(const ConstIteratorExt& other) const noexcept {
            return (_index <= other._index);
        }
    };

    bool empty() const noexcept {
        return _size == 0;
    }

    size_type capacity() const noexcept {
        return _count;
    }

    size_type size() {
        return _size;
    }

    IteratorExt begin() noexcept {
        return IteratorExt(_start_memory, _head, _count);
    }

    ConstIteratorExt begin() const noexcept {
        return ConstIteratorExt(_start_memory, _head, _count);
    }

    IteratorExt end() noexcept {
        return IteratorExt(_start_memory, _tail - 1, _count);
    }

    ConstIteratorExt end() const noexcept {
        return ConstIteratorExt(_start_memory, _tail - 1, _count);
    }

    ConstIteratorExt cbegin() const noexcept {
        return ConstIteratorExt(_start_memory, _head, _count);
    }

    ConstIteratorExt cend() const noexcept {
        return ConstIteratorExt(_start_memory, _tail - 1, _count);
    }

    void resize(size_type new_capacity) {
        _count = new_capacity;
        pointer new_start_memory = _allocator.allocate(new_capacity);
        for (size_t i = 0; i < _count; ++i) {
            _allocator.construct(&new_start_memory[i]);
            new_start_memory[i] = _start_memory[i];
            _allocator.destroy(&_start_memory[i]);
        }
        _start_memory = new_start_memory;
    }

    template<typename Args>
    void push_back(Args&& arg) {
        if (_size == _count) {
            resize();
        }
        _start_memory[_tail] = arg;
        _tail = _tail + 1;
        ++_size;

    }

    template<typename... Args>
    IteratorExt emplace(const IteratorExt& position, Args&& ... args) noexcept {
        size_type index_to_insert = position._index;
        if (_size == _count) {
            resize();
        }

        for (size_type i = _count - 1; i > index_to_insert; --i) {
            _start_memory[i] = _start_memory[i - 1];
        }
        ++_tail;
        new(&_start_memory[index_to_insert]) T(std::forward<Args>(args)...);
        ++_size;
        return IteratorExt(_start_memory, index_to_insert, _size);

    }

    template<typename Arg>
    IteratorExt insert(IteratorExt position, int counter, Arg&& arg) noexcept {
        for (int i = 0; i < counter; ++i) {
            emplace(position, arg);
        }
        position._size_container = _size;
        return position;
    }

    reference front() const noexcept {
        return *(_start_memory + _head);
    }

    reference back() const noexcept {
        return *(_start_memory + _tail - 1);
    }

    reference operator[](size_type index) const noexcept {
        return *(_start_memory + index);
    }

    const_reference at(size_type index) const {
        if (index >= size()) {
            throw std::out_of_range("Index out of range");
        }
        return _start_memory[_head + index];
    }

    reference at(size_type index) {
        if (index >= size()) {
            throw std::out_of_range("Index out of range");
        }
        return _start_memory[_head + index];
    }

    void clear() noexcept {
        for (size_t i = 0; i < _size; ++i) {
            std::allocator_traits<allocator_type>::destroy(_allocator, _start_memory + i);
        }
        _head = 0;
        _tail = 0;
        _size = 0;
    }

    IteratorExt erase(IteratorExt position) {
        IteratorExt iterator(position);
        if (position == end()) {
            throw std::runtime_error("Cannot erase the end iterator");
        }

        size_type index = *position + _head;

        for (size_type i = index - 1; i < _tail; ++i) {
            std::allocator_traits<allocator_type>::construct(_allocator, _start_memory + i,
                                                             std::move(*(_start_memory + i + 1)));
            std::allocator_traits<allocator_type>::destroy(_allocator, _start_memory + i + 1);
        }

        std::allocator_traits<allocator_type>::destroy(_allocator, _start_memory + _tail);
        _tail = _tail + _count - 1;

        iterator._pointer = ++position._pointer;
        iterator._pointer = _start_memory;
        iterator._size_container = --_size;
        return iterator;
    }

    void assign(const IteratorExt start, const IteratorExt finish) {
        _count = start._size_container;
        clear();
        for (auto it = start; it != finish; ++it) {
            push_back(*it);
        }
        push_back(*finish);
    }

    bool operator==(CircularBufferExt& other) const {
        if(_size != other.size()) {
            return false;
        }
        for(size_type i = 0; i < _size; ++i) {
            if(_start_memory[i] != other[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(CircularBufferExt& other) const {
        if(_size != other.size()) {
            return true;
        }
        for(size_type i = 0; i < _size; ++i) {
            if(_start_memory[i] != other[i]) {
                return true;
            }
        }
        return false;
    }

    void swap(CircularBufferExt& other) {
        if(_size == other.size()) {
            for(size_t i = 0; i < _size; ++i) {
                std::swap(*(_start_memory + i), *(other._start_memory + i));
            }
        } else {
            return;
        }
    }

};