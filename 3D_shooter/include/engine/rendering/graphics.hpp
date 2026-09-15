///============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  Brian Russin
//	File:	 graphics.hpp
//	Purpose: Central location to include OpenGL files
//============================================================================

#ifndef __SCENE_GRAPHICS_HPP__
#define __SCENE_GRAPHICS_HPP__

#if defined(BUILD_MACOS)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#elif defined(BUILD_LINUX)
#include <GL/gl.h>
#include <GL/glext.h>
#elif defined(BUILD_WINDOWS)
#include <GL/glew.h>
#include <windows.h>
#endif

#include <SDL3/SDL.h>

#endif
