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


#include "os.h"

#ifdef MODULE_STANDALONE
#include <boing.h>
#else
#include "../../boing.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#define __WINDOWS__
	#include <Winsock2.h>
	#include <Windows.h>
	#include <ws2tcpip.h>
#else
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <netinet/in.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <dirent.h>
#endif

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif



/* simple str as buffer container cleanup */
static int module_os_managed_cleanup(boing_t *boing, boing_value_t *value)
{
	if(boing_str_release(boing, ((boing_managed_ptr_t *)value->external.ptr)->ptr))
	{
		boing_error(boing, 0, "could not release managed ptr buffer");
		return 1;
	}

	return 0;
}

static boing_value_t *module_os_function_time(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(!(ret = boing_value_from_double(boing, (double)time(NULL))))
	{
		boing_error(boing, 0, "could not turn time into value");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_getenv(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	char *temp_str = NULL, *env_result = NULL;
	boing_value_t *ret = NULL;


	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "OS_GETENV expects a single string argument");
		/* TODO throw error */
		return NULL;
	}

	if(!(temp_str = boing_str_from_value_array(boing, args->array[0])))
	{
		boing_error(boing, 0, "could not get string from arg0");
		/* TODO throw error */
		return NULL;
	}

	if((env_result = getenv(temp_str)))
	{
		if(!(ret = boing_value_from_str(boing, env_result)))
		{
			boing_error(boing, 0, "could not create array value from string");
			/* TODO throw error */
			return NULL;
		}
	}
	else
	{
		if(!(ret = boing_value_from_double(boing, 0.0)))
		{
			boing_error(boing, 0, "could not create number value from double");
			/* TODO throw error */
			return NULL;
		}
	}

	/* cleanup */
	if(boing_str_release(boing, temp_str))
	{
		boing_error(boing, 0, "could not release string");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_srand(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "OS_SRAND expects a single numeric argument");
		/* TODO throw error */
		return NULL;
	}

	srand((int)args->array[0]->number);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not turn double into value");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_system(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	char *temp_str = NULL, *env_result = NULL;
	boing_value_t *ret = NULL;


	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "OS_SYSTEM expects a single string argument");
		/* TODO throw error */
		return NULL;
	}

	if(!(temp_str = boing_str_from_value_array(boing, args->array[0])))
	{
		boing_error(boing, 0, "could not get string from arg0");
		/* TODO throw error */
		return NULL;
	}

	if(!(ret = boing_value_from_double(boing, (double)system(temp_str))))
	{
		boing_error(boing, 0, "could not create number value from double");
		/* TODO throw error */
		return NULL;
	}

	/* cleanup */
	if(boing_str_release(boing, temp_str))
	{
		boing_error(boing, 0, "could not release string");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_remove(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	char *temp_str = NULL, *env_result = NULL;
	boing_value_t *ret = NULL;


	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "OS_REMOVE expects a single string argument");
		/* TODO throw error */
		return NULL;
	}

	if(!(temp_str = boing_str_from_value_array(boing, args->array[0])))
	{
		boing_error(boing, 0, "could not get string from arg0");
		/* TODO throw error */
		return NULL;
	}

	if(!(ret = boing_value_from_double(boing, (double)remove(temp_str))))
	{
		boing_error(boing, 0, "could not create number value from double");
		/* TODO throw error */
		return NULL;
	}

	/* cleanup */
	if(boing_str_release(boing, temp_str))
	{
		boing_error(boing, 0, "could not release string");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_rename(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	char *temp_str = NULL, *rename_str = NULL, *env_result = NULL;
	boing_value_t *ret = NULL;


	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY || args->array[1]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "OS_RENAME expects a 2 string arguments");
		/* TODO throw error */
		return NULL;
	}

	if(!(temp_str = boing_str_from_value_array(boing, args->array[0])))
	{
		boing_error(boing, 0, "could not get string from arg0");
		/* TODO throw error */
		return NULL;
	}

	if(!(rename_str = boing_str_from_value_array(boing, args->array[1])))
	{
		boing_error(boing, 0, "could not get string from arg1");
		/* TODO throw error */
		return NULL;
	}

	if(!(ret = boing_value_from_double(boing, (double)rename(temp_str, rename_str))))
	{
		boing_error(boing, 0, "could not create number value from double");
		/* TODO throw error */
		return NULL;
	}

	/* cleanup */
	if(boing_str_release(boing, temp_str))
	{
		boing_error(boing, 0, "could not release string");
		/* TODO throw error */
		return NULL;
	}

	if(boing_str_release(boing, rename_str))
	{
		boing_error(boing, 0, "could not release string");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_exit(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "OS_EXIT expects a single numeric argument");
		/* TODO throw error */
		return NULL;
	}


	boing_destroy(boing, 0);
	exit((int)args->array[0]->number);

	return ret;
}

static boing_value_t *module_os_function_sleep(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "OS_SLEEP expects a single numeric argument");
		/* TODO throw error */
		return NULL;
	}

	sleep((int)args->array[0]->number);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not create numeric return");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_socket(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	int fd, *fd_ptr = NULL;
	boing_value_t *ret = NULL;


	if(args->length != 3 || args->array[0]->type != BOING_TYPE_VALUE_NUMBER || args->array[1]->type != BOING_TYPE_VALUE_NUMBER || args->array[2]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "OS_SOCKET expects 3 numeric arguments");
		/* TODO throw error */
		return NULL;
	}

	#ifdef __WINDOWS__
		if((fd = socket((int)args->array[0]->number, (int)args->array[1]->number, (int)args->array[2]->number)) == INVALID_SOCKET)
	#else
		if((fd = socket((int)args->array[0]->number, (int)args->array[1]->number, (int)args->array[2]->number)) < 0)
	#endif
	{
		boing_error(boing, 0, "could not open socket");
		/* TODO throw error */
		return NULL;
	}

	if(!(fd_ptr = (int *)boing_str_request(boing, sizeof(int))))
	{
		boing_error(boing, 0, "could not request buffer for integer");
		/* TODO throw error */
		return NULL;
	}

	*fd_ptr = fd;

	if(!(ret = boing_value_from_ptr_managed(boing, fd_ptr, &module_os_managed_cleanup)))
	{
		boing_error(boing, 0, "could not turn double into value");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_setsockopt(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	int opt = 1, fd = 0;


	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || !args->array[0]->external.ptr)
	{
		boing_error(boing, 0, "OS_SETSOCKOPT expects a single external type argument");
		/* TODO throw error */
		return NULL;
	}

	fd = *((int *)((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr);

	#ifdef __WINDOWS__
		if(!(ret = boing_value_from_double(boing, (double)setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))))
	#else
		if(!(ret = boing_value_from_double(boing, (double)setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))))
	#endif
	{
		boing_error(boing, 0, "could not turn double into value");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_bind(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	struct sockaddr_in addr;
	int fd = 0;


	if(args->length != 4 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || !args->array[0]->external.ptr || args->array[1]->type != BOING_TYPE_VALUE_NUMBER || args->array[2]->type != BOING_TYPE_VALUE_NUMBER || args->array[3]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "OS_BIND expects a single external pointer argument and 3 numeric arguments");
		/* TODO throw error */
		return NULL;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));

	fd = *((int *)((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr);

	addr.sin_family = args->array[1]->number;
	addr.sin_addr.s_addr = htons((unsigned int)args->array[2]->number);
	addr.sin_port = htons((unsigned int)args->array[3]->number);

	if(!(ret = boing_value_from_double(boing, (double)bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)))))
	{
		boing_error(boing, 0, "could not turn double into value");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_listen(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	int fd = 0;


	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || !args->array[0]->external.ptr || args->array[1]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "OS_LISTEN expects a single external pointer argument and a numeric argument");
		/* TODO throw error */
		return NULL;
	}

	fd = *((int *)((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr);

	if(!(ret = boing_value_from_double(boing, (double)listen(fd, (int)args->array[1]->number))))
	{
		boing_error(boing, 0, "could not turn double into value");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_accept(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *temp = NULL;
	struct sockaddr_in addr;
	int fd = 0, new_fd = 0, *fd_ptr = NULL;


	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || !args->array[0]->external.ptr)
	{
		boing_error(boing, 0, "OS_ACCEPT expects a single external pointer argument");
		/* TODO throw error */
		return NULL;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));

	fd = *((int *)((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr);

	if((new_fd = accept(fd, (struct sockaddr *)&addr, (socklen_t *)&addr)) < 0)
	{
		if(!(ret = boing_value_from_double(boing, (double)-1.0)))
		{
			boing_error(boing, 0, "could not turn double into value");
			/* TODO throw error */
			return NULL;
		}
	}
	else
	{
		if(!(ret = boing_value_from_double(boing, (double)0.0)))
		{
			boing_error(boing, 0, "could not turn double into value");
			/* TODO throw error */
			return NULL;
		}

		if(!(fd_ptr = (int *)boing_str_request(boing, sizeof(int))))
		{
			boing_error(boing, 0, "could not request buffer for integer");
			/* TODO throw error */
			return NULL;
		}

		*fd_ptr = new_fd;

		if(!(temp = boing_value_from_ptr_managed(boing, fd_ptr, &module_os_managed_cleanup)))
		{
			boing_error(boing, 0, "could not turn double into external value");
			/* TODO throw error */
			return NULL;
		}

		if(boing_value_set(boing, args->array[1], temp))
		{
			boing_error(boing, 0, "could not set arg1 to external value");
			/* TODO throw error */
			return NULL;
		}

		if(boing_value_reference_dec(boing, temp))
		{
			boing_error(boing, 0, "could not refdec external value");
			/* TODO throw error */
			return NULL;
		}
	}


	return ret;
}

static boing_value_t *module_os_function_recv(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *temp = NULL;
	uint8_t buffer[2048];
	int fd = 0, len = 0, max = -1, total = 0, i;


	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || !args->array[0]->external.ptr || args->array[1]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "OS_RECV expects a single external type argument and a single numeric argument");
		/* TODO throw error */
		return NULL;
	}

	fd = *((int *)((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr);

	if(args->array[1]->number > 0.0)
		max = (int)args->array[1]->number;
	
	if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, max > 0 ? max : 1)))
	{
		boing_error(boing, 0, "could not request array value");
		/* TODO throw error */
		return NULL;
	}

	/* just keep recieving in a loop */
	while((len = recv(fd, buffer, 2048, 0)) >= 0)
	{
		if(max >= 0 && total + len > max)
		{
			for(i = 0; i < max; ++i)
			{
				if(!(temp = boing_value_from_double(boing, (double)buffer[i])))
				{
					boing_error(boing, 0, "could not request numeric value");
					/* TODO throw error */
					return NULL;
				}

				if(boing_value_array_append_insert(boing, ret, total + i, temp))
				{
					boing_error(boing, 0, "could not append value from buffer to return array");
					/* TODO throw error */
					return NULL;
				}
			}

			break;
		}

		for(i = 0; i < len; ++i)
		{
			if(!(temp = boing_value_from_double(boing, (double)buffer[i])))
			{
				boing_error(boing, 0, "could not request numeric value");
				/* TODO throw error */
				return NULL;
			}

			if(boing_value_array_append_insert(boing, ret, total + i, temp))
			{
				boing_error(boing, 0, "could not append value from buffer to return array");
				/* TODO throw error */
				return NULL;
			}
		}

		total += len;

		if(len < 2048)
			break;
	}

	return ret;
}

static boing_value_t *module_os_function_send(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	uint8_t *buffer = NULL;
	int fd = 0;


	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || !args->array[0]->external.ptr || args->array[1]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "OS_SEND expects a single external type argument and an array");
		/* TODO throw error */
		return NULL;
	}

	if(!(buffer = boing_str_from_value_array(boing, args->array[1])))
	{
		boing_error(boing, 0, "could not turn string into buffer");
		/* TODO throw error */
		return NULL;
	}


	fd = *((int *)((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr);

	/* past a certain number, a double wont be able to represent the value here. Not a huge deal though */
	if(!(ret = boing_value_from_double(boing, (double)send(fd, buffer, args->array[1]->length, 0))))
	{
		boing_error(boing, 0, "could not turn double into value");
		/* TODO throw error */
		return NULL;
	}

	if(boing_str_release(boing, buffer))
	{
		boing_error(boing, 0, "could not release buffer");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_shutdown(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	int fd = 0;


	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || !args->array[0]->external.ptr || args->array[1]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "OS_SHUTDOWN expects a single external pointer argument and a numeric argument");
		/* TODO throw error */
		return NULL;
	}

	fd = *((int *)((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr);

	if(!(ret = boing_value_from_double(boing, (double)shutdown(fd, (int)args->array[1]->number))))
	{
		boing_error(boing, 0, "could not turn double into value");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_connect(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	struct sockaddr_in addr;
	char *str = NULL;
	int fd = 0;


	if(args->length != 3 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || !args->array[0]->external.ptr || args->array[1]->type != BOING_TYPE_VALUE_ARRAY || args->array[2]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "OS_CONNECT expects a single external pointer argument, an array argument, and a numeric argument");
		/* TODO throw error */
		return NULL;
	}

	fd = *((int *)((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr);

	if(!(str = boing_str_from_value_array(boing, args->array[1])))
	{
		boing_error(boing, 0, "could not create string from array");
		/* TODO throw error */
		return NULL;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(args->array[2]->number);
	addr.sin_addr.s_addr = inet_addr(str);

	if(!(ret = boing_value_from_double(boing, (double)connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)))))
	{
		boing_error(boing, 0, "could not turn double into value");
		/* TODO throw error */
		return NULL;
	}

	if(boing_str_release(boing, str))
	{
		boing_error(boing, 0, "could not release string");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_get_ipv4(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(struct sockaddr_in);
	char str[INET_ADDRSTRLEN];
	int fd = 0;


	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || !args->array[0]->external.ptr)
	{
		boing_error(boing, 0, "OS_GET_IPVFOUR expects a single external type argument and an array");
		/* TODO throw error */
		return NULL;
	}


	fd = *((int *)((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr);

	if(getpeername(fd, (struct sockaddr *)&addr, &addr_len) < 0)
	{
		boing_error(boing, 0, "could not getpeername from socket");
		/* TODO throw error */
		return NULL;
	}

	inet_ntop(AF_INET, &addr.sin_addr, str, INET_ADDRSTRLEN);

	if(!(ret = boing_value_from_str(boing, str)))
	{
		boing_error(boing, 0, "could not turn string into value");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_get_ipv6(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	struct sockaddr_in6 addr;
	socklen_t addr_len = sizeof(struct sockaddr_in);
	char str[INET6_ADDRSTRLEN];
	int fd = 0;


	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || !args->array[0]->external.ptr)
	{
		boing_error(boing, 0, "OS_GET_IPVSIX expects a single external type argument and an array");
		/* TODO throw error */
		return NULL;
	}


	fd = *((int *)((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr);

	if(getpeername(fd, (struct sockaddr *)&addr, &addr_len) < 0)
	{
		boing_error(boing, 0, "could not getpeername from socket");
		/* TODO throw error */
		return NULL;
	}

	inet_ntop(AF_INET6, &addr.sin6_addr, str, INET_ADDRSTRLEN);

	if(!(ret = boing_value_from_str(boing, str)))
	{
		boing_error(boing, 0, "could not turn string into value");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

static boing_value_t *module_os_function_file_append(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	char *file_str = NULL;
	uint8_t *write_data = NULL;
	FILE *f;

	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY || args->array[1]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "OS_FILE_APPEND expects 2 arrays");
		/* TODO throw error */
		return NULL;
	}

	if(!(file_str = boing_str_from_value_array(boing, args->array[0])))
	{
		boing_error(boing, 0, "could not create file string from array");
		/* TODO throw error */
		return NULL;
	}

	if(!(f = fopen(file_str, "a")))
	{
		if(!(ret = boing_value_from_double(boing, 0.0)))
		{
			boing_error(boing, 0, "could not turn double into value");
			/* TODO throw error */
			return NULL;
		}

		if(boing_str_release(boing, file_str))
		{
			boing_error(boing, 0, "could not release file_str");
			/* TODO throw error */
			return NULL;
		}

		return ret;
	}

	if(boing_str_release(boing, file_str))
	{
		boing_error(boing, 0, "could not release file_str");
		/* TODO throw error */
		return NULL;
	}

	if(!(write_data = boing_str_from_value_array(boing, args->array[1])))
	{
		boing_error(boing, 0, "could not create write buffer from array");
		/* TODO throw error */
		return NULL;
	}

	if(fwrite(write_data, 1, args->array[1]->length, f) != args->array[1]->length)
	{
		boing_error(boing, 0, "could not append data to file");
		/* TODO throw error */
		return NULL;
	}

	if(boing_str_release(boing, write_data))
	{
		boing_error(boing, 0, "could not release write_data");
		/* TODO throw error */
		return NULL;
	}

	if(fclose(f))
	{
		if(!(ret = boing_value_from_double(boing, 0.0)))
		{
			boing_error(boing, 0, "could not turn double into value");
			/* TODO throw error */
			return NULL;
		}
	}
	else
	{
		if(!(ret = boing_value_from_double(boing, 1.0)))
		{
			boing_error(boing, 0, "could not turn double into value");
			/* TODO throw error */
			return NULL;
		}
	}


	return ret;
}

static boing_value_t *module_os_function_is_file(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	char *file_str = NULL;
	#ifndef __WINDOWS__
		struct stat st;
	#endif

	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "OS_IS_FILE expects 1 array");
		/* TODO throw error */
		return NULL;
	}

	if(!(file_str = boing_str_from_value_array(boing, args->array[0])))
	{
		boing_error(boing, 0, "could not create file string from array");
		/* TODO throw error */
		return NULL;
	}


	#ifdef __WINDOWS__
		if(PathFileExistsA(file_str))
	#elif __linux__
		if(!stat(file_str, &st) && (st.st_mode & __S_IFREG))
	#else
		if(!stat(file_str, &st) && (st.st_mode & S_IFREG))
	#endif
	{
		if(!(ret = boing_value_from_double(boing, 1.0)))
		{
			boing_error(boing, 0, "could not turn double into value");
			/* TODO throw error */
			return NULL;
		}
	}
	else
	{
		if(!(ret = boing_value_from_double(boing, 0.0)))
		{
			boing_error(boing, 0, "could not turn double into value");
			/* TODO throw error */
			return NULL;
		}
	}

	if(boing_str_release(boing, file_str))
	{
		boing_error(boing, 0, "could not release file_str");
		/* TODO throw error */
		return NULL;
	}


	return ret;
}

static boing_value_t *module_os_function_is_dir(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	char *file_str = NULL;
	#ifndef __WINDOWS__
		struct stat st;
	#endif

	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "OS_IS_DIR expects 1 array");
		/* TODO throw error */
		return NULL;
	}

	if(!(file_str = boing_str_from_value_array(boing, args->array[0])))
	{
		boing_error(boing, 0, "could not create file string from array");
		/* TODO throw error */
		return NULL;
	}

	#ifdef __WINDOWS__
		if(PathIsDirectoryA(file_str))
	#elif __linux__
		if(!stat(file_str, &st) && (st.st_mode & __S_IFDIR))
	#else
		if(!stat(file_str, &st) && (st.st_mode & S_IFDIR))
	#endif
	{
		if(!(ret = boing_value_from_double(boing, 1.0)))
		{
			boing_error(boing, 0, "could not turn double into value");
			/* TODO throw error */
			return NULL;
		}
	}
	else
	{
		if(!(ret = boing_value_from_double(boing, 0.0)))
		{
			boing_error(boing, 0, "could not turn double into value");
			/* TODO throw error */
			return NULL;
		}
	}

	if(boing_str_release(boing, file_str))
	{
		boing_error(boing, 0, "could not release file_str");
		/* TODO throw error */
		return NULL;
	}


	return ret;
}

static boing_value_t *module_os_function_list_dir(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *temp = NULL;
	char *file_str = NULL;
	#ifdef __WINDOWS__
		HANDLE file;
		LPWIN32_FIND_DATAA file_data;
	#else
		DIR *dir;
		struct dirent *dire;
	#endif

	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "OS_LIST_DIR expects 1 array");
		/* TODO throw error */
		return NULL;
	}

	if(!(file_str = boing_str_from_value_array(boing, args->array[0])))
	{
		boing_error(boing, 0, "could not create file string from array");
		/* TODO throw error */
		return NULL;
	}



	#ifdef __WINDOWS__
		if((file = FindFirstFileA(file_str, file_data)))
		{
			if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 1)))
			{
				boing_error(boing, 0, "could not request an array");
				/* TODO throw error */
				return NULL;
			}


			do
			{
				if(!(temp = boing_value_from_str(boing, file_data->cFileName)))
				{
					boing_error(boing, 0, "could not create value from file name");
					/* TODO throw error */
					return NULL;
				}

				/* add each file to the array */

				if(boing_value_array_append_insert(boing, ret, ret->length, temp))
				{
					boing_error(boing, 0, "could not add value to return array");
					/* TODO throw error */
					return NULL;
				}
			} while((file = FindNextFileA(file, file_data)));
		}
	#else
		if((dir = opendir(file_str)))
		{
			if(!(ret = boing_value_request(boing, BOING_TYPE_VALUE_ARRAY, 1)))
			{
				boing_error(boing, 0, "could not request an array");
				/* TODO throw error */
				return NULL;
			}

			while((dire = readdir(dir)))
			{
				if(!(temp = boing_value_from_str(boing, dire->d_name)))
				{
					boing_error(boing, 0, "could not create value from file name");
					/* TODO throw error */
					return NULL;
				}

				/* add each file to the array */

				if(boing_value_array_append_insert(boing, ret, ret->length, temp))
				{
					boing_error(boing, 0, "could not add value to return array");
					/* TODO throw error */
					return NULL;
				}
			}

			closedir(dir);
		}
	#endif
	else
	{
		if(!(ret = boing_value_from_double(boing, 0.0)))
		{
			boing_error(boing, 0, "could not turn double into value");
			/* TODO throw error */
			return NULL;
		}
	}

	if(boing_str_release(boing, file_str))
	{
		boing_error(boing, 0, "could not release file_str");
		/* TODO throw error */
		return NULL;
	}


	return ret;
}

#ifdef __EMSCRIPTEN__
/* unable to get around having to create a global for this.
Could maybe get around it if cleaning up the module didnt
require this */
typedef struct
{
	boing_t *boing;
	boing_value_t *callback, *program, *stack;
} emscripten_container_t;

emscripten_container_t container;


static void module_os_emscripten_callback_router(void *arg)
{
	emscripten_container_t *internal_container = (emscripten_container_t *)arg;
	boing_value_t *previous = NULL, *eval_ret = NULL;

	if(!(previous = boing_value_from_double(internal_container->boing, 0.0)))
	{
		boing_error(internal_container->boing, 0, "could not create temporary previous value for callback router");
		return;
	}

	if(!(eval_ret = boing_eval_value(internal_container->boing, internal_container->boing->program, internal_container->callback, internal_container->stack, previous)))
	{
		boing_error(internal_container->boing, 0, "error in callback eval");
	}

	/* cleanup */
	boing_value_reference_dec(internal_container->boing, previous);
}

static boing_value_t *module_os_function_emscripten_set_loop(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length != 1)
	{
		boing_error(boing, 0, "OS_EMSCRIPTEN_SET_LOOP expects a single argument");
		/* TODO throw error */
		return NULL;
	}

	if(container.boing) /* already initialized. need to cancel the current loop in case */
	{
		emscripten_cancel_main_loop();

		/* clean up the old references */
		boing_value_reference_dec(boing, container.callback);
		boing_value_reference_dec(boing, container.stack);
		boing_value_reference_dec(boing, container.program);
	}
	
	container.boing = boing;
	container.program = program;
	container.callback = args->array[0];
	container.stack = boing_value_stack_get_root(boing, stack);

	boing_value_reference_inc(boing, container.callback);
	boing_value_reference_inc(boing, container.program);
	boing_value_reference_inc(boing, container.stack);

	emscripten_set_main_loop_arg(&module_os_emscripten_callback_router, &container, 0, 1);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not create numeric return");
		/* TODO throw error */
		return NULL;
	}

	return ret;
}

#endif

/* public functions */


#ifdef MODULE_STANDALONE
int module_init(boing_t *boing, boing_module_t *module)
{
	return module_os_init(boing, module);
}

int module_destroy(boing_t *boing, boing_module_t *module)
{
	return module_os_destroy(boing, module);
}

int module_stack_add(boing_t *boing, boing_value_t *stack, boing_module_t *module)
{
	return module_stack_add(boing, stack, module);
}
#endif


int module_os_init(boing_t *boing, boing_module_t *module)
{
	#ifdef __EMSCRIPTEN__
	memset(&container, 0, sizeof(emscripten_container_t));
	#endif
	return 0;
}

int module_os_destroy(boing_t *boing, boing_module_t *module)
{
	#ifdef __EMSCRIPTEN__
	emscripten_cancel_main_loop();

	/* clean up the old references */
	boing_value_reference_dec(boing, container.callback);
	boing_value_reference_dec(boing, container.stack);
	boing_value_reference_dec(boing, container.program);
	#endif


	return 0;
}

int module_os_stack_add(boing_t *boing, boing_value_t *stack, boing_module_t *module)
{
	BOING_ADD_GLOBAL("OS_TIME", boing_value_from_ptr(boing, &module_os_function_time, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_GETENV", boing_value_from_ptr(boing, &module_os_function_getenv, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_SRAND", boing_value_from_ptr(boing, &module_os_function_srand, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_SYSTEM", boing_value_from_ptr(boing, &module_os_function_system, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_REMOVE", boing_value_from_ptr(boing, &module_os_function_remove, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_RENAME", boing_value_from_ptr(boing, &module_os_function_rename, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_EXIT", boing_value_from_ptr(boing, &module_os_function_exit, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_SLEEP", boing_value_from_ptr(boing, &module_os_function_sleep, BOING_EXTERNAL_FUNCTION, NULL));

	BOING_ADD_GLOBAL("OS_SOCKET", boing_value_from_ptr(boing, &module_os_function_socket, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_SETSOCKOPT", boing_value_from_ptr(boing, &module_os_function_setsockopt, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_BIND", boing_value_from_ptr(boing, &module_os_function_bind, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_LISTEN", boing_value_from_ptr(boing, &module_os_function_listen, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_ACCEPT", boing_value_from_ptr(boing, &module_os_function_accept, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_RECV", boing_value_from_ptr(boing, &module_os_function_recv, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_SEND", boing_value_from_ptr(boing, &module_os_function_send, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_SHUTDOWN", boing_value_from_ptr(boing, &module_os_function_shutdown, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_CONNECT", boing_value_from_ptr(boing, &module_os_function_connect, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_GET_IPVFOUR", boing_value_from_ptr(boing, &module_os_function_get_ipv4, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_GET_IPVSIX", boing_value_from_ptr(boing, &module_os_function_get_ipv6, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_FILE_APPEND", boing_value_from_ptr(boing, &module_os_function_file_append, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_IS_FILE", boing_value_from_ptr(boing, &module_os_function_is_file, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_IS_DIR", boing_value_from_ptr(boing, &module_os_function_is_dir, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("OS_LIST_DIR", boing_value_from_ptr(boing, &module_os_function_list_dir, BOING_EXTERNAL_FUNCTION, NULL));
	
	#ifdef __EMSCRIPTEN__
	BOING_ADD_GLOBAL("OS_EMSCRIPTEN_SET_LOOP", boing_value_from_ptr(boing, &module_os_function_emscripten_set_loop, BOING_EXTERNAL_FUNCTION, NULL));
	#endif

	/* non-functions */

	BOING_ADD_GLOBAL("OS_AF_INET", boing_value_from_double(boing, (double)AF_INET));
	BOING_ADD_GLOBAL("OS_AF_INET6", boing_value_from_double(boing, (double)AF_INET6));
	BOING_ADD_GLOBAL("OS_AF_UNIX", boing_value_from_double(boing, (double)AF_UNIX));

	BOING_ADD_GLOBAL("OS_SOCK_STREAM", boing_value_from_double(boing, (double)SOCK_STREAM));
	BOING_ADD_GLOBAL("OS_SOCK_DGRAM", boing_value_from_double(boing, (double)SOCK_DGRAM));
	BOING_ADD_GLOBAL("OS_SOCK_RAW", boing_value_from_double(boing, (double)SOCK_RAW));

	BOING_ADD_GLOBAL("OS_IPPROTO_UDP", boing_value_from_double(boing, (double)IPPROTO_UDP));
	BOING_ADD_GLOBAL("OS_IPPROTO_TCP", boing_value_from_double(boing, (double)IPPROTO_TCP));

	BOING_ADD_GLOBAL("OS_INADDR_ANY", boing_value_from_double(boing, (double)INADDR_ANY));

	#ifdef __WINDOWS__
		BOING_ADD_GLOBAL("OS_SHUT_WR", boing_value_from_double(boing, (double)SD_SEND));
		BOING_ADD_GLOBAL("OS_SHUT_RD", boing_value_from_double(boing, (double)SD_RECEIVE));
		BOING_ADD_GLOBAL("OS_SHUT_RDWR", boing_value_from_double(boing, (double)SD_BOTH));
	#else
		BOING_ADD_GLOBAL("OS_SHUT_WR", boing_value_from_double(boing, (double)SHUT_WR));
		BOING_ADD_GLOBAL("OS_SHUT_RD", boing_value_from_double(boing, (double)SHUT_RD));
		BOING_ADD_GLOBAL("OS_SHUT_RDWR", boing_value_from_double(boing, (double)SHUT_RDWR));
	#endif

	#ifdef __WINDOWS__
		BOING_ADD_GLOBAL("OS_NAME", boing_value_from_str(boing, "windows"));
	#elif __ANDROID__
		BOING_ADD_GLOBAL("OS_NAME", boing_value_from_str(boing, "android"));
	#elif __linux__
		BOING_ADD_GLOBAL("OS_NAME", boing_value_from_str(boing, "linux"));
	#elif unix
		BOING_ADD_GLOBAL("OS_NAME", boing_value_from_str(boing, "unix"));
	#elif __EMSCRIPTEN__
		BOING_ADD_GLOBAL("OS_NAME", boing_value_from_str(boing, "emscripten"));
	#else
		BOING_ADD_GLOBAL("OS_NAME", boing_value_from_str(boing, "unknown"));
	#endif
	
	return 0;
}