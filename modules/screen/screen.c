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


#include "screen.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef MODULE_STANDALONE
#include <boing.h>
#else
#include "../../boing.h"
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


typedef struct
{
	SDL_Window *window;
	SDL_Renderer *renderer;
} module_screen_window_t;

typedef struct
{
	SDL_Texture *texture;
	int width, height, access;
} module_screen_texture_t;

typedef struct
{
	TTF_Font *font;
	uint8_t quality;
} module_screen_font_t;


/* table creators */

static boing_value_t *module_screen_table_rect_create(boing_t *boing, SDL_Rect *rect)
{
	boing_value_t *ret = NULL;


	if(!(ret = boing_value_from_table_fmt(boing, "x|%d,y|%d,w|%d,h|%d", rect->x, rect->y, rect->w, rect->h)))
	{
		boing_error(boing, 0, "could not make table from rect");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_table_event_create(boing_t *boing, SDL_Event *event)
{
	boing_value_t *ret = NULL, *subtable = NULL;


	switch(event->type)
	{
		case SDL_QUIT:
			if(!(ret = boing_value_from_table_fmt(boing, "type|%u,timestamp|%u", SDL_QUIT, event->quit.timestamp)))
			{
				boing_error(boing, 0, "could not make table from event SDL_QUIT");
				return NULL;
			}
		break;
		case SDL_WINDOWEVENT:
			if(!(ret = boing_value_from_table_fmt(boing, "type|%u,timestamp|%u,windowID|%u,event|%u,data1|%d,data2|%d", SDL_WINDOWEVENT, event->window.timestamp, event->window.windowID, event->window.event, event->window.data1, event->window.data2)))
			{
				boing_error(boing, 0, "could not make table from event SDL_WINDOWEVENT");
				return NULL;
			}
		break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			if(!(subtable = boing_value_from_table_fmt(boing, "scancode|%u,keycode|%u,mod|%u", event->key.keysym.scancode, event->key.keysym.sym, event->key.keysym.mod)))
			{
				boing_error(boing, 0, "could not make subtable table from event SDL_KEYDOWN(or SDL_KEYUP)'s keysym");
				return NULL;
			}

			if(!(ret = boing_value_from_table_fmt(boing, "type|%u,timestamp|%u,windowID|%u,state|%u,repeat|%u,keysym|%v", event->type, event->key.timestamp, event->key.windowID, event->key.state, event->key.repeat, subtable)))
			{
				boing_error(boing, 0, "could not make table from event SDL_KEYDOWN/KEYUP");
				boing_value_reference_dec(boing, subtable);
				return NULL;
			}
		break;
		case SDL_MOUSEMOTION:
			if(!(ret = boing_value_from_table_fmt(boing, "type|%u,timestamp|%u,windowID|%u,which|%u,state|%u,x|%d,y|%d,xrel|%d,yrel|%d", event->type, event->motion.timestamp, event->motion.windowID, event->motion.which, event->motion.state, event->motion.x, event->motion.y, event->motion.xrel, event->motion.yrel)))
			{
				boing_error(boing, 0, "could not make table from event SDL_MOUSEMOTION");
				return NULL;
			}
		break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if(!(ret = boing_value_from_table_fmt(boing, "type|%u,timestamp|%u,windowID|%u,which|%u,button|%u,state|%u,x|%d,y|%d", event->type, event->button.timestamp, event->button.windowID, event->button.which, event->button.button, event->button.state, event->button.clicks, event->button.x, event->button.y)))
			{
				boing_error(boing, 0, "could not make table from event SDL_MOUSEBUTTONDOWN/UP");
				return NULL;
			}
		break;
		case SDL_MOUSEWHEEL:
			if(!(ret = boing_value_from_table_fmt(boing, "type|%u,timestamp|%u,windowID|%u,which|%u,x|%d,y|%d,direction|%u", event->type, event->wheel.timestamp, event->wheel.windowID, event->wheel.which, event->wheel.x, event->wheel.y, event->wheel.direction)))
			{
				boing_error(boing, 0, "could not make table from event SDL_MOUSEWHEEL");
				return NULL;
			}
		break;
		case SDL_USEREVENT:
			if(!(ret = boing_value_from_table_fmt(boing, "type|%u,timestamp|%u,windowID|%u,code|%d,data1|%v,data2|%v", event->type, event->user.timestamp, event->user.windowID, event->user.code, event->user.data1, event->user.data2)))
			{
				boing_error(boing, 0, "could not make table from event SDL_USEREVENT");
				return NULL;
			}
		break;
		/* TODO: add the rest of the SDL_Event types */
		default:
			if(!(ret = boing_value_from_table_fmt(boing, "type|%u", event->type)))
			{
				boing_error(boing, 0, "could not make table from an event");
				return NULL;
			}
		break;
	}

	return ret;
}


/* table converters */

int boing_screen_table_rect_convert(boing_t *boing, boing_value_t *table, SDL_Rect *rect)
{
	boing_value_t *temp = NULL;


	if(!(temp = boing_value_table_get_str(boing, table, "x")) && temp->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "incorrect table format for rect");
		return 1;
	}

	rect->x = temp->number;

	if(!(temp = boing_value_table_get_str(boing, table, "y")) && temp->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "incorrect table format for rect");
		return 1;
	}

	rect->y = temp->number;

	if(!(temp = boing_value_table_get_str(boing, table, "w")) && temp->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "incorrect table format for rect");
		return 1;
	}

	rect->w = temp->number;

	if(!(temp = boing_value_table_get_str(boing, table, "h")) && temp->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "incorrect table format for rect");
		return 1;
	}

	rect->h = temp->number;

	return 0;
}


/* cleanup callbacks */


static int module_screen_window_cleanup(boing_t *boing, boing_value_t *value)
{
	module_screen_window_t *window = NULL;


	window = ((boing_managed_ptr_t *)value->external.ptr)->ptr;

	/* destroy renderer and window */

	SDL_DestroyRenderer(window->renderer);
	SDL_DestroyWindow(window->window);

	if(boing_str_release(boing, (char *)window))
	{
		boing_error(boing, 0, "could not release window struct");
		return 1;
	}

	return 0;
}

static int module_screen_texture_cleanup(boing_t *boing, boing_value_t *value)
{
	module_screen_texture_t *texture = NULL;


	texture = ((boing_managed_ptr_t *)value->external.ptr)->ptr;

	SDL_DestroyTexture(texture->texture);

	if(boing_str_release(boing, (char *)texture))
	{
		boing_error(boing, 0, "could not release texture struct");
		return 1;
	}

	return 0;
}

static int module_screen_font_cleanup(boing_t *boing, boing_value_t *value)
{
	module_screen_font_t *font = NULL;


	font = ((boing_managed_ptr_t *)value->external.ptr)->ptr;

	TTF_CloseFont(font->font);

	if(boing_str_release(boing, (char *)font))
	{
		boing_error(boing, 0, "could not release font struct");
		return 1;
	}

	return 0;
}

/* functions */

static boing_value_t *module_screen_function_create_window(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	module_screen_window_t *window = NULL;
	boing_value_t *ret = NULL;
	char *title = NULL;

	/* title width height resizeable */
	if(args->length != 4 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY || args->array[1]->type != BOING_TYPE_VALUE_NUMBER || args->array[2]->type != BOING_TYPE_VALUE_NUMBER || args->array[3]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_CREATE_WINDOW expects a string title, numeric width and height and 1 or 0 for if its resizable");
		return NULL;
	}

	if(!(window = (module_screen_window_t *)boing_str_request(boing, sizeof(module_screen_window_t))))
	{
		boing_error(boing, 0, "could not create window struct");
		return NULL;
	}

	if(!(title = boing_str_from_value_array(boing, args->array[0])))
	{
		boing_error(boing, 0, "could not create title string");
		boing_str_release(boing, (char *)window);
		return NULL;
	}

	/* create the window and renderer */
	if(!(window->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (int)args->array[1]->number, (int)args->array[2]->number, SDL_WINDOW_SHOWN | (args->array[3]->number ? SDL_WINDOW_RESIZABLE : 0))))
	{
		boing_error(boing, 0, "could not create window");
		boing_str_release(boing, (char *)window);
		boing_str_release(boing, title);
		return NULL;
	}

	if(!(window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_ACCELERATED)))
	{
		boing_error(boing, 0, "could not create renderer");
		SDL_DestroyWindow(window->window);
		boing_str_release(boing, (char *)window);
		boing_str_release(boing, title);
		return NULL;
	}

	/* cleanup and then create a managed pointer from the struct */

	if(boing_str_release(boing, title))
	{
		boing_error(boing, 0, "could not release title string");
		SDL_DestroyWindow(window->window);
		SDL_DestroyRenderer(window->renderer);
		boing_str_release(boing, (char *)window);
		return NULL;
	}

	if(!(ret = boing_value_from_ptr_managed(boing, window, &module_screen_window_cleanup)))
	{
		boing_error(boing, 0, "could not make managed pointer for window");
		SDL_DestroyWindow(window->window);
		SDL_DestroyRenderer(window->renderer);
		boing_str_release(boing, (char *)window);
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_window_clear(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	module_screen_window_t *window = NULL;
	boing_value_t *ret = NULL;


	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL)
	{
		boing_error(boing, 0, "SCREEN_WINDOW_CLEAR expects single window external value");
		return NULL;
	}

	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;

	SDL_RenderClear(window->renderer);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_window_present(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	module_screen_window_t *window = NULL;
	boing_value_t *ret = NULL;


	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL)
	{
		boing_error(boing, 0, "SCREEN_WINDOW_PRESENT expects single window external value");
		return NULL;
	}

	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;

	SDL_RenderPresent(window->renderer);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_draw_color(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	module_screen_window_t *window = NULL;
	boing_value_t *ret = NULL;


	if(args->length != 5 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[1]->type != BOING_TYPE_VALUE_NUMBER || args->array[2]->type != BOING_TYPE_VALUE_NUMBER || args->array[3]->type != BOING_TYPE_VALUE_NUMBER || args->array[4]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_DRAW_COLOR expects single window external value and 4 numbers for rgba");
		return NULL;
	}

	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;

	SDL_SetRenderDrawColor(window->renderer, args->array[1]->number, args->array[2]->number, args->array[3]->number, args->array[4]->number);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_draw_line(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	module_screen_window_t *window = NULL;
	boing_value_t *ret = NULL;


	if(args->length != 5 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[1]->type != BOING_TYPE_VALUE_NUMBER || args->array[2]->type != BOING_TYPE_VALUE_NUMBER || args->array[3]->type != BOING_TYPE_VALUE_NUMBER || args->array[4]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_DRAW_LINE expects single window external value and 4 numbers for x1, y1, x2, y2");
		return NULL;
	}

	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;

	SDL_RenderDrawLine(window->renderer, args->array[1]->number, args->array[2]->number, args->array[3]->number, args->array[4]->number);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_draw_pixel(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	module_screen_window_t *window = NULL;
	boing_value_t *ret = NULL;


	if(args->length != 3 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[1]->type != BOING_TYPE_VALUE_NUMBER || args->array[2]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_DRAW_PIXEL expects single window external value and 2 numbers for x, y");
		return NULL;
	}

	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;

	/* this is very slow. It would be faster to make a temporary texture in the window struct and use that to draw points to with UpdateTexture */
	SDL_RenderDrawPoint(window->renderer, args->array[1]->number, args->array[2]->number);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_draw_rect(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	module_screen_window_t *window = NULL;
	boing_value_t *ret = NULL;
	SDL_Rect rect;


	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[1]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "SCREEN_DRAW_RECT expects single window external value and a rect table");
		return NULL;
	}

	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;

	if(boing_screen_table_rect_convert(boing, args->array[1], &rect))
	{
		boing_error(boing, 0, "could not make rect from table");
		return NULL;
	}

	SDL_RenderFillRect(window->renderer, &rect);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_draw_rect_lines(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	module_screen_window_t *window = NULL;
	boing_value_t *ret = NULL;
	SDL_Rect rect;


	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[1]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "SCREEN_DRAW_RECT_LINES expects single window external value and a rect table");
		return NULL;
	}

	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;

	if(boing_screen_table_rect_convert(boing, args->array[1], &rect))
	{
		boing_error(boing, 0, "could not make rect from table");
		return NULL;
	}

	SDL_RenderDrawRect(window->renderer, &rect);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_rect(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	SDL_Rect rect;


	if(args->length != 4 || args->array[0]->type != BOING_TYPE_VALUE_NUMBER || args->array[1]->type != BOING_TYPE_VALUE_NUMBER || args->array[2]->type != BOING_TYPE_VALUE_NUMBER || args->array[3]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_RECT expects 4 number values for x, y, w, h");
		return NULL;
	}

	rect.x = args->array[0]->number;
	rect.y = args->array[1]->number;
	rect.w = args->array[2]->number;
	rect.h = args->array[3]->number;


	if(!(ret = module_screen_table_rect_create(boing, &rect)))
	{
		boing_error(boing, 0, "could not make table return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_set_blending(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	module_screen_window_t *window = NULL;
	boing_value_t *ret = NULL;


	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[1]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_SET_BLENDING expects single window external value and a single number for the blendmode");
		return NULL;
	}

	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;

	SDL_SetRenderDrawBlendMode(window->renderer, (SDL_BlendMode)args->array[1]->number);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_poll_event(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *temp = NULL;
	SDL_Event event;
	int result = 0;
	


	if(args->length != 1)
	{
		boing_error(boing, 0, "SCREEN_POLL_EVENT expects a value to be set to an event table");
		return NULL;
	}

	
	if((result = SDL_PollEvent(&event)))
	{
		if(!(temp = module_screen_table_event_create(boing, &event)))
		{
			boing_error(boing, 0, "could not create event table from event");
			return NULL;
		}

		if(boing_value_set(boing, args->array[0], temp))
		{
			boing_error(boing, 0, "could not set arg1 to event table");
			return NULL;
		}

		if(boing_value_reference_dec(boing, temp))
		{
			boing_error(boing, 0, "could not refdec temporary value");
			return NULL;
		}
	}
	

	if(!(ret = boing_value_from_double(boing, (double)result)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_delay(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_DELAY expects a single number");
		return NULL;
	}

	SDL_Delay((Uint32)args->array[0]->number);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_ticks(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(!(ret = boing_value_from_double(boing, (double)SDL_GetTicks())))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_window_title(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	module_screen_window_t *window = NULL;
	boing_value_t *ret = NULL;
	char *title = NULL;


	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[1]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "SCREEN_WINDOW_TITLE expects single window external value and an array for the title string");
		return NULL;
	}

	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;

	if(!(title = boing_str_from_value_array(boing, args->array[1])))
	{
		boing_error(boing, 0, "could not make string from argument");
		return NULL;
	}
	

	SDL_SetWindowTitle(window->window, title);

	if(boing_str_release(boing, title))
	{
		boing_error(boing, 0, "could not release string");
		return NULL;
	}

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_messagebox(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	module_screen_window_t *window = NULL;
	boing_value_t *ret = NULL;
	char *title = NULL, *message = NULL;


	if(args->length != 3 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY || args->array[1]->type != BOING_TYPE_VALUE_ARRAY || args->array[2]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_MESSAGEBOX expects a string for title, string for message, and a number for type");
		return NULL;
	}

	if(!(title = boing_str_from_value_array(boing, args->array[0])))
	{
		boing_error(boing, 0, "could not make string from argument");
		return NULL;
	}

	if(!(message = boing_str_from_value_array(boing, args->array[1])))
	{
		boing_error(boing, 0, "could not make string from argument");
		boing_str_release(boing, title);
		return NULL;
	}
	

	SDL_ShowSimpleMessageBox(args->array[2]->number, title, message, NULL);

	if(boing_str_release(boing, title))
	{
		boing_error(boing, 0, "could not release string");
		boing_str_release(boing, message);
		return NULL;
	}

	if(boing_str_release(boing, message))
	{
		boing_error(boing, 0, "could not release string");
		return NULL;
	}

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_cursor(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	SDL_Cursor *cur = NULL;


	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_CURSOR expects single number for the type");
		return NULL;
	}

	if(!(cur = SDL_CreateSystemCursor(args->array[0]->number)))
	{
		if(!(ret = boing_value_from_double(boing, 1.0)))
		{
			boing_error(boing, 0, "could not make number return");
			return NULL;
		}

		return ret;
	}
	
	SDL_SetCursor(cur);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_cursor_visible(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;


	if(args->length != 1 || args->array[0]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_CURSOR_VISIBLE expects single number for the status");
		return NULL;
	}
	
	SDL_ShowCursor(args->array[0]->number);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_intersect_rect(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL, *temp = NULL;
	SDL_Rect rect_a, rect_b, rect_result;


	if(args->length != 3 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY || args->array[1]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "SCREEN_INTERSECT_RECT expects 2 rect tables and 1 value to store the result");
		return NULL;
	}


	if(boing_screen_table_rect_convert(boing, args->array[0], &rect_a))
	{
		boing_error(boing, 0, "could not make rect from table");
		return NULL;
	}

	if(boing_screen_table_rect_convert(boing, args->array[1], &rect_b))
	{
		boing_error(boing, 0, "could not make rect from table");
		return NULL;
	}

	if(SDL_IntersectRect(&rect_a, &rect_b, &rect_result))
	{
		if(!(ret = boing_value_from_double(boing, 1.0)))
		{
			boing_error(boing, 0, "could not make number return");
			return NULL;
		}

		if(!(temp = module_screen_table_rect_create(boing, &rect_result)))
		{
			boing_error(boing, 0, "could not make result rect table");
			return NULL;
		}

		if(boing_value_set(boing, args->array[2], temp))
		{
			boing_error(boing, 0, "could not set arg2 to rect table");
			return NULL;
		}

		if(boing_value_reference_dec(boing, temp))
		{
			boing_error(boing, 0, "could not refdec temporary value");
			return NULL;
		}
	}
	else
	{
		if(!(ret = boing_value_from_double(boing, 0.0)))
		{
			boing_error(boing, 0, "could not make number return");
			return NULL;
		}
	}

	return ret;
}

static boing_value_t *module_screen_function_create_texture(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	module_screen_texture_t *texture = NULL;
	module_screen_window_t *window = NULL;


	if(args->length != 4 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[1]->type != BOING_TYPE_VALUE_NUMBER || args->array[2]->type != BOING_TYPE_VALUE_NUMBER || args->array[3]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_CREATE_TEXTURE expects an external value for the window/renderer and 3 numbers for width, height, and the type");
		return NULL;
	}

	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;

	if(!(texture = (module_screen_texture_t *)boing_str_request(boing, sizeof(module_screen_texture_t))))
	{
		boing_error(boing, 0, "could not request buffer for texture struct");
		return NULL;
	}

	if(!(texture->texture = SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_RGBA32, args->array[3]->number ? SDL_TEXTUREACCESS_TARGET : SDL_TEXTUREACCESS_STREAMING, args->array[1]->number, args->array[2]->number)))
	{
		boing_error(boing, 0, "could not create texture");
		boing_str_release(boing, (char *)texture);
		return NULL;
	}

	texture->width = args->array[1]->number;
	texture->height = args->array[2]->number;
	texture->access = args->array[3]->number ? SDL_TEXTUREACCESS_TARGET : SDL_TEXTUREACCESS_STREAMING;

	SDL_SetTextureBlendMode(texture->texture, SDL_BLENDMODE_BLEND);

	if(!(ret = boing_value_from_ptr_managed(boing, texture, &module_screen_texture_cleanup)))
	{
		boing_error(boing, 0, "could not make managed texture return");
		boing_str_release(boing, (char *)texture);
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_create_texture_image(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	size_t size = 0;
	uint8_t *file_data = NULL, *image_data = NULL;
	boing_value_t *ret = NULL;
	module_screen_texture_t *texture = NULL;
	module_screen_window_t *window = NULL;
	int width = 0, height = 0, channels = 0;


	if(args->length != 3 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[1]->type != BOING_TYPE_VALUE_ARRAY || args->array[2]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_CREATE_TEXTURE_IMAGE expects an external value for the window/renderer, an array (image data) for the image to read, and a number for true false for textureaccess");
		return NULL;
	}

	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;

	if(!(texture = (module_screen_texture_t *)boing_str_request(boing, sizeof(module_screen_texture_t))))
	{
		boing_error(boing, 0, "could not request buffer for texture struct");
		return NULL;
	}

	if(!(file_data = boing_str_from_value_array(boing, args->array[1])))
	{
		boing_error(boing, 0, "could not create file data from array");
		boing_str_release(boing, (char *)texture);
		return NULL;
	}

	size = args->array[1]->length;

	/* use stb to load the image */
	if(!(image_data = stbi_load_from_memory(file_data, size, &width, &height, &channels, STBI_rgb_alpha)))
	{
		boing_error(boing, 0, "could not read image");
		boing_str_release(boing, (char *)texture);
		boing_str_release(boing, file_data);
		return NULL;
	}

	if(boing_str_release(boing, file_data))
	{
		boing_error(boing, 0, "could not release file string");
		boing_str_release(boing, (char *)texture);
		stbi_image_free(image_data);
		return NULL;
	}
	
	/* create and write the image to the texture */

	if(!(texture->texture = SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_RGBA32, args->array[2]->number ? SDL_TEXTUREACCESS_TARGET : SDL_TEXTUREACCESS_STREAMING, width, height)))
	{
		boing_error(boing, 0, "could not create texture");
		boing_str_release(boing, (char *)texture);
		stbi_image_free(image_data);
		return NULL;
	}

	texture->width = width;
	texture->height = height;
	texture->access = args->array[2]->number ? SDL_TEXTUREACCESS_TARGET : SDL_TEXTUREACCESS_STREAMING;

	SDL_SetTextureBlendMode(texture->texture, SDL_BLENDMODE_BLEND);

	SDL_UpdateTexture(texture->texture, NULL, image_data, width * (sizeof(uint8_t) * 4));

	stbi_image_free(image_data);

	if(!(ret = boing_value_from_ptr_managed(boing, texture, &module_screen_texture_cleanup)))
	{
		boing_error(boing, 0, "could not make managed texture return");
		boing_str_release(boing, (char *)texture);
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_draw_texture(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	module_screen_texture_t *texture = NULL;
	module_screen_window_t *window = NULL;
	SDL_Rect src, dest, *srcptr = NULL, *destptr = NULL;
	SDL_Point center;
	int flip, ret_int = 0;


	if(args->length < 4 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[1]->type != BOING_TYPE_VALUE_EXTERNAL || (args->array[2]->type != BOING_TYPE_VALUE_ARRAY && args->array[2]->type != BOING_TYPE_VALUE_NUMBER) || (args->array[3]->type != BOING_TYPE_VALUE_ARRAY && args->array[3]->type != BOING_TYPE_VALUE_NUMBER))
	{
		boing_error(boing, 0, "SCREEN_DRAW_TEXTURE expects an external type for the window/renderer, a texture, and 2 tables/two numbers for the source and destination rectangles at least");
		return NULL;
	}
	else if(args->length == 7 && (args->array[4]->type != BOING_TYPE_VALUE_NUMBER || args->array[5]->type != BOING_TYPE_VALUE_ARRAY || args->array[6]->type != BOING_TYPE_VALUE_NUMBER))
	{
		boing_error(boing, 0, "SCREEN_DRAW_TEXTURE expects, if given the extra arguments, an external type for the window/renderer, a texture, 2 tables/two numbers for the source and destination rectangles, a number angle, a point array for the rotation point [x, y], and a number for the flip type");
		return NULL;
	}
	


	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;

	texture = ((boing_managed_ptr_t *)args->array[1]->external.ptr)->ptr;

	if(args->array[2]->type == BOING_TYPE_VALUE_ARRAY && boing_screen_table_rect_convert(boing, args->array[2], &src))
	{
		boing_error(boing, 0, "could not create rect from table");
		return NULL;
	}
	else if(args->array[2]->type == BOING_TYPE_VALUE_ARRAY)
		srcptr = &src;


	if(args->array[3]->type == BOING_TYPE_VALUE_ARRAY && boing_screen_table_rect_convert(boing, args->array[3], &dest))
	{
		boing_error(boing, 0, "could not create rect from table");
		return NULL;
	}
	else if(args->array[3]->type == BOING_TYPE_VALUE_ARRAY)
		destptr = &dest;


	/* determine whether simple or complex version */
	if(args->length == 4)
	{
		ret_int = SDL_RenderCopy(window->renderer, texture->texture, srcptr, destptr);
	}
	else
	{
		if(args->array[5]->length >= 2)
		{
			center.x = args->array[5]->array[0]->number;
			center.x = args->array[5]->array[1]->number;
		}

		ret_int = SDL_RenderCopyEx(window->renderer, texture->texture, srcptr, destptr, args->array[4]->number, args->array[5]->length ? &center : NULL, args->array[6]->number);
	}

	
	if(!(ret = boing_value_from_double(boing, (double)ret_int)))
	{
		boing_error(boing, 0, "could not make number return");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_draw_target(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	module_screen_texture_t *texture = NULL;
	module_screen_window_t *window = NULL;


	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || (args->array[1]->type != BOING_TYPE_VALUE_NUMBER && args->array[1]->type != BOING_TYPE_VALUE_EXTERNAL))
	{
		boing_error(boing, 0, "SCREEN_DRAW_TARGET expects an external value for the window/renderer and a number/external value for the texture");
		return NULL;
	}

	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;

	if(args->array[1]->type == BOING_TYPE_VALUE_EXTERNAL)
		texture = ((boing_managed_ptr_t *)args->array[1]->external.ptr)->ptr;

	if(!(ret = boing_value_from_double(boing, (double)SDL_SetRenderTarget(window->renderer, texture ? texture->texture : NULL))))
	{
		boing_error(boing, 0, "could not make return number");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_create_font(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	module_screen_font_t *font = NULL;
	SDL_RWops *rw = NULL;
	uint8_t *file_str = NULL;


	if(args->length != 3 || args->array[0]->type != BOING_TYPE_VALUE_ARRAY || args->array[1]->type != BOING_TYPE_VALUE_NUMBER || args->array[2]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_CREATE_FONT expects an array value with the font file data and a number for the point (size), and a number for quality");
		return NULL;
	}


	if(!(file_str = boing_str_from_value_array(boing, args->array[0])))
	{
		boing_error(boing, 0, "could not make array from arg0");
		return NULL;
	}

	if(!(rw = SDL_RWFromMem(file_str, args->array[0]->length)))
	{
		boing_error(boing, 0, "could not SDL_RWFromMem the data");
		boing_str_release(boing, (char *)file_str);
		return NULL;
	}


	if(boing_str_release(boing, (char *)file_str))
	{
		boing_error(boing, 0, "could not release file_str");
		return NULL;
	}

	if((!(font = (module_screen_font_t *)boing_str_request(boing, sizeof(module_screen_font_t)))))
	{
		boing_error(boing, 0, "could not request font buffer");
		return NULL;
	}

	font->quality = args->array[2]->number;


	if(!(font->font = TTF_OpenFontRW(rw, 1, (int)args->array[1]->number)))
	{
		boing_error(boing, 0, "could not open font");
		boing_str_release(boing, (char *)font);
		return NULL;
	}


	if(!(ret = boing_value_from_ptr_managed(boing, font, &module_screen_font_cleanup)))
	{
		boing_error(boing, 0, "could not make return managed font ptr");
		TTF_CloseFont(font->font);
		boing_str_release(boing, (char *)font);
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_draw_string(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	char *str = NULL;
	SDL_Rect dest;
	SDL_Color fg_color, bg_color;
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;
	boing_value_t *ret = NULL;
	module_screen_font_t *font = NULL;
	module_screen_window_t *window = NULL;


	if(args->length != 7 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[1]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[2]->type != BOING_TYPE_VALUE_ARRAY || args->array[3]->type != BOING_TYPE_VALUE_ARRAY || args->array[4]->type != BOING_TYPE_VALUE_ARRAY || args->array[5]->type != BOING_TYPE_VALUE_NUMBER || args->array[6]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_DRAW_STRING expects an external value for the window/renderer, an external value for the font, a string for the string, an array for the fg color (RGBA), an array for the bg color (RGBA), a number for the x pos, and a number for the y pos");
		return NULL;
	}

	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;
	font = ((boing_managed_ptr_t *)args->array[1]->external.ptr)->ptr;
	
	if(args->array[3]->length == 4)
	{
		fg_color.r = args->array[4]->array[0]->number;
		fg_color.g = args->array[4]->array[1]->number;
		fg_color.b = args->array[4]->array[2]->number;
		fg_color.a = args->array[4]->array[3]->number;
	}
	else
	{
		fg_color.r = 255;
		fg_color.g = 255;
		fg_color.b = 255;
		fg_color.a = 255;
	}

	if(args->array[4]->length == 4)
	{
		bg_color.r = args->array[4]->array[0]->number;
		bg_color.g = args->array[4]->array[1]->number;
		bg_color.b = args->array[4]->array[2]->number;
		bg_color.a = args->array[4]->array[3]->number;
	}
	else
	{
		bg_color.r = 0;
		bg_color.g = 0;
		bg_color.b = 0;
		bg_color.a = 0;
	}

	if(!(str = boing_str_from_value_array(boing, args->array[2])))
	{
		boing_error(boing, 0, "could not make string");
		return NULL;
	}


	switch(font->quality)
	{
		case 0:
			if(!(surface = TTF_RenderUTF8_Solid(font->font, str, fg_color)))
			{
				boing_error(boing, 0, "could not render solid text");
				boing_str_release(boing, str);
				return NULL;
			}
		break;
		case 1:
			if(!(surface = TTF_RenderUTF8_Shaded(font->font, str, fg_color, bg_color)))
			{
				boing_error(boing, 0, "could not render shaded text");
				boing_str_release(boing, str);
				return NULL;
			}
		break;
		case 2:
			if(!(surface = TTF_RenderUTF8_Blended(font->font, str, fg_color)))
			{
				boing_error(boing, 0, "could not render blended text");
				boing_str_release(boing, str);
				return NULL;
			}
		break;
	}


	if(boing_str_release(boing, str))
	{
		boing_error(boing, 0, "could not release string");
		SDL_FreeSurface(surface);
		return NULL;
	}


	/* convert surface to texture, draw it, then clean up everything */
	if(!(texture = SDL_CreateTextureFromSurface(window->renderer, surface)))
	{
		boing_error(boing, 0, "could not create texture");
		SDL_FreeSurface(surface);
		return NULL;
	}
	
	dest.x = args->array[5]->number;
	dest.y = args->array[6]->number;
	dest.w = dest.x + surface->w;
	dest.h = dest.y + surface->h;

	SDL_FreeSurface(surface);

	SDL_RenderCopy(window->renderer, texture, NULL, &dest);


	SDL_DestroyTexture(texture);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make return number");
		
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_string_drawsize(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	SDL_Rect rect;
	char *str = NULL;
	boing_value_t *ret = NULL;
	module_screen_font_t *font = NULL;


	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[1]->type != BOING_TYPE_VALUE_ARRAY)
	{
		boing_error(boing, 0, "SCREEN_STRING_DRAWSIZE expects an external value for the font and an array for the string");
		return NULL;
	}

	memset(&rect, 0, sizeof(SDL_Rect));

	font = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;


	if(!(str = boing_str_from_value_array(boing, args->array[1])))
	{
		boing_error(boing, 0, "could not make string");
		return NULL;
	}
	
	TTF_SizeUTF8(font->font, str, &rect.w, &rect.h);

	if(boing_str_release(boing, str))
	{
		boing_error(boing, 0, "could not release string");
		return NULL;
	}

	if(!(ret = module_screen_table_rect_create(boing, &rect)))
	{
		boing_error(boing, 0, "could not make return rect table");
		return NULL;
	}

	return ret;
}

static boing_value_t *module_screen_function_set_font_style(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	boing_value_t *ret = NULL;
	module_screen_font_t *font = NULL;


	if(args->length != 2 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[1]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_SET_FONT_STYLE expects an external value for the font and a number for the attribute to set");
		return NULL;
	}

	font = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;

	TTF_SetFontStyle(font->font, TTF_GetFontStyle(font->font) | (int)args->array[1]->number);

	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make return rect table");
		return NULL;
	}

	return ret;
}

#if 0
static boing_value_t *module_screen_function_resize_texture(boing_t *boing, boing_value_t *program, boing_value_t *stack, boing_value_t *previous, boing_value_t *args)
{
	module_screen_texture_t *texture = NULL;
	module_screen_window_t *window = NULL;
	SDL_Texture *new = NULL;
	SDL_Rect rect = {0, 0, 0, 0};
	boing_value_t *ret = NULL;
	uint8_t *data = NULL;
	int pitch = 4;


	if(args->length != 4 || args->array[0]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[1]->type != BOING_TYPE_VALUE_EXTERNAL || args->array[2]->type != BOING_TYPE_VALUE_NUMBER || args->array[3]->type != BOING_TYPE_VALUE_NUMBER)
	{
		boing_error(boing, 0, "SCREEN_RESIZE_TEXTURE expects an external value for the window and then another for the texture then 2 numbers for width and height");
		return NULL;
	}

	window = ((boing_managed_ptr_t *)args->array[0]->external.ptr)->ptr;
	texture = ((boing_managed_ptr_t *)args->array[1]->external.ptr)->ptr;

	if(!(new = SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_RGBA32, texture->access, (int)args->array[2]->number, (int)args->array[3]->number)))

	if(SDL_LockTexture(texture->texture, NULL, (void **)&data, &pitch))
	{
		boing_error(boing, 0, "could not lock texture");
		SDL_DestroyTexture(new);
		return NULL;
	}
	
	rect.w = texture->width;
	rect.h = texture->height;

	/* update new texture and slice or stretch the old texture onto it */
	printf("fgdfgdfg %d %p\n", pitch, data);
	SDL_UpdateTexture(new, &rect, data, pitch);


	SDL_UnlockTexture(texture->texture);
	SDL_DestroyTexture(texture->texture);

	texture->texture = new;

	texture->width = args->array[2]->number;
	texture->height = args->array[3]->number;


	if(!(ret = boing_value_from_double(boing, 0.0)))
	{
		boing_error(boing, 0, "could not make return rect table");
		return NULL;
	}

	return ret;
}
#endif

#ifdef MODULE_STANDALONE
int module_init(boing_t *boing, boing_module_t *module)
{
	return module_screen_init(boing, module);
}

int module_destroy(boing_t *boing, boing_module_t *module)
{
	return module_screen_destroy(boing, module);
}

int module_stack_add(boing_t *boing, boing_value_t *stack, boing_module_t *module)
{
	return module_screen_stack_add(boing, stack, module);
}
#endif


int module_screen_init(boing_t *boing, boing_module_t *module)
{
	if(SDL_Init(SDL_INIT_EVERYTHING))
	{
		boing_error(boing, 0, "could not initialize SDL2");
		return 1;
	}

	if(TTF_Init())
	{
		boing_error(boing, 0, "could not initialize SDL2_ttf");
		return 1;
	}

	return 0;
}

int module_screen_destroy(boing_t *boing, boing_module_t *module)
{
	TTF_Quit();
	SDL_Quit();

	return 0;
}

int module_screen_stack_add(boing_t *boing, boing_value_t *stack, boing_module_t *module)
{
	#define ADD_SDL_ENUM(en) BOING_ADD_GLOBAL("SCREEN_" #en, boing_value_from_double(boing, (double)SDL_##en))
	
	/* functions */
	BOING_ADD_GLOBAL("SCREEN_CREATE_WINDOW", boing_value_from_ptr(boing, &module_screen_function_create_window, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SCREEN_WINDOW_CLEAR", boing_value_from_ptr(boing, &module_screen_function_window_clear, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SCREEN_WINDOW_PRESENT", boing_value_from_ptr(boing, &module_screen_function_window_present, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SCREEN_WINDOW_TITLE", boing_value_from_ptr(boing, &module_screen_function_window_title, BOING_EXTERNAL_FUNCTION, NULL));

	BOING_ADD_GLOBAL("SCREEN_CREATE_FONT", boing_value_from_ptr(boing, &module_screen_function_create_font, BOING_EXTERNAL_FUNCTION, NULL));

	BOING_ADD_GLOBAL("SCREEN_CREATE_TEXTURE", boing_value_from_ptr(boing, &module_screen_function_create_texture, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SCREEN_CREATE_TEXTURE_IMAGE", boing_value_from_ptr(boing, &module_screen_function_create_texture_image, BOING_EXTERNAL_FUNCTION, NULL));

	/* BOING_ADD_GLOBAL("SCREEN_RESIZE_TEXTURE", boing_value_from_ptr(boing, &module_screen_function_resize_texture, BOING_EXTERNAL_FUNCTION, NULL)); */

	BOING_ADD_GLOBAL("SCREEN_DRAW_COLOR", boing_value_from_ptr(boing, &module_screen_function_draw_color, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SCREEN_DRAW_LINE", boing_value_from_ptr(boing, &module_screen_function_draw_line, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SCREEN_DRAW_PIXEL", boing_value_from_ptr(boing, &module_screen_function_draw_pixel, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SCREEN_DRAW_RECT", boing_value_from_ptr(boing, &module_screen_function_draw_rect, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SCREEN_DRAW_RECT_LINES", boing_value_from_ptr(boing, &module_screen_function_draw_rect_lines, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SCREEN_DRAW_TEXTURE", boing_value_from_ptr(boing, &module_screen_function_draw_texture, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SCREEN_DRAW_TARGET", boing_value_from_ptr(boing, &module_screen_function_draw_target, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SCREEN_DRAW_STRING", boing_value_from_ptr(boing, &module_screen_function_draw_string, BOING_EXTERNAL_FUNCTION, NULL));

	BOING_ADD_GLOBAL("SCREEN_STRING_DRAWSIZE", boing_value_from_ptr(boing, &module_screen_function_string_drawsize, BOING_EXTERNAL_FUNCTION, NULL));
	
	BOING_ADD_GLOBAL("SCREEN_SET_BLENDING", boing_value_from_ptr(boing, &module_screen_function_set_blending, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SCREEN_SET_FONT_STYLE", boing_value_from_ptr(boing, &module_screen_function_set_font_style, BOING_EXTERNAL_FUNCTION, NULL));
	
	BOING_ADD_GLOBAL("SCREEN_POLL_EVENT", boing_value_from_ptr(boing, &module_screen_function_poll_event, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SCREEN_DELAY", boing_value_from_ptr(boing, &module_screen_function_delay, BOING_EXTERNAL_FUNCTION, NULL));
	
	BOING_ADD_GLOBAL("SCREEN_TICKS", boing_value_from_ptr(boing, &module_screen_function_ticks, BOING_EXTERNAL_FUNCTION, NULL));
	
	BOING_ADD_GLOBAL("SCREEN_MESSAGEBOX", boing_value_from_ptr(boing, &module_screen_function_messagebox, BOING_EXTERNAL_FUNCTION, NULL));
	
	BOING_ADD_GLOBAL("SCREEN_CURSOR", boing_value_from_ptr(boing, &module_screen_function_cursor, BOING_EXTERNAL_FUNCTION, NULL));
	BOING_ADD_GLOBAL("SCREEN_CURSOR_VISIBLE", boing_value_from_ptr(boing, &module_screen_function_cursor_visible, BOING_EXTERNAL_FUNCTION, NULL));

	BOING_ADD_GLOBAL("SCREEN_INTERSECT_RECT", boing_value_from_ptr(boing, &module_screen_function_intersect_rect, BOING_EXTERNAL_FUNCTION, NULL));
	
	
	/* table creator functions */
	BOING_ADD_GLOBAL("SCREEN_RECT", boing_value_from_ptr(boing, &module_screen_function_rect, BOING_EXTERNAL_FUNCTION, NULL));


	/* globals */
	BOING_ADD_GLOBAL("SCREEN_FONT_QUALITY_SOLID", boing_value_from_double(boing, 0.0));
	BOING_ADD_GLOBAL("SCREEN_FONT_QUALITY_SHADED", boing_value_from_double(boing, 1.0));
	BOING_ADD_GLOBAL("SCREEN_FONT_QUALITY_BLENDED", boing_value_from_double(boing, 2.0));

	BOING_ADD_GLOBAL("SCREEN_FONT_STYLE_BOLD", boing_value_from_double(boing, (double)TTF_STYLE_BOLD));
	BOING_ADD_GLOBAL("SCREEN_FONT_STYLE_ITALIC", boing_value_from_double(boing, (double)TTF_STYLE_ITALIC));
	BOING_ADD_GLOBAL("SCREEN_FONT_STYLE_UNDERLINE", boing_value_from_double(boing, (double)TTF_STYLE_UNDERLINE));
	BOING_ADD_GLOBAL("SCREEN_FONT_STYLE_STRIKETHROUGH", boing_value_from_double(boing, (double)TTF_STYLE_STRIKETHROUGH));
	BOING_ADD_GLOBAL("SCREEN_FONT_STYLE_NORMAL", boing_value_from_double(boing, (double)TTF_STYLE_NORMAL));
	
	BOING_ADD_GLOBAL("SCREEN_BLENDMODE_NONE", boing_value_from_double(boing, (double)SDL_BLENDMODE_NONE));
	BOING_ADD_GLOBAL("SCREEN_BLENDMODE_BLEND", boing_value_from_double(boing, (double)SDL_BLENDMODE_BLEND));
	BOING_ADD_GLOBAL("SCREEN_BLENDMODE_ADD", boing_value_from_double(boing, (double)SDL_BLENDMODE_ADD));
	BOING_ADD_GLOBAL("SCREEN_BLENDMODE_MOD", boing_value_from_double(boing, (double)SDL_BLENDMODE_MOD));

	ADD_SDL_ENUM(KEYDOWN);
	ADD_SDL_ENUM(KEYUP);
	ADD_SDL_ENUM(MOUSEBUTTONDOWN);
	ADD_SDL_ENUM(MOUSEBUTTONUP);
	ADD_SDL_ENUM(MOUSEWHEEL);
	ADD_SDL_ENUM(MOUSEMOTION);
	ADD_SDL_ENUM(QUIT);
	ADD_SDL_ENUM(USEREVENT);
	ADD_SDL_ENUM(WINDOWEVENT);

	ADD_SDL_ENUM(MOUSEWHEEL_NORMAL);
	ADD_SDL_ENUM(MOUSEWHEEL_FLIPPED);

	ADD_SDL_ENUM(MESSAGEBOX_ERROR);
	ADD_SDL_ENUM(MESSAGEBOX_WARNING);
	ADD_SDL_ENUM(MESSAGEBOX_INFORMATION);

	ADD_SDL_ENUM(SYSTEM_CURSOR_ARROW);
	ADD_SDL_ENUM(SYSTEM_CURSOR_IBEAM);
	ADD_SDL_ENUM(SYSTEM_CURSOR_WAIT);
	ADD_SDL_ENUM(SYSTEM_CURSOR_CROSSHAIR);
	ADD_SDL_ENUM(SYSTEM_CURSOR_WAITARROW);
	ADD_SDL_ENUM(SYSTEM_CURSOR_SIZENWSE);
	ADD_SDL_ENUM(SYSTEM_CURSOR_SIZENESW);
	ADD_SDL_ENUM(SYSTEM_CURSOR_SIZEWE);
	ADD_SDL_ENUM(SYSTEM_CURSOR_SIZENS);
	ADD_SDL_ENUM(SYSTEM_CURSOR_SIZEALL);
	ADD_SDL_ENUM(SYSTEM_CURSOR_NO);
	ADD_SDL_ENUM(SYSTEM_CURSOR_HAND);

	ADD_SDL_ENUM(FLIP_NONE);
	ADD_SDL_ENUM(FLIP_HORIZONTAL);
	ADD_SDL_ENUM(FLIP_VERTICAL);
	BOING_ADD_GLOBAL("SCREEN_FLIP_ALL", boing_value_from_double(boing, (double)(SDL_FLIP_VERTICAL | SDL_FLIP_HORIZONTAL)));
	
	#include "screen_keys.h" /* use these to compare against the keycode in keyup/down events */
	

	return 0;
}