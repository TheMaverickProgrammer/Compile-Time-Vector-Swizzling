# Compile Time Vector Swizzling In C++
By using C++17's constexpr if-statements and exploiting the string literal operator for GCC, provide GLSL vector swizzling in compile time for Clang (with extensions) and GNU compilers

[Example on godbolt!](https://godbolt.org/z/ecxidZ)

[Ex 2: Macros make it look more native](https://godbolt.org/z/z45Ug8)

See #Notes about MSVC

# Swizzling in GLSL
GLSL offers 'swizzling' on vector types. E.G.

```
vec3 myVec(24, 34, 5);

vec4 otherVec = myVec.xxyz; // Returns (24, 24, 34, 5)
```

This allows vectors to be dynamically made from other vector sizes from a supplied order of variables in a row. 

One solution that has been done many time is overloading every possible combination of (x,y,z,w) pairs. This is a long and tedious practice.
The other practiced solution is to overload the `[]` operator and supply strings such as `["xxyz"]` to return a much larger vector dimension that can be reduced down to the target vector size.

This solution... 
1) Is NOT compile-time
2) Does NOT prevent mismatched vector types, it encourages it
3) Because of #2, mismatched types will fail silently, and does not give us meaningful error messages.

# Swizzling in Modern C++
Using variadic templates and constant expressions (constexpr) we can reduce the string used to compile time and determine if the correct properties are supplied.
Using the literal string `""_` operator we can return a function that takes in a constant string and returns any vector size from the input.

The end result is [on godbolt](https://godbolt.org/z/ecxidZ)

```
Vec3 myVec{1, 2, 80};
Vec4 otherVec = "xxyz"_swizzle(myVec);

print(otherVec);
// (1, 1, 2, 80)

Vec2 smallVec = "ww"_swizzle(otherVec);

print(smallVec);
// (80, 80)

Vec2 failVec = "wzzx"_swizzle(otherVec); // error Vec4 returned does not match target type Vec2
```

If we introduce macros 😱 (just for this case!) we can hide away the ugliness through a member function inherited by a base-trait class. [Visit godbolt](https://godbolt.org/z/z45Ug8)

```
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
```

This looks much nicer and might be a small price to pay.

# Notes
The `auto` return type for the literal string operator is a nice feature to have and enables us to return exact types, however is available as a GNU extension. If on Clang, use `-Wgnu-string-literal-operator-template`

Toggle MSVC and see that it does not support this language extension like clang does. We can only wait for C++20's Non-Type Paramter support for strings to get this working across all platforms.
