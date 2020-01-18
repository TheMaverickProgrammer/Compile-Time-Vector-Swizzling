/*
Author: TheMaverickProgrammer
1/18/2020

Try it on godbolt!
https://godbolt.org/z/ecxidZ
*/

#include <iostream>
#include <cstring>
#include <array>

//vec types
struct Vec1 {float x;};
struct Vec2 {float x, y;};
struct Vec3 {float x, y, z;};
struct Vec4 {float x, y, z, w;};

//vec type helper for meta programming: get amount of elements in vec
template <typename T>
constexpr size_t getElementCount()
{
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
constexpr float getElement(TVec v)
{
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
constexpr size_t vecCharToIndex(char c)
{
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
constexpr size_t isValidSwizzleString(ConstantStringHolder holder)
{
    constexpr std::array string = holder();
    constexpr size_t length = std::size(string)-1u;

    //length must be between 1 and 4 swizzle characters
    if(length < 1u || length > 4u)
        return 0u;

    //only xyzw allowed
    for(size_t i = 0; i < length; ++i)
    {
        char current = string[i];
        if(current != 'x' && current != 'y' && current != 'z' && current != 'w')
            return 0u;
    }

    //must end with null terminator
    if(string[length] != '\0')
        return 0u;

    return 1u;
}

//returns the largest swizzle-index of a swizzle string. i.e. "xzxx" will return 2 since x is 0 and z is 2 and z is largest - packaged as constexpr lambda
template <typename ConstantStringHolder>
constexpr size_t getLargestSwizzleIndex(ConstantStringHolder holder)
{
    constexpr std::array string = holder();
    constexpr size_t length = std::size(string)-1u;

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
constexpr auto swizzleDo(ConstantStringHolder holder, Vec vec) -> auto
{
    constexpr std::array string = holder();
    constexpr size_t elementCount = getElementCount<Vec>();
    static_assert(isValidSwizzleString(holder) == 1u, "invalid swizzle string");
    static_assert(getLargestSwizzleIndex(holder) < getElementCount<Vec>(), "swizzle characters out of range of given vec");

    constexpr size_t swizzleCount = std::size(string)-1u;

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

template <typename CharT, CharT...chars>
constexpr auto operator "" _swizzle()
{
    return [](auto vec) constexpr -> auto
    { 
        constexpr auto retfunc = []() constexpr -> std::array<char, sizeof...(chars)+1u> { return {chars..., '\0'}; };
        return swizzleDo(retfunc, vec);
    };
}

volatile float x = 10.0f;
volatile float y = 20.0f;
volatile float z = 30.0f;
volatile float w = 40.0f;

int main() {

  Vec3 myVec3{x, y, z};

  Vec4 myVec4 = "xyyx"_swizzle(myVec3); // string lit. op w/ lambda

  x = myVec4.x;
  y = myVec4.y;
  z = myVec4.z;
  w = myVec4.w;
}
