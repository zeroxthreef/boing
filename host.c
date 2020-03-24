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


#ifndef HOST_MODULE_LOCATION
	#define HOST_MODULE_LOCATION ""
#endif

#define ERROR_PRINT_LIMIT_DEFAULT 10

#define HOST_FREE_STRING 20
#define HOST_FREE_ARRAY 50
#define HOST_FREE_VALUE 80

/* initialize the default free amount to 0 in the host because it can change with arguments */
#define BOING_DEFAULT_FREE_STRING 0
#define BOING_DEFAULT_FREE_VALUE_ARRAY_INTERNAL 0
#define BOING_DEFAULT_FREE_VALUE_BASE 0
#define BOING_HASH_64
#define BOING_IMPLEMENTATION
#include "boing.h"

/* module headers */
#ifdef MODULE_OS_ADDED
	#include "modules/os/os.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#include <Windows.h>
#elif __APPLE__
	#include <dlfcn.h>
#elif unix
	#include <dlfcn.h>
#endif



/* the host file has an argument parser. Everything near here is for this */

enum
{
	ARG_TYPE_VOID,
	ARG_TYPE_NUMBER,
	ARG_TYPE_STRING
};

typedef struct argument_t argument_t;

struct argument_t
{
	char *arg, *about;
	uint8_t type;
	int (*callback)(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);
};


/* argument callback prototypes */
int argument_help(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);
int argument_version(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);
int argument_print_no_newline(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);
int argument_inspect_program(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);int argument_inspect_return(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);
int argument_pool_str_size(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);
int argument_pool_str_max_free(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);
int argument_pool_str_init_free(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);
int argument_pool_array_size(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);
int argument_pool_array_max_free(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);
int argument_pool_array_init_free(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);
int argument_pool_value_max_free(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);
int argument_pool_value_init_free(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);
int argument_cleanup_error_limit(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str);


/* simple argument parser */
int host_parse_args(boing_t *boing, int argc, char **argv, argument_t *arguments, int argument_amount)
{
	int i, j, num = 0, ret = 0;
	char *str = NULL;


	/* start at 1 to skip the binary arg */
	for(i = 1; i < argc; ++i)
	{
		for(j = 0; j < argument_amount; ++j)
		{
			if(strncmp(arguments[j].arg, argv[i], strlen(arguments[j].arg)) == 0)
			{
				if(arguments[j].type != ARG_TYPE_VOID)
				{
					/* use a str regardless */
					if(!*(str = &argv[i][strlen(arguments[j].arg) + 1])) /* advance over = */
					{
						boing_error(boing, 0, "bad argument format on arg %s", argv[i]);
						return -1;
					}

					if(arguments[j].type == ARG_TYPE_NUMBER)
						num = strtol(str, NULL, 10);
				}
				
				/* call the argument function */
				if((ret = arguments[j].callback(boing, arguments, argument_amount, num, str)))
					return ret;


				break;
			}

			/* detect if the argument is not found and that means its the script path */
			if(j + 1 >= argument_amount)
				return i;
		}

		num = 0;
		str = NULL;
	}


	return i;
}

int host_finalize_pool(boing_t *boing)
{
	/* not a very smart way of checking if pools have been fully initialized past the create free part */

	/* NOTE: these arent synchronized with the header version because the host imports modules sometimes */
	
	if(argument_pool_str_init_free(boing, NULL, 0, HOST_FREE_STRING, NULL) != 0)
		return -1;
	
	if(argument_pool_array_init_free(boing, NULL, 0, HOST_FREE_ARRAY, NULL) != 0)
		return -1;

	if(argument_pool_value_init_free(boing, NULL, 0, HOST_FREE_VALUE, NULL) != 0)
		return -1;

	return 0;
}

unsigned int host_count_strings(char *src, char *search)
{
	char *offset = src;
	unsigned int i, count = 0;


	for(i = 0; i < strlen(src); ++i)
	{
		if(!(offset = strstr(offset + 1, search)))
			return count;
		
		++count;
	}

	return 0;
}

boing_value_t *host_repl(boing_t *boing)
{
	char *input = NULL, *temp = NULL;
	size_t size = 0;
	uint8_t previous_type = -1;
	boing_value_t *ret = NULL;


	while(1)
	{
		if(!(input = boing_str_request(boing, 1)))
		{
			boing_error(boing, 0, "could not request input string in REPL");
			return NULL;
		}

		input[0] = 0x0;

		printf("> ");

		while(1)
		{
			if(!(temp = boing_read_input(boing, &size, 0, '\n')))
			{
				boing_error(boing, 0, "could not read stdin in REPL");
				return NULL;
			}

			temp[size] = 0x0;

			if(strcmp(temp, "\\quit") == 0)
			{
				boing_str_release(boing, input);
				boing_str_release(boing, temp);
				
				return ret;
			}

			/* add temp to in progress script */
			if(boing_str_ncat(boing, &input, temp, size))
			{
				boing_error(boing, 0, "could not concat input to in progress script");
				return NULL;
			}


			/* check if [], (), and {} balance equally because then it'll know to parse after newlines */
			
			if((host_count_strings(input, "(") == host_count_strings(input, ")")) && (host_count_strings(input, "[") == host_count_strings(input, "]")) && (host_count_strings(input, "{") == host_count_strings(input, "}")))
			{
				boing_str_release(boing, temp);
				break;
			}

			printf(">> ");

			boing_str_release(boing, temp);
		}

		/* eval the input and print it*/
		if(ret && boing_value_reference_dec(boing, ret))
		{
			boing_error(boing, 0, "could not refdec return value in REPL");
			return NULL;
		}

		if(!(ret = boing_eval(boing, input, NULL, "stdin")))
			printf("script error\n");
		else
		{
			if(!(temp = boing_str_from_value_readable(boing, ret, &previous_type, BOING_VALUE_STRING_READABLE, 0)))
			{
				boing_error(boing, 0, "could not stringify value evlauated in REPL");
				return NULL;
			}

			printf("%s\n", temp);

			boing_str_release(boing, temp);
		}
		
		boing_str_release(boing, input);
	}

	return ret;
}

int host_root_stack_init(boing_t *boing, boing_value_t *stack)
{
	/* add default modules to each new stack */

	#define ADD_STACK_MODULE(name) do	\
	{	\
		boing_value_t *module_value = NULL, *key = NULL;	\
		if(!(key = boing_value_from_str(boing, #name)))	\
		{	\
			boing_error(boing, 0, "could not create lookup string key for module " #name);	\
			return 1;	\
		}	\
			\
		if(!(module_value = boing_value_table_get(boing, boing->modules, key)))	\
		{	\
			boing_error(boing, 0, "could not find os module");	\
			boing_value_reference_dec(boing, key);	\
			return 1;	\
		}	\
			\
		if(boing_value_reference_dec(boing, key))	\
		{	\
			boing_error(boing, 0, "could not refdec lookup string key for module " #name);	\
			return 1;	\
		}	\
			\
		if(module_##name##_stack_add(boing, stack, ((boing_module_t *)module_value->external.ptr)))	\
		{		\
			boing_error(boing, 0, "could not perform stack add in module " #name);	\
			return 1;	\
		}	\
	} while(0)
	
	
	#ifdef MODULE_OS_ADDED
		/* add os module */
		ADD_STACK_MODULE(os);


	#endif

	return 0;
}

int host_init_modules(boing_t *boing)
{
	/* add modules */

	#ifdef MODULE_OS_ADDED
		/* add os module */
		if(boing_module_add(boing, "os", &module_os_init, &module_os_destroy, &module_os_stack_add, NULL))
		{
			boing_error(boing, 0, "could not initialize os module");
			return 1;
		}
	#endif


	return 0;
}

int host_module_cleanup(boing_t *boing, boing_module_t *module)
{
	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
		if(module->handle)
		{
			if(!FreeLibrary((HMODULE)module->handle))
			{
				boing_error(boing, 0, "could not close module handle");
				return 1;
			}
		}
	#elif __APPLE__
		if(module->handle)
		{
			if(dlclose(module->handle))
			{
				boing_error(boing, 0, "could not close module handle");
				return 1;
			}
		}
	#elif unix
		if(module->handle)
		{
			if(dlclose(module->handle))
			{
				boing_error(boing, 0, "could not close module handle");
				return 1;
			}
		}
	#endif

	return 0;
}

static uint8_t *host_import(boing_t *boing, boing_value_t *stack, char *path)
{
	void *handle = NULL, *module_init = NULL, *module_destroy = NULL, *module_stack_add = NULL;
	uint8_t *ret = NULL, *temp = NULL;
	char *ext = NULL, *install_path = NULL;
	size_t size = 0;

	/* allows importing from anywhere */

	/* determine if importing a native module or just a script */

	/* also looks in the install path after lookin in the interpreter working directory */

	if(!(ext = strrchr(path, '.')))
		ext = "";
	
	if(strlen(HOST_MODULE_LOCATION) && !(install_path = boing_str_sprintf(boing, "%s/%s", HOST_MODULE_LOCATION, path)))
	{
		boing_error(boing, 0, "could not create install directory path string");
		return NULL;
	}

	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
		if(!strcmp(ext, ".dll"))
		{
			/* first search the current directory */
			if(!(handle = (void *)LoadLibraryA(path)))
			{
				if(!(handle = (void *)LoadLibraryA(install_path)))
				{
					boing_error(boing, 0, "could not dlopen \"%s\"", path);
					return NULL;
				}
			}

			if(!(module_init = GetProcAddress((HMODULE)handle, "module_init")))
			{
				FreeLibrary((HMODULE)handle);
				boing_error(boing, 0, "could not find module_init symbol for \"%s\"", path);
				return NULL;
			}

			if(!(module_destroy = GetProcAddress((HMODULE)handle, "module_destroy")))
			{
				FreeLibrary((HMODULE)handle);
				boing_error(boing, 0, "could not find module_destroy symbol for \"%s\"", path);
				return NULL;
			}

			if(!(module_stack_add = GetProcAddress((HMODULE)handle, "module_stack_add")))
			{
				FreeLibrary((HMODULE)handle);
				boing_error(boing, 0, "could not find module_stack_add symbol for \"%s\"", path);
				return NULL;
			}

			if(boing_module_add(boing, path, module_init, module_destroy, module_stack_add, handle))
			{
				FreeLibrary((HMODULE)handle);
				boing_error(boing, 0, "could not add module \"%s\"", path);
				return NULL;
			}

			if(boing_module_stack_add(boing, stack, path))
			{
				boing_error(boing, 0, "could not add stack values for \"%s\"", path);
				return NULL;
			}

			/* NOTE: the interpreter always needs _something_ to run so just pass an empty string if its a native module */

			if(!(ret = boing_str_ndup(boing, "", 0)))
			{
				boing_error(boing, 0, "could not add run string after module \"%s\"", path);
				return NULL;
			}
		}
	#elif __APPLE__ /* TODO: test this, macos and unix would be the same here but im not sure if thats the case. Dont have the ability to test */
		/* first search the current directory */
		if(!strcmp(ext, ".dylib"))
		{
			if(!(handle = dlopen(path, RTLD_NOW)))
			{
				if(!(handle = dlopen(install_path, RTLD_NOW)))
				{
					boing_error(boing, 0, "could not dlopen \"%s\"", path);
					return NULL;
				}
			}

			if(!(module_init = dlsym(handle, "module_init")))
			{
				dlclose(handle);
				boing_error(boing, 0, "could not find module_init symbol for \"%s\"", path);
				return NULL;
			}

			if(!(module_destroy = dlsym(handle, "module_destroy")))
			{
				dlclose(handle);
				boing_error(boing, 0, "could not find module_destroy symbol for \"%s\"", path);
				return NULL;
			}

			if(!(module_stack_add = dlsym(handle, "module_stack_add")))
			{
				dlclose(handle);
				boing_error(boing, 0, "could not find module_stack_add symbol for \"%s\"", path);
				return NULL;
			}

			if(boing_module_add(boing, path, module_init, module_destroy, module_stack_add, handle))
			{
				dlclose(handle);
				boing_error(boing, 0, "could not add module \"%s\"", path);
				return NULL;
			}

			if(boing_module_stack_add(boing, stack, path))
			{
				boing_error(boing, 0, "could not add stack values for \"%s\"", path);
				return NULL;
			}

			/* NOTE: the interpreter always needs _something_ to run so just pass an empty string if its a native module */

			if(!(ret = boing_str_ndup(boing, "", 0)))
			{
				boing_error(boing, 0, "could not add run string after module \"%s\"", path);
				return NULL;
			}
		}
	#elif unix
		/* first search the current directory */
		if(!strcmp(ext, ".so"))
		{
			if(!(handle = dlopen(path, RTLD_NOW)))
			{
				if(!(handle = dlopen(install_path, RTLD_NOW)))
				{
					boing_error(boing, 0, "could not dlopen \"%s\"", path);
					return NULL;
				}
			}

			if(!(module_init = dlsym(handle, "module_init")))
			{
				dlclose(handle);
				boing_error(boing, 0, "could not find module_init symbol for \"%s\"", path);
				return NULL;
			}

			if(!(module_destroy = dlsym(handle, "module_destroy")))
			{
				dlclose(handle);
				boing_error(boing, 0, "could not find module_destroy symbol for \"%s\"", path);
				return NULL;
			}

			if(!(module_stack_add = dlsym(handle, "module_stack_add")))
			{
				dlclose(handle);
				boing_error(boing, 0, "could not find module_stack_add symbol for \"%s\"", path);
				return NULL;
			}

			if(boing_module_add(boing, path, module_init, module_destroy, module_stack_add, handle))
			{
				dlclose(handle);
				boing_error(boing, 0, "could not add module \"%s\"", path);
				return NULL;
			}

			if(boing_module_stack_add(boing, stack, path))
			{
				boing_error(boing, 0, "could not add stack values for \"%s\"", path);
				return NULL;
			}

			/* NOTE: the interpreter always needs _something_ to run so just pass an empty string if its a native module */

			if(!(ret = boing_str_ndup(boing, "", 0)))
			{
				boing_error(boing, 0, "could not add run string after module \"%s\"", path);
				return NULL;
			}
		}
	#endif
	else
	{
		if(!(ret = boing_read_file(boing, path, &size, 0, 0)))
		{
			if(!(ret = boing_read_file(boing, install_path, &size, 0, 0)))
			{
				boing_error(boing, 0, "could not read file \"%s\" for import", path);
				return NULL;
			}
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
	}


	if(install_path)
	{
		if(boing_str_release(boing, install_path))
		{
			boing_error(boing, 0, "could not release install path string");
			boing_str_release(boing, ret);
			return NULL;
		}
	}


	return ret;
}

int main(int argc, char **argv)
{
	int retint = 1, arg_offset = 0;
	boing_t b;
	boing_value_t *ret = NULL, *args = NULL;

	argument_t arguments[] = {
		{"--help", "prints this mesage", ARG_TYPE_VOID, &argument_help},
		{"--version", "prints the version as BOING_VER_MAJOR.BOING_VER_MINOR.BOING_VER_REVISION", ARG_TYPE_VOID, &argument_version},
		{"--print-no-newline", "tells the print operation to not print newlines to stdout automatically", ARG_TYPE_VOID, &argument_print_no_newline},
		
		{"--debug-inspect-program", "prints the debug view of the entire program", ARG_TYPE_VOID, &argument_inspect_program},
		{"--debug-inspect-return", "prints the debug view of the returned value from eval", ARG_TYPE_VOID, &argument_inspect_return},
		

		{"--pool-string-default-size", "sets the default alloc size of strings", ARG_TYPE_NUMBER, &argument_pool_str_size},
		{"--pool-string-max-free", "sets the max free strings in the string pool", ARG_TYPE_NUMBER, &argument_pool_str_max_free},
		{"--pool-string-init-free", "sets the amount total of free strings in the string pool", ARG_TYPE_NUMBER, &argument_pool_str_init_free},
		
		{"--pool-array-default-size", "sets the default alloc size of array buffers", ARG_TYPE_NUMBER, &argument_pool_array_size},
		{"--pool-array-max-free", "sets the max free array buffers in the array buffer pool", ARG_TYPE_NUMBER, &argument_pool_array_max_free},
		{"--pool-array-init-free", "sets the amount total of free array buffers in the array buffer pool", ARG_TYPE_NUMBER, &argument_pool_array_init_free},
		
		{"--pool-value-max-free", "sets the max free values in the value pool", ARG_TYPE_NUMBER, &argument_pool_value_max_free},
		{"--pool-value-init-free", "sets the amount total of free values in the value pool", ARG_TYPE_NUMBER, &argument_pool_value_init_free},

		{"--cleanup-error-limit", "sets the maximum amount of errors to print at cleanup. 0 prints nothing", ARG_TYPE_NUMBER, &argument_cleanup_error_limit}
	};


	/* initialize the boing struct */
	memset(&b, 0, sizeof(boing_t));

	b.callback.boing_root_stack_init_cb = &host_root_stack_init;
	b.callback.boing_module_cleanup_cb = &host_module_cleanup;
	b.callback.boing_import_cb = &host_import;

	if(boing_init(&b))
	{
		boing_error(&b, 0, "unable to intialize boing");

		if(boing_destroy(&b, argument_cleanup_error_limit(NULL, NULL, 0, 0, NULL)))
		{
			boing_error(&b, 0, "could not clean up boing after an init error");
			return 1;
		}

		return 1;
	}


	/* parse the arguments */


	/* parse interpreter arguments */
	else if((arg_offset = host_parse_args(&b, argc, argv, arguments, sizeof(arguments) / sizeof(arguments[0]))) < 0)
	{
		if(arg_offset == -1)
			boing_error(&b, 0, "please enter a valid argument format");
	}

	else if(host_finalize_pool(&b))
		boing_error(&b, 0, "could not finalize one or more pools");

	/* handle args and eval the file wanted */
	else if(!(args = boing_value_from_args(&b, argc - arg_offset, argv + arg_offset)))
		boing_error(&b, 0, "could not parse arguments into the arguments passed to eval");
	
	/* initialize default modules */
	else if(host_init_modules(&b))
		boing_error(&b, 0, "could not initialize default modules");

	/* send to REPL mode */
	else if(argc == 1)
	{
		printf("%s REPL. Type '\\quit' to exit REPL\n", BOING_VERSION_STRING);

		if(!(ret = host_repl(&b)))
			boing_error(&b, 0, "error in runtime");
		
		if(boing_value_reference_dec(&b, args))
			boing_error(&b, 0, "could not refdec arguments");
	}

	/* everything parsed correctly and eval can proceed */
	else
	{
		if(!(ret = boing_eval_file(&b, argv[arg_offset], args)))
			boing_error(&b, 0, "error in runtime");
		
		if(boing_value_reference_dec(&b, args))
			boing_error(&b, 0, "could not refdec arguments");
	}


	/* handle debug prints of return value and program */

	if(argument_inspect_program(NULL, NULL, 0, 0, NULL) != 0)
	{
		printf("program debug:\n");
		/* TODO make a base stack print as well */
		boing_value_debug_print(b.program->array[0], 0, stdout);
	}

	if(argument_inspect_return(NULL, NULL, 0, 0, NULL) != 0)
	{
		printf("return value debug:\n");
		boing_value_debug_print(ret, 0, stdout);
	}



	/* return what it wants. Note, numeric types will be returned like normal,
	but everything else just turns to 0 */

	if(ret)
	{
		retint = (ret->type == BOING_TYPE_VALUE_NUMBER) ? (int)ret->number : 0;

		if(boing_value_reference_dec(&b, ret))
		{
			boing_error(&b, 0, "could not refdec return value from file eval");
			retint = 1;
		}
	}

	
	/* cleanup */
	if(boing_destroy(&b, argument_cleanup_error_limit(NULL, NULL, 0, 0, NULL)))
	{
		boing_error(&b, 0, "could not clean up after boing");
		return 1;
	}


	return retint;
}

/* argument functions */

/* print help */
int argument_help(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str)
{
	int i;


	printf("%s help:\n\n\nformat\n --argument-name=str_or_integer\n example: --pool-string-max-free=400\n\n%-25s %s\n", BOING_VERSION_STRING, "command", "about/usage");

	for(i = 0; i < argument_amount; ++i)
		printf(" %-25s  %s\n", arguments[i].arg, arguments[i].about);
	
	printf("\n");

	return -2;
}

int argument_version(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str)
{
	printf("%s\n", BOING_VERSION_STRING);
	return -2;
}

int argument_inspect_program(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str)
{
	static int ret = 1;
	
	ret--;
	return ret;
}

int argument_inspect_return(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str)
{
	static int ret = 1;
	
	ret--;
	return ret;
}

int argument_print_no_newline(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str)
{
	BOING_FLAG_SET(boing->flags, BOING_FLAG_NO_PRINT_NEWLINE);
	
	return 0;
}

int argument_pool_str_size(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str)
{
	boing->pool.string.block_alloc = number;
	return 0;
}

int argument_pool_str_max_free(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str)
{
	boing->pool.string.max_free = number;
	return 0;
}

int argument_pool_str_init_free(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str)
{
	size_t i;
	static int ret = 1;

	if(!ret)
		return 0;

	if(number < 2)
	{
		boing_error(boing, 0, "unable to have fewer than 2 initial free strings in string pool");
		return -1;
	}
	
	for(i = 0; i < number; ++i)
	{
		if(boing_str_create_free(boing, boing->pool.string.block_alloc))
		{
			boing_error(boing, 0, "could not initialize string pool");
			return 1;
		}
	}


	ret--;

	return ret; /* terrible hack to keep track of when the a pool has been set in the args */
}

int argument_pool_array_size(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str)
{
	boing->pool.array_internal.block_alloc = number;
	return 0;
}

int argument_pool_array_max_free(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str)
{
	boing->pool.array_internal.max_free = number;
	return 0;
}

int argument_pool_array_init_free(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str)
{
	size_t i;
	static int ret = 1;

	if(!ret)
		return 0;

	if(number < 2)
	{
		boing_error(boing, 0, "unable to have fewer than 2 initial free array buffers in array buffer pool");
		return -1;
	}

	for(i = 0; i < number; ++i)
	{
		if(boing_array_internl_create_free(boing, boing->pool.array_internal.block_alloc))
		{
			boing_error(boing, 0, "could not initialize internal array buffer pool");
			return 1;
		}
	}


	ret--;

	return ret; /* terrible hack to keep track of when the a pool has been set in the args */
}

int argument_pool_value_max_free(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str)
{
	boing->pool.value.max_free = number;
	return 0;
}

int argument_pool_value_init_free(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str)
{
	size_t i;
	static int ret = 1;

	if(!ret)
		return 0;

	if(number < 2)
	{
		boing_error(boing, 0, "unable to have fewer than 2 initial free values in value pool");
		return -1;
	}

	for(i = 0; i < number; ++i)
	{
		if(boing_value_pool_create_free(boing, boing->pool.value.block_alloc))
		{
			boing_error(boing, 0, "could not initialize value base struct pool");
			return 1;
		}
	}


	ret--;

	return ret; /* terrible hack to keep track of when the a pool has been set in the args */
}

int argument_cleanup_error_limit(boing_t *boing, argument_t *arguments, int argument_amount, int number, char *str)
{
	static int ret = ERROR_PRINT_LIMIT_DEFAULT;
	

	if(arguments)
	{
		ret = number;
		return 0;
	}
	
	return ret;
}