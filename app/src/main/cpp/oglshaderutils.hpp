//
// Created by CAMOBAP on 2019-05-05.
//

#pragma once

#include <GLES3/gl3.h>

/**
 *
 * @param name
 * @param s
 */
void printGLString(const char *name, GLenum s);

/**
 *
 * @param op
 */
void checkGlError(const char* op);

/**
 *
 * @param shaderType
 * @param pSource
 * @return
 */
GLuint loadShader(GLenum shaderType, const char* pSource);

/*
 *
 */
GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);