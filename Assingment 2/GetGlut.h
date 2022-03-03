//
//  GetGlut.h
//
//  A header file in import OpenGL, GLU, and GLUT on Windows,
//    Mac OSX, Linux, or Solaris.  The intermediate file is
//    helpful because differant includesd are needed on
//    differant platforms.
//
//  On Windows you also get <string> because the GLUT library
//    may not compile unless a standard library (not including
//    <cassert>) is included first.
//

#ifndef GET_GLUT_H
#define GET_GLUT_H


// Unix
#ifdef unix
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#elif __unix
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#elif __unix__
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Linux
#elif linux
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#elif __linux
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#elif __linux__
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Mac OSX
#elif __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#elif __MACH__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

// Solaris
#elif sun
OpenGL Includes for solaris
#elif __sun
OpenGL Includes for solaris

// FreeBSD
#elif __FreeBSD__
OpenGL Includes for FreeBSD

// Windows
#elif _WIN32	// include 64-bit enviroments
#include "freeglut.h"	// http://freeglut.sourceforge.net/index.php
#elif __WIN32__
#include "freeglut.h"
#endif



#endif
