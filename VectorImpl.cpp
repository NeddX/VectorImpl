#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <iostream>
#include <iterator>

using usize   = std::size_t;
using ptrdiff = std::ptrdiff_t;
using u32     = std::uint32_t;
using i32     = std::int32_t;

template <typename T>
class Vec
{
private:
    T*    m_Buffer = nullptr;
    usize m_Size;
    usize m_Capacity;

private:
    class Iterator
    {
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
        constexpr Iterator operator+(const i32 disp) const noexcept
        {
            Iterator temp = *this;
            temp.m_Ptr += disp;
            return temp;
        };
        constexpr Iterator operator-(const i32 disp) const noexcept
        {
            Iterator temp = *this;
            temp.m_Ptr -= disp;
            return temp;
        }

    public:
        friend bool operator==(const Iterator& lhv, const Iterator& rhv) noexcept { return lhv.m_Ptr == rhv.m_Ptr; }
        friend bool operator!=(const Iterator& lhv, const Iterator& rhv) noexcept { return !(lhv == rhv); }
    };

public:
    Vec(const std::initializer_list<T> list)
    {
        if (m_Buffer)
            delete[] m_Buffer;
        m_Size     = list.size();
        m_Capacity = m_Size;
        m_Buffer   = new T[m_Capacity];
        std::copy(list.begin(), list.end(), m_Buffer);
    }
    Vec(const usize size = 1) : m_Size(size), m_Capacity(size * 2) { m_Buffer = new T[m_Capacity]; }
    ~Vec()
    {
        delete[] m_Buffer;
        m_Buffer = nullptr;
    }

public:
    constexpr usize Size() const noexcept { return m_Size; }
    constexpr usize Capacity() const noexcept { return m_Capacity; }
    constexpr bool  Empty() const noexcept { return m_Size == 0; }
    constexpr T*    Data() const noexcept { return m_Buffer; }

public:
    constexpr Iterator begin() noexcept { return Iterator(m_Buffer); }
    constexpr Iterator end() noexcept { return Iterator(m_Buffer + m_Size); }

private:
    void Realloc(const usize newSize, const bool reserveExtra = true)
    {
        const usize prev_size = m_Size;

        if (reserveExtra)
            m_Capacity = newSize * 2;
        else
            m_Capacity = newSize;

        m_Size = newSize;

        T* temp  = m_Buffer;
        m_Buffer = new T[m_Capacity * sizeof(T)];
        if (!m_Buffer)
            throw std::bad_alloc();
        std::memcpy(m_Buffer, temp, prev_size * sizeof(T));
        if (temp != m_Buffer)
            delete[] temp;
    }

public:
    void Push(const T& e)
    {
        if (m_Size >= m_Capacity)
            Realloc(m_Size + 1);
        m_Buffer[m_Size++] = e;
    }
    T& Pop()
    {
        if (m_Size > 0)
            return m_Buffer[m_Size--];
        else
            throw std::out_of_range("Tried calling Pop() on an empty vector.");
    }
    T& Front()
    {
        if (m_Size > 0)
            return m_Buffer[0];
        else
            throw std::out_of_range("Tried calling Front() on an empty vector.");
    }
    T& Back()
    {
        if (m_Size > 0)
            return m_Buffer[m_Size - 1];
        else
            throw std::out_of_range("Tried calling Back() on an empty vector.");
    }
    T& At(const usize index)
    {
        if (m_Size - 1 >= index)
            return m_Buffer[index];
        else
            throw std::out_of_range("Index out of bounds.");
    }
    void ShrinkToFit() { Realloc(m_Size, false); }
    void Clear() noexcept { m_Size = 0; }
    void Assign(const usize count, const T& value)
    {
        Realloc(count);
        for (auto& e : this)
            e = value;
    }
    void Assign(Iterator begin, Iterator end)
    {
        Realloc(end - begin);
        for (auto it = begin; it != end; ++it)
            m_Buffer[it - begin] = *it;
    }
    void Assign(const std::initializer_list<T> list)
    {
        Realloc(list.size());
        std::copy(list.begin(), list.end(), begin());
    }
    void Swap(Vec<T>& other)
    {
        std::swap(m_Buffer, other.m_Buffer);
        std::swap(m_Capacity, other.m_Capacity);
        std::swap(m_Size, other.m_Size);
    }
    void Insert(const Iterator pos, const T& value)
    {
        const auto  diff          = pos - begin();
        const usize prev_capacity = m_Capacity;
        const T*    temp          = m_Buffer;

        ++m_Size;
        m_Capacity = m_Size * 2;
        m_Buffer   = new T[m_Capacity];
        for (usize i = 0, j = 0; i < m_Size; ++i)
        {
            if (i == diff)
                m_Buffer[i] = value;
            else
                m_Buffer[i] = temp[j++];
        }
    }
    template <typename... TArgs>
    void Emplace(const Iterator pos, TArgs&&... args)
    {
        auto t = T(std::forward<TArgs>(args)...);
    }

public:
    Vec<T>& operator=(const std::initializer_list<T> list)
    {
        Realloc(list.size());
        std::copy(list.begin(), list.end(), begin());
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
    // ShrinToFit
    // Swap
    // Clear
    // Pop
    // <<
    // =
    //
    std::srand(std::time(nullptr));

    Vec<int> vec;
    Vec<int> vec2 = { 5, 3, 1 };
    for (usize i = 0; i < 10; ++i)
        vec.Push(std::rand() % 256);

    vec.Swap(vec2);

    std::cout << vec << std::endl;
    std::cout << vec2 << std::endl;

    std::cout << "Ptr: " << vec.Data() << std::endl;
    vec.Clear();
    std::cout << vec << std::endl;
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

    vec.Insert(vec.begin(), 88);
    std::cout << "\nAfter insert: " << std::endl;
    std::cout << vec << std::endl;
    return 0;
}
