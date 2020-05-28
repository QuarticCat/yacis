# YACIS

**CUHK(SZ) CSC3002 group project - compiler part**

YACIS is short for "Yet Another C++ Implementation for STLC (Simply Typed Lambda Calculus)".

## Build

### Requirements

- CMake 3.12 or higher
- One of:
  - Clang 5 or higher
  - GCC 7 or higher
  - MSVC 2017 or higher
- C++ 17 (set `-std=c++17` please)

### Build as An Executable

```
$ cd yacis
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release -DAS_EXECUTABLE=ON
$ cmake --build .
```

Now you have an executable file named "yacis".

Let's have a test:

```
$ ./yacis ../examples/a_plus_b.yac
```

You will see the MIPS assembly output.

There are two ways to use the executable file:

```
$ ./yacis <path-to-input-file>
```

This will directly print the result.

```
$ ./yacis <path-to-input-file> <path-to-output-file>
```

This will write the result to file.

### Build as A Library

In your `CMakeLists.txt`:

```
add_subdirectory(path-to/yacis)
target_link_libraries(your-target yacis)
```

Then you can:

```cpp
#include <yacis/yacis.hpp>
```

For details of APIs, please see `yacis/include/yacis/yacis.hpp`.

## YACIS Language

### Comments

Comments are start with `--`. For example:

```
-- This is a comment.
```

### Variables

All variables should start with a lower case letter or `_`.

You can assign a value to a variable like this:

```
a = -1
b = '\n'
c = False
```

All variables are immutable, i.e. you can not assign values to them twice.

In YACIS, functions are first-class. They are treated as the same as "normal" variables. So the definition of function is just the assignment of lambda expression:

```
func = \n:Int -> if eq n 0 then 1 else n
```

You can also assign a type to a variable:

```
a : Int
a = -1
func : Int -> Int
func = \n:Int -> if eq n 0 then 1 else n
```

But this is not necessary. If the type is assigned, the compiler will check that. If not, the compiler will infer the type. Only recursive functions need to be assigned a type before being assigned a value.

### Types

There are three basic types:

- `Int` (signed, 32 bit)
- `Char`
- `Bool` (`True` or `False`)

The function types are combinations of them, like this:

```
Int -> Int -> Bool
```

This function accepts two `Int` arguments and then returns a `Bool` value.

Let's see something more complex:

```
Int -> (Char -> Int) -> Bool
```

This function accepts a `Int` argument and a `Char -> Int` argument and then returns a `Bool` value. Don't forget that functions are first-class.

YACIS supports currying. That means these types are equal:

```
Int -> Char -> Bool -> Bool
Int -> (Char -> Bool -> Bool)
Int -> Char -> (Bool -> Bool)
Int -> (Char -> (Bool -> Bool))
```

And you can do this:

```
addTwo = add 2
n = addTwo 3  -- n is 5
```

You can also make type aliases:

```
data ICIB = Int -> (Char -> Int) -> Bool
data Balabala = Int -> Char -> Bool -> Bool
```

All type names should start with a upper case letter.

### Application Expressions

Application expressions are to apply a value to a function. It is very simple:

```
<expression> <expression>
```

`a b` means apply `b` to `a`.

Note that this is left-combined. So these expressions are the same (don't forget currying):

```
a b c d
(a b c) d
((a b) c) d
```

You can use brackets to control the evaluation order:

```
neq (add 2 (mod 5 2)) 4  -- the final value is True
```

### Conditional Expressions

Conditional expressions, or if-expressions, in YACIS are like:

```
if <expression> then <expression> else <expression>
```

The first expression doesn't need to be `Bool` type. For `Int` and `Char`, it is true when the value is not zero. It can not be function type.

The last two expressions should have the same type.

The priority of conditional expressions is very low. So you can write:

```
if eq n 0
    then n1
    else fibHelper (sub n 1) n2 (add n1 n2)
```

```
if neq n 0
    then \x:Int -> 1
    else \x:Int -> 2
```

### Lambda Expressions

Lambda expressions are like:

```
\<arg-name>:<type> <arg-name>:<type> ... -> <expression>
```

Arguments are separated by white space(s).

Note that if the type is function type, you should add a bracket:

```
\ f : (Int -> Int) -> f 1
```

Lambda expressions are right-combined. So these expressions are equal:

```
\a:Int -> \b:Char -> \c:Bool -> 0
\a:Int -> (\b:Char -> \c:Bool -> 0)
\a:Int -> \b:Char -> (\c:Bool -> 0)
\a:Int -> (\b:Char -> (\c:Bool -> 0))
\a:Int b:Char c:Bool -> 0
```

### Inputs and Outputs

To keep purely functional, YACIS doesn't have any input method. And the outputs are more like "inspect", it can not be combined in expressions.

The outputs are just expressions that are not assigned to any variable:

```
a = -1
b = '\''
a  -- Output "-1"
b  -- Output "'"
neq a 0  -- Output "True"
```

### Built-In Functions

(Just to illustrate the effect. YACIS doesn't support these operators)

```
negate : Int -> Int
negate = \a:Int -> -a

add : Int -> Int -> Int
add = \a:Int b:int -> a + b

sub : Int -> Int -> Int
sub = \a:Int b:int -> a - b

mul : Int -> Int -> Int
mul = \a:Int b:int -> a * b

div : Int -> Int -> Int
div = \a:Int b:int -> a / b

mod : Int -> Int -> Int
mod = \a:Int b:int -> a % b

eq : Int -> Int -> Bool
eq = \a:Int b:int -> a == b

neq : Int -> Int -> Bool
neq = \a:Int b:int -> a != b

lt : Int -> Int -> Bool
lt = \a:Int b:int -> a < b

gt : Int -> Int -> Bool
gt = \a:Int b:int -> a > b

leq : Int -> Int -> Bool
leq = \a:Int b:int -> a <= b

geq : Int -> Int -> Bool
geq = \a:Int b:int -> a >= b

and : Bool -> Bool -> Bool
and = \a:Bool b:Bool -> a && b

or : Bool -> Bool -> Bool
or = \a:Bool b:Bool -> a || b

not : Bool -> Bool
not = \a:Bool -> !a
```

### Examples

See `yacis/examples/` for examples.
