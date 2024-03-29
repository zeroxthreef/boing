# Boing Documentation
A quick reference is located at the top of the boing.h file.

Modules each have their own documentation readmes.


<!-- TOC -->

- [Boing Documentation](#boing-documentation)
	- [Boing Operations](#boing-operations)
	- [Types](#types)
		- [Literals](#literals)
		- [Identifiers](#identifiers)
	- [Syntax](#syntax)
		- [Operation Syntax](#operation-syntax)
		- [Array Syntax](#array-syntax)
		- [Block Syntax](#block-syntax)
		- [Pass Block Syntax](#pass-block-syntax)
		- [Notes](#notes)
	- [Value Evolution](#value-evolution)
		- [Tables](#tables)
		- [Scope Stack](#scope-stack)
		- [Program](#program)
	- [Interpreter Behavior](#interpreter-behavior)
		- [Operations, the single character prefixes, can have implicit or explicit arguments.](#operations-the-single-character-prefixes-can-have-implicit-or-explicit-arguments)
		- [Literals](#literals-1)
		- [Explicit args AND implicit args vs implicit only operations](#explicit-args-and-implicit-args-vs-implicit-only-operations)
		- [Identifiers and variables](#identifiers-and-variables)
		- [Evaluated blocks vs passed blocks](#evaluated-blocks-vs-passed-blocks)
		- [Evaluated vs. passed arguments](#evaluated-vs-passed-arguments)
		- [Custom functions and eval](#custom-functions-and-eval)
		- [Value passing, mutation, and the cascade of previous values](#value-passing-mutation-and-the-cascade-of-previous-values)
	- [Scope](#scope)
		- [Not working example](#not-working-example)
		- [Working example:](#working-example)
	- [Operations](#operations)
		- [print](#print)
			- [Behavior with different parameters](#behavior-with-different-parameters)
		- [stringify](#stringify)
			- [Behavior with different parameters](#behavior-with-different-parameters-1)
		- [console read](#console-read)
			- [Behavior with different parameters](#behavior-with-different-parameters-2)
		- [plus](#plus)
			- [Behavior with different parameters](#behavior-with-different-parameters-3)
		- [minus](#minus)
			- [Behavior with different parameters](#behavior-with-different-parameters-4)
		- [multiply](#multiply)
			- [Behavior with different parameters](#behavior-with-different-parameters-5)
		- [divide](#divide)
			- [Behavior with different parameters](#behavior-with-different-parameters-6)
		- [modulo](#modulo)
			- [Behavior with different parameters](#behavior-with-different-parameters-7)
		- [power](#power)
			- [Behavior with different parameters](#behavior-with-different-parameters-8)
		- [copy](#copy)
			- [Behavior with different parameters](#behavior-with-different-parameters-9)
		- [index](#index)
			- [Behavior with different parameters](#behavior-with-different-parameters-10)
		- [table](#table)
			- [Behavior with different parameters](#behavior-with-different-parameters-11)
		- [sizeof](#sizeof)
			- [Behavior with different parameters](#behavior-with-different-parameters-12)
		- [type](#type)
			- [Behavior with different parameters](#behavior-with-different-parameters-13)
		- [equal](#equal)
			- [Behavior with different parameters](#behavior-with-different-parameters-14)
		- [less than](#less-than)
			- [Behavior with different parameters](#behavior-with-different-parameters-15)
		- [greater than](#greater-than)
			- [Behavior with different parameters](#behavior-with-different-parameters-16)
		- [if](#if)
			- [Behavior with different parameters](#behavior-with-different-parameters-17)
		- [loop](#loop)
			- [Behavior with different parameters](#behavior-with-different-parameters-18)
		- [set (write)](#set-write)
			- [Behavior with different parameters](#behavior-with-different-parameters-19)
		- [eval](#eval)
			- [Behavior with different parameters](#behavior-with-different-parameters-20)
		- [file](#file)
			- [Behavior with different parameters](#behavior-with-different-parameters-21)
		- [logical and](#logical-and)
			- [Behavior with different parameters](#behavior-with-different-parameters-22)
		- [logical or](#logical-or)
			- [Behavior with different parameters](#behavior-with-different-parameters-23)
		- [logical not](#logical-not)
			- [Behavior with different parameters](#behavior-with-different-parameters-24)
		- [increment](#increment)
			- [Behavior with different parameters](#behavior-with-different-parameters-25)
		- [decrement](#decrement)
			- [Behavior with different parameters](#behavior-with-different-parameters-26)
		- [external call](#external-call)
			- [Behavior with different parameters](#behavior-with-different-parameters-27)
		- [hash](#hash)
			- [Behavior with different parameters](#behavior-with-different-parameters-28)
		- [scope stack control](#scope-stack-control)
			- [Behavior with different parameters](#behavior-with-different-parameters-29)
		- [search](#search)
			- [Behavior with different parameters](#behavior-with-different-parameters-30)
		- [random](#random)
			- [Behavior with different parameters](#behavior-with-different-parameters-31)
		- [sort](#sort)
			- [Behavior with different parameters](#behavior-with-different-parameters-32)
		- [array setup](#array-setup)
			- [Behavior with different parameters](#behavior-with-different-parameters-33)
		- [numeric wrap](#numeric-wrap)
			- [Behavior with different parameters](#behavior-with-different-parameters-34)
	- [Operation Quick Reference](#operation-quick-reference)
	- [Default Interpreter Identifiers](#default-interpreter-identifiers)

<!-- /TOC -->

## Boing Operations
A boing operation is a single character (ASCII) that has a certain number of implicit arguments. These implicit argument restrictions can easily be overridden with parentheses `(`, `)` directly after operations that allow explicit arguments. Note that some operations **do not allow** explicit arguments so parentheses will just eval the expression but pass as arg0 instead of all arguments. An example is the ``f``, or 'if' operation.

Example:
```
# lets print a hello world

# implicit argument version
p"Hello, World!"

# parentheses were not necessary because the print operation `p` implicitly absorbs 1 argument. The amount of implicit arguments can be anywhere from 0 to a very, very large number.

# explicit argument version
p("Hello" ", " "World!")

```

## Types
There are only 4 types in boing. Numbers(doubles), arrays(array of value pointers), externals(pointers to C data), and operations(the operation id and args which is an array type). 

Out of these 4 types, we can build the entire program or any other kind of structure. Tables are just arrays of a 2 member array with a key value and value value. Internally, this style is repeated in the "program" and "stack" (scope stack) values.

As you can guess, this has a performance impact on boing, but it does mean that we can do lots of things with just a few types. The entire AST is just a bunch of operations and literals parsed into values then pushed into arguments for other values.

### Literals
Literals are parsed to values that best represent itself as one of the 4 types. String literals turn into an array of numbers with each char as the value. certain characters like `(`, `{`, and `[` turn into operations and `)`, `}`, and `]` end the argument array for each.

Number literals can only exist without a single quote if they are positive whole numbers in base 10. Any other requires surrounding single quotes. This is because numbers would interfere with operation literals.

Example:
```
# each are equivalent
5 '5'

1 '1.0'

3 '+3.0000'

0 '0x0'
```

While the parser doesn't recognize negative number literals, the minus operation can be used to make a number literal negative and behave the same as if it were in single quotes. The same applies for the plus operation. Both of these operations implicitly absorb a single value.

Example:
```
# each are equivalent

-5 '-5'

+5 '+5'
```

As mentioned before, string literals are arrays of numbers.

Example:
```
# the array operation starts and ends with '[' and ']'
# the array operation always runs when the interpreter sees it and it will output an array each time

# each are equivalent

"hello" [ 104 101 108 111 ]
```


Strings behave like normal C strings for the most part. Most string escapes work, but any \(number) escapes will not work aside from \0.

Example:
```
"hello"

"hello\"hello"
```

### Identifiers
Identifiers are uninterrupted strings of upper case A-Z characters and the underscore character `_`. An identifier is only interrupted by any not A-Z_ characters. This includes whitespace and other symbols.

Example:
```
HELLO_VARIABLE

VARIABLE

_______VARIABLE____

V_A_RIABLE___
```

## Syntax
The parser has no lexer and knows what to parse solely from whatever the current character is in the script. There are a few situations where the parser doesnt parse operations as expected. These cases are string literals, number literals (both single quoted and whole numbers), identifiers, arrays, pass operation blocks, and operation blocks.

### Operation Syntax
Operation syntax includes the single character name, and what follows _may_ be parentheses or further values. Parentheses are **only** allowed (or behave as expected) if the operation specifies that it allows explicit arguments. If it doesnt, the parentheses will be parsed as an operation block that will behave like nested expressions do in C.

If the operation allows explicit arguments:
`operation chracter`(`variable number of arguments`)

or

`operation character` `default number of arguments`

If it does not, then the last case is the only one that will work the way you expect.

Example:
`p"hello"` print expects a single implicit argument. The argument is the "hello" string (which turns into a number array).

`p("he" "llo")` print allows explicit arguments so any number of arguments may be passed

### Array Syntax
Arrays follow the format of:
`[` `value1` `value2` ... `]`

Note that you can put _anything_ in an array value. This includes pass blocks.

### Block Syntax
Format:
`(` ... `)`

Operation blocks are what surround the outside of the script file to the interpreter but arent visible. Theyre what pass the previous value along. A new operation block can be started and it will return the last value evaluated.

These behave exactly how you'd expect them to.

The previous value returned from each operation cascades through the list of these and is returned as the value.

Example:
```
p("the value should be 5: " ( +(6 7) 5 )  )

# note that there was a + operation before that would have retirned the sum of 6 and 7, but 5 came last so it is the value returned to the 2nd argument for the print operation
```

### Pass Block Syntax
Format:
`{` ... `}`

Pass blocks are similar to the one before but they do not evaluate the values inside at runtime. Instead, they pass the arguments as an operation block so that it can be evaluated later. Useful for making functions.


### Notes
Because the parser does not care what closes one of the 3 opening operations (`[`, `(`, `{`) close with, doing:

``["hello" "im" "an" "array")``

is valid.


Also, unbalanced closing types can affect how a script is parsed. This will cut your script short:

```
p"hey"

]

p"goodbye"
```

"goodbye is not printed"


Another parser note is that the end of the script counts as a full stop for everything. This means the following is valid:

```
p"hello
```


It is possible to interrupt implicit argument parsing with one of the closing characters as well. There are very few use cases where this does anything useful, but it does do something.

```
# the 'u' operation (array setup for when you dont want to write brackets) expects 2 implicit arguments.
# The following are equivalent

u(5) u5)

u(5) u5]

u(5) u5}
```

## Value Evolution
In order to make something more complex, we have to use lots of arrays and values. Consider tables (associative arrays). They're just an array with a row array inside and 2 other values for the key and value.


### Tables
Simply an array containing anothe rarray that contains a key and value. It's easy to make one out of literals.

Example:
```
[
	["key1" "hello"]
	["key2" "world"]
	[5 "!"]
	["key4" 100]
	["key5" ["hello, world"] ]
]
```

### Scope Stack
Because boing is dynamically scoped, each time a new level is added, it is pushed and the last scope array is pointed to by the element at position 0. If the element at position 0 is a number, it is considered the root. Boing doesnt expose the scope stack to scripts, but it does provide a way to get a pointer to any level (or even a brand new scope stack for sandboxing) for use with the eval operation or external call operation.

The value at position 1 is a table of local identifiers and their values.

Format: (for the root. It is complex to represent for higher levels)
``[0 [["VARIABLE_A" "helo world"]]]``

### Program
This is only used internally in the interpreter, but it is described in the boing_t struct in boing.h.

## Interpreter Behavior
There are 8 important details about boing that make reading and writing scripts easier:


### Operations, the single character prefixes, can have implicit or explicit arguments.
Example:
```
# the print operation expects 1 implicit argument, but by using parentheses we can make it take more

p"hello"               # prints hello

p("hello " "world")    # prints hello world
```

### Literals
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

### Explicit args AND implicit args vs implicit only operations
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

### Identifiers and variables
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

### Evaluated blocks vs passed blocks
Example:
```
# the 2 types of operation blocks are () and {}. ()'s are sometimes seen as operation argument explicit openers. This may be a problem if the intention is, for example, to do: "+( (+( 3 3 )) 5 )" but "+ ( +( 3 3 ) ) 5" is written, the 5 will be dropped as the ()'s were parsed as the explicit argument open.

p("this should print the sum: " (+(5 6 7 8 9)) )

p("this should print nothing: " {+(5 6 7 8 9)} )
```

### Evaluated vs. passed arguments
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

### Custom functions and eval
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

### Value passing, mutation, and the cascade of previous values
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

## Scope
Boing is dynamically scoped. If recursion doesn't seem to be working, this is because identifiers are created just as theyre seen for the first time. If it didn't exist before, the identifier will be placed in the current scope stack level. If it did, the last one will be overwritten.

The solution is to pass the root scope (or any scope higher than the one you're sure is interfering) over and over so it gets a new identifier each time.


### Not working example
```
wRECURSE
{
	wVARIABLE i(ARGS 0)

	0f(!= i(ARGS 0) 5)
	{
		eRECURSE[+(i(ARGS 0)  1)]
	}

	pVARIABLE
}

eRECURSE[0]
```

It just prints 4 over and over


### Working example:
```
wRECURSE
{
	wVARIABLE i(ARGS 0)

	0f(!= i(ARGS 0) 5)
	{
		e(RECURSE[+(i(ARGS 0)  1)] k0)
	}

	pVARIABLE
}

e(RECURSE [0] k0) # 'k' is the stack operator. 0 gets the root stack and passes it
```

Now, it prints 4 3 2 1 0

## Operations

### print
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `p` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior And Description |
| --- | --- |
| (...) | Focus toward strings. If passed a number, it will turn it into a string (not typecasted. It is passed as a char). If an array of numbers passed, it won't follow stringify and convert to printable syntax. It prints a newline at the end. |


### stringify
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `s` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (anything) | By default, it will output a string thats more readable than the minified option. |
| (anything, number(1/0)) | Still outputs a string but if the 2nd argument is nonzero, the output will be minified |


### console read
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `r` | yes | 0 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (void) | Returns a single character from the console input |
| (number) | If 0, it will read until EOF of input stream |
| (array) | It expects the array to be a string of a single character. This character will determine when the input stops reading and outputs an array |


### plus
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `+` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (number) | Force the number positive |
| (array) | Sum all array numbers |
| (number, number, ...) | Sum arguments and output the result as a number |
| (array, array, ...) | Concatenate arrays together and output the result |



### minus
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `-` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (number) | Force the number negative |
| (array) | Return the difference of all array numbers |
| (number, number, ...) | Subtract all arguments |
| (array, array, ...) | Remove all matches from arg0 |


### multiply
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `*` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (number) | Square the number |
| (array) | Returns the product of all array numbers |
| (number, number, ...) | Returns the product of all arguments |
| (array, array, ...) | Combines all array members into one continuous array |


### divide
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `/` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (number) | Square root of number |
| (array) | Return the quotient of all array numbers |
| (number, number, ...) | Return the quotient of all arguments |
| (array, array, ...) | Split array at arg0 by the following arguments and return all parts in an array |


### modulo
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `%` | yes | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (number, number) | Returns modulo of arg0 by arg1 |


### power
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `^` | yes | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (number, number) | Returns arg0 to the power of arg1 |


### copy
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `c` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (any) | Recursively copy all values that may somehow be stored inside |


### index
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `i` | yes | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (array, number) | Returns the element at the position of arg1 |
| (operation, number) | Returns the argument at the operation in arg0 |
| (array, number, number) | Returns the range between the arg0 array. Returns as an array, and if either arg1 or ar2 is -1, it will snap to the beginning or end |
| (operation, number, number) | Returns the range of operation arguments. -1 in either arg1 and/or arg2 will snap to beginning or end |


### table
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `t` | yes | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (array, any) | First, the array **must**, if not empty, have rows of exatly 2 elements wide. The row with an element at position 0 will act as the key, and if arg1 matches, the value will be returned. If not found, numeric 0 will be returned |
| (array, any, any) | Same as before with table array rows, but if the key exists, the existing one will be set to the value of arg2. If one does not exist, a new row will be created with the value of arg2 |

NOTE: rows are deleted from tables by setting the value to numeric 0. This is fine for setting it to 0 regardless because tables return 0 for not found rows which is the same as it being set to 0. Its generally best to think of tables as an infinite table of every kind of key possible set to 0.

If this is undesirable, wrap values in arrays like:
```
wTABLE []

t(TABLE "key" ["value"])

# this way, its easy to set it to 0 and still "exist"
t(TABLE "key" [0])


# just index by 0 to get the real value
p("value: " i(tTABLE"key"  0))
```


### sizeof
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `z` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (any) | Returns the length attribute of the value. Really only useful for arrays |


### type
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `y` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (any) | Returns a number equal to the global identifier of the same name (NUMBER, ARRAY, OPERATION, EXTERNAL) |
| (any, number) | Returns arg0 typecasted to the type passed in arg1. Recommended to use the global identifiers mentioned before. <br>If a number is passed in arg0, casting it to an ARRAY will stringify the number. <br>If a number is passed in arg0, casting it to an OPERATION will return an operation with the character set to the number. <br>If a number is passed in arg0, casting it to EXTERNAL will fail <br><br>If an array is passed in arg0, casting it to a NUMBER will expect a string and turn that string into a number <br>If an array is passed in arg0, casting it to an OPERATION will expect a number for the operation character as a number in the first position, and an array in the next position for the arguments <br>If an array is passed in arg0, casting it to EXTERNAL will fail <br><br>If an operation is passed in arg0, casting it to a number will return a number equal to the character the operation represents <br>If an operation is passed in arg0, casting it to an array will return an array with a number the operation represents as a char and the args as an array in the second position <br>If an external value is passed in arg0, it will fail regardless |


### equal
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `=` | yes | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (any, any) | Recursively test if both arguments are exactly equal. Returns a numeric 1 if equal and a numeric 0 if not |
| (any, any, ...) | Recursively test all arguments and return numeric 1 if all the same. Numeric 0 is returned if not |


### less than
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `<` | yes | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (any, any) | Recursively test if arg0 is less than arg1. Returns numeric 1 if true and a numeric 0 if not |


### greater than
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `>` | yes | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (any, any) | Recursively test if arg0 is greater than arg1. Returns numeric 1 if true and a numeric 0 if not |


### if
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `f` | no | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (any, any) | If arg0 is _anything but_ a numeric 0, it will evaluate arg1 and return numeric 1. If not, it will return a numeric 0 and not eval arg1 |


### loop
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `l` | no | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (any, any) | If arg0 is _anything but_ a numeric 0, it will evaluate arg1 and continue testing arg0 forever until a numeric 0 is tested. If never run, it will return a numeric 0. If it ran any number of times, the number of times run will be returned |


### set (write)
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `w` | yes | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (any, any) | Set arg0 to arg1. Note that this does not mean arg0 becomes arg1. It does not reside in the same memmory location and any changes made to either values will not be reflected in the opposite. However, because arrays, operations, and external values contain pointers to other things, those will be shared between both values and changes to anything inside will affect the other's elements. If this is not desired, use the copy operation |


### eval
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `e` | yes | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (any, any) | Returns arg0 evaluated and passed with the arguments ('_' and ARGS) that arg0 is insterted into. It wont be insterted into a new argument array if arg1 is an array itself, which in that case, will become the argument array itself |
| (any, any, external(stack)) | Returns arg0 evaluated and passed with the arguments ('_' and ARGS) that arg0 is insterted into. It wont be insterted into a new argument array if arg1 is an array itself, which in that case, will become the argument array itself. Arg2 is expected to be an external value that is returned from the stack operator `k` |
| (any, any, external(stack), number) | Returns arg0 evaluated and passed with the arguments ('_' and ARGS) that arg0 is insterted into. It wont be insterted into a new argument array if arg1 is an array itself, which in that case, will become the argument array itself. Arg2 is expected to be an external value that is returned from the stack operator `k`. If the number at arg3 is nonzero, a new stack will be pushed to the stack passed in arg2. This stack will be popped after eval. If arg3 is 0, arg1 is evaluated in the stack passed in arg2 |


### file
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `o` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (array) | Return an array with the bytes of a file read from the path string provided. If the file does not exist, a numeric 0 will be returned |
| (array, any) | As long as the file exists, arg1 will be stringified and the file contents will be overwritten to arg1. A successful write returns a numeric 1 |
| (array, number, number) | Return an array with the bytes of a file read from the range of arg1 to arg2. If not found, a 0 will be returned |



### logical and
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `&` | yes | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (any, any) | Returns numeric 1 if both arguments are _anything but_ numeric 0. Returns numeric 0 if not |
| (any, any, ...) | Returns numeric 1 if all arguments are _anything but_ numeric 0. Returns numeric 0 if not |


### logical or
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `\|` | yes | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (any, any) | Returns numeric 1 if any arguments are _anything but_ numeric 0. Returns numeric 0 if not |
| (any, any, ...) | Returns numeric 1 if any of all of the arguments are _anything but_ numeric 0. Returns numeric 0 if not |


### logical not
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `!` | no | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (any) | Returns numeric 1 if arg0 is _anything but_ numeric 0. Returns numeric 0 otherwise |


### increment
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `n` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (number) | arg0 will be incremented by exactly 1.0 and set to that value. Returns the result as well |
| (number, number) | arg0 will be incremented by the number in arg1 and set to that value. Returns the result as well |


### decrement
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `d` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (number) | arg0 will be decremented by exactly 1.0 and set to that value. Returns the result as well |
| (number, number) | arg0 will be decremented by the number in arg1 and set to that value. Returns the result as well |


### external call
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `x` | yes | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (external, any) | If arg1 is anything but an array, internally the value will be put into a new args array. If arg1 is an array, it becomes the args array |
| (external, any, external(stack)) | If arg1 is anything but an array, internally the value will be put into a new args array. If arg1 is an array, it becomes the args array. Arg2 is expected to be an external value that is returned from the stack operator `k` |
| (external, any, external(stack), number) | If arg1 is anything but an array, internally the value will be put into a new args array. If arg1 is an array, it becomes the args array. Arg2 is expected to be an external value that is returned from the stack operator `k`. If the number at arg3 is nonzero, a new stack will be pushed to the stack passed in arg2 |


### hash
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `h` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (any) | Returns the numeric value of the value recursively hashed |


### scope stack control
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `k` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (number) | Returns an external value of the scope stack selected. If a numeric value of 0 is passed, it returns the root scope stack of the stack visible to this operation when evaluated. If -1, a whole new scope stack will be initialized and passed. Useful for sandboxing. If arg1 >0, the scope level visible to the operation n levels up will be returned |


### search
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `a` | yes | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (array, any) | Returns the position of the array arg1 matches. Note that this has a focus toward strings so encasing a number in an array or passing a number in arg1 have the behave the same. If the desire is to search exactly for a numbr in an array, surround arg1 in another array |


### random
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `g` | yes | 0 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (void) | Returns a numeric random number |
| (number) | Returns a numeric random number limited to any positive or negative number |
| (number, number) | Returns a numeric random number in the range from arg0 to arg1 |


### sort
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `q` | yes | 1 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (array) | Returns an array of the elements in arg0 but in ascending sorted order |
| (any, any, ...) | Returns an array of the arguments but in ascending sorted order |


### array setup
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `u` | yes | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (number) | Returns an array initialized to 0 of arg0 elements |
| (number, any) | Returns an array initialized to whatever arg1 is evaluated to/returns. Arg1 is evaluated like the eval operation and '_'/'ARGS' are passed containing the current index at position 0 which is given a new stack pushed from the current stack. |

### numeric wrap
| Character | Explicit Arguments Allowed | Implicit Argument Count |
| --- | --- | --- |
| `j` | no (however, ranged wrap is a consideration for the future) | 2 |

#### Behavior with different parameters
| Arguments | Behavior |
| --- | --- |
| (number, number) | Returns a wrapped value around `[0, arg1)` or, if negative `(arg1, 0]` |

## Operation Quick Reference
| Character | Explicit Arguments Allowed | Implicit Argument Count | Notes |
| --- | --- | --- | --- |
| `p` | yes | 1 | print |
| `s` | yes | 1 | stringify |
| `r` | yes | 0 | console read |
| `+` | yes | 1 | addition/concat/positive |
| `-` | yes | 1 | subtraction/remove/negative |
| `*` | yes | 1 | multiplication/combination/square |
| `/` | yes | 1 | division/split/sqrt |
| `%` | yes | 2 | modulo |
| `^` | yes | 2 | power |
| `c` | yes | 1 | copy |
| `i` | yes | 2 | index |
| `t` | yes | 2 | table get/set |
| `z` | yes | 1 | sizeof |
| `y` | yes | 1 | type/typecast |
| `=` | yes | 2 | equality test |
| `<` | yes | 2 | less than |
| `>` | yes | 2 | greater than |
| `f` | no | 2 | if |
| `l` | no | 2 | loop |
| `w` | yes | 2 | write(set) |
| `e` | yes | 2 | eval |
| `m` | yes | 1 | import/parse string |
| `o` | yes | 1 | file read/file write |
| `&` | yes | 2 | logical and |
| `\|` | yes | 2 | logical or |
| `!` | no | 1 | logical not |
| `n` | yes | 1 | increment by 1/increment by any |
| `d` | yes | 1 | decrement by 1/decrement by any |
| `x` | yes | 2 | external call |
| `h` | yes | 1 | recursive hash |
| `k` | yes | 1 | scope stack control |
| `a` | yes | 2 | find |
| `g` | yes | 0 | generate(rand)/random to max/random within range |
| `q` | yes | 1 | quicksort array/quicksort all arguments |
| `u` | yes | 2 | array setup X number of elements to 0/array setup to X number of elements to X |
| `j` | no | 2 | wrap number from `[0 arg1)` or `(arg1 0]` |


## Default Interpreter Identifiers
| Identifier | Value | Notes |
| --- | --- | --- |
| `BOING_VERSION` | (array)BOING_VERSION_STRING in boing.h | Contains the major, minor, and revision as a string. Also includes the comilation date. |
| `BOING_VERSION_MAJOR` | (number)BOING_VERSION_MAJOR in boing.h | The major number |
| `BOING_VERSION_MINOR` | (number)BOING_VERSION_MINOR in boing.h | The minor number |
| `BOING_VERSION_REVISION` | (number)BOING_VERSION_REVISION in boing.h | The revision number |
| `ARRAY` | (number)BOING_TYPE_VALUE_ARRAY in boing.h | Used for comparing types and casting with the type 'y' operation |
| `NUMBER` | (number)BOING_TYPE_VALUE_NUMBER in boing.h | Used for comparing types and casting with the type 'y' operation |
| `OPERATION` | (number)BOING_TYPE_VALUE_OPERATION in boing.h | Used for comparing types and casting with the type 'y' operation |
| `EXTERNAL` | (number)BOING_TYPE_VALUE_EXTERNAL in boing.h | Used for comparing types and casting with the type 'y' operation |
| `TRUE` | (number)1.0 |  |
| `FALSE` | (number)0.0 |  |
| `M_PI` | (number)3.14159265 |  |
| `NULL` | (external) pointer set to NULL |  |
| `BOING_POOL_STRING_SIZE_GET` | (external)C function that returns | No arguments necessary/ignores all arguments |
| `BOING_POOL_STRING_SIZE_SET` | (external)C function that returns | Expects a numeric argument |
| `BOING_POOL_STRING_AMOUNT_GET` | (external)C function that returns | No arguments necessary/ignores all arguments |
| `BOING_POOL_STRING_FREE_GET` | (external)C function that returns | No arguments necessary/ignores all arguments |
| `BOING_POOL_STRING_MAX_FREE_GET` | (external)C function that returns | No arguments necessary/ignores all arguments |
| `BOING_POOL_STRING_MAX_FREE_SET` | (external)C function that returns | Expects a numeric argument |
| `BOING_POOL_ARRAY_SIZE_GET` | (external)C function that returns | No arguments necessary/ignores all arguments |
| `BOING_POOL_ARRAY_SIZE_SET` | (external)C function that returns | Expects a numeric argument |
| `BOING_POOL_ARRAY_AMOUNT_GET` | (external)C function that returns | No arguments necessary/ignores all arguments |
| `BOING_POOL_ARRAY_FREE_GET` | (external)C function that returns | No arguments necessary/ignores all arguments |
| `BOING_POOL_ARRAY_MAX_FREE_GET` | (external)C function that returns | No arguments necessary/ignores all arguments |
| `BOING_POOL_ARRAY_MAX_FREE_SET` | (external)C function that returns | No arguments necessary/ignores all arguments |
| `BOING_POOL_VALUE_SIZE_GET` | (external)C function that returns | No arguments necessary/ignores all arguments |
| `BOING_POOL_VALUE_SIZE_SET` | (external)C function that returns | Expects a numeric argument |
| `BOING_POOL_VALUE_AMOUNT_GET` | (external)C function that returns | No arguments necessary/ignores all arguments |
| `BOING_POOL_VALUE_FREE_GET` | (external)C function that returns | No arguments necessary/ignores all arguments |
| `BOING_POOL_VALUE_MAX_FREE_GET` | (external)C function that returns | No arguments necessary/ignores all arguments |
| `BOING_POOL_VALUE_MAX_FREE_SET` | (external)C function that returns | Expects a numeric argument |
| `SIN` | (external)C function that returns | Expects a numeric argument |
| `ASIN` | (external)C function that returns | Expects a numeric argument |
| `SINH` | (external)C function that returns | Expects a numeric argument |
| `COS` | (external)C function that returns | Expects a numeric argument |
| `ACOS` | (external)C function that returns | Expects a numeric argument |
| `COSH` | (external)C function that returns | Expects a numeric argument |
| `TAN` | (external)C function that returns | Expects a numeric argument |
| `ATAN` | (external)C function that returns | Expects a numeric argument |
| `TANH` | (external)C function that returns |Expects a numeric argument  |
| `EXP` | (external)C function that returns | Expects a numeric argument |
| `LOG` | (external)C function that returns | Expects a numeric argument |
| `LOGTEN` | (external)C function that returns | Expects a numeric argument |
| `CEIL` | (external)C function that returns | Expects a numeric argument |
| `ABS` | (external)C function that returns | Expects a numeric argument |
| `FLOOR` | (external)C function that returns | Expects a numeric argument |
| `ATANTWO` | (external)C function that returns | Expects two numeric arguments |
| `RELATIVE` | (external)C function that returns | Expects a single array argument (NOTE: will only work as expected if the array came from the parser (string literal). If it was created at runtime, the interpreter working directory will be used instead) |