//
// Created by ilya on 15.06.19.
//


#ifndef EKZAMEN_VECTOR_H
#define EKZAMEN_VECTOR_H

#include <iostream>
#include <memory>
#include <cstring>
#include <assert.h>


template<typename T>
struct my_vector {

    typedef T value_type;
    typedef T *iterator;
    typedef T const *const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;


    my_vector();

    // ========================= iterator =========================== //
    iterator begin();

    const_iterator begin() const;

    iterator end();

    const_iterator end() const;

    reverse_iterator rbegin();

    const_reverse_iterator rbegin() const;

    reverse_iterator rend();

    const_reverse_iterator rend() const;

    iterator insert(const_iterator pos, T val);

    iterator erase(const_iterator pos);

    iterator erase(const_iterator first, const_iterator last);

    // ======================= end iterator ===========================//

    my_vector(size_t, T);

    my_vector(my_vector const &);

    my_vector &operator=(my_vector const &);

    size_t size() const;

    T *data();

    const T *data() const;

    T &back();

    const T &back() const;

    T &front();

    const T &front() const;

    T &operator[](size_t);

//    bool operator==(my_vector<T> const &) const;

    const T &operator[](size_t) const;

    void pop_back();

    void push_back(T);

    void resize(size_t, T);

    void *alloc(size_t);

    ~my_vector();

    void reserve(size_t);

    bool empty() const noexcept;

    bool is_small() const;

    void shrink_to_fit();

    void clear() noexcept;

    size_t all_size();

    size_t cnt_links();

    void make_small(T x);

    void inv();

    void free_data();

    template<typename InputIterator>
    my_vector(InputIterator first, InputIterator last);

    template<typename InputIterator>
    void assign(InputIterator, InputIterator);

    // friend bool operator !=(my_vector<T> const &a, my_vector<T> const & b);
    union {
        T small_object;
    };
    void *dat; // cur_size, all_size, links, data[0], data[1] ...

};

template<typename T>
my_vector<T>::my_vector() {
    dat = nullptr;
}

template<typename T>
size_t my_vector<T>::size() const {
    if (!dat) {
        return 0;
    }
    if (dat == &small_object) {
        return 1;
    }
    return *(static_cast<size_t *> (dat));
}

template<typename T>
void my_vector<T>::make_small(T x) {
    // small_object = x;
    new(&small_object) T(x);
    dat = &small_object;
}

template<typename T>
my_vector<T>::my_vector(size_t sz, T x) {
    if (sz == 0) {
        dat = nullptr;
    } else if (sz == 1) {
        make_small(x);
    } else {
        dat = (void *) malloc(sizeof(size_t) * 3 + sizeof(T) * sz * 2);
        // (*data) = sz;
        *(static_cast<size_t *> (dat)) = sz;
        *(static_cast<size_t *> ((size_t *) dat + 1)) = sz * 2;
        *(static_cast<size_t *> ((size_t *) dat + 2)) = 1;
        size_t *cur = (size_t *) dat + 3;
        T *st = (T *) cur;
        for (size_t i = 0; i < sz; ++i) {
            *(st + i) = x;
        }
    }
}

template<typename T>
T &my_vector<T>::operator[](size_t sz) {
    if (is_small()) {
        assert(sz == 0);
        return small_object;
    }
    inv();
    size_t *cur = (size_t *) dat + 3;
    T *st = (T *) cur;
    return (*(st + sz));
}

template<typename T>
my_vector<T>::~my_vector() {

    free_data();
}

template<typename T>
const T &my_vector<T>::operator[](size_t sz) const {
    if (size() == 1) {
        return small_object;
    }
    size_t *cur = (size_t *) dat + 3;
    T *st = (T *) cur;
    return *(st + sz);
}

template<typename T>
void my_vector<T>::inv() {
    if (empty() || is_small()) {
        return;
    }
    size_t cur_sz = size();
    size_t all = all_size();
    size_t links = cnt_links();
    if (links != 1) {
        //std::cout << "inv " << '\n';
        size_t sz = sizeof(size_t) * 3 + sizeof(T) * all;
        void *new_data;
        try {
            new_data = (void *) malloc(sz);
        } catch (...) {
            throw;
        }
        try {
            T *nd = (T *) ((size_t *) new_data + 3);
            T *d = (T *) ((size_t *) dat + 3);
            for (size_t i = 0; i < cur_sz; ++i) {
                try {
                    new(nd + i) T(*(d + i));
                } catch (...) {
                    for (size_t j = 0; j < i; ++j) {
                        (*(nd + j)).~T();
                    }
                    throw;
                }
            }
        } catch (...) {
            free(new_data);
            throw;
        }
        *(static_cast<size_t *> ((size_t *) dat + 2)) = links - 1;
        dat = new_data;
        *(static_cast<size_t *> ((size_t *) dat)) = cur_sz;
        *(static_cast<size_t *> ((size_t *) dat + 1)) = all;
        *(static_cast<size_t *> ((size_t *) dat + 2)) = 1;
    }
}

template<typename T>
size_t my_vector<T>::all_size() {
    if (empty()) {
        return 0;
    }
    if (is_small()) {
        return 1;
    }
    return *(static_cast<size_t *> ((size_t *) dat + 1));
}

template<typename T>
size_t my_vector<T>::cnt_links() {
    return *(static_cast<size_t *> ((size_t *) dat + 2));
}

template<typename T>
void *my_vector<T>::alloc(size_t new_size) {
    void *res = nullptr;
    try {
        res = (void *) malloc(sizeof(size_t) * 3 + sizeof(T) * new_size);
    } catch (...) {
        res = nullptr;
        throw;
    }

    *(static_cast<size_t *> ((size_t *) res)) = 0;
    *(static_cast<size_t *> ((size_t *) res + 1)) = new_size;
    *(static_cast<size_t *> ((size_t *) res + 2)) = 1;
    return res;
}

template<typename T>
void my_vector<T>::reserve(size_t new_size) {
    new_size = std::max(new_size, size_t(2));
    if (empty()) {
        try {
            dat = (void *) malloc(sizeof(size_t) * 3 + sizeof(T) * new_size);
        } catch (...) {
            free(dat);
            throw;
        }
        *(static_cast<size_t *> ((size_t *) dat)) = 0;
        *(static_cast<size_t *> ((size_t *) dat + 1)) = new_size;
        *(static_cast<size_t *> ((size_t *) dat + 2)) = 1;
        return;
    } else if (is_small()) {
        try {
            dat = (void *) malloc(sizeof(size_t) * 3 + sizeof(T) * new_size);
        } catch (...) {
            free(dat);
            throw;
        }
        size_t *cur = (size_t *) dat + 3;
        T *st = (T *) cur;
        try {
            new(st) T(small_object);
        } catch (...) {
            free(dat);
            dat = &small_object;
            throw;
        }
        small_object.~T();
        *(static_cast<size_t *> ((size_t *) dat)) = 1;
        *(static_cast<size_t *> ((size_t *) dat + 1)) = new_size;
        *(static_cast<size_t *> ((size_t *) dat + 2)) = 1;
        return;
    }
    inv();
    size_t all = new_size;
    size_t sz = size();
    size_t links = cnt_links();
    void *new_data = (void *) malloc(sizeof(size_t) * 3 + sizeof(T) * new_size);
    try {
        T *nd = (T *) ((size_t *) new_data + 3);
        T *d = (T *) ((size_t *) dat + 3);

        for (size_t i = 0; i < sz; ++i) {
            try {
                new(nd + i) T(*(d + i));
            } catch (...) {
                for (size_t j = 0; j < i; ++j) {
                    (*(nd + j)).~T();
                }
                throw;
            }
        }
        for (size_t i = 0; i < sz; ++i) {
            (*(d + i)).~T();
        }
    } catch (...) {
        free(new_data);
        throw;
    }
    free(dat);
    dat = new_data;
    *(static_cast<size_t *> ((size_t *) dat)) = sz;
    *(static_cast<size_t *> ((size_t *) dat + 1)) = all;
    *(static_cast<size_t *> ((size_t *) dat + 2)) = links;
}

template<typename T>
void my_vector<T>::push_back(T x) {
    if (empty()) {
        new(&small_object) T(x);
        dat = &small_object;
    } else if (is_small()) {
        void *new_data = alloc(4);
        size_t *cur = (size_t *) new_data + 3;
        T *st = (T *) cur;
        try {
            new(st) T(small_object);
        } catch (...) {
            free(new_data);
            throw;
        }
        try {
            new(st + 1) T(x);
        } catch (...) {
            free(new_data);
            (*st).~T();
            throw;
        }
        small_object.~T();
        *(static_cast<size_t *> ((size_t *) new_data)) = 2;
        dat = new_data;
    } else {
        size_t all = all_size();
        size_t cur_sz = size();
        T lol = x;
        inv();
        if (cur_sz + 1 >= all) {
            reserve(cur_sz * 2);
        }
        size_t *cur = (size_t *) dat + 3;
        T *st = (T *) cur;
        try {
            new(st + cur_sz) T(lol);
        } catch (...) {
            throw;
        }
        *(static_cast<size_t *> ((size_t *) dat)) = cur_sz + 1;

    }
}

template<typename T>
void my_vector<T>::free_data() {
    if (!empty() && !is_small()) {
        *(static_cast<size_t *> ((size_t *) dat + 2)) = cnt_links() - 1;
    }
    if (is_small()) {
        small_object.~T();
    }
    if (!is_small() && !empty() && cnt_links() == 0) {
        size_t *cur = (size_t *) dat + 3;
        T *st = (T *) cur;
        for (size_t i = 0; i < size(); ++i) {
            (*(st + i)).~T();
        }
        free(dat);
    }
}

template<typename T>
my_vector<T> &my_vector<T>::operator=(const my_vector<T> &other) {
    if (dat == other.dat) {
        return *this;
    }
    free_data();
    if (other.empty()) {
        dat = nullptr;
    } else if (other.is_small()) {
        new(&small_object) T(other.small_object);
        dat = &small_object;
    } else {
        dat = other.dat;
        *(static_cast<size_t *> ((size_t *) dat + 2)) = cnt_links() + 1;
    }
    return *this;
}

template<typename T>
bool my_vector<T>::is_small() const {
    return dat == &small_object;
}

template<typename T>
bool my_vector<T>::empty() const noexcept {
    return dat == nullptr;
}

template<typename T>
my_vector<T>::my_vector(my_vector const &other) {
    if (other.empty()) {
        dat = nullptr;
    } else if (other.is_small()) {
        new(&small_object) T(other.small_object);
        dat = &small_object;
    } else {
        dat = other.dat;
        *(static_cast<size_t *> ((size_t *) dat + 2)) = cnt_links() + 1;
    }
}

template<typename T>
void my_vector<T>::pop_back() {
    assert(!empty());
    if (is_small()) {
        small_object.~T();
        dat = nullptr;
    } else if (size() == 2) {
        T mem = (*this)[0];
        new(&small_object) T(mem);
        free_data();
        dat = &small_object;
    } else {
        inv();
        size_t *cur = (size_t *) dat + 3;
        T *st = (T *) cur;
        size_t sz = size();
        (*(st + sz - 1)).~T();
        *(static_cast<size_t *> ((size_t *) dat)) = size() - 1;
    }
}

template<typename T>
void my_vector<T>::resize(size_t new_sz, T x) {
    while (size() < new_sz) {
        push_back(x);
    }
    while (size() > new_sz) {
        pop_back();
    }
    // *(static_cast<size_t *> ((size_t *) dat)) = new_sz;
}

template<typename T>
T &my_vector<T>::back() {
    if (is_small()) {
        return small_object;
    }
    return (*this)[size() - 1];
}

template<typename T>
T &my_vector<T>::front() {
    if (is_small()) {
        return small_object;
    }
    return (*this)[0];;
}

template<typename T>
const T &my_vector<T>::back() const {
    if (is_small()) {
        return small_object;
    }
    return (*this)[size() - 1];;
}

template<typename T>
const T &my_vector<T>::front() const {
    if (is_small()) {
        return small_object;
    }
    return (*this)[0];
}

template<typename T>
typename my_vector<T>::iterator my_vector<T>::begin() {
    inv();
    if (is_small()) {
        return (T *) dat;
    }
    size_t *cur = (size_t *) dat + 3;
    T *st = (T *) cur;
    return st;
}

template<typename T>
typename my_vector<T>::const_iterator my_vector<T>::begin() const {
    if (is_small()) {
        return (T *) dat;
    }
    size_t *cur = (size_t *) dat + 3;
    T *st = (T *) cur;
    return st;
}

template<typename T>
typename my_vector<T>::reverse_iterator my_vector<T>::rbegin() {
    iterator e = (iterator) end();
    return std::reverse_iterator<iterator>(e);
}

template<typename T>
typename my_vector<T>::const_reverse_iterator my_vector<T>::rbegin() const {
    iterator e = (iterator) end();
    return std::reverse_iterator<iterator>(e);
}

template<typename T>
typename my_vector<T>::iterator my_vector<T>::end() {
    inv();
    if (is_small()) {
        return (T *) dat + 1;
    }
    size_t *cur = (size_t *) dat + 3;
    T *st = (T *) cur;
    return st + size();
}

template<typename T>
typename my_vector<T>::const_iterator my_vector<T>::end() const {
    if (is_small()) {
        return (T *) dat + 1;
    }
    size_t *cur = (size_t *) dat + 3;
    T *st = (T *) cur;
    return st + size();
}

template<typename T>
typename my_vector<T>::reverse_iterator my_vector<T>::rend() {
    return std::reverse_iterator<iterator>((iterator) begin());
}

template<typename T>
typename my_vector<T>::const_reverse_iterator my_vector<T>::rend() const {
    return std::reverse_iterator<iterator>((iterator) begin());
}

template<typename T>
typename my_vector<T>::iterator my_vector<T>::insert(my_vector::const_iterator pos, T val) {
    if (empty()) {
        size_t len = pos - begin();
        push_back(val);
        return begin() + len;
    }
    size_t len = pos - begin();
    //std::cout << "len! " << len << '\n';
    if (is_small()) {
        reserve(4);
    }

    inv();
    size_t cur_sz = size();
    if (cur_sz + 2 >= all_size()) {
        reserve(cur_sz * 2);
    }
    push_back(val);
    pos = begin() + len;
    auto i = end() - 1;
    //std::cout << pos << " " << end() << " " << (end() - pos) << '\n';
    for (; i > pos; --i) {
        std::swap(*i, *(i - 1));
    }

    return begin() + len;
}


template<typename T>
T *my_vector<T>::data() {
    if (is_small()) {
        return (T *) dat;
    }
    size_t *cur = (size_t *) dat + 3;
    T *st = (T *) cur;
    return st;
}

template<typename T>
const T *my_vector<T>::data() const {
    if (is_small()) {
        return (T *) dat;
    }
    size_t *cur = (size_t *) dat + 3;
    T *st = (T *) cur;
    return st;
}

template<typename T>

typename my_vector<T>::iterator my_vector<T>::erase(my_vector::const_iterator first, my_vector::const_iterator last) {
    inv();
    size_t len = last - first;
    size_t prev_size = size();
    for (iterator i = (iterator) last; i < end(); ++i) {
        std::swap(*(i), *(i - len));
    }
    size_t new_size = prev_size - len;
    iterator i = begin() + new_size;
    while (i < end()) {
        i++;
        (*(i - 1)).~T();
    }
    if (new_size == 0) {
        free(dat);
        dat = nullptr;
    } else if (new_size == 1) {
        T mem = (*this)[0];
        new(&small_object) T(mem);
        free_data();
        dat = &small_object;
    } else {
        *(static_cast<size_t *> (dat)) = new_size;
        // *(static_cast<size_t *> ((size_t *) dat + 1)) = sz * 2;
        // *(static_cast<size_t *> ((size_t *) dat + 2)) = 1;
    }
    return data() + size();
}

template<typename T>
typename my_vector<T>::iterator my_vector<T>::erase(my_vector::const_iterator pos) {
    return erase(pos, pos + 1);
}

template<typename T>
void my_vector<T>::clear() noexcept {
    inv();
    free_data();
}

template<typename T>
void my_vector<T>::shrink_to_fit() {
    inv();
    my_vector<T> c;
    c.reserve(size());
    for (size_t i = 0; i < size(); ++i) {
        c.push_back((*this)[i]);
    }
    swap(c, *this);

}

template<typename T>
template<typename InputIterator>
my_vector<T>::my_vector(InputIterator first, InputIterator last) {
    my_vector<T> c;
    while (first != last) {
        c.push_back(*first);
        first++;
    }
    swap(c, *this);
}

template<typename T>
template<typename InputIterator>
void my_vector<T>::assign(InputIterator first, InputIterator second) {
    inv();
    my_vector<T> c;
    while (first != second) {
        c.push_back(*first);
        first++;
    }
    swap(c, *this);
}

template<typename T>
bool operator==(const my_vector<T> &a, const my_vector<T> &b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

template<typename T>
bool operator<(const my_vector<T> &a, const my_vector<T> &b) {
    size_t i = 0;
    while (i < a.size() && i < b.size()) {
        if (a[i] < b[i]) {
            return true;
        }
        if (a[i] > b[i]) {
            return false;
        }
        i++;
    }
    return i == a.size() && i < b.size();
}

template<typename T>
bool operator<=(const my_vector<T> &a, const my_vector<T> &b) {
    return (a < b || a == b);
}

template<typename T>
bool operator>(const my_vector<T> &a, const my_vector<T> &b) {
    return !(a <= b);
}

template<typename T>
bool operator>=(const my_vector<T> &a, const my_vector<T> &b) {
    return !(a < b);
}

template<typename T>
bool operator!=(const my_vector<T> &a, const my_vector<T> &b) {
    return !(a == b);
}

template<typename T>
void swap(my_vector<T> &a, my_vector<T> &b) {
    if (a.is_small() && b.is_small()) {
        std::swap(a.small_object, b.small_object);
    } else if (a.is_small() && !b.empty()) {
        try {
            new(&b.small_object) T(a.small_object);
        } catch (...) {
            throw;
        }
        a.dat = b.dat;
        a.small_object.~T();
        b.dat = &b.small_object;

    } else if (b.is_small() && !a.empty()) {
        try {
            new(&a.small_object) T(b.small_object);
        } catch (...) {
            throw;
        }
        b.dat = a.dat;
        b.small_object.~T();
        a.dat = &a.small_object;

    } else {
        std::swap(a, b);
    }
}

#endif //EKZAMEN_VECTOR_H
