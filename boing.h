/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/


#ifndef BOING_H__
#define BOING_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <setjmp.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <math.h>

/*
syntax:
if not enclosed in () after an operation, the next operations,
depending on the amount by default it requests, will be passed


example: `operation`(`default number of arguments`)

p"im a literal"
p("multiple ""things, "s'100')

notes:
 - everything is passed by reference. It is possible to copy explicitly
 - 
 - MUST wrap operations not wanted divided into implicit arguments with ()

value evolutions:
- associative arrays (tables):
	[[key][value]...]
- stacks:
	[above_stack_reference identifier_table return_buf]
- programs:
	[ast root_identifier_table]

non-operation syntax:
#    - parser ignores everything until the next \n
""   - string literal. Follows C style escapes.
''   - number literal. Can do 0x prefix, -,+, and decimal.
0-9  - number literal.
A-Z_ - signals to the parser that this is a multi-char identifier
()   - this just evaluates the operations inside immediately and returns. This
     - runs in the same scope as the thing it surrounds.
{}   - prevents the operations inside from being executed. Just returns the AST.
[]   - sets the following expressions to be the values of an array. Examples:
     - 
     - ["h""e""l""l""o"['23''0.23']]
     - 	 
     - Note: operations will be evaluated at runtime, but anything enclosed in
	 - {} list brackets will not. Can be evaluated at any time later
     - 
     - p*["hello,"r('0')]" "
     - 
	 - outputs "hello, whatever was input until newline"
	 - 
	 - ["test"{}]

operations (#_of_implicit_args,explicit&implicit(e) OR implicit ONLY(i)):
p(1e)- print value passed to stdout. Note, arrays are handled as strings
	 - and printed as an array of chars and individial values are numbers. If
     - it is necessary to print a double, use the following operation
s(1e)- turns values into an ascii string array, usually formatted how they would
     - appear as literals. If the second argument is provided, and a 1 or 0, the
	 - output is either minified or a more readable form is created.
r(0e)- returns a value read from stdin, single character if no value passed. 
     - if '0', then it will return everything until an EOF. >0 just blocks
	 - until the input amount reaches the limit passed. If an array is passed,
	 - it is tested as a string and will use the first character to read until
+(1e)- adds values or makes positive. If 1 double, it just makes it positive.
     - If 1 array, all values inside will  be added together and their sum returned.
	 - If 2+ doubles, its normal addition. If its any mix of
     - arrays and doubles, it will just concatenate them all into a single array
-(1e)- subtracts values or makes negative if its 1 double. TBD if 1 array
     - If 2+ doubles its normal subtraction. Will error if
     - an array is passed
*(1e)- multiplies values. 1 arg is array op. If 2+ doubles, its normal multiplication. If its
     - under 2 and an array is passed with an array or double, then, on a single level,
	 - each value in the first array will be concatenated to the next with whatever
	 - follows between. Exampe:
	 - 
	 - p*(["hello""world"]" ")
	 - 
	 - outputs "hello world"
/(1e)- divides values. 1 arg is array op. If 2+ doubles, its normal division. Behaves similar to 
     - multiplication, but tokenizes by the string of arguments after the first
	 - array
%(2e)- modulo of 2+ numbers
^(2e)- power of 2+ numbers
c(1e)- copies the value recursively and returns the copy
i(2e)- indexes arrays. The 1st arg is (only arrays and operations). The 2ng arg, depending on if there
	 - are only 2 total arguments, it just returns the value at the position in the array.
	 - If there are 3 arguments provided, the last 2 arguments are used as a range to return.
	 - The range will return an array of the array values within that range. a -1 in either
	 - arg1 or arg2 will just return a range of values from, if the arg1 is -1, will read till arg2.
	 - The inverse is true if arg3 is -1.
	 - example:
	 -
	 - i[1 2 3 4]2
     - 
	 - output: 3
	 -
	 - # range example:
	 -
	 - i([1 2 3 4 5] 1 3)
	 -
	 - output: [2 3 4]
	 -
	 - # negative argument read till begin/end example:
	 -
	 - i([1 2 3 4 5] 2 -1)
	 -
	 - output: [3 4 5]
t(2e)- searches a table (associative array) for the first instance of the value passed.
     - If 2 arguments passed, it will use the 1st as the source for the associative array
	 - and the 2nd as the cmp. If 3 arguments are passed, it follows the same setup
	 - as if 2 were passed, but it will set (or create) the key by the value compared
	 - in the associative array. Notes below:
	 - 
	 - associative array format (will return '0' if not found (get only) or improper array)
	 - also, _anything_ can be used as the cmp value or a key. It is recursively compared.
	 -
	 - [[key_value data_value]...]
	 -
	 - examples:
	 -
	 - t[["test""hey"]]"test"
	 - returns: "hey"
	 -
	 - t(same_value_as_prev "test")
	 - returns: "hey"
     - 
	 - t([] "test" "hey")
	 - returns: 0 (only because it didnt exist before. If it had, it would return 1)
	 - the array inside the ast, because all values are passed by reference will
	 - remember this the next time it is run.
z(1e)- returns the size of the array.
y(1e)- returns the type as a number (BOING_TYPE_VALUE_...). Can be compared against
     - NUMBER, ARRAY, OPERATION, EXTERNAL. If more than 1 argument passed, arg0 is
	 - used as the castee and arg1 is used as the type to cast it to.
	 - A NOTE ABOUT CONVERSION:
	 -
	 - array -> number: [1 3 4 5 6] -> 23456
	 - this is because the first number is what is used to store the sign of the
	 - original number and also to store the fractional part.

=(2e)- tests for equality between values recursively. 
<(2e)- less than. Tests if arg1 is less than arg2. TBD if more than 2
>(2e)- greater than. Follows same style as the one above.
f(2i)- if statement. Uses the previous argument always and tests the first arg if 
     - nonzero. The 2nd argument, which _has_ to be a block, will get executed if
	 - the test argument is nonzero.
	 - examples:
	 -
	 - f(...){...} #note, the (...) isnt necessary, but its probably a good idea
	 -
	 - 0f(=3'0'){p"impossible"}f(=3'3'){p"this ran"}
	 - minified version: f=3'0{p"impossible"}f=3'3{p"this ran"}
l(2i)- loops. if just 2 arguments, it acts like a while loop in C.
	 - examples:
	 - 
	 - l(=ITER5){... } #or alternatively,
	 - l=ITER5{... }
w(2e)- sets arg1 to arg2 (NOTE: value is _replicated_. Not the same as copying, but
     - it does mean that if it's being set to an array, it will share all array members)
e(2e)- eval. This evaluates arg1. Arg2 is what "_" or "ARGS" is set to.
	 - If more than 2 arguments are passed and there's a 3rd argument, it takes a
	 - stack external type. If a 4th argument is passed, it expects a number. If that
	 - number is nonzero, a new stack will be pushed. Otherwise, the stack passed in arg3
	 - is the stack used.
m(1e)- parse/import. Parses files into an ast that can be evaluated by the above operation
	 - Arg1 is the file to parse and eval. If a second argument is provided, and it contains
	 - a string, it will use arg0 as a string to parse rather than as a filepath. Also, arg1
	 - becomes the "filename". Useful for tracking errors.
	 - Usually used to import othre files like:
	 - 
	 - m"file.bg"
	 -
	 - Parsing a string instead:
	 -
	 - em("p\"hello\"" "testscript")] # evals a string that prints hello. The "]" could be replaced
	 - with a 0 or anything else, but its just for convenience.
o(1e)- file handling. Just a filepath reads the entire file to an array and returns that.
     - If 2 arguments, it is a write to the file and will overwrite the contents of the
	 - file with the stringified contents of the array in arg2. If 3 arguments and it
	 - follows o("path" '0' '-1'), it will read the contents from arg2-arg3. If arg3 is
	 - negative, it will read the entire file.
	 - Examples:
	 - 
	 - po"examples/hello_world.bg" # prints the hello world script to console
	 -
	 - o("file.txt""hello, I am a text file")
	 -
	 - em(o"examples/hello_world.bg" 1)] #a more verbose way of doing just m"whatever.bg"
&(2e)- logical & (&&). Will compare all arguments if == 1. Allows 2 or more args.
|(2e)- logical | (||). Will compare all arguments if == 1. Allows 2 or more args.
!(1i)- logical ! (!). Negates arg0.
n(1i)- increment numeric value. Returns the value incremented. Equivalent to postfix inc.
	 - If 2 arguments supplied, it will increment by that number.
d(1i)- decrement numeric value. Returns the value decremented. Equivalent to postfix dec
	 - If 2 arguments supplied, it will decrement by that number.
x(2e)- external call. This function takes 2(default) arguments, arg0 being the external typed value
	 - (must be the function subtype), and arg1 is the argument array. Follows the same
	 - convention as the 'e' eval operation. If more than 2 arguments are passed and there's
	 - a 3rd argument, it takes a stack external type.
h(1e)- hash (FNV-1a). THis recursively hashes the value passed. Only takes a single
	 - argument.
k(1e)- scope stack manual control. depending on the argument, it can be used to pass
	 - the root stack or n level stacks up. If arg0 == 0, it returns the
	 - root stack. >0 is n level stacks up. If arg0 == -1, it creates a
	 - whole new stack.
	 - NOTE: circular references are very easy to make. If you set something
	 - to your current stack, it will leak. Manually setting that variable to
	 - some other type will fix this but it means it has to be manually managed
	 - in any case where you do use your own stack.
	 - Example:
	 -
	 - wEXT "initial"
	 - wFUNC{wEXT "test"}
	 - e(FUNC [] k-2)
	 - pEXT	# prints "initial" because FUNC didnt have access to EXT
	 -
	 - e(FUNC [] k-1)
	 - pEXT	# prints "test" because FUNC had access to EXT
a(2e)- search. arg0 is an array and arg1 is anything to be searched inside. Note:
	 - this has a bias toward strings so a("hey?hey" 63) and a("hey?hey" [63]) are
	 - functionally the same
g(0e)- random number. Generates a random number with no arguments, but if a single argument
	 - is passed, that number will be what limits the output. It can be negative and the output
	 - will be limited to a negative range. If 2 numeric arguments are supplied, the number
	 - fall in that range.
q(1e)- quicksort. If a single array argument is supplied, the return value will be the sorted array.
	 - If more than one argument, all of the arguments will be sorted and an array will be returned
u(2e)- array initialize/setup. if a single argument and a number, the array resilting array of
	 - length arg0 will be initialized to numeric 0. If 2 aguments, the array will contain
	 - arg0 elements but arg1 is evaluated for every index. This also passes the current index
	 - as index 0 of the ARGS/_ identifiers.
	 - Example:
	 -
	 - wARRAY_VAR u(6)
	 -
	 - produces [0 0 0 0 0 0]
	 -
	 - wARRAY_VAR u6 {-i_0}
	 -
	 - produces [-0 -1 -2 -3 -4 -5]

automatic default identifiers
	ARGS
	- arguments to operation block. Created when evaluated by something
	_ (note: "_")
	- same as ARGS
	NUMBER
	- equal to BOING_TYPE_VALUE_NUMBER
	ARRAY
	- equal to BOING_TYPE_VALUE_ARRAY
	OPERATION
	- equal to BOING_TYPE_VALUE_OPERATION
	EXTERNAL
	- equal to BOING_TYPE_VALUE_EXTERNAL
	TODO: add all of the stdlib identifiers and boing controls


*/

/* this may be a bit out of place, but because
various struct definitions exist below, this is necessary */
#ifndef BOING_ENABLE_LINE_NUM
	#define BOING_ENABLE_LINE_NUM 1
#endif


enum
{
	BOING_FLAG_NONE = 0ull,
	BOING_FLAG_INITIALIZED = 1ull,
	BOING_FLAG_NO_PRINT_NEWLINE = (1ull<<2),
};

enum
{
	BOING_VALUE_STRING_MINIFIED,
	BOING_VALUE_STRING_READABLE,
};

enum
{
	BOING_OPERATION_IMPLICIT,
	BOING_OPERATION_EXPLICIT
};

enum
{
	BOING_OPERATION_PASS_ARGS,
	BOING_OPERATION_EVAL_ARGS
};

enum
{
	BOING_EXTERNAL_POINTER,
	BOING_EXTERNAL_FUNCTION
};

enum
{
	BOING_TYPE_VALUE_ARRAY = 1,
	BOING_TYPE_VALUE_NUMBER = (1<<1),
	BOING_TYPE_VALUE_OPERATION = (1<<2),
	BOING_TYPE_VALUE_EXTERNAL = (1<<3)
};

enum
{
	BOING_COMPARISON_NOT_EQUAL,
	BOING_COMPARISON_EQUAL,
	BOING_COMPARISON_LESS_THAN,
	BOING_COMPARISON_GREATER_THAN
};

typedef struct boing_value_t boing_value_t;
typedef struct boing_t boing_t;
typedef struct boing_pool_t boing_pool_t;
typedef struct boing_module_t boing_module_t;

/* pool types */

typedef struct boing_pool_container_t
{
	uint8_t taken;
	size_t size; /* DO NOT USE IF VALUE */
	struct boing_pool_container_t *next, *prev;
	void *data;
} boing_pool_container_t;

typedef size_t (*boing_pool_container_destroy_cb_t)(boing_pool_t *pool, boing_pool_container_t *container);
typedef size_t (*boing_pool_container_size_cb_t)(boing_pool_t *pool, boing_pool_container_t *container);

struct boing_pool_t
{
	boing_pool_container_t *head, *end;
	size_t amount, free, max_free, block_alloc; /* if max_free is 0, then it is unlimited */
	boing_pool_container_destroy_cb_t container_cleanup;
	boing_pool_container_size_cb_t get_size;
};

/* boing types */

/* only used to report runtime errors if enabled */
typedef struct
{
	char *script, *name;
} boing_script_t;

typedef struct
{
	size_t references;
	void *ptr, *extra;
	int (*destroy_cb)(boing_t *boing, boing_value_t *value);
} boing_managed_ptr_t;

typedef boing_value_t *(*boing_external_function_t)(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);

/* operation data for the boing boing */
typedef struct
{
	char op;
	size_t arg_amount;
	uint8_t can_explicit, eval_args;
	boing_external_function_t callback;
} boing_operation_data_t;

typedef struct
{
	char op;
	boing_value_t *args_value; /* always an array */
} boing_operation_t;

typedef struct
{
	uint8_t type;
	union
	{
		void *ptr;
		boing_external_function_t fun;
	};
	int (*destroy_cb)(boing_t *boing, boing_value_t *value);
	/* because pointers sometimes need to be handled independently of where they originated,
	its necessary to provide a way to handle when they actually gain/lose references like
	when external values get copied and set or when they get released.
	
	If used this way, turning destroy into reference decrement works. */
	int (*reference_inc_cb)(boing_t *boing, boing_value_t *value);
} boing_external_t;

struct boing_module_t
{
	int (*module_init)(boing_t *boing, boing_module_t *module);
	int (*module_destroy)(boing_t *boing, boing_module_t *module);
	int (*module_stack_add)(boing_t *boing, boing_value_t *stack, boing_module_t *module);
	void *handle, *user_data;
};

struct boing_value_t
{
	#if BOING_ENABLE_LINE_NUM
	uint32_t line;
	boing_value_t *script;
	#endif
	boing_pool_container_t *container;
	uint32_t length, allocated, references;
	uint8_t type;
	union
	{
		double number;
		boing_value_t **array;
		boing_operation_t operation;
		boing_external_t external;
	};
};

struct boing_t
{
	void *user_data;
	size_t flags;
	/* program value. This is stored as
	[AST base_stack] */
	boing_value_t *program;
	boing_operation_data_t *operations;
	uint8_t operation_amount;
	/* the modules table stores all native modules and their functions */
	boing_value_t *modules;
	/* pools */
	struct
	{
		boing_pool_t value, 
		array_internal,
		string;
	} pool;

	struct
	{
		uint8_t *(*boing_read_file_cb)(boing_t *boing, char *path, size_t *size_read, size_t offset, size_t amount);
		size_t (*boing_write_file_cb)(boing_t *boing, char *path, void *data, size_t write_size);
		void (*boing_print_cb)(boing_t *boing, char *message);
		uint8_t *(*boing_read_cb)(boing_t *boing, size_t *read_size, size_t read_limit, uint8_t read_until_char);
		uint8_t *(*boing_import_cb)(boing_t *boing, boing_value_t *stack, char *path);
		void (*boing_error_cb)(boing_t *boing, char *message, int fatal);
		int (*boing_root_stack_init_cb)(boing_t *boing, boing_value_t *stack);
		int (*boing_module_cleanup_cb)(boing_t *boing, boing_module_t *module);
	} callback;

};


/* macros */

#define BOING_FLAG_READ(subject, flag) (subject & flag)
#define BOING_FLAG_SET(subject, flag) do{ subject |= flag; }while(0);
#define BOING_TO_STR_BEFORE(number) #number
#define BOING_TO_STR(number) BOING_TO_STR_BEFORE(number)
#define BOING_ADD_GLOBAL(identifier, value) do{	\
		boing_value_t *ident = NULL;	\
		boing_value_t *value_ptr = NULL;	\
		value_ptr = value;	\
		if(!value_ptr){	\
			boing_error(boing, 0, "value is NULL for " identifier);	\
			return 1;	\
		}	\
		if(!(ident = boing_value_from_str(boing, identifier))){	\
			boing_error(boing, 0, "could not create identifier " identifier);	\
			return 1;	\
		}	\
		if(boing_value_stack_add_set(boing, stack, ident, value_ptr)){	\
			boing_error(boing, 0, "could not add identifier " identifier " to stack");	\
			return 1;	\
		}	\
		/* need to refdec both */	\
		if(boing_value_reference_dec(boing, ident)){	\
			boing_error(boing, 0, "could not refdec identifier value for " identifier);	\
			return 1;	\
		}	\
		if(boing_value_reference_dec(boing, value_ptr)){	\
			boing_error(boing, 0, "could not refdec value for " identifier);	\
			return 1;	\
		}	\
	} while(0)
/* global definitions */

#define BOING_VERSION_MAJOR 0
#define BOING_VERSION_MINOR 1
#define BOING_VERSION_REVISION 2
#define BOING_VERSION_STRING "Boing v."BOING_TO_STR(BOING_VERSION_MAJOR)"."BOING_TO_STR(BOING_VERSION_MINOR)"."BOING_TO_STR(BOING_VERSION_REVISION)", compiled "__DATE__" "__TIME__

/* function prototypes */

void boing_value_debug_print(boing_value_t *v, int level, FILE *stream);
uint8_t *boing_read_file(boing_t *boing, char *path, size_t *size_read, size_t offset, size_t amount);
size_t boing_write_file(boing_t *boing, char *path, void *data, size_t write_size);
void boing_print(boing_t *boing, char *message);
uint8_t *boing_read_input(boing_t *boing, size_t *read_size, size_t read_limit, uint8_t read_until_char);
uint8_t *boing_import_file(boing_t *boing, boing_value_t *stack, char *path);
void boing_error(boing_t *boing, int fatal, char *fmt, ...);
void boing_error_script_print(boing_t *boing, boing_value_t *value);
int boing_module_add(boing_t *boing, char *name, int (*module_init)(boing_t *boing, boing_module_t *module), int (*module_destroy)(boing_t *boing, boing_module_t *module), int (*module_stack_add)(boing_t *boing, boing_value_t *stack, boing_module_t *module), void *handle);
int boing_module_cleanup_handle(boing_t *boing, boing_module_t *module);
int boing_module_stack_add(boing_t *boing, boing_value_t *stack, char *name);
int boing_root_stack_init(boing_t *boing, boing_value_t *stack);
size_t boing_sort_partition(boing_t *boing, boing_value_t *array, size_t start, size_t end);
void boing_sort_continue(boing_t *boing, boing_value_t *array, size_t start, size_t end);
boing_value_t *boing_sort_array(boing_t *boing, boing_value_t *array);

boing_value_t *boing_std_sin(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_asin(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_sinh(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_cos(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_acos(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_cosh(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_tan(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_atan(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_tanh(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_exp(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_log(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_log10(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_ceil(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_fabs(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_floor(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_atan2(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_string_get_block_alloc(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_string_set_block_alloc(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_string_get_amount(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_string_get_free(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_string_get_max_free(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_string_set_max_free(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_array_internal_get_block_alloc(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_array_internal_set_block_alloc(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_array_internal_get_amount(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_array_internal_get_free(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_array_internal_get_max_free(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_array_internal_set_max_free(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_value_get_block_alloc(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_value_set_block_alloc(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_value_get_amount(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_value_get_free(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_value_get_max_free(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_std_pool_value_set_max_free(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);

int boing_pool_init(boing_t *boing, boing_pool_t *pool, boing_pool_container_destroy_cb_t cleanup_callback, boing_pool_container_size_cb_t size_callback, size_t max_free);
int boing_pool_destroy(boing_t *boing, boing_pool_t *pool);
int boing_pool_list_remove(boing_t *boing, boing_pool_t *pool, boing_pool_container_t *remove);
int boing_pool_list_inject(boing_t *boing, boing_pool_t *pool, boing_pool_container_t *at, boing_pool_container_t *container);
int boing_pool_list_inject_after(boing_t *boing, boing_pool_t *pool, boing_pool_container_t *at, boing_pool_container_t *container);
void *boing_pool_request(boing_t *boing, boing_pool_t *pool, size_t min_size);
int boing_pool_add(boing_t *boing, boing_pool_t *pool, void *data, uint8_t taken, size_t size, boing_pool_container_t **data_container);
int boing_pool_shrink(boing_t *boing, boing_pool_t *pool);
boing_pool_container_t *boing_pool_data_container(boing_t *boing, boing_pool_t *pool, void *data);
int boing_pool_data_release(boing_t *boing, boing_pool_t *pool, void *data);
int boing_pool_data_release_container(boing_t *boing, boing_pool_t *pool, boing_pool_container_t *container);

char *boing_str_request(boing_t *boing, size_t size);
int boing_str_create_free(boing_t *boing, size_t size);
int boing_str_release(boing_t *boing, char *ptr);
size_t boing_str_alloc_size(boing_t *boing, char *ptr);
char *boing_str_resize(boing_t *boing, char *ptr, size_t size);
char *boing_str_sprintf(boing_t *boing, char *fmt, ...);
int boing_str_ncat(boing_t *boing, char **dest, char *src, size_t length);
char *boing_str_ndup(boing_t *boing, char *str, size_t length);
int boing_str_replace(boing_t *boing, char **source, char *replacee, char *replacement);

boing_value_t **boing_array_internl_request(boing_t *boing, size_t size);
int boing_array_internl_create_free(boing_t *boing, size_t size);
int boing_array_internl_release(boing_t *boing, void *ptr);
size_t boing_array_internl_alloc_size(boing_t *boing, void *ptr);
boing_value_t **boing_array_internl_resize(boing_t *boing, boing_value_t **ptr, size_t size);

boing_value_t *boing_value_pool_request(boing_t *boing, size_t size);
int boing_value_pool_create_free(boing_t *boing, size_t size);
int boing_value_pool_release(boing_t *boing, boing_value_t *ptr);

boing_value_t *boing_value_stack_create(boing_t *boing, boing_value_t *program);
boing_value_t *boing_value_stack_push(boing_t *boing, boing_value_t *parent);
boing_value_t *boing_value_stack_pop(boing_t *boing, boing_value_t *stack);
int boing_value_stack_is_root(boing_t *boing, boing_value_t *stack);
boing_value_t *boing_value_stack_search(boing_t *boing, boing_value_t *stack, boing_value_t *identifier);
int boing_value_stack_add_set(boing_t *boing, boing_value_t *stack, boing_value_t *identifier, boing_value_t *value);
boing_value_t *boing_value_stack_get_root(boing_t *boing, boing_value_t *stack);

int boing_value_table_add_set(boing_t *boing, boing_value_t *table, boing_value_t *key, boing_value_t *value, int add_not_found);
int boing_value_table_remove(boing_t *boing, boing_value_t *table, boing_value_t *key);
boing_value_t *boing_value_table_get(boing_t *boing, boing_value_t *table, boing_value_t *key);
boing_value_t *boing_value_table_get_str(boing_t *boing, boing_value_t *table, char *key);

boing_value_t *boing_operation_print(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_string(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_read(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_plus(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_minus(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_multiply(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_divide(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_modulo(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_power(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_logical_and(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_logical_or(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_logical_not(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_write(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_array(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_block_runtime(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_block_pass(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_identifier(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_eval(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_external(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_index(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_if(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_equality(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_lessthan(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_greaterthan(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_loop(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_sizeof(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_type(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_copy(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_inc(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_dec(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_table(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_hash(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_stack(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_file(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_import(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_search(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_random(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_sort(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);
boing_value_t *boing_operation_array_setup(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args);

boing_value_t *boing_value_request(boing_t *boing, uint8_t type, uint32_t alloc_length);
int boing_value_release(boing_t *boing, boing_value_t *value);
int boing_value_array_append_insert(boing_t *boing, boing_value_t *array, size_t position, boing_value_t *value);
int boing_value_array_remove(boing_t *boing, boing_value_t *array, size_t position);
boing_value_t *boing_value_array_index(boing_t *boing, boing_value_t *array, size_t position);
int boing_value_set(boing_t *boing, boing_value_t *target, boing_value_t *value);
boing_value_t *boing_value_copy(boing_t *boing, boing_value_t *value);
boing_value_t *boing_value_cast(boing_t *boing, boing_value_t *value, uint8_t cast);
void boing_value_reference_inc(boing_t *boing, boing_value_t *value);
int boing_value_reference_dec(boing_t *boing, boing_value_t *value);

boing_value_t *boing_value_from_ptr(boing_t *boing, void *ptr, uint8_t type, int (*destroy_cb)(boing_t *boing, boing_value_t *value));
boing_value_t *boing_value_from_ptr_managed(boing_t *boing, void *ptr, int (*destroy_cb)(boing_t *boing, boing_value_t *value));
boing_value_t *boing_value_from_args(boing_t *boing, int argc, char **argv);
boing_value_t *boing_value_from_double(boing_t *boing, double value);
boing_value_t *boing_value_from_strn(boing_t *boing, char *str, size_t length);
boing_value_t *boing_value_from_str(boing_t *boing, char *str);
boing_value_t *boing_value_from_table_fmt(boing_t *boing, char *fmt, ...);
char *boing_str_from_value_array_dest(boing_t *boing, boing_value_t *value, char *dest);
char *boing_str_from_value_array(boing_t *boing, boing_value_t *value);
char *boing_str_from_value_readable(boing_t *boing, boing_value_t *value, uint8_t *previous_type, uint8_t style, uint32_t level);
double boing_number_from_value_array(boing_t *boing, boing_value_t *value);

int boing_external_decrement_value_cleanup(boing_t *boing, boing_value_t *value);
int boing_external_increment_value(boing_t *boing, boing_value_t *value);
int boing_external_decrement_managed_cleanup(boing_t *boing, boing_value_t *value);
int boing_external_increment_managed(boing_t *boing, boing_value_t *value);

int boing_value_compare(boing_t *boing, boing_value_t *lhs, boing_value_t *rhs);
int boing_value_array_search(boing_t *boing, boing_value_t *array, boing_value_t *needle, size_t offset, size_t *pos);
size_t boing_value_hash_continue(boing_t *boing, boing_value_t *value, size_t start);
size_t boing_value_hash(boing_t *boing, boing_value_t *value);

int boing_is_operation(boing_t *boing, char op);
boing_operation_data_t *boing_operation_data(boing_t *boing, char op);
int boing_operation_has_explicit(boing_t *boing, char op);
size_t boing_operation_arg_amount(boing_t *boing, char op);
void boing_consume_whitespace(boing_t *boing, char **str);

boing_value_t *boing_parse_number(boing_t *boing, char **str);
boing_value_t *boing_parse_string(boing_t *boing, char **str);
boing_value_t *boing_parse_identifier(boing_t *boing, char **str);
boing_value_t *boing_parse_from_string(boing_t *boing, boing_value_t *parent, boing_value_t *current, boing_value_t *script, char **str, size_t *line);

boing_value_t *boing_program_call_global(boing_t *boing, boing_value_t *program, char *identifier, boing_value_t *args);
boing_value_t *boing_program_get_global(boing_t *boing, boing_value_t *program, char *identifier);

boing_value_t *boing_eval_value(boing_t *boing, boing_value_t *program, boing_value_t *value, boing_value_t *stack, boing_value_t *previous);
boing_value_t *boing_eval(boing_t *boing, char *str, boing_value_t *args, char *script_name);
boing_value_t *boing_eval_file(boing_t *boing, char *path, boing_value_t *args);
int boing_init(boing_t *boing);
int boing_destroy(boing_t *boing, int error_print_limit);



#ifdef BOING_IMPLEMENTATION

/* config */

#ifndef BOING_DEFAULT_SIZE_STRING
#define BOING_DEFAULT_SIZE_STRING 40 /* minimum of 0 */
#endif

#ifndef BOING_DEFAULT_FREE_STRING
#define BOING_DEFAULT_FREE_STRING 20 /* minimum of 2 */
#endif

#ifndef BOING_DEFAULT_FREE_STRING_MAX
#define BOING_DEFAULT_FREE_STRING_MAX 50 /* minimum of 0 */
#endif


/* just the parts of the internal array buffer */
#ifndef BOING_DEFAULT_SIZE_VALUE_ARRAY_INTERNAL
#define BOING_DEFAULT_SIZE_VALUE_ARRAY_INTERNAL 40 /* minimum of 0 */
#endif

#ifndef BOING_DEFAULT_FREE_VALUE_ARRAY_INTERNAL
#define BOING_DEFAULT_FREE_VALUE_ARRAY_INTERNAL 50 /* minimum of 2 */
#endif

#ifndef BOING_DEFAULT_FREE_VALUE_ARRAY_INTERNAL_MAX
#define BOING_DEFAULT_FREE_VALUE_ARRAY_INTERNAL_MAX 80 /* minimum of 0 */
#endif


/* the base value struct pool */
#ifndef BOING_DEFAULT_FREE_VALUE_BASE
#define BOING_DEFAULT_FREE_VALUE_BASE 80 /* minimum of 2 */
#endif

#ifndef BOING_DEFAULT_FREE_VALUE_BASE_MAX
#define BOING_DEFAULT_FREE_VALUE_BASE_MAX 150 /* minimum of 0 */
#endif



#ifdef BOING_HASH_64
	#define BOING_FNV_OFFSET 0xcbf29ce484222325LL
	#define BOING_FNV_PRIME 0x100000001b3LL
#else /* just default to a 32 bit version */
	#define BOING_FNV_OFFSET 0x811c9dc5L
	#define BOING_FNV_PRIME 0x01000193L
#endif


#ifndef BOING_VSNPRINTF
	#define BOING_VSNPRINTF vsnprintf
#endif

/* ========================= standard operations ========================= */


static int boing_script_data_cleanup(boing_t *boing, boing_value_t *value)
{
	/* free the strings inside first */
	if(boing_str_release(boing, ((boing_script_t *)value->external.ptr)->script))
	{
		boing_error(boing, 0, "could not release script text data in script %s", ((boing_script_t *)value->external.ptr)->name);
		return 1;
	}

	if(boing_str_release(boing, ((boing_script_t *)value->external.ptr)->name))
	{
		/* well, couldnt release it so might as well read it */
		boing_error(boing, 0, "could not release script name string in script %s", ((boing_script_t *)value->external.ptr)->name);
		return 1;
	}

	/* free the container */
	if(boing_str_release(boing, value->external.ptr))
	{
		boing_error(boing, 0, "could not release script container");
		return 1;
	}

	return 0;
}

boing_value_t *boing_operation_print(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	/* convert array to a bunch of strings */
	char *message = NULL, *temp = NULL;
	size_t i;


	if(!(message = boing_str_request(boing, 1)))
	{
		boing_error(boing, 0, "could not request empty string for message in print operation");
		return NULL;
	}

	message[0] = 0;

	for(i = 0; i < args->length; ++i)
	{
		switch(args->array[i]->type)
		{
			case BOING_TYPE_VALUE_ARRAY:
				if(!(temp = boing_str_from_value_array(boing, args->array[i])))
				{
					boing_error(boing, 0, "could not create string from value array in print operation");
					/* TODO throw error */
					return NULL;
				}

				if(boing_str_ncat(boing, &message, temp, strlen(temp)))
				{
					boing_error(boing, 0, "could not concatenate string in print operation");
					/* TODO throw error */
					return NULL;
				}

				if(boing_str_release(boing, temp))
				{
					boing_error(boing, 0, "could not release temporary string in print operation");
					/* TODO throw error */
					return NULL;
				}
			break;
			case BOING_TYPE_VALUE_NUMBER:
				if(!(temp = boing_str_sprintf(boing, "%g", args->array[i]->number)))
				{
					boing_error(boing, 0, "could not convert number to string in print operation");
					/* TODO throw error */
					return NULL;
				}

				if(boing_str_ncat(boing, &message, temp, strlen(temp)))
				{
					boing_error(boing, 0, "could not concatenate string in print operation");
					/* TODO throw error */
					return NULL;
				}

				if(boing_str_release(boing, temp))
				{
					boing_error(boing, 0, "could not release temporary string in print operation");
					/* TODO throw error */
					return NULL;
				}
			break;
		}
		
	}

	boing_print(boing, message);

	if(boing_str_release(boing, message))
	{
		boing_error(boing, 0, "could not release message string after printing");
		/* TODO throw error */
		return NULL;
	}

	return boing_value_from_double(boing, 0);
}

boing_value_t *boing_operation_string(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	char *temp = NULL;
	uint8_t previous_type = -1, style = BOING_VALUE_STRING_MINIFIED;


	if(args->length < 1 || args->length > 2)
	{
		boing_error(boing, 0, "'s' operation expects just 1 or 2 arguments");
		/* TODO throw error */
		return NULL;
	}

	if(args->length == 2 && args->array[1]->type == BOING_TYPE_VALUE_NUMBER && args->array[1]->number)
		style = BOING_VALUE_STRING_READABLE;
	else if(args->length == 2 && args->array[1]->type == BOING_TYPE_VALUE_NUMBER && !args->array[1]->number)
		style = BOING_VALUE_STRING_MINIFIED;
	else if(args->length == 2)
	{
		boing_error(boing, 0, "expected numeric type in arg1 in 's' operation");
		/* TODO throw error */
		return NULL;
	}

	if(!(temp = boing_str_from_value_readable(boing, args->array[0], &previous_type, style, 0)))
	{
		boing_error(boing, 0, "could not turn value into string in 's' operation");
		/* TODO throw error */
		return NULL;
	}


	if(!(ret = boing_value_from_str(boing, temp)))
	{
		boing_error(boing, 0, "could not turn string into value array in 's' operation");
		/* TODO throw error */
		return NULL;
	}


	if(boing_str_release(boing, temp))
	{
		boing_error(boing, 0, "could not release string in 's' operation");
		/* TODO throw error */
		return NULL;
	}


	return ret;
}

boing_value_t *boing_operation_read(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	uint8_t *input = NULL;
	size_t size = 0;


	if(!args->length)
	{
		/* read single character */
		if(!(input = boing_read_input(boing, &size, 1, 0)))
		{
			boing_error(boing, 0, "could not read from input in the read operation 'r'");
			/* TODO throw error */
			return NULL;
		}

		/* turn into single number */
		if(!(ret = boing_value_from_double(boing, (double)input[0])))
		{
			boing_error(boing, 0, "could not request value from character in the read operation 'r'");
			/* TODO throw error */
			return NULL;
		}

		if(boing_str_release(boing, input))
		{
			boing_error(boing, 0, "could not release input str in the read operation 'r'");
			/* TODO throw error */
			return NULL;
		}
	}
	else if(args->length == 1 && (args->array[0]->type == BOING_TYPE_VALUE_NUMBER || args->array[0]->type == BOING_TYPE_VALUE_ARRAY))
	{
		/* if 0, read till EOF, if >0 read until amount of chars satisfied */
		if(args->array[0]->type == BOING_TYPE_VALUE_NUMBER && args->array[0]->number)
		{
			/* read until amount set */
			if(!(input = boing_read_input(boing, &size, args->array[0]->number, 0)))
			{
				boing_error(boing, 0, "could not read from input in the read operation 'r'");
				/* TODO throw error */
				return NULL;
			}
		}
		else if(args->array[0]->type == BOING_TYPE_VALUE_ARRAY && args->array[0]->length && args->array[0]->array[0]->type == BOING_TYPE_VALUE_NUMBER)
		{
			/* read until character inside string */
			if(!(input = boing_read_input(boing, &size, 0, (char)args->array[0]->array[0]->number)))
			{
				boing_error(boing, 0, "could not read from input in the read operation 'r'");
				/* TODO throw error */
				return NULL;
			}
		}
		else
		{
			/* read until EOF */
			if(!(input = boing_read_input(boing, &size, 0, 0)))
			{
				boing_error(boing, 0, "could not read from input in the read operation 'r' until newline");
				/* TODO throw error */
				return NULL;
			}
		}

		/*  make the return array, and clean up the input string */
		
		if(!(ret = boing_value_from_strn(boing, input, size)))
		{
			boing_error(boing, 0, "could not create array value in the read operation 'r'");
			/* TODO throw error */
			return NULL;
		}

		if(boing_str_release(boing, input))
		{
			boing_error(boing, 0, "could not release input str in the read operation 'r'");
			/* TODO throw error */
			return NULL;
		}
		
	}
	else
	{
		boing_error(boing, 0, "the read operation 'r' only expects 0-1 arguments and expects a number if provided");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

boing_value_t *boing_operation_plus(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *temp = NULL;
	char *str = NULL;
	size_t i, j;

	/* NOTE: refer to documentation. This operation does a lot */

	/* this operation requires at least 1 argument */
	if(!args->length)
	{
		boing_error(boing, 0, "too few arguments supplied to '+' operation");
		/* TODO throw error */
		return NULL;
	}

	/* determine functionality */
	if(args->length == 1 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER) /* positive */
	{
		if(!(ret = boing_value_from_double(boing, fabs(args->array[0]->number))))
		{
			boing_error(boing, 0, "could not request new value for abs of number");
			/* TODO throw error */
			return NULL;
		}
	}
	else if(args->length == 1 && args->array[0]->type == BOING_TYPE_VALUE_ARRAY) /* array sum */
	{
		if(!(ret = boing_value_from_double(boing, 0)))
		{
			boing_error(boing, 0, "could not request new value for array sum");
			/* TODO throw error */
			return NULL;
		}

		for(i = 0; i < args->array[0]->length; ++i)
		{
			if(args->array[0]->array[i]->type == BOING_TYPE_VALUE_NUMBER)
				ret->number += args->array[0]->array[i]->number;
		}
	}
	else if(args->length > 1 && args->array[0]->type == BOING_TYPE_VALUE_ARRAY) /* append arrays or numbers */
	{
		if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, args->array[0]->length)))
		{
			boing_error(boing, 0, "could not request new value for array concatenation");
			/* TODO throw error */
			return NULL;
		}

		/* because the primary use of this is is going to be string concatenation,
		outer arrays will be ignored */

		for(i = 0; i < args->length; ++i)
		{
			/* same result, but have to jump into the array to append the members */
			if(args->array[i]->type == BOING_TYPE_VALUE_ARRAY)
			{
				for(j = 0; j < args->array[i]->length; ++j)
				{
					if(boing_value_array_append_insert(boing, ret, ret->length, args->array[i]->array[j]))
					{
						boing_error(boing, 0, "could not insert value into returned array");
						/* TODO throw error */
						return NULL;
					}

					boing_value_reference_inc(boing, args->array[i]->array[j]);
				}
			}
			else
			{
				if(boing_value_array_append_insert(boing, ret, ret->length, args->array[i]))
				{
					boing_error(boing, 0, "could not insert value into returned array");
					/* TODO throw error */
					return NULL;
				}

				boing_value_reference_inc(boing, args->array[i]);
			}
		}

	}
	else if(args->length > 1 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER) /* add all numbers and return the sum */
	{
		if(!(ret = boing_value_from_double(boing, args->array[0]->number)))
		{
			boing_error(boing, 0, "could not request new value for '+' operation sum");
			/* TODO throw error */
			return NULL;
		}

		for(i = 1; i < args->length; ++i)
		{
			if(args->array[i]->type == BOING_TYPE_VALUE_NUMBER)
				ret->number += args->array[i]->number;
			else if(args->array[i]->type == BOING_TYPE_VALUE_ARRAY)
				ret->number += boing_number_from_value_array(boing, args->array[i]);
			else
			{
				boing_error(boing, 0, "bad argument type in arg%lu '+' operation sum", i);
				/* TODO throw error */
				return NULL;
			}
		}
	}

	return ret;
}

boing_value_t *boing_operation_minus(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	char *str = NULL, *temp = NULL, subtraction_started = 0;
	size_t i, j, pos = 0;
	int search = 0;

	/* NOTE: refer to documentation. This operation does a lot */

	/* this operation requires at least 1 argument */
	if(!args->length)
	{
		boing_error(boing, 0, "too few arguments supplied to '-' operation");
		/* TODO throw error */
		return NULL;
	}

	/* determine functionality */
	if(args->length == 1 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER) /* negative */
	{
		if(!(ret = boing_value_from_double(boing, -fabs(args->array[0]->number)))) /* could test for things, but branching is slower than this */
		{
			boing_error(boing, 0, "could not request new value for -abs of number");
			/* TODO throw error */
			return NULL;
		}
	}
	else if(args->length == 1 && args->array[0]->type == BOING_TYPE_VALUE_ARRAY) /* array difference */
	{
		if(!(ret = boing_value_from_double(boing, 0)))
		{
			boing_error(boing, 0, "could not request new value for array difference");
			/* TODO throw error */
			return NULL;
		}

		for(i = 0; i < args->array[0]->length; ++i)
		{
			if(subtraction_started)
			{
				if(args->array[0]->array[i]->type == BOING_TYPE_VALUE_NUMBER)
					ret->number -= args->array[0]->array[i]->number;
			}
			else
			{
				if(args->array[0]->array[i]->type == BOING_TYPE_VALUE_NUMBER)
					ret->number = args->array[0]->array[i]->number;
				
				subtraction_started++;
			}
		}
	}
	else if(args->length > 1 && args->array[0]->type == BOING_TYPE_VALUE_ARRAY) /* remove strings or numbers */
	{
		if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 0)))
		{
			boing_error(boing, 0, "could not request new value for array member removal");
			/* TODO throw error */
			return NULL;
		}

		/* add the arg0 values to this new array */
		if(boing_value_set(boing, ret, args->array[0]))
		{
			boing_error(boing, 0, "could not set returned array value arg0");
			/* TODO throw error */
			return NULL;
		}


		/* because the primary use of this is is going to be string removal,
		outer arrays will be ignored (this can be wrapped in a second array to
		give this behavior back) */

		for(i = 1; i < args->length; ++i)
		{
			while((search = boing_value_array_search(boing, ret, args->array[i], pos, &pos)))
			{
				for(j = 0; j < (args->array[i]->length ? args->array[i]->length : 1); ++j)
				{
					if(boing_value_array_remove(boing, ret, pos))
					{
						boing_error(boing, 0, "could not remove value from array at arg%lu", i);
						/* TODO throw error */
						return NULL;
					}
				}
			}

			if(!search)
				break;
		}
	}
	else if(args->length > 1 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER) /* subtract all numbers and return the sum */
	{
		if(!(ret = boing_value_from_double(boing, args->array[0]->number)))
		{
			boing_error(boing, 0, "could not request new value for '-' operation difference");
			/* TODO throw error */
			return NULL;
		}

		for(i = 1; i < args->length; ++i)
		{
			if(args->array[i]->type == BOING_TYPE_VALUE_NUMBER)
				ret->number -= args->array[i]->number;
			else if(args->array[i]->type == BOING_TYPE_VALUE_ARRAY)
				ret->number -= boing_number_from_value_array(boing, args->array[i]);
			else
			{
				boing_error(boing, 0, "bad argument type in arg%lu '-' operation sum", i);
				/* TODO throw error */
				return NULL;
			}
		}
	}

	return ret;
}

boing_value_t *boing_operation_multiply(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	char *str = NULL, *temp_str = NULL, multiplication_started = 0;
	size_t i, j;

	/* NOTE: refer to documentation. This operation does a lot */

	/* this operation requires at least 1 argument */
	if(!args->length)
	{
		boing_error(boing, 0, "too few arguments supplied to '*' operation");
		/* TODO throw error */
		return NULL;
	}

	/* determine functionality */
	if(args->length == 1 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER) /* square */
	{
		if(!(ret = boing_value_from_double(boing, args->array[0]->number * args->array[0]->number)))
		{
			boing_error(boing, 0, "could not request new value for arg0^2 of number");
			/* TODO throw error */
			return NULL;
		}
	}
	else if(args->length == 1 && args->array[0]->type == BOING_TYPE_VALUE_ARRAY) /* array multiplication */
	{
		if(!(ret = boing_value_from_double(boing, 0)))
		{
			boing_error(boing, 0, "could not request new value for array multiplication");
			/* TODO throw error */
			return NULL;
		}

		for(i = 0; i < args->array[0]->length; ++i)
		{
			if(multiplication_started)
			{
				if(args->array[0]->array[i]->type == BOING_TYPE_VALUE_NUMBER)
					ret->number *= args->array[0]->array[i]->number;
			}
			else
			{
				if(args->array[0]->array[i]->type == BOING_TYPE_VALUE_NUMBER)
					ret->number = args->array[0]->array[i]->number;
				
				multiplication_started++;
			}
		}
	}
	else if(args->length > 1 && args->array[0]->type == BOING_TYPE_VALUE_ARRAY) /* remove return an array of split array connected by token */
	{
		#define CONCAT_ITEM(arr, val)	\
		do	\
		{	\
			size_t k;	\
			if(val->type == BOING_TYPE_VALUE_ARRAY)	\
			{	\
				for(k = 0; k < val->length; ++k)	\
				{	\
					if(boing_value_array_append_insert(boing, arr, arr->length, val->array[k]))	\
					{	\
						boing_error(boing, 0, "could not insert value in '*' operation union");	\
						/* TODO throw error */	\
						return NULL;	\
					}	\
					boing_value_reference_inc(boing, val->array[k]);	\
				}	\
			}	\
			else	\
			{	\
				if(boing_value_array_append_insert(boing, arr, arr->length, val))	\
				{	\
					boing_error(boing, 0, "could not insert value in '*' operation union");	\
					/* TODO throw error */	\
					return NULL;	\
				}	\
				boing_value_reference_inc(boing, val);	\
			}	\
		} while(0)


		/* TODO: estimate array size */
		if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 1)))
		{
			boing_error(boing, 0, "could not request new value for '*' operation union");
			/* TODO throw error */
			return NULL;
		}

		/* loop through array and concat everything with the token(s) provided */
		for(i = 0; i < args->array[0]->length - 1; ++i)
		{
			/* add the beginning */
			CONCAT_ITEM(ret, args->array[0]->array[i]);

			/* concat with arg1+ */
			for(j = 1; j < args->length; ++j)
			{
				CONCAT_ITEM(ret, args->array[j]);
			}
		}

		/* add the end OR, in case the previous didnt run, add it back */
		if(args->array[0]->length > 1)
			CONCAT_ITEM(ret, args->array[0]->array[args->array[0]->length - 1]);
		else if(args->array[0]->length == 1)
			CONCAT_ITEM(ret, args->array[0]->array[0]);
		
	}
	else if(args->length > 1 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER) /* multiply all numbers and return the product */
	{
		if(!(ret = boing_value_from_double(boing, args->array[0]->number)))
		{
			boing_error(boing, 0, "could not request new value for '*' operation product");
			/* TODO throw error */
			return NULL;
		}

		for(i = 1; i < args->length; ++i)
		{
			if(args->array[i]->type == BOING_TYPE_VALUE_NUMBER)
				ret->number *= args->array[i]->number;
			else if(args->array[i]->type == BOING_TYPE_VALUE_ARRAY)
				ret->number *= boing_number_from_value_array(boing, args->array[i]);
			else
			{
				boing_error(boing, 0, "bad argument type in arg%lu '*' operation sum", i);
				/* TODO throw error */
				return NULL;
			}
		}
	}

	return ret;
}

boing_value_t *boing_operation_divide(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *old_array = NULL, *temp = NULL;
	char *str = NULL, *temp_str = NULL, division_started = 0;
	size_t i, j, k, pos, last = 0;

	/* NOTE: refer to documentation. This operation does a lot */

	/* this operation requires at least 1 argument */
	if(!args->length)
	{
		boing_error(boing, 0, "too few arguments supplied to '/' operation");
		/* TODO throw error */
		return NULL;
	}

	/* determine functionality */
	if(args->length == 1 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER) /* sqrt */
	{
		if(!(ret = boing_value_from_double(boing, sqrt(args->array[0]->number))))
		{
			boing_error(boing, 0, "could not request new value for sqrt of number");
			/* TODO throw error */
			return NULL;
		}
	}
	else if(args->length == 1 && args->array[0]->type == BOING_TYPE_VALUE_ARRAY) /* array quotient */
	{
		if(!(ret = boing_value_from_double(boing, 0)))
		{
			boing_error(boing, 0, "could not request new value for array quotient");
			/* TODO throw error */
			return NULL;
		}

		for(i = 0; i < args->array[0]->length; ++i)
		{
			if(division_started)
			{
				if(args->array[0]->array[i]->type == BOING_TYPE_VALUE_NUMBER)
					ret->number /= args->array[0]->array[i]->number;
			}
			else
			{
				if(args->array[0]->array[i]->type == BOING_TYPE_VALUE_NUMBER)
					ret->number = args->array[0]->array[i]->number;
				
				division_started++;
			}
		}
	}
	else if(args->length > 1 && args->array[0]->type == BOING_TYPE_VALUE_ARRAY) /* tokenize array */
	{
		if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 1)))
		{
			boing_error(boing, 0, "could not request new value for '/' operation tokenization");
			/* TODO throw error */
			return NULL;
		}


		if(!(temp = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 1)))
		{
			boing_error(boing, 0, "could not request new member value for '/' operation tokenization");
			/* TODO throw error */
			return NULL;
		}

		if(boing_value_set(boing, temp, args->array[0]))
		{
			boing_error(boing, 0, "could not set tokenized return value for '/' operation tokenization");
			/* TODO throw error */
			return NULL;
		}


		if(boing_value_array_append_insert(boing, ret, 0, temp))
		{
			boing_error(boing, 0, "could not insert tokenized return value for '/' operation tokenization");
			/* TODO throw error */
			return NULL;
		}


		/* split everything that matches arg1+ */
		for(i = 1; i < args->length; ++i)
		{

			for(j = 0; j < ret->length; ++j) /* TODO: jump forward another if split */
			{
				last = 0;

				while(boing_value_array_search(boing, ret->array[j], args->array[i], last, &pos))
				{
					/* TODO: macroify all of this */

					/* split old array into 2 new arrays, insert, and decrement the old array */
					

					/* remember old array even though it's destroyed later on */
					boing_value_reference_inc(boing, ret->array[j]);
					old_array = ret->array[j];

					/* before token */
					if(!(temp = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, pos - last)))
					{
						boing_error(boing, 0, "could not request split member value for '/' operation tokenization");
						/* TODO throw error */
						return NULL;
					}


					for(k = last; k < pos; ++k)
					{
						if(boing_value_array_append_insert(boing, temp, temp->length, old_array->array[k]))
						{
							boing_error(boing, 0, "could not insert array member in temp array in '/' operation tokenization");
							/* TODO throw error */
							return NULL;
						}

						boing_value_reference_inc(boing, old_array->array[k]);
					}



					/* remove old array */
					if(boing_value_array_remove(boing, ret, j))
					{
						boing_error(boing, 0, "could not release old array member in '/' operation tokenization");
						/* TODO throw error */
						return NULL;
					}

					/* add new before */
					if(boing_value_array_append_insert(boing, ret, j, temp))
					{
						boing_error(boing, 0, "could not insert new before token array member in '/' operation tokenization");
						/* TODO throw error */
						return NULL;
					}


					last = pos + (args->array[i]->length ? args->array[i]->length : 1); /* TODO shift over the size of the pos */


					/* after token */
					if(!(temp = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, old_array->length - last)))
					{
						boing_error(boing, 0, "could not request split member value for '/' operation tokenization");
						/* TODO throw error */
						return NULL;
					}


					for(k = last; k < old_array->length; ++k)
					{
						if(boing_value_array_append_insert(boing, temp, temp->length, old_array->array[k]))
						{
							boing_error(boing, 0, "could not insert array member in temp array in '/' operation tokenization");
							/* TODO throw error */
							return NULL;
						}

						boing_value_reference_inc(boing, old_array->array[k]);
					}
					

					/* add new after last */
					if(boing_value_array_append_insert(boing, ret, j + 1, temp))
					{
						boing_error(boing, 0, "could not insert new after token array member in '/' operation tokenization");
						/* TODO throw error */
						return NULL;
					}


					last = pos + (args->array[i]->length ? args->array[i]->length : 1);


					if(boing_value_reference_dec(boing, old_array))
					{
						boing_error(boing, 0, "could not release old array member in '/' operation tokenization");
						/* TODO throw error */
						return NULL;
					}
				}
			}
		}

	}
	else if(args->length > 1 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER) /* divide all numbers and return the sum */
	{
		if(!(ret = boing_value_from_double(boing, args->array[0]->number)))
		{
			boing_error(boing, 0, "could not request new value for '/' operation quotient");
			/* TODO throw error */
			return NULL;
		}

		for(i = 1; i < args->length; ++i)
		{
			if(args->array[i]->type == BOING_TYPE_VALUE_NUMBER)
				ret->number /= args->array[i]->number;
			else if(args->array[i]->type == BOING_TYPE_VALUE_ARRAY)
				ret->number /= boing_number_from_value_array(boing, args->array[i]);
			else
			{
				boing_error(boing, 0, "bad argument type in arg%lu '/' operation sum", i);
				/* TODO throw error */
				return NULL;
			}
		}
	}

	return ret;
}

boing_value_t *boing_operation_modulo(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	size_t i;
	

	if(args->length < 2 || args->array[0]->type != BOING_TYPE_VALUE_NUMBER || args->array[1]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "modulo operation expects at least 2 numeric arguments");
		/* TODO throw error */
		return NULL;
	}


	if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_NUMBER, 0)))
	{
		boing_error(boing, 0, "could not request return value in modulo operation '%%'");
		/* TODO throw error */
		return NULL;
	}

	for(i = 0; i < args->length; ++i)
	{
		if(!i)
		{
			ret->number = fmod(args->array[0]->number, args->array[1]->number);
			i = 1;
		}
		else
			ret->number = fmod(ret->number, args->array[i]->number);
	}

	return ret;
}

boing_value_t *boing_operation_power(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	size_t i;
	

	if(args->length < 2 || args->array[0]->type != BOING_TYPE_VALUE_NUMBER || args->array[1]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "power operation expects at least 2 numeric arguments");
		/* TODO throw error */
		return NULL;
	}


	if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_NUMBER, 0)))
	{
		boing_error(boing, 0, "could not request return value in power operation '^'");
		/* TODO throw error */
		return NULL;
	}

	for(i = 0; i < args->length; ++i)
	{
		if(!i)
		{
			ret->number = pow(args->array[0]->number, args->array[1]->number);
			i = 1;
		}
		else
			ret->number = pow(ret->number, args->array[i]->number);
	}

	return ret;
}

boing_value_t *boing_operation_logical_and(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	size_t i;


	if(args->length < 2)
	{
		boing_error(boing, 0, "too few arguments supplied to '&' operation");
		/* TODO throw error */
		return NULL;
	}

	/* loop through arguments and return 0 if any numeric 0 found */
	for(i = 0; i < args->length; ++i)
	{
		if(args->array[i]->type == BOING_TYPE_VALUE_NUMBER && !args->array[i]->number)
		{
			if(!(ret = boing_value_from_double(boing, 0.0)))
			{
				boing_error(boing, 0, "could not convert double to value to return in '&' operation");
				/* TODO throw error */
				return NULL;
			}

			return ret;
		}
	}


	/* everything was nonzero */
	if(!(ret = boing_value_from_double(boing, 1.0)))
	{
		boing_error(boing, 0, "could not convert double to value to return in '&' operation");
		/* TODO throw error */
		return NULL;
	}


	return ret;
}

boing_value_t *boing_operation_logical_or(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	size_t i;


	if(args->length < 2)
	{
		boing_error(boing, 0, "too few arguments supplied to '|' operation");
		/* TODO throw error */
		return NULL;
	}

	/* loop through arguments and return 1 if nonzero is found */
	for(i = 0; i < args->length; ++i)
	{
		if(args->array[i]->type != BOING_TYPE_VALUE_NUMBER || (args->array[i]->type == BOING_TYPE_VALUE_NUMBER && args->array[i]->number))
		{
			if(!(ret = boing_value_from_double(boing, 1.0)))
			{
				boing_error(boing, 0, "could not convert double to value to return in '|' operation");
				/* TODO throw error */
				return NULL;
			}

			return ret;
		}
	}


	/* everything was zero */
	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not convert double to value to return in '|' operation");
		/* TODO throw error */
		return NULL;
	}


	return ret;
}

boing_value_t *boing_operation_logical_not(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length != 1)
	{
		boing_error(boing, 0, "'!' operation only takes 1 argument");
		/* TODO throw error */
		return NULL;
	}

	if(args->array[0]->type == BOING_TYPE_VALUE_NUMBER && !args->array[0]->number)
	{
		/* return 1 */
		if(!(ret = boing_value_from_double(boing, 1.0)))
		{
			boing_error(boing, 0, "could not create numeric value to return from '!' operation");
			/* TODO throw error */
			return NULL;
		}
	}
	else
	{
		/* return 0 */
		if(!(ret = boing_value_from_double(boing, 0.0)))
		{
			boing_error(boing, 0, "could not create numeric value to return from '!' operation");
			/* TODO throw error */
			return NULL;
		}
	}


	return ret;
}


boing_value_t *boing_operation_write(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *temp = NULL;


	if(args->length < 1)
	{
		boing_error(boing, 0, "too few arguments to write operation");
		return NULL;
	}
	else if(args->length == 1)
	{
		if(!(temp = boing_value_from_double(boing, 0)))
		{
			boing_error(boing, 0, "could not set rhs value to 0 in write operation");
			return NULL;
		}
	}
	else /* everything is 2<= */
		temp = args->array[1];


	if(boing_value_set(boing, args->array[0], temp))
	{
		boing_error(boing, 0, "could not set target value in write operation");
		return NULL;
	}
	

	/* return the value that was the target */
	ret = args->array[0];
	boing_value_reference_inc(boing, ret);


	return ret;
}

boing_value_t *boing_operation_array(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *temp = NULL;
	size_t i;


	/* request an array that sallocated ot be at least the size of the args */
	if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, args->length)))
	{
		boing_error(boing, 0, "could not allocate array for literal");
		/* TODO throw error */
		return NULL;
	}


	for(i = 0; i < args->length; ++i)
	{
		switch(args->array[i]->type)
		{
			case BOING_TYPE_VALUE_NUMBER:
			case BOING_TYPE_VALUE_ARRAY:
			case BOING_TYPE_VALUE_EXTERNAL:
				temp = args->array[i];
				boing_value_reference_inc(boing, temp);
			break;
			case BOING_TYPE_VALUE_OPERATION: /* have to eval */
				if(!(temp = boing_eval_value(boing, program, args->array[i], stack, previous)))
				{
					boing_error(boing, 0, "could not eval operation in array literal");
					/* TODO throw error */
					return NULL;
				}
			break;
		}

		/* throw the value in the final array */
		if(boing_value_array_append_insert(boing, ret, i, temp))
		{
			boing_error(boing, 0, "could not insert value into array for literal");
			/* TODO throw error */
			return NULL;
		}

	}


	return ret;
}

boing_value_t *boing_operation_block_runtime(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	size_t i;


	ret = previous;
	boing_value_reference_inc(boing, ret);

	/* this is where true eval happens */
	for(i = 0; i < args->length; ++i)
	{
		/* clean up after last ret if it exists. Previous is set rather than the pointer being transfered */
		if(ret)
		{
			if(boing_value_reference_dec(boing, ret))
			{
				boing_error(boing, 0, "could not refdec last arg in '(' operation");
				/* TODO throw error */
				return NULL;
			}
		}

		/* eval the current value */
		if(!(ret = boing_eval_value(boing, program, args->array[i], stack, previous)))
		{
			boing_error(boing, 0, "could not eval value in '(' operation");
			/* TODO throw error */
			return NULL;
		}


		/* handle previous value propigation */
		if(boing_value_set(boing, previous, ret))
		{
			boing_error(boing, 0, "could not set previous to operation return value");
			/* TODO throw error */
			return NULL;
		}
	}


	return ret;
}

boing_value_t *boing_operation_block_pass(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	/* simply return the operation ast enclosed in a runtime block operation */
	if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_OPERATION, 0)))
	{
		boing_error(boing, 0, "could not request '(' operation inside block operation");
		/* TODO throw error */
		return NULL;
	}

	ret->operation.op = '(';

	/* set the runtime block operation's args to the ast */
	if(boing_value_set(boing, ret->operation.args_value, args))
	{
		boing_error(boing, 0, "could not set runtime block operation arguments to arg0");
		/* TODO throw error */
		return NULL;
	}


	return ret;
}

boing_value_t *boing_operation_identifier(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *temp = NULL;

	if(!(ret = boing_value_stack_search(boing, stack, args->array[0])))
	{
		/* create new identifier on the stack if it cant find one */

		/* create a quick default */
		if(!(temp = boing_value_request(boing, BOING_TYPE_VALUE_NUMBER, 0)))
		{
			boing_error(boing, 0, "could not create temporary identifier value");
			/* TODO throw error */
			return NULL;
		}

		if(boing_value_stack_add_set(boing, stack, args->array[0], temp))
		{
			boing_error(boing, 0, "could not add temporary identifier to stack");
			/* TODO throw error */
			return NULL;
		}

		/* check 1 more time */
		if(!(ret = boing_value_stack_search(boing, stack, args->array[0])))
		{
			boing_error(boing, 0, "could not find identifier on stack just created, strange");
			/* TODO throw error */
			return NULL;
		}
	}
	else
		boing_value_reference_inc(boing, ret);

	

	return ret;
}

boing_value_t *boing_operation_eval(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *eval_args = NULL, *new_stack = NULL, *temp_ident = NULL;


	/* handle non array arguments */
	if(args->length == 1)
	{
		if(!(eval_args = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 0)))
		{
			boing_error(boing, 0, "could not request empty args array for eval operation");
			/* TODO throw error */
			return NULL;
		}
	}
	else if(args->length >= 2 && args->array[1]->type != BOING_TYPE_VALUE_ARRAY)
	{
		/* put the value into an array */
		if(!(eval_args = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 1)))
		{
			boing_error(boing, 0, "could not request new args array for eval operation");
			/* TODO throw error */
			return NULL;
		}

		if(boing_value_array_append_insert(boing, eval_args, 0, args->array[1]))
		{
			boing_error(boing, 0, "could not insert value into new array for eval operation");
			/* TODO throw error */
			return NULL;
		}

		boing_value_reference_inc(boing, args->array[1]);
	}
	else if(args->length >= 2 && args->array[1]->type == BOING_TYPE_VALUE_ARRAY)
	{
		/* use the args[1] array for the arguments instead */
		boing_value_reference_inc(boing, args->array[1]);
		eval_args = args->array[1];
	}
	else
	{
		boing_error(boing, 0, "incorrect argument format for eval operation");
		/* TODO throw error */
		return NULL;
	}
	

	/* determine if a manual stack has been passed */
	if(args->length >= 3 && args->array[2]->type == BOING_TYPE_VALUE_EXTERNAL)
		stack = (boing_value_t *)args->array[2]->external.ptr;
	else if(args->length >= 3 && args->array[2]->type != BOING_TYPE_VALUE_EXTERNAL)
	{
		boing_error(boing, 0, "type not allowed in arg2 passed to eval operation");
		/* TODO throw error */
		return NULL;
	}


	/* push new stack if wanted or omitted */
	if(args->length == 4 && args->array[3]->type == BOING_TYPE_VALUE_NUMBER && !args->array[3]->number)
	{
		new_stack = stack;
	}
	else if(args->length == 4 && args->array[3]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "type not allowed in arg3 passed to eval operation");
		/* TODO throw error */
		return NULL;
	}
	else
	{
		if(!(new_stack = boing_value_stack_push(boing, stack)))
		{
			boing_error(boing, 0, "could not create new stack in eval operation");
			/* TODO throw error */
			return NULL;
		}
	}
		


	/* place arguments into stack */

	/* add '_' */
	if(!(temp_ident = boing_value_from_str(boing, "_")))
	{
		boing_error(boing, 0, "could not create '_' identifier");
		/* TODO throw error */
		return NULL;
	}

	/* cant use the add set function. Need to write to current stack only */
	if(boing_value_table_add_set(boing, new_stack->array[1], temp_ident, eval_args, 1))
	{
		boing_error(boing, 0, "could not add '_' to new stack");
		/* TODO throw error */
		return NULL;
	}

	if(boing_value_reference_dec(boing, temp_ident))
	{
		boing_error(boing, 0, "could not refdec the '_' identifier");
		/* TODO throw error */
		return NULL;
	}


	/* add 'ARGS' */
	if(!(temp_ident = boing_value_from_str(boing, "ARGS")))
	{
		boing_error(boing, 0, "could not create 'ARGS' identifier");
		/* TODO throw error */
		return NULL;
	}

	if(boing_value_table_add_set(boing, new_stack->array[1], temp_ident, eval_args, 1))
	{
		boing_error(boing, 0, "could not add 'ARGS' to new stack");
		/* TODO throw error */
		return NULL;
	}

	if(boing_value_reference_dec(boing, temp_ident))
	{
		boing_error(boing, 0, "could not refdec the 'ARGS' identifier");
		/* TODO throw error */
		return NULL;
	}


	/* finally eval */
	if(!(ret = boing_eval_value(boing, program, args->array[0], new_stack, previous)))
	{
		boing_error(boing, 0, "could not eval value passed");
		/* TODO throw error */
		return NULL;
	}


	/* pop stack if a new one was created */
	if(new_stack != stack && !boing_value_stack_pop(boing, new_stack))
	{
		boing_error(boing, 0, "could not pop stack in eval operation");
		/* TODO throw error */
		return NULL;
	}


	if(boing_value_reference_dec(boing, eval_args))
	{
		boing_error(boing, 0, "could not refdec eval args");
		/* TODO throw error */
		return NULL;
	}
	

	return ret;
}

boing_value_t *boing_operation_external(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *new_args = NULL;


	if(args->length >= 1 && args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL)
	{
		boing_error(boing, 0, "incorrect argument supplied to arg0 in 'x' external operation");
		/* TODO throw error */
		return NULL;
	}


	if(args->length > 3)
	{
		boing_error(boing, 0, "too many arguments supplied to 'x' external operation");
		/* TODO throw error */
		return NULL;
	}

	/* verify that it is an external function rather than just data pointer */
	if(args->array[0]->external.type != BOING_EXTERNAL_FUNCTION)
	{
		boing_error(boing, 0, "arg0 is not an external function supplied to 'x' external operation");
		/* TODO throw error */
		return NULL;
	}


	/* choose how to handle the arguments */
	if(args->length == 1)
	{
		if(!(new_args = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 0)))
		{
			boing_error(boing, 0, "could not request empty args array for 'x' external operation");
			/* TODO throw error */
			return NULL;
		}
	}
	else if(args->length >= 2 && args->array[1]->type != BOING_TYPE_VALUE_ARRAY)
	{
		/* put the value into an array */
		if(!(new_args = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 1)))
		{
			boing_error(boing, 0, "could not request new args array for 'x' external operation");
			/* TODO throw error */
			return NULL;
		}

		if(boing_value_array_append_insert(boing, new_args, 0, args->array[1]))
		{
			boing_error(boing, 0, "could not insert value into new array for 'x' external operation");
			/* TODO throw error */
			return NULL;
		}

		boing_value_reference_inc(boing, args->array[1]);
	}
	else if(args->length >= 2 && args->array[1]->type == BOING_TYPE_VALUE_ARRAY)
	{
		/* use the args[1] array for the arguments instead */
		boing_value_reference_inc(boing, args->array[1]);
		new_args = args->array[1];
	}

	/* determine if a manual stack has been passed */
	if(args->length == 3 && args->array[2]->type == BOING_TYPE_VALUE_EXTERNAL)
		stack = (boing_value_t *)args->array[2]->external.ptr;
	else if(args->length == 3)
	{
		boing_error(boing, 0, "type not allowed in arg2 supplied to 'x' external operation");
		/* TODO throw error */
		return NULL;
	}

	/* call and return the result */
	if(!(ret = args->array[0]->external.fun(boing, program, stack, previous, new_args)))
	{
		boing_error(boing, 0, "external function call in 'x' external operation errored");
		/* TODO throw error */
		return NULL;
	}


	if(boing_value_reference_dec(boing, new_args))
	{
		boing_error(boing, 0, "could not refdec new arguments passed to external function");
		/* TODO throw error */
		return NULL;
	}


	return ret;
}

boing_value_t *boing_operation_index(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	size_t i, begin = 0, end = 0;


	/* verify arguments */
	if(args->length < 2 || args->length > 3)
	{
		boing_error(boing, 0, "too few or too many arguments passed to index operation");
		/* TODO throw error */
		return NULL;
	}

	if(args->array[0]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "not an indexable type in arg0 of index operation");
		/* TODO throw error */
		return NULL;
	}

	if(args->array[1]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "cannot use arg1 to index arg0 as it is not a numeric type");
		/* TODO throw error */
		return NULL;
	}

	if(args->length == 3 && args->array[2]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "cannot use arg2 to range index arg0 as it is not a numeric type");
		/* TODO throw error */
		return NULL;
	}


	/* perform action depending on arguments */

	if(args->length == 2)
	{
		if(args->array[1]->number < 0.0)
		{
			boing_error(boing, 0, "arg1 is below arg0 array bounds index operation");
			/* TODO throw error */
			return NULL;
		}

		if(args->array[1]->number > args->array[0]->length)
		{
			boing_error(boing, 0, "arg1 is above arg0 array bounds index operation");
			/* TODO throw error */
			return NULL;
		}

		/* actually index */
		if(!(ret = boing_value_array_index(boing, args->array[0], (size_t)args->array[1]->number)))
		{
			boing_error(boing, 0, "could not index arg0 in index operation");
			/* TODO throw error */
			return NULL;
		}

		boing_value_reference_inc(boing, ret);
	}
	else if(args->length == 3)
	{
		end = args->array[0]->length - 1;


		/* verify that the arguments are larger and smaller */

		/* arg1 */
		if(args->array[1]->number != -1.0 && args->array[1]->number >= 0.0)
		{
			/* check if its out of bounds */
			if((begin = args->array[1]->number) > args->array[0]->length)
			{
				boing_error(boing, 0, "could not index range arg0 because arg1 was above arg0 bounds");
				/* TODO throw error */
				return NULL;
			}
		}
		else if(args->array[1]->number != -1.0 && args->array[1]->number < 0.0)
		{
			boing_error(boing, 0, "could not index range arg0 because arg1 was below arg0 bounds and not -1");
			/* TODO throw error */
			return NULL;
		}

		/* arg2 */
		if(args->array[2]->number != -1.0 && args->array[2]->number >= 0.0)
		{
			/* check if its out of bounds */
			if((end = args->array[2]->number) > args->array[0]->length)
			{
				boing_error(boing, 0, "could not index range arg2 because arg1 was above arg0 bounds");
				/* TODO throw error */
				return NULL;
			}
		}
		else if(args->array[2]->number != -1.0 && args->array[2]->number < 0.0)
		{
			boing_error(boing, 0, "could not index range arg0 because arg2 was below arg0 bounds and not -1");
			/* TODO throw error */
			return NULL;
		}

		/* make sure begin is not greater than end */
		if(begin > end)
		{
			boing_error(boing, 0, "could not index range arg0 because arg2 was smaller that arg1");
			/* TODO throw error */
			return NULL;
		}


		/* return range now */

		if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 0)))
		{
			boing_error(boing, 0, "could not index request range array");
			/* TODO throw error */
			return NULL;
		}

		for(i = begin; i <= end; ++i)
		{
			if(boing_value_array_append_insert(boing, ret, ret->length, args->array[0]->array[i]))
			{
				boing_error(boing, 0, "could not append arg0 array element to index range");
				/* TODO throw error */
				return NULL;
			}

			boing_value_reference_inc(boing, args->array[0]->array[i]);
		}
	}

	return ret;
}

boing_value_t *boing_operation_if(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *new_stack = NULL, *conditional_eval = NULL, *runtime_block = NULL, *temp = NULL;


	/* before anything, check if the previous is nonzero or not */
	if(previous->type != BOING_TYPE_VALUE_NUMBER || (previous->type == BOING_TYPE_VALUE_NUMBER && previous->number))
	{
		/* return 1 to propigate through if chain if it exists */
		if(!(ret = boing_value_from_double(boing, 1.0)))
		{
			boing_error(boing, 0, "could not create return value to propigate through if operation");
			/* TODO throw error */
			return NULL;
		}

		return ret;
	}

	/* this if operation is allowed to test the conditional and possibly execute */
	if(args->length != 2)
	{
		boing_error(boing, 0, "if operation has too many arguments");
		/* TODO throw error */
		return NULL;
	}


	/* only need to eval arg0 */
	if(!(conditional_eval = boing_eval_value(boing, program, args->array[0], stack, previous)))
	{
		boing_error(boing, 0, "could not eval arg0 conditional in if operation");
		/* TODO throw error */
		return NULL;
	}


	/* if the conditional arg0 is anything but numeric 0, it will run. Otherwise, it will skip and propigate a 0 */
	if(conditional_eval->type == BOING_TYPE_VALUE_NUMBER && !conditional_eval->number)
	{
		/* return 0 to propigate through if chain if it exists */
		if(!(ret = boing_value_from_double(boing, 0.0)))
		{
			boing_error(boing, 0, "could not create return value to propigate through if operation");
			/* TODO throw error */
			return NULL;
		}

		/* cleanup conditional */
		if(boing_value_reference_dec(boing, conditional_eval))
		{
			boing_error(boing, 0, "could not refdec the conditional eval value from if operation arg0");
			/* TODO throw error */
			return NULL;
		}

		return ret;
	}

	/* cleanup conditional */
	if(boing_value_reference_dec(boing, conditional_eval))
	{
		boing_error(boing, 0, "could not refdec the conditional eval value from if operation arg0");
		/* TODO throw error */
		return NULL;
	}

	/* can absolutely execute now */
	
	/* push the new stack */
	if(!(new_stack = boing_value_stack_push(boing, stack)))
	{
		boing_error(boing, 0, "could not create new stack in if operation");
		/* TODO throw error */
		return NULL;
	}

	/* eval everything */

	/* TODO: only eval once by looking into the '{' operation arguments */

	/* NOTE: because the eval before this operation would normally eval the arguments, this has
	to eval twice. Once to get the {} -> () operation, and then another time to eval the () */
	if(!(temp = boing_eval_value(boing, program, args->array[1], new_stack, previous)))
	{
		boing_error(boing, 0, "could not eval value passed in if operation arg1 to a runtime block");
		/* TODO throw error */
		return NULL;
	}

	if(!(runtime_block = boing_eval_value(boing, program, temp, new_stack, previous)))
	{
		boing_error(boing, 0, "could not eval value passed in if operation arg1");
		/* TODO throw error */
		return NULL;
	}



	/* return 1 to propigate through if chain if it exists */
	if(!(ret = boing_value_from_double(boing, 1.0)))
	{
		boing_error(boing, 0, "could not create return value to propigate through if operation");
		/* TODO throw error */
		return NULL;
	}
	

	/* the returned value is thrown away because if operation stuff has to propigate */
	if(boing_value_reference_dec(boing, runtime_block))
	{
		boing_error(boing, 0, "could not refdec the returned value from if operation arg1");
		/* TODO throw error */
		return NULL;
	}

	/* the returned value from the {} operation needs to be destroyed */
	if(boing_value_reference_dec(boing, temp))
	{
		boing_error(boing, 0, "could not refdec the returned value from if operation arg1 from the '{' operation");
		/* TODO throw error */
		return NULL;
	}


	/* pop the new stack */
	if(!boing_value_stack_pop(boing, new_stack))
	{
		boing_error(boing, 0, "could not pop stack in if operation");
		/* TODO throw error */
		return NULL;
	}
	

	return ret;
}

boing_value_t *boing_operation_equality(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length != 2)
	{
		boing_error(boing, 0, "could not compare values because more or less than 2 arguments");
		/* TODO throw error */
		return NULL;
	}

	/* compare for equality only */
	if(boing_value_compare(boing, args->array[0], args->array[1]) == BOING_COMPARISON_EQUAL)
	{
		if(!(ret = boing_value_from_double(boing, 1.0)))
		{
			boing_error(boing, 0, "error returning values from value comparison");
			/* TODO throw error */
			return NULL;
		}
	}
	else
	{
		if(!(ret = boing_value_from_double(boing, 0.0)))
		{
			boing_error(boing, 0, "error returning values from value comparison");
			/* TODO throw error */
			return NULL;
		}
	}

	
	return ret;
}

boing_value_t *boing_operation_lessthan(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length != 2)
	{
		boing_error(boing, 0, "could not compare values because more or less than 2 arguments");
		/* TODO throw error */
		return NULL;
	}

	/* compare for less than only */
	if(boing_value_compare(boing, args->array[0], args->array[1]) == BOING_COMPARISON_LESS_THAN)
	{
		if(!(ret = boing_value_from_double(boing, 1.0)))
		{
			boing_error(boing, 0, "error returning values from value comparison");
			/* TODO throw error */
			return NULL;
		}
	}
	else
	{
		if(!(ret = boing_value_from_double(boing, 0.0)))
		{
			boing_error(boing, 0, "error returning values from value comparison");
			/* TODO throw error */
			return NULL;
		}
	}

	
	return ret;
}

boing_value_t *boing_operation_greaterthan(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length != 2)
	{
		boing_error(boing, 0, "could not compare values because more or less than 2 arguments");
		/* TODO throw error */
		return NULL;
	}

	/* compare for less than only */
	if(boing_value_compare(boing, args->array[0], args->array[1]) == BOING_COMPARISON_GREATER_THAN)
	{
		if(!(ret = boing_value_from_double(boing, 1.0)))
		{
			boing_error(boing, 0, "error returning values from value comparison");
			/* TODO throw error */
			return NULL;
		}
	}
	else
	{
		if(!(ret = boing_value_from_double(boing, 0.0)))
		{
			boing_error(boing, 0, "error returning values from value comparison");
			/* TODO throw error */
			return NULL;
		}
	}

	
	return ret;
}

boing_value_t *boing_operation_loop(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *new_stack = NULL, *conditional_eval = NULL, *runtime_block = NULL, *temp = NULL;
	double i;

	/* make sure arguments follow what it wants */
	if(args->length != 2)
	{
		boing_error(boing, 0, "loop operation can only take 2 arguments");
		/* TOD throw error */
		return NULL;
	}


	/* push the new stack */
	if(!(new_stack = boing_value_stack_push(boing, stack)))
	{
		boing_error(boing, 0, "could not create new stack in if operation");
		/* TODO throw error */
		return NULL;
	}


	/* loop only while the condition is nonzero */
	for(i = 0;;++i)
	{
		/* need to eval arg0 each time */
		if(!(conditional_eval = boing_eval_value(boing, program, args->array[0], new_stack, previous)))
		{
			boing_error(boing, 0, "could not eval arg0 conditional in loop operation");
			/* TODO throw error */
			return NULL;
		}


		/* if the conditional arg0 is anything but numeric 0, it will run. Otherwise, it will break */
		if(conditional_eval->type == BOING_TYPE_VALUE_NUMBER && !conditional_eval->number)
		{
			/* cleanup conditional */
			if(boing_value_reference_dec(boing, conditional_eval))
			{
				boing_error(boing, 0, "could not refdec the conditional eval value from loop operation arg0");
				/* TODO throw error */
				return NULL;
			}

			break;
		}

		/* cleanup conditional */
		if(boing_value_reference_dec(boing, conditional_eval))
		{
			boing_error(boing, 0, "could not refdec the conditional eval value from loop operation arg0");
			/* TODO throw error */
			return NULL;
		}

		/* eval everything */

		/* NOTE: because the eval before this operation would normally eval the arguments, this has
		to eval twice. Once to get the {} -> () operation, and then another time to eval the () */
		if(!(temp = boing_eval_value(boing, program, args->array[1], new_stack, previous)))
		{
			boing_error(boing, 0, "could not eval value passed in loop operation arg1 to a runtime block");
			/* TODO throw error */
			return NULL;
		}

		if(!(runtime_block = boing_eval_value(boing, program, temp, new_stack, previous)))
		{
			boing_error(boing, 0, "could not eval value passed in loop operation arg1");
			/* TODO throw error */
			return NULL;
		}


		/* the returned value is thrown away because loop operation stuff has to return loop times */
		if(boing_value_reference_dec(boing, runtime_block))
		{
			boing_error(boing, 0, "could not refdec the returned eval value from loop operation arg1");
			/* TODO throw error */
			return NULL;
		}

		/* the returned value from the {} operation needs to be destroyed */
		if(boing_value_reference_dec(boing, temp))
		{
			boing_error(boing, 0, "could not refdec the returned value from loop operation arg1 from the '{' operation");
			/* TODO throw error */
			return NULL;
		}
	}
	


	if(!(ret = boing_value_from_double(boing, i)))
	{
		boing_error(boing, 0, "could not create return value to propigate through if operation");
		/* TODO throw error */
		return NULL;
	}


	/* pop the new stack */
	if(!boing_value_stack_pop(boing, new_stack))
	{
		boing_error(boing, 0, "could not pop stack in if operation");
		/* TODO throw error */
		return NULL;
	}
	

	return ret;
}

boing_value_t *boing_operation_sizeof(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;

	
	/* only accepts a single argument */
	if(args->length != 1)
	{
		boing_error(boing, 0, "the sizeof operation 'z' only takes a single argument");
		/* TODO throw error */
		return NULL;
	}


	if(!(ret = boing_value_from_double(boing, 1.0)))
	{
		boing_error(boing, 0, "could not request number to return in sizeof operation 'z'");
		/* TODO throw error */
		return NULL;
	}


	/* only arrays and operations have a size more than 1 */
	switch(args->array[0]->type)
	{
		case BOING_TYPE_VALUE_ARRAY:
			ret->number = (double)args->array[0]->length;
		break;
		/* returns the number of arguments */
		case BOING_TYPE_VALUE_OPERATION:
			ret->number = (double)args->array[0]->operation.args_value->length;
		break;

	}


	return ret;
}

boing_value_t *boing_operation_type(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	/* make sure valid arguments */
	if(args->length == 2 && args->array[1]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "the type operation 'y' only takes a numeric type in arg1 if 2 args provided to cast arg0");
		/* TODO throw error */
		return NULL;
	}
	else if(args->length != 1 && args->length != 2)
	{
		boing_error(boing, 0, "the type operation 'y' only takes 2 arguments at maximum, 1 argument at minimum");
		/* TODO throw error */
		return NULL;
	}


	/* determine functionality: type get or cast */
	if(args->length == 1)
	{
		if(!(ret = boing_value_from_double(boing, args->array[0]->type)))
		{
			boing_error(boing, 0, "could not request number to return in the type operation 'y'");
			/* TODO throw error */
			return NULL;
		}
	}
	else if(args->length == 2)
	{
		if(!(ret = boing_value_cast(boing, args->array[0], args->array[1]->number)))
		{
			boing_error(boing, 0, "could not cast type in the type operation 'y'");
			/* TODO throw error */
			return NULL;
		}
	}


	return ret;
}

boing_value_t *boing_operation_copy(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	/* only accepts a single argument */
	if(args->length != 1)
	{
		boing_error(boing, 0, "the copy operation 'c' only takes a single argument");
		/* TODO throw error */
		return NULL;
	}


	if(!(ret = boing_value_copy(boing, args->array[0])))
	{
		boing_error(boing, 0, "could not copy value to return in copy operation 'c'");
		/* TODO throw error */
		return NULL;
	}


	return ret;
}

boing_value_t *boing_operation_inc(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length == 1 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER)
	{
		ret = args->array[0];
		boing_value_reference_inc(boing, ret);

		++ret->number;
			
	}
	else if(args->length == 2 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER && args->array[1]->type == BOING_TYPE_VALUE_NUMBER)
	{
		ret = args->array[0];
		boing_value_reference_inc(boing, ret);

		ret->number += args->array[1]->number;
	}
	else
	{
		boing_error(boing, 0, "the increment operation 'n' only takes 1 or 2 numeric arguments");
		/* TODO throw error */
		return NULL;
	}


	return ret;
}

boing_value_t *boing_operation_dec(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length == 1 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER)
	{
		ret = args->array[0];
		boing_value_reference_inc(boing, ret);

		--ret->number;
			
	}
	else if(args->length == 2 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER && args->array[1]->type == BOING_TYPE_VALUE_NUMBER)
	{
		ret = args->array[0];
		boing_value_reference_inc(boing, ret);

		ret->number -= args->array[1]->number;
	}
	else
	{
		boing_error(boing, 0, "the decrement operation 'd' only takes 1 or 2 numeric arguments");
		/* TODO throw error */
		return NULL;
	}


	return ret;
}

boing_value_t *boing_operation_table(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	double exists = 0.0;


	/* verify arguments */
	if(args->length != 2 && args->length != 3)
	{
		boing_error(boing, 0, "the table operation 't' expects 2 or 3 arguments only");
		/* TODO throw error */
		return NULL;
	}

	if(args->array[0]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "the table operation 't' expects an array/table as arg0");
		/* TODO throw error */
		return NULL;
	}


	/* determine function */
	if(args->length == 2)
	{
		/* search table for key */
		if(!(ret = boing_value_table_get(boing, args->array[0], args->array[1])))
		{
			/* not an error. Just return 0 */
			if(!(ret = boing_value_from_double(boing, 0)))
			{
				boing_error(boing, 0, "could not request numeric return value in table operation 't'");
				/* TODO throw error */
				return NULL;
			}

			/* do need to return early to avoid refinc the number value */
			return ret;
		}

		boing_value_reference_inc(boing, ret);
	}
	else if(args->length == 3)
	{
		/* check if it already exists to return 1 or 0 */
		if(boing_value_table_get(boing, args->array[0], args->array[1]))
			exists = 1.0;

		if(boing_value_table_add_set(boing, args->array[0], args->array[1], args->array[2], 1))
		{
			boing_error(boing, 0, "could not set/add row in table operation 't'");
			/* TODO throw error */
			return NULL;
		}

		if(!(ret = boing_value_from_double(boing, exists)))
		{
			boing_error(boing, 0, "could not request numeric return value in table operation 't'");
			/* TODO throw error */
			return NULL;
		}
	}


	return ret;
}

boing_value_t *boing_operation_hash(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length == 1)
	{
		if(!(ret = boing_value_from_double(boing, (double)boing_value_hash(boing, args->array[0]))))
		{
			boing_error(boing, 0, "could not request numeric return value in hash operation 'h'");
			/* TODO throw error */
			return NULL;
		}
	}
	else
	{
		boing_error(boing, 0, "the hash operation 'h' expects just a single argument");
		/* TODO throw error */
		return NULL;
	}


	return ret;
}

boing_value_t *boing_operation_stack(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	size_t i;
	boing_value_t *ret = NULL, *temp = NULL;


	if(args->length == 1 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER)
	{
		if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_EXTERNAL, 0)))
		{
			boing_error(boing, 0, "could not request external return value in stack operation 'k'");
			/* TODO throw error */
			return NULL;
		}

		ret->external.type = BOING_EXTERNAL_POINTER;
		ret->external.destroy_cb = &boing_external_decrement_value_cleanup;
		ret->external.reference_inc_cb = &boing_external_increment_value;


		if(args->array[0]->number == -1) /* new stack */
		{
			/* setup a blank stack */
			if(!(ret->external.ptr = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 2)))
			{
				boing_error(boing, 0, "could not request array return value in stack operation 'k'");
				/* TODO throw error */
				return NULL;
			}

			if(!(temp = boing_value_from_double(boing, 0)))
			{
				boing_error(boing, 0, "could not request numeric 0 root in stack operation 'k'");
				/* TODO throw error */
				return NULL;
			}

			if(boing_value_array_append_insert(boing, (boing_value_t *)ret->external.ptr, 0, temp))
			{
				boing_error(boing, 0, "could not appent numeric 0 root in stack operation 'k'");
				/* TODO throw error */
				return NULL;
			}

			if(!(temp = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 0)))
			{
				boing_error(boing, 0, "could not request empty identifier table root in stack operation 'k'");
				/* TODO throw error */
				return NULL;
			}

			if(boing_value_array_append_insert(boing, (boing_value_t *)ret->external.ptr, 1, temp))
			{
				boing_error(boing, 0, "could not append empty identifier table root in stack operation 'k'");
				/* TODO throw error */
				return NULL;
			}
			


			if(boing_root_stack_init(boing, (boing_value_t *)ret->external.ptr))
			{
				boing_error(boing, 0, "could not initialize new stack in stack operation 'k'");
				/* TODO throw error */
				return NULL;
			}
		}
		else if(!args->array[0]->number) /* root of current stack */
		{
			if(!(ret->external.ptr = boing_value_stack_get_root(boing, stack)))
			{
				boing_error(boing, 0, "could not get root stack in stack operation 'k'");
				/* TODO throw error */
				return NULL;
			}

			boing_value_reference_inc(boing, (boing_value_t *)ret->external.ptr);
		}
		else if(args->array[0]->number > 0)
		{
			for(i = 0; i < args->array[0]->number; ++i) /* TODO extend this because it starts in the stack _just_ created for this op */
			{
				if(i + 1 < args->array[0]->number && stack->array[0]->type == BOING_TYPE_VALUE_NUMBER)
				{
					boing_error(boing, 0, "stack traversal out of range in stack operation 'k'");
					/* TODO throw error */
					return NULL;
				}

				stack = stack->array[0];
			}
			
			ret->external.ptr = stack;
			boing_value_reference_inc(boing, (boing_value_t *)ret->external.ptr);
		}
		else
		{
			boing_error(boing, 0, "incorrect argument range provided to the stack operation 'k'");
			/* TODO throw error */
			return NULL;
		}

	}
	else
	{
		boing_error(boing, 0, "the stack operation 'k' expects just a single numeric argument");
		/* TODO throw error */
		return NULL;
	}


	return ret;
}

boing_value_t *boing_operation_file(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	size_t size = 0, start = 0, end = 0;
	uint8_t *file = NULL, previous_type = -1;
	char *path = NULL;


	/* handle plain read and write */
	if(args->length < 1 || args->length > 3 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "bad arguments provided to file operation 'o'");
		/* TODO throw error */
		return NULL;
	}

	if(!(path = boing_str_from_value_array(boing, args->array[0])))
	{
		boing_error(boing, 0, "could not turn path in arg0 into string in file operation 'o'");
		/* TODO throw error */
		return NULL;
	}


	if(args->length == 3)
	{
		if(args->array[1]->type != BOING_TYPE_VALUE_NUMBER || args->array[2]->type != BOING_TYPE_VALUE_NUMBER)
		{
			boing_error(boing, 0, "arg1 and arg2 expected type numeric in file operation 'o'");
			/* TODO throw error */
			return NULL;
		}

		start = args->array[2]->number;
		end = args->array[2]->number;
	}
	
	if(args->length == 1 || args->length == 3)
	{
		/* read file, and if it exists, turn it into a number array */

		if(!(file = boing_read_file(boing, path, &size, start, end)))
		{
			if(!(ret = boing_value_from_double(boing, 0.0))) /* just return 0 */
			{
				boing_error(boing, 0, "could not make numeric value in file operation 'o'");
				boing_str_release(boing, path);
				/* TODO throw error */
				return NULL;
			}
		}

		if(file && !(ret = boing_value_from_strn(boing, file, size)))
		{
			boing_error(boing, 0, "could not make file data array in file operation 'o'");
			boing_str_release(boing, path);
			if(file)
				boing_str_release(boing, file);
			/* TODO throw error */
			return NULL;
		}
	}
	else if(args->length == 2)
	{
		/* write to file */
		
		switch(args->array[1]->type)
		{
			case BOING_TYPE_VALUE_NUMBER:
				if(!(file = boing_str_from_value_readable(boing, args->array[1], &previous_type, BOING_VALUE_STRING_READABLE, 0)))
				{
					boing_error(boing, 0, "could not make file data array in file operation 'o'");
					boing_str_release(boing, path);
					/* TODO throw error */
					return NULL;
				}

				size = strlen(file);
			break;
			case BOING_TYPE_VALUE_EXTERNAL:
				if(!(file = boing_str_from_value_readable(boing, args->array[1], &previous_type, BOING_VALUE_STRING_READABLE, 0)))
				{
					boing_error(boing, 0, "could not make file data array in file operation 'o'");
					boing_str_release(boing, path);
					/* TODO throw error */
					return NULL;
				}

				size = strlen(file);
			break;
			case BOING_TYPE_VALUE_OPERATION:
				if(!(file = boing_str_from_value_readable(boing, args->array[1], &previous_type, BOING_VALUE_STRING_READABLE, 0)))
				{
					boing_error(boing, 0, "could not make file data array in file operation 'o'");
					boing_str_release(boing, path);
					/* TODO throw error */
					return NULL;
				}

				size = strlen(file);
			break;
			case BOING_TYPE_VALUE_ARRAY:
				if(!(file = boing_str_from_value_array(boing, args->array[1])))
				{
					boing_error(boing, 0, "could not make file data array in file operation 'o'");
					boing_str_release(boing, path);
					/* TODO throw error */
					return NULL;
				}

				size = args->array[1]->length;
			break;
		}

		if(size != boing_write_file(boing, path, file, size))
		{
			boing_error(boing, 0, "could not write to file in file operation 'o'");
			boing_str_release(boing, path);
			boing_str_release(boing, file);
			/* TODO throw error */
			return NULL;
		}

		/* need to return something so return 1 */
		if(!(ret = boing_value_from_double(boing, 1.0)))
		{
			boing_error(boing, 0, "could not make numeric value in file operation 'o'");
			boing_str_release(boing, path);
			boing_str_release(boing, file);
			/* TODO throw error */
			return NULL;
		}
	}
	

	if(file && boing_str_release(boing, file))
	{
		boing_error(boing, 0, "could not release file string in file operation 'o'");
		/* TODO throw error */
		return NULL;
	}

	if(boing_str_release(boing, path))
	{
		boing_error(boing, 0, "could not release string path in file operation 'o'");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

boing_value_t *boing_operation_import(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *script = NULL, *temp = NULL, *ast = NULL;
	char *path = NULL, *file = NULL, *temp_file = NULL;
	boing_script_t *script_data = NULL;
	size_t line = 1, eval = 0;


	if(args->length < 1 || args->length > 2 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "too few or too many arguments and arg0 expected type array in import/parse operation 'm'");
		/* TODO throw error */
		return NULL;
	}



	/* import vs parse vs invalid argument style */
	if(args->length == 1)
	{
		if(!(path = boing_str_from_value_array(boing, args->array[0])))
		{
			boing_error(boing, 0, "could not create path string in import/parse operation 'm'");
			/* TODO throw error */
			return NULL;
		}

		/* NOTE: handle importing modules vs files in the import_file function */	
		if(!(file = boing_import_file(boing, stack, path)))
		{
			boing_error(boing, 0, "could not import file in import/parse operation 'm'");
			/* TODO throw error */
			return NULL;
		}

		eval = 1;
	}
	else if(args->length == 2 && args->array[1]->type == BOING_TYPE_VALUE_ARRAY)
	{
		if(!(path = boing_str_from_value_array(boing, args->array[1])))
		{
			boing_error(boing, 0, "could not create path string in import/parse operation 'm'");
			/* TODO throw error */
			return NULL;
		}

		/* NOTE: handle importing modules vs files in the import function */	
		if(!(file = boing_str_from_value_array(boing, args->array[0])))
		{
			boing_error(boing, 0, "could not convert arg0 string in import/parse operation 'm'");
			/* TODO throw error */
			return NULL;
		}
	}
	else
	{
		boing_error(boing, 0, "expected array for arg1 in import/parse operation 'm'");
		/* TODO throw error */
		return NULL;
	}


	temp_file = file;


	/* setup the script data */

	/* might be a bad idea, but the data returned should at least be the size of uint8_t
	so if anything, the issue might be too much data used */
	if(!(script_data = (boing_script_t *)boing_str_request(boing, sizeof(boing_script_t))))
	{
		boing_error(boing, 0, "could not use the string pool to request a script struct");
		boing_str_release(boing, file);
		/* TODO throw error */
		return NULL;
	}

	if(!(script_data->name = boing_str_ndup(boing, path, strlen(path))))
	{
		boing_error(boing, 0, "could not duplicate the script name");
		boing_str_release(boing, file);
		/* TODO throw error */
		return NULL;
	}

	if(!(script_data->script = boing_str_ndup(boing, file, strlen(file))))
	{
		boing_error(boing, 0, "could not duplicate the script text");
		boing_str_release(boing, file);
		/* TODO throw error */
		return NULL;
	}

	/* turn this into an external value so it gets garbage collected */
	if(!(script = boing_value_request(boing, BOING_TYPE_VALUE_EXTERNAL, 0)))
	{
		boing_error(boing, 0, "could not request external value for script struct");
		boing_str_release(boing, file);
		/* TODO throw error */
		return NULL;
	}

	script->external.type = BOING_EXTERNAL_POINTER;
	script->external.destroy_cb = &boing_script_data_cleanup;
	script->external.ptr = script_data;

	/* parse script */
	if(!(temp = boing_parse_from_string(boing, NULL, NULL, script, &temp_file, &line)))
	{
		boing_error(boing, 0, "could not parse script in import/parse operation 'm'");
		boing_value_reference_dec(boing, script);
		boing_str_release(boing, file);
		/* TODO throw error */
		return NULL;
	}

	if(boing_value_reference_dec(boing, script))
	{
		boing_error(boing, 0, "could not refdec script line container in import/parse operation 'm'");
		/* TODO throw error */
		return NULL;
	}

	if(boing_str_release(boing, file))
	{
		boing_error(boing, 0, "could not release script text in import/parse operation 'm'");
		/* TODO throw error */
		return NULL;
	}

	/* put ast in runtime op */
	if(!(ast = boing_value_request(boing, BOING_TYPE_VALUE_OPERATION, 0)))
	{
		boing_error(boing, 0, "could not create base AST in import/parse operation 'm'");
		/* TODO throw error */
		return NULL;
	}

	ast->operation.op = '(';

	if(boing_value_set(boing, ast->operation.args_value, temp))
	{
		boing_error(boing, 0, "could not set AST as runtime operation in import/parse operation 'm'");
		/* TODO throw error */
		return NULL;
	}

	if(boing_value_reference_dec(boing, temp))
	{
		boing_error(boing, 0, "could not refdec parsed AST in import/parse operation 'm'");
		/* TODO throw error */
		return NULL;
	}

	
	/* eval the ast if allowed */
	if(eval && !(ret = boing_eval_value(boing, boing->program, ast, stack, previous)))
	{
		boing_error(boing, 0, "could not eval AST in import/parse operation 'm'");
		/* TODO throw error */
		return NULL;
	}
	else if(!eval)
		ret = ast;


	/* cleanup */
	if(eval && boing_value_reference_dec(boing, ast))
	{
		boing_error(boing, 0, "could not refdec container AST in import/parse operation 'm'");
		/* TODO throw error */
		return NULL;
	}


	if(boing_str_release(boing, path))
	{
		boing_error(boing, 0, "could not release string path in import/parse operation 'm'");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

boing_value_t *boing_operation_search(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	size_t pos = 0;


	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "the search operation 'a' expects at least 2 arguments and arg0 has to be an array");
		/* TODO throw error */
		return NULL;
	}

	if(boing_value_array_search(boing, args->array[0], args->array[1], 0, &pos))
	{
		/* may possibly encounter issues with high numbers not being able to be represented as a bouble */
		if(!(ret = boing_value_from_double(boing, (double)pos)))
		{
			boing_error(boing, 0, "could not create position value in search operation 'a'");
			/* TODO throw error */
			return NULL;
		}
	}
	else if(!(ret = boing_value_from_double(boing, -1.0)))
	{
		boing_error(boing, 0, "could not create position value in search operation 'a'");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

boing_value_t *boing_operation_random(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(!args->length)
	{
		if(!(ret = boing_value_from_double(boing, (double)rand())))
		{
			boing_error(boing, 0, "could not create a random number value in the random operation 'g'");
			/* TODO throw error */
			return NULL;
		}
	}
	else if(args->length == 1 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER)
	{
		if(!(ret = boing_value_from_double(boing, (1 + ((double)rand() / (double)RAND_MAX) * abs(args->array[0]->number)))))
		{
			boing_error(boing, 0, "could not create a random number value in the random operation 'g'");
			/* TODO throw error */
			return NULL;
		}

		/* determine if its a negative or positive limit */
		if(args->array[0]->number < 0.0)
			ret->number = -ret->number;
	}
	else if(args->length == 2 && args->array[0]->type == BOING_TYPE_VALUE_NUMBER && args->array[1]->type == BOING_TYPE_VALUE_NUMBER)
	{
		if(!(ret = boing_value_from_double(boing, 1 + args->array[0]->number + ((double)rand() / (double)RAND_MAX) * (fabs(args->array[1]->number - args->array[0]->number)))))
		{
			boing_error(boing, 0, "could not create a random number value in the random operation 'g'");
			/* TODO throw error */
			return NULL;
		}
	}
	else
	{
		boing_error(boing, 0, "the random operation 'g' takes only 2 number arguments at max");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

boing_value_t *boing_operation_sort(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length == 0)
	{
		boing_error(boing, 0, "the sort operation 'q' takes at least 1 argument");
		/* TODO throw error */
		return NULL;
	}
	else if(args->length == 1 && args->array[0]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "the sort operation 'q' requires that arg0 be an array if it is the only argument");
		/* TODO throw error */
		return NULL;
	}


	/* determine whether or not its sorting an array or sorting the arguments */
	if(args->length == 1)
	{
		if(!(ret = boing_sort_array(boing, args->array[0])))
		{
			boing_error(boing, 0, "could not create sorted array in the sort operation 'q'");
			/* TODO throw error */
			return NULL;
		}
	}
	else
	{
		if(!(ret = boing_sort_array(boing, args)))
		{
			boing_error(boing, 0, "could not create sorted array in the sort operation 'q'");
			/* TODO throw error */
			return NULL;
		}
	}

	return ret;
}

boing_value_t *boing_operation_array_setup(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	size_t i;
	boing_value_t *ret = NULL, *eval = NULL, *new_stack = NULL, *temp = NULL, *iterator = NULL, *new_args = NULL;


	if(args->length < 1 || args->array[0]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "the array setup operation 'u' requires that arg0 be a number if it is the only argument");
		/* TODO throw error */
		return NULL;
	}
	else if(args->length > 2 || args->array[0]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "the array setup operation 'u' requires that arg0 be a number and not greater than 2 arguments");
		/* TODO throw error */
		return NULL;
	}


	if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, args->array[0]->number)))
	{
		boing_error(boing, 0, "could not request array");
		/* TODO throw error */
		return NULL;
	}

	if(!(iterator = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not request number for iterator");
		/* TODO throw error */
		return NULL;
	}


	/* just eval 0 if no initializer provided */
	if(args->length == 1)
	{
		if(!(eval = boing_value_from_double(boing, 0.0)))
		{
			boing_error(boing, 0, "could not request value to eval");
			boing_value_reference_dec(boing, iterator);
			/* TODO throw error */
			return NULL;
		}
	}
	else
	{
		eval = args->array[1];
		boing_value_reference_inc(boing, eval);
	}


	/* push the new stack */
	if(!(new_stack = boing_value_stack_push(boing, stack)))
	{
		boing_error(boing, 0, "could not create new stack in array setup operation 'u'");
		boing_value_reference_dec(boing, iterator);
		/* TODO throw error */
		return NULL;
	}

	/* put the iterator into the new args array */
	if(!(new_args = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 1)))
	{
		boing_error(boing, 0, "could not create args array container for the iterator");
		boing_value_reference_dec(boing, iterator);
		return NULL;
	}

	if(boing_value_array_append_insert(boing, new_args, 0, iterator))
	{
		boing_error(boing, 0, "could not insert iterator into new args");
		boing_value_reference_dec(boing, iterator);
		boing_value_reference_dec(boing, new_args);
		return NULL;
	}

	/* add '_' and ARGS to new stack */
	if(!(temp = boing_value_from_str(boing, "_")))
	{
		boing_error(boing, 0, "could not create string for args '_' key");
		boing_value_reference_dec(boing, iterator);
		boing_value_reference_dec(boing, new_args);
		return NULL;
	}

	if(boing_value_stack_add_set(boing, new_stack, temp, new_args))
	{
		boing_error(boing, 0, "could not add args '_' key to current stack level");
		boing_value_reference_dec(boing, iterator);
		boing_value_reference_dec(boing, new_args);
		return NULL;
	}

	if(boing_value_reference_dec(boing, temp))
	{
		boing_error(boing, 0, "could not refdec '_' key");
		boing_value_reference_dec(boing, iterator);
		boing_value_reference_dec(boing, new_args);
		return NULL;
	}

	/* add ARGS to globals */
	if(!(temp = boing_value_from_str(boing, "ARGS")))
	{
		boing_error(boing, 0, "could not create string for args 'ARGS' key");
		boing_value_reference_dec(boing, iterator);
		boing_value_reference_dec(boing, new_args);
		return NULL;
	}

	if(boing_value_stack_add_set(boing, new_stack, temp, iterator))
	{
		boing_error(boing, 0, "could not add args 'ARGS' key to current stack level");
		boing_value_reference_dec(boing, iterator);
		boing_value_reference_dec(boing, new_args);
		return NULL;
	}

	if(boing_value_reference_dec(boing, temp))
	{
		boing_error(boing, 0, "could not refdec 'ARGS' key");
		boing_value_reference_dec(boing, iterator);
		boing_value_reference_dec(boing, new_args);
		return NULL;
	}


	/* eval in a loop */

	for(i = 0; i < (size_t)args->array[0]->number; ++i)
	{
		iterator->number = (double)i;

		if(!(temp = boing_eval_value(boing, boing->program, eval, new_stack, previous)))
		{
			boing_error(boing, 0, "could not eval at %lu", i);
			boing_value_reference_dec(boing, iterator);
			boing_value_reference_dec(boing, new_args);
			return NULL;
		}
		
		if(boing_value_array_append_insert(boing, ret, i, temp))
		{
			boing_error(boing, 0, "could not append value to return array at %lu", i);
			boing_value_reference_dec(boing, iterator);
			boing_value_reference_dec(boing, new_args);
			return NULL;
		}
	}


	/* cleanup */

	/* pop the new stack */
	if(!boing_value_stack_pop(boing, new_stack))
	{
		boing_error(boing, 0, "could not pop stack in array setup operation 'u'");
		/* TODO throw error */
		return NULL;
	}

	if(boing_value_reference_dec(boing, new_args))
	{
		boing_error(boing, 0, "could not refdec new args in array setup operation 'u'");
		/* TODO throw error */
		return NULL;
	}

	/* NOTE: dont need to refdec iterator because it resided in the new_args container */

	if(boing_value_reference_dec(boing, eval))
	{
		boing_error(boing, 0, "could not refdec eval in array setup operation 'u'");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}


/* =========================== boing functions ============================ */

/* util functions */

void boing_value_debug_print(boing_value_t *v, int level, FILE *stream)
{
	int i;

	#define SPACE(t) do{int ii; for(ii = 0; ii < t; ii++) fprintf(stream, "\t"); }while(0)
	if(!v)
	{
		fprintf(stream, "null value\n");
		return;
	}

	switch(v->type)
	{
		case BOING_TYPE_VALUE_ARRAY:
			SPACE(level); fprintf(stream, "[ length: %lu %lu %lu %p\n", v->length, v->references, v->allocated, v);
			for(i = 0; i < v->length; i++)
				boing_value_debug_print(v->array[i], level + 1, stream);
			SPACE(level); fprintf(stream, "]\n");
		break;
		case BOING_TYPE_VALUE_NUMBER:
			SPACE(level); fprintf(stream, "\'%f\' %lu %lu %p\n", v->number, v->references, v->allocated, v);
		break;
		case BOING_TYPE_VALUE_OPERATION:
			SPACE(level); fprintf(stream, "op: '%c'( %lu %lu %p\n", v->operation.op, v->references, v->allocated, v);
			boing_value_debug_print(v->operation.args_value, level + 1, stream);
			SPACE(level); fprintf(stream, ")\n");
		break;
		case BOING_TYPE_VALUE_EXTERNAL:
			SPACE(level); fprintf(stream, "ext: :%p %p: %lu %lu %p\n", v->external.ptr, v->external.destroy_cb, v->references, v->allocated, v);
		break;
		default:
			fprintf(stream, "impossible type\n");
	}
}

uint8_t *boing_read_file(boing_t *boing, char *path, size_t *size_read, size_t offset, size_t amount)
{
	return boing->callback.boing_read_file_cb(boing, path, size_read, offset, amount);
}

size_t boing_write_file(boing_t *boing, char *path, void *data, size_t write_size)
{
	return boing->callback.boing_write_file_cb(boing, path, data, write_size);
}

void boing_print(boing_t *boing, char *message)
{
	boing->callback.boing_print_cb(boing, message);
}

uint8_t *boing_read_input(boing_t *boing, size_t *read_size, size_t read_limit, uint8_t read_until_char)
{
	return boing->callback.boing_read_cb(boing, read_size, read_limit, read_until_char);
}

uint8_t *boing_import_file(boing_t *boing, boing_value_t *stack, char *path)
{
	return boing->callback.boing_import_cb(boing, stack, path);
}

void boing_error(boing_t *boing, int fatal, char *fmt, ...)
{
	va_list ap;
	int size;
	char buffer[512], *message = NULL;


	va_start(ap, fmt);
	size = BOING_VSNPRINTF(message, 0, fmt, ap); /* the only C99 function used */
	va_end(ap);

	/* check if small enough to use the stack buffer */
	if(size + 1 <= 512)
		message = buffer;
	else
	{
		/* this function may be called before the boing is initialized or while it's
		initializing. Do not try to access the string pool until it is initialized */

		if(!boing->pool.string.container_cleanup)
		{
			boing->callback.boing_error_cb(boing, "THE BOING_ERROR FUNCTION WAS CALLED BEFORE THE STRING POOL HAS INITIALIZED. THE FOLLOWING ERROR IS UNFORMATTED:", 0);
			boing->callback.boing_error_cb(boing, fmt, fatal);
			return;
		}

		/* if boing is initialized */

		if(!(message = boing_str_request(boing, size + 1)))
		{
			/* could not request a string. Do not format the string and only pass the unformatted str */
			boing->callback.boing_error_cb(boing, "THE BOING_ERROR FUNCTION ENCOUNTERED A CRITICAL ERROR. THE FOLLOWING ERROR IS UNFORMATTED:", 0);
			boing->callback.boing_error_cb(boing, fmt, fatal);
			boing->callback.boing_error_cb(boing, "FATAL ERROR", 1); /* if the user's error cb halts the program, its not good to pass fatal until the unformatted
			str has had a chance to print */
			return;
		}
	}

	va_start(ap, fmt);
	if(BOING_VSNPRINTF(message, size + 1, fmt, ap) < 0)
	{
		boing_str_release(boing, message);
		boing->callback.boing_error_cb(boing, "BOING_ERROR COULD NOT FORMAT STRING. THE FOLLOWING IS UNFORMATTED:", 0);
		boing->callback.boing_error_cb(boing, fmt, fatal);
		boing->callback.boing_error_cb(boing, "FATAL ERROR", 1);
	}
	va_end(ap);

	message[size] = 0;

	/* finally, pass the formatted message along */
	boing->callback.boing_error_cb(boing, message, fatal);

	if(message != buffer)
		boing_str_release(boing, message);
}

void boing_error_script_print(boing_t *boing, boing_value_t *value)
{
	size_t i, line = 1;
	boing_script_t *script = NULL;
	char found = 0, *start = NULL, *end = NULL, *temp = NULL;
	
	/* prints where the error was and the line */
	#if BOING_ENABLE_LINE_NUM
	
	if(value->script)
	{
		
		script = ((boing_script_t *)value->script->external.ptr);

		start = script->script;


		for(i = 0; i < strlen(script->script); ++i)
		{
			if(line == value->line)
			{
				/* look for end */
				for(; i < strlen(script->script); ++i)
				{
					if(script->script[i + 1] == '\n' || i + 1 >= strlen(script->script))
					{
						end = &script->script[i + 1];
						found = 1;
						break;
					}
				}
				break;
			}

			if(script->script[i] == '\n')
			{
				start = &script->script[i + 1];
				line++;
			}
		}

		/* might somehow exist outside the scrpt string so just in case still print
		the error, but omit the line text printing */
		if(found)
		{
			boing_consume_whitespace(boing, &start);

			if(!(temp = boing_str_ndup(boing, start, end - start)))
			{
				boing_error(boing, 0, "could not make string from script line");
				return;
			}
			boing_error(boing, 0, "%s:%lu:  \t\"%s\"", script->name, value->line, temp);

			if(boing_str_release(boing, temp))
			{
				boing_error(boing, 0, "could not release line string");
				return;
			}
		}
		else
		{
			boing_error(boing, 0, "%s:%lu:  \t(could not print script line because the line was past the end somehow", script->name, value->line);
		}
	}
	#endif
}

int boing_module_add(boing_t *boing, char *name, int (*module_init)(boing_t *boing, boing_module_t *module), int (*module_destroy)(boing_t *boing, boing_module_t *module), int (*module_stack_add)(boing_t *boing, boing_value_t *stack, boing_module_t *module), void *handle)
{
	boing_module_t *module = NULL;
	boing_value_t *key = NULL, *value = NULL;


	if(!(module = (boing_module_t *)boing_str_request(boing, sizeof(boing_module_t))))
	{
		boing_value_reference_dec(boing, key);
		boing_error(boing, 0, "could not allocate module struct in module '%s'", name);
		return 1;
	}

	memset(module, 0, sizeof(boing_module_t));

	module->module_init = module_init;
	module->module_stack_add = module_stack_add;
	module->module_destroy = module_destroy;
	module->handle = handle;


	if(!(key = boing_value_from_str(boing, name)))
	{
		boing_error(boing, 0, "could not create module key for '%s'", name);
		boing_str_release(boing, (char *)module);
		return 1;
	}

	/* first search if module already exists, and if it does, just do nothing */
	if(boing_value_table_get(boing, boing->modules, key))
	{
		boing_value_reference_dec(boing, key);
		/* destroy the handle as well */
		if(boing_module_cleanup_handle(boing, module))
		{
			boing_str_release(boing, (char *)module);
			return 1;
		}

		boing_str_release(boing, (char *)module);
		return 0;
	}
	
	/* need to initialize and add the module */
	if(module_init(boing, module))
	{
		boing_value_reference_dec(boing, key);
		boing_error(boing, 0, "could not initialize module '%s'", name);
		return 1;
	}


	/* make external value then toss in the table */
	if(!(value = boing_value_from_ptr(boing, module, BOING_EXTERNAL_POINTER, NULL)))
	{
		boing_value_reference_dec(boing, key);
		boing_error(boing, 0, "could not create external pointer value in module '%s'", name);
		return 1;
	}

	if(boing_value_table_add_set(boing, boing->modules, key, value, 1))
	{
		boing_value_reference_dec(boing, key);
		boing_value_reference_dec(boing, value);
		boing_error(boing, 0, "could not add module pointer in module '%s'", name);
		return 1;
	}

	/* cleanup */

	if(boing_value_reference_dec(boing, key))
	{
		boing_value_reference_dec(boing, value);
		boing_error(boing, 0, "could not refdec key in module '%s'", name);
		return 1;
	}

	if(boing_value_reference_dec(boing, value))
	{
		boing_error(boing, 0, "could not refdec value in module '%s'", name);
		return 1;
	}

	return 0;
}

int boing_module_cleanup_handle(boing_t *boing, boing_module_t *module)
{
	return boing->callback.boing_module_cleanup_cb(boing, module);
}

int boing_module_stack_add(boing_t *boing, boing_value_t *stack, char *name)
{
	boing_value_t *module = NULL, *key = NULL;


	if(!(key = boing_value_from_str(boing, name)))
	{
		boing_error(boing, 0, "could create key for module '%s'", name);
		return 1;
	}

	if(!(module = boing_value_table_get(boing, boing->modules, key)))
	{
		boing_error(boing, 0, "could not find module '%s'", name);
		boing_value_reference_dec(boing, key);
		return 1;
	}


	/* run the module stack add function */
	if(((boing_module_t *)module->external.ptr)->module_stack_add(boing, stack, ((boing_module_t *)module->external.ptr)))
	{
		boing_error(boing, 0, "could not run module stack add function for '%s'", name);
		boing_value_reference_dec(boing, key);
		return 1;
	}


	if(boing_value_reference_dec(boing, key))
	{
		boing_error(boing, 0, "could refdec key for module '%s'", name);
		return 1;
	}

	return 0;
}

int boing_root_stack_init(boing_t *boing, boing_value_t *stack)
{
	/* add the defaults and boing internals */
	

	/* global boing numbers, strings, and arrays */
	
	BOING_ADD_GLOBAL("BOING_VERSION", boing_value_from_str(boing, BOING_VERSION_STRING));
	BOING_ADD_GLOBAL("BOING_VERSION_MAJOR", boing_value_from_double(boing, BOING_VERSION_MAJOR));
	BOING_ADD_GLOBAL("BOING_VERSION_MINOR", boing_value_from_double(boing, BOING_VERSION_MINOR));
	BOING_ADD_GLOBAL("BOING_VERSION_REVISION", boing_value_from_double(boing, BOING_VERSION_REVISION));

	BOING_ADD_GLOBAL("ARRAY", boing_value_from_double(boing, BOING_TYPE_VALUE_ARRAY));
	BOING_ADD_GLOBAL("NUMBER", boing_value_from_double(boing, BOING_TYPE_VALUE_NUMBER));
	BOING_ADD_GLOBAL("OPERATION", boing_value_from_double(boing, BOING_TYPE_VALUE_OPERATION));
	BOING_ADD_GLOBAL("EXTERNAL", boing_value_from_double(boing, BOING_TYPE_VALUE_EXTERNAL));

	BOING_ADD_GLOBAL("TRUE", boing_value_from_double(boing, 1.0));
	BOING_ADD_GLOBAL("FALSE", boing_value_from_double(boing, 0.0));

	BOING_ADD_GLOBAL("M_PI", boing_value_from_double(boing, 3.14159265));
	BOING_ADD_GLOBAL("NULL", boing_value_from_ptr(boing, NULL, BOING_EXTERNAL_POINTER, NULL));

	/* global external functions as the standard library */

	/* boing api */
	BOING_ADD_GLOBAL("BOING_POOL_STRING_SIZE_GET", boing_value_from_ptr(boing, &boing_std_pool_string_get_block_alloc, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_STRING_SIZE_SET", boing_value_from_ptr(boing, &boing_std_pool_string_set_block_alloc, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_STRING_AMOUNT_GET", boing_value_from_ptr(boing, &boing_std_pool_string_get_amount, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_STRING_FREE_GET", boing_value_from_ptr(boing, &boing_std_pool_string_get_free, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_STRING_MAX_FREE_GET", boing_value_from_ptr(boing, &boing_std_pool_string_get_max_free, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_STRING_MAX_FREE_SET", boing_value_from_ptr(boing, &boing_std_pool_string_set_max_free, BOING_EXTERNAL_FUNCTION, NULL));

	BOING_ADD_GLOBAL("BOING_POOL_ARRAY_SIZE_GET", boing_value_from_ptr(boing, &boing_std_pool_array_internal_get_block_alloc, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_ARRAY_SIZE_SET", boing_value_from_ptr(boing, &boing_std_pool_array_internal_set_block_alloc, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_ARRAY_AMOUNT_GET", boing_value_from_ptr(boing, &boing_std_pool_array_internal_get_amount, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_ARRAY_FREE_GET", boing_value_from_ptr(boing, &boing_std_pool_array_internal_get_free, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_ARRAY_MAX_FREE_GET", boing_value_from_ptr(boing, &boing_std_pool_array_internal_get_max_free, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_ARRAY_MAX_FREE_SET", boing_value_from_ptr(boing, &boing_std_pool_array_internal_set_max_free, BOING_EXTERNAL_FUNCTION, NULL));

	BOING_ADD_GLOBAL("BOING_POOL_VALUE_SIZE_GET", boing_value_from_ptr(boing, &boing_std_pool_value_get_block_alloc, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_VALUE_SIZE_SET", boing_value_from_ptr(boing, &boing_std_pool_value_set_block_alloc, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_VALUE_AMOUNT_GET", boing_value_from_ptr(boing, &boing_std_pool_value_get_amount, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_VALUE_FREE_GET", boing_value_from_ptr(boing, &boing_std_pool_value_get_free, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_VALUE_MAX_FREE_GET", boing_value_from_ptr(boing, &boing_std_pool_value_get_max_free, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("BOING_POOL_VALUE_MAX_FREE_SET", boing_value_from_ptr(boing, &boing_std_pool_value_set_max_free, BOING_EXTERNAL_FUNCTION, NULL));

	/* math */
	BOING_ADD_GLOBAL("SIN", boing_value_from_ptr(boing, &boing_std_sin, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("ASIN", boing_value_from_ptr(boing, &boing_std_asin, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SINH", boing_value_from_ptr(boing, &boing_std_sinh, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("COS", boing_value_from_ptr(boing, &boing_std_cos, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("ACOS", boing_value_from_ptr(boing, &boing_std_acos, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("COSH", boing_value_from_ptr(boing, &boing_std_cosh, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("TAN", boing_value_from_ptr(boing, &boing_std_tan, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("ATAN", boing_value_from_ptr(boing, &boing_std_atan, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("TANH", boing_value_from_ptr(boing, &boing_std_tanh, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("EXP", boing_value_from_ptr(boing, &boing_std_exp, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("LOG", boing_value_from_ptr(boing, &boing_std_log, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("LOGTEN", boing_value_from_ptr(boing, &boing_std_log10, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("CEIL", boing_value_from_ptr(boing, &boing_std_ceil, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("ABS", boing_value_from_ptr(boing, &boing_std_fabs, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("FLOOR", boing_value_from_ptr(boing, &boing_std_floor, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("ATANTWO", boing_value_from_ptr(boing, &boing_std_atan2, BOING_EXTERNAL_FUNCTION, NULL));


	return boing->callback.boing_root_stack_init_cb(boing, stack);
}

size_t boing_sort_partition(boing_t *boing, boing_value_t *array, size_t start, size_t end)
{
	size_t i, j;

	
	#define SWAP(a, b) do	\
	{	\
		boing_value_t *temp = NULL;	\
		temp = (a);	\
		(a) = (b);	\
		(b) = temp;	\
	} while(0)

	for(j = i = start + 1; j <= end; ++j)
	{
		switch(boing_value_compare(boing, array->array[j], array->array[start]))
		{
			case BOING_COMPARISON_LESS_THAN:
				SWAP(array->array[i], array->array[j]);

				++i;
			break;
			/* TODO: determine how to handle unsortable cases like external values */
		}
	}
	SWAP(array->array[start], array->array[i - 1]);

	return i - 1;
}

void boing_sort_continue(boing_t *boing, boing_value_t *array, size_t start, size_t end)
{
	size_t pos;

	
	while(start < end && end != (size_t)-1)
	{
		pos = boing_sort_partition(boing, array, start, end);
	
		boing_sort_continue(boing, array, start, pos - 1);

		start = pos + 1;
	}
}

boing_value_t *boing_sort_array(boing_t *boing, boing_value_t *array)
{
	boing_value_t *ret = NULL;

	
	if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, array->length)))
	{
		boing_error(boing, 0, "could not create blank array for sorting");
		return NULL;
	}

	if(boing_value_set(boing, ret, array))
	{
		boing_error(boing, 0, "could not set the sorted array with the contents of the array to be sorted");
		return NULL;
	}

	boing_sort_continue(boing, ret, 0, ret->length - 1);

	return ret;
}

/* internal api exposed to the interpreter itself */

#define BOING_SINGLE_NUMERIC(function, identifier_string)	\
boing_value_t *boing_std_##function(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)	\
{	\
	boing_value_t *ret = NULL;	\
	\
	if(args->length != 1 && args->array[0]->type != BOING_TYPE_VALUE_NUMBER)	\
	{	\
		boing_error(boing, 0, "the "identifier_string" function expects a single numeric argument only");	\
		/* TODO throw error */	\
		return NULL;	\
	}	\
	\
	if(!(ret = boing_value_from_double(boing, function(args->array[0]->number))))	\
	{	\
		boing_error(boing, 0, "could not create return value for "identifier_string" function");	\
		/* TODO throw error */	\
		return NULL;	\
	}	\
	\
	return ret;	\
}

#define BOING_POOL_GET(pool_name, attribute, identifier_string)	\
boing_value_t *boing_std_pool_##pool_name##_get_##attribute(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)	\
{	\
	boing_value_t *ret = NULL;	\
	\
	if(args->length != 0)	\
	{	\
		boing_error(boing, 0, "the "identifier_string" function expects no arguments");	\
		/* TODO throw error */	\
		return NULL;	\
	}	\
	\
	if(!(ret = boing_value_from_double(boing, boing->pool.pool_name.attribute)))	\
	{	\
		boing_error(boing, 0, "could not create return value for "identifier_string" function");	\
		/* TODO throw error */	\
		return NULL;	\
	}	\
	\
	return ret;	\
}

#define BOING_POOL_SET(pool_name, attribute, identifier_string)	\
boing_value_t *boing_std_pool_##pool_name##_set_##attribute(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)	\
{	\
	boing_value_t *ret = NULL;	\
	\
	if(args->length != 1 && args->array[0]->type != BOING_TYPE_VALUE_NUMBER)	\
	{	\
		boing_error(boing, 0, "the "identifier_string" function expects a single numeric argument only");	\
		/* TODO throw error */	\
		return NULL;	\
	}	\
	\
	boing->pool.pool_name.attribute = (size_t)args->array[0]->number;	\
	if(!(ret = boing_value_from_double(boing, boing->pool.pool_name.attribute)))	\
	{	\
		boing_error(boing, 0, "could not create return value for "identifier_string" function");	\
		/* TODO throw error */	\
		return NULL;	\
	}	\
	\
	return ret;	\
}

BOING_SINGLE_NUMERIC(sin, "SIN");
BOING_SINGLE_NUMERIC(asin, "ASIN");
BOING_SINGLE_NUMERIC(sinh, "SINH");
BOING_SINGLE_NUMERIC(cos, "COS");
BOING_SINGLE_NUMERIC(acos, "ACOS");
BOING_SINGLE_NUMERIC(cosh, "COSH");
BOING_SINGLE_NUMERIC(tan, "TAN");
BOING_SINGLE_NUMERIC(atan, "ATAN");
BOING_SINGLE_NUMERIC(tanh, "TANH");
BOING_SINGLE_NUMERIC(exp, "EXP");
BOING_SINGLE_NUMERIC(log, "LOG");
BOING_SINGLE_NUMERIC(log10, "LOGTEN");
BOING_SINGLE_NUMERIC(ceil, "CEIL");
BOING_SINGLE_NUMERIC(fabs, "ABS");
BOING_SINGLE_NUMERIC(floor, "FLOOR");


BOING_POOL_GET(string, block_alloc, "BOING_POOL_STRING_SIZE_GET");
BOING_POOL_SET(string, block_alloc, "BOING_POOL_STRING_SIZE_SET");
BOING_POOL_GET(string, amount, "BOING_POOL_STRING_AMOUNT_GET");
BOING_POOL_GET(string, free, "BOING_POOL_STRING_FREE_GET");
BOING_POOL_GET(string, max_free, "BOING_POOL_STRING_AMOUNT_GET");
BOING_POOL_SET(string, max_free, "BOING_POOL_STRING_AMOUNT_SET");

BOING_POOL_GET(array_internal, block_alloc, "BOING_POOL_ARRAY_SIZE_GET");
BOING_POOL_SET(array_internal, block_alloc, "BOING_POOL_ARRAY_SIZE_SET");
BOING_POOL_GET(array_internal, amount, "BOING_POOL_ARRAY_AMOUNT_GET");
BOING_POOL_GET(array_internal, free, "BOING_POOL_ARRAY_FREE_GET");
BOING_POOL_GET(array_internal, max_free, "BOING_POOL_ARRAY_AMOUNT_GET");
BOING_POOL_SET(array_internal, max_free, "BOING_POOL_ARRAY_AMOUNT_SET");

BOING_POOL_GET(value, block_alloc, "BOING_POOL_VALUE_SIZE_GET");
BOING_POOL_SET(value, block_alloc, "BOING_POOL_VALUE_SIZE_SET");
BOING_POOL_GET(value, amount, "BOING_POOL_VALUE_AMOUNT_GET");
BOING_POOL_GET(value, free, "BOING_POOL_VALUE_FREE_GET");
BOING_POOL_GET(value, max_free, "BOING_POOL_VALUE_AMOUNT_GET");
BOING_POOL_SET(value, max_free, "BOING_POOL_VALUE_AMOUNT_SET");

boing_value_t *boing_std_atan2(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length != 2 && args->array[0]->type != BOING_TYPE_VALUE_NUMBER &&
		args->array[1]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "the ATANTWO function expects 2 numeric arguments only");
		/* TODO throw error */
		return NULL;
	}

	if(!(ret = boing_value_from_double(boing, atan2(args->array[0]->number, args->array[1]->number))))
	{
		boing_error(boing, 0, "could not create return value for ATANTWO function");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}


/* pool management */

int boing_pool_init(boing_t *boing, boing_pool_t *pool, boing_pool_container_destroy_cb_t cleanup_callback, boing_pool_container_size_cb_t size_callback, size_t max_free)
{
	memset(pool, 0, sizeof(boing_pool_t));

	pool->max_free = max_free;
	pool->get_size = size_callback;
	pool->container_cleanup = cleanup_callback;
	

	return 0;
}

int boing_pool_destroy(boing_t *boing, boing_pool_t *pool)
{
	/* loop through entire pool and call free callbacks */
	size_t i;
	boing_pool_container_t *temp = NULL;

	/* NOTE: this is a sorta roundabout way to free everything, but this is only
	called when boing is supposed to be destroyed so it doesn't have to be the 
	fastest */

	temp = pool->head;

	for(i = 0; i < pool->amount; ++i)
	{
		if(!temp)
		{
			boing_error(boing, 0, "pool out of sync: amount is too large");
			return 1;
		}
		if(temp->taken)
		{
			temp->taken = 0;
			pool->free++;
		}

		temp = temp->next;
	}

	/* shrink to 0 */
	pool->max_free = 0;

	if(boing_pool_shrink(boing, pool))
	{
		boing_error(boing, 0, "problem shrinking pool to 0");
		return 1;
	}

	return 0;
}

int boing_pool_list_remove(boing_t *boing, boing_pool_t *pool, boing_pool_container_t *remove)
{
	if(!remove)
	{
		boing_error(boing, 0, "NULL container provided");
		return 1;
	}

	if(remove == pool->end)
		pool->end = pool->end->prev;
	else if(remove == pool->head)
		pool->head = pool->head->next;

	if(remove->prev)
		remove->prev->next = remove->next;

	if(remove->next)
		remove->next->prev = remove->prev;
	
	remove->prev = remove->next = NULL;
	
	return 0;
}

int boing_pool_list_inject(boing_t *boing, boing_pool_t *pool, boing_pool_container_t *at, boing_pool_container_t *container)
{
	if(!at || !container)
	{
		boing_error(boing, 0, "one or more NULL containers provided");
		return 1;
	}

	if(at->prev)
	{
		container->prev = at->prev;
		at->prev->next = container;
	}

	at->prev = container;
	container->next = at;


	if(pool->head == at)
		pool->head = container;

	return 0;
}

int boing_pool_list_inject_after(boing_t *boing, boing_pool_t *pool, boing_pool_container_t *at, boing_pool_container_t *container)
{
	if(!at || !container)
	{
		boing_error(boing, 0, "one or more NULL containers provided");
		return 1;
	}

	if(at->next)
	{
		if(boing_pool_list_inject(boing, pool, at->next, container))
		{
			boing_error(boing, 0, "unable to inject container");
			return 1;
		}
	}
	else
	{
		at->next = container;
		container->prev = at;
		container->next = NULL;

		if(pool->end == at)
			pool->end = container;
	}

	return 0;
}

/* if min_size is 0, then the first unclaimed value of the any size will be returned */
void *boing_pool_request(boing_t *boing, boing_pool_t *pool, size_t min_size)
{
	size_t i;
	boing_pool_container_t *temp = NULL;

	if(!pool->free)
		return NULL; /* tell the caller to alloc and add new data */
	
	temp = pool->head;
	/* scan through free part of list */
	/* looping like this to prevent problems with a circular pointer */
	for(i = 0; i < pool->free; ++i)
	{
		if(!temp)
		{
			boing_error(boing, 0, "pool is out of sync: went out of bounds. Too many 'free'");
			return NULL;
		}
		if(temp->taken)
		{
			boing_error(boing, 0, "pool is out of sync: too few free");
			return NULL;
		}

		/* compare if minimum size requirement is met */
		if(min_size <= pool->get_size(pool, temp))
		{
			pool->free--;

			/* move from position to end */
			temp->taken = 1;

			if(boing_pool_list_remove(boing, pool, temp))
			{
				boing_error(boing, 0, "could not remove temp container");
				return NULL;
			}
			if(boing_pool_list_inject_after(boing, pool, pool->end, temp))
			{
				boing_error(boing, 0, "could not append temp container to end of pool");
				return NULL;
			}

			return temp->data;
		}


		if(temp->next && !temp->next->taken && i + 1 >= pool->free)
		{
			boing_error(boing, 0, "pool is out of sync: too many free");
			/* no need to return. Just warn */
		}

		temp = temp->next;
	}


	return NULL;
}

int boing_pool_add(boing_t *boing, boing_pool_t *pool, void *data, uint8_t taken, size_t size, boing_pool_container_t **data_container)
{
	boing_pool_container_t *container = NULL;

	if(!(container = malloc(sizeof(boing_pool_container_t))))
	{
		boing_error(boing, 0, "could not allocate empty container");
		return 1;
	}

	memset(container, 0, sizeof(boing_pool_container_t));

	container->data = data;
	container->size = size;


	/* initialize pool if nothing exists */
	if(!pool->amount)
	{
		if(!(container->taken = taken))
			pool->free++;

		pool->head = container;
		pool->end = container;
	}
	else
	{
		if(taken) /* add to back */
		{
			container->taken = taken;

			if(boing_pool_list_inject_after(boing, pool, pool->end, container))
			{
				boing_error(boing, 0, "problem appending new container to end of pool");
				return 1;
			}
		}
		else /* add to front */
		{
			if(boing_pool_list_inject(boing, pool, pool->head, container))
			{
				boing_error(boing, 0, "problem inserting container at the beginning of the pool");
				return 1;
			}

			pool->free++;
		}
	}

	/* because it is optional, if the container pointer to pointer is not NULL, the container will be set */
	if(data_container)
		*data_container = container;

	pool->amount++;

	return 0;
}

/* shrink free containers to the max */
int boing_pool_shrink(boing_t *boing, boing_pool_t *pool)
{
	size_t i, total;
	boing_pool_container_t *temp = NULL, *tmp_nxt = NULL;

	if(pool->free <= pool->max_free)
		return 0;

	total = pool->free - pool->max_free;

	temp = pool->head;
	/* scan through free part of list */
	/* looping like this to prevent problems with a circular pointer */
	for(i = 0; i < total; ++i)
	{
		if(!temp)
		{
			boing_error(boing, 0, "pool is out of sync: went out of bounds. Too many 'free'");
			return 1;
		}
		if(temp->taken)
		{
			boing_error(boing, 0, "pool is out of sync: too few free");
			return 1;
		}

		tmp_nxt = temp->next;

		/* remove, clean up the data, then free the memory */
		if(boing_pool_list_remove(boing, pool, temp))
		{
			boing_error(boing, 0, "could not remove from list");
			return 1;
		}
		if(pool->container_cleanup(pool, temp))
		{
			boing_error(boing, 0, "container cleanup callback errored");
			return 1;
		}
		free(temp);

		pool->amount--;
		pool->free--;


		/* now check if the free amount is small enough */
		if(pool->free <= pool->max_free)
			return 0;

		if(tmp_nxt && !tmp_nxt->taken && i + 1 >= total)
		{
			boing_error(boing, 0, "pool is out of sync: too many free");
			/* no need to return. Just warn */
		}

		temp = tmp_nxt;
	}


	return 0;
}

boing_pool_container_t *boing_pool_data_container(boing_t *boing, boing_pool_t *pool, void *data)
{
	size_t i;
	boing_pool_container_t *temp = NULL;

	/* look for a pointer that matches the one passed as data */
	/* NOTE: its faster to search in reverse because the taken containers
	are appended to the end and thats where most specifically wanted data
	will come from */

	temp = pool->end;

	for(i = 0; i < pool->amount - pool->free; ++i)
	{
		if(!temp)
		{
			boing_error(boing, 0, "pool is out of sync: went out of bounds. Too many 'free' or the 'amount' is too few");
			return NULL;
		}


		if(data == temp->data)
		{
			return temp;
		}


		/* TODO: determine if ever looking for free containers as well, and if not, uncomment */
		/* if(temp->prev && temp->prev->taken && i + 1 >= pool->amount - pool->free)
		{
			boing_error(boing, 0, "pool is out of sync: either the amount or amount free is incorrect");
			/* dont return. Just warn *//*
		} */

		temp = temp->prev;
	}


	return NULL;
}

/* returns 1 if error finding and returns 2 if error freeing */
int boing_pool_data_release(boing_t *boing, boing_pool_t *pool, void *data)
{
	boing_pool_container_t *container = NULL;
	/* look for the container and if it would make too many free containers,
	free it from memory */

	if(!(container = boing_pool_data_container(boing, pool, data)))
	{
		boing_error(boing, 0, "could not find data container in pool");
		return 1;
	}

	return boing_pool_data_release_container(boing, pool, container);
}

int boing_pool_data_release_container(boing_t *boing, boing_pool_t *pool, boing_pool_container_t *container)
{
	/* look for the container and if it would make too many free containers,
	free it from memory */

	if(!container)
	{
		boing_error(boing, 0, "container is NULL");
		return 1;
	}

	container->taken = 0;

	/* first, remove the container from where it was */
	if(boing_pool_list_remove(boing, pool, container))
	{
		boing_error(boing, 0, "could not remove container in list");
		return 2;
	}

	/* move container to free slots */
	if(boing_pool_list_inject(boing, pool, pool->head, container))
	{
		boing_error(boing, 0, "could not place free container to front of pool");
		return 2;
	}

	pool->free++;

	if(pool->free > pool->max_free)
	{
		if(boing_pool_shrink(boing, pool))
		{
			boing_error(boing, 0, "could not cleanly shrink released containers");
			return 2;
		}
	}

	return 0;
}

/* pool managers */

/* string pool */

static size_t boing_str_destroy_cb(boing_pool_t *pool, boing_pool_container_t *container)
{
	free(container->data);
	return 0;
}

static size_t boing_str_size_cb(boing_pool_t *pool, boing_pool_container_t *container)
{
	return container->size;
}

char *boing_str_request(boing_t *boing, size_t size)
{
	char *ret = NULL;

	if(!(ret = boing_pool_request(boing, &boing->pool.string, size)))
	{
		/* add a new string */
		if(!(ret = malloc(sizeof(char) * size)))
		{
			boing_error(boing, 0, "could not allocate new string for pool");
			return NULL;
		}
		memset(ret, 0, sizeof(char) * size);

		if(boing_pool_add(boing, &boing->pool.string, ret, 1, size, NULL))
		{
			boing_error(boing, 0, "could not add to pool after string alloc");
			free(ret);
			return NULL;
		}
	}

	return ret;
}

int boing_str_create_free(boing_t *boing, size_t size)
{
	char *temp = NULL;
	/* add a new string */
	if(!(temp = malloc(sizeof(char) * size)))
	{
		boing_error(boing, 0, "could not allocate new string for pool");
		return 1;
	}
	memset(temp, 0, sizeof(char) * size);

	if(boing_pool_add(boing, &boing->pool.string, temp, 0, size, NULL))
	{
		boing_error(boing, 0, "could not add to pool after string alloc");
		free(temp);
		return 1;
	}
	return 0;
}

int boing_str_release(boing_t *boing, char *ptr)
{
	return boing_pool_data_release(boing, &boing->pool.string, ptr);
}

size_t boing_str_alloc_size(boing_t *boing, char *ptr)
{
	boing_pool_container_t *temp = NULL;

	if(!(temp = boing_pool_data_container(boing, &boing->pool.string, ptr)))
	{
		boing_error(boing, 0, "string not found in pool");
		return 0; /* intentional */
	}

	return temp->size;
}

char *boing_str_resize(boing_t *boing, char *ptr, size_t size)
{
	char *ret = ptr, *temp = NULL;

	if(size > boing_str_alloc_size(boing, ptr))
	{
		/* search for new container that meets the size */
		if(!(temp = boing_str_request(boing, size)))
		{
			boing_error(boing, 0, "could not request a string from the pool of %lu units", size);
			return NULL;
		}

		/* memcpy old to new */
		memcpy(temp, ptr, boing_str_alloc_size(boing, ptr) * sizeof(char));

		ret = temp;

		/* release old container */
		if(boing_str_release(boing, ptr))
		{
			boing_error(boing, 0, "problem releasing old pool container in resize");
			/* not really sure the best action to take from here. The 
			new one already exists so might as well return it */
			return ret;
		}
	}

	return ret;
}

char *boing_str_sprintf(boing_t *boing, char *fmt, ...)
{
	va_list ap;
	size_t size;
	char *ret = NULL;


	va_start(ap, fmt);
	size = BOING_VSNPRINTF(ret, 0, fmt, ap); /* not available in C89 so might want to change this */
	va_end(ap);

	if(!(ret = boing_str_request(boing, size + 1)))
	{
		boing_error(boing, 0, "could not request string for str sprintf");
		return NULL;
	}

	va_start(ap, fmt);
	BOING_VSNPRINTF(ret, size + 1, fmt, ap);
	va_end(ap);


	return ret;
}

int boing_str_ncat(boing_t *boing, char **dest, char *src, size_t length)
{
	char *temp = NULL;
	

	if(!*dest)
	{
		boing_error(boing, 0, "null string source passed to ncat");
		return 1;
	}
	
	if(!(temp = boing_str_resize(boing, *dest, strlen(*dest) + length + 1)))
	{
		boing_error(boing, 0, "could not resize destination string for ncat");
		return 1;
	}

	*dest = temp;

	strncat(*dest, src, length + 1);
	
	return 0;
}

char *boing_str_ndup(boing_t *boing, char *str, size_t length)
{
	char *ret = NULL;


	if(!(ret = boing_str_request(boing, length + 1)))
	{
		boing_error(boing, 0, "could not request string in str_ndup");
		return NULL;
	}

	memmove(ret, str, length);
	ret[length] = 0x0;

	return ret;
}

int boing_str_replace(boing_t *boing, char **source, char *replacee, char *replacement)
{
	size_t i, replacement_length = 0, replacee_length = 0, new_length = 0;
	char *temp = NULL;


	/* TODO: store the length of the source between edits as well */
	if(!*source)
	{
		boing_error(boing, 0, "null string source passed to str_replace");
		return 1;
	}

	/* because a string with \0 is just "", all "" strings will test for \0 */
	if(!(replacee_length = strlen(replacee)))
		replacee_length = 1;
	
	if(!(replacement_length = strlen(replacement)))
		replacement_length = 1;


	for(i = 0; i < strlen(*source); ++i)
	{
		/* make sure theres enough string to test left */
		if(replacee_length <= strlen(*source) - i && strncmp(&(*source)[i], replacee, replacee_length) == 0)
		{
			/* if its smaller, just replace, if its larger resize */
			if(replacement_length > replacee_length)
			{
				new_length = strlen(*source) + (replacement_length - replacee_length);

				if(!(temp = boing_str_resize(boing, *source, new_length + 1)))
				{
					boing_error(boing, 0, "could not resize string in str_replace");
					return 1;
				}

				temp[new_length] = 0x0;

				*source = temp;
			}

			/* memmove everything past the replacement zone */
			memmove(&(*source)[i + replacement_length], &(*source)[i + replacee_length], strlen(*source) - (abs(replacement_length - replacee_length) + i));

			/* overwrite the replacee */
			memmove(&(*source)[i], replacement, replacement_length);
		}
	}


	return 0;
}

/* array internal pool */

static size_t boing_array_internl_destroy_cb(boing_pool_t *pool, boing_pool_container_t *container)
{
	free(container->data);
	return 0;
}

static size_t boing_array_internl_size_cb(boing_pool_t *pool, boing_pool_container_t *container)
{
	return container->size;
}

boing_value_t **boing_array_internl_request(boing_t *boing, size_t size)
{
	boing_value_t **ret = NULL;

	if(!(ret = boing_pool_request(boing, &boing->pool.array_internal, size)))
	{
		/* add a new internal array */
		if(!(ret = malloc(sizeof(boing_value_t **) * size)))
		{
			boing_error(boing, 0, "could not allocate new internal array for pool");
			return NULL;
		}
		memset(ret, 0, sizeof(boing_value_t **) * size);

		if(boing_pool_add(boing, &boing->pool.array_internal, ret, 1, size, NULL))
		{
			boing_error(boing, 0, "could not add to pool after internal array alloc");
			free(ret);
			return NULL;
		}
	}

	return ret;
}

int boing_array_internl_create_free(boing_t *boing, size_t size)
{
	char *temp = NULL;
	/* add a new internal array buffer */
	if(!(temp = malloc(sizeof(boing_value_t **) * size)))
	{
		boing_error(boing, 0, "could not allocate new internal array for pool");
		return 1;
	}
	memset(temp, 0, sizeof(boing_value_t **) * size);

	if(boing_pool_add(boing, &boing->pool.array_internal, temp, 0, size, NULL))
	{
		boing_error(boing, 0, "could not add to pool after internal alloc");
		free(temp);
		return 1;
	}
	return 0;
}

int boing_array_internl_release(boing_t *boing, void *ptr)
{
	return boing_pool_data_release(boing, &boing->pool.array_internal, ptr);
}

size_t boing_array_internl_alloc_size(boing_t *boing, void *ptr)
{
	boing_pool_container_t *temp = NULL;

	if(!(temp = boing_pool_data_container(boing, &boing->pool.array_internal, ptr)))
	{
		boing_error(boing, 0, "internal array not found in pool");
		return 0; /* intentional */
	}

	return temp->size;
}

boing_value_t **boing_array_internl_resize(boing_t *boing, boing_value_t **ptr, size_t size)
{
	boing_value_t **ret = ptr, **temp = NULL;

	if(size > boing_array_internl_alloc_size(boing, ptr))
	{
		/* increase size with extra room */
		size += boing->pool.array_internal.block_alloc;

		/* search for new container that meets the size */
		if(!(temp = boing_array_internl_request(boing, size)))
		{
			boing_error(boing, 0, "could not request an internal array buffer from the pool of %lu units", size);
			return NULL;
		}

		/* memcpy old to new */
		memcpy(temp, ptr, boing_array_internl_alloc_size(boing, ptr) * sizeof(boing_value_t **));

		ret = temp;

		/* release old container */
		if(boing_array_internl_release(boing, ptr))
		{
			boing_error(boing, 0, "problem releasing old pool container in resize");
			/* not really sure the best action to take from here. The 
			new one already exists so might as well return it */
			return ret;
		}
	}

	return ret;
}

/* value pool */

static size_t boing_value_pool_destroy_cb(boing_pool_t *pool, boing_pool_container_t *container)
{
	free(container->data);
	return 0;
}

static size_t boing_value_pool_size_cb(boing_pool_t *pool, boing_pool_container_t *container)
{
	return container->size;
}

boing_value_t *boing_value_pool_request(boing_t *boing, size_t size)
{
	boing_pool_container_t *container = NULL;
	boing_value_t *ret = NULL;

	if(!(ret = boing_pool_request(boing, &boing->pool.value, size)))
	{
		/* add a new value base struct */
		if(!(ret = malloc(sizeof(boing_value_t))))
		{
			boing_error(boing, 0, "could not allocate new value base for pool");
			return NULL;
		}
		memset(ret, 0, sizeof(boing_value_t));

		if(boing_pool_add(boing, &boing->pool.value, ret, 1, size, &container))
		{
			boing_error(boing, 0, "could not add to pool after value base alloc");
			free(ret);
			return NULL;
		}

		ret->container = container;
	}
	else
	{
		/* need to preserve the container before memsets */
		container = ret->container;
		memset(ret, 0, sizeof(boing_value_t));
		ret->container = container;
	}

	return ret;
}

int boing_value_pool_create_free(boing_t *boing, size_t size)
{
	boing_pool_container_t *container = NULL;
	boing_value_t *temp = NULL;


	/* add a new value base */
	if(!(temp = malloc(sizeof(boing_value_t))))
	{
		boing_error(boing, 0, "could not allocate new value base for pool");
		return 1;
	}
	memset(temp, 0, sizeof(boing_value_t));

	if(boing_pool_add(boing, &boing->pool.value, temp, 0, size, &container))
	{
		boing_error(boing, 0, "could not add to pool after value base alloc");
		free(temp);
		return 1;
	}

	temp->container = container;

	return 0;
}

int boing_value_pool_release(boing_t *boing, boing_value_t *ptr)
{
	if(!ptr->container)
		return boing_pool_data_release(boing, &boing->pool.value, ptr);

	return boing_pool_data_release_container(boing, &boing->pool.value, ptr->container);
}

/* boing value handling and eval */

/* meta structures like tables and stacks*/

boing_value_t *boing_value_stack_create(boing_t *boing, boing_value_t *program)
{
	boing_value_t *ret = NULL, *temp = NULL;

	/* verify that its a valid program */

	if(program->type != BOING_TYPE_VALUE_ARRAY || program->length != 2)
	{
		boing_error(boing, 0, "invalid program provided to the stack initialize function");
		return NULL;
	}

	/* create the stack array and initialize to [0 root_identifier_table]*/

	if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 2)))
	{
		boing_error(boing, 0, "could not request array for initial stack");
		return NULL;
	}


	if(!(temp = boing_value_from_double(boing, 0)))
	{
		boing_error(boing, 0, "could not request 0 for upref in initial stack");
		return NULL;
	}

	if(boing_value_array_append_insert(boing, ret, 0, temp))
	{
		boing_error(boing, 0, "could not insert 0 into stack array");
		return NULL;
	}


	/* inc the references because the program needs it and also the root stack does as well */
	boing_value_reference_inc(boing, program->array[1]);

	if(boing_value_array_append_insert(boing, ret, 1, program->array[1]))
	{
		boing_error(boing, 0, "could not insert 0 into stack array");
		boing_value_reference_dec(boing, program->array[1]);
		return NULL;
	}


	/* add all root stack variables */
	if(boing_root_stack_init(boing, ret))
	{
		boing_error(boing, 0, "could not add root stack variables");
		boing_value_reference_dec(boing, program->array[1]);
		return NULL;
	}


	return ret;
}

boing_value_t *boing_value_stack_push(boing_t *boing, boing_value_t *parent)
{
	boing_value_t *ret = NULL, *temp = NULL;


	/* refer back to the layout of the stack */

	if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 2)))
	{
		boing_error(boing, 0, "could not request array for stack");
		return NULL;
	}


	boing_value_reference_inc(boing, parent);
	if(boing_value_array_append_insert(boing, ret, 0, parent))
	{
		boing_error(boing, 0, "could not insert parent into stack array");
		return NULL;
	}


	if(!(temp = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 0)))
	{
		boing_error(boing, 0, "could not request an empty array for identifiers in stack");
		boing_value_reference_dec(boing, parent);
		return NULL;
	}

	if(boing_value_array_append_insert(boing, ret, 1, temp))
	{
		boing_error(boing, 0, "could not insert emtpy identifer table into stack array");
		return NULL;
	}


	return ret;
}

boing_value_t *boing_value_stack_pop(boing_t *boing, boing_value_t *stack)
{
	boing_value_t *ret = NULL;


	if(stack && stack->type != BOING_TYPE_VALUE_ARRAY || stack->length != 2)
	{
		boing_error(boing, 0, "not a valid stack to pop");
		return NULL;
	}

	/* NOTE: need to check if at root stack or not */

	if(boing_value_stack_is_root(boing, stack))
	{
		boing_error(boing, 0, "cannot pop stack as it is the root");
		return NULL;
	}

	if(!(ret = stack->array[0]))
	{
		boing_error(boing, 0, "stack corruption when trying to pop current stack");
		return NULL;
	}

	/* free old identifiers */
	if(boing_value_reference_dec(boing, stack))
	{
		boing_error(boing, 0, "could not refdec old stack */");
		return NULL;
	}


	return ret;
}

int boing_value_stack_is_root(boing_t *boing, boing_value_t *stack)
{
	if(stack && stack->type == BOING_TYPE_VALUE_ARRAY && stack->length == 2 && stack->array[0] && stack->array[0]->type == BOING_TYPE_VALUE_NUMBER)
		return 1;

	return 0;
}

boing_value_t *boing_value_stack_search(boing_t *boing, boing_value_t *stack, boing_value_t *identifier)
{
	boing_value_t *ret = NULL;


	if(stack && stack->type != BOING_TYPE_VALUE_ARRAY || stack->length != 2)
	{
		boing_error(boing, 0, "invalid stack passed to search");
		return NULL;
	}


	/* recursively search */
	if(!(ret = boing_value_table_get(boing, stack->array[1], identifier)) && !boing_value_stack_is_root(boing, stack))
		ret = boing_value_stack_search(boing, stack->array[0], identifier);


	return ret;
}

int boing_value_stack_add_set(boing_t *boing, boing_value_t *stack, boing_value_t *identifier, boing_value_t *value)
{
	/* NOTE: do not modify references, those are handled by the table function */

	if(stack && stack->type != BOING_TYPE_VALUE_ARRAY || stack->length != 2)
	{
		boing_error(boing, 0, "invalid stack passed to insert");
		return 1;
	}

	if(boing_value_table_add_set(boing, stack->array[1], identifier, value, 1))
	{
		boing_error(boing, 0, "could not add or modify identifier on stack");
		return 1;
	}

	return 0;
}

boing_value_t *boing_value_stack_get_root(boing_t *boing, boing_value_t *stack)
{
	boing_value_t *ret = stack;

	
	if(stack && stack->type != BOING_TYPE_VALUE_ARRAY || stack->length != 2)
	{
		boing_error(boing, 0, "invalid stack passed to get root");
		return NULL;
	}


	if(!boing_value_stack_is_root(boing, stack))
		ret = boing_value_stack_get_root(boing, stack->array[0]);

	return ret;
}

int boing_value_stack_add_set_root(boing_t *boing, boing_value_t *stack, boing_value_t *identifier, boing_value_t *value)
{
	boing_value_t *root = NULL;

	if(!(root = boing_value_stack_get_root(boing, stack)))
	{
		boing_error(boing, 0, "could not get root for set root");
		return 1;
	}

	if(boing_value_stack_add_set(boing, root, identifier, value))
	{
		boing_error(boing, 0, "could not add or set value in root stack");
		return 1;
	}

}


int boing_value_table_add_set(boing_t *boing, boing_value_t *table, boing_value_t *key, boing_value_t *value, int add_not_found)
{
	boing_value_t *row = NULL;
	size_t i;
	
	
	if(!table)
	{
		boing_error(boing, 0, "table value NULL");
		return 1;
	}

	if(table->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "value not a table");
		return 1;
	}

	/* look for a match */
	for(i = 0; i < table->length; ++i)
	{
		if(table->array[i]->type != BOING_TYPE_VALUE_ARRAY)
			return 1;
		if(table->array[i]->length != 2)
			return 1;
		
		if(boing_value_compare(boing, table->array[i]->array[0], key) == BOING_COMPARISON_EQUAL)
		{
			row = table->array[i];
			break;
		}
	}

	if(row)
	{
		if(boing_value_set(boing, table->array[i]->array[1], value))
		{
			boing_error(boing, 0, "could not set the value after finding the key in table");
			return 1;
		}

		return 0;
	}
	else if(!row && add_not_found)
	{
		if(!(row = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 2)))
		{
			boing_error(boing, 0, "could not request array for table row");
			return 1;
		}

		/* add both the key and value to new row */

		if(boing_value_array_append_insert(boing, row, row->length, key))
		{
			boing_error(boing, 0, "could not add key to table row");
			boing_value_reference_dec(boing, row);
			return 1;
		}

		if(boing_value_array_append_insert(boing, row, row->length, value))
		{
			boing_error(boing, 0, "could not add value to table row");
			boing_value_reference_dec(boing, row);
			return 1;
		}

		/* toss the new row into the table */

		if(boing_value_array_append_insert(boing, table, table->length, row))
		{
			boing_error(boing, 0, "could not add row to table");
			boing_value_reference_dec(boing, row);
			return 1;
		}

		
		/* increment the references of the key and value values */
		boing_value_reference_inc(boing, key);
		boing_value_reference_inc(boing, value);

		return 0;
	}

	return 2; /* just in case a distinction needs to be made */
}

int boing_value_table_remove(boing_t *boing, boing_value_t *table, boing_value_t *key)
{
	size_t i;


	if(!table)
	{
		boing_error(boing, 0, "table value NULL");
		return 1;
	}

	if(table->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "value not a table");
		return 1;
	}

	for(i = 0; i < table->length; ++i)
	{
		if(table->array[i]->type != BOING_TYPE_VALUE_ARRAY)
			return 1;
		if(table->array[i]->length != 2)
			return 1;
		
		if(boing_value_compare(boing, table->array[i]->array[0], key) == BOING_COMPARISON_EQUAL)
		{
			if(boing_value_array_remove(boing, table, i))
			{
				boing_error(boing, 0, "could not remove table row");
				return 1;
			}
			
			return 0;
		}
	}

	return 1;
}

boing_value_t *boing_value_table_get(boing_t *boing, boing_value_t *table, boing_value_t *key)
{
	size_t i;


	if(!table)
	{
		boing_error(boing, 0, "table value NULL");
		return NULL;
	}

	if(table->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "value not a table");
		return NULL;
	}

	for(i = 0; i < table->length; ++i)
	{
		if(table->array[i]->type != BOING_TYPE_VALUE_ARRAY)
			return NULL;
		if(table->array[i]->length != 2)
			return NULL;
		
		if(boing_value_compare(boing, table->array[i]->array[0], key) == BOING_COMPARISON_EQUAL)
			return table->array[i]->array[1];
	}

	return NULL;
}

boing_value_t *boing_value_table_get_str(boing_t *boing, boing_value_t *table, char *key)
{
	boing_value_t *ret = NULL, *key_value = NULL;


	if(!(key_value = boing_value_from_str(boing, key)))
	{
		boing_error(boing, 0, "could not create key from string");
		return NULL;
	}

	ret = boing_value_table_get(boing, table, key_value);

	if(boing_value_reference_dec(boing, key_value))
	{
		boing_error(boing, 0, "could not refdec key");
		return NULL;
	}

	return ret;
}

/* if an array, this will _not_ initialize the array elements
this only allocates/guarantees the array to be this size at minimum */
boing_value_t *boing_value_request(boing_t *boing, uint8_t type, uint32_t alloc_length)
{
	uint32_t i;
	boing_value_t *ret = NULL, *temp = NULL;

	/* request base value struct from its pool */

	if(!(ret = boing_value_pool_request(boing, 0)))
	{
		boing_error(boing, 0, "problem requesting value from the value pool");
		return NULL;
	}

	/* NOTE: temporary stuff */
	ret->references = 1;
	ret->type = type;
	ret->allocated = alloc_length;

	switch(type)
	{
		case BOING_TYPE_VALUE_ARRAY:

			if(!(ret->array = boing_array_internl_request(boing, alloc_length)))
			{
				boing_error(boing, 0, "problem requesting internal array buffer from pool");
				return NULL;
			}

			ret->allocated = boing_array_internl_alloc_size(boing, ret->array);
			/* boing_error(boing, 0, "neato array: %lu, requested %lu", ret->allocated, alloc_length); */

		break;
		case BOING_TYPE_VALUE_OPERATION:
			if(!(ret->operation.args_value = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 0)))
			{
				boing_error(boing, 0, "problem requesting operation argument array");
				return NULL;
			}
		break;
	}
	

	return ret;
}

int boing_value_release(boing_t *boing, boing_value_t *value)
{
	uint32_t i;

	if(!value)
	{
		boing_error(boing, 0, "passed a null value");
		return 1;
	}

	/* recursively release values back to whatever pools the parts make up */

	switch(value->type)
	{
		case BOING_TYPE_VALUE_ARRAY:
			if(boing_array_internl_release(boing, value->array))
			{
				boing_error(boing, 0, "problem releasing internal array buffer back to pool");
				return 1;
			}
		break;
	}

	/* release value container */
	if(boing_value_pool_release(boing, value))
	{
		boing_error(boing, 0, "problem releasing value struct base back to pool");
	}

	return 0;
}

int boing_value_array_append_insert(boing_t *boing, boing_value_t *array, size_t position, boing_value_t *value)
{
	boing_value_t **temp = NULL;

	if(!value)
	{
		boing_error(boing, 0, "null value provided to append/insert");
		return 1;
	}

	if(array->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "cannot append to something that isnt an array");
		return 1;
	}

	if(array->allocated < array->length + 1) /* need to extend the array */
	{
		/* if(!(temp = (boing_value_t **)realloc(array->array, sizeof(boing_value_t **) * (array->allocated + 1)))) */
		/* NOTE: this should allocate in chunks. Its probably a good idea to call this as few times as possible */
		if(!(temp = boing_array_internl_resize(boing, array->array, array->allocated + 1)))
		{
			boing_error(boing, 0, "problem resizing array");
			return 1;
		}

		array->array = temp;
		array->allocated++;
	}

	/* only actually instert if not at the end of the array. 0 indexed */
	if(position >= array->length)
		array->array[array->length] = value; /* only actually write to the end if it goes over */
	else
	{
		/* need to shuffle things around */
		memmove(&array->array[position + 1], &array->array[position], (array->length - position) * sizeof(boing_value_t **));
		array->array[position] = value;
	}

	array->length++;

	return 0;
}

int boing_value_array_remove(boing_t *boing, boing_value_t *array, size_t position)
{
	if(array->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "value is not array type");
		return 1;
	}
	if(position >= array->length)
	{
		boing_error(boing, 0, "array indexed at invalid position");
		return 1;
	}

	if(boing_value_reference_dec(boing, array->array[position]))
	{
		boing_error(boing, 0, "could not decrement references for array member");
		return 1;
	}

	/* memmove everything to the right down 1 if the position is anything but the last */
	if(position != array->length - 1)
		memmove(&array->array[position], &array->array[position + 1], (array->length - (position + 1)) * sizeof(boing_value_t *));


	array->length--;

	return 0;
}

/* NOTE: THIS DOES NOT INCREMENT REFERENCE COUNTER */
/* TODO: Maybe make a ref incrementing ver or arg */
boing_value_t *boing_value_array_index(boing_t *boing, boing_value_t *array, size_t position)
{
	boing_value_t *ret = NULL;

	if(array->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "value is not array type");
		return NULL;
	}
	if(position >= array->length)
	{
		boing_error(boing, 0, "array indexed at invalid position");
		return NULL;
	}

	ret = array->array[position];

	return ret;
}

void boing_value_reference_inc(boing_t *boing, boing_value_t *value)
{
	value->references++;
}

int boing_value_reference_dec(boing_t *boing, boing_value_t *value)
{
	size_t i;


	if(!value)
	{
		boing_error(boing, 0, "null value passed to value_reference_dec");
		return 1;
	}

	if(value->references == 0 || --value->references == 0)
	{
		#if BOING_ENABLE_LINE_NUM
			/* not all values have a script value. The only ones that do come from the parser */
			if(value->script)
				if(boing_value_reference_dec(boing, value->script))
				{
					boing_error(boing, 0, "could not refdec script external data value");
					return 1;
				}
		#endif
		/* anything that has stuff inside should be cleaned up or handled */
		switch(value->type)
		{
			/* if any children, decrement their references */
			case BOING_TYPE_VALUE_ARRAY:
				for(i = 0; i < value->length; ++i)
				{
					if(value->array[i]->references == 0 || --value->array[i]->references == 0)
					{
						if(boing_value_reference_dec(boing, value->array[i]))
						{
							boing_error(boing, 0, "could not decrement references in an array memeber");
							return 1;
						}
					}
				}
			break;
			case BOING_TYPE_VALUE_OPERATION:
				if(boing_value_reference_dec(boing, value->operation.args_value))
				{
					boing_error(boing, 0, "could not decrement args array in an operation");
					return 1;
				}
			break;
			case BOING_TYPE_VALUE_EXTERNAL:
				if(value->external.destroy_cb)
				{
					if(value->external.destroy_cb(boing, value))
					{
						/* TODO, because this is really bad, it might be a good idea to make it a fatal error */
						boing_error(boing, 0, "could not destroy external value cleanly. May have been corrupted");
						return 1;
					}
				}
			break;
		}

		/* finally release this value */
		if(boing_value_release(boing, value))
		{
			boing_error(boing, 0, "could not release value after decrementing references");
			return 1;
		}
	}

	return 0;
}

int boing_value_set(boing_t *boing, boing_value_t *target, boing_value_t *value)
{
	boing_pool_container_t *old_container = NULL;
	boing_value_t **temp_arr = NULL;
	size_t i, old_ref = 0;

	/* detect if it's the same value and silently do nothing */
	if(target == value)
		return 0;

	/* destroy old internals */

	/* refdec and nullify the script value if it is enabled */
	#if BOING_ENABLE_LINE_NUM
		if(target->script)
		{
			if(boing_value_reference_dec(boing, target->script))
			{
				boing_error(boing, 0, "could not refdec target script value in set");
				return 1;
			}

			target->script = NULL;
		}
	#endif

	switch(target->type)
	{
		case BOING_TYPE_VALUE_ARRAY:
			/* destroy old parts inside */
			for(i = 0; i < target->length; ++i)
			{
				if(boing_value_reference_dec(boing, target->array[i]))
				{
					boing_error(boing, 0, "could not decrement reference in set function while inside the target internal array buffer");
					return 1;
				}
			}

			/* destroy the old array buffer */
			if(boing_array_internl_release(boing, target->array))
			{
				boing_error(boing, 0, "could not release old array buffer in set");
				return 1;
			}
			
		break;
		case BOING_TYPE_VALUE_OPERATION:
			if(boing_value_reference_dec(boing, target->operation.args_value))
			{
				boing_error(boing, 0, "could not decrement reference in set function while inside the target operation args");
				return 1;
			}
		break;
		case BOING_TYPE_VALUE_EXTERNAL:
			if(target->external.destroy_cb && target->external.destroy_cb(boing, target))
			{
				boing_error(boing, 0, "could not handle external destruction case");
				return 1;
			}
		break;
	}

	old_ref = target->references;
	old_container = target->container;
	/* just memcpy at first */
	memcpy(target, value, sizeof(boing_value_t));

	/* increment references to things depending on the type. */
	switch(target->type)
	{
		case BOING_TYPE_VALUE_ARRAY:
			/* update new parts inside */

			/* need a new internal array buffer */
			if(!(temp_arr = boing_array_internl_request(boing, value->allocated)))
			{
				boing_error(boing, 0, "unable to request new internal array buffer while setting value");
				return 1;
			}

			/* only copy length because it may be a very large allocated buffer */
			memcpy(temp_arr, value->array, value->length * sizeof(boing_value_t **));

			target->array = temp_arr;

			/* unless cosmic rays flew through ram, impossible for this to fail if it got this far */
			for(i = 0; i < target->length; ++i)
				boing_value_reference_inc(boing, target->array[i]);
			
		break;
		case BOING_TYPE_VALUE_OPERATION:
			boing_value_reference_inc(boing, target->operation.args_value);
		break;
		case BOING_TYPE_VALUE_EXTERNAL:
			if(target->external.reference_inc_cb && target->external.reference_inc_cb(boing, target))
			{
				boing_error(boing, 0, "could not handle case where external type has an increment callback");
				return 1;
			}
		break;
	}
	
	target->container = old_container; /* preserve the container */
	target->references = old_ref; /* reset references in case overwritten */

	/* if enabled, refinc the script value */
	#if BOING_ENABLE_LINE_NUM
		if(value->script)
			boing_value_reference_inc(boing, value->script);
	#endif
	
	return 0;
}

/* recursively copy values */
boing_value_t *boing_value_copy(boing_t *boing, boing_value_t *value)
{
	size_t i;
	boing_pool_container_t *container;
	boing_value_t *ret = NULL, *temp = NULL;


	switch(value->type)
	{
		case BOING_TYPE_VALUE_NUMBER:
			if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_NUMBER, 0)))
			{
				boing_error(boing, 0, "could not request numeric value for copy");
				return NULL;
			}

			container = ret->container;

			memcpy(ret, value, sizeof(boing_value_t));
		break;
		case BOING_TYPE_VALUE_EXTERNAL:
			if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_EXTERNAL, 0)))
			{
				boing_error(boing, 0, "could not request external value for copy");
				return NULL;
			}

			if(value->external.reference_inc_cb && value->external.reference_inc_cb(boing, value))
			{
				boing_error(boing, 0, "could not increment external pointer's references");
				return NULL;
			}

			container = ret->container;

			memcpy(ret, value, sizeof(boing_value_t));
		break;
		case BOING_TYPE_VALUE_OPERATION:
			if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_OPERATION, 0)))
			{
				boing_error(boing, 0, "could not request operation value for copy");
				return NULL;
			}

			/* destroy the allocated args because they are overwritten later */
			if(boing_value_reference_dec(boing, ret->operation.args_value))
			{
				boing_error(boing, 0, "could not clean up original arguments in operation copy");
				return NULL;
			}

			container = ret->container;

			memcpy(ret, value, sizeof(boing_value_t));

			/* also copy the argument array */
			if(!(ret->operation.args_value = boing_value_copy(boing, value->operation.args_value)))
			{
				boing_error(boing, 0, "could not copy operation arguments");
				return NULL;
			}
		break;
		case BOING_TYPE_VALUE_ARRAY:
			if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, value->length)))
			{
				boing_error(boing, 0, "could not request array value for copy");
				return NULL;
			}

			container = ret->container;

			for(i = 0; i < value->length; ++i)
			{
				if(!(temp = boing_value_copy(boing, value->array[i])))
				{
					boing_error(boing, 0, "could not copy array value");
					return NULL;
				}

				if(boing_value_array_append_insert(boing, ret, i, temp))
				{
					boing_error(boing, 0, "could not copy value in the array");
					return NULL;
				}
			}
		break;
	}
	
	/* in case overwritten by memcpy */
	ret->references = 1;
	ret->container = container;

	/* if enabled, refinc the script value */
	#if BOING_ENABLE_LINE_NUM
		if(value->script && value->type != BOING_TYPE_VALUE_ARRAY)
			boing_value_reference_inc(boing, value->script);
	#endif


	return ret;
}

boing_value_t *boing_value_cast(boing_t *boing, boing_value_t *value, uint8_t cast)
{
	boing_value_t *ret = NULL, *temp = NULL;
	char *temp_str = NULL;


	/* make sure not already the same type */
	if(value->type == cast)
	{
		/* silently do nothing but refinc */
		ret = value;
		boing_value_reference_inc(boing, ret);
	}

	/* cast anything castable. NOTE: the external type is unviewable and uncastable */
	switch(value->type)
	{
		case BOING_TYPE_VALUE_ARRAY:
			/* [] -> # = string -> num */
			/* [] -> op = [op [args]] -> op(args) */

			switch(cast)
			{
				case BOING_TYPE_VALUE_NUMBER:
					/* need to convert the array to a c string then to a number */
					if(!(temp_str = boing_str_from_value_array(boing, value)))
					{
						boing_error(boing, 0, "could not request number string for array to number cast");
						return NULL;
					}

					if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_NUMBER, 0)))
					{
						boing_error(boing, 0, "could not request number value for array to number cast");
						return NULL;
					}

					ret->number = strtod(temp_str, NULL);

					/* cleanup */
					if(boing_str_release(boing, temp_str))
					{
						boing_error(boing, 0, "could not free conversion string int array to number cast");
						return NULL;
					}
				break;
				case BOING_TYPE_VALUE_OPERATION:
					/* first, verify that this follows the structure of [op [args]] */
					if(value->length != 2 || value->array[0]->type != BOING_TYPE_VALUE_NUMBER || value->array[1]->type != BOING_TYPE_VALUE_ARRAY)
					{
						boing_error(boing, 0, "incorrect format for array to operation cast");
						return NULL;
					}

					/* cast it */
					if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_OPERATION, 0)))
					{
						boing_error(boing, 0, "could not request operation value for array to operation cast");
						return NULL;
					}


					/* the operation char */
					ret->operation.op = value->array[0]->number;

					/* the args. Reuse the array (just set to refinc the args) */
					if(boing_value_set(boing, ret->operation.args_value, value->array[1]))
					{
						boing_error(boing, 0, "could not set operation result for array to operation cast");
						return NULL;
					}
				break;
			}
		break;
		case BOING_TYPE_VALUE_NUMBER:
			/* # -> [] = num -> string */
			/* # -> op = num -> op() */

			switch(cast)
			{
				case BOING_TYPE_VALUE_ARRAY:
					/* the number string. Maybe %g isnt the best choice */
					if(!(temp_str = boing_str_sprintf(boing, "%g", value->number)))
					{
						boing_error(boing, 0, "could not request number string for number to array cast");
						return NULL;
					}

					if(!(ret = boing_value_from_str(boing, temp_str)))
					{
						boing_error(boing, 0, "could not create string (array) value for number to string cast");
						return NULL;
					}

					/* cleanup */
					if(boing_str_release(boing, temp_str))
					{
						boing_error(boing, 0, "could not free conversion string in number to string cast");
						return NULL;
					}
				break;
				case BOING_TYPE_VALUE_OPERATION:
					/* just make an empty operation with the char set to the number passed */
					if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_OPERATION, 0)))
					{
						boing_error(boing, 0, "could not request operation value for number to operation cast");
						return NULL;
					}


					/* the operation char */
					ret->operation.op = value->number;
				break;
			}
		break;
		case BOING_TYPE_VALUE_OPERATION:
			/* op(args) -> # = op(args) -> num */
			/* op(args) -> [] = op(args) -> [op [args]] */

			switch(cast)
			{
				case BOING_TYPE_VALUE_NUMBER:
					if(!(ret = boing_value_from_double(boing, (double)value->operation.op)))
					{
						boing_error(boing, 0, "could not request number value for operation to number cast");
						return NULL;
					}
				break;
				case BOING_TYPE_VALUE_ARRAY:
					/* simpy take the operation and args and increase the refcounter */
					if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 2)))
					{
						boing_error(boing, 0, "could not request array value for number to operation cast");
						return NULL;
					}

					if(!(temp = boing_value_from_double(boing, (double)value->operation.op)))
					{
						boing_error(boing, 0, "could not request number sub value for operation to array cast");
						return NULL;
					}

					/* add num to array and refinc the args */

					if(boing_value_array_append_insert(boing, ret, 0, temp))
					{
						boing_error(boing, 0, "could not insert number sub value for operation to array cast");
						return NULL;
					}

					if(boing_value_array_append_insert(boing, ret, 1, value->operation.args_value))
					{
						boing_error(boing, 0, "could not insert args sub value for operation to array cast");
						return NULL;
					}

					boing_value_reference_inc(boing, value->operation.args_value);
				break;
			}
		break;
		default:
			boing_error(boing, 0, "value passed to value_cast is uncastable");
			return NULL;
	}

	return ret;
}

boing_value_t *boing_value_from_ptr(boing_t *boing, void *ptr, uint8_t type, int (*destroy_cb)(boing_t *boing, boing_value_t *value))
{
	boing_value_t *ret = NULL;

	if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_EXTERNAL, 0)))
	{
		boing_error(boing, 0, "could not request external value");
		return NULL;
	}

	ret->external.type = type;
	ret->external.ptr = ptr; /* because its a union, it should be the same as setting fun as well */
	ret->external.destroy_cb = destroy_cb;


	return ret;
}

boing_value_t *boing_value_from_ptr_managed(boing_t *boing, void *ptr, int (*destroy_cb)(boing_t *boing, boing_value_t *value))
{
	boing_value_t *ret = NULL;
	boing_managed_ptr_t *container = NULL;

	if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_EXTERNAL, 0)))
	{
		boing_error(boing, 0, "could not request external value");
		return NULL;
	}

	if(!(container = (boing_managed_ptr_t *)boing_str_request(boing, sizeof(boing_managed_ptr_t))))
	{
		boing_error(boing, 0, "could not request mem for a managed pointer container");
		return NULL;
	}

	container->references = 1;
	container->ptr = ptr;
	container->extra = NULL;
	container->destroy_cb = destroy_cb;

	ret->external.type = BOING_EXTERNAL_POINTER;
	ret->external.ptr = container; /* because its a union, it should be the same as setting fun as well */
	ret->external.destroy_cb = &boing_external_decrement_managed_cleanup;
	ret->external.reference_inc_cb = &boing_external_increment_managed;

	return ret;
}

boing_value_t *boing_value_from_args(boing_t *boing, int argc, char **argv)
{
	boing_value_t *ret = NULL, *str = NULL;
	int i;

	/* NOTE: the host should pass everything past and including the script file path.
	If the interpreter gets arguments before the filepath and also the binary, it should
	skip those */

	if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, argc)))
	{
		boing_error(boing, 0, "could not request args array for argc & argv conversion");
		return NULL;
	}

	for(i = 0; i < argc; ++i)
	{
		if(!(str = boing_value_from_str(boing, argv[i])))
		{
			boing_error(boing, 0, "could not create string in arg converter on arg %d", i);
			return NULL;
		}

		if(boing_value_array_append_insert(boing, ret, i, str))
		{
			boing_error(boing, 0, "could not insert converted arg into the array");
			return NULL;
		}
	}
	

	return ret;
}

boing_value_t *boing_value_from_double(boing_t *boing, double value)
{
	boing_value_t *ret = NULL;

	if((ret = boing_value_request(boing, BOING_TYPE_VALUE_NUMBER, 0)))
		ret->number = value;

	return ret;
}

boing_value_t *boing_value_from_strn(boing_t *boing, char *str, size_t length)
{
	boing_value_t *ret = NULL;
	size_t i;

	if((ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, length)))
	{
		for(i = 0; i < length; ++i)
		{
			if(boing_value_array_append_insert(boing, ret, ret->length, boing_value_from_double(boing, (double)str[i])))
			{
				boing_error(boing, 0, "problem adding character to array");
				return NULL;

				/* TODO cleanup the array fragment? */
			}
		}
	}

	return ret;
}

boing_value_t *boing_value_from_str(boing_t *boing, char *str)
{
	return boing_value_from_strn(boing, str, strlen(str));
}

/* NOTE: while it may work in the future, strings and format substitutions cannot coexist
in the same table key/row.

example:
works: "row name|%s" or "%s|hey,%s|%d"
doesnt work: "row %s place|thing %d"
*/
boing_value_t *boing_value_from_table_fmt(boing_t *boing, char *fmt, ...)
{
	int table_progress = 0, escaped = 0;;
	char *start = NULL, char_holder = 0;
	va_list va;
	boing_value_t *ret = NULL, *key = NULL, *value = NULL, *row = NULL, **select = NULL;


	if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 1)))
	{
		boing_error(boing, 0, "could not request array for table from str");
		return NULL;
	}


	/* TODO: unescape %| %, %% in the actual key/value->boing value */

	va_start(va, fmt);

	do
	{
		switch(*fmt)
		{
			case '%':
				if(!table_progress)
					select = &key;
				else
					select = &value;
				

				switch(*(fmt + 1))
				{
					case 'd':
					case 'i':
						/* the next next arg is an signed int */
						if(!(*select = boing_value_from_double(boing, (double)va_arg(va, int))))
						{
							boing_error(boing, 0, "could not create value from int");
							if(key)
								boing_value_reference_dec(boing, key);
							if(value)
								boing_value_reference_dec(boing, value);
							boing_value_reference_dec(boing, ret);
							return NULL;
						}
					break;
					case 'u':
						/* the next arg is an unsigned int */
						if(!(*select = boing_value_from_double(boing, (double)va_arg(va, unsigned int))))
						{
							boing_error(boing, 0, "could not create value from unsigned int");
							if(key)
								boing_value_reference_dec(boing, key);
							if(value)
								boing_value_reference_dec(boing, value);
							boing_value_reference_dec(boing, ret);
							return NULL;
						}
					break;
					case 'f':
						/* the next arg is a float/double int */
						if(!(*select = boing_value_from_double(boing, va_arg(va, double))))
						{
							boing_error(boing, 0, "could not create value from double");
							if(key)
								boing_value_reference_dec(boing, key);
							if(value)
								boing_value_reference_dec(boing, value);
							boing_value_reference_dec(boing, ret);
							return NULL;
						}
					break;
					case 'c':
						/* the next arg is a char (string) */
						char_holder = (char)va_arg(va, int);

						if(!(*select = boing_value_from_strn(boing, &char_holder, 1)))
						{
							boing_error(boing, 0, "could not create value from char");
							if(key)
								boing_value_reference_dec(boing, key);
							if(value)
								boing_value_reference_dec(boing, value);
							boing_value_reference_dec(boing, ret);
							return NULL;
						}
					break;
					case 'p':
						/* the next arg is a pointer (external type) */
						if(!(*select = boing_value_from_ptr(boing, va_arg(va, void *), BOING_EXTERNAL_POINTER, NULL)))
						{
							boing_error(boing, 0, "could not create external value from pointer");
							if(key)
								boing_value_reference_dec(boing, key);
							if(value)
								boing_value_reference_dec(boing, value);
							boing_value_reference_dec(boing, ret);
							return NULL;
						}
					break;
					case 's':
						/* the next arg is a string */
						if(!(*select = boing_value_from_str(boing, va_arg(va, char *))))
						{
							boing_error(boing, 0, "could not create value from string");
							if(key)
								boing_value_reference_dec(boing, key);
							if(value)
								boing_value_reference_dec(boing, value);
							boing_value_reference_dec(boing, ret);
							return NULL;
						}
					break;
					case 'v':
						/* the next arg is a boing value */
						if(!(*select = va_arg(va, void *)))
						{
							boing_error(boing, 0, "could not assign select with passed value as it was null");
							if(key)
								boing_value_reference_dec(boing, key);
							if(value)
								boing_value_reference_dec(boing, value);
							boing_value_reference_dec(boing, ret);
							return NULL;
						}
					break;
					case '%':
					case '|':
					case ',':
						++escaped;
					break;
				}

				++fmt;
			break;
			case '|':
				if(!table_progress && start && !key) /* assembling key from string */
				{
					if(!(key = boing_value_from_strn(boing, start, fmt - start)))
					{
						boing_error(boing, 0, "could not create key from string");
						boing_value_reference_dec(boing, ret);
						return NULL;
					}

					escaped = 0;
					start = NULL;
				}
				else if(!key)
				{
					boing_error(boing, 0, "incorrect format for table -- expected key");
					boing_value_reference_dec(boing, ret);
					return NULL;
				}

				++table_progress;
			break;
			case '\0':
			case ',':
				/* handle the value and also the row value */
				if(table_progress == 1 && start && !value)
				{
					if(!(value = boing_value_from_strn(boing, start, fmt - start)))
					{
						boing_error(boing, 0, "could not create value from string");
						if(key)
							boing_value_reference_dec(boing, key);
						boing_value_reference_dec(boing, ret);
						return NULL;
					}

					escaped = 0;
					start = NULL;
				}
				else if(!value)
				{
					boing_error(boing, 0, "incorrect format for table -- expected value");
					if(key)
							boing_value_reference_dec(boing, key);
					boing_value_reference_dec(boing, ret);
					return NULL;
				}

				/* add key and value to a row, then that row to the table */
				if(!(row = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 2)))
				{
					boing_error(boing, 0, "could not create row array");
					if(key)
						boing_value_reference_dec(boing, key);
					if(value)
						boing_value_reference_dec(boing, value);
					boing_value_reference_dec(boing, ret);
					return NULL;
				}

				/* key insert into row */
				if(boing_value_array_append_insert(boing, row, 0, key))
				{
					boing_error(boing, 0, "could not append key to row");
					if(key)
						boing_value_reference_dec(boing, key);
					if(value)
						boing_value_reference_dec(boing, value);
					boing_value_reference_dec(boing, row);
					boing_value_reference_dec(boing, ret);
					return NULL;
				}

				/* value insert into row */
				if(boing_value_array_append_insert(boing, row, 1, value))
				{
					boing_error(boing, 0, "could not append value to row");
					if(key)
						boing_value_reference_dec(boing, key);
					if(value)
						boing_value_reference_dec(boing, value);
					boing_value_reference_dec(boing, row);
					boing_value_reference_dec(boing, ret);
					return NULL;
				}


				/* append row to main table */
				if(boing_value_array_append_insert(boing, ret, row->length, row))
				{
					boing_error(boing, 0, "could not append row to ret table");
					if(key)
						boing_value_reference_dec(boing, key);
					if(value)
						boing_value_reference_dec(boing, value);
					boing_value_reference_dec(boing, row);
					boing_value_reference_dec(boing, ret);
					return NULL;
				}

				key = value = row = NULL;

				table_progress = 0;
			break;
			default:
				if(!start)
					start = fmt;
			break;
		}

	} while(*fmt && ++fmt);

	va_end(va);


	return ret;
}

char *boing_str_from_value_array_dest(boing_t *boing, boing_value_t *value, char *dest)
{
	size_t i, j;


	if(!value)
	{
		boing_error(boing, 0, "str from value recieved null value");
		return NULL;
	}

	if(value->type == BOING_TYPE_VALUE_ARRAY)
	{
		for(j = i = 0; i < value->length; ++i,++j)
		{
			if(!value->array[i])
			{
				boing_error(boing, 0, "value in array is null");
				return NULL;
			}

			/* skip non-numbers */
			if(value->array[i]->type != BOING_TYPE_VALUE_NUMBER)
			{
				j--;
				continue;
			}
			
			dest[j] = (char)value->array[i]->number;
		}
	}

	return dest;
}

char *boing_str_from_value_array(boing_t *boing, boing_value_t *value)
{
	char *ret = NULL;
	

	if((ret = boing_str_request(boing, value->length + 1)))
	{
		memset(ret, 0, sizeof(char) * (value->length + 1));

		if(!(boing_str_from_value_array_dest(boing, value, ret)))
		{
			boing_error(boing, 0, "problem turning array into string");
			boing_str_release(boing, ret);
			return NULL;
		}
	}

	return ret;
}

/* TODO: repeat less code */
char *boing_str_from_value_readable(boing_t *boing, boing_value_t *value, uint8_t *previous_type, uint8_t style, uint32_t level)
{
	char *ret = NULL, *temp = NULL, *resize = NULL, *fmt = NULL, indentation[256] = {0}, indentation_inside[256] = {0}; /* set a maximum indentation but remember to check it */
	size_t i, j;


	#define REFORMAT(replacee, replacement)	\
		case replacee:	\
			if(!(resize = boing_str_resize(boing, temp, strlen(temp) + 2)))	\
			{	\
				boing_error(boing, 0, "could not resize string for string format");	\
				return NULL;	\
			}	\
			temp = resize;	\
			memmove(&temp[j + 1], &temp[j], strlen(temp) - j + 1);	\
			/* write the literal */	\
			temp[j] = '\\';	\
			temp[j + 1] = replacement;	\
			++j;	\
		break



	for(i = 0; i < 255; ++i)
	{
		indentation[i] = '\t';
		if(i + 1 > level)
			break;
	}

	indentation[i] = 0x0;

	for(i = 0; i < 255; ++i)
	{
		indentation_inside[i] = '\t';
		if(i + 1 > level + 1)
			break;
	}

	indentation_inside[i] = 0x0;

	switch(value->type)
	{
		case BOING_TYPE_VALUE_NUMBER:
			if(((double)INT_MIN <= value->number && (double)INT_MAX >= value->number) && *previous_type == BOING_TYPE_VALUE_NUMBER && floor(value->number) == value->number && value->number >= 0.0)
			{
				if(!(ret = boing_str_sprintf(boing, " %d", (int)value->number)))
				{
					boing_error(boing, 0, "could not convert number into string");
					return NULL;
				}
			}
			else if(((double)INT_MIN <= value->number && (double)INT_MAX >= value->number) && floor(value->number) == value->number && value->number >= 0.0)
			{
				if(!(ret = boing_str_sprintf(boing, "%d", (int)value->number)))
				{
					boing_error(boing, 0, "could not convert number into string");
					return NULL;
				}
			}
			else
			{
				if(!(ret = boing_str_sprintf(boing, "'%f'", value->number)))
				{
					boing_error(boing, 0, "could not convert number into string");
					return NULL;
				}
			}
			
			

			*previous_type = BOING_TYPE_VALUE_NUMBER;
		break;
		case BOING_TYPE_VALUE_EXTERNAL:
			/* just put NULL because it should be impossible to write an external literal */
			if(*previous_type == BOING_TYPE_VALUE_OPERATION) /* good chance its an identifier op */
				fmt = " NULL";
			else
				fmt = "NULL";

			if(!(ret = boing_str_sprintf(boing, fmt)))
			{
				boing_error(boing, 0, "could not convert string into string");
				return NULL;
			}

			*previous_type = BOING_TYPE_VALUE_OPERATION;
		break;
		case BOING_TYPE_VALUE_OPERATION:
			/* make sure th eoperation exists */
			if(!boing_operation_data(boing, value->operation.op))
			{
				boing_error(boing, 0, "could not convert operation to string because operation does not exist");
				return NULL;
			}
			
			switch(value->operation.op)
			{
				case '{':
					switch(style)
					{
						case BOING_VALUE_STRING_MINIFIED:
							if(!(ret = boing_str_sprintf(boing, "{")))
							{
								boing_error(boing, 0, "could not convert operation '{' into string");
								return NULL;
							}

							/* solution for space after operation starts */
							*previous_type = -1;

							for(j = 0; j < value->operation.args_value->length; ++j)
							{
								if(!(temp = boing_str_from_value_readable(boing, value->operation.args_value->array[j], previous_type, style, level + 1)))
								{
									boing_error(boing, 0, "could not turn member of '{' operation arguments into string");
									boing_str_release(boing, ret);
									return NULL;
								}

								if(boing_str_ncat(boing, &ret, temp, strlen(temp)))
								{
									boing_error(boing, 0, "could not concat string in readable function");
									boing_str_release(boing, ret);
									return NULL;
								}

								if(boing_str_release(boing, temp))
								{
									boing_error(boing, 0, "could not release recursive in readable function");
									boing_str_release(boing, ret);
									return NULL;
								}
							}

							if(boing_str_ncat(boing, &ret, "}", 1))
							{
								boing_error(boing, 0, "could not convert operation '{' into string");
								boing_str_release(boing, ret);
								return NULL;
							}
						break;
						case BOING_VALUE_STRING_READABLE:
							if(!(ret = boing_str_sprintf(boing, "\n%s{\n", indentation, indentation)))
							{
								boing_error(boing, 0, "could not convert operation '{' into string");
								return NULL;
							}

							*previous_type = -1;

							for(j = 0; j < value->operation.args_value->length; ++j)
							{
								if(!(temp = boing_str_from_value_readable(boing, value->operation.args_value->array[j], previous_type, style, level + 1)))
								{
									boing_error(boing, 0, "could not turn member of '{' operation arguments into string");
									boing_str_release(boing, ret);
									return NULL;
								}

								if(boing_str_ncat(boing, &ret, indentation_inside, strlen(indentation_inside)))
								{
									boing_error(boing, 0, "could not concat indentation string in readable function");
									boing_str_release(boing, ret);
									return NULL;
								}

								if(boing_str_ncat(boing, &ret, temp, strlen(temp)))
								{
									boing_error(boing, 0, "could not concat string in readable function");
									boing_str_release(boing, ret);
									return NULL;
								}

								if(boing_str_release(boing, temp))
								{
									boing_error(boing, 0, "could not release recursive in readable function");
									boing_str_release(boing, ret);
									return NULL;
								}

								if(boing_str_ncat(boing, &ret, "\n", 1))
								{
									boing_error(boing, 0, "could not concat string in readable function");
									boing_str_release(boing, ret);
									return NULL;
								}

								if(j + 1 >= value->operation.args_value->length && boing_str_ncat(boing, &ret, indentation, strlen(indentation)))
								{
									boing_error(boing, 0, "could not concat later indentation in readable function");
									boing_str_release(boing, ret);
									return NULL;
								}
							}

							if(boing_str_ncat(boing, &ret, "}\n", 2))
							{
								boing_error(boing, 0, "could not convert operation '{' into string");
								boing_str_release(boing, ret);
								return NULL;
							}
						break;
					}
					*previous_type = BOING_TYPE_VALUE_OPERATION;
				break;
				case '[':
					if(!(ret = boing_str_sprintf(boing, "[")))
					{
						boing_error(boing, 0, "could not convert operation '[' into string");
						return NULL;
					}

					*previous_type = -1;

					for(j = 0; j < value->operation.args_value->length; ++j)
					{
						if(!(temp = boing_str_from_value_readable(boing, value->operation.args_value->array[j], previous_type, style, level + 1)))
						{
							boing_error(boing, 0, "could not turn member of '[' operation arguments into string");
							boing_str_release(boing, ret);
							return NULL;
						}

						if(boing_str_ncat(boing, &ret, temp, strlen(temp)))
						{
							boing_error(boing, 0, "could not concat string in readable function");
							boing_str_release(boing, ret);
							return NULL;
						}

						if(boing_str_release(boing, temp))
						{
							boing_error(boing, 0, "could not release recursive in readable function");
							boing_str_release(boing, ret);
							return NULL;
						}
					}

					if(boing_str_ncat(boing, &ret, "]", 1))
					{
						boing_error(boing, 0, "could not convert operation '[' into string");
						boing_str_release(boing, ret);
						return NULL;
					}

					*previous_type = BOING_TYPE_VALUE_OPERATION;
				break;
				case '(':
					if(!(ret = boing_str_sprintf(boing, "(")))
					{
						boing_error(boing, 0, "could not convert operation '(' into string");
						return NULL;
					}

					*previous_type = -1;

					for(j = 0; j < value->operation.args_value->length; ++j)
					{
						if(!(temp = boing_str_from_value_readable(boing, value->operation.args_value->array[j], previous_type, style, level + 1)))
						{
							boing_error(boing, 0, "could not turn member of '(' operation arguments into string");
							boing_str_release(boing, ret);
							return NULL;
						}

						if(boing_str_ncat(boing, &ret, temp, strlen(temp)))
						{
							boing_error(boing, 0, "could not concat string in readable function");
							boing_str_release(boing, ret);
							return NULL;
						}

						if(boing_str_release(boing, temp))
						{
							boing_error(boing, 0, "could not release recursive in readable function");
							boing_str_release(boing, ret);
							return NULL;
						}
					}

					if(boing_str_ncat(boing, &ret, ")", 1))
					{
						boing_error(boing, 0, "could not convert operation '(' into string");
						boing_str_release(boing, ret);
						return NULL;
					}

					*previous_type = BOING_TYPE_VALUE_OPERATION;
				break;
				case 'A':
					if(!value->operation.args_value->length || !(temp = boing_str_from_value_array(boing, value->operation.args_value->array[0])))
					{
						boing_error(boing, 0, "could not make string from the value array in identifier in readable function");
						return NULL;
					}

					if(*previous_type == BOING_TYPE_VALUE_OPERATION) /* theres a chance that the last operation was an identifier */
					{
						if(!(ret = boing_str_sprintf(boing, " %s", temp)))
						{
							boing_error(boing, 0, "could not concat string in identifier in readable function");
							boing_str_release(boing, ret);
							return NULL;
						}

						if(boing_str_release(boing, temp))
						{
							boing_error(boing, 0, "could not release temp string in identifier in readable function");
							boing_str_release(boing, ret);
							return NULL;
						}
					}
					else
						ret = temp;

					*previous_type = BOING_TYPE_VALUE_OPERATION;
				break;
				default: /* TODO: if shorter than expected, do the unmatched closing char size optimization */
					switch(style)
					{
						case BOING_VALUE_STRING_MINIFIED:
							if(!(ret = boing_str_sprintf(boing, "%c%s", value->operation.op, (value->operation.args_value->length != boing_operation_arg_amount(boing, value->operation.op) ? "(" : ""))))
							{
								boing_error(boing, 0, "could not convert operation '(' into string");
								return NULL;
							}

							*previous_type = -1;

							for(j = 0; j < value->operation.args_value->length; ++j)
							{
								if(!(temp = boing_str_from_value_readable(boing, value->operation.args_value->array[j], previous_type, style, level + 1)))
								{
									boing_error(boing, 0, "could not turn member of '(' operation arguments into string");
									boing_str_release(boing, ret);
									return NULL;
								}

								if(boing_str_ncat(boing, &ret, temp, strlen(temp)))
								{
									boing_error(boing, 0, "could not concat string in readable function");
									boing_str_release(boing, ret);
									return NULL;
								}

								if(boing_str_release(boing, temp))
								{
									boing_error(boing, 0, "could not release recursive in readable function");
									boing_str_release(boing, ret);
									return NULL;
								}
							}

							if(boing_str_ncat(boing, &ret, (value->operation.args_value->length != boing_operation_arg_amount(boing, value->operation.op) ? ")" : ""), (value->operation.args_value->length != boing_operation_arg_amount(boing, value->operation.op) ? 1 : 0)))
							{
								boing_error(boing, 0, "could not convert operation '(' into string");
								boing_str_release(boing, ret);
								return NULL;
							}

							if(value->operation.args_value->length != boing_operation_arg_amount(boing, value->operation.op))
								*previous_type = BOING_TYPE_VALUE_OPERATION;
						break;
						case BOING_VALUE_STRING_READABLE:
							if(!(ret = boing_str_sprintf(boing, "%c%s", value->operation.op, (boing_operation_data(boing, value->operation.op))->can_explicit ? "(" : "")))
							{
								boing_error(boing, 0, "could not convert operation '(' into string");
								return NULL;
							}

							*previous_type = -1;

							for(j = 0; j < value->operation.args_value->length; ++j)
							{
								if(!(temp = boing_str_from_value_readable(boing, value->operation.args_value->array[j], previous_type, style, level + 1)))
								{
									boing_error(boing, 0, "could not turn member of '(' operation arguments into string");
									boing_str_release(boing, ret);
									return NULL;
								}

								if(boing_str_ncat(boing, &ret, temp, strlen(temp)))
								{
									boing_error(boing, 0, "could not concat string in readable function");
									boing_str_release(boing, ret);
									return NULL;
								}

								if(boing_str_release(boing, temp))
								{
									boing_error(boing, 0, "could not release recursive in readable function");
									boing_str_release(boing, ret);
									return NULL;
								}
							}

							if(boing_str_ncat(boing, &ret, (boing_operation_data(boing, value->operation.op))->can_explicit ? ")" : "", (boing_operation_data(boing, value->operation.op))->can_explicit ? 1 : 0))
							{
								boing_error(boing, 0, "could not convert operation '(' into string");
								boing_str_release(boing, ret);
								return NULL;
							}

							*previous_type = BOING_TYPE_VALUE_OPERATION;
						break;
					}
				break;
			}
		break;
		case BOING_TYPE_VALUE_ARRAY: /* strong case for being a string literal. Be sure to check though */
			/* make sure it doesnt contain any possible non-characters */
			for(i = 0; i < value->length; ++i)
			{
				if(value->array[i]->type != BOING_TYPE_VALUE_NUMBER)
					break;
				else if(value->array[i]->type == BOING_TYPE_VALUE_NUMBER && (value->array[i]->number < 0 || floor(value->array[i]->number) != value->array[i]->number || value->array[i]->number > 255))
					break;
			}

			if(i == value->length) /* its a string literal */
			{
				if(!(temp = boing_str_from_value_array(boing, value)))
				{
					boing_error(boing, 0, "could not create string literal in readable function");
					return NULL;
				}
				
				/* escape characters */
				/* TODO, need to somehow determine the actual string but stop at repeating 0's. Getting the container size wont work */
				for(j = 0; j < strlen(temp); ++j)
				{
					switch(temp[j])
					{
						REFORMAT('\a', 'a');
						REFORMAT('\e', 'e');
						REFORMAT('\'', '\'');
						REFORMAT('\"', '\"');
						REFORMAT('\\', '\\');
						REFORMAT('\n', 'n');
						REFORMAT('\r', 'r');
						REFORMAT('\t', 't');
						REFORMAT('\v', 'v');
						REFORMAT('\b', 'b');
						REFORMAT('\f', 'f');
						REFORMAT('\0', '0');
					}
				}
				

				if(!(ret = boing_str_sprintf(boing, "\"%s\"", temp)))
				{
					boing_error(boing, 0, "could not finalize string literal in readable function");
					boing_str_release(boing, temp);
					return NULL;
				}

				if(boing_str_release(boing, temp))
				{
					boing_error(boing, 0, "could not release recursive in readable function");
					boing_str_release(boing, ret);
					return NULL;
				}
			}
			else /* treat it like an array literal */
			{
				if(!(ret = boing_str_sprintf(boing, "[")))
				{
					boing_error(boing, 0, "could not convert operation '[' into string");
					return NULL;
				}

				*previous_type = -1;

				for(j = 0; j < value->length; ++j)
				{
					if(!(temp = boing_str_from_value_readable(boing, value->array[j], previous_type, style, level + 1)))
					{
						boing_error(boing, 0, "could not turn member of '[' operation arguments into string");
						boing_str_release(boing, ret);
						return NULL;
					}

					if(boing_str_ncat(boing, &ret, temp, strlen(temp)))
					{
						boing_error(boing, 0, "could not concat string in readable function");
						boing_str_release(boing, ret);
						return NULL;
					}

					if(boing_str_release(boing, temp))
					{
						boing_error(boing, 0, "could not release recursive in readable function");
						boing_str_release(boing, ret);
						return NULL;
					}
				}

				if(boing_str_ncat(boing, &ret, "]", 1))
				{
					boing_error(boing, 0, "could not convert operation '[' into string");
					boing_str_release(boing, ret);
					return NULL;
				}
			}

			*previous_type = BOING_TYPE_VALUE_ARRAY;
		break;
	}

	return ret;
}

double boing_number_from_value_array(boing_t *boing, boing_value_t *value)
{
	char *str = NULL;
	double ret = 0;


	/* turn the string (array) into a c string and then strtod it */
	if(!(str = boing_str_from_value_array(boing, value)))
	{
		boing_error(boing, 0, "problem turning array into string for num from str");
		return 0;
	}

	ret = strtod(str, NULL);

	if(boing_str_release(boing,str))
	{
		boing_error(boing, 0, "could not release conversion string in num from str");
		return 0;
	}


	return ret;
}

int boing_external_decrement_value_cleanup(boing_t *boing, boing_value_t *value)
{
	if(boing_value_reference_dec(boing, (boing_value_t *)value->external.ptr))
	{
		boing_error(boing, 0, "could not decrement value that was used in an external value");
		return 1;
	}

	return 0;
}

int boing_external_increment_value(boing_t *boing, boing_value_t *value)
{
	boing_value_reference_inc(boing, (boing_value_t *)value->external.ptr);

	return 0;
}

int boing_external_decrement_managed_cleanup(boing_t *boing, boing_value_t *value)
{
	if(!((boing_managed_ptr_t *)value->external.ptr)->references || --(((boing_managed_ptr_t *)value->external.ptr)->references) <= 0)
	{
		if(((boing_managed_ptr_t *)value->external.ptr)->destroy_cb(boing, value))
		{
			boing_error(boing, 0, "could not clean up managed ptr cleanly");
			return 1;
		}

		/* destroy the struct */
		if(boing_str_release(boing, value->external.ptr))
		{
			boing_error(boing, 0, "could not clean up managed ptr struct");
			return 1;
		}
	}

	return 0;
}

int boing_external_increment_managed(boing_t *boing, boing_value_t *value)
{
	((boing_managed_ptr_t *)value->external.ptr)->references++;

	return 0;
}

int boing_value_compare(boing_t *boing, boing_value_t *lhs, boing_value_t *rhs)
{
	size_t i;
	int cmp;
	if(lhs->type != rhs->type)
	{
		/* return just not equal. Types need to be recursively exactly the same types,
		but can have different values and lengths */
		return BOING_COMPARISON_NOT_EQUAL;
	}
	
	/* special type specific compares. generally, if it has child values, lengths will
	be compared first */

	switch(lhs->type)
	{
		case BOING_TYPE_VALUE_ARRAY:
			/* determine length and return if not equal */
			if(lhs->length != rhs->length)
			{
				if(lhs->length < rhs->length)
					return BOING_COMPARISON_LESS_THAN;
				else if(lhs->length > rhs->length)
					return BOING_COMPARISON_GREATER_THAN;
			}

			/* look inside each value */
			for(i = 0; i < lhs->length; ++i)
			{
				if((cmp = boing_value_compare(boing, lhs->array[i], rhs->array[i])) != BOING_COMPARISON_EQUAL)
					return cmp;
			}

		break;
		case BOING_TYPE_VALUE_NUMBER:
			/* this feels like a naive method */
			if(lhs->number < rhs->number)
				return BOING_COMPARISON_LESS_THAN;
			else if(lhs->number > rhs->number)
				return BOING_COMPARISON_GREATER_THAN;

		break;
		case BOING_TYPE_VALUE_OPERATION:
			/* determine if even the same operation */
			if(lhs->operation.op != rhs->operation.op)
				return BOING_COMPARISON_NOT_EQUAL;

			/* compare the argument array */
			if((cmp = boing_value_compare(boing, lhs->operation.args_value, rhs->operation.args_value)) != BOING_COMPARISON_EQUAL)
					return cmp;

		break;
		case BOING_TYPE_VALUE_EXTERNAL:
			return BOING_COMPARISON_NOT_EQUAL; /* never compare external types */
		break;
	}

	return BOING_COMPARISON_EQUAL;
}

int boing_value_array_search(boing_t *boing, boing_value_t *array, boing_value_t *needle, size_t offset, size_t *pos)
{
	size_t i, j;


	/* focus toward "strings" so array needles will use their elements instead */
	if(needle->type == BOING_TYPE_VALUE_ARRAY && needle->length > 0)
	{
		if(needle->length <= array->length)
		{
			/* offset the compare start */
			for(i = offset; i < (array->length - needle->length) + 1; ++i)
			{
				/* compare all elements */
				for(j = 0; j < needle->length; ++j)
				{
					if(boing_value_compare(boing, array->array[i + j], needle->array[j]) != BOING_COMPARISON_EQUAL)
						break;
				}

				if(j == needle->length)
				{
					if(pos)
						*pos = i;
					return 1;
				}
			}
		}
	}
	else if(needle->type != BOING_TYPE_VALUE_ARRAY)
	{
		for(i = offset; i < array->length; ++i)
		{
			if(boing_value_compare(boing, array->array[i], needle) == BOING_COMPARISON_EQUAL)
			{
				if(pos)
					*pos = i;
				return 1;
			}
		}
	}


	return 0;
}

/* this is FNV-1a */
size_t boing_value_hash_continue(boing_t *boing, boing_value_t *value, size_t start)
{
	/* 64 bit hashing only because it will end up as a double */
	size_t ret = start;
	size_t i;
	uint8_t *ptr = NULL;

	#define HASH_LOOP(member, size)	\
		do	\
		{	\
			for(i = 0; i < size; ++i)	\
			{	\
				ret ^= ((uint8_t *)&member)[i];	\
				ret *= BOING_FNV_PRIME;	\
			}	\
		} while(0);


	ptr = (uint8_t *)value;


	/* avoid hashing pointers because they will never match between values unless
	comparing itself */

	HASH_LOOP(value->length, sizeof(value->length));
	HASH_LOOP(value->type, sizeof(value->type));

	/* recurse down value members */
	switch(value->type)
	{
		case BOING_TYPE_VALUE_ARRAY:
			for(i = 0; i < value->length; ++i)
				ret = boing_value_hash_continue(boing, value->array[i], ret);
		break;
		case BOING_TYPE_VALUE_OPERATION:
			HASH_LOOP(value->operation.op, sizeof(value->operation.op));

			ret = boing_value_hash_continue(boing, value->operation.args_value, ret);
		break;
		case BOING_TYPE_VALUE_NUMBER:
			HASH_LOOP(value->number, sizeof(value->number));
		break;
		case BOING_TYPE_VALUE_EXTERNAL:
			HASH_LOOP(value->external, sizeof(value->external));
		break;
	}


	return ret;
}

size_t boing_value_hash(boing_t *boing, boing_value_t *value)
{
	/* 64 bit hashing only because it will end up as a double */
	return boing_value_hash_continue(boing, value, BOING_FNV_OFFSET);
}

int boing_is_operation(boing_t *boing, char op)
{
	uint8_t i;

	if(!boing->operations)
	{
		boing_error(boing, 0, "unable to test op due to uninitialized boing boing");
		return 0;
	}

	
	for(i = 0; i < boing->operation_amount; ++i)
		if(op == boing->operations[i].op)
			return 1;


	return 0;
}

boing_operation_data_t *boing_operation_data(boing_t *boing, char op)
{
	uint8_t i;

	if(!boing->operations)
	{
		boing_error(boing, 0, "unable to test op due to uninitialized boing boing");
		return NULL;
	}

	for(i = 0; i < boing->operation_amount; ++i)
		if(op == boing->operations[i].op)
			return &boing->operations[i];
	
	return NULL;
}

int boing_operation_has_explicit(boing_t *boing, char op)
{
	uint8_t i;

	if(!boing->operations)
	{
		boing_error(boing, 0, "unable to test op due to uninitialized boing boing");
		return 0;
	}

	
	for(i = 0; i < boing->operation_amount; ++i)
		if(op == boing->operations[i].op && boing->operations[i].can_explicit)
			return 1;


	return 0;
}

/* see how many implicit arguments are required for the current operation */
size_t boing_operation_arg_amount(boing_t *boing, char op)
{
	uint8_t i;

	if(!boing->operations)
	{
		boing_error(boing, 0, "unable to test op due to uninitialized boing boing");
		return 0;
	}

	for(i = 0; i < boing->operation_amount; ++i)
		if(op == boing->operations[i].op)
			return boing->operations[i].arg_amount;
	
	return 0;
}

void boing_consume_whitespace(boing_t *boing, char **str)
{
	do
	{
		if(!isspace(**str))
			return;
	} while(**str && ++(*str));
}

boing_value_t *boing_parse_number(boing_t *boing, char **str)
{
	boing_value_t *ret = NULL;
	uint8_t quoted = 0;
	char *start = NULL, *temp = NULL;

	if(**str == '\'')
	{
		quoted = 1;
		++(*str);
	}

	start = *str;

	do
	{
		/* allows decimal and other representations */
		/* only allows integers if unquoted */
		if((quoted && **str == '\'') || (!quoted && (**str > 57 || **str < 48)) || !(**str))
		{
			/* if(!(temp = malloc(*str - start + 1))) */
			if(!(temp = boing_str_request(boing, *str - start + 1)))
			{
				boing_error(boing, 0, "error allocating number string");
				return NULL;
			}
			memset(temp, 0, *str - start + 1);
			strncpy(temp, start, *str - start);
			ret = boing_value_from_double(boing, atof(temp));

			/* release now because the string is no longer used */
			if(boing_str_release(boing, temp))
			{
				boing_error(boing, 0, "could not release string back to pool");
				/* well, we successfully got the value so might as well just issue
				a warning. Not the sign of a healthy pool :/ */
			}

			/* bad hack, but TODO, look ahead rather than current */
			if(!quoted)
				--(*str);

			break;
		}
	} while(**str && ++(*str));


	return ret;
}

boing_value_t *boing_parse_string(boing_t *boing, char **str)
{
	boing_value_t *ret = NULL;
	char *start = NULL, *temp = NULL, escaped = 0, wasescaped = 0;

	#define FORMAT(replace, replacement)	\
		if(boing_str_replace(boing, &temp, replace, replacement))	\
		{	\
			boing_error(boing, 0, "could not replace "replace);	\
			return NULL;	\
		}

	++(*str);

	start = *str;

	do
	{
		if(**str == '\\' && !escaped)
		{
			escaped = 1;
			wasescaped = 1;
			continue;
		}
		else if(escaped && **str)
		{
			escaped = 0;
			continue;
		}

		if(**str == '\"' || !**str)
		{
			/* check if "wasescaped" and format the string */
			if(wasescaped)
			{
				if(!(temp = boing_str_ndup(boing, start, *str - start)))
				{
					boing_error(boing, 0, "could not duplicate string from literal");
					return NULL;
				}

				/* format the escapes */
				FORMAT("\\a", "\a")
				FORMAT("\\e", "\e")
				FORMAT("\\'", "'")
				FORMAT("\\\"", "\"")
				FORMAT("\\\\", "\\")
				FORMAT("\\n", "\n")
				FORMAT("\\r", "\r")
				FORMAT("\\t", "\t")
				FORMAT("\\v", "\v")
				FORMAT("\\b", "\b")
				FORMAT("\\f", "\f")
				FORMAT("\\0", "\0")


				if(!(ret = boing_value_from_str(boing, temp)))
				{
					boing_error(boing, 0, "could not create string from literal");
					return NULL;
				}


				/* in tis case, we have to clean up the string used */
				if(boing_str_release(boing, temp))
				{
					boing_error(boing, 0, "could not release string from literal");
					return NULL;
				}
			}
			else
			{
				if(!(ret = boing_value_from_strn(boing, start, *str - start)))
				{
					boing_error(boing, 0, "could not create string from literal");
					return NULL;
				}
			}

			break;
		}
	} while(**str && ++(*str));


	return ret;
}

boing_value_t *boing_parse_identifier(boing_t *boing, char **str)
{
	boing_value_t *ret = NULL, *ident = NULL, *arr = NULL;
	char *start = NULL;


	/* look for A-Z, _ and only run until ends */
	do
	{
		if(((**str >= 'A' && **str <= 'Z') || **str == '_') && !start)
		{
			start = *str;
		}
		else if(!((**str >= 'A' && **str <= 'Z') || **str == '_') && start)
		{

			/* turn the identifier string into an array, add that array to an args array,
			then toss that args array into an operation */

			if(!(ident = boing_value_from_strn(boing, start, *str - start)))
			{
				boing_error(boing, 0, "could not allocate string for identifier");
				return NULL;
			}

			if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_OPERATION, 0)))
			{
				boing_error(boing, 0, "could not request operation value");
				return NULL;
			}

			if(boing_value_array_append_insert(boing, ret->operation.args_value, 0, ident))
			{
				boing_error(boing, 0, "could not insert the identifier string into the identifier operation argument array");
				return NULL;
			}

			ret->operation.op = 'A';

			--(*str); /* need to rewind */

			break;
		}
	} while(**str && ++(*str));

	return ret;
}

boing_value_t *boing_parse_from_string(boing_t *boing, boing_value_t *parent, boing_value_t *current, boing_value_t *script, char **str, size_t *line)
{
	boing_value_t *temp = NULL, *sublist = NULL;
	uint8_t comment = 0;


	if(!current) /* initialize the operation list */
	{
		if(!(temp = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 0)))
		{
			boing_error(boing, 0, "problem initializing the operation list");
			return NULL;
		}

		current = temp;
	}

	do
	{
		if(**str == '\n')
			(*line)++;
		

		if(comment && **str == '\n')
			comment = 0;
		else if(comment)
			continue;
		

		if(isspace(**str))
			continue;

		/* TODO: convert the number parser to like "} while(**str && *(++(*str)));" and delete this: */
		if(!**str)
			break;
		
		

		switch(**str)
		{
			case '#':
				comment = 1;
				continue;
			break;
			case '\"':
				if(!(temp = boing_parse_string(boing, str)))
				{
					boing_error(boing, 0, "could not parse string");
					return NULL;
				}
				if(boing_value_array_append_insert(boing, current, current->length, temp))
				{
					boing_error(boing, 0, "could not append string");
					return NULL;
				}

				#if BOING_ENABLE_LINE_NUM
					temp->line = *line;
					temp->script = script;
					boing_value_reference_inc(boing, script);
				#endif
			break;
			case '\'':
				if(!(temp = boing_parse_number(boing, str)))
				{
					boing_error(boing, 0, "could not parse number");
					return NULL;
				}
				if(boing_value_array_append_insert(boing, current, current->length, temp))
				{
					boing_error(boing, 0, "could not append number");
					return NULL;
				}

				#if BOING_ENABLE_LINE_NUM
					temp->line = *line;
					temp->script = script;
					boing_value_reference_inc(boing, script);
				#endif
			break;
			case '[':
				/* NOTE: these always are "created" at runtime. Everything is just passed to the internal array operation */
				if(!(temp = boing_value_request(boing, BOING_TYPE_VALUE_OPERATION, 0)))
				{
					boing_error(boing, 0, "could not request operation value for array literal");
					return NULL;
				}

				/* parse the list */
				++(*str);
				if(!(sublist = boing_parse_from_string(boing, temp, temp->operation.args_value, script, str, line)))
				{
					boing_error(boing, 0, "could not parse array operation arguments in literal");
					return NULL;
				}

				temp->operation.op = '[';

				/* always add the array even if it is empty */
				if(boing_value_array_append_insert(boing, current, current->length, temp))
				{
					boing_error(boing, 0, "could not append array");
					return NULL;
				}

				#if BOING_ENABLE_LINE_NUM
					temp->line = *line;
					temp->script = script;
					boing_value_reference_inc(boing, script);
				#endif
			break;
			case '{': /* NOTE: all operations are arguments to the internal operation block operation */
				/* parse operation block */
				if(!(temp = boing_value_request(boing, BOING_TYPE_VALUE_OPERATION, 0)))
				{
					boing_error(boing, 0, "could not request operation value for operation block");
					return NULL;
				}

				/* parse the list */
				++(*str);
				if(!(sublist = boing_parse_from_string(boing, temp, temp->operation.args_value, script, str, line)))
				{
					boing_error(boing, 0, "could not parse operation block arguments");
					return NULL;
				}

				temp->operation.op = '{';

				/* always add the block even if it is empty */
				if(boing_value_array_append_insert(boing, current, current->length, temp))
				{
					boing_error(boing, 0, "could not append block");
					return NULL;
				}

				#if BOING_ENABLE_LINE_NUM
					temp->line = *line;
					temp->script = script;
					boing_value_reference_inc(boing, script);
				#endif
			break;
			case '(': /* NOTE: these blocks will always run when passed and return the previous value rather than the { blocks which return their structure */
				/* parse runtime operation block */
				if(!(temp = boing_value_request(boing, BOING_TYPE_VALUE_OPERATION, 0)))
				{
					boing_error(boing, 0, "could not request operation value for runtime operation block");
					return NULL;
				}

				/* parse the list */
				++(*str);
				if(!(sublist = boing_parse_from_string(boing, temp, temp->operation.args_value, script, str, line)))
				{
					boing_error(boing, 0, "could not parse runtime block operation arguments");
					return NULL;
				}

				temp->operation.op = '(';

				/* always add the runtim eblock even if it is empty */
				if(boing_value_array_append_insert(boing, current, current->length, temp))
				{
					boing_error(boing, 0, "could not append runtime block");
					return NULL;
				}

				#if BOING_ENABLE_LINE_NUM
					temp->line = *line;
					temp->script = script;
					boing_value_reference_inc(boing, script);
				#endif
			break;
			case ']':
			case '}':
			case ')':
				return current;
			break;
			default:
				/* first, test for explicit integers */
				if(**str <= 57 && **str >= 48)
				{
					if(!(temp = boing_parse_number(boing, str)))
					{
						boing_error(boing, 0, "could not parse number");
						return NULL;
					}
					if(boing_value_array_append_insert(boing, current, current->length, temp))
					{
						boing_error(boing, 0, "could not append number");
						return NULL;
					}

					#if BOING_ENABLE_LINE_NUM
						temp->line = *line;
						temp->script = script;
						boing_value_reference_inc(boing, script);
					#endif
				}
				else if((**str >= 'A' && **str <= 'Z') || **str == '_') /* next, test for identifiers */
				{
					if(!(temp = boing_parse_identifier(boing, str)))
					{
						boing_error(boing, 0, "could not parse identifier");
						return NULL;
					}
					if(boing_value_array_append_insert(boing, current, current->length, temp))
					{
						boing_error(boing, 0, "could not append number");
						return NULL;
					}

					#if BOING_ENABLE_LINE_NUM
						temp->line = *line;
						temp->script = script;
						boing_value_reference_inc(boing, script);
					#endif
				}
				else if(boing_is_operation(boing, **str)) /* test for operations, and if ( follows for explicit argument lists */
				{
					if(!(temp = boing_value_request(boing, BOING_TYPE_VALUE_OPERATION, 0)))
					{
						boing_error(boing, 0, "could not allocate operation when parsing");
						return NULL;
					}

					temp->operation.op = **str;
					++(*str);


					/* parse arguments */
					boing_consume_whitespace(boing, str); /* read until next non-whitespace char */
					if(**str == '(' && boing_operation_has_explicit(boing, temp->operation.op)) /* there is an explicit end to the argument list */
					{
						++(*str);

						/* it is intentional that a parent is not passed. It wont know if its an explicit list if it is */
						if(!(sublist = boing_parse_from_string(boing, NULL, temp->operation.args_value, script, str, line)))
						{
							boing_error(boing, 0, "could not parse explicit arg list");
							return NULL;
						}
					}
					else if(boing_operation_arg_amount(boing, temp->operation.op))
					{
						if(!(sublist = boing_parse_from_string(boing, temp, temp->operation.args_value, script, str, line)))
						{
							boing_error(boing, 0, "could not parse implicit arg list");
							return NULL;
						}
					}


					if((boing_value_array_append_insert(boing, current, current->length, temp)))
					{
						boing_error(boing, 0, "could not insert operation");
						return NULL;
					}

					#if BOING_ENABLE_LINE_NUM
						temp->line = *line;
						temp->script = script;
						boing_value_reference_inc(boing, script);
					#endif
				}
				else
				{
					boing_error(boing, 0, "'%c' is not a valid operation on line %lu in %s", **str, *line, ((boing_script_t *)script->external.ptr)->name);
					return NULL;
				}
				
		}

		/* make sure this isn't going past the implicit arg defaults */
		if(parent && parent->type == BOING_TYPE_VALUE_OPERATION && current->length == boing_operation_arg_amount(boing, parent->operation.op))
			return current;
		

		temp = NULL;
		sublist = NULL;
	} while(**str && ++(*str));


	return current;
}

boing_value_t *boing_program_call_global(boing_t *boing, boing_value_t *program, char *identifier, boing_value_t *args)
{
	boing_value_t *ret = NULL, *ast = NULL, *stack = NULL, *temp = NULL, *previous = NULL;


	if(!(ast = boing_program_get_global(boing, program, identifier)))
		return NULL;
	
	/* setup new stack,  '_' and ARGS with the arguments, then eval */

	/* make sure args arent NULL. If they are, set it to 0 */
	if(!args)
	{
		if(!(temp = boing_value_from_double(boing, 0)))
		{
			boing_error(boing, 0, "could not initialize args0 to 0 after null passed");
			return NULL;
		}

		if(!(args = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 1)))
		{
			boing_error(boing, 0, "could not request an array for ARGS/_ value");
			return NULL;
		}

		if(boing_value_array_append_insert(boing, args, args->length, temp))
		{
			boing_error(boing, 0, "could not insert args value into array");
			return NULL;
		}
	}
	else
		boing_value_reference_inc(boing, args);



	/* make stack */
	if(!(stack = boing_value_stack_create(boing, program)))
	{
		boing_error(boing, 0, "could not create stack");
		return NULL;
	}


	/* add the args */

	/* add _ to new stack */
	if(!(temp = boing_value_from_str(boing, "_")))
	{
		boing_error(boing, 0, "could not create string for args '_' key");
		return NULL;
	}

	if(boing_value_stack_add_set(boing, stack, temp, args))
	{
		boing_error(boing, 0, "could not add args '_' key and value to stack");
		return NULL;
	}

	if(boing_value_reference_dec(boing, temp))
	{
		boing_error(boing, 0, "could not refdec '_' key");
		return NULL;
	}

	/* add ARGS to new stack */
	if(!(temp = boing_value_from_str(boing, "ARGS")))
	{
		boing_error(boing, 0, "could not create string for args 'ARGS' key");
		return NULL;
	}

	if(boing_value_stack_add_set(boing, stack, temp, args))
	{
		boing_error(boing, 0, "could not add args 'ARGS' key and value to stack");
		return NULL;
	}

	if(boing_value_reference_dec(boing, temp))
	{
		boing_error(boing, 0, "could not refdec 'ARGS' key");
		return NULL;
	}


	/* setup previous */
	if(!(previous = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not create previous value");
		return NULL;
	}


	/* eval whatevers in the table */
	if(!(ret = boing_eval_value(boing, program, ast, stack, previous)))
	{
		boing_error(boing, 0, "could not eval global");
		return NULL;
	}



	/* cleanup */

	if(boing_value_reference_dec(boing, ast))
	{
		boing_error(boing, 0, "could not refdec ast");
		return NULL;
	}

	if(boing_value_reference_dec(boing, stack))
	{
		boing_error(boing, 0, "could not clean up stack");
		return NULL;
	}

	if(boing_value_reference_dec(boing, previous))
	{
		boing_error(boing, 0, "could not clean up previous");
		return NULL;
	}

	if(boing_value_reference_dec(boing, args))
	{
		boing_error(boing, 0, "could not clean up previous");
		return NULL;
	}


	return ret;
}

boing_value_t *boing_program_get_global(boing_t *boing, boing_value_t *program, char *identifier)
{
	boing_value_t *ret = NULL, *key = NULL;


	if(!program)
	{
		boing_error(boing, 0, "could not find global due to a null program");
		return NULL;
	}

	if(program->length != 2 || !program->array[1] || program->array[1]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "could not find global due to an improperly formatted program");
		return NULL;
	}

	if(!(key = boing_value_from_str(boing, identifier)))
	{
		boing_error(boing, 0, "could not create value key from identifier string");
		return NULL;
	}

	if((ret = boing_value_table_get(boing, program->array[1], key)))
		boing_value_reference_inc(boing, ret);
	

	if(boing_value_reference_dec(boing, key))
	{
		boing_error(boing, 0, "could not refdec key");
		return NULL;
	}

	return ret;
}

boing_value_t *boing_eval_value(boing_t *boing, boing_value_t *program, boing_value_t *value, boing_value_t *stack, boing_value_t *previous)
{
	boing_value_t *ret = NULL, *args = NULL, *temp = NULL;
	boing_operation_data_t *op = NULL;
	size_t i;


	/* eval an ast */
	if(!value)
	{
		boing_error(boing, 0, "null value passed to eval");
		/* TODO throw error */
		return NULL;
	}

	if(!stack)
	{
		boing_error(boing, 0, "null stack passed to eval");
		/* TODO throw error */
		#if BOING_ENABLE_LINE_NUM
		boing_error_script_print(boing, value);
		#endif
		return NULL;
	}


	switch(value->type)
	{
		case BOING_TYPE_VALUE_NUMBER:
		case BOING_TYPE_VALUE_ARRAY:
		case BOING_TYPE_VALUE_EXTERNAL:
			ret = value;
			boing_value_reference_inc(boing, value);
		break;
		case BOING_TYPE_VALUE_OPERATION:
			if(!(op = boing_operation_data(boing, value->operation.op)))
			{
				/* TODO throw error */
				boing_error(boing, 0, "could get operation '%c' data", value->operation.op);
				#if BOING_ENABLE_LINE_NUM
				boing_error_script_print(boing, value);
				#endif
				return NULL;
			}


			/* determine whether or not operation wants the arguments evaluated and put into a new array or
			just passed the arg array from the ast */
			if(op->eval_args)
			{
				/* need to make new args based on the results of evaluating all of the arguments */
				if(!(args = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, value->operation.args_value->length)))
				{
					boing_error(boing, 0, "could not create args array for eval");
					/* TODO throw error */
					#if BOING_ENABLE_LINE_NUM
					boing_error_script_print(boing, value);
					#endif
					return NULL;
				}

				for(i = 0; i < value->operation.args_value->length; ++i)
				{
					if(!(temp = boing_eval_value(boing, program, value->operation.args_value->array[i], stack, previous)))
					{
						boing_error(boing, 0, "could not eval argument in operation");
						/* TODO throw error */
						#if BOING_ENABLE_LINE_NUM
						boing_error_script_print(boing, value->operation.args_value->array[i]);
						#endif
						return NULL;
					}


					/* add the argument to the args array */
					if(boing_value_array_append_insert(boing, args, i, temp))
					{
						boing_error(boing, 0, "could not add argument to args array");
						/* TODO throw error */
						#if BOING_ENABLE_LINE_NUM
						boing_error_script_print(boing, value);
						#endif
						return NULL;
					}
				}
			}
			else
			{
				/* just refinc the ast args array */
				boing_value_reference_inc(boing, value->operation.args_value);
				args = value->operation.args_value;
			}

			/* ======= ARGS handling */

			/* call the operation callback now */
			if(!(ret = op->callback(boing, program, stack, previous, args)))
			{
				boing_error(boing, 0, "error in '%c' operation call", op->op);
				/* TODO throw error */
				#if BOING_ENABLE_LINE_NUM
				boing_error_script_print(boing, value);
				#endif
				return NULL;
			}

			/* refdec the args array */
			if(boing_value_reference_dec(boing, args))
			{
				boing_error(boing, 0, "could not cleanly decrement argument array reference");
				/* TODO throw error */
				#if BOING_ENABLE_LINE_NUM
				boing_error_script_print(boing, value);
				#endif
				return NULL;
			}
		break;
	}

	return ret;
}

/* uses the boing program value */
boing_value_t *boing_eval(boing_t *boing, char *str, boing_value_t *args, char *script_name)
{
	size_t line = 1;
	boing_value_t *ret = NULL, *ast = NULL, *stack = NULL, *temp = NULL;
	boing_script_t *script_data = NULL;

	/* first, initialize the program value */

	if(!boing->program)
	{
		/* [ast root_identifier_table] */
		if(!(boing->program = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 2)))
		{
			boing_error(boing, 0, "could not request an array for default program");
			return NULL;
		}

		/* set defaults that are overwritten or modified later */

		if(!(temp = boing_value_from_double(boing, 0)))
		{
			boing_error(boing, 0, "could not initialize program temporary ast");
			return NULL;
		}

		if(boing_value_array_append_insert(boing, boing->program, 0, temp))
		{
			boing_error(boing, 0, "could not insert temporary ast into program");
			return NULL;
		}

		/* identifier table */

		if(!(temp = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 0)))
		{
			boing_error(boing, 0, "could not initialize program identifier table");
			return NULL;
		}

		if(boing_value_array_append_insert(boing, boing->program, 1, temp))
		{
			boing_error(boing, 0, "could not insert identifier table into program");
			return NULL;
		}
	}

	/* parse the string, make an ast, then toss it into the program value */

	/* might be a bad idea, but the data returned should at least be the size of uint8_t
	so if anything, the issue might be too much data used */
	if(!(script_data = (boing_script_t *)boing_str_request(boing, sizeof(boing_script_t))))
	{
		boing_error(boing, 0, "could not use the string pool to request a script struct");
		return NULL;
	}

	if(!(script_data->name = boing_str_ndup(boing, script_name, strlen(script_name))))
	{
		boing_error(boing, 0, "could not duplicate the script name");
		return NULL;
	}

	if(!(script_data->script = boing_str_ndup(boing, str, strlen(str))))
	{
		boing_error(boing, 0, "could not duplicate the script text");
		return NULL;
	}

	/* turn this into an external value so it gets garbage collected */
	if(!(temp = boing_value_request(boing, BOING_TYPE_VALUE_EXTERNAL, 0)))
	{
		boing_error(boing, 0, "could not request external value for script struct");
		return NULL;
	}

	temp->external.type = BOING_EXTERNAL_POINTER;
	temp->external.destroy_cb = &boing_script_data_cleanup;
	temp->external.ptr = script_data;


	if(!(ast = boing_parse_from_string(boing, NULL, NULL, temp, &str, &line)))
	{
		boing_error(boing, 0, "could not parse script for eval in default program");
		return NULL;
	}


	if(boing_value_reference_dec(boing, temp))
	{
		boing_error(boing, 0, "could not refdec script external value");
		return NULL;
	}

	/* need to contain ast in a '(' operation so it becomes executable */
	if(!(temp = boing_value_request(boing, BOING_TYPE_VALUE_OPERATION, 0)))
	{
		boing_error(boing, 0, "could not request '(' operation to put ast in");
		return NULL;
	}

	/* need to modify the operation and remove the old array and replace (note, not set) the new one */
	if(boing_value_reference_dec(boing, temp->operation.args_value))
	{
		boing_error(boing, 0, "could not remove old '(' operation args");
		return NULL;
	}

	temp->operation.op = '(';
	temp->operation.args_value = ast;
	ast = temp;

	/* would normally use the set function, but it doesnt really work too well when initializing structures */
	if(boing_value_reference_dec(boing, boing->program->array[0]))
	{
		boing_error(boing, 0, "could not remove old ast from program");
		return NULL;
	}

	boing->program->array[0] = ast;


	/* initialize the root identifiers */

	/* make sure args arent NULL. If they are, set it to 0 */
	if(!args)
	{
		if(!(temp = boing_value_from_double(boing, 0)))
		{
			boing_error(boing, 0, "could not initialize args0 to 0 after null passed");
			return NULL;
		}

		if(!(args = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 1)))
		{
			boing_error(boing, 0, "could not request an array for ARGS/_ value");
			return NULL;
		}

		if(boing_value_array_append_insert(boing, args, args->length, temp))
		{
			boing_error(boing, 0, "could not insert args value into array");
			return NULL;
		}
	}
	else
		boing_value_reference_inc(boing, args);


	/* add the args */

	/* add _ to globals */
	if(!(temp = boing_value_from_str(boing, "_")))
	{
		boing_error(boing, 0, "could not create string for args '_' key");
		return NULL;
	}

	if(boing_value_table_add_set(boing, boing->program->array[1], temp, args, 1))
	{
		boing_error(boing, 0, "could not add args '_' key and value to program identifier table");
		return NULL;
	}

	if(boing_value_reference_dec(boing, temp))
	{
		boing_error(boing, 0, "could not refdec '_' key");
		return NULL;
	}

	/* add ARGS to globals */
	if(!(temp = boing_value_from_str(boing, "ARGS")))
	{
		boing_error(boing, 0, "could not create string for args 'ARGS' key");
		return NULL;
	}

	if(boing_value_table_add_set(boing, boing->program->array[1], temp, args, 1))
	{
		boing_error(boing, 0, "could not add args 'ARGS' key and value to program identifier table");
		return NULL;
	}

	if(boing_value_reference_dec(boing, temp))
	{
		boing_error(boing, 0, "could not refdec 'ARGS' key");
		return NULL;
	}


	/* create a stack */
	if(!(stack = boing_value_stack_create(boing, boing->program)))
	{
		boing_error(boing, 0, "passed NULL stack and could not create a new one");
		/* TODO throw error */
		return NULL;
	}


	/* finally, execute it */

	/* create initial previous value as a number == 0*/
	if(!(temp = boing_value_from_double(boing, 0)))
	{
		boing_error(boing, 0, "could not initialize previous value");
		return NULL;
	}

	/* eval */
	if(!(ret = boing_eval_value(boing, boing->program, ast, stack, temp)))
	{
		boing_error(boing, 0, "eval error");
		return NULL;
	}


	/* cleanup args */
	if(boing_value_reference_dec(boing, args))
	{
		boing_error(boing, 0, "could not refdec args value");
		return NULL;
	}

	/* cleanup stack */
	if(boing_value_reference_dec(boing, stack))
	{
		boing_error(boing, 0, "could not refdec the stack");
		return NULL;
	}

	/* cleanup previous */
	if(boing_value_reference_dec(boing, temp))
	{
		boing_error(boing, 0, "could not refdec the previous value");
		return NULL;
	}

	return ret;
}

boing_value_t *boing_eval_file(boing_t *boing, char *path, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	char *file = NULL;


	if(!(file = boing_import_file(boing, NULL, path)))
	{
		boing_error(boing, 0, "could not read '%s' in boing_eval_file", path);
		return NULL;
	}


	if(!(ret = boing_eval(boing, file, args, path)))
	{
		boing_error(boing, 0, "eval error from '%s'", path);
		return NULL;
	}


	if(boing_str_release(boing, file))
	{
		boing_error(boing, 0, "problem releasing file read in file eval");
		return NULL;
	}

	return ret;
}


/* init and destroy functions. Includes default callbacks */

static uint8_t *boing_default_read_file_cb(boing_t *boing, char *path, size_t *size_read, size_t offset, size_t amount)
{
	uint8_t *ret = NULL;
	long total;
	FILE *f = NULL;
	/* amount == 0 means read whole file past the start */

	if(size_read)
		*size_read = 0;

	if(!(f = fopen(path, "rb")))
	{
		boing_error(boing, 0, "could not open file \"%s\"", path);
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	total = ftell(f);
	fseek(f, 0, SEEK_SET);

	/* determine true amount total to read and trim if necessary */

	if(offset > total)
	{
		boing_error(boing, 0, "offet further than file size for file \"%s\"", path);
		fclose(f);
		return NULL;
	}

	else if(amount && offset + amount <= total) /* the amount of file past offset */
		total = amount;
	else /* all of file past offset AND if the amount needs to be clamped */
		total = total - offset;
	
	/* TODO: determine if this is a bad idea. I dont want to call alloc functions
	anywhere outside the pool managers, but the string pool doesnt feel like the best
	place to request uint8_t<= types. Could potentially end up being 2x the size on
	some platforms. */

	total = sizeof(char) != 1 ? total / sizeof(char) : total; /* might be a bad idea still */

	if(!(ret = boing_str_request(boing, total)))
	{
		boing_error(boing, 0, "could not request a string to store file from pool for file \"%s\"", path);
		fclose(f);
		return NULL;
	}

	if(fread(ret, sizeof(uint8_t), total, f) != total)
	{
		boing_error(boing, 0, "could not read file \"%s\"", path);
		fclose(f);
		boing_str_release(boing, ret);
	}

	if(size_read)
		*size_read = total;
	

	if(fclose(f))
	{
		boing_error(boing, 0, "could not close file \"%s\"", path);
		return ret; /* if we got here, its not really that bad, but it might happen so just ignore */
	}

	return ret;
}

static size_t boing_default_write_file_cb(boing_t *boing, char *path, void *data, size_t write_size)
{
	FILE *f;
	size_t ret = 0;
	/* have to trust the amount to write */

	/* TODO (if catch op): throw an error and restore here if bad read and causes a segfault*/

	if(!(f = fopen(path, "wb")))
	{
		boing_error(boing, 0, "could not open file \"%s\" to write", path);
		return 0;
	}

	ret = fwrite(data, 1, write_size, f);

	if(fclose(f))
	{
		boing_error(boing, 0, "could not close file \"%s\"", path);
		return ret; /* if we got here, its not really that bad, but it might happen so just ignore */
	}

	return ret;
}

static void boing_default_print_cb(boing_t *boing, char *message)
{
	fprintf(stdout, "%s%s", message, BOING_FLAG_READ(boing->flags, BOING_FLAG_NO_PRINT_NEWLINE) ? "" : "\n");
}

static uint8_t *boing_default_read_cb(boing_t *boing, size_t *read_size, size_t read_limit, uint8_t read_until_char)
{
	uint8_t *ret = NULL;
	size_t buffer_size = 0, total_size = 0, offset = 0;
	int ch = 0;

	buffer_size = read_limit ? read_limit : 1;

	/* depending on if read_until_char, read until this char and then return the string */
	/* same goes for read limit. Read until char or stream EOF */

	/* NOTE: it might not be a good idea to use the string pool again for this */
	
	/* have a size 1 buffer */
	if(!(ret = (uint8_t *)boing_str_request(boing, buffer_size)))
	{
		boing_error(boing, 0, "could not request buffer for input read");
		return NULL;
	}

	if(!read_limit || read_until_char)
	{
		--buffer_size;

		do
		{
			/* fread(ret + offset, 1, 1, stdin); */
			if((ch = fgetc(stdin)) == EOF)
				break;
			
			ret[offset] = (char)ch;

			if(!(ret = boing_str_resize(boing, ret, ++buffer_size + 1)))
			{
				boing_error(boing, 0, "could not realloc while reading input");
				return NULL;
			}

			++offset;
		} while(ret[offset - 1] != read_until_char);

		total_size = buffer_size - 1;
	}
	else
		total_size = fread(ret + offset, 1, read_limit, stdin);
	

	*read_size = total_size;

	return ret;
}

/* the reason this exists is if the path resolution needs to be altered or files need to be protected from importing */
static uint8_t *boing_default_import_cb(boing_t *boing, boing_value_t *stack, char *path)
{
	uint8_t *ret = NULL, *temp = NULL;
	size_t size = 0;

	/* allows importing from anywhere */

	if(!(ret = boing_read_file(boing, path, &size, 0, 0)))
	{
		boing_error(boing, 0, "could not read file \"%s\" for import", path);
		return NULL;
	}

	/* resize and set null terminator */

	if(!(temp = boing_str_resize(boing, ret, size + 1)))
	{
		boing_error(boing, 0, "could not finalize file \"%s\" for import", path);
		boing_str_release(boing, ret);
		return NULL;
	}

	ret = temp;
	ret[size] = 0;


	return ret;
}

static void boing_default_error_cb(boing_t *boing, char *message, int fatal)
{
	/* TODO: if adding catch operator, need to use setjmp and longjmp */
	fprintf(stderr, "%s\n", message);
}

static int boing_root_stack_init_cb(boing_t *boing, boing_value_t *stack)
{
	/* this callback does nothing by default */

	return 0;
}

static int boing_module_cleanup_cb(boing_t *boing, boing_module_t *module)
{
	/* this callback does nothing by default */

	return 0;
}

int boing_init(boing_t *boing)
{
	size_t i;

	/* before anything happens, set the default callbacks if they are NULL */
	if(!boing->callback.boing_read_file_cb)
		boing->callback.boing_read_file_cb = &boing_default_read_file_cb;
	if(!boing->callback.boing_write_file_cb)
		boing->callback.boing_write_file_cb = &boing_default_write_file_cb;
	if(!boing->callback.boing_print_cb)
		boing->callback.boing_print_cb = &boing_default_print_cb;
	if(!boing->callback.boing_read_cb)
		boing->callback.boing_read_cb = &boing_default_read_cb;
	if(!boing->callback.boing_import_cb)
		boing->callback.boing_import_cb = &boing_default_import_cb;
	if(!boing->callback.boing_error_cb)
		boing->callback.boing_error_cb = &boing_default_error_cb;
	if(!boing->callback.boing_root_stack_init_cb)
		boing->callback.boing_root_stack_init_cb = &boing_root_stack_init_cb;
	if(!boing->callback.boing_module_cleanup_cb)
		boing->callback.boing_module_cleanup_cb = &boing_module_cleanup_cb;

	/* init defaults. These shouldn't be changed, only extended */
	boing_operation_data_t defaults[] = {
		{'[', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_PASS_ARGS, &boing_operation_array},
		{'{', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_PASS_ARGS, &boing_operation_block_pass},
		{'(', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_PASS_ARGS, &boing_operation_block_runtime},
		{'p', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_print},
		{'s', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_string},
		{'r', 0, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_read},
		{'+', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_plus},
		{'-', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_minus},
		{'*', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_multiply},
		{'/', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_divide},
		{'%', 2, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_modulo},
		{'^', 2, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_power},
		{'&', 2, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_logical_and},
		{'|', 2, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_logical_or},
		{'!', 1, BOING_OPERATION_IMPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_logical_not},
		{'c', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_copy},
		{'n', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_inc},
		{'d', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_dec},
		{'i', 2, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_index},
		{'t', 2, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_table},
		{'z', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_sizeof},
		{'y', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_type},
		{'=', 2, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_equality},
		{'<', 2, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_lessthan},
		{'>', 2, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_greaterthan},
		{'f', 2, BOING_OPERATION_IMPLICIT, BOING_OPERATION_PASS_ARGS, &boing_operation_if},
		{'l', 2, BOING_OPERATION_IMPLICIT, BOING_OPERATION_PASS_ARGS, &boing_operation_loop},
		{'w', 2, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_write},
		{'e', 2, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_eval},
		{'A', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_identifier},
		{'x', 2, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_external},
		{'h', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_hash},
		{'k', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_stack},
		{'o', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_file},
		{'m', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_import},
		{'a', 2, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_search},
		{'g', 0, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_random},
		{'q', 1, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_sort},
		{'u', 2, BOING_OPERATION_EXPLICIT, BOING_OPERATION_EVAL_ARGS, &boing_operation_array_setup}
	};

	if(!(boing->operations = malloc(sizeof(defaults))))
	{
		boing_error(boing, 0, "could not allocate operation array");
		return 1;
	}
	memset(boing->operations, 0, sizeof(defaults));
	memcpy(boing->operations, defaults, sizeof(defaults));

	boing->operation_amount = sizeof(defaults) / sizeof(defaults[0]);

	/* initialize pools in specific order */

	/* strings */
	boing->pool.string.get_size = &boing_str_size_cb;
	boing->pool.string.container_cleanup = &boing_str_destroy_cb;
	boing->pool.string.max_free = BOING_DEFAULT_FREE_STRING_MAX;
	boing->pool.string.block_alloc = BOING_DEFAULT_SIZE_STRING;

	for(i = 0; i < BOING_DEFAULT_FREE_STRING; ++i)
	{
		if(boing_str_create_free(boing, boing->pool.string.block_alloc))
		{
			boing_error(boing, 0, "could not initialize string pool");
			return 1;
		}
	}

	/* array internal buffers */
	boing->pool.array_internal.get_size = &boing_array_internl_size_cb;
	boing->pool.array_internal.container_cleanup = &boing_array_internl_destroy_cb;
	boing->pool.array_internal.max_free = BOING_DEFAULT_FREE_VALUE_ARRAY_INTERNAL_MAX;
	boing->pool.array_internal.block_alloc = BOING_DEFAULT_SIZE_VALUE_ARRAY_INTERNAL;

	for(i = 0; i < BOING_DEFAULT_FREE_VALUE_ARRAY_INTERNAL; ++i)
	{
		if(boing_array_internl_create_free(boing, boing->pool.array_internal.block_alloc))
		{
			boing_error(boing, 0, "could not initialize internal array buffer pool");
			return 1;
		}
	}

	/* initialize value base struct pool */
	boing->pool.value.get_size = &boing_value_pool_size_cb;
	boing->pool.value.container_cleanup = &boing_value_pool_destroy_cb;
	boing->pool.value.max_free = BOING_DEFAULT_FREE_VALUE_BASE_MAX;
	boing->pool.value.block_alloc = 0;

	for(i = 0; i < BOING_DEFAULT_FREE_VALUE_BASE; ++i)
	{
		if(boing_value_pool_create_free(boing, boing->pool.value.block_alloc))
		{
			boing_error(boing, 0, "could not initialize value base struct pool");
			return 1;
		}
	}
	

	/* initialize modules table */
	if(!(boing->modules = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 1)))
	{
		boing_error(boing, 0, "could not initialize native module table");
		return 1;
	}



	BOING_FLAG_SET(boing->flags, BOING_FLAG_INITIALIZED);

	return 0;
}

int boing_destroy(boing_t *boing, int error_print_limit)
{
	char *temp_str = NULL;
	size_t i, j = 0;
	boing_pool_container_t *node = NULL;
	boing_value_t *value = NULL;


	/* destroy all values and clean up*/

	free(boing->operations);

	/* clean up pools in specific order to avoid double frees. Destroy
	is reverse order initialized */

	/* release all known values */
	if(boing->program)
	{
		if(boing_value_reference_dec(boing, boing->program))
		{
			boing_error(boing, 0, "could not clean up existing program");
			/* do not exit, just clean up anyway */
		}
	}

	/* call all destructor functions for native modules then destroy the table */
	for(i = 0; i < boing->modules->length; ++i)
	{
		if(boing->modules->array[i]->type != BOING_TYPE_VALUE_ARRAY || boing->modules->array[i]->length != 2)
		{
			boing_error(boing, 0, "module table corrupted");
			return 1;
		}

		if(boing->modules->array[i]->array[1])
		{
			if(((boing_module_t *)boing->modules->array[i]->array[1]->external.ptr)->module_destroy(boing, ((boing_module_t *)boing->modules->array[i]->array[1]->external.ptr)))
			{
				if(!(temp_str = boing_str_from_value_array(boing, boing->modules->array[i]->array[0])))
				{
					boing_error(boing, 0, "could not turn module table key into string");
					return 1;
				}

				boing_error(boing, 0, "could not clean up module '%s'", temp_str);

				if(boing_str_release(boing, temp_str))
				{
					boing_error(boing, 0, "could not release module key string");
					return 1;
				}

				return 1;
			}

			if(boing_module_cleanup_handle(boing, ((boing_module_t *)boing->modules->array[i]->array[1]->external.ptr)))
			{
				boing_error(boing, 0, "could not cleanup module handle");
				return 1;
			}

			/* release the module ptr */
			if(boing_str_release(boing, boing->modules->array[i]->array[1]->external.ptr))
			{
				boing_error(boing, 0, "could not release modlue structure");
				return 1;
			}
		}
	}
	
	if(boing_value_reference_dec(boing, boing->modules))
	{
		boing_error(boing, 0, "could not clean up native module table");
		/* do not exit, just clean up anyway */
	}

	/* NOTE: do NOT put anything after this. It will look like a leak even if it isnt */
	
	/* print pool free amounts to indicate possible leaks if they appear to be leaking */

	if(error_print_limit)
	{
		if(boing->pool.string.amount - boing->pool.string.free)
			boing_error(boing, 0, "string pool in use at exit (possible leak): %lu", boing->pool.string.amount - boing->pool.string.free);
		if(boing->pool.array_internal.amount - boing->pool.array_internal.free)
			boing_error(boing, 0, "array buffer pool in use at exit (possible leak): %lu", boing->pool.array_internal.amount - boing->pool.array_internal.free);
		if(boing->pool.value.amount - boing->pool.value.free)
		{
			boing_error(boing, 0, "value pool in use at exit (possible leak): %lu", boing->pool.value.amount - boing->pool.value.free);
			
			#if BOING_ENABLE_LINE_NUM
			/* print where in the script things were leaked */
			boing_error(boing, 0, "origins of values left over (if originated as literals):");

			node = boing->pool.value.end;
			for(i = 0; i < boing->pool.value.amount - boing->pool.value.free; ++i)
			{
				if(j >= error_print_limit)
				{
					boing_error(boing, 0, "... (%lu more)", (boing->pool.value.amount - boing->pool.value.free) - i);
					break;
				}
				
				if((value = ((boing_value_t *)node->data)))
				{
					boing_error_script_print(boing, value);
					if(((boing_value_t *)node->data)->line)
						++j;
				}

				if(!(node = node->prev))
				{
					boing_error(boing, 0, "critical pool corruption error");
					break;
				}
			}
			#endif
		}
	}

	/* free the value base struct pool */
	boing_pool_destroy(boing, &boing->pool.value);

	/* free the internal array buffer pool */
	boing_pool_destroy(boing, &boing->pool.array_internal);

	/* free string pool last */
	boing_pool_destroy(boing, &boing->pool.string);
	

	return 0;
}


#endif /* BOING_IMPLEMENTATION */


#endif