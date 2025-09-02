#pragma once

#include "SDL3/SDL.h"
#ifdef _WIN32
#define APIENTRY __stdcall
#endif
#define SDL_OPENGL_1_NO_PROTOTYPES
#define SDL_OPENGL_1_FUNCTION_TYPEDEFS
#include "SDL3/SDL_opengl.h"
#ifdef _WIN32
#undef APIENTRY
#endif

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_opengl2.h"
