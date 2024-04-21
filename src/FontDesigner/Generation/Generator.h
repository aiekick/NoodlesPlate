#pragma once

#include "Globals.h"

#include "cTools.h"

#include <vector>

#include "ModelRendering.h"

#include "GenerationThreadParams.h"

#include <atomic>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/constants.hpp> // glm::pi

#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <chrono>
#include <map>
#include <vector>
#include <sstream>
#include <exception>

#include "FontExplorer.h"

#include "core\Bitmap.h"

#include "stb_rect_pack.h"
#include "stb\stb_image_write.h"

#include "Globals.h"

struct GLFWwindow;
class RenderPack;
class FontExplorer;
class Generator
{
private:
	FontExplorer *m_FontExplorer;

protected:
	GLFWwindow *m_Window;
	GenerationThreadParams *m_Params;

public:
	virtual void GenerateFullMap(
		std::atomic< float >& vProgress,
		std::atomic< bool >& vWorking,
		std::atomic< GLuint >& vCurrentTexture,
		std::atomic< float >& vGenerationTime);
	virtual void UpdateGhars(
		std::atomic< float >& vProgress, 
		std::atomic< bool >& vWorking,
		std::atomic< GLuint >& vCurrentTexture,
		std::atomic< float >& vGenerationTime
		);
	
public:
	Generator(GLFWwindow *vWin, GenerationThreadParams *vParams);
	~Generator();
	
private:
	void InvertGlyph(GlyphStruct *vGlyph);

private: // texture methods
	bool CreateImageBuffer();
	bool CreateImageBuffer(int vWidth, int vHeight);
	void FreeImageBuffer();

	void Upload1ChannelData(stbrp_coord *vX, stbrp_coord *vY,
		msdfgen::Bitmap<float> *vBuffer, int vMaxWidth);
	void Upload3ChannelData(stbrp_coord *vX, stbrp_coord *vY,
		msdfgen::Bitmap<msdfgen::FloatRGB> *vBuffer, int vMaxWidth);

	void CreateAndUploadFontTexture(ct::ivec2 vSize, bool vJustUpdate = false);
	void UpdateFontTexture(ct::fvec2 vOffset, ct::fvec2 vSize);

public:
	bool SaveFontTextureToPng(std::string vFilePathName);
};
