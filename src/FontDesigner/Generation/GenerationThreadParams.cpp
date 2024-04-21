// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <FontDesigner/Generation/GenerationThreadParams.h>
#include <Headers/Globals.h>

GenerationThreadParams::GenerationThreadParams()
{
	IsFontLoaded = false;

	scale = 2.0f;
	autoFrame = true;
	autoSize = true;
	msdfColoringAngle = 3.0;
	translate = 0;
	texMaxSize = 512;
	glyphSize = ct::ivec2(50, 60);
	glyphPadding = 5;
	texPadding = 2;
	charSet = defaultCharSet;
	fontName = "";
	algo = AlgoEnum::ALGO_SDF;
	range = 5.0f;
	InvertY = true;
	exportFlipY = false;
	bytesPerPixel = 3;
	autoUpdate = true;
	formatFloat = false;
	theme = 1;
	lastShader = "";
	SubSamples = 0;
	exportFileName = "font.fnt";
	exportPath = ".";
	openPath = ".";
	FontLoadingMode = FontLoadingModeEnum::FONT_LOADING_FROM_SYSTEM;
	FontFileLoaded = "";
	renderingQuality = 1.0f;

	m_ImageDataSize = 0;
	
	// export
	ExportTexType = FontTextureTypeEnum::FONT_TEXTURE_TYPE_RENDERED;

	textureFont = 0;
	
	lineHeight = 0;
	defaultLineHeight = 0;
	baseHeight = 0;
	defaultBaseHeight = 0;

	fontName = "";
	fontPath = "";
	m_CurrentFont = "";
}

GenerationThreadParams::~GenerationThreadParams()
{

}

void GenerationThreadParams::Copy(GenerationThreadParams *vParams)
{
	if (vParams)
	{
		IsFontLoaded = vParams->IsFontLoaded;
		scale = vParams->scale;
		autoFrame = vParams->autoFrame;
		autoSize = vParams->autoSize;
		msdfColoringAngle = vParams->msdfColoringAngle;
		translate = vParams->translate;
		texMaxSize = vParams->texMaxSize;
		glyphSize = vParams->glyphSize;
		glyphPadding = vParams->glyphPadding;
		texPadding = vParams->texPadding;
		charSet = vParams->charSet;
		fontName = vParams->fontName;
		algo = vParams->algo;
		range = vParams->range;
		InvertY = vParams->InvertY;
		bytesPerPixel = vParams->bytesPerPixel;
		autoUpdate = vParams->autoUpdate;
		formatFloat = vParams->formatFloat;
		theme = vParams->theme;
		lastShader = vParams->lastShader;
		SubSamples = vParams->SubSamples;
		exportFlipY = vParams->exportFlipY;
		exportFileName = vParams->exportFileName;
		exportPath = vParams->exportPath;
		openPath = vParams->openPath;
		FontLoadingMode = vParams->FontLoadingMode;
		FontFileLoaded = vParams->FontFileLoaded;
		renderingQuality = vParams->renderingQuality;
		ExportTexType = vParams->ExportTexType;

		textureFont = vParams->textureFont;
		textureSize = vParams->textureSize;
		charSetNotPacked = vParams->charSetNotPacked;
		ModifiedGlyphs = vParams->ModifiedGlyphs;

		Glyphs.clear();
		for (auto it = vParams->Glyphs.begin(); it != vParams->Glyphs.end(); ++it)
		{
			char c = it->first;
			GlyphStruct *glyph = &it->second;
			Glyphs[c] = GlyphStruct(glyph);
		}
		
		lineHeight = vParams->lineHeight;
		defaultLineHeight = vParams->defaultLineHeight;
		baseHeight = vParams->baseHeight;
		defaultBaseHeight = vParams->defaultBaseHeight;

		fontName = vParams->fontName;
		fontPath = vParams->fontPath;
		m_CurrentFont = vParams->m_CurrentFont;
	}
}

GlyphStruct* GenerationThreadParams::GetGlyph(char vC)
{
	if (Glyphs.find(vC) != Glyphs.end()) // trouvé
	{
		return &Glyphs[vC];
	}
	return 0;
}

void GenerationThreadParams::SetGlyph(char vC, GlyphStruct vGlyph)
{
	if (Glyphs.find(vC) != Glyphs.end()) // trouvé
	{
		Glyphs[vC] = vGlyph;
	}
}

///////////////////////////////////////////////////////
//// CONFIGURATION ////////////////////////////////////
///////////////////////////////////////////////////////

std::string GenerationThreadParams::getXml(const std::string& vOffset, const std::string& vUserDatas) {
	std::string str;

	str += vOffset + "<scale>" + ct::toStr(scale) + "</scale>\n";
	str += vOffset + "<autoFrame>" + ct::toStr(autoFrame ? "true" : "false") + "</autoFrame>\n";
	str += vOffset + "<autoSize>" + ct::toStr(autoSize ? "true" : "false") + "</autoSize>\n";
	str += vOffset + "<translate>" + translate.string() + "</translate>\n";
	str += vOffset + "<glyphSize>" + glyphSize.string() + "</glyphSize>\n";
	str += vOffset + "<glyphPadding>" + glyphPadding.string() + "</glyphPadding>\n";
	str += vOffset + "<texPadding>" + texPadding.string() + "</texPadding>\n";
	str += vOffset + "<texMaxSize>" + texMaxSize.string() + "</texMaxSize>\n";
    str += vOffset + "<charSet>" + charSet + "</charSet>\n";
	str += vOffset + "<fontName>" + fontName + "</fontName>\n";
	str += vOffset + "<algo>" + ct::toStr((int)algo) + "</algo>\n";
	str += vOffset + "<theme>" + ct::toStr(theme) + "</theme>\n";
	str += vOffset + "<range>" + ct::toStr(range) + "</range>\n";
	str += vOffset + "<msdfColoringAngle>" + ct::toStr(msdfColoringAngle) + "</msdfColoringAngle>\n";
	str += vOffset + "<InvertY>" + ct::toStr(InvertY ? "true" : "false") + "</InvertY>\n";
	str += vOffset + "<ExportTexType>" + ct::toStr((int)ExportTexType) + "</ExportTexType>\n";
	str += vOffset + "<FontLoadingMode>" + ct::toStr((int)FontLoadingMode) + "</FontLoadingMode>\n";
	str += vOffset + "<autoUpdate>" + ct::toStr(autoUpdate ? "true" : "false") + "</autoUpdate>\n";
	str += vOffset + "<formatFloat>" + ct::toStr(formatFloat ? "true" : "false") + "</formatFloat>\n";
	str += vOffset + "<exportFileName>" + exportFileName + "</exportFileName>\n";
	str += vOffset + "<exportPath>" + exportPath + "</exportPath>\n";
	str += vOffset + "<exportflipy>" + ct::toStr(exportFlipY ? "true" : "false") + "</exportflipy>\n";
	str += vOffset + "<openPath>" + openPath + "</openPath>\n";
	str += vOffset + "<SubSamples>" + ct::toStr(SubSamples) + "</SubSamples>\n";
	str += vOffset + "<FontFileLoaded>" + FontFileLoaded + "</FontFileLoaded>\n";
	str += vOffset + "<lastshader>" + lastShader + "</lastshader>\n";

	return str;
}

bool GenerationThreadParams::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) {
	// The value of this child identifies the name of this element
	std::string strName = "";
	std::string strValue = "";
	std::string strParentName = "";

	strName = vElem->Value();

	if (vElem->GetText())
		strValue = std::string(vElem->GetText());
	if (vParent != 0)
		strParentName = vParent->Value();

	if (strName == "scale") 
		scale = ct::fvariant(strValue).GetF();
	if (strName == "autoFrame") 
		autoFrame = ct::ivariant(strValue).GetB();
	if (strName == "autoSize") 
		autoSize = ct::ivariant(strValue).GetB();
	if (strName == "translate") 
		translate = ct::ivariant(strValue).GetV2();
	if (strName == "glyphSize") 
		glyphSize = ct::ivariant(strValue).GetV2();
	if (strName == "glyphPadding") 
		glyphPadding = ct::ivariant(strValue).GetV2();
	if (strName == "texPadding") 
		texPadding = ct::ivariant(strValue).GetV2();
	if (strName == "texMaxSize") 
		texMaxSize = ct::ivariant(strValue).GetV2();
	if (strName == "charSet")
        charSet = strValue;
	if (strName == "fontName") 
		fontName = strValue;
	if (strName == "algo") 
		algo = (AlgoEnum)ct::ivariant(strValue).GetI();
	if (strName == "theme") 
		theme = ct::ivariant(strValue).GetI();
	if (strName == "range") 
		range = ct::fvariant(strValue).GetF();
	if (strName == "msdfColoringAngle") 
		msdfColoringAngle = ct::fvariant(strValue).GetF();
	if (strName == "InvertY") 
		InvertY = ct::ivariant(strValue).GetB();
	if (strName == "exportflipy")
		exportFlipY = ct::ivariant(strValue).GetB();
	if (strName == "autoUpdate") 
		autoUpdate = ct::ivariant(strValue).GetB();
	if (strName == "ExportTexType") 
		ExportTexType = (FontTextureTypeEnum)ct::ivariant(strValue).GetI();
	if (strName == "FontLoadingMode") 
		FontLoadingMode = (FontLoadingModeEnum)ct::ivariant(strValue).GetI();
	if (strName == "formatFloat") 
		formatFloat = ct::ivariant(strValue).GetB();
	if (strName == "lastshader") 
		lastShader = strValue;
	if (strName == "exportFileName") 
		exportFileName = strValue;
	if (strName == "exportPath") 
		exportPath = strValue;
	if (strName == "openPath") 
		openPath = strValue;
	if (strName == "SubSamples") 
		SubSamples = ct::ivariant(strValue).GetI();
	if (strName == "FontFileLoaded") 
		FontFileLoaded = strValue;

	///////////////////////////////////////////

	return true;
}
