# Boing
Boing is an esoteric language with an easily embeddable interpreter in C89*. The core is just a single header (boing.h) that can be placed into any file.

The core itself has no dependencies on anything but the C standard library.

Boing has garbage collection (automatic reference counting), but it's very easy to make a circular reference and there is no circular reference detection yet. This will cause a pool leak if care isn't taken to avoid this.


Note: Nothing will cause a true leak (except for pool corrupting errors) because the various pools keep track of currently in use and free things. As long as the pools are cleaned up, there will be no real leaks.

Note#2: Boing has not reached version 1.0 yet. Lots will change before then. Havent finished setting up all of the readmes and documentation yet.


\*It is mostly C89, but the few C99 features it does use (like stdint.h and vsnprintf) are easily changed with a few #defines like BOING_VSNPRINTF and defining uint8_t, uint32_t as something else.

## Building
If you don't want to build certain modules, there are options for each to be turned on and off. All modules are built by default and the os module is statically linked by default.

Linux & Unixes:
---
```
git clone TODO PUT URL
cd Boing
mkdir build && cd build && cmake .. && make
```

Windows:
---
```
git clone TODO PUT URL
cd Boing
mkdir build
cd build
cmake ..
TODO FINISH WINDOWS BUILD INSTRS
```

## Installation
Linux & Unixes: (prefix with sudo or run as root)
---
```
make install
```

Windows:
---
```
TODO FINISH WINDOWS INSTALL INSTRS
```

## Full Documentation
The full documentation of operations, types, and various other globals and information is detailed at ``TODO INSERT LINK TO DOC MARKDOWN``

## Syntax and Usage
There are 8 important details about boing that make reading and writing scripts easier:


Operations, the single character prefixes, can have implicit or explicit arguments.
---
Example:
```
# the print operation expects 1 implicit argument, but by using parentheses we can make it take more

p"hello"               # prints hello

p("hello " "world")    # prints hello world
```

Literals
---
Example:
```
# strings (technically number arrays)
"hello"

# numbers (decimals MUST be placed inside ' ' )
5       # equal to 5.0
7498    # equal to 7498.0
'-400'  # equal to -400.0
'5.700' # equal to 5.700
'0x3F'  # equal to 63

# arrays
[1 2 3 4 5]

["test" ["nested" "array" "allowed"] 123]
```

Explicit args AND implicit args vs implicit only operations
---
Example:
```
# operations like l and f (loop and if) take only implicit arguments. In the instance of f, providing any paretheses will just add to readability but whatever comes after will still be evaluated

f=55
{
	p"hello"
}

# same as:

f(= 5 5)
{
	p"hello"
}
```

Identifiers and variables
---
Example
```
# identifiers can only be a single, uninterrupted, string of A-Z _ characters.

# possible identifiers:
TEST
I
TEST_VALUE
____A
A_B_C_D_E_

# impossible identifiers
hello_test
TeST_IdenTIFIER
test6


# variables exist as soon as an identifier for it has been interpreted. This means the following are equivalent

TEST_VARIABLE #initialized to 0 by default

w(TEST_VARIABLE 0) #the write/set operation, 'w'

wTEST_VARIABLE0
```

Evaluated blocks vs passed blocks
---
Example:
```
# the 2 types of operation blocks are () and {}. ()'s are sometimes seen as operation argument explicit openers. This may be a problem if the intention is, for example, to do: "+( (+( 3 3 )) 5 )" but "+ ( +( 3 3 ) ) 5" is written, the 5 will be dropped as the ()'s were parsed as the explicit argument open.

p("this should print the sum: " (+(5 6 7 8 9)) )

p("this should print nothing: " {+(5 6 7 8 9)} )
```

Evaluated vs. passed arguments
---
Example:
```
# using the l operation example from before, it might seem inconsistent that argument 1, the test which is executed for every loop, is run without being enclosed in {}. Operations can choose to have their arguments evaluated or passed, allowing manual control of argument evaluation. This is the case for l.

I            #make I variable and set it to 0
l(< I 20)
{
	p"Hello, World!"
	w(I +(I 1)) #TODO: replace this with the increment op
}

# equal to: Note that I wasn't declared before, but it's fine because it gets created as soon as it appears
l<I20{p"Hello, World!"wI+(I1)}
```

Custom functions and eval
---
To do anything with passed arguments, {...}, the 'e' (eval) operation needs to be used. Note that the eval operation takes 2 implicit arguments, but more than that passed explicitly allow for more functionality like resetting the scope stack so that it doesn't interfere with earlier variables.

Example:
```
# plain eval
e{
	+(4 4 4)
}[]

# returns '8'



# making a function

wTEST_FUNC
{
	+(4 4 4)
}

eTEST_FUNC[]

# also returns '8'


# retrieving arguments

wTEST_FUNC_ARGS
{
	# note that "ARGS" and "_" are variables created by _any_ eval operation. This includes arguments passed to the boing interpreter.
	
	# Important: the interpreter usually follows how you'd expect dynamic scoping to behave but ARGS and _ are declared at the current scope stack any time eval is run. It will not overwrite existing ones so calling functions in functions is safe.

	# index the arguments to the position at 0 since they both point to the same array
	p("I was passed " i(ARGS 0) " which is the same as doing " i(_ 0))
}

eTEST_FUNC_ARGS["hello argument"]
eTEST_FUNC_ARGS0 # it is fine to not put the argument in an array if only passing 1 argument, most importantly, not an array type. Its usually a good idea to always wrap your argument list in an array in case it ever becomes a string or array
```

Value passing, mutation, and the cascade of previous values
---
ALL values passed as arguments, stored in tables, etc are passed by reference. It is possibly to pass by "value" if the 'c' (recursive copy) operation is used.

Anytime the interpreter is evaluating a block, it takes note of the previous value. This is not normally something to think about except for when returning from functions, chaining ifs, and in any other case where you want to know the previous value without taking it as an argument. This is why the start of if operations need a 0 in front, as the previous value is used to cascade through if chains so that when one runs it can stop the following from executing.

All values are mutable, but because of the way values are stored internally, changing a value, depending on the type, sometimes will not have the desired effect. The following example illustrates problems when converting between types and when using arays:

```
# trying to change something after writing another, even though things are passed by reference, may not behave the same

# set and copy are different. Set is like a single level copy and the copy operation is recursive. Everything provided to it will be copied.

wTEST_NUM8
wTEST_NUMBER TEST_NUM

wTEST_NUMBER 1
p("original num will == 8 " TEST_NUM)
p("new value, even though set to num, did not modify both " TEST_NUMBER)


#things, however, get strange with strings/arrays. They act like pointers so while the length won't be effected by any set modifications, the members will be shared.

wTEST_STR "hello"
wTEST_STRING TEST_STR

w(iTEST_STRING0) 63

p("used the same set operation, but notice both strings are the same: " TEST_STR) #prints ?ello
p("same, but from different value: " TEST_STRING) #prints ?ello


# it is impossible to change the length of the first string though

wTEST_STRING +(TEST_STRING " world")

p("original is unchanged: " TEST_STR) #prints ?ello
p("new string has new characters added: " TEST_STRING) #prints ?ello world
```

* TODO: finish this section. Its complicated to document

## Embedding The Interpreter
If, for some reason you think it's a good idea to embed this interpreter into something larger, ~~you should reconsider~~ it's as simple as:


```
/* pretend this is the top of a file called embedder.c */
#define BOING_IMPLEMENTATION
#include "boing.h"

/* various includes or code may follow */
```


Configuring parts like the pool system is as easy as, for example:
```
#define BOING_DEFAULT_FREE_STRING 400
#define BOING_IMPLEMENTATION
#include "boing.h"
```

```
TODO: put bare minimun initialization and eval code
```

## Dependencies
The core of the interpreter (boing.h) needs nothing but the C89 (with a few exceptions that can be changed by the user) standard library.

The OS module, depending on your operating system, may depend on certain system libraries.

The screen module depends on SDL2 and SDL2_ttf, which then depends on freetype

## License
This project is licensed under the Unlicense (a more verbose public domain).

Note that if you build and link with any module that links with another library, the libraries linked with that module have their own individual licenses that will apply.

## TODO
\* reduce the recursion in boing_value_reference_dec() because it entirely halts when opening a 100kb+ file and then destroying its array
\* change the boing_error() function to a loglevel version
\* maybe add error catching and throwing. Will require a lot more work on making sure things are cleaned up when error conditions are met
\* more operations to utilize the rest of non-upper case + !'_' ascii
\* various code cleaning and hopefully improvements in documentation