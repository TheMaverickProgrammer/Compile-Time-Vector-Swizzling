# Compile Time Vector Swizzling In C++
By using C++17's constexpr if-statements and exploiting the string literal operator, provide GLSL vector swizzling in compile time for Clang and GNU compilers

[Try it on godbolt!](https://godbolt.org/z/ecxidZ)

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

The end result is so:

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

# Notes
The `auto` return type for the literal string operator is a nice feature to have and enables us to return exact types, however is available as a GNU extension. If on Clang, use `-Wgnu-string-literal-operator-template`

Toggle MSVC and see that the microsoft compiler diverges from the others regarding this behavior and seems to prevent this type of exploitation by restricting the input for the operator as the function argument list only. 
