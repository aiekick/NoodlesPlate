#pragma once

#include <ctools/cTools.h>

#include <ft2build.h>
#include FT_OUTLINE_H
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H

#include <freetype\freetype.h>

#include <core/Shape.h>

#include <string>
#include <list>
#include <vector>

#include <FontDesigner/Generation/GenerationThreadParams.h>

#include <ctools/FileHelper.h>

struct GlyphContextStruct {
    msdfgen::Point2 position;
    msdfgen::Shape *shape;
    msdfgen::Contour *contour;

    GlyphContextStruct() {
        shape = 0;
        contour = 0;
    }
};

class FontExplorer;
struct FontInfosStruct {
    std::string name;
    std::string path;
    std::string filePathName;
    bool confExist;
    std::string ext;
    bool isOk;
    FontInfosStruct() {
        confExist = false;
        isOk = false;
    }
    FontInfosStruct(std::string vfontName, std::string vFilePathName, FontExplorer *vFontExplorer);
};

class FontExplorer {
private:
    FT_Library lib;
    FT_Face face;
    std::map<std::string, FontInfosStruct> m_SystemFontsDataBase;

public:
    // std::map<char, GlyphStruct> Glyphs;

public:
public:
    FontExplorer();
    ~FontExplorer();
    void Finish(GenerationThreadParams *vParams);

    void Init();

    bool LoadFontFromSystem(std::string vFontName, std::string vCharSet, GenerationThreadParams *vParams);
    bool LoadFontFromFile(std::string vFontFilepathName, std::string vCharSet, GenerationThreadParams *vParams);
    bool LoadFontFile(std::string vFontFilePathName, std::string vCharSet, GenerationThreadParams *vParams);

    std::string GetFontFilePathName(std::string vFontName);

    std::map<std::string, FontInfosStruct>::iterator begin();
    std::map<std::string, FontInfosStruct>::iterator end();
    size_t GetSysFontsDBSize();
    bool IsFontExisting(std::string vFontName);

    std::string GetFontParamFileName(std::string vFontName);
    void SaveFontParamFile(std::string vFontName, GenerationThreadParams *vParams);
    void LoadFontParamFile(std::string vFontName, GenerationThreadParams *vParams);

private:
    void AddGlyphInGlyphInversionList(GlyphStruct *vGlyph);

public:
    void ListSystemsFont_Win32();
};
