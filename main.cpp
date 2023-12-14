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
using f128    = long double;

template <typename T>
class Vec
{
private:
    T*    m_Buffer   = nullptr;
    usize m_Size     = 0;
    usize m_Capacity = 0;

public:
    class ConstIterator;
    class Iterator
    {
        friend class ConstIterator;

        using iterator_category = std::input_iterator_tag;
        using difference_type   = ptrdiff;
        using value_type        = T;
        using pointer           = value_type*;
        using reference         = value_type&;

    private:
        pointer m_Ptr;

    public:
        explicit Iterator(pointer ptr) noexcept : m_Ptr(ptr) {}

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
        using iterator_category = std::input_iterator_tag;
        using difference_type   = ptrdiff;
        using value_type        = T;
        using pointer           = const value_type*;
        using reference         = const value_type&;

    private:
        pointer m_Ptr;

    public:
        explicit ConstIterator(pointer ptr) noexcept : m_Ptr(ptr) {}
        explicit ConstIterator(Iterator it) noexcept : m_Ptr(it.m_Ptr) {}

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
    class ForwardIterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = ptrdiff;
        using value_type        = T;
        using pointer           = value_type*;
        using reference         = value_type&;

    private:
        pointer m_Ptr = nullptr;

    public:
        ForwardIterator() = default;
        ForwardIterator(pointer ptr) noexcept : m_Ptr(ptr) {}
        ForwardIterator(Iterator it) noexcept : m_Ptr(it.m_Ptr) {}

    public:
        constexpr reference   operator*() noexcept { return *m_Ptr; }
        constexpr pointer     operator->() noexcept { return m_Ptr; };
        inline ForwardIterator& operator++() noexcept
        {
            ++m_Ptr;
            return *this;
        }
        inline ForwardIterator& operator++(const i32) noexcept
        {
            auto t = *this;
            ++(*this);
            return t;
        }
        constexpr ptrdiff    operator-(const ForwardIterator& other) noexcept { return m_Ptr - other.m_Ptr; }
        inline ForwardIterator operator+(const i32 disp) noexcept
        {
            auto temp = *this;
            temp.m_Ptr += disp;
            return temp;
        };
        inline ForwardIterator operator-(const i32 disp) noexcept
        {
            auto temp = *this;
            temp.m_Ptr -= disp;
            return temp;
        }

    public:
        friend bool operator==(const ForwardIterator& lhv, const ForwardIterator& rhv) noexcept
        {
            return lhv.m_Ptr == rhv.m_Ptr;
        }
        friend bool operator!=(const ForwardIterator& lhv, const ForwardIterator& rhv) noexcept { return !(lhv == rhv); }
    };
    class RandomAccessIterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = ptrdiff;
        using value_type        = T;
        using pointer           = value_type*;
        using reference         = value_type&;

    private:
        pointer m_Ptr = nullptr;

    public:
        ForwardIterator() = default;
        ForwardIterator(pointer ptr) noexcept : m_Ptr(ptr) {}
        ForwardIterator(Iterator it) noexcept : m_Ptr(it.m_Ptr) {}

    public:
        constexpr reference   operator*() noexcept { return *m_Ptr; }
        constexpr pointer     operator->() noexcept { return m_Ptr; };
        inline ForwardIterator& operator++() noexcept
        {
            ++m_Ptr;
            return *this;
        }
        inline ForwardIterator& operator++(const i32) noexcept
        {
            auto t = *this;
            ++(*this);
            return t;
        }
        constexpr ptrdiff    operator-(const ForwardIterator& other) noexcept { return m_Ptr - other.m_Ptr; }
        inline ForwardIterator operator+(const i32 disp) noexcept
        {
            auto temp = *this;
            temp.m_Ptr += disp;
            return temp;
        };
        inline ForwardIterator operator-(const i32 disp) noexcept
        {
            auto temp = *this;
            temp.m_Ptr -= disp;
            return temp;
        }

    public:
        friend bool operator==(const ForwardIterator& lhv, const ForwardIterator& rhv) noexcept
        {
            return lhv.m_Ptr == rhv.m_Ptr;
        }
        friend bool operator!=(const ForwardIterator& lhv, const ForwardIterator& rhv) noexcept { return !(lhv == rhv); }
    };

public:
    Vec() = default;
    Vec(const usize size) : m_Size(size), m_Capacity(size * 2), m_Buffer(new T[m_Capacity]) {}
    Vec(const std::initializer_list<T> list)
    {
        m_Size     = list.size();
        m_Capacity = m_Size * 2;
        m_Buffer   = new T[m_Capacity];
        if (!m_Buffer)
            throw std::bad_alloc();
        std::copy(list.begin(), list.end(), m_Buffer);
    }
    Vec(const Vec<T>& other)
    {
        if (&other == this)
            return;

        m_Size     = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_Buffer   = new T[m_Capacity];
        if (!m_Buffer)
            throw std::bad_alloc();
        std::memcpy(m_Buffer, other.m_Buffer, m_Size * sizeof(T));
    }
    Vec(Vec<T>&& other)
    {
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
        delete[] m_Buffer;
        m_Buffer   = nullptr;
        m_Size     = 0;
        m_Capacity = 0;
    }

public:
    void Push(const T& e)
    {
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
        if (m_Size > 0)
            return std::move(m_Buffer[m_Size--]);
        else
            throw std::out_of_range("Tried calling Pop() on an empty vector.");
    }
    inline T& Front()
    {
        if (m_Size > 0)
            return m_Buffer[0];
        else
            throw std::out_of_range("Tried calling Front() on an empty vector.");
    }
    inline const T& Front() const
    {
        if (m_Size > 0)
            return m_Buffer[0];
        else
            throw std::out_of_range("Tried calling Front() on an empty vector.");
    }
    inline T& Back()
    {
        if (m_Size > 0)
            return m_Buffer[m_Size - 1];
        else
            throw std::out_of_range("Tried calling Back() on an empty vector.");
    }
    inline const T& Back() const
    {
        if (m_Size > 0)
            return m_Buffer[m_Size - 1];
        else
            throw std::out_of_range("Tried calling Back() on an empty vector.");
    }
    inline T& At(const usize index)
    {
        if (m_Size - 1 >= index)
            return m_Buffer[index];
        else
            throw std::out_of_range("Index out of bounds.");
    }
    inline const T& At(const usize index) const
    {
        if (m_Size - 1 >= index)
            return m_Buffer[index];
        else
            throw std::out_of_range("Index out of bounds.");
    }
    void Assign(const usize count, const T& value)
    {
        Realloc(count);
        std::fill(begin(), end(), value);
    }
    void Assign(const ConstIterator begin, const ConstIterator end)
    {
        Realloc(end - begin);
        std::copy(begin, end, this->begin());
    }
    void Assign(const std::initializer_list<T> list)
    {
        Realloc(list.size());
        std::copy(list.begin(), list.end(), begin());
    }
    constexpr void Swap(Vec<T>& other)
    {
        std::swap(m_Size, other.m_Size);
        std::swap(m_Capacity, other.m_Capacity);
        std::swap(m_Buffer, other.m_Buffer);
    }
    inline void Resize(const usize newSize) { Realloc(newSize); }
    void        Insert(const ConstIterator pos, const T& value)
    {
        const auto diff = pos - begin();
        const T*   temp = m_Buffer;

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
        const auto  diff        = pos - begin();
        const usize insert_size = last - first;
        const usize prev_size   = m_Size;
        T*          temp        = m_Buffer;

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
        const auto diff = pos - begin();
        const T*   temp = m_Buffer;

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
    constexpr T&       operator[](const usize index) noexcept { return m_Buffer[index]; }
    constexpr const T& operator[](const usize index) const noexcept { return m_Buffer[index]; }
    inline Vec<T>&     operator=(const std::initializer_list<T> list)
    {
        Realloc(list.size());
        std::copy(list.begin(), list.end(), begin());
        return *this;
    }
    inline Vec<T>& operator=(const Vec<T>& other)
    {
        if (&other == this)
            return *this;

        Realloc(other.m_Size);
        std::memcpy(m_Buffer, other.m_Buffer, m_Size * sizeof(T));
        return *this;
    }
    inline Vec<T>& operator=(Vec<T>&& other) noexcept
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
    inline Vec<T>& operator<<(const Vec<T>& other)
    {
        if (&other == this)
            return *this;

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
    // Based on std::bitset

    using BufferType              = u8;
    static constexpr auto BitSize = sizeof(BufferType) * 8;

private:
    BufferType* m_Buffer   = nullptr;
    usize       m_Size     = 0;
    usize       m_Capacity = 0;

public:
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
        inline BitRef& operator=(const BitRef& value) noexcept { return this->operator=(value.operator bool()); }
        constexpr bool operator~() const noexcept
        {
            return ~(((m_Ptr[m_Index / BitSize] >> ((BitSize - 1) - m_Index % BitSize)) & 1));
        }
        constexpr bool operator&(const bool value) const noexcept
        {
            return (bool)((u8)value & (u8)this->operator bool());
        }
        inline BitRef& operator&=(const bool value) noexcept
        {
            this->operator=(this->operator&(value));
            return *this;
        }
        constexpr bool operator|(const bool value) const noexcept
        {
            return (bool)((u8)this->operator bool() | (u8)value);
        }
        inline BitRef& operator|=(const bool value) noexcept
        {
            this->operator=(this->operator|(value));
            return *this;
        }
        constexpr bool operator^(const bool value) const noexcept
        {
            return (bool)((u8)this->operator bool() ^ (u8)value);
        }
        inline BitRef& operator^=(const bool value) noexcept
        {
            this->operator=(this->operator^(value));
            return *this;
        }
        inline BitRef& Flip() noexcept { return this->operator=(!*this); }

    public:
        friend std::ostream& operator<<(std::ostream& stream, const BitRef& ref)
        {
            stream << ref.operator bool();
            return stream;
        }
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
    Vec(const usize size) noexcept : m_Size(size), m_Capacity(std::ceil((f128)m_Size / (f32)BitSize) * 2)
    {
        m_Buffer = new BufferType[m_Capacity];
    }
    Vec(const std::initializer_list<bool> list)
    {
        m_Size     = list.size();
        m_Capacity = std::ceil((f128)m_Size / (f32)BitSize) * 2;
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
        if (&other == this)
            return;

        if (other.m_Buffer)
        {
            m_Size     = other.m_Size;
            m_Capacity = other.m_Capacity;
            m_Buffer   = new BufferType[m_Capacity]{ 0 };
            std::memcpy(m_Buffer, other.m_Buffer, std::ceil((f128)m_Size / (f32)BitSize));
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
        if (newSize == m_Size)
            return;

        const usize prev_size     = m_Size;
        const usize prev_capacity = m_Capacity;
        m_Size                    = newSize;

        if (reserveExtra)
            m_Capacity = std::ceil((f128)newSize * 2.0 / (f32)BitSize);
        else
            m_Capacity = std::ceil((f128)newSize / (f32)BitSize);

        if (prev_size > 0)
        {
            BufferType* temp = m_Buffer;
            m_Buffer         = new BufferType[m_Capacity];
            if (!m_Buffer)
                throw std::bad_alloc();
            if (prev_size < m_Size)
                std::memcpy(m_Buffer, temp, prev_capacity * sizeof(BufferType));
            else
                std::memcpy(m_Buffer, temp, m_Capacity * sizeof(BufferType));
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
        delete[] m_Buffer;
        m_Buffer   = nullptr;
        m_Size     = 0;
        m_Capacity = 0;
    }

public:
    inline BitRef       operator[](const usize index) noexcept { return BitRef(m_Buffer, index); }
    inline const BitRef operator[](const usize index) const noexcept { return BitRef(m_Buffer, index); }
    inline Vec<bool>&   operator=(const Vec<bool>& other) noexcept
    {
        if (&other == this)
            return *this;

        if (m_Buffer)
            Drop();

        m_Size     = other.m_Size;
        m_Capacity = other.m_Capacity;
        m_Buffer   = new BufferType[m_Capacity]{ 0 };
        if (other.m_Buffer)
            std::memcpy(m_Buffer, other.m_Buffer, std::ceil((f128)m_Size / (f32)BitSize));

        return *this;
    }
    inline Vec<bool>& operator=(Vec<bool>&& other) noexcept
    {
        if (&other == this)
            return *this;

        if (m_Buffer)
            Drop();

        std::swap(m_Size, other.m_Size);
        std::swap(m_Capacity, other.m_Capacity);
        std::swap(m_Buffer, other.m_Buffer);
        return *this;
    }
    inline Vec<bool>& operator&=(const Vec<bool>& other) noexcept
    {
        for (usize i = 0; i < m_Size / BitSize; ++i)
        {
            if (other.m_Size / BitSize >= i)
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
    inline Vec<bool> operator|(const Vec<bool>& other) const noexcept
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
    inline Vec<bool>& operator<<=(const usize pos) noexcept
    {
        BufferType t1{}, t2{};
        for (usize i = m_Size / BitSize; i >= 0 && i != (usize)-1; --i)
        {
            if (i == m_Size / BitSize)
            {
                t1 = m_Buffer[i];
                t1 >>= (sizeof(BufferType) * 8) - pos;
                m_Buffer[i] <<= pos;
            }
            else
            {
                t2 = m_Buffer[i];
                t2 >>= (sizeof(BufferType) * 8) - pos;
                m_Buffer[i] = (m_Buffer[i] << pos) | t1;
                t1          = t2;
            }
        }
        return *this;
    }
    inline Vec<bool>& operator>>=(const usize pos) noexcept
    {
        BufferType t1{}, t2{};
        for (usize i = 0; i < m_Size / BitSize; ++i)
        {
            if (i == 0)
            {
                t1 = m_Buffer[i];
                t1 <<= (sizeof(BufferType) * 8) - pos;
                m_Buffer[i] >>= pos;
            }
            else
            {
                t2 = m_Buffer[i];
                t2 <<= (sizeof(BufferType) * 8) - pos;
                m_Buffer[i] = (m_Buffer[i] >> pos) | t1;
                t1          = t2;
            }
        }
        return *this;
    }
    inline Vec<bool> operator>>=(const usize pos) const noexcept
    {
        auto       cpy = *this;
        BufferType t1{}, t2{};
        for (usize i = 0; i < m_Size / BitSize; ++i)
        {
            if (i == 0)
            {
                t1 = cpy.m_Buffer[i];
                t1 <<= (sizeof(BufferType) * 8) - pos;
                cpy.m_Buffer[i] >>= pos;
            }
            else
            {
                t2 = cpy.m_Buffer[i];
                t2 <<= (sizeof(BufferType) * 8) - pos;
                cpy.m_Buffer[i] = (cpy.m_Buffer[i] >> pos) | t1;
                t1              = t2;
            }
        }
        return cpy;
    }
    inline Vec<bool> operator<<(const usize pos) const noexcept
    {
        auto       cpy = *this;
        BufferType t1{}, t2{};
        for (usize i = cpy.m_Size / BitSize; i >= 0 && i != (usize)-1; --i)
        {
            if (i == cpy.m_Size / BitSize)
            {
                t1 = cpy.m_Buffer[i];
                t1 >>= (sizeof(BufferType) * 8) - pos;
                cpy.m_Buffer[i] <<= pos;
            }
            else
            {
                t2 = cpy.m_Buffer[i];
                t2 >>= (sizeof(BufferType) * 8) - pos;
                cpy.m_Buffer[i] = (cpy.m_Buffer[i] << pos) | t1;
                t1              = t2;
            }
        }
        return cpy;
    }
    inline Vec<bool>& operator<<(const Vec<bool>& other) noexcept
    {
        if (&other == this)
            return *this;

        if (other.m_Size > 0)
        {
            const usize prev_size = m_Size;
            Realloc(other.m_Size + m_Size);
            for (usize i = prev_size; i < m_Size; ++i)
                this->operator[](i) = other[i - prev_size];
        }
        return *this;
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
        if (m_Size > 0)
            return BitAt(m_Size--);
        else
            throw std::out_of_range("Tried calling Pop() on an empty vector.");
    }
    inline BitRef Front()
    {
        if (m_Size > 0)
            return this->operator[](0);
        else
            throw std::out_of_range("Tried calling Front() on an empty vector.");
    }
    inline const BitRef Front() const
    {
        if (m_Size > 0)
            return this->operator[](0);
        else
            throw std::out_of_range("Tried calling Front() on an empty vector.");
    }
    inline BitRef Back()
    {
        if (m_Size > 0)
            return this->operator[](m_Size - 1);
        else
            throw std::out_of_range("Tried calling Back() on an empty vector.");
    }
    inline const BitRef Back() const
    {
        if (m_Size > 0)
            return this->operator[](m_Size - 1);
        else
            throw std::out_of_range("Tried calling Back() on an empty vector.");
    }
    inline BitRef At(const usize index)
    {
        if (m_Size - 1 >= index)
            return this->operator[](index);
        else
            throw std::out_of_range("Index out of bounds.");
    }
    inline const BitRef At(const usize index) const
    {
        if (m_Size - 1 >= index)
            return this->operator[](index);
        else
            throw std::out_of_range("Index out of bounds.");
    }
    constexpr void Swap(Vec<bool>& other)
    {
        std::swap(m_Size, other.m_Size);
        std::swap(m_Capacity, other.m_Capacity);
        std::swap(m_Buffer, other.m_Buffer);
    }
    inline void    Resize(const usize newSize) { Realloc(newSize); }
    constexpr void Reserve(const usize newCapacity)
    {
        if (newCapacity > m_Capacity)
        {
            const usize prev_capacity = m_Capacity;
            m_Capacity                = newCapacity;
            BufferType* temp          = m_Buffer;
            m_Buffer                  = new BufferType[m_Capacity];
            std::memcpy(m_Buffer, temp, (prev_capacity / BitSize) * sizeof(BufferType));
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
    Vec<bool> vec  = { 1, 0, 1, 0, 0, 1, 1, 0, 0 };
    Vec<bool> vec1 = { 1, 1, 1, 0, 0, 1, 1, 0, 1 };
    vec << vec1;
    std::cout << vec.ToString() << std::endl;
    vec <<= 3;
    std::cout << vec.ToString() << std::endl;
    vec >>= 1;
    std::cout << vec.ToString() << std::endl;
}

int main()
{
    std::bitset<2> a;
    auto           b = a[0];
    auto           d = 10;
    std::srand(std::time(nullptr));

    // TestVec();
}
