#include <bitset>
#include <cmath>
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
using u64     = std::uint64_t;
using i64     = std::int64_t;
using f32     = float;
using f64     = double;

template <typename T>
class Vec
{
private:
    T*    m_Buffer   = nullptr;
    usize m_Size     = 0;
    usize m_Capacity = 0;

    // Nested iterator classes (const and non-const) for iteration.
public:
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
        inline Iterator&    operator++() noexcept
        {
            ++m_Ptr;
            return *this;
        }
        Iterator operator++(const i32) noexcept
        {
            auto t = *this;
            ++(*this);
            return t;
        }
        constexpr ptrdiff operator-(const Iterator& other) const noexcept { return m_Ptr - other.m_Ptr; }
        inline Iterator   operator+(const uintptr disp) const noexcept
        {
            Iterator temp = *this;
            temp.m_Ptr += disp;
            return temp;
        };
        inline Iterator operator-(const uintptr disp) const noexcept
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
        constexpr reference   operator*() const noexcept { return *m_Ptr; }
        constexpr pointer     operator->() const noexcept { return m_Ptr; };
        inline ConstIterator& operator++() noexcept
        {
            ++m_Ptr;
            return *this;
        }
        inline ConstIterator operator++(const i32) noexcept
        {
            auto t = *this;
            ++(*this);
            return t;
        }
        constexpr ptrdiff    operator-(const ConstIterator& other) const noexcept { return m_Ptr - other.m_Ptr; }
        inline ConstIterator operator+(const i32 disp) const noexcept
        {
            ConstIterator temp = *this;
            temp.m_Ptr += disp;
            return temp;
        };
        inline ConstIterator operator-(const i32 disp) const noexcept
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
        if (&other == this)
            return;

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
    inline Iterator      begin() noexcept { return Iterator(m_Buffer); }
    inline Iterator      end() noexcept { return Iterator(m_Buffer + m_Size); }
    inline ConstIterator begin() const noexcept { return ConstIterator(m_Buffer); }
    inline ConstIterator end() const noexcept { return ConstIterator(m_Buffer + m_Size); }

private:
    void Realloc(const usize newSize, const bool reserveExtra = true)
    {
        std::vector<int> a;
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
    inline T Pop()
    {
        std::vector<int> a;
        // Pop the element and return it efficiently while also performing bounds checks.
        if (m_Size > 0)
            return std::move(m_Buffer[m_Size--]);
        else
            throw std::out_of_range("Tried calling Pop() on an empty vector.");
    }
    inline T& Front()
    {
        // Grab a reference to the first element with bounds checking.
        if (m_Size > 0)
            return m_Buffer[0];
        else
            throw std::out_of_range("Tried calling Front() on an empty vector.");
    }
    inline T& Back()
    {
        // Same as above except we grab a reference to the last element.
        if (m_Size > 0)
            return m_Buffer[m_Size - 1];
        else
            throw std::out_of_range("Tried calling Back() on an empty vector.");
    }
    inline T& At(const usize index)
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
    constexpr T& operator[](const usize index) noexcept { return m_Buffer[index]; }
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
        if (&other == this)
            return *this;

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

template <typename T>
concept Integral = std::is_integral_v<T>;

template <>
class Vec<bool>
{
    using BufferType              = u8;
    static constexpr auto BitSize = sizeof(BufferType) * 8;

private:
    BufferType* m_Buffer   = nullptr;
    usize       m_Size     = 0;
    usize       m_Capacity = 0;

public:
    // Proxy class for holding a 'reference' to a bit.
    class BitRef
    {
    private:
        BufferType* m_Ptr   = nullptr;
        usize       m_Index = 0;

    public:
        BitRef(BufferType* ptr, const usize index) : m_Ptr(ptr), m_Index(index) {}

    public:
        constexpr operator bool() const noexcept
        {
            return (m_Ptr[m_Index / BitSize] >> ((BitSize - 1) - m_Index % BitSize) & 1);
        }
        inline BitRef& operator=(const bool value) noexcept
        {
            if (value)
                m_Ptr[m_Index / BitSize] |= 1 << ((BitSize - 1) - m_Index % BitSize);
            else
                m_Ptr[m_Index / BitSize] &= ~(1 << ((BitSize - 1) - m_Index % BitSize));
            return *this;
        }
        inline BitRef& operator=(const BitRef& value) noexcept { return this->operator=(value); }
        constexpr bool operator~() const noexcept
        {
            return ~(((m_Ptr[m_Index / BitSize] >> ((BitSize - 1) - m_Index % BitSize)) & 1));
        }
        inline BitRef& Flip() noexcept { return this->operator=(!*this); }
    };
    class ConstIterator;
    class Iterator
    {
        friend class ConstIterator;

        using iterator_category = std::forward_iterator_tag;
        using difference_type   = ptrdiff;
        using value_type        = bool;
        using pointer           = BufferType*;
        using reference         = BitRef;

    private:
        pointer m_Ptr;
        usize   m_Index;

    public:
        Iterator(pointer ptr, const usize index) noexcept : m_Ptr(ptr), m_Index(index) {}

    public:
        inline reference  operator*() const noexcept { return BitRef(m_Ptr, m_Index); }
        constexpr pointer operator->() const noexcept = delete;
        inline Iterator&  operator++() noexcept
        {
            ++m_Index;
            return *this;
        }
        inline Iterator operator++(const i32) noexcept
        {
            auto t = *this;
            ++(*this);
            return t;
        }
        constexpr ptrdiff operator-(const Iterator& other) const noexcept { return m_Index - other.m_Index; }
        inline Iterator   operator+(const uintptr disp) const noexcept
        {
            Iterator temp = *this;
            temp.m_Index += disp;
            return temp;
        };
        inline Iterator operator-(const uintptr disp) const noexcept
        {
            Iterator temp = *this;
            temp.m_Index -= disp;
            return temp;
        }

    public:
        friend bool operator==(const Iterator& lhv, const Iterator& rhv) noexcept { return lhv.m_Index == rhv.m_Index; }
        friend bool operator!=(const Iterator& lhv, const Iterator& rhv) noexcept { return !(lhv == rhv); }
    };
    class ConstIterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = ptrdiff;
        using value_type        = bool;
        using pointer           = const BufferType*;
        using reference         = const BitRef;

    private:
        pointer m_Ptr;
        usize   m_Index;

    public:
        ConstIterator(pointer ptr, const usize index) noexcept : m_Ptr(ptr), m_Index(index) {}

    public:
        inline reference      operator*() const noexcept { return BitRef((BufferType*)m_Ptr, m_Index); }
        constexpr pointer     operator->() const noexcept = delete;
        inline ConstIterator& operator++() noexcept
        {
            ++m_Index;
            return *this;
        }
        inline ConstIterator operator++(const i32) noexcept
        {
            auto t = *this;
            ++(*this);
            return t;
        }
        constexpr ptrdiff    operator-(const Iterator& other) const noexcept { return m_Index - other.m_Index; }
        inline ConstIterator operator+(const uintptr disp) const noexcept
        {
            auto temp = *this;
            temp.m_Index += disp;
            return temp;
        };
        inline ConstIterator operator-(const uintptr disp) const noexcept
        {
            auto temp = *this;
            temp.m_Index -= disp;
            return temp;
        }

    public:
        friend bool operator==(const ConstIterator& lhv, const ConstIterator& rhv) noexcept
        {
            return lhv.m_Index == rhv.m_Index;
        }
        friend bool operator!=(const ConstIterator& lhv, const ConstIterator& rhv) noexcept { return !(lhv == rhv); }
    };

public:
    Vec() = default;
    Vec(const usize size) noexcept : m_Size(size), m_Capacity((usize)std::ceil((f64)m_Size / (f64)BitSize) * 2)
    {
        m_Buffer = new BufferType[m_Capacity];
    }
    Vec(const std::initializer_list<bool> list)
    {
        m_Size     = list.size();
        m_Capacity = (usize)std::ceil((f64)m_Size / (f64)BitSize) * 2;
        m_Buffer   = new BufferType[m_Capacity]{ 0 };
        if (!m_Buffer)
            throw std::bad_alloc();

        usize i = 0;
        for (const auto& e : list)
        {
            m_Buffer[i / BitSize] |= e << ((BitSize - 1) - i % BitSize);
            ++i;
        }
    }
    Vec(const Vec<bool>& other) noexcept
    {
        if (other.m_Buffer)
        {
            m_Size     = other.m_Size;
            m_Capacity = other.m_Capacity;
            m_Buffer   = new BufferType[m_Capacity];
            std::memcpy(m_Buffer, other.m_Buffer, std::ceil((f64)(m_Size / BitSize)));
        }
    }
    Vec(Vec<bool>&& other) noexcept
    {
        if (&other == this)
            return;

        if (other.m_Buffer && other.m_Size)
        {
            std::swap(m_Size, other.m_Size);
            std::swap(m_Capacity, other.m_Capacity);
            std::swap(m_Buffer, other.m_Buffer);
        }
    }
    ~Vec() { Drop(); }

public:
    constexpr usize       Size() const noexcept { return m_Size; }
    constexpr usize       Capacity() const noexcept { return m_Capacity; }
    constexpr bool        Empty() const noexcept { return m_Size == 0; }
    constexpr BufferType* Data() const noexcept { return m_Buffer; }

public:
    inline Iterator begin() const noexcept { return Iterator(m_Buffer, 0); }
    inline Iterator end() const noexcept { return Iterator(m_Buffer, m_Size); }

private:
    constexpr void BitInsert(const bool e, const usize index) noexcept
    {
        m_Buffer[index / BitSize] |= e << ((BitSize - 1) - index % BitSize);
    }
    constexpr bool BitAt(const usize index) const noexcept
    {
        return (m_Buffer[index / BitSize] >> ((BitSize - 1) - index % BitSize)) & 1;
    }
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
            m_Capacity = std::ceil((f64)(newSize * 2.0 / BitSize));
        else
            m_Capacity = std::ceil(newSize / BitSize);

        if (prev_size > 0)
        {
            BufferType* temp = m_Buffer;
            m_Buffer         = new BufferType[m_Capacity];
            if (!m_Buffer)
                throw std::bad_alloc();
            if (prev_size < m_Size)
                std::memcpy(m_Buffer, temp, prev_size * BitSize);
            else
                std::memcpy(m_Buffer, temp, m_Size * BitSize);
            delete[] temp;
        }
        else
        {
            if (m_Buffer)
                delete[] m_Buffer;
            m_Buffer = new BufferType[m_Capacity]{ 0 };
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
    inline BitRef     operator[](const usize index) noexcept { return BitRef(m_Buffer, index); }
    inline Vec<bool>& operator&=(const Vec<bool>& other) noexcept
    {
        for (usize i = 0; i < m_Size; ++i)
        {
            if (other.m_Size > i)
                m_Buffer[i] &= other.m_Buffer[i];
            else
                m_Buffer[i] &= 0;
        }
        return *this;
    }
    inline Vec<bool> operator&(const Vec<bool>& other) const noexcept
    {
        auto cpy = *this;
        for (usize i = 0; i < m_Size / BitSize; ++i)
        {
            if (other.m_Size / BitSize >= i)
                cpy.m_Buffer[i] &= other.m_Buffer[i];
            else
                cpy.m_Buffer[i] &= 0;
        }
        return cpy;
    }
    inline Vec<bool>& operator|=(const Vec<bool>& other) noexcept
    {
        for (usize i = 0; i < m_Size / BitSize; ++i)
        {
            if (other.m_Size / BitSize >= i)
                m_Buffer[i] |= other.m_Buffer[i];
            else
                m_Buffer[i] |= 0;
        }
        return *this;
    }
    inline Vec<bool> operator|=(const Vec<bool>& other) const noexcept
    {
        auto cpy = *this;
        for (usize i = 0; i < m_Size / BitSize; ++i)
        {
            if (other.m_Size / BitSize >= i)
                cpy.m_Buffer[i] |= other.m_Buffer[i];
            else
                cpy.m_Buffer[i] |= 0;
        }
        return cpy;
    }
    inline Vec<bool>& operator^=(const Vec<bool>& other) noexcept
    {
        for (usize i = 0; i < m_Size / BitSize; ++i)
        {
            if (other.m_Size / BitSize >= i)
                m_Buffer[i] ^= other.m_Buffer[i];
            else
                m_Buffer[i] ^= 0;
        }
        return *this;
    }
    inline Vec<bool> operator^(const Vec<bool>& other) const noexcept
    {
        auto cpy = *this;
        for (usize i = 0; i > m_Size / BitSize; ++i)
        {
            if (other.m_Size / BitSize >= i)
                cpy.m_Buffer[i] ^= other.m_Buffer[i];
            else
                cpy.m_Buffer[i] ^= 0;
        }
        return cpy;
    }
    inline Vec<bool> operator~() const noexcept
    {
        auto copy = *this;
        for (usize i = 0; i < m_Size / BitSize; ++i)
            copy.m_Buffer[i] = ~m_Buffer[i];
        return copy;
    }

public:
    void Push(const bool e)
    {
        // Reallocate if there's not enough space, since Realloc() changes m_Size, we have to accommodate for it
        // by doing m_Size - 1 (- 1 because we increase our storage space by 1), otherwise just straight up assign the
        // element.
        if (m_Size >= m_Capacity)
        {
            Realloc(m_Size + 1);
            BitInsert(e, m_Size - 1);
        }
        else
            BitInsert(e, m_Size++);
    }
    constexpr bool Pop()
    {
        // Pop the element and return it efficiently while also performing bounds checks.
        if (m_Size > 0)
            return std::move(BitAt(m_Size--));
        else
            throw std::out_of_range("Tried calling Pop() on an empty vector.");
    }
    constexpr bool Front()
    {
        // Grab a reference to the first element with bounds checking.
        if (m_Size > 0)
            return BitAt(0);
        else
            throw std::out_of_range("Tried calling Front() on an empty vector.");
    }
    constexpr bool Back()
    {
        // Same as above except we grab a reference to the last element.
        if (m_Size > 0)
            return BitAt(m_Size - 1);
        else
            throw std::out_of_range("Tried calling Back() on an empty vector.");
    }
    constexpr bool At(const usize index)
    {
        // Same as the subscript operator but with bounds checking.
        if (m_Size - 1 >= index)
            return BitAt(index);
        else
            throw std::out_of_range("Index out of bounds.");
    }
    void Assign(const usize count, const bool value)
    {
        // First overload of Assign(): Fills the vector with 'count' times 'value'.
        Realloc(count);
        std::fill(begin(), end(), value);
    }
    void Assign(const Iterator begin, const Iterator end)
    {
        // Second overload of Assign(): Fills the contents of this vector with the range of the other
        // using its iterators.
        Realloc(end - begin);
        std::copy(begin, end, this->begin());
    }
    void Assign(const std::initializer_list<bool> list)
    {
        // Third overload of Assign(): Fills the vector with the contents of the supplied initializer list.
        Realloc(list.size());
        usize i = 0;
        for (const auto& e : list)
        {
            m_Buffer[i / BitSize] |= e << ((BitSize - 1) - i % BitSize);
            ++i;
        }
    }
    constexpr void Swap(Vec<bool>& other)
    {
        // Swaps the fields, creating an illusion of a swap.
        std::swap(m_Size, other.m_Size);
        std::swap(m_Capacity, other.m_Capacity);
        std::swap(m_Buffer, other.m_Buffer);
    }
    inline void Resize(const usize newSize) { Realloc(newSize); }
    void        Insert(const Iterator pos, const bool value)
    {
        // First overload of Insert(): Inserts 'value' at 'diff', which is the distance between these two iterators (pos
        // - begin). The distance (diff) here is our index at which we will insert 'value' onto. We do this by creating
        // a new buffer that has the size 'm_Size + 1' and copy the contents of the previous buffer while also making
        // space for 'value' and inserting it.
        const auto        diff          = pos - begin();
        const usize       prev_capacity = m_Capacity;
        const BufferType* temp          = m_Buffer;

        ++m_Size;
        m_Capacity = (usize)std::ceil((f64)m_Size / (f64)BitSize) * 2;
        m_Buffer   = new BufferType[m_Capacity]{ 0 };

        for (usize i = 0, j = 0; i < m_Size; ++i)
        {
            if (i != diff)
                m_Buffer[i / BitSize] |= ((temp[j / BitSize] >> ((BitSize - 1) - j++ % BitSize)) & 1)
                                         << ((BitSize - 1) - i % BitSize);
            else
                m_Buffer[i / BitSize] |= value << ((BitSize - 1) - i % BitSize);
        }

        delete[] temp;
    }
    void Insert(const Iterator pos, const Iterator first, const Iterator last)
    {
        // Second overload of Insert(): Merge a different vector's range at 'pos' using its iterators.

        const auto  diff          = pos - begin();
        const usize insert_size   = last - first;
        const usize prev_capacity = m_Capacity;
        const usize prev_size     = m_Size;
        BufferType* temp          = m_Buffer;

        m_Size += insert_size;
        m_Capacity = (usize)std::ceil((f64)m_Size / (f64)BitSize) * 2;
        m_Buffer   = new BufferType[m_Capacity]{ 0 };

        for (usize i = 0, j = 0; i < m_Size; ++i)
        {
            if (i != diff)
                m_Buffer[i / BitSize] |= ((temp[j / BitSize] >> ((BitSize - 1) - j++ % BitSize)) & 1)
                                         << ((BitSize - 1) - i % BitSize);
            else
            {
                for (auto it = first; it != last; ++it)
                    m_Buffer[i / BitSize] |= *it << ((BitSize - 1) - i++ % BitSize);
            }
        }

        delete[] temp;
    }
    void Erase(const Iterator pos)
    {
        if (!Empty())
        {
            const usize   prev_size = m_Size;
            const ptrdiff index     = pos - begin();

            BufferType* temp = m_Buffer;
            --m_Size;
            m_Capacity = (usize)std::ceil((f64)m_Size / (f64)BitSize) * 2;
            m_Buffer   = new BufferType[m_Capacity]{ 0 };

            for (usize i = 0, j = 0; i < prev_size; ++i)
                if (i != index)
                    m_Buffer[j++ / BitSize] |= ((temp[i / BitSize] >> ((BitSize - 1) - i % BitSize)) & 1)
                                               << ((BitSize - 1) - i % BitSize);
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
            BufferType* temp          = m_Buffer;
            m_Buffer                  = new BufferType[m_Capacity];
            std::memcpy(m_Buffer, temp, prev_capacity * BitSize);
            delete[] temp;
        }
    }
    inline std::string ToString() const noexcept
    {
        std::string str;
        str.resize(m_Size);
        for (usize i = 0; i < m_Size; ++i)
            str[i] = 48 + BitAt(i);
        return str;
    }
    inline void Flip() noexcept
    {
        for (auto b : *this)
            b.Flip();
    }
    inline bool Any() const noexcept
    {
        for (const auto b : *this)
            if (b)
                return true;
        return false;
    }
    inline usize Count() const noexcept
    {
        usize count = 0;
        for (const auto b : *this)
            count = (b) ? ++count : count;
        return count;
    }
    void Erase(const Iterator first, const Iterator last)
    {
        if (!Empty())
        {
            const usize prev_size = m_Size;
            BufferType* temp      = m_Buffer;
            m_Size -= last - first;
            m_Capacity = (usize)std::ceil((f64)m_Size / (f64)BitSize) * 2;
            m_Buffer   = new BufferType[m_Capacity]{ 0 };

            /*
            std::copy(temp, temp + (first - temp), m_Buffer);
            std::copy(temp + (last - first) + (first - temp), temp + prev_size, m_Buffer + (first - temp));
            */

            delete[] temp;
        }
        else
            throw std::out_of_range("Tried calling Erase() on an empty vector.");
    }
    constexpr void Clear() noexcept { m_Size = 0; }
    constexpr void Reset() noexcept { std::memset(m_Buffer, 0, m_Capacity); }

public:
    friend std::ostream& operator<<(std::ostream& stream, const Vec<bool>& other) noexcept
    {
        stream << "[ ";
        const auto size = other.Size();
        for (usize i = 0; i < size; ++i)
        {
            stream << ((other.m_Buffer[i / BitSize] >> ((BitSize - 1) - i % BitSize)) & 1);
            if (i + 1 != size)
                stream << ", ";
        }
        stream << " ]";
        return stream;
    }
};

void TestVec()
{
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
}

void TestBitset()
{
    std::bitset<10> n;
    Vec<bool>       vec  = { 1, 1, 1, 0, 0, 1, 1, 0, 0 };
    Vec<bool>       vec1 = { 1 };
    std::cout << (vec & vec1) << std::endl;
    std::cout << "Count: " << vec.Count() << std::endl;
}

int main()
{
    std::srand(std::time(nullptr));

    // TestVec();
    TestBitset();
    return 0;
}
