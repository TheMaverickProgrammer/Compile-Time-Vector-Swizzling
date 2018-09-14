# Compile Time Vector Swizzling In C++
Using latest C++ language features, provide GLSL vector swizzling in compile time

# Swizzling in GLSL
GLSL offers 'swizzling' on vector types. E.G.

```
vec3 myVec(24, 34, 5);

vec4 otherVec = myVec.xxyz; // Returns (24, 24, 34, 5)
```

This allows vectors to be dynamically made from other vector sizes from a supplied order of variables in a row. 

One solution that has been done many time is overloading every possible combination of (x,y,z,w) pairs. This is a long and tedious practice.
The other solution is to overload the `[]` operator and supply strings such as `["xxyz"]` to return a larger vector that can be reduced down to the target vector size.

This solution is 1) Not compile-time and 2) Does not prevent mismatched vector types, it encourages it.

# Swizzling in C++
Using varadaic templates and constexpr we can reduce the string used to compile time and determine if the correct properties are supplied.
Using the literal operator we can return a function that takes in a constant string and returns any vector size from the input.

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
