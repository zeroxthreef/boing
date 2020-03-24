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


#include "ffi.h"

#ifdef MODULE_STANDALONE
#include <boing.h>
#else
#include "../../boing.h"
#endif


/* 1. TODO: make a library container that holds the handle to a dll or so
   2. make boing value->c type and c type->boing value converter functions
   3. make a table->struct and struct->table converter
      - will require rows inside the table for alignment and type
   4. make bit operation functions
*/


#ifdef MODULE_STANDALONE
int module_init(boing_t *boing, boing_module_t *module)
{
	return module_ffi_init(boing, module);
}

int module_destroy(boing_t *boing, boing_module_t *module)
{
	return module_ffi_destroy(boing, module);
}

int module_stack_add(boing_t *boing, boing_value_t *stack, boing_module_t *module)
{
	return module_ffi_stack_add(boing, stack, module);
}
#endif


int module_ffi_init(boing_t *boing, boing_module_t *module)
{
	
	return 0;
}

int module_ffi_destroy(boing_t *boing, boing_module_t *module)
{
	
	return 0;
}

int module_ffi_stack_add(boing_t *boing, boing_value_t *stack, boing_module_t *module)
{

	return 0;
}