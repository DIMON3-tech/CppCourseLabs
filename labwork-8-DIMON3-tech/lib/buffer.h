#include <algorithm>
#include <iterator>
#include <initializer_list>
#include <memory>
#include <stdexcept>

template<typename T, class Allocator = std::allocator<T>>
class CircularBuffer {
private:
    Allocator _allocator;
    T* _start_memory;
    size_t _head;
    size_t _tail;
    size_t _size;
    size_t _count;
public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = size_t;
    using reference = value_type&;
    using pointer = value_type*;
    using const_reference = const value_type&;
    using const_pointer = const value_type*;
    using traits = std::allocator_traits<allocator_type>;


    CircularBuffer() : CircularBuffer(0) {}

    CircularBuffer(size_t capacity) {
        _start_memory = std::allocator_traits<allocator_type>::allocate(_allocator, capacity);
        for (size_type i = 0; i < capacity; ++i) {
            traits::construct(_allocator, &_start_memory[i]);
        }
        _head = 0;
        _tail = 0;
        _size = 0;
        _count = capacity;

    }

    CircularBuffer(CircularBuffer&& other) noexcept {
        _start_memory = traits::allocate(_allocator, other._count);
        for (size_type i = 0; i < other._size; ++i) {
            _start_memory + i = other._start_memory;
        }
        _allocator = other._allocator;
        _size = other._size;
        _head = other._head;
        _tail = other._tail;
        _count = other._count;

        other._size = 0;
        other.count = 0;
        other._head = 0;
        other._end = 0;
    }

    CircularBuffer(CircularBuffer& other) {
        _start_memory = traits::allocate(_allocator, other._count);
        for (size_type i = 0; i < other._size; ++i) {
            _start_memory + i = other._start_memory;
        }
        _allocator = other._allocator;
        _size = other._size;
        _head = other._head;
        _tail = other._tail;
        _count = other._count;

        other._size = 0;
        other.count = 0;
        other._head = 0;
        other._end = 0;
    }

    CircularBuffer(std::initializer_list<T> init_list) : CircularBuffer(init_list.size()) {
        for (const auto& element: init_list) {
            push_back(element);
        }
    }

    // random access iterator
    class Iterator {
        friend class CircularBuffer;

    private:
        T* _pointer;
        size_t _index; // for indexing in buffer (when overflow too)
        size_t _size_container;
    public:
        Iterator() = default;

        explicit Iterator(T* ptr, size_t index, size_t size_container) {
            _pointer = ptr;
            _index = index;
            _size_container = size_container;
        }

        Iterator(const Iterator& other) {
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

        Iterator& operator++() noexcept {
            if (_index + 1 == _size_container) {
                _index = 0;
            } else {
                ++_index;
            }
            return *this;
        }

        Iterator operator++(int) noexcept {
            Iterator iterator(*this);
            if (_index + 1 == _size_container) {
                _index = 0;
            } else {
                ++_index;
            }
            return iterator;
        }

        Iterator& operator--() noexcept {
            if (_index - 1 < 0) {
                _index = _size_container - 1;
            } else {
                --_index;
            }
            return *this;
        }

        Iterator operator--(int) noexcept {
            Iterator iterator(*this);
            --_index;
            if (_index < 0) {
                _index = _size_container - 1;
            }
            return iterator;
        }

        Iterator operator+=(size_type number) noexcept {
            _index += number;
            if (_index >= _size_container) {
                _index -= _size_container;
            }
            return *(this);
        }

        Iterator operator-=(size_type number) noexcept {
            if (number > _index) {
                number -= _index;
                _index = _size_container - number;
            } else {
                _index -= number;
            }
            return *(this);
        }

        Iterator operator+(size_type number) noexcept {
            Iterator iterator(*this);
            iterator._index += number;
            if (iterator._index >= _size_container) {
                iterator._index -= _size_container;
            }
            return iterator;
        }

        Iterator operator+=(const Iterator& last) noexcept {
            size_type temp = _index + last._index;
            if (temp >= _size_container) {
                _index = _size_container - temp - 1;
            } else {
                _index += last._index;
            }
            return (*this);
        }

        Iterator operator+(const Iterator& last) noexcept {
            Iterator iterator(*this);
            size_type temp = _index + last._index;
            if (temp > _size_container) {
                iterator._index = _size_container - temp - 1;
            } else {
                iterator._index = _index + last._index;
            }
            return iterator;
        }

        Iterator operator-(const Iterator& last) noexcept {
            size_type temp = last._index;
            if (temp > _index) {
                temp -= _index;
                _index = _size_container - temp;
            } else {
                _index -= last._index;
            }
            return (*this);
        }

        Iterator operator-(size_type number) noexcept {
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

        bool operator==(const Iterator& other) const noexcept {
            return (_index == other._index and _pointer != other._pointer);
        }

        bool operator!=(const Iterator& other) const noexcept {
            return (_index != other._index);
        }

        bool operator>(const Iterator& other) const noexcept {
            return (_index > other._index);
        }

        bool operator<(const Iterator& other) const noexcept {
            return (_index < other._index);
        }

        bool operator>=(const Iterator& other) const noexcept {
            return (_index >= other._index);
        }

        bool operator<=(const Iterator& other) const noexcept {
            return (_index <= other._index);
        }
    };

    // implementing const random access iterator
    class ConstIterator {
    private:
        const T* _pointer;
        size_t _index; // for indexing in buffer (when overflow too)
        size_t _size_container;
    public:
        explicit ConstIterator(T* ptr, size_t index, size_t size_container, T* memory) {
            _pointer = ptr;
            _index = index;
            _size_container = size_container;
        }

        explicit ConstIterator(const ConstIterator& other) {
            _pointer = other._pointer;
            _index = other._index;
            _size_container = other._size_container;
        }

        ConstIterator() = default;

        virtual const_reference operator*() const noexcept {
            return *(_pointer + _index);
        }

        const_pointer operator->() const noexcept {
            return _pointer + _index;
        }

        ConstIterator& operator++() noexcept {
            if (_index + 1 == _size_container) {
                _index = 0;
            } else {
                ++_index;
            }
            return *this;
        }

        ConstIterator operator++(int) noexcept {
            Iterator iterator(*this);
            if (_index + 1 == _size_container) {
                _index = 0;
            } else {
                ++_index;
            }
            return iterator;
        }

        ConstIterator& operator--() noexcept {
            if (_index - 1 < 0) {
                _index = _size_container - 1;
            } else {
                --_index;
            }
            return *this;
        }

        ConstIterator operator--(int) noexcept {
            Iterator iterator(*this);
            --_index;
            if (_index < 0) {
                _index = _size_container - 1;
            }
            return iterator;
        }

        virtual ConstIterator operator+=(size_t number) noexcept {
            _index += number;
            if (_index >= _size_container) {
                _index -= _size_container;
            }
            return *(this);
        }

        virtual ConstIterator operator-=(size_t number) noexcept {
            if (number > _index) {
                number -= _index;
                _index = _size_container - number;
            } else {
                _index -= number;
            }
            return *(this);
        }

        constexpr ConstIterator operator+(size_t number) noexcept {
            ConstIterator const_iterator(*this);
            const_iterator._index += number;
            if (const_iterator._index >= _size_container) {
                const_iterator._index -= _size_container;
            }
            return const_iterator;
        }

        constexpr ConstIterator operator+=(const ConstIterator& last) noexcept {
            size_type temp = _index + last._index;
            if (temp >= _size_container) {
                _index = _size_container - temp - 1;
            } else {
                _index += last._index;
            }
            return (*this);
        }

        ConstIterator operator+(const ConstIterator& last) noexcept {
            ConstIterator const_iterator(*this);
            size_type temp = _index + last._index;
            if (temp > _size_container) {
                const_iterator._index = _size_container - temp - 1;
            } else {
                const_iterator._index = _index + last._index;
            }
            return const_iterator;
        }

        ConstIterator operator-(const ConstIterator& last) noexcept {
            size_type temp = last._index;
            if (temp > _index) {
                temp -= _index;
                _index = _size_container - temp;
            } else {
                _index -= last._index;
            }
            return (*this);
        }

        ConstIterator operator-(size_t number) noexcept {
            if (number > _index) {
                number -= _index;
                _index = _size_container - number;
            } else {
                _index -= number;
            }
            return *(this);
        }

        reference operator[](size_t index) const noexcept {
            return *(_pointer + index);
        }

        bool operator==(const ConstIterator& other) const noexcept {
            return (_index == other._index and _pointer == other._pointer);
        }

        virtual bool operator!=(const ConstIterator& other) const noexcept {
            return (_index != other._index);
        }

        virtual bool operator>(const ConstIterator& other) const noexcept {
            return (_index > other._index);
        }

        virtual bool operator<(const ConstIterator& other) const noexcept {
            return (_index < other._index);
        }

        virtual bool operator>=(const ConstIterator& other) const noexcept {
            return (_index >= other._index);
        }

        virtual bool operator<=(const ConstIterator& other) const noexcept {
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

    Iterator begin() noexcept {
        return Iterator(_start_memory, _head, _count);
    }

    ConstIterator begin() const noexcept {
        return ConstIterator(_start_memory, _head, _count);
    }

    Iterator end() noexcept {
        return Iterator(_start_memory, _tail - 1, _count);
    }

    ConstIterator end() const noexcept {
        return ConstIterator(_start_memory, _tail - 1, _count);
    }

    ConstIterator cbegin() const noexcept {
        return ConstIterator(_start_memory, _tail - 1, _count);
    }

    ConstIterator cend() const noexcept {
        return ConstIterator(_start_memory, _tail - 1, _count);
    }

    template<typename Arg>
    void push_back(Arg&& arg) {
        if (_tail >= _count) {
            _start_memory[0] = arg;
            _head = (_head + 1);
            _tail = _head;
        } else {
            _start_memory[_tail] = arg;
            _tail = _tail + 1;
            ++_size;
        }

    }

    template<typename Arg>
    Iterator emplace(const Iterator& position, Arg&& arg) {
        size_t index_to_insert = position._index;
        if (_size == _count) { // when buffer is already full
            T temp = _start_memory[_tail - 1];
            for (size_t i = _count - 1; i > index_to_insert; --i) {
                _start_memory[i] = _start_memory[i - 1];
            }
            _start_memory[_head] = temp;
            if (index_to_insert == 0) {
                --_head;
            }
            _start_memory[index_to_insert] = std::forward<Arg>(arg);
            return Iterator(_start_memory + index_to_insert, index_to_insert, _size);
        } else {
            for (size_t i = _count - 1; i > index_to_insert; --i) {
                _start_memory[i] = _start_memory[i - 1];
            }
            _start_memory[index_to_insert] = std::forward<Arg>(arg);
            ++_size;
            return Iterator(_start_memory, index_to_insert, _size);
        }
    }

    template<typename Arg>
    Iterator insert(Iterator position, int counter, Arg&& arg) noexcept {
        for (int i = 0; i < counter; ++i) {
            emplace(position, arg);
        }
        position._size_container = _size;
        return position;
    }

    void assign(const Iterator start, const Iterator finish) {
        _count = start._size_container;
        clear();
        for (auto it = start; it != finish; ++it) {
            push_back(*it);
        }
        push_back(*finish);
    }

    reference front() const noexcept {
        return *(_start_memory + _head);
    }

    reference back() const noexcept {
        return *(_start_memory + _tail - 1);
    }

    reference operator[](size_t index) const noexcept {
        return *(_start_memory + index);
    }

    reference at(size_t index) const {
        if (index >= size()) {
            throw std::out_of_range("Index out of range");
        }
        return _start_memory[_head + index];
    }

    reference at(size_t index) {
        if (index >= size()) {
            throw std::out_of_range("Index out of range");
        }
        return _start_memory[_head + index];
    }

    void clear() {
        for (size_t i = 0; i < _size; ++i) {
            std::allocator_traits<allocator_type>::destroy(_allocator, _start_memory + i);
        }
        _head = 0;
        _tail = 0;
        _size = 0;
    }

    Iterator erase(Iterator position) {
        if (position == end()) {
            throw std::runtime_error("Cannot erase the end iterator");
        }

        size_t index = *position;

        for (size_t i = index - 1; i < _tail; ++i) {
            std::allocator_traits<allocator_type>::construct(_allocator, _start_memory + i,
                                                             std::move(*(_start_memory + i + 1)));
            std::allocator_traits<allocator_type>::destroy(_allocator, _start_memory + i + 1);
        }

        std::allocator_traits<allocator_type>::destroy(_allocator, _start_memory + _tail);
        ++position._index;

        _tail = _count - 1;
        --_size;
        return position;
    }

    int accumulate(Iterator begin, Iterator end, int sum) const {
        if constexpr (std::is_integral<T>()) {
            if (begin > end) {
                throw std::out_of_range("First Iterator > than second Iterator!!!");
            }
            sum = _start_memory[begin._index];
            for (size_t i = begin._index + 1; i <= end._index; ++i) {
                sum += _start_memory[i];
            }
            return sum;
        }
    }

    void swap(CircularBuffer& other) {
        if(_size == other.size()) {
            for(size_t i = 0; i < _size; ++i) {
                std::swap(*(_start_memory + i), *(other._start_memory + i));
            }
        } else {
            return;
        }
    }

    bool operator==(CircularBuffer& other) const {
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

    bool operator!=(CircularBuffer& other) const {
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


};  // end of class CircularBuffer
