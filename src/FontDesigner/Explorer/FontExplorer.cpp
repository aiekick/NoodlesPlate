// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "FontExplorer.h"

#include <msdfgen.h>
#include <core/edge-segments.h>

#include <SoGLSL/src/Renderer/RenderPack.h>

#include <FontDesigner/Generation/GenerationThread.h>

#include <ctools/Logger.h>

//////////////////////////////////////////////////////////////////////////////

FontInfosStruct::FontInfosStruct(std::string vfontName, std::string vFilePathName, FontExplorer *vFontExplorer) {
    isOk = false;
    confExist = false;

    name = vfontName;
    filePathName = vFilePathName;
    auto ps = FileHelper::Instance()->ParsePathFileName(vFilePathName);
    if (ps.isOk) {
        path = ps.path;
        ext = ps.ext;

        isOk = true;
    }

    if (vFontExplorer) {
        std::string confFile = vFontExplorer->GetFontParamFileName(name);
        if (FileHelper::Instance()->IsFileExist(confFile)) {
            confExist = true;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

static msdfgen::Point2 ftPoint2(const FT_Vector &vector) {
    return msdfgen::Point2(vector.x / 64., vector.y / 64.);
}

static int ftMoveTo(const FT_Vector *to, void *user) {
    GlyphContextStruct *context = reinterpret_cast<GlyphContextStruct *>(user);
    context->contour = &context->shape->addContour();
    context->position = ftPoint2(*to);
    return 0;
}

static int ftLineTo(const FT_Vector *to, void *user) {
    GlyphContextStruct *context = reinterpret_cast<GlyphContextStruct *>(user);
    context->contour->addEdge(new msdfgen::LinearSegment(context->position, ftPoint2(*to)));
    context->position = ftPoint2(*to);
    return 0;
}

static int ftConicTo(const FT_Vector *control, const FT_Vector *to, void *user) {
    GlyphContextStruct *context = reinterpret_cast<GlyphContextStruct *>(user);
    context->contour->addEdge(new msdfgen::QuadraticSegment(context->position, ftPoint2(*control), ftPoint2(*to)));
    context->position = ftPoint2(*to);
    return 0;
}

static int ftCubicTo(const FT_Vector *control1, const FT_Vector *control2, const FT_Vector *to, void *user) {
    GlyphContextStruct *context = reinterpret_cast<GlyphContextStruct *>(user);
    context->contour->addEdge(new msdfgen::CubicSegment(context->position, ftPoint2(*control1), ftPoint2(*control2), ftPoint2(*to)));
    context->position = ftPoint2(*to);
    return 0;
}

//////////////////////////////////////////////////////////////////////////////

FontExplorer::FontExplorer() {
    // on met rien ici pour pas risquer de creer une boucle infinie
    // vu que FontExplorer::Instance() risquerait d'etre appel� ici
}

FontExplorer::~FontExplorer() {
}

void FontExplorer::Finish(GenerationThreadParams *vParams) {
    SaveFontParamFile(vParams->m_CurrentFont, vParams);
}

void FontExplorer::Init() {
    ListSystemsFont_Win32();
}

bool FontExplorer::LoadFontFromSystem(std::string vFontName, std::string vCharSet, GenerationThreadParams *vParams) {
    bool res = false;

    if (vFontName.size() > 0) {
        GenerationThread::Params->fontName = vFontName;
        vParams->fontPath = GetFontFilePathName(GenerationThread::Params->fontName);

        SaveFontParamFile(vParams->m_CurrentFont, vParams);
        res = LoadFontFile(vParams->fontPath, vCharSet, vParams);
        LoadFontParamFile(vFontName, vParams);
        vParams->fontName = vFontName;
        vParams->m_CurrentFont = vFontName;
    }

    return res;
}

bool FontExplorer::LoadFontFromFile(std::string vFontFilepathName, std::string vCharSet, GenerationThreadParams *vParams) {
    bool res = false;

    if (vFontFilepathName.size() > 0) {
        std::size_t lastSlash = vFontFilepathName.find_last_of('\\');
        //	std::string filePath = vFontFilepathName.substr(0, lastSlash);
        std::string fileNameExt = vFontFilepathName.substr(lastSlash + 1);
        std::size_t lastPoint = fileNameExt.find_last_of('.');
        std::string fileName = fileNameExt.substr(0, lastPoint);
        //	std::string fileExt = fileNameExt.substr(lastPoint + 1);

        GenerationThread::Params->fontName = fileName;
        vParams->fontName = fileName;
        vParams->fontPath = vFontFilepathName;

        SaveFontParamFile(vParams->m_CurrentFont, vParams);
        res = LoadFontFile(vParams->fontPath, vCharSet, vParams);
        LoadFontParamFile(fileName, vParams);
        vParams->m_CurrentFont = fileName;
    }

    return res;
}

bool FontExplorer::LoadFontFile(std::string vFontFilePathName, std::string vCharSet, GenerationThreadParams *vParams) {
    bool res = false;

    if (vFontFilePathName.size()) {
        vParams->Glyphs.clear();

        FT_Error error = FT_Init_FreeType(&lib);
        if (!error) {
            if (lib) {
                error = FT_New_Face(lib, vFontFilePathName.c_str(), 0, &face);
                if (!error) {
                    vParams->lineHeight = face->height / 64.0;
                    vParams->baseHeight = face->ascender / 64.0;

                    vParams->defaultLineHeight = vParams->lineHeight;
                    vParams->defaultBaseHeight = vParams->baseHeight;

                    std::string charSetFiletered = vCharSet;
                    ct::replaceString(charSetFiletered, "\n", "");
                    ct::replaceString(charSetFiletered, "\t", "");

                    for (std::string::iterator it = charSetFiletered.begin(); it != charSetFiletered.end(); ++it) {
                        GlyphStruct glyph = GlyphStruct((char)*it);

                        // LogStr("char loaded : " + *it);

                        error = FT_Load_Char(face, glyph.c, FT_LOAD_NO_SCALE);
                        if (!error) {
                            glyph.shape.contours.clear();
                            glyph.shape.inverseYAxis = false;
                            glyph.advx = face->glyph->advance.x / 64.0f;

                            glyph.size = ct::fvec2(face->glyph->metrics.width, face->glyph->metrics.height) / 64.0f;

                            glyph.horiBearing = ct::fvec3(face->glyph->metrics.horiBearingX, face->glyph->metrics.horiBearingY, face->glyph->metrics.horiAdvance) / 64.0f;

                            glyph.vertBearing = ct::fvec3(face->glyph->metrics.vertBearingX, face->glyph->metrics.vertBearingY, face->glyph->metrics.vertAdvance) / 64.0f;

                            GlyphContextStruct context = {};
                            context.shape = &glyph.shape;

                            FT_Outline_Funcs ftFunctions;
                            ftFunctions.move_to = &ftMoveTo;
                            ftFunctions.line_to = &ftLineTo;
                            ftFunctions.conic_to = &ftConicTo;
                            ftFunctions.cubic_to = &ftCubicTo;
                            ftFunctions.shift = 0;
                            ftFunctions.delta = 0;

                            error = FT_Outline_Decompose(&face->glyph->outline, &ftFunctions, &context);
                            if (!error) {
                                AddGlyphInGlyphInversionList(&glyph);

                                vParams->Glyphs[glyph.c] = glyph;
                            }
                        }
                    }

                    if (vParams->Glyphs.size() > 0) {
                        res = true;
                    }

                    FT_Done_Face(face);
                }
            }

            FT_Done_FreeType(lib);
        }
    }

    return res;
}

void FontExplorer::ListSystemsFont_Win32() {
    m_SystemFontsDataBase.clear();

    static const LPCSTR fontRegistryPath = "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
    HKEY hKey;
    LONG result;

    // Open Windows font registry key
    result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, fontRegistryPath, 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        return;
    }

    DWORD maxValueNameSize, maxValueDataSize;
    result = RegQueryInfoKey(hKey, 0, 0, 0, 0, 0, 0, 0, &maxValueNameSize, &maxValueDataSize, 0, 0);
    if (result != ERROR_SUCCESS) {
        return;
    }

    DWORD valueIndex = 0;
    LPSTR valueName = new CHAR[maxValueNameSize];
    LPBYTE valueData = new BYTE[maxValueDataSize];
    DWORD valueNameSize, valueDataSize, valueType;
    std::string sFontFile;

    // Build full font file path
    CHAR winDir[MAX_PATH];
    GetWindowsDirectory(winDir, MAX_PATH);

    // Look for a matching font name
    do {
        sFontFile.clear();
        valueDataSize = maxValueDataSize;
        valueNameSize = maxValueNameSize;

        result = RegEnumValue(hKey, valueIndex, valueName, &valueNameSize, 0, &valueType, valueData, &valueDataSize);

        valueIndex++;

        if (result != ERROR_SUCCESS || valueType != REG_SZ) {
            continue;
        }

        sFontFile.assign((LPSTR)valueData, valueDataSize);

        if (sFontFile.empty()) {
            return;
        }

        std::stringstream ss;
        ss << winDir << "\\Fonts\\" << sFontFile;
        sFontFile = ss.str();

        std::string sValueName(valueName, valueNameSize);

        // seulement les font TTF
        // if (sFontFile.find("ttf") != std::string::npos)
        {
            auto fStruct = FontInfosStruct(sValueName, sFontFile, this);
            if (fStruct.isOk)
                m_SystemFontsDataBase[sValueName] = fStruct;
        }

    } while (result != ERROR_NO_MORE_ITEMS);

    delete[] valueName;
    delete[] valueData;

    RegCloseKey(hKey);
}

std::string FontExplorer::GetFontFilePathName(std::string vFontName) {
    if (m_SystemFontsDataBase.find(vFontName) != m_SystemFontsDataBase.end()) {
        return m_SystemFontsDataBase[vFontName].filePathName;
    }

    return "";
}

std::map<std::string, FontInfosStruct>::iterator FontExplorer::GetStartIter_SysFontsDB() {
    return m_SystemFontsDataBase.begin();
}

std::map<std::string, FontInfosStruct>::iterator FontExplorer::GetEndIter_SysFontsDB() {
    return m_SystemFontsDataBase.end();
}

size_t FontExplorer::GetSysFontsDBSize() {
    return m_SystemFontsDataBase.size();
}

bool FontExplorer::IsFontExisting(std::string vFontName) {
    return m_SystemFontsDataBase.find(vFontName) != m_SystemFontsDataBase.end();  // trouv�
}

std::string FontExplorer::GetFontParamFileName(std::string vFontName) {
    if (vFontName.size() > 0) {
        std::string fontName = vFontName;
        ct::replaceString(fontName, " ", "_");
        ct::replaceString(fontName, "(", "_");
        ct::replaceString(fontName, ")", "_");
        return FileHelper::Instance()->GetAbsolutePathForFileLocation(fontName + ".conf", (int)FILE_LOCATION_Enum::FILE_LOCATION_FONTS);
    } else {
        LogVarError("vFontName est vide");
    }
    return "";
}

void FontExplorer::SaveFontParamFile(std::string vFontName, GenerationThreadParams *vParams) {
    std::string configFile = GetFontParamFileName(vFontName);

    if (configFile.size() > 0) {
        std::string fileStream;

        fileStream += "lineheight:" + ct::fvariant(vParams->lineHeight).GetS() + "\n";
        fileStream += "baseheight:" + ct::fvariant(vParams->baseHeight).GetS() + "\n";

        for (auto it = vParams->Glyphs.begin(); it != vParams->Glyphs.end(); ++it) {
            GlyphStruct glyph = it->second;

            if (glyph.c != ' ') {
                fileStream += glyph.c;
                fileStream += ":";

                auto l = glyph.contourSdfSigns;
                for (auto itL = l.begin(); itL != l.end(); ++itL) {
                    bool b = *itL;

                    if (itL != l.begin())
                        fileStream += ",";
                    fileStream += std::string(b ? "true" : "false");
                }

                fileStream += ":" + ct::fvariant(glyph.shaderCenterOffset).GetS();
                fileStream += ":" + std::string(glyph.dfInverted ? "true" : "false");

                fileStream += "\n";
            }
        }

        std::ofstream configFileWriter(configFile, std::ios::out);
        if (configFileWriter.bad() == false) {
            configFileWriter << fileStream;
            configFileWriter.close();
        }
    }
}

void FontExplorer::LoadFontParamFile(std::string vFontName, GenerationThreadParams *vParams) {
    std::string configFile = GetFontParamFileName(vFontName);

    if (configFile.size() > 0) {
        std::string file;

        ifstream docFile(configFile, ios::in);
        if (docFile.is_open()) {
            stringstream strStream;

            strStream << docFile.rdbuf();  // read the file

            file = strStream.str();

            docFile.close();
        }

        std::vector<std::string> lines = ct::splitStringToVector(file, '\n');

        for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
            std::string line = *it;

            auto vec = ct::splitStringToVector(line, ':');

            if (vec.size() > 1) {
                if (vec[0] == "baseheight") {
                    vParams->baseHeight = ct::ivariant(vec[1]).GetI();
                } else if (vec[0] == "lineheight") {
                    vParams->lineHeight = ct::ivariant(vec[1]).GetI();
                } else {
                    char c = vec[0][0];

                    if (vParams->Glyphs.find(c) != vParams->Glyphs.end())  // trouv�
                    {
                        if (vec.size() > 1) {
                            std::string contourSigns = vec[1];
                            auto arrBool = ct::splitStringToVector(contourSigns, ',');
                            vParams->Glyphs[c].contourSdfSigns.clear();
                            for (auto itB = arrBool.begin(); itB != arrBool.end(); ++itB) {
                                bool b = ct::fvariant(*itB).GetB();
                                vParams->Glyphs[c].contourSdfSigns.push_back(b);
                            }
                        }

                        if (vec.size() > 2) {
                            std::string centerOffset = vec[2];
                            // le dernier c'est le vec2 du shader uv offset
                            vParams->Glyphs[c].shaderCenterOffset = ct::fvariant(centerOffset).GetV2();
                        }

                        if (vec.size() > 3) {
                            std::string dfInverted = vec[3];
                            // le dernier c'est le bool du shader dfInverted
                            vParams->Glyphs[c].dfInverted = ct::fvariant(dfInverted).GetB();
                        }
                    }
                }
            }
        }
    }
}

void FontExplorer::AddGlyphInGlyphInversionList(GlyphStruct *vGlyph) {
    if (vGlyph->contourSdfSigns.size() == 0) {
        for (std::vector<msdfgen::Contour>::iterator contour = vGlyph->shape.contours.begin(); contour != vGlyph->shape.contours.end(); ++contour) {
            vGlyph->contourSdfSigns.push_back(false);
        }
    }
}
