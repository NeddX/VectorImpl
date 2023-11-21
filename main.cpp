#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <iostream>
#include <iterator>
#include <limits>
#include <vector>

using usize   = std::size_t;
using ptrdiff = std::ptrdiff_t;
using intptr  = std::intptr_t;
using uintptr = std::uintptr_t;
using u8      = std::uint8_t;
using i8      = std::int8_t;
using u16     = std::uint16_t;
using i16     = std::int16_t;
using u32     = std::uint32_t;
using i32     = std::int32_t;

template <typename T>
class Vec
{
private:
    T*    m_Buffer   = nullptr;
    usize m_Size     = 0;
    usize m_Capacity = 0;

    // Nested iterator classes (const and non-const) for iteration.
private:
    class ConstIterator;
    class Iterator
    {
        friend class ConstIterator;

        using iterator_category = std::forward_iterator_tag;
        using difference_type   = ptrdiff;
        using value_type        = T;
        using pointer           = value_type*;
        using reference         = value_type&;

    private:
        pointer m_Ptr;

    public:
        Iterator(pointer ptr) noexcept : m_Ptr(ptr) {}

    public:
        constexpr reference operator*() const noexcept { return *m_Ptr; }
        constexpr pointer   operator->() const noexcept { return m_Ptr; };
        constexpr Iterator& operator++() noexcept
        {
            ++m_Ptr;
            return *this;
        }
        constexpr Iterator& operator++(const i32) noexcept
        {
            auto t = *this;
            ++(*this);
            return t;
        }
        constexpr ptrdiff  operator-(const Iterator& other) const noexcept { return m_Ptr - other.m_Ptr; }
        constexpr Iterator operator+(const uintptr disp) const noexcept
        {
            Iterator temp = *this;
            temp.m_Ptr += disp;
            return temp;
        };
        constexpr Iterator operator-(const uintptr disp) const noexcept
        {
            Iterator temp = *this;
            temp.m_Ptr -= disp;
            return temp;
        }

    public:
        friend bool operator==(const Iterator& lhv, const Iterator& rhv) noexcept { return lhv.m_Ptr == rhv.m_Ptr; }
        friend bool operator!=(const Iterator& lhv, const Iterator& rhv) noexcept { return !(lhv == rhv); }
    };
    class ConstIterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = ptrdiff;
        using value_type        = T;
        using pointer           = const value_type*;
        using reference         = const value_type&;

    private:
        pointer m_Ptr;

    public:
        ConstIterator(pointer ptr) noexcept : m_Ptr(ptr) {}
        ConstIterator(Iterator it) noexcept : m_Ptr(it.m_Ptr) {}

    public:
        constexpr reference      operator*() const noexcept { return *m_Ptr; }
        constexpr pointer        operator->() const noexcept { return m_Ptr; };
        constexpr ConstIterator& operator++() noexcept
        {
            ++m_Ptr;
            return *this;
        }
        constexpr ConstIterator& operator++(const i32) noexcept
        {
            auto t = *this;
            ++(*this);
            return t;
        }
        constexpr ptrdiff       operator-(const ConstIterator& other) const noexcept { return m_Ptr - other.m_Ptr; }
        constexpr ConstIterator operator+(const i32 disp) const noexcept
        {
            ConstIterator temp = *this;
            temp.m_Ptr += disp;
            return temp;
        };
        constexpr ConstIterator operator-(const i32 disp) const noexcept
        {
            ConstIterator temp = *this;
            temp.m_Ptr -= disp;
            return temp;
        }

    public:
        friend bool operator==(const ConstIterator& lhv, const ConstIterator& rhv) noexcept
        {
            return lhv.m_Ptr == rhv.m_Ptr;
        }
        friend bool operator!=(const ConstIterator& lhv, const ConstIterator& rhv) noexcept { return !(lhv == rhv); }
    };

public:
    Vec() = default;
    Vec(const usize size) : m_Size(size), m_Capacity(size * 2), m_Buffer(new T[m_Capacity]) {}
    Vec(const std::initializer_list<T> list)
    {
        // Initializer list constructor.
        m_Size     = list.size();
        m_Capacity = m_Size * 2;
        m_Buffer   = new T[m_Capacity];
        if (!m_Buffer)
            throw std::bad_alloc();
        std::copy(list.begin(), list.end(), m_Buffer);
    }
    Vec(const Vec<T>& other)
    {
        // Copy constructor.
        m_Size     = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_Buffer   = new T[m_Capacity];
        if (!m_Buffer)
            throw std::bad_alloc();
        std::memcpy(m_Buffer, other.m_Buffer, m_Size * sizeof(T));
    }
    Vec(Vec<T>&& other)
    {
        // Move constructor.
        m_Size           = other.m_Size;
        m_Capacity       = other.m_Capacity;
        other.m_Size     = 0;
        other.m_Capacity = 0;
        std::swap(m_Buffer, other.m_Buffer);
    }
    ~Vec() { Drop(); }

public:
    constexpr usize Size() const noexcept { return m_Size; }
    constexpr usize Capacity() const noexcept { return m_Capacity; }
    constexpr bool  Empty() const noexcept { return m_Size == 0; }
    constexpr T*    Data() const noexcept { return m_Buffer; }
    constexpr usize MaxSize() const noexcept { return std::numeric_limits<usize>::max() / sizeof(T); }

public:
    constexpr Iterator      begin() noexcept { return Iterator(m_Buffer); }
    constexpr Iterator      end() noexcept { return Iterator(m_Buffer + m_Size); }
    constexpr ConstIterator begin() const noexcept { return ConstIterator(m_Buffer); }
    constexpr ConstIterator end() const noexcept { return ConstIterator(m_Buffer + m_Size); }

private:
    void Realloc(const usize newSize, const bool reserveExtra = true)
    {
        // Reallocates memory to accommodate the new requested size. If there was a previous m_Buffer,
        // it will simply creating a new buffer, copy the previous elements into it and release the previous one,
        // otherwise it will only allocate a new buffer.
        // It also doubles the capacity if reserveExtra is set to true.
        if (newSize == m_Size)
            return;

        const usize prev_size = m_Size;
        m_Size                = newSize;

        if (reserveExtra)
            m_Capacity = newSize * 2;
        else
            m_Capacity = newSize;

        if (prev_size > 0)
        {
            T* temp  = m_Buffer;
            m_Buffer = new T[m_Capacity];
            if (!m_Buffer)
                throw std::bad_alloc();
            if (prev_size < m_Size)
                std::memcpy(m_Buffer, temp, prev_size * sizeof(T));
            else
                std::memcpy(m_Buffer, temp, m_Size * sizeof(T));
            delete[] temp;
        }
        else
        {
            if (m_Buffer)
                delete[] m_Buffer;
            m_Buffer = new T[m_Capacity];
            if (!m_Buffer)
                throw std::bad_alloc();
        }
    }
    inline void Drop() noexcept
    {
        // Disposes our vector.
        delete[] m_Buffer;
        m_Buffer   = nullptr;
        m_Size     = 0;
        m_Capacity = 0;
    }

public:
    void Push(const T& e)
    {
        // Reallocate if there's not enough space, since Realloc() changes m_Size, we have to accommodate for it
        // by doing m_Size - 1 (- 1 because we increase our storage space by 1), otherwise just straight up assign the
        // element.
        if (m_Size >= m_Capacity)
        {
            Realloc(m_Size + 1);
            m_Buffer[m_Size - 1] = e;
        }
        else
            m_Buffer[m_Size++] = e;
    }
    constexpr T Pop()
    {
        // Pop the element and return it efficiently while also performing bounds checks.
        if (m_Size > 0)
            return std::move(m_Buffer[m_Size--]);
        else
            throw std::out_of_range("Tried calling Pop() on an empty vector.");
    }
    constexpr T& Front()
    {
        // Grab a reference to the first element with bounds checking.
        if (m_Size > 0)
            return m_Buffer[0];
        else
            throw std::out_of_range("Tried calling Front() on an empty vector.");
    }
    constexpr T& Back()
    {
        // Same as above except we grab a reference to the last element.
        if (m_Size > 0)
            return m_Buffer[m_Size - 1];
        else
            throw std::out_of_range("Tried calling Back() on an empty vector.");
    }
    constexpr T& At(const usize index)
    {
        // Same as the subscript operator but with bounds checking.
        if (m_Size - 1 >= index)
            return m_Buffer[index];
        else
            throw std::out_of_range("Index out of bounds.");
    }
    void Assign(const usize count, const T& value)
    {
        // First overload of Assign(): Fills the vector with 'count' times 'value'.
        Realloc(count);
        std::fill(begin(), end(), value);
    }
    void Assign(const ConstIterator begin, const ConstIterator end)
    {
        // Second overload of Assign(): Fills the contents of this vector with the range of the other
        // using its iterators.
        Realloc(end - begin);
        std::copy(begin, end, this->begin());
    }
    void Assign(const std::initializer_list<T> list)
    {
        // Third overload of Assign(): Fills the vector with the contents of the supplied initializer list.
        Realloc(list.size());
        std::copy(list.begin(), list.end(), begin());
    }
    constexpr void Swap(Vec<T>& other)
    {
        // Swaps the fields, creating an illusion of a swap.
        std::swap(m_Size, other.m_Size);
        std::swap(m_Capacity, other.m_Capacity);
        std::swap(m_Buffer, other.m_Buffer);
    }
    inline void Resize(const usize newSize) { Realloc(newSize); }
    void        Insert(const ConstIterator pos, const T& value)
    {
        // First overload of Insert(): Inserts 'value' at 'diff', which is the distance between these two iterators (pos
        // - begin). The distance (diff) here is our index at which we will insert 'value' onto. We do this by creating
        // a new buffer that has the size 'm_Size + 1' and copy the contents of the previous buffer while also making
        // space for 'value' and inserting it.
        const auto  diff          = pos - begin();
        const usize prev_capacity = m_Capacity;
        const T*    temp          = m_Buffer;

        ++m_Size;
        m_Capacity = m_Size * 2;
        m_Buffer   = new T[m_Capacity];
        std::copy(temp, temp + diff, m_Buffer);
        m_Buffer[diff] = value;
        std::copy(temp + diff, temp + m_Size - 1, m_Buffer + diff + 1);

        delete[] temp;
    }
    void Insert(const ConstIterator pos, const ConstIterator first, const ConstIterator last)
    {
        // Second overload of Insert(): Merge a different vector's range at 'pos' using its iterators.

        const auto  diff          = pos - begin();
        const usize insert_size   = last - first;
        const usize prev_capacity = m_Capacity;
        const usize prev_size     = m_Size;
        T*          temp          = m_Buffer;

        m_Size += insert_size;
        m_Capacity = m_Size * 2;
        m_Buffer   = new T[m_Capacity];
        std::copy(temp, temp + diff, m_Buffer);
        std::copy(first, last, m_Buffer + diff);
        std::copy(temp + diff, temp + prev_size, m_Buffer + diff + insert_size);

        delete[] temp;
    }
    void Erase(const ConstIterator pos)
    {
        if (!Empty())
        {
            const usize   prev_size = m_Size;
            const ptrdiff index     = pos - begin();

            T* temp = m_Buffer;
            --m_Size;
            m_Capacity = m_Size * 2;
            m_Buffer   = new T[m_Capacity];

            for (usize i = 0, j = 0; i < prev_size; ++i)
                if (i != index)
                    m_Buffer[j++] = temp[i];
            delete[] temp;
        }
        else
            throw std::out_of_range("Tried calling Erase() on an empty vector.");
    }
    constexpr void Reserve(const usize newCapacity)
    {
        if (newCapacity > m_Capacity)
        {
            const usize prev_capacity = m_Capacity;
            m_Capacity                = newCapacity;
            T* temp                   = m_Buffer;
            m_Buffer                  = new T[m_Capacity];
            std::memcpy(m_Buffer, m_Capacity, prev_capacity * sizeof(T));
            delete[] temp;
        }
    }
    void Erase(const ConstIterator first, const ConstIterator last)
    {
        if (!Empty())
        {
            const usize prev_size = m_Size;
            T*          temp      = m_Buffer;
            m_Size -= last - first;
            m_Capacity = m_Size * 2;
            m_Buffer   = new T[m_Capacity];

            std::copy(temp, temp + (first - temp), m_Buffer);
            std::copy(temp + (last - first) + (first - temp), temp + prev_size, m_Buffer + (first - temp));

            delete[] temp;
        }
        else
            throw std::out_of_range("Tried calling Erase() on an empty vector.");
    }
    inline void    ShrinkToFit() { Realloc(m_Size, false); }
    constexpr void Clear() noexcept { m_Size = 0; }

public:
    template <typename... TArgs>
    void Emplace(const Iterator pos, TArgs&&... args)
    {
        const auto  diff          = pos - begin();
        const usize prev_capacity = m_Capacity;
        const T*    temp          = m_Buffer;

        ++m_Size;
        m_Capacity = m_Size * 2;
        m_Buffer   = new T[m_Capacity];

        std::copy(temp, temp + diff, m_Buffer);
        m_Buffer[diff] = T(std::forward<TArgs>(args)...);
        std::copy(temp + diff, temp + m_Size - 1, m_Buffer + diff + 1);

        delete[] temp;
    }
    template <typename... TArgs>
    void EmplaceBack(TArgs&&... args)
    {
        if (m_Size >= m_Capacity)
        {
            Realloc(m_Size + 1);
            m_Buffer[m_Size - 1] = T(std::forward<TArgs>(args)...);
        }
        else
            m_Buffer[m_Size++] = T(std::forward<TArgs>(args)...);
    }

public:
    constexpr T& operator[](const usize index) const noexcept { return m_Buffer[index]; }
    Vec<T>&      operator=(const std::initializer_list<T> list)
    {
        Realloc(list.size());
        std::copy(list.begin(), list.end(), begin());
        return *this;
    }
    Vec<T>& operator=(const Vec<T>& other)
    {
        Realloc(other.m_Size);
        std::memcpy(m_Buffer, other.m_Buffer, m_Size * sizeof(T));
        return *this;
    }
    Vec<T>& operator=(Vec<T>&& other) noexcept
    {
        Drop();
        m_Size           = other.m_Size;
        m_Capacity       = other.m_Capacity;
        other.m_Size     = 0;
        other.m_Capacity = 0;
        std::swap(m_Buffer, other.m_Buffer);
        return *this;
    }
    Vec<T>& operator<<(const Vec<T>& other)
    {
        const usize prev_size = m_Size;
        Realloc(m_Size + other.m_Size);
        std::copy(other.begin(), other.end(), begin() + prev_size);
        return *this;
    }

public:
    friend std::ostream& operator<<(std::ostream& stream, const Vec<T>& other)
    {
        stream << "[ ";
        for (usize i = 0; i < other.m_Size; ++i)
        {
            if (i + 1 != other.m_Size)
                stream << other.m_Buffer[i] << ", ";
            else
                stream << other.m_Buffer[i];
        }
        stream << " ]";
        return stream;
    }
};

int main()
{
    std::srand(std::time(nullptr));

    Vec<int> vec;
    Vec<int> vec2 = { 5, 3, 1 };
    for (usize i = 0; i < 9; ++i)
        vec.Push(std::rand() % 256);

    std::cout << "Initial vec: " << vec << std::endl;
    std::cout << "Initial vec2: " << vec2 << std::endl;

    vec.Swap(vec2);

    std::cout << "\nAfter swap vec: " << vec << std::endl;
    std::cout << "InAfter swap vec2: " << vec2 << std::endl;

    std::cout << "\nvec ptr: " << vec.Data() << std::endl;

    vec.Clear();
    std::cout << "\nAfter vec clear: " << vec << std::endl;

    vec = { 1, 5, 10, 9 };
    std::cout << "\nInit cap: " << vec.Capacity() << std::endl;
    vec.ShrinkToFit();
    std::cout << "After cap: " << vec.Capacity() << std::endl;

    std::cout << "\nBefore list assign: " << vec << std::endl;
    vec.Assign({ 9, 10, 5, 1 });
    std::cout << "After list assign: " << vec << std::endl;

    std::cout << "\nBefore vec assign: " << vec << std::endl;
    vec2 = { 5, 1, 6, 10, 8 };
    std::cout << "vec2: " << vec2 << std::endl;
    vec.Assign(vec2.begin() + 2, vec2.end() - 1);
    std::cout << "After vec assign: " << vec << std::endl;

    vec.Insert(vec.end(), 88);
    std::cout << "\nAfter insert: ";
    std::cout << vec << std::endl;
    vec.Clear();
    vec.ShrinkToFit();
    vec.Assign({ 1, 5, 10, 99, 199 });
    std::cout << "\nAfter total reset: " << vec << std::endl;

    vec.Resize(vec.Size() / 2);
    std::cout << "\nAfter resize: " << vec << std::endl;

    std::cout << "\nvec2: " << vec2 << std::endl;
    vec << vec2;
    std::cout << "After merge: " << vec << std::endl;

    vec.Erase(vec.begin() + 3);
    std::cout << "\nAfter erasing the third element: " << vec << std::endl;
    vec.Erase(vec.begin() + 1, vec.begin() + 4);
    std::cout << "After first elements: " << vec << std::endl;

    decltype(vec[0]) max = vec[0];
    for (const auto& e : vec)
    {
        if (e > max)
            max = e;
    }
    std::cout << "\nMax is: " << max << std::endl;

    std::cout << "\nvec: " << vec << std::endl;
    std::cout << "vec2: " << vec2 << std::endl;
    vec2 = vec;
    vec  = { 1, 1, 1, 1 };
    std::cout << "vec2 after copy assigning to: " << vec2 << std::endl;
    std::cout << "vec: " << vec << std::endl;
    vec = std::move(vec2);
    std::cout << "vec2 after move assign: " << vec2 << std::endl;
    std::cout << "vec: " << vec << std::endl;

    vec2 = { 5, 5, 1, 6, 7 };
    std::cout << "\nvec: " << vec << std::endl;
    std::cout << "vec2: " << vec2 << std::endl;
    vec.Insert(vec.begin() + 1, vec2.begin() + 1, vec2.begin() + 3);
    std::cout << "vec after inserting vec2's range (at 1 range: 2..5): " << vec << std::endl;
    return 0;
}
