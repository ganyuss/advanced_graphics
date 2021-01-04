//
//  Framework for a raytracer
//  File: triple.h
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Author: Maarten Everts
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#ifndef TRIPLE_H_SEVQHPTA
#define TRIPLE_H_SEVQHPTA

// 0 : nothing, 1 : output in std::cerr, 2 : throwing error
#define DEBUG_CLAMP_LEVEL 0

#include <cmath>
#include <iostream>

template <typename T>
struct is_triple : std::integral_constant<bool, false> {};

template <typename T>
constexpr bool is_triple_v = is_triple<T>::value;

class Color;
class Vector;

template<>
struct is_triple<Color> : std::integral_constant<bool, true> {};
template<>
struct is_triple<Vector> : std::integral_constant<bool, true> {};
template<>
struct is_triple<const Color> : std::integral_constant<bool, true> {};
template<>
struct is_triple<const Vector> : std::integral_constant<bool, true> {};


template <typename Triple, typename Type, typename = std::enable_if<is_triple_v<Triple>, bool>>
class triple_iterator {
public:
    explicit triple_iterator(Triple& triple) : triple(triple), index(0)
    { }


    Type& operator*() { return triple[index]; }
    const Type& operator*() const { return triple[index]; }
    bool operator==(const triple_iterator<Triple, Type>& other) const { return index == other.index; }
    bool operator!=(const triple_iterator<Triple, Type>& other) const { return ! (*this == other); }

    template<typename Index, typename = std::enable_if<std::is_integral<Index>::value, bool>>
    triple_iterator& operator+=(Index i) { index += i; return *this; }
    template<typename Index, typename = std::enable_if<std::is_integral<Index>::value, bool>>
    triple_iterator& operator-=(Index i) { index -= i; return *this; }
    template<typename Index, typename = std::enable_if<std::is_integral<Index>::value, bool>>
    Type& operator[](Index i) { return *((*this) + i); }

    explicit operator std::size_t() const { return index; }

    virtual ~triple_iterator() = default;

private:
    Triple& triple;
    std::size_t index;
};

template <typename Triple, typename Type>
using const_triple_iterator = std::conditional_t<
        std::is_const<Triple>::value,
        triple_iterator<Triple, Type>,
        triple_iterator<Triple, const Type>>;

template<typename Triple, typename Type, typename Index>
triple_iterator<Triple, Type> operator+(triple_iterator<Triple, Type> it, Index index) { return it += index; }
template<typename Triple, typename Type, typename Index>
triple_iterator<Triple, Type> operator+(Index index, triple_iterator<Triple, Type> it) { return it + index; }

template<typename Triple, typename Type, typename Index>
triple_iterator<Triple, Type> operator-(triple_iterator<Triple, Type> it, Index i) { return it + (-i); }
template<typename Triple, typename Type>
int operator-(const triple_iterator<Triple, Type>& it1, const triple_iterator<Triple, Type>& it2)
{ return static_cast<int>(it1) - static_cast<int>(it2); }

template<typename Triple, typename Type>
triple_iterator<Triple, Type>& operator++(triple_iterator<Triple, Type>& it) { return it += 1; }
template<typename Triple, typename Type>
triple_iterator<Triple, Type> operator++(triple_iterator<Triple, Type>& it, int) { triple_iterator<Triple, Type> copy(it); it += 1; return copy; }
template<typename Triple, typename Type>
triple_iterator<Triple, Type> operator--(triple_iterator<Triple, Type>& it) { return it += -1; }
template<typename Triple, typename Type>
triple_iterator<Triple, Type> operator--(triple_iterator<Triple, Type>& it, int) { triple_iterator<Triple, Type> copy(it); it -= 1; return copy; }


template<typename Triple, typename Type>
inline bool operator<(const triple_iterator<Triple, Type>& a, const triple_iterator<Triple, Type>& b) { return b - a > 0; }
template<typename Triple, typename Type>
inline bool operator>(const triple_iterator<Triple, Type>& a, const triple_iterator<Triple, Type>& b) { return b - a < 0; }
template<typename Triple, typename Type>
inline bool operator<=(const triple_iterator<Triple, Type>& a, const triple_iterator<Triple, Type>& b) { return !(a > b); }
template<typename Triple, typename Type>
inline bool operator>=(const triple_iterator<Triple, Type>& a, const triple_iterator<Triple, Type>& b) { return !(a < b); }


// Can't make minimum and maximum ValueType type (floating point type in template only available in C++20)
template<typename ValueType, int minimum, int maximum>
class clamped_value {
public:

    clamped_value() = default;

    clamped_value(ValueType v) : val(v) {
        check_val();
    }

    operator ValueType&() {
        check_val();
        return val;
    }

    operator ValueType() const {
        check_val();
        return val;
    }


private:

    void check_val() {
        if (val < minimum) {
            val = minimum;
        }
        else if (val > maximum) {
            val = maximum;
        }
    }

    ValueType val;
};


class Color {
public:

    typedef triple_iterator<Color, double> iterator;
    typedef triple_iterator<const Color, const double> const_iterator;

    typedef clamped_value<double, 0, 1> component;

    Color() = default;
    Color(const Color&) = default;
    Color(Color&&) = default;
    Color& operator=(const Color&) = default;
    Color& operator=(Color&&) = default;

    Color(component red, component green, component blue);

    template <typename Triple, class = typename std::enable_if<is_triple_v<Triple>, bool>>
    explicit Color(const Triple&);

    component& Red() { return r; }
    component& Green() { return g; }
    component& Blue() { return b; }
    component Red() const { return r; }
    component Green() const { return g; }
    component Blue() const { return b; }

    void set(component red, component green, component blue);

    component& operator[] (std::size_t i) {
        component& (Color::*methods[])() = { &Color::Red, &Color::Green, &Color::Blue };
        return (this->*methods[i])();
    }
    component operator[] (std::size_t i) const {
        component (Color::*methods[])() const = { &Color::Red, &Color::Green, &Color::Blue };
        return (this->*methods[i])();
    }

    bool operator==(const Color& other) const {
        return Red() == other.Red() && Green() == other.Green() && Blue() == other.Blue();
    }

    iterator begin() { return iterator(*this); }
    iterator end() { return iterator(*this) + 3; }
    const_iterator cbegin() const { return const_iterator(*this); }
    const_iterator cend() const { return const_iterator(*this) + 3; }


    ~Color() = default;

private:
    component r, g, b;
};

class Vector {
public:

    typedef triple_iterator<Vector, double> iterator;
    typedef triple_iterator<const Vector, const double> const_iterator;

    Vector() = default;
    Vector(const Vector&) = default;
    Vector(Vector&&) = default;
    Vector& operator=(const Vector&) = default;
    Vector& operator=(Vector&&) = default;

    Vector(double x, double y, double z);

    template <typename Triple, typename = std::enable_if<is_triple_v<Triple>, bool>>
    explicit Vector(const Triple&);

    double& X() { return x; }
    double& Y() { return y; }
    double& Z() { return z; }

    double X() const { return x; }
    double Y() const { return y; }
    double Z() const { return z; }

    double& operator[] (std::size_t i) {
        double& (Vector::*methods[])() = { &Vector::X, &Vector::Y, &Vector::Z };
        return (this->*methods[i])();
    }
    double operator[] (std::size_t i) const {
        double (Vector::*methods[])() const = { &Vector::X, &Vector::Y, &Vector::Z };
        return (this->*methods[i])();
    }

    double dot(const Vector& other) const;
    inline double norm() const;
    void normalize();
    Vector normalized() const;

    iterator begin() { return iterator{*this}; }
    iterator end() { return iterator{*this} + 3; }
    const_iterator cbegin() const { return const_iterator{*this}; }
    const_iterator cend() const { return const_iterator{*this} + 3; }


    ~Vector() = default;

private:
    double x, y, z;
};

using Point = Vector;



template <typename Triple, class = typename std::enable_if<is_triple_v<Triple>, bool>::type>
bool operator==(const Triple& t1, const Triple& t2) { for (int i = 0; i < 3; ++i) if (t1[i] != t2[i]) { return false; } return true; }
template <typename Triple, class = typename std::enable_if<is_triple_v<Triple>, bool>::type>
inline bool operator!=(const Triple& t1, const Triple& t2) { return ! (t1 == t2); }

template <typename Triple, class = typename std::enable_if<is_triple_v<Triple>, bool>::type>
Triple operator-( const Triple& t1) { Triple output(t1); for (int i = 0; i < 3; ++i) output[i] *= -1; return output; }

template <typename Triple, class = typename std::enable_if<is_triple_v<Triple>, bool>::type>
inline Triple& operator+=(Triple& t1, const Triple& t2) { for (int i = 0; i < 3; ++i) t1[i] += t2[i]; return t1; }
template <typename Triple, class = typename std::enable_if<is_triple_v<Triple>, bool>::type>
inline Triple& operator-=(Triple& t1, const Triple& t2) { return t1 += (-t2); }


template <typename Triple, class = typename std::enable_if<is_triple_v<Triple>, bool>::type>
inline Triple operator+(const Triple& t1, const Triple& t2) { Triple output(t1); return output += t2; }
template <typename Triple, class = typename std::enable_if<is_triple_v<Triple>, bool>::type>
inline Triple operator-(const Triple& t1, const Triple& t2) { return t1 + (-t2); }

template <typename Triple, typename Number, class = typename std::enable_if<is_triple_v<Triple>, bool>::type, bool = std::is_arithmetic<Number>::value>
Triple operator+(const Triple& t1, Number n) { Triple output(t1); for (int i = 0; i < 3; ++i) output[i] += n; return output; }
template <typename Triple, typename Number, class = typename std::enable_if<is_triple_v<Triple>, bool>::type, bool = std::is_arithmetic<Number>::value>
inline Triple operator+(Number n, const Triple& t1) { return t1 + n; }
template <typename Triple, typename Number, class = typename std::enable_if<is_triple_v<Triple>, bool>::type, bool = std::is_arithmetic<Number>::value>
inline Triple operator-(const Triple& t1, Number n) { return t1 + (-n); }

template <typename Triple, typename Number, class = typename std::enable_if<is_triple_v<Triple>, bool>::type, bool = std::is_arithmetic<Number>::value>
Triple operator*(const Triple& t1, Number n) { Triple output(t1); for (int i = 0; i < 3; ++i) output[i] *= n; return output; }
template <typename Triple, typename Number, class = typename std::enable_if<is_triple_v<Triple>, bool>::type, bool = std::is_arithmetic<Number>::value>
inline Triple operator*(Number n, const Triple& t1) { return t1 * n; }
template <typename Triple, typename Number, class = typename std::enable_if<is_triple_v<Triple>, bool>::type, bool = std::is_arithmetic<Number>::value>
Triple operator/(Triple t1, Number n) { for (int i = 0; i < 3; ++i) t1[i] /= n; return t1; }


#endif /* end of include guard: TRIPLE_H_SEVQHPTA */
