#ifndef LOOPITERABLE_H
#define LOOPITERABLE_H

#include <iterator>
#include <vector>


// Iterator for circularly linked list where the end and start are the same
//   uses isStart to resolve ambiguity
//   for empty list, first is nullptr
//
// E - cursor type
// V - value type
// AccessFunc - get reference to value from cursor
// AdvanceFunc - move cursor to next position
template <typename E, typename V, V& AccessFunc(E* e), E* AdvanceFunc(E* e)>
class LoopIterable  {
protected:
    E* first;
public:
    class Iterator {
    public:
        E* current;
        bool isStart;
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = V;
        using pointer           = value_type*;
        using reference         = value_type&;

        Iterator& operator++() {
            isStart = false;
            current = AdvanceFunc(current);
            return *this;
        }
        Iterator operator++(int) {
            Iterator i = *this;
            isStart = false;
            current = AdvanceFunc(current);
            return i;
        }
        bool operator==(const Iterator& other) const {
            return (current == other.current && isStart == other.isStart) || !current;
        }

        value_type& operator*() { return AccessFunc(current); }
    };

public:
    LoopIterable(E* first) : first{first} {}
    Iterator begin() const { return Iterator{first, true}; }
    Iterator end() const { return Iterator{first, false}; }
    std::vector<V> toVector() const;
    std::vector<V*> toVectorOfPtr() const;

    bool contains(E* element) {
        if (!this->first) {
            return false;
        }
        E* curr = this->first;
        do {
            if (curr == element) {
                return true;
            }
            curr = AdvanceFunc(curr);
        } while (curr != this->first);
        return false;
    }
};

template <typename E, typename V, V& AccessFunc(E* e), E* AdvanceFunc(E* e)>
inline std::vector<V> LoopIterable<E,V,AccessFunc,AdvanceFunc>::toVector() const
{
    std::vector<V> values;
    auto i = begin();
    auto e = end();
    while (i != e) {
        values.push_back(*i++);
    }
    return values;
}


template <typename E, typename V, V& AccessFunc(E* e), E* AdvanceFunc(E* e)>
inline std::vector<V*> LoopIterable<E,V,AccessFunc,AdvanceFunc>::toVectorOfPtr() const
{
    std::vector<V*> ptrs;
    auto i = begin();
    auto e = end();
    while (i != e) {
        ptrs.push_back(&(*i++));
    }
    return ptrs;
}


template <typename E>
class LoopIterableSimple  {
protected:
    E* first;
public:
    class Iterator {
    public:
        E* current;
        bool isStart;
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = E;
        using pointer           = value_type*;
        using reference         = value_type&;

        Iterator& operator++() {
            isStart = false;
            current = current->getNext();
            return *this;
        }
        Iterator operator++(int) {
            Iterator i = *this;
            isStart = false;
            current = current->getNext();
            return i;
        }
        bool operator==(const Iterator& other) const {
            return (current == other.current && isStart == other.isStart) || !current;
        }

        value_type& operator*() { return *current; }
    };

public:
    LoopIterableSimple(E* first) : first{first} {}
    Iterator begin() const { return Iterator{first, true}; }
    Iterator end() const { return Iterator{first, false}; }
    std::vector<E> toVector() const;
    std::vector<E*> toVectorOfPtr() const;

    bool contains(E* element) {
        if (!this->first) {
            return false;
        }
        E* curr = this->first;
        do {
            if (curr == element) {
                return true;
            }
            curr = curr->getNext();
        } while (curr != this->first);
        return false;
    }
};

template <typename E>
inline std::vector<E> LoopIterableSimple<E>::toVector() const
{
    std::vector<E> values;
    auto i = begin();
    auto e = end();
    while (i != e) {
        values.push_back(*i++);
    }
    return values;
}

template <typename E>
inline std::vector<E*> LoopIterableSimple<E>::toVectorOfPtr() const
{
    std::vector<E*> ptrs;
    auto i = begin();
    auto e = end();
    while (i != e) {
        ptrs.push_back(&(*i++));
    }
    return ptrs;
}

template <typename T>
class CircularList : public LoopIterableSimple<T> {
private:
    size_t count{};

public:
    CircularList() : LoopIterableSimple<T>{nullptr} {}

    void push_back(T* element) {
        if (this->first) {
            element->next = this->first;
            element->prev = this->first->prev;
            this->first->prev->next = element;
            this->first->prev = element;
        }
        else {
            element->next = element;
            element->prev = element;
            this->first = element;
        }
        count++;
    }

    void remove(T* element) {
        element->prev->next = element->next;
        element->next->prev = element->prev;
        if (this->first == element) {
            if (this->first->next == this->first) {
                // last element
                this->first = nullptr;
            }
            else {
                this->first = this->first->getNext();
            }
        }
        count--;
    }
    // Calls delete on all elements and clears the list
    void clear() {
        if (this->first) {
            auto* ptr = this->first->getNext();
            while (ptr != this->first) {
                auto next = ptr->getNext();
                delete ptr;
                count--;
                ptr = next;
            }
            delete this->first;
            count--;
            this->first = nullptr;
        }
    }

    T& front() { return *this->first; }
    T& back() { return *this->first->prev; }

    size_t size() const { return count; }
    bool empty() const { return this->first == nullptr; }

    T& pop_front() { T& f = front(); remove(&f); return f; }
};



#endif // LOOPITERABLE_H
