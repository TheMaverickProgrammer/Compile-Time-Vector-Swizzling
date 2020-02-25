# Compile Time Vector Swizzling In C++
By using C++17's constexpr if-statements provide GLSL vector swizzling in compile time for Clang, MSVC, and GNU compilers

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

This looks much nicer and one macro might be a small price to pay. If you're against using macros, just pass a string:

`Vec4 myVec4 = myVec3.swizzle("xyyz");`
