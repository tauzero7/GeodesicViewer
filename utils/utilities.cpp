/**
 * @file    utilities.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "utilities.h"

#ifndef _WIN32
#include <unistd.h>
#endif // _WIN32

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

bool CopyString(const char* src, char*& dest)
{
    if (src == nullptr) {
        return false;
    }

    if (dest != nullptr) {
        delete[] dest;
    }
    size_t len = strlen(src);

    bool isOkay = true;
#ifdef _WIN32
    dest = new char[len + 4];
    isOkay &= (strncpy_s(dest, len + 4, src, len) == nullptr);
#else
    dest = new char[len + 2];
    isOkay &= (strcpy(dest, src) != nullptr);
#endif
    return isOkay;
}

bool GetExePath(char*& path)
{
    if (path != nullptr) {
        delete[] path;
    }

#ifdef _WIN32
    const unsigned int nSize = MAX_PATH;
#else
    const unsigned int nSize = 0xFFFF;
#endif

    char* cwd = new char[nSize];
#ifdef _WIN32
    if (::GetModuleFileNameA(nullptr, cwd, nSize) == ERROR_INSUFFICIENT_BUFFER) {
        cwd[0] = 0;
    }
    else {
        if (::GetLastError() != ERROR_SUCCESS) {
            cwd[0] = 0;
        }
    }
#elif defined(__APPLE__)
    char apath[1024];
    uint32_t asize = sizeof(apath);
    if (_NSGetExecutablePath(apath, &asize) == 0) {
        realpath(apath, cwd);
    }
    else {
        cwd[0] = 0;
    }
#else
    char* tmp = new char[nSize];
    sprintf(tmp, "/proc/%d/exe", getpid());
    ssize_t size = readlink(tmp, cwd, nSize - 1);
    if (size >= 0) {
        cwd[size] = 0;
    }
    else {
        cwd[0] = 0;
    }
    delete[] tmp;
#endif

    size_t cwdlen = static_cast<size_t>(strlen(cwd)) + 1;
    path = new char[cwdlen];

#ifdef _WIN32
    strcpy_s(path, cwdlen, cwd);
#else
    strcpy(path, cwd);
#endif

    // remove exe name
    GetFilePath(cwd, path);

    bool isOkay = true;
    if (cwd[0] == 0) {
        fprintf(stderr, "Cannot determine current working directory!\n");
        isOkay = false;
    }
    delete[] cwd;
    return isOkay;
}

bool GetFilePath(const char* filename, char*& path)
{
    std::string fname = std::string(filename);
    std::string mpath = std::string();

#ifdef _WIN32
    size_t offset = fname.rfind("\\");
    if (offset != std::string::npos) {
        mpath = fname.substr(0, offset) + "\\";
    }
    else {
        offset = fname.rfind("/");
        if (offset != std::string::npos) {
            mpath = fname.substr(0, offset) + "/";
        }
    }
#else
    size_t offset = fname.rfind("/");
    if (offset == std::string::npos) {
        mpath = std::string("");
    }
    else {
        mpath = fname.substr(0, offset) + "/";
    }
#endif

    CopyString(mpath.c_str(), path);
    return true;
}

bool tokenizeFile(const std::string& filename, std::vector<std::vector<std::string>>& tokens)
{
    std::ifstream in(filename.c_str());

    if (!in) {
        fprintf(stderr, "Cannot open file \"%s\"\n", filename.c_str());
        return false;
    }

    do {
        std::string line;
        getline(in, line);

        bool ignoreLine = false;
        if (!line.compare(0, 1, "")) {
            ignoreLine = true;
        }

        if (!line.compare(0, 1, "#")) {
            ignoreLine = true;
        }

        if (!ignoreLine) {
            std::string buf;
            std::stringstream ss(line);

            std::vector<std::string> line_tokens;

            while (ss >> buf) {
                line_tokens.push_back(buf);
            }

            tokens.push_back(line_tokens);
        }
    } while (!in.eof());

    in.close();
    return true;
}

int readObjectFile(const std::string& filename, std::vector<MyObject*>& objects, bool clear)
{
    if (clear && !objects.empty()) {
        objects.clear();
    }

    std::vector<std::vector<std::string>> tokens;
    if (!tokenizeFile(filename, tokens)) {
        return 0;
    }

    return readObjectsFromTokens(tokens, objects);
}

int readObjectsFromTokens(std::vector<std::vector<std::string>>& tokens, std::vector<MyObject*>& objects)
{
    MyObject* obj;
    for (unsigned int i = 0; i < tokens.size(); i++) {
        // std::cerr << tokens[i][0] << std::endl;
        // ------------------------------
        //     text2d
        // ------------------------------
        if (tokens[i][0] == stl_object_type[enum_object_text2d].toStdString() && tokens[i].size() > 4) {
            obj = new MyObject();
            if (tokens[i].size() > 7) {
                obj->setColor(atof(tokens[i][5].c_str()), atof(tokens[i][6].c_str()), atof(tokens[i][7].c_str()));
            }

            obj->create_2d_text(
                atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), tokens[i][3], atof(tokens[i][4].c_str()));
            objects.push_back(obj);
        }
        // ------------------------------
        //     text3d
        // ------------------------------
        else if (tokens[i][0] == stl_object_type[enum_object_text3d].toStdString() && tokens[i].size() > 5) {
            obj = new MyObject();
            if (tokens[i].size() > 8) {
                obj->setColor(atof(tokens[i][6].c_str()), atof(tokens[i][7].c_str()), atof(tokens[i][8].c_str()));
            }

            obj->create_3d_text(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()),
                tokens[i][4], atof(tokens[i][5].c_str()));
            objects.push_back(obj);
        }
        // ------------------------------
        //     sphere2d
        // ------------------------------
        else if (tokens[i][0] == stl_object_type[enum_object_sphere2d].toStdString() && tokens[i].size() > 5) {
            obj = new MyObject();
            if (tokens[i].size() > 8) {
                obj->setColor(atof(tokens[i][6].c_str()), atof(tokens[i][7].c_str()), atof(tokens[i][8].c_str()));
            }

            obj->create_2d_sphere(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()),
                atof(tokens[i][4].c_str()), atof(tokens[i][5].c_str()));
            objects.push_back(obj);
        }
        // ------------------------------
        //     sphere3d
        // ------------------------------
        else if (tokens[i][0] == stl_object_type[enum_object_sphere3d].toStdString() && tokens[i].size() > 6) {
            obj = new MyObject();
            if (tokens[i].size() > 10) {
                obj->setColor(atof(tokens[i][7].c_str()), atof(tokens[i][8].c_str()), atof(tokens[i][9].c_str()),
                    atof(tokens[i][10].c_str()));
            }
            else if (tokens[i].size() > 9) {
                obj->setColor(atof(tokens[i][7].c_str()), atof(tokens[i][8].c_str()), atof(tokens[i][9].c_str()));
            }

            obj->create_3d_sphere(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()),
                atof(tokens[i][4].c_str()), atof(tokens[i][5].c_str()), atof(tokens[i][6].c_str()));
            objects.push_back(obj);
        }
        // ------------------------------
        //     box2d
        // ------------------------------
        else if (tokens[i][0] == stl_object_type[enum_object_box2d].toStdString() && tokens[i].size() > 5) {
            obj = new MyObject();
            if (tokens[i].size() > 8) {
                obj->setColor(atof(tokens[i][6].c_str()), atof(tokens[i][7].c_str()), atof(tokens[i][8].c_str()));
            }

            obj->create_2d_box(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()),
                atof(tokens[i][4].c_str()), atof(tokens[i][5].c_str()));
            objects.push_back(obj);
        }
        // ------------------------------
        //     box3d
        // ------------------------------
        else if (tokens[i][0] == stl_object_type[enum_object_box3d].toStdString() && tokens[i].size() > 7) {
            obj = new MyObject();
            if (tokens[i].size() > 10) {
                obj->setColor(atof(tokens[i][8].c_str()), atof(tokens[i][9].c_str()), atof(tokens[i][10].c_str()));
            }

            obj->create_3d_box(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()),
                atof(tokens[i][4].c_str()), atof(tokens[i][5].c_str()), atof(tokens[i][6].c_str()),
                atof(tokens[i][7].c_str()));
            objects.push_back(obj);
        }
        // ------------------------------
        //     line2d
        // ------------------------------
        else if (tokens[i][0] == stl_object_type[enum_object_line2d].toStdString() && tokens[i].size() > 5) {
            obj = new MyObject();
            if (tokens[i].size() > 8) {
                obj->setColor(atof(tokens[i][6].c_str()), atof(tokens[i][7].c_str()), atof(tokens[i][8].c_str()));
            }

            obj->create_2d_line(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()),
                atof(tokens[i][4].c_str()), atof(tokens[i][5].c_str()));
            objects.push_back(obj);
        }
        // ------------------------------
        //     line3d
        // ------------------------------
        else if (tokens[i][0] == stl_object_type[enum_object_line3d].toStdString() && tokens[i].size() > 7) {
            obj = new MyObject();
            if (tokens[i].size() > 10) {
                obj->setColor(atof(tokens[i][8].c_str()), atof(tokens[i][9].c_str()), atof(tokens[i][10].c_str()));
            }

            obj->create_3d_line(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()),
                atof(tokens[i][4].c_str()), atof(tokens[i][5].c_str()), atof(tokens[i][6].c_str()),
                atof(tokens[i][7].c_str()));
            objects.push_back(obj);
        }
        // ------------------------------
        //     quad2d
        // ------------------------------
        else if (tokens[i][0] == stl_object_type[enum_object_quad2d].toStdString() && tokens[i].size() > 9) {
            obj = new MyObject();
            if (tokens[i].size() > 13) {
                obj->setColor(atof(tokens[i][10].c_str()), atof(tokens[i][11].c_str()), atof(tokens[i][12].c_str()),
                    atof(tokens[i][13].c_str()));
            }
            else if (tokens[i].size() > 12) {
                obj->setColor(atof(tokens[i][10].c_str()), atof(tokens[i][11].c_str()), atof(tokens[i][12].c_str()));
            }

            obj->create_2d_quad(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()),
                atof(tokens[i][4].c_str()), atof(tokens[i][5].c_str()), atof(tokens[i][6].c_str()),
                atof(tokens[i][7].c_str()), atof(tokens[i][8].c_str()), atof(tokens[i][9].c_str()));
            objects.push_back(obj);
        }
        // ------------------------------
        //     plane3d
        // ------------------------------
        else if (tokens[i][0] == stl_object_type[enum_object_plane3d].toStdString() && tokens[i].size() > 9) {
            obj = new MyObject();
            if (tokens[i].size() > 13) {
                obj->setColor(atof(tokens[i][10].c_str()), atof(tokens[i][11].c_str()), atof(tokens[i][12].c_str()),
                    atof(tokens[i][13].c_str()));
            }
            else if (tokens[i].size() > 12) {
                obj->setColor(atof(tokens[i][10].c_str()), atof(tokens[i][11].c_str()), atof(tokens[i][12].c_str()));
            }

            obj->create_3d_plane(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()),
                atof(tokens[i][4].c_str()), atof(tokens[i][5].c_str()), atof(tokens[i][6].c_str()),
                atof(tokens[i][7].c_str()), atof(tokens[i][8].c_str()), atof(tokens[i][9].c_str()));
            objects.push_back(obj);
        }
        // ------------------------------
        //     disk2d
        // ------------------------------
        else if (tokens[i][0] == stl_object_type[enum_object_disk2d].toStdString() && tokens[i].size() > 4) {
            obj = new MyObject();
            if (tokens[i].size() > 7) {
                obj->setColor(atof(tokens[i][5].c_str()), atof(tokens[i][6].c_str()), atof(tokens[i][7].c_str()));
            }

            obj->create_2d_disk(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()),
                atof(tokens[i][4].c_str()));
            objects.push_back(obj);
        }
        // ------------------------------
        //     disk3d
        // ------------------------------
        else if (tokens[i][0] == stl_object_type[enum_object_disk3d].toStdString() && tokens[i].size() > 9) {
            obj = new MyObject();
            if (tokens[i].size() > 14) {
                obj->setColor(atof(tokens[i][11].c_str()), atof(tokens[i][12].c_str()), atof(tokens[i][13].c_str()),
                    atof(tokens[i][14].c_str()));
            }
            else if (tokens[i].size() > 13) {
                obj->setColor(atof(tokens[i][11].c_str()), atof(tokens[i][12].c_str()), atof(tokens[i][13].c_str()));
            }

            obj->create_3d_disk(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()),
                atof(tokens[i][4].c_str()), atof(tokens[i][5].c_str()), atof(tokens[i][6].c_str()),
                atof(tokens[i][7].c_str()), atof(tokens[i][8].c_str()), atof(tokens[i][9].c_str()),
                atof(tokens[i][10].c_str()));
            objects.push_back(obj);
        }
        // ------------------------------
        //     cylinder3d
        // ------------------------------
        else if (tokens[i][0] == stl_object_type[enum_object_cylinder3d].toStdString() && tokens[i].size() > 10) {
            obj = new MyObject();
            if (tokens[i].size() > 14) {
                obj->setColor(atof(tokens[i][11].c_str()), atof(tokens[i][12].c_str()), atof(tokens[i][13].c_str()),
                    atof(tokens[i][14].c_str()));
            }
            else if (tokens[i].size() > 13) {
                obj->setColor(atof(tokens[i][11].c_str()), atof(tokens[i][12].c_str()), atof(tokens[i][13].c_str()));
            }

            obj->create_3d_cylinder(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()),
                atof(tokens[i][4].c_str()), atof(tokens[i][5].c_str()), atof(tokens[i][6].c_str()),
                atof(tokens[i][7].c_str()), atof(tokens[i][8].c_str()), atof(tokens[i][9].c_str()),
                atof(tokens[i][10].c_str()));
            objects.push_back(obj);
        }
        // ------------------------------
        //     torus3d
        // ------------------------------
        else if (tokens[i][0] == stl_object_type[enum_object_torus3d].toStdString() && tokens[i].size() > 10) {
            obj = new MyObject();
            if (tokens[i].size() > 14) {
                obj->setColor(atof(tokens[i][11].c_str()), atof(tokens[i][12].c_str()), atof(tokens[i][13].c_str()),
                    atof(tokens[i][14].c_str()));
            }
            else if (tokens[i].size() > 13) {
                obj->setColor(atof(tokens[i][11].c_str()), atof(tokens[i][12].c_str()), atof(tokens[i][13].c_str()));
            }

            obj->create_3d_torus(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()),
                atof(tokens[i][4].c_str()), atof(tokens[i][5].c_str()), atof(tokens[i][6].c_str()),
                atof(tokens[i][7].c_str()), atof(tokens[i][8].c_str()), atof(tokens[i][9].c_str()),
                atof(tokens[i][10].c_str()));
            objects.push_back(obj);
        }
        // ------------------------------
        //     tube3d
        // ------------------------------
        else if (tokens[i][0] == stl_object_type[enum_object_tube3d].toStdString() && tokens[i].size() > 14) {
            obj = new MyObject();
            if (tokens[i].size() > 18) {
                obj->setColor(atof(tokens[i][15].c_str()), atof(tokens[i][16].c_str()), atof(tokens[i][17].c_str()),
                    atof(tokens[i][17].c_str()));
            }
            else if (tokens[i].size() > 17) {
                obj->setColor(atof(tokens[i][15].c_str()), atof(tokens[i][16].c_str()), atof(tokens[i][17].c_str()));
            }

            obj->create_3d_tube(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()),
                atof(tokens[i][4].c_str()), atof(tokens[i][5].c_str()), atof(tokens[i][6].c_str()),
                atof(tokens[i][7].c_str()), atof(tokens[i][8].c_str()), atof(tokens[i][9].c_str()),
                atof(tokens[i][10].c_str()), atof(tokens[i][11].c_str()), atof(tokens[i][12].c_str()),
                atof(tokens[i][13].c_str()), atof(tokens[i][14].c_str()));
            objects.push_back(obj);
        }
    }
    return (int)objects.size();
}

void setStandardParams(struct_params* par)
{
    par->filename = std::string();

    par->axis_orientation = 0;

    par->geod_chi_init = DEF_INIT_CHI;
    par->geod_chi_step = DEF_INIT_CHI_STEP;
    par->geod_ksi_init = DEF_INIT_KSI;
    par->geod_ksi_step = DEF_INIT_KSI_STEP;
    par->geod_vel_init = DEF_INIT_VEL;
    par->geod_vel_step = DEF_INIT_VEL_STEP;

    par->opengl_projection = 0;
    par->opengl_draw3d_type = m4d::enum_draw_pseudocart;

    if (!par->opengl_emb_params.empty()) {
        par->opengl_emb_params.clear();
    }

    par->opengl_eye_pos = m4d::vec3(DEF_INIT_EYE_POS_X, DEF_INIT_EYE_POS_Y, DEF_INIT_EYE_POS_Z);
    par->opengl_eye_pos_step = 0.1;
    par->opengl_eye_poi = m4d::vec3(DEF_INIT_POI_X, DEF_INIT_POI_Y, DEF_INIT_POI_Z);
    par->opengl_eye_poi_step = 0.1;
    par->opengl_eye_vup = m4d::vec3(DEF_INIT_VUP_X, DEF_INIT_VUP_Y, DEF_INIT_VUP_Z);
    par->opengl_eye_dir = par->opengl_eye_poi - par->opengl_eye_pos;
    par->opengl_eye_dir.normalize();
    par->opengl_fov = DEF_INIT_FOV;
    par->opengl_fov_step = 0.1;

    par->opengl_stereo_use = 0;
    par->opengl_stereo_glasses = enum_stereo_red_cyan;
    par->opengl_stereo_type = static_cast<enum_stereo_type>(DEF_STEREO_TYPE);
    par->opengl_stereo_sep = DEF_STEREO_SEP;
    par->opengl_stereo_step = DEF_STEREO_STEP;

    par->opengl_fog_use = 0;
    par->opengl_fog_init = DEF_OPENGL_FOG_DENSITY_INIT;
    par->opengl_fog_step = DEF_OPENGL_FOG_STEP;

    par->opengl_scale_x = 1.0;
    par->opengl_scale_y = 1.0;
    par->opengl_scale_z = 1.0;

    par->opengl_anim_local = 0;
    par->opengl_anim_rot_x = DEF_OPENGL_ANIM_ROT_X_INIT;
    par->opengl_anim_rot_y = DEF_OPENGL_ANIM_ROT_Y_INIT;
    par->opengl_anim_rot_z = DEF_OPENGL_ANIM_ROT_Z_INIT;

    par->opengl_line_color = QColor(DEF_OPENGL_LINE_COLOR);
    par->opengl_line_width = DEF_OPENGL_LINE_WIDTH;
    par->opengl_bg_color = QColor(DEF_OPENGL_BG_COLOR);
    par->opengl_line_smooth = 0;

    par->opengl_leg1_col1 = QColor(DEF_OPENGL_LEG1_COL1);
    par->opengl_leg1_col2 = QColor(DEF_OPENGL_LEG1_COL2);
    par->opengl_leg1_freq = DEF_OPENGL_LEG1_FREQ;
    par->opengl_leg2_col1 = QColor(DEF_OPENGL_LEG2_COL1);
    par->opengl_leg2_col2 = QColor(DEF_OPENGL_LEG2_COL2);
    par->opengl_leg2_freq = DEF_OPENGL_LEG2_FREQ;

    par->opengl_emb_color = QColor(DEF_OPENGL_EMB_COLOR);
    par->opengl_sachs_system = enum_sachs_e1e2e3;
    par->opengl_sachs_legs = enum_sachs_legs_right_up;
    par->opengl_sachs_scale = 0.3;

    double aspect = (DEF_OPENGL_HEIGHT - DEF_DRAW2D_BOTTOM_BORDER)
        / static_cast<double>(DEF_OPENGL_WIDTH - DEF_DRAW2D_LEFT_BORDER);
    par->draw2d_xMin = DEF_DRAW2D_X_INIT_MIN;
    par->draw2d_xMax = DEF_DRAW2D_X_INIT_MAX;
    par->draw2d_yMin = -(DEF_DRAW2D_X_INIT_MAX - DEF_DRAW2D_X_INIT_MIN) * 0.5 * aspect;
    par->draw2d_yMax = (DEF_DRAW2D_X_INIT_MAX - DEF_DRAW2D_X_INIT_MIN) * 0.5 * aspect;

    par->draw2d_bg_color = QColor(DEF_DRAW2D_BG_COLOR);
    par->draw2d_line_color = QColor(DEF_DRAW2D_LINE_COLOR);
    par->draw2d_line_width = DEF_OPENGL_LINE_WIDTH;
    par->draw2d_line_smooth = 0;

    par->draw2d_grid_color = QColor(DEF_DRAW2D_GRID_COLOR);

    par->draw2d_representation = static_cast<m4d::enum_draw_type>(0);
    par->draw2d_abscissa = 0;
    par->draw2d_ordinate = 0;
}

bool loadParamFile(const std::string& filename, struct_params* par)
{
    std::vector<std::vector<std::string>> tokens;
    if (!tokenizeFile(filename, tokens)) {
        return false;
    }

    if (tokens.size() == 0) {
        return false;
    }

    if (!par->opengl_emb_params.empty()) {
        par->opengl_emb_params.clear();
    }

    for (unsigned int i = 0; i < tokens.size(); i++) {
        if (tokens[i].size() == 0) {
            continue;
        }

        std::string baseString = tokens[i][0];
        if (baseString.compare("OGL_PROJECTION") == 0 && tokens[i].size() > 1) {
            par->opengl_projection = atoi(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_DRAW3D_TYPE") == 0 && tokens[i].size() > 1) {
            par->opengl_draw3d_type = (m4d::enum_draw_type)atoi(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_EYE_POS") == 0 && tokens[i].size() > 3) {
            par->opengl_eye_pos
                = m4d::vec3(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()));
        }
        else if (baseString.compare("OGL_EYE_POS_STEP") == 0 && tokens[i].size() > 1) {
            par->opengl_eye_pos_step = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_EYE_POI") == 0 && tokens[i].size() > 3) {
            par->opengl_eye_poi
                = m4d::vec3(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()));
        }
        else if (baseString.compare("OGL_EYE_POI_STEP") == 0 && tokens[i].size() > 1) {
            par->opengl_eye_poi_step = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_EYE_VUP") == 0 && tokens[i].size() > 3) {
            par->opengl_eye_vup
                = m4d::vec3(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()));
        }
        else if (baseString.compare("OGL_EYE_DIR") == 0 && tokens[i].size() > 3) {
            par->opengl_eye_dir
                = m4d::vec3(atof(tokens[i][1].c_str()), atof(tokens[i][2].c_str()), atof(tokens[i][3].c_str()));
        }
        else if (baseString.compare("OGL_FOV") == 0 && tokens[i].size() > 1) {
            par->opengl_fov = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_FOV_STEP") == 0 && tokens[i].size() > 1) {
            par->opengl_fov_step = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_LINE_COLOR") == 0 && tokens[i].size() > 3) {
            par->opengl_line_color
                = QColor(atoi(tokens[i][1].c_str()), atoi(tokens[i][2].c_str()), atoi(tokens[i][3].c_str()));
        }
        else if (baseString.compare("OGL_LINE_WIDTH") == 0 && tokens[i].size() > 1) {
            par->opengl_line_width = atoi(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_LINE_SMOOTH") == 0 && tokens[i].size() > 1) {
            par->opengl_line_smooth = atoi(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_BG_COLOR") == 0 && tokens[i].size() > 3) {
            par->opengl_bg_color
                = QColor(atoi(tokens[i][1].c_str()), atoi(tokens[i][2].c_str()), atoi(tokens[i][3].c_str()));
        }
        else if (baseString.compare("OGL_EMBED_COLOR") == 0 && tokens[i].size() > 3) {
            par->opengl_emb_color
                = QColor(atoi(tokens[i][1].c_str()), atoi(tokens[i][2].c_str()), atoi(tokens[i][3].c_str()));
        }
        else if (baseString.compare("OGL_STEREO_USE") == 0 && tokens[i].size() > 1) {
            par->opengl_stereo_use = atoi(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_STEREO_GLASSES") == 0 && tokens[i].size() > 1) {
            par->opengl_stereo_glasses = static_cast<enum_stereo_glasses>(atoi(tokens[i][1].c_str()));
        }
        else if (baseString.compare("OGL_STEREO_TYPE") == 0 && tokens[i].size() > 1) {
            par->opengl_stereo_type = static_cast<enum_stereo_type>(atoi(tokens[i][1].c_str()));
        }
        else if (baseString.compare("OGL_STEREO_SEP") == 0 && tokens[i].size() > 1) {
            par->opengl_stereo_sep = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_STEREO_STEP") == 0 && tokens[i].size() > 1) {
            par->opengl_stereo_step = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_FOG_USE") == 0 && tokens[i].size() > 1) {
            par->opengl_fog_use = atoi(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_FOG_DENSITY") == 0 && tokens[i].size() > 1) {
            par->opengl_fog_init = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_FOG_STEP") == 0 && tokens[i].size() > 1) {
            par->opengl_fog_step = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_SCALE_X") == 0 && tokens[i].size() > 1) {
            par->opengl_scale_x = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_SCALE_Y") == 0 && tokens[i].size() > 1) {
            par->opengl_scale_y = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_SCALE_Z") == 0 && tokens[i].size() > 1) {
            par->opengl_scale_z = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_ANIM_LOCAL") == 0 && tokens[i].size() > 1) {
            par->opengl_anim_local = atoi(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_ANIM_ROT_X") == 0 && tokens[i].size() > 1) {
            par->opengl_anim_rot_x = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_ANIM_ROT_Y") == 0 && tokens[i].size() > 1) {
            par->opengl_anim_rot_y = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_ANIM_ROT_Z") == 0 && tokens[i].size() > 1) {
            par->opengl_anim_rot_z = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_SACHS_SYSTEM") == 0 && tokens[i].size() > 1) {
            par->opengl_sachs_system = static_cast<enum_sachs_system>(atoi(tokens[i][1].c_str()));
        }
        else if (baseString.compare("OGL_SACHS_LEGS") == 0 && tokens[i].size() > 1) {
            par->opengl_sachs_legs = static_cast<enum_sachs_legs>(atoi(tokens[i][1].c_str()));
        }
        else if (baseString.compare("OGL_SACHS_SCALE") == 0 && tokens[i].size() > 1) {
            par->opengl_sachs_scale = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_LEG_1_COL_1") == 0 && tokens[i].size() > 3) {
            par->opengl_leg1_col1
                = QColor(atoi(tokens[i][1].c_str()), atoi(tokens[i][2].c_str()), atoi(tokens[i][3].c_str()));
        }
        else if (baseString.compare("OGL_LEG_1_COL_2") == 0 && tokens[i].size() > 3) {
            par->opengl_leg1_col2
                = QColor(atoi(tokens[i][1].c_str()), atoi(tokens[i][2].c_str()), atoi(tokens[i][3].c_str()));
        }
        else if (baseString.compare("OGL_LEG_1_FREQ") == 0 && tokens[i].size() > 1) {
            par->opengl_leg1_freq = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_LEG_2_COL_1") == 0 && tokens[i].size() > 3) {
            par->opengl_leg2_col1
                = QColor(atoi(tokens[i][1].c_str()), atoi(tokens[i][2].c_str()), atoi(tokens[i][3].c_str()));
        }
        else if (baseString.compare("OGL_LEG_2_COL_2") == 0 && tokens[i].size() > 3) {
            par->opengl_leg2_col2
                = QColor(atoi(tokens[i][1].c_str()), atoi(tokens[i][2].c_str()), atoi(tokens[i][3].c_str()));
        }
        else if (baseString.compare("OGL_LEG_2_FREQ") == 0 && tokens[i].size() > 1) {
            par->opengl_leg2_freq = atof(tokens[i][1].c_str());
        }
        else if (baseString.compare("OGL_EMB") == 0 && tokens[i].size() > 2) {
            par->opengl_emb_params.insert(std::pair<std::string, double>(tokens[i][1], atof(tokens[i][2].c_str())));
        }
        else if (baseString.compare("DRAW2D_BORDER") == 0 && tokens[i].size() > 4) {
            par->draw2d_xMin = atof(tokens[i][1].c_str());
            par->draw2d_xMax = atof(tokens[i][2].c_str());
            par->draw2d_yMin = atof(tokens[i][3].c_str());
            par->draw2d_yMax = atof(tokens[i][4].c_str());
        }
        else if (baseString.compare("DRAW2D_BG_COLOR") == 0 && tokens[i].size() > 3) {
            par->draw2d_bg_color
                = QColor(atoi(tokens[i][1].c_str()), atoi(tokens[i][2].c_str()), atoi(tokens[i][3].c_str()));
        }
        else if (baseString.compare("DRAW2D_LINE_COLOR") == 0 && tokens[i].size() > 3) {
            par->draw2d_line_color
                = QColor(atoi(tokens[i][1].c_str()), atoi(tokens[i][2].c_str()), atoi(tokens[i][3].c_str()));
        }
        else if (baseString.compare("DRAW2D_LINE_WIDTH") == 0 && tokens[i].size() > 1) {
            par->draw2d_line_width = atoi(tokens[i][1].c_str());
        }
        else if (baseString.compare("DRAW2D_LINE_SMOOTH") == 0 && tokens[i].size() > 1) {
            par->draw2d_line_smooth = atoi(tokens[i][1].c_str());
        }
        else if (baseString.compare("DRAW2D_GRID_COLOR") == 0 && tokens[i].size() > 3) {
            par->draw2d_grid_color
                = QColor(atoi(tokens[i][1].c_str()), atoi(tokens[i][2].c_str()), atoi(tokens[i][3].c_str()));
        }
        else if (baseString.compare("DRAW2D_REPRESENTATION") == 0 && tokens[i].size() > 1) {
            par->draw2d_representation = static_cast<m4d::enum_draw_type>(atoi(tokens[i][1].c_str()));
        }
        else if (baseString.compare("DRAW2D_ABSCISSA") == 0 && tokens[i].size() > 1) {
            par->draw2d_abscissa = atoi(tokens[i][1].c_str());
        }
        else if (baseString.compare("DRAW2D_ORDINATE") == 0 && tokens[i].size() > 1) {
            par->draw2d_ordinate = atoi(tokens[i][1].c_str());
        }
    }
    return true;
}

bool saveParamFile(const std::string& filename, struct_params* par)
{
    FILE* fptr = nullptr;
#ifdef _WIN32
    fopen_s(&fptr, filename.c_str(), "w");
#else
    fptr = fopen(filename.c_str(), "w");
#endif

    if (fptr == nullptr) {
        fprintf(stderr, "Cannot open %s for parameter output!\n", filename.c_str());
        return false;
    }

    fprintf(fptr, "# ----------------------------------------------------------\n");
    fprintf(fptr, "# Parameter file : %s\n", filename.c_str());
    fprintf(fptr, "# ----------------------------------------------------------\n");

    fprintf(fptr, "OGL_PROJECTION       %d\n", par->opengl_projection);
    fprintf(fptr, "OGL_DRAW3D_TYPE      %d\n", static_cast<int>(par->opengl_draw3d_type));
    fprintf(fptr, "# -----\n");
    fprintf(fptr, "OGL_EYE_POS          %16.12f %16.12f %16.12f\n", par->opengl_eye_pos[0], par->opengl_eye_pos[1],
        par->opengl_eye_pos[2]);
    fprintf(fptr, "OGL_EYE_POS_STEP     %16.12f\n", par->opengl_eye_pos_step);
    fprintf(fptr, "OGL_EYE_POI          %16.12f %16.12f %16.12f\n", par->opengl_eye_poi[0], par->opengl_eye_poi[1],
        par->opengl_eye_poi[2]);
    fprintf(fptr, "OGL_EYE_POI_STEP     %16.12f\n", par->opengl_eye_poi_step);
    fprintf(fptr, "OGL_EYE_VUP          %16.12f %16.12f %16.12f\n", par->opengl_eye_vup[0], par->opengl_eye_vup[1],
        par->opengl_eye_vup[2]);
    fprintf(fptr, "OGL_EYE_DIR          %16.12f %16.12f %16.12f\n", par->opengl_eye_dir[0], par->opengl_eye_dir[1],
        par->opengl_eye_dir[2]);
    fprintf(fptr, "OGL_FOV              %16.12f\n", par->opengl_fov);
    fprintf(fptr, "OGL_FOV_STEP         %16.12f\n", par->opengl_fov_step);
    fprintf(fptr, "# -----\n");
    fprintf(fptr, "OGL_LINE_COLOR       %3d %3d %3d\n", par->opengl_line_color.red(), par->opengl_line_color.green(),
        par->opengl_line_color.blue());
    fprintf(fptr, "OGL_LINE_WIDTH       %d\n", par->opengl_line_width);
    fprintf(fptr, "OGL_LINE_SMOOTH      %d\n", par->opengl_line_smooth);
    fprintf(fptr, "OGL_BG_COLOR         %3d %3d %3d\n", par->opengl_bg_color.red(), par->opengl_bg_color.green(),
        par->opengl_bg_color.blue());
    fprintf(fptr, "OGL_EMBED_COLOR      %3d %3d %3d\n", par->opengl_emb_color.red(), par->opengl_emb_color.green(),
        par->opengl_emb_color.blue());
    if (par->opengl_emb_params.size() > 0) {
        fprintf(fptr, "# -----\n");
        std::map<std::string, double>::iterator mapItr = par->opengl_emb_params.begin();
        while (mapItr != par->opengl_emb_params.end()) {
            fprintf(fptr, "OGL_EMB              %-12s %10.6f\n", mapItr->first.c_str(), mapItr->second);
            ++mapItr;
        }
    }
    fprintf(fptr, "# -----\n");
    fprintf(fptr, "OGL_STEREO_USE         %d\n", par->opengl_stereo_use);
    fprintf(fptr, "OGL_STEREO_GLASSES     %d\n", par->opengl_stereo_glasses);
    fprintf(fptr, "OGL_STEREO_TYPE        %d\n", par->opengl_stereo_type);
    fprintf(fptr, "OGL_STEREO_SEP         %6.4f\n", par->opengl_stereo_sep);
    fprintf(fptr, "OGL_STEREO_STEP        %6.4f\n", par->opengl_stereo_step);
    fprintf(fptr, "OGL_FOG_USE            %d\n", par->opengl_fog_use);
    fprintf(fptr, "OGL_FOG_DENSITY        %6.4f\n", par->opengl_fog_init);
    fprintf(fptr, "OGL_FOG_STEP           %6.4f\n", par->opengl_fog_step);
    fprintf(fptr, "OGL_SCALE_X            %6.4f\n", par->opengl_scale_x);
    fprintf(fptr, "OGL_SCALE_Y            %6.4f\n", par->opengl_scale_y);
    fprintf(fptr, "OGL_SCALE_Z            %6.4f\n", par->opengl_scale_z);
    fprintf(fptr, "OGL_ANIM_LOCAL         %d\n", par->opengl_anim_local);
    fprintf(fptr, "OGL_ANIM_ROT_X         %6.4f\n", par->opengl_anim_rot_x);
    fprintf(fptr, "OGL_ANIM_ROT_Y         %6.4f\n", par->opengl_anim_rot_y);
    fprintf(fptr, "OGL_ANIM_ROT_Z         %6.4f\n", par->opengl_anim_rot_z);
    fprintf(fptr, "# -----\n");
    fprintf(fptr, "OGL_SACHS_SYSTEM       %d\n", static_cast<int>(par->opengl_sachs_system));
    fprintf(fptr, "OGL_SACHS_LEGS         %d\n", static_cast<int>(par->opengl_sachs_legs));
    fprintf(fptr, "OGL_SACHS_SCALE        %4.1f\n", par->opengl_sachs_scale);
    fprintf(fptr, "OGL_LEG_1_COL_1        %3d %3d %3d\n", par->opengl_leg1_col1.red(), par->opengl_leg1_col1.green(),
        par->opengl_leg1_col1.blue());
    fprintf(fptr, "OGL_LEG_1_COL_2        %3d %3d %3d\n", par->opengl_leg1_col2.red(), par->opengl_leg1_col2.green(),
        par->opengl_leg1_col2.blue());
    fprintf(fptr, "OGL_LEG_1_FREQ         %4.1f\n", par->opengl_leg1_freq);
    fprintf(fptr, "OGL_LEG_2_COL_1        %3d %3d %3d\n", par->opengl_leg2_col1.red(), par->opengl_leg2_col1.green(),
        par->opengl_leg2_col1.blue());
    fprintf(fptr, "OGL_LEG_2_COL_2        %3d %3d %3d\n", par->opengl_leg2_col2.red(), par->opengl_leg2_col2.green(),
        par->opengl_leg2_col2.blue());
    fprintf(fptr, "OGL_LEG_2_FREQ         %4.1f\n", par->opengl_leg2_freq);
    fprintf(fptr, "# -----\n");
    fprintf(fptr, "DRAW2D_BORDER          %10.6f %10.6f %10.6f %10.6f\n", par->draw2d_xMin, par->draw2d_xMax,
        par->draw2d_yMin, par->draw2d_yMax);
    fprintf(fptr, "DRAW2D_BG_COLOR        %3d %3d %3d\n", par->draw2d_bg_color.red(), par->draw2d_bg_color.green(),
        par->draw2d_bg_color.blue());
    fprintf(fptr, "DRAW2D_LINE_COLOR      %3d %3d %3d\n", par->draw2d_line_color.red(), par->draw2d_line_color.green(),
        par->draw2d_line_color.blue());
    fprintf(fptr, "DRAW2D_LINE_WIDTH      %d\n", par->draw2d_line_width);
    fprintf(fptr, "DRAW2D_LINE_SMOOTH     %d\n", par->draw2d_line_smooth);
    fprintf(fptr, "DRAW2D_GRID_COLOR      %3d %3d %3d\n", par->draw2d_grid_color.red(), par->draw2d_grid_color.green(),
        par->draw2d_grid_color.blue());
    fprintf(fptr, "DRAW2D_REPRESENTATION  %d\n", (int)par->draw2d_representation);
    fprintf(fptr, "DRAW2D_ABSCISSA        %d\n", par->draw2d_abscissa);
    fprintf(fptr, "DRAW2D_ORDINATE        %d\n", par->draw2d_ordinate);
    fclose(fptr);
    return true;
}

#ifdef HAVE_LUA
int setGVObject(lua_State*)
{
    return 0;
}

void lua_reg_gvutils(lua_State* L)
{
    lua_register(L, "SetGVObject", setGVObject);
}

#endif // HAVE_LUA
