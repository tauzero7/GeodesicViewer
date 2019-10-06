/**
 * @file    utilities.h
 * @author  Thomas Mueller
 *
 * @brief  Utility functions.
 *
 * This file is part of GeodesicView.
 */
#ifndef UTILITIES_H
#define UTILITIES_H

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "myobject.h"

#ifdef HAVE_LUA
#include "lua/m4dlua.h"
#include "lua/m4dlua_utils.h"
void lua_reg_gvutils(lua_State* L);
#endif // HAVE_LUA

bool CopyString(const char* src, char*& dest);

bool GetExePath(char*& path);
bool GetFilePath(const char* filename, char*& path);

/**
 * @brief Split file into string tokens.
 * @param filename : name of file
 * @param tokens : reference of tokens build from the file
 * @return true : success
 */
bool tokenizeFile(const std::string& filename, std::vector<std::vector<std::string>>& tokens);

/*! Read object file.
 * @param filename : name of file
 * @param objects : reference to object list
 * @param clear : if true then clear object list
 * @return number of objects
 */
int readObjectFile(const std::string& filename, std::vector<MyObject*>& objects, bool clear = true);

/*! Read objects from tokens.
 * @param tokens : reference to token list
 * @param objects : reference to object list
 * @return number of objects
 */
int readObjectsFromTokens(std::vector<std::vector<std::string>>& tokens, std::vector<MyObject*>& objects);

void setStandardParams(struct_params* par);

/*! Load cfg-parameter file.
 * @param filename : file name.
 * @param par      : pointer to parameter structure.
 * @return true : successfull.
 */
bool loadParamFile(const std::string& filename, struct_params* par);

/*! Save cfg-parameter file.
 * @param filename : file name.
 * @param par      : pointer to parameter structure.
 * @return true : successfull.
 */
bool saveParamFile(const std::string& filename, struct_params* par);

/**
 * @brief Safely delete 1D arrays generated with 'new'.
 *
 *  Each pointer to a 1D array should be immediately
 *  initialized or set to nullptr. This method checks
 *  if pointer is not 'nullptr', deletes the array,
 *  and sets the pointer to 'nullptr'.
 *
 * @tparam T  Pointer to data array.
 */
template <typename T> void SafeDelete(T*& ptr)
{
    if (ptr != nullptr) {
        delete[] ptr;
        ptr = nullptr;
    }
}

#endif // UTILITIES_H
