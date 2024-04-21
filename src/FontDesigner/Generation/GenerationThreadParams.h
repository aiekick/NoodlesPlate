#pragma once

#include <ctools/cTools.h>
#include <Headers/Globals.h>
#include <msdfgen/core/Shape.h>
#include <msdfgen/core/Bitmap.h>
#include <ctools/ConfigAbstract.h>

///////////////////////////////////////////////////////
//// WORKER THREAD ////////////////////////////////////
///////////////////////////////////////////////////////

struct GlyphStruct
{
	int idx;
	msdfgen::Shape shape;
	float advx;
	char c;
	ct::fvec4 rc;
	ct::fvec4 rect;
	ct::fvec2 offset;

	bool dfInverted;
	bool invertedY;
	bool IsModified;

	ct::fvec2 size;
	ct::fvec3 horiBearing; // x,y,adv
	ct::fvec3 vertBearing; // x,y,adv

	ct::fvec2 shaderCenterOffset;
	std::vector<bool> contourSdfSigns;

	bool useSVG;
	msdfgen::Shape svgShape;
	std::string svgFilePathName;

	GlyphStruct()
	{
		advx = 0;
		c = 0;
		shaderCenterOffset = 0.5f;
		useSVG = false;
		invertedY = false;
		IsModified = false;
		dfInverted = false;
		idx = 0;
		contourSdfSigns.clear();
	}

	GlyphStruct(char vC)
	{
		advx = 0;
		c = 0;
		shaderCenterOffset = 0.5f;
		useSVG = false;
		invertedY = false;
		IsModified = false;
		dfInverted = false;
		idx = 0;
		contourSdfSigns.clear();
		c = vC;
	}

	GlyphStruct(GlyphStruct *vGlyph)
	{
		Copy(vGlyph);
	}

	void Copy(GlyphStruct *vGlyph)
	{
		if (vGlyph)
		{
			idx = vGlyph->idx;
			shape = vGlyph->shape;
			advx = vGlyph->advx;
			c = vGlyph->c;
			rc = vGlyph->rc;
			rect = vGlyph->rect;
			offset = vGlyph->offset;

			dfInverted = vGlyph->dfInverted;
			invertedY = vGlyph->invertedY;
			IsModified = vGlyph->IsModified;

			size = vGlyph->size;
			horiBearing = vGlyph->horiBearing;
			vertBearing = vGlyph->vertBearing;

			shaderCenterOffset = vGlyph->shaderCenterOffset;
			contourSdfSigns = vGlyph->contourSdfSigns;

			useSVG = vGlyph->useSVG;
			svgShape = vGlyph->svgShape;
			svgFilePathName = vGlyph->svgFilePathName;
		}
	}
};

struct stbrp_rect;
struct BinPackRectStruct
{
	msdfgen::Bitmap<float> sdf;
    msdfgen::Bitmap<float, 3> msdf;
	stbrp_rect *rect;
	GlyphStruct *glyphStruct;
};

class GenerationThreadParams : public conf::ConfigAbstract
{
public:
	int m_ImageDataSize;
	GLubyte *m_ImageData;
	std::list<BinPackRectStruct> m_BinPackRects;
	
public:
	bool IsFontLoaded;

	float scale;
	bool autoFrame;
	bool autoSize;
	ct::ivec2 translate;
	float msdfColoringAngle;
	ct::ivec2 glyphSize;
	ct::ivec2 glyphPadding;
	ct::ivec2 texPadding;
	ct::ivec2 texMaxSize;
	std::string charSet;
	AlgoEnum algo;
	float range;
	bool InvertY;
	int bytesPerPixel;
	FontTextureTypeEnum ExportTexType;
	bool autoUpdate;
	bool formatFloat;
	int theme;
	float renderingQuality;
	std::string lastShader;
	bool exportFlipY;
	int SubSamples;
	std::string exportFileName;
	std::string exportPath;
	std::string openPath;
	FontLoadingModeEnum FontLoadingMode;
	std::string FontFileLoaded;
	
	int defaultLineHeight;
	int lineHeight;
	int defaultBaseHeight;
	int baseHeight;

	std::string fontName;
	std::string fontPath;
	std::string m_CurrentFont;

public:
	GLuint textureFont;
	ct::ivec2 textureSize;
	std::string charSetNotPacked;
	std::map<char, GlyphStruct> Glyphs;
	std::map<char, bool> ModifiedGlyphs;

public:
	GenerationThreadParams();
	~GenerationThreadParams();

	void Copy(GenerationThreadParams *vParams);

	GlyphStruct* GetGlyph(char vC);
	void SetGlyph(char vC, GlyphStruct vGlyph);

	///////////////////////////////////////////////////////
	//// CONFIGURATION ////////////////////////////////////
	///////////////////////////////////////////////////////

    std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
    bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;
};
