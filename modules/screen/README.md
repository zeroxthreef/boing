# Screen
This module lets you call a bunch of SDL and SDL2_ttf functions.

## Dependencies
\* SDL2 (and various other dependencies that SDL needs)
\* SDL2_ttf
\* freetype

Uses the wonderful [stb_image](https://github.com/nothings/stb)

## Documentation
Note: this entire project is not at version 1.0.0 yet. Things will change with this module.

Non-Function Identifiers
---
Note, lots of these come from the SDL2 enumerations. Look at the [SDL2 documentation](https://wiki.libsdl.org/CategoryAPI) and the values will be the same. The only difference is there is a SCREEN prefix instead of SDL.

Note#2, _lots_ of identifiers are omitted. Listing them all here would make this list incredibly long.

| Name | Value | Description |
| --- | --- | --- |
| `SCREEN_FONT_QUALITY_SOLID` | 0.0 |  |
| `SCREEN_FONT_QUALITY_SHADED` | 1.0 |  |
| `SCREEN_FONT_QUALITY_BLENDED` | 2.0 |  |
| `SCREEN_FONT_STYLE_BOLD` | TTF_STYLE_BOLD |  |
| `SCREEN_FONT_STYLE_ITALIC` | TTF_STYLE_ITALIC |  |
| `SCREEN_FONT_STYLE_UNDERLINE` | TTF_STYLE_UNDERLINE |  |
| `SCREEN_FONT_STYLE_STRIKETHROUGH` | TTF_STYLE_STRIKETHROUGH |  |
| `SCREEN_FONT_STYLE_NORMAL` | TTF_STYLE_NORMAL |  |
| `SCREEN_BLENDMODE_NONE` | SDL_BLENDMODE_NONE |  |
| `SCREEN_BLENDMODE_BLEND` | SDL_BLENDMODE_BLEND |  |
| `SCREEN_BLENDMODE_ADD` | SDL_BLENDMODE_ADD |  |
| `SCREEN_BLENDMODE_MOD` | SDL_BLENDMODE_MOD |  |
| `SCREEN_KEYDOWN` | SDL_KEYDOWN | this is an event type |
| `SCREEN_KEYUP` | SDL_KEYUP | this is an event type |
| `SCREEN_MOUSEBUTTONDOWN` | SDL_MOUSEBUTTONDOWN | this is an event type |
| `SCREEN_MOUSEBUTTONUP` | SDL_MOUSEBUTTONUP | this is an event type |
| `SCREEN_MOUSEWHEEL` | SDL_MOUSEWHEEL | this is an event type |
| `SCREEN_MOUSEMOTION` | SDL_MOUSEMOTION | this is an event type |
| `SCREEN_QUIT` | SDL_QUIT | this is an event type |
| `SCREEN_USEREVENT` | SDL_USEREVENT | this is an event type |
| `SCREEN_WINDOWEVENT` | SDL_WINDOWEVENT | this is an event type |
| `SCREEN_MOUSEWHEEL_NORMAL` | SDL_MOUSEWHEEL_NORMAL |  |
| `SCREEN_MOUSEWHEEL_FLIPPED` | SDL_MOUSEWHEEL_FLIPPED |  |
| `SCREEN_MESSAGEBOX_ERROR` | SDL_MESSAGEBOX_ERROR |  |
| `SCREEN_MESSAGEBOX_WARNING` | SDL_MESSAGEBOX_WARNING |  |
| `SCREEN_MESSAGEBOX_INFORMATION` | SDL_MESSAGEBOX_INFORMATION |  |
| `SCREEN_SYSTEM_CURSOR_ARROW` | SDL_SYSTEM_CURSOR_ARROW |  |
| `SCREEN_SYSTEM_CURSOR_IBEAM` | SDL_SYSTEM_CURSOR_IBEAM |  |
| `SCREEN_SYSTEM_CURSOR_WAIT` | SDL_SYSTEM_CURSOR_WAIT |  |
| `SCREEN_SYSTEM_CURSOR_CROSSHAIR` | SDL_SYSTEM_CURSOR_CROSSHAIR |  |
| `SCREEN_SYSTEM_CURSOR_WAITARROW` | SDL_SYSTEM_CURSOR_WAITARROW |  |
| `SCREEN_SYSTEM_CURSOR_SIZENWSE` | SDL_SYSTEM_CURSOR_SIZENWSE |  |
| `SCREEN_SYSTEM_CURSOR_SIZENESW` | SDL_SYSTEM_CURSOR_SIZENESW |  |
| `SCREEN_SYSTEM_CURSOR_SIZEWE` | SDL_SYSTEM_CURSOR_SIZEWE |  |
| `SCREEN_SYSTEM_CURSOR_SIZENS` | SDL_SYSTEM_CURSOR_SIZENS |  |
| `SCREEN_SYSTEM_CURSOR_SIZEALL` | SDL_SYSTEM_CURSOR_SIZEALL |  |
| `SCREEN_SYSTEM_CURSOR_NO` | SDL_SYSTEM_CURSOR_NO |  |
| `SCREEN_SYSTEM_CURSOR_HAND` | SDL_SYSTEM_CURSOR_HAND |  |
| `SCREEN_FLIP_NONE` | SDL_FLIP_NONE |  |
| `SCREEN_FLIP_HORIZONTAL` | SDL_FLIP_HORIZONTAL |  |
| `SCREEN_FLIP_VERTICAL` | SDL_FLIP_VERTICAL |  |
| `SCREEN_SCREEN_FLIP_ALL` | (SDL_FLIP_HORIZONTAL \| SDL_FLIP_VERTICAL) |  |
| `SCREEN_KEY_`... | SDLK_... | look at the [SDL2 keycode docs](https://wiki.libsdl.org/SDL_Keycode) or screen_keys.h |

Function Identifiers
---
In most cases, a numeric 0 means success.

| Name | Value | Attributes |
| --- | --- | --- |
| `SCREEN_CREATE_WINDOW` | external value (C function pointer) | returns an external value for the window struct or fails on error, format: (array(string) title, number width, number height, number is_resizable) |
| `SCREEN_WINDOW_CLEAR` | external value (C function pointer) | returns a number, format: (external(window) window) |
| `SCREEN_WINDOW_PRESENT` | external value (C function pointer) | returns a number, format: (external(window) window) |
| `SCREEN_WINDOW_TITLE` | external value (C function pointer) | returns a number, format: (external(window) window, array(string) title) |
| `SCREEN_CREATE_FONT` | external value (C function pointer) | returns an external value(font), format: (array ttf_font_data, number point, number quality) |
| `SCREEN_CREATE_TEXTURE` | external value (C function pointer) | returns an external value(texture), format: (external(window) window, number width, number height, number type) |
| `SCREEN_CREATE_TEXTURE_IMAGE` | external value (C function pointer) | returns an external value(texture), format: (external(window) window, array image_data, number type) |
| `SCREEN_DRAW_COLOR` | external value (C function pointer) | returns a number, format: (external(window), number R, number G, number B, number A) |
| `SCREEN_DRAW_LINE` | external value (C function pointer) | returns a number, format: (external(window) window, number x1, number y1, number x2, number y2) |
| `SCREEN_DRAW_PIXEL` | external value (C function pointer) | returns a number, format: (external(window) window, number x, number y) |
| `SCREEN_DRAW_RECT` | external value (C function pointer) | returns a number, format: (external(window) window, array(table) rect) |
| `SCREEN_DRAW_RECT_LINES` | external value (C function pointer) | returns a number, format: (external(window) window, array(table) rect) |
| `SCREEN_DRAW_TEXTURE` | external value (C function pointer) | returns a number, format: (external(window) window, external(texture) texture, array(table)\|number src_rect, array(table)\|number dest_rect, void\|number angle, void\|array center_coordinate, void\|number flip_type) ``note that this function can take void arguments so the arguments past the dest_rect are optional. Also, all arguments must be present if just one past dest_rect is present and passing an empty array to center will automatically make the center coordinates the center of the dest_rect`` |
| `SCREEN_DRAW_TARGET` | external value (C function pointer) | returns a number, format: (external(window) window, external(texture)|number target) ``note that passing 0 to the target will make the window the draw taget again`` |
| `SCREEN_DRAW_STRING` | external value (C function pointer) | returns a number, format: (external(window) window, external(font) font, array(string) message, array(number) foreground_color, array(number) background_color, number x, number y) ``note that the color arrays can be empty (will use [255 255 255 255] instead) but they normally expect [R G B A]`` |
| `SCREEN_STRING_DRAWSIZE` | external value (C function pointer) | returns an array(table) for the rect, format: (external(font) font, array(string) message) |
| `SCREEN_SET_BLENDING` | external value (C function pointer) | returns a number, format: (external(window) window, number blendmode) |
| `SCREEN_SET_FONT_STYLE` | external value (C function pointer) | returns a number, format: (external(font) font, number attribute) |
| `SCREEN_POLL_EVENT` | external value (C function pointer) | returns a number for how many events are in the queue, format: (any event_set) ``note that arg0 is set to the event table, not returned`` |
| `SCREEN_DELAY` | external value (C function pointer) | returns a number, format: (number milliseconds) |
| `SCREEN_TICKS` | external value (C function pointer) | returns a number, format: (void) |
| `SCREEN_MESSAGEBOX` | external value (C function pointer) | returns a number, format: (array(string) title, array(string) message, number type) |
| `SCREEN_CURSOR` | external value (C function pointer) | returns a number, format: (number cursor_type) |
| `SCREEN_CURSOR_VISIBLE` | external value (C function pointer) | returns a number, format: (number is_visible) |
| `SCREEN_INTERSECT_RECT` | external value (C function pointer) | returns a number value of 1 if it intersected a and b or 0 if no intersection, format: (array(table) rect_a, array(table) rect_b, any intersect_rect_set) ``note that arg2 is set to the the intersect rect table`` |
| `SCREEN_RECT` | external value (C function pointer) | returns an array(table) or fails on error, format: (number x, number y, number w, number h) |

## TODO
\* better identifier names
\* get more font attributes