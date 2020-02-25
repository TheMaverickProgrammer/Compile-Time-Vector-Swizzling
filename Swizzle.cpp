/*
Author: TheMaverickProgrammer
2/24/2020

Try it on godbolt!
https://godbolt.org/z/GDHrJe
*/
/*
Author: TheMaverickProgrammer
9/14/2018
*/

#include <iostream>
#include <cstring>
#include <array>

//vec types
template<typename T>
struct swizzle_trait {
    using ParentT = T;    
    ParentT* parent;

    swizzle_trait(ParentT* p) : parent(p){}
 
    template<typename Holder>
    constexpr auto swizzle(Holder holder);
};

struct Vec1 : swizzle_trait<Vec1> {
    float x;
    
    Vec1(float x) : x(x), swizzle_trait<Vec1>(this) { }
};
struct Vec2 : swizzle_trait<Vec2> {
    float x, y;
    
    Vec2(float x, float y) : x(x), y(y), swizzle_trait<Vec2>(this) { }
};
struct Vec3 : swizzle_trait<Vec3> {
    float x, y, z;
    
    Vec3(float x, float y, float z) : x(x), y(y), z(z), swizzle_trait<Vec3>(this) { }
};
struct Vec4 : swizzle_trait<Vec4> {
    float x, y, z, w;

    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w), swizzle_trait<Vec4>(this) { }
};

//vec type helper for meta programming: get amount of elements in vec
template <typename T>
constexpr size_t getElementCount() {
    if constexpr(std::is_same<T, Vec1>::value)
        return 1u;
    else if constexpr(std::is_same<T, Vec2>::value)
        return 2u;
    else if constexpr(std::is_same<T, Vec3>::value)
        return 3u;
    else if constexpr(std::is_same<T, Vec4>::value)
        return 4u;
    else 
        return SIZE_MAX;
}

//vec type helper for meta programming: get x y z or w element based on index
template <size_t i, typename TVec>
constexpr float getElement(TVec v) {
    static_assert(i < getElementCount<TVec>(), "error too big size given");

    if constexpr(i == 0)
        return v.x;
    else if constexpr(i == 1)
        return v.y;
    else if constexpr(i == 2)
        return v.z;
    else // we asserted before, this must be i == 3
        return v.w;
}

//helper to convert swizzle char into index. i.e. x -> 0, y -> 1  etc
constexpr size_t vecCharToIndex(char c) {
    if(c == 'x')
        return 0u;
    else if(c == 'y')
        return 1u;
    else if(c == 'z')
        return 2u;
    else if(c == 'w')
        return 3u;
    else
        return SIZE_MAX;
}

//returns true if the given swizzle string is validly formatted - packaged as constexpr lambda
template <typename ConstantStringHolder>
constexpr size_t isValidSwizzleString(ConstantStringHolder holder) {
    constexpr auto string = holder();
    constexpr size_t length = string.size();

    //length must be between 1 and 4 swizzle characters
    if(length < 1u || length > 4u)
        return 0u;

    //only xyzw allowed
    for(size_t i = 0; i < length; ++i) {
        char current = string[i];
        if(current != 'x' && current != 'y' && current != 'z' && current != 'w')
            return 0u;
    }

    return 1u;
}

//returns the largest swizzle-index of a swizzle string. i.e. "xzxx" will return 2 since x is 0 and z is 2 and z is largest - packaged as constexpr lambda
template <typename ConstantStringHolder>
constexpr size_t getLargestSwizzleIndex(ConstantStringHolder holder) {
    constexpr auto string = holder();
    constexpr size_t length = string.size();

    size_t largest = 0;

    for(size_t i = 0; i < length; ++i)
    {
        size_t thisIndex = vecCharToIndex(string[i]);
        if(thisIndex > largest)
            largest = thisIndex;
    }

    return largest;
}

//swizzles!!!!! fully compile time validated. takes swizzle string packaged as constexpr lambda
template <typename ConstantStringHolder, typename Vec>
constexpr auto swizzleDo(ConstantStringHolder holder, Vec vec) -> auto {
    constexpr auto string = holder();
    constexpr size_t elementCount = getElementCount<Vec>();
    static_assert(isValidSwizzleString(holder) == 1u, "invalid swizzle string");
    static_assert(getLargestSwizzleIndex(holder) < getElementCount<Vec>(), "swizzle characters out of range of given vec");

    constexpr size_t swizzleCount = string.size();

    if constexpr (swizzleCount == 1)
        return Vec1{
            getElement<vecCharToIndex(string[0])>(vec),
        };
    else if constexpr (swizzleCount == 2)
        return Vec2{
            getElement<vecCharToIndex(string[0])>(vec),
            getElement<vecCharToIndex(string[1])>(vec),
        };
    else if constexpr (swizzleCount == 3)
        return Vec3{
            getElement<vecCharToIndex(string[0])>(vec),
            getElement<vecCharToIndex(string[1])>(vec),
            getElement<vecCharToIndex(string[2])>(vec),
        };
    else if constexpr (swizzleCount == 4)
        return Vec4{
            getElement<vecCharToIndex(string[0])>(vec),
            getElement<vecCharToIndex(string[1])>(vec),
            getElement<vecCharToIndex(string[2])>(vec),
            getElement<vecCharToIndex(string[3])>(vec),
        };
    else
    {
        std::terminate();
    }
}

class str_const {
    const char* const p_;
    const std::size_t sz_;
public:
    template <std::size_t N>
    constexpr str_const(const char(&a)[N]) : p_(a), sz_(N-1) {}
    constexpr char operator[](const std::size_t& n) const {
        return n < sz_ ? p_[n] : throw std::out_of_range("");
    }
    constexpr std::size_t size() const {
        return sz_;
    }
};

template<typename T>
template<typename Holder>
constexpr auto swizzle_trait<T>::swizzle(Holder holder) {
    return swizzleDo(holder, *parent);
}

#define flatten(x, y) x ## y
#define swizzle(str) swizzle([]() constexpr -> str_const { return #str; })

volatile float x = 10.0f;
volatile float y = 20.0f;
volatile float z = 30.0f;
volatile float w = 40.0f;

int main() {

  Vec3 myVec3{x, y, z};

  Vec4 myVec4 = myVec3.swizzle(xyyz);

  x = myVec4.x;
  y = myVec4.y;
  z = myVec4.z;
  w = myVec4.w;
}
