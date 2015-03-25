// --------------------------------------------------------------------------------
/*
    utilities.h

  Copyright (c) 2009-2015  Thomas Mueller, Frank Grave


   This file is part of the GeodesicViewer.

   The GeodesicViewer is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   The GeodesicViewer is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the GeodesicViewer.  If not, see <http://www.gnu.org/licenses/>.
*/

/*!  \file   utilities.h
     \brief  Utility functions.

*/
// --------------------------------------------------------------------------------

#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

#include "myobject.h"

#ifdef HAVE_LUA
#include "lua/m4dlua.h"
#include "lua/m4dlua_utils.h"
void  lua_reg_gvutils(lua_State *L);
#endif // HAVE_LUA


bool tokenizeFile(const std::string filename, std::vector<std::vector<std::string> > &tokens);
int  readObjectFile(const std::string filename, std::vector<MyObject*>  &objects, bool clear = true);
int  readObjectsFromTokens(std::vector<std::vector<std::string> > &tokens, std::vector<MyObject*>  &objects);

void setStandardParams(struct_params* par);
bool loadParamFile(const std::string filename, struct_params* par);
bool saveParamFile(const std::string filename, struct_params* par);

#endif
