#define STB_RECT_PACK_IMPLEMENTATION
#include <FontDesigner/Generation/Generator.h>

#include <omp.h>

#include <ctools/Logger.h>

#include <msdfgen.h>
#include <FontDesigner/Explorer/FontExplorer.h>
#include <FontDesigner/Generation/GenerationThread.h>
#include <FontDesigner/Generation/GenerationThreadParams.h>

#include <SoGLSL/src/Gui/GuiBackend.h>

#include <Backends/MainBackend.h>

#include "stb_image_write.h"

Generator::Generator(GLFWwindow *vWin, GenerationThreadParams *vParams)
{
	m_Window = vWin;
	m_Params = vParams;
	m_Params->m_ImageData = 0;
	m_Params->m_ImageDataSize = 0;
	
	m_FontExplorer = new FontExplorer();
	m_FontExplorer->Init();
}

Generator::~Generator()
{
	FreeImageBuffer();
	m_FontExplorer->Finish(m_Params);
	SAFE_DELETE(m_FontExplorer);
}

void Generator::GenerateFullMap(
	std::atomic< float >& vProgress, 
	std::atomic< bool >& vWorking,
	std::atomic< GLuint >& vCurrentTexture,
	std::atomic< float >& vGenerationTime)
{
    GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

	FreeImageBuffer();

	if (!m_Params->IsFontLoaded)
	{
		if (m_Params->FontLoadingMode == FontLoadingModeEnum::FONT_LOADING_FROM_SYSTEM)
		{
			m_Params->IsFontLoaded = m_FontExplorer->LoadFontFromSystem(
				m_Params->fontName, m_Params->charSet, m_Params);
		}
		else if (m_Params->FontLoadingMode == FontLoadingModeEnum::FONT_LOADING_FROM_FILE)
		{
			m_Params->IsFontLoaded = m_FontExplorer->LoadFontFromFile(
				m_Params->FontFileLoaded, m_Params->charSet, m_Params);
		}
	}

	if (m_Params->IsFontLoaded)
	{
		if (CreateImageBuffer())
		{
			// STB RECT PACK
			stbrp_context *context = new stbrp_context();
			int            num_nodes = (int)ct::maxi<int>(m_Params->texMaxSize.x, m_Params->texMaxSize.y);
			stbrp_node    *nodes = new stbrp_node[num_nodes];
			int		       num_rects = (int)m_Params->charSet.size();
			stbrp_rect	  *rects = new stbrp_rect[num_rects];
			stbrp_init_target(context, (int)m_Params->texMaxSize.x, (int)m_Params->texMaxSize.y, nodes, num_nodes);

			// collect all rect in first pass
			int countGlyph = m_Params->Glyphs.size();
			vProgress = 0.0f;
			int index = 0;
			for (auto itGlyphs = m_Params->Glyphs.begin();
				itGlyphs != m_Params->Glyphs.end(); ++itGlyphs)
			{
				vProgress = (float)index / (float)countGlyph * 0.8f;
				if (!vWorking) break;

				char key = itGlyphs->first;
				GlyphStruct *glyph = &(itGlyphs->second);

				assert(glyph != 0);

				BinPackRectStruct binPackRect;
				binPackRect.rect = &rects[index];
				binPackRect.glyphStruct = glyph;

				if (glyph->shape.validate())
				{
					InvertGlyph(&(itGlyphs->second));

					double LARGE_VALUE = 1e8;
					double l = LARGE_VALUE, b = LARGE_VALUE, r = -LARGE_VALUE, t = -LARGE_VALUE;
                    glyph->shape.bound(l, b, r, t);
					l -= m_Params->glyphPadding.x / m_Params->scale;
					b -= m_Params->glyphPadding.y / m_Params->scale;
					r += m_Params->glyphPadding.x / m_Params->scale;
					t += m_Params->glyphPadding.y / m_Params->scale;

					glyph->shape.normalize();
					if (m_Params->InvertY != glyph->invertedY)
					{
						glyph->shape.inverseYAxis = !glyph->shape.inverseYAxis;
						glyph->invertedY = m_Params->InvertY;
					}
					if (m_Params->algo == AlgoEnum::ALGO_MSDF)	msdfgen::edgeColoringSimple(glyph->shape, (double)m_Params->msdfColoringAngle);

					bool CanWeGo = true;

					msdfgen::Vector2 _translate = msdfgen::Vector2(m_Params->translate.x, m_Params->translate.y);
					msdfgen::Vector2 _dims(r - l, t - b);

					if (m_Params->autoSize)
					{
						binPackRect.rect->w = (stbrp_coord)ct::maxi<double>(_dims.x * m_Params->scale, 1.0);
						binPackRect.rect->h = (stbrp_coord)ct::maxi<double>(_dims.y * m_Params->scale, 1.0);
					}
					else
					{
						binPackRect.rect->w = (stbrp_coord)m_Params->glyphSize.x;
						binPackRect.rect->h = (stbrp_coord)m_Params->glyphSize.y;
					}

					msdfgen::Vector2 _frame(binPackRect.rect->w, binPackRect.rect->h);

					if (m_Params->autoFrame)
					{
						_translate = .5*(_frame / m_Params->scale - _dims) - msdfgen::Vector2(l, b);
						glyph->offset = ct::fvec2(_translate.x, _translate.y) / ct::fvec2((int)m_Params->texMaxSize.x, (int)m_Params->texMaxSize.y);
					}

					if (CanWeGo == true)
					{
						float sdfRange = m_Params->range / m_Params->scale;

						if (m_Params->algo != AlgoEnum::ALGO_MSDF)
						{
							msdfgen::Bitmap<float> sdf((int)binPackRect.rect->w, (int)binPackRect.rect->h);

							if (m_Params->algo == AlgoEnum::ALGO_SDF)
								generateSDF(sdf, glyph->shape, sdfRange, m_Params->scale, _translate);

							if (m_Params->algo == AlgoEnum::ALGO_PSDF)
								generatePseudoSDF(sdf, glyph->shape, sdfRange, m_Params->scale, _translate);

							binPackRect.sdf = sdf;
						}
						else if (m_Params->algo == AlgoEnum::ALGO_MSDF)
						{
							msdfgen::Bitmap<float, 3> sdf((int)binPackRect.rect->w, (int)binPackRect.rect->h);

							generateMSDF(sdf, glyph->shape, sdfRange, m_Params->scale, _translate);

							binPackRect.msdf = sdf;
						}

						m_Params->m_BinPackRects.push_back(binPackRect);
					}
				}

				index++;
			}

			vWorking = true;

			// on pack
			stbrp_pack_rects(context, rects, num_rects);

			// rendering in second path
			m_Params->charSetNotPacked = "";
			index = 0;
			for (std::list<BinPackRectStruct>::iterator it = m_Params->m_BinPackRects.begin(); 
				it != m_Params->m_BinPackRects.end(); ++it)
			{
				vProgress = 0.8f + (float)index / (float)countGlyph * 0.2f;
				if (!vWorking) break;

				BinPackRectStruct bpr = *it;

				if (bpr.rect->was_packed == 1)
				{
					if (m_Params->algo != AlgoEnum::ALGO_MSDF)
					{
						Upload1ChannelData(&bpr.rect->x, &bpr.rect->y, &bpr.sdf, m_Params->texMaxSize.x);
					}
					else if (m_Params->algo == AlgoEnum::ALGO_MSDF)
					{
						Upload3ChannelData(&bpr.rect->x, &bpr.rect->y, &bpr.msdf, m_Params->texMaxSize.x);
					}

					bpr.glyphStruct->rect.x = bpr.rect->x;
					bpr.glyphStruct->rect.y = bpr.rect->y;
					bpr.glyphStruct->rect.z = bpr.rect->w;
					bpr.glyphStruct->rect.w = bpr.rect->h;

					bpr.glyphStruct->rc.x = (float)bpr.rect->x / (float)m_Params->texMaxSize.x;
					bpr.glyphStruct->rc.y = (float)bpr.rect->y / (float)m_Params->texMaxSize.y;
					bpr.glyphStruct->rc.z = ((float)bpr.rect->x + (float)bpr.rect->w) / (float)m_Params->texMaxSize.x;
					bpr.glyphStruct->rc.w = ((float)bpr.rect->y + (float)bpr.rect->h) / (float)m_Params->texMaxSize.y;
				}
				else
				{
					m_Params->charSetNotPacked += bpr.glyphStruct->c;
				}

				index++;
			}

			// on libere la memoire
			SAFE_DELETE_ARRAY(nodes);
			SAFE_DELETE_ARRAY(rects);
			SAFE_DELETE(context);
			m_Params->m_BinPackRects.clear();

			CreateAndUploadFontTexture(m_Params->texMaxSize);
		}
	}

	vWorking = false;
}

void Generator::UpdateGhars(
	std::atomic< float >& vProgress,
	std::atomic< bool >& vWorking,
	std::atomic< GLuint >& vCurrentTexture,
	std::atomic< float >& vGenerationTime) {
    GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

	if (m_Params->IsFontLoaded && m_Params->textureFont > 0)
	{
		vProgress = 0.0f;

		int count = GenerationThread::Params->ModifiedGlyphs.size();
		int index = 0;
		for (auto it = GenerationThread::Params->ModifiedGlyphs.begin();
			it != GenerationThread::Params->ModifiedGlyphs.end(); ++it)
		{
			if (it->second)
			{
				char c = it->first;

				vProgress = (float)index / (float)count;
				if (!vWorking) return;

				GlyphStruct *glyph = GenerationThread::Params->GetGlyph(c);

				if (glyph)
				{
					if (glyph->shape.validate())
					{
						InvertGlyph(glyph);

						double LARGE_VALUE = 1e8;
						double l = LARGE_VALUE, b = LARGE_VALUE, r = -LARGE_VALUE, t = -LARGE_VALUE;
						glyph->shape.bound(l, b, r, t);
						l -= m_Params->glyphPadding.x / m_Params->scale;
						b -= m_Params->glyphPadding.y / m_Params->scale;
						r += m_Params->glyphPadding.x / m_Params->scale;
						t += m_Params->glyphPadding.y / m_Params->scale;

						msdfgen::Vector2 _translate = msdfgen::Vector2(m_Params->translate.x, m_Params->translate.y);
						msdfgen::Vector2 _dims(r - l, t - b);

						msdfgen::Vector2 _frame(glyph->rect.z, glyph->rect.w);

						if (m_Params->autoFrame)
						{
							_translate = .5*(_frame / m_Params->scale - _dims) - msdfgen::Vector2(l, b);
							glyph->offset = ct::fvec2(_translate.x, _translate.y) / ct::fvec2((int)m_Params->texMaxSize.x, (int)m_Params->texMaxSize.y);
						}

						float sdfRange = m_Params->range / m_Params->scale;

						if (m_Params->algo != AlgoEnum::ALGO_MSDF)
						{
							msdfgen::Bitmap<float> sdf((int)glyph->rect.z, (int)glyph->rect.w);

							if (m_Params->algo == AlgoEnum::ALGO_SDF)
								generateSDF(sdf, glyph->shape, sdfRange, m_Params->scale, _translate);

							if (m_Params->algo == AlgoEnum::ALGO_PSDF)
								generatePseudoSDF(sdf, glyph->shape, sdfRange, m_Params->scale, _translate);

							stbrp_coord x = 0;
							stbrp_coord y = 0;

							CreateImageBuffer(glyph->rect.z, glyph->rect.w);

							Upload1ChannelData(&x, &y, &sdf, glyph->rect.z);

							UpdateFontTexture(glyph->rect.xy(), glyph->rect.zw());

							FreeImageBuffer();
						}
						else if (m_Params->algo == AlgoEnum::ALGO_MSDF)
						{
                            msdfgen::Bitmap<float, 3> sdf((int)glyph->rect.z, (int)glyph->rect.w);

							generateMSDF(sdf, glyph->shape, sdfRange, m_Params->scale, _translate);

							stbrp_coord x = 0;
							stbrp_coord y = 0;

							CreateImageBuffer(glyph->rect.z, glyph->rect.w);

							Upload3ChannelData(&x, &y, &sdf, glyph->rect.z);

							UpdateFontTexture(glyph->rect.xy(), glyph->rect.zw());

							FreeImageBuffer();
						}
					}
				}
				
				index++;
			}
			
			vProgress = 1.0f;
		}
	}

	vWorking = false;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void Generator::InvertGlyph(GlyphStruct *vGlyph)
{
	GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

	std::vector<bool> *signs = &vGlyph->contourSdfSigns;

	int idx = 0;

	for (auto itInv = signs->begin();
		itInv != signs->end(); ++itInv)
	{
		bool b = *itInv;
		if (idx < vGlyph->shape.contours.size())
		{
			msdfgen::Contour *cont = &vGlyph->shape.contours[idx];
			if (b == true)
				cont->sdfSign = -1;
			else
				cont->sdfSign = 1;
		}
		idx++;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
///// TEXTURE METHOD /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

bool Generator::CreateImageBuffer()
{
	GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

	bool res = false;

	if (m_Params->texMaxSize.x < 4096 &&
		m_Params->texMaxSize.y < 4096 &&
		m_Params->bytesPerPixel >= 1 &&
		m_Params->bytesPerPixel <= 4)
	{
		m_Params->m_ImageDataSize =
			(int)m_Params->texMaxSize.x *
			(int)m_Params->texMaxSize.y *
			m_Params->bytesPerPixel;
		m_Params->m_ImageData = new GLubyte[m_Params->m_ImageDataSize];

		// init to a 0 // pour que quoi qu'il arrive le fond soit noir
		for (int i = 0; i<m_Params->m_ImageDataSize; i++)
			m_Params->m_ImageData[i] = (GLubyte)ct::clamp<int>(int(0.0f * 0x100), 0xff);

		res = true;
	}

	return res;
}

bool Generator::CreateImageBuffer(int vWidth, int vHeight)
{
	GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

	bool res = false;

	if (m_Params->m_ImageDataSize > 0 || m_Params->m_ImageData != 0)
	{
		FreeImageBuffer();
	}

	m_Params->m_ImageDataSize =	vWidth * vHeight *	m_Params->bytesPerPixel;
	m_Params->m_ImageData = new GLubyte[m_Params->m_ImageDataSize];

	// init to a 0 // pour que quoi qu'il arrive le fond soit noir
	for (int i = 0; i<m_Params->m_ImageDataSize; i++)
		m_Params->m_ImageData[i] = (GLubyte)ct::clamp<int>(int(0.0f * 0x100), 0xff);

	res = true;

	return res;
}

void Generator::FreeImageBuffer()
{
	GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

	SAFE_DELETE_ARRAY(m_Params->m_ImageData);
	m_Params->m_ImageDataSize = 0;
}

void Generator::Upload1ChannelData(stbrp_coord *vX, stbrp_coord *vY,
	msdfgen::Bitmap<float> *vBuffer, int vMaxWidth)
{
	GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

	if (m_Params->bytesPerPixel == 3 || m_Params->bytesPerPixel == 1)
	{
		if (m_Params->m_ImageDataSize > 0 && m_Params->m_ImageData != 0)
		{
			int w = vBuffer->width();
			int h = vBuffer->height();

			if (w > 0 && h > 0)
			{
				for (int j = 0; j < h; j++)
				{
					for (int i = 0; i < w; i++)
					{
                        float *red = vBuffer->operator()(i, j);

						int xo = i + (int)*vX;
						int yo = j + (int)*vY;

						int o = (xo + yo * vMaxWidth) * m_Params->bytesPerPixel;

						if (o + 2 < m_Params->m_ImageDataSize)
						{
							if (m_Params->bytesPerPixel == 3)
							{
								m_Params->m_ImageData[o + 0] = (GLubyte)ct::clamp<int>(int(red[0] * 0x100), 0xff);
								m_Params->m_ImageData[o + 1] = m_Params->m_ImageData[o + 0];
								m_Params->m_ImageData[o + 2] = m_Params->m_ImageData[o + 1];
							}
							else if (m_Params->bytesPerPixel == 1)
							{
								m_Params->m_ImageData[o + 0] = (GLubyte)ct::clamp<int>(int(red[0] * 0x100), 0xff);
							}
						}
						else
						{
							//LogStr("o + 2 > imageSize");
						}
					}
				}
			}
		}
	}
	else
	{
		LogVarError("msdf only support RGB or ALPHA");
	}
}

void Generator::Upload3ChannelData(stbrp_coord *vX, stbrp_coord *vY,
	msdfgen::Bitmap<float, 3> *vBuffer, int vMaxWidth)
{
	GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

	if (m_Params->bytesPerPixel == 3)
	{
		if (m_Params->m_ImageDataSize > 0 && m_Params->m_ImageData != 0)
		{
			int w = vBuffer->width();
			int h = vBuffer->height();

			if (w > 0 && h > 0)
			{
				for (int j = 0; j < h; j++)
				{
					for (int i = 0; i < w; i++)
					{
                        float* rgb = vBuffer->operator()(i, j);

						int xo = i + (int)*vX;
						int yo = j + (int)*vY;

						int o = (xo + yo * vMaxWidth) * m_Params->bytesPerPixel;

						if (o + 2 < m_Params->m_ImageDataSize)
						{
							if (m_Params->bytesPerPixel == 3)
							{
								m_Params->m_ImageData[o + 0] = (GLubyte)ct::clamp<int>(int(rgb[0] * 0x100), 0xff);
								m_Params->m_ImageData[o + 1] = (GLubyte)ct::clamp<int>(int(rgb[1] * 0x100), 0xff);
								m_Params->m_ImageData[o + 2] = (GLubyte)ct::clamp<int>(int(rgb[2] * 0x100), 0xff);
							}
						}
						else
						{
							//LogStr("o + 2 > imageSize");
						}
					}
				}
			}
		}
	}
	else
	{
		LogVarError("msdf only support RGB");
	}
}

void Generator::CreateAndUploadFontTexture(ct::ivec2 vSize, bool vJustUpdate)
{
	GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

	if (m_Params->m_ImageData != 0 && m_Params->m_ImageDataSize > 0)
	{
		if (!vJustUpdate)
		{
			glBindTexture(GL_TEXTURE_2D, 0);

			if (m_Params->textureFont > 0)
			{
				glDeleteTextures(1, &m_Params->textureFont);
				m_Params->textureFont = 0;
				m_Params->textureSize = 0;
			}

			glGenTextures(1, &m_Params->textureFont);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Params->textureFont);

		if (m_Params->bytesPerPixel == 3)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RGB,
				(GLsizei)vSize.x,
				(GLsizei)vSize.y,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				m_Params->m_ImageData);
		
			m_Params->textureSize = vSize;
		}
		else if (m_Params->bytesPerPixel == 4)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RGBA,
				(GLsizei)vSize.x,
				(GLsizei)vSize.y,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				m_Params->m_ImageData);

			m_Params->textureSize = vSize;
		}
		else if (m_Params->bytesPerPixel == 1)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_ALPHA,
				(GLsizei)vSize.x,
				(GLsizei)vSize.y,
				0,
				GL_ALPHA,
				GL_UNSIGNED_BYTE,
				m_Params->m_ImageData);

			m_Params->textureSize = vSize;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Generator::UpdateFontTexture(ct::fvec2 vOffset, ct::fvec2 vSize)
{
	GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

	if (m_Params->m_ImageData != 0 && m_Params->m_ImageDataSize > 0 && m_Params->textureFont > 0)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Params->textureFont);
		
		if (m_Params->bytesPerPixel == 3)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexSubImage2D(GL_TEXTURE_2D,
				0,
				(GLsizei)vOffset.x,
				(GLsizei)vOffset.y,
				(GLsizei)vSize.x,
				(GLsizei)vSize.y,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				m_Params->m_ImageData);
		}
		else if (m_Params->bytesPerPixel == 4)
		{
			glTexSubImage2D(GL_TEXTURE_2D,
				0,
				(GLsizei)vOffset.x,
				(GLsizei)vOffset.y,
				(GLsizei)vSize.x,
				(GLsizei)vSize.y,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				m_Params->m_ImageData);
		}
		else if (m_Params->bytesPerPixel == 1)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexSubImage2D(GL_TEXTURE_2D,
				0,
				(GLsizei)vOffset.x,
				(GLsizei)vOffset.y,
				(GLsizei)vSize.x,
				(GLsizei)vSize.y,
				GL_ALPHA,
				GL_UNSIGNED_BYTE,
				m_Params->m_ImageData);
		}

		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenerateMipmap(GL_TEXTURE_2D);*/

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

bool Generator::SaveFontTextureToPng(std::string vFilePathName)
{
	GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

	int res = stbi_write_png(vFilePathName.c_str(),
		m_Params->texMaxSize.x,
		m_Params->texMaxSize.y,
		m_Params->bytesPerPixel,
		m_Params->m_ImageData,
		m_Params->texMaxSize.x * m_Params->bytesPerPixel);

	return res == 0;
}
