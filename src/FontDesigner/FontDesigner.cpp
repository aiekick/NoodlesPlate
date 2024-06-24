#include <FontDesigner/FontDesigner.h>
#include <Backends/MainBackend.h>
#include <Gui/GuiBackend.h>

bool FontDesigner::init(CodeTreeWeak vCodeTree) {
    GenerationThread::sGenerationThread = GuiBackend::Instance()->CreateGuiBackendWindow_Hidden(1, 1, "SdfFontDesigner", MainBackend::sMainThread);
    m_FontExplorerPtr = std::make_unique<FontExplorer>();
    m_FontExplorerPtr->Init();
    return false;
}

void FontDesigner::unit() {
    m_FontExplorerPtr.reset();
}

void FontDesigner::enable(const bool& vEnabled) {
    m_isEnabled = vEnabled;
}

const bool& FontDesigner::isEnabled() {
    return m_isEnabled;
}

void FontDesigner::drawPane() {
    m_displaySystemFontExplorer();
    m_diplayGenerationThread();
    m_displayCharSet();
    m_displayParams();
    m_displayGlyphSelection();
    m_displayGlyphInfos();
    m_displayTexture();
}

void FontDesigner::drawOverlay() {
    /*IsThereSomeMessages();

    if (m_IsThereSomeShaderErrors && m_ShowErrors && GenerationThread::Params->textureFont > 0) {
        DisplayMessages(vX, vY, vWidth, vHeight);
    } else if (m_IsThereSomeShaderWarnings && m_ShowWarnings && GenerationThread::Params->textureFont > 0) {
        DisplayMessages(vX, vY, vWidth, vHeight);
    } else {
        ImVec4 color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, color);

        ImGui::Begin("CentralView", (bool*)0, flags);

        ImGui::SetWindowSize(ImVec2(vWidth, vHeight));
        ImGui::SetWindowPos(ImVec2(vX, vY));

        if (ImGui::BeginTabBar("##font_Map_Test_tabs", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
            if (ImGui::BeginTabItem("Font Map", 0, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton)) {
                MainFrame::g_GizmoWindowHovered |= ImGui::IsWindowHovered();

                float thick = 10.0f;
                float y = ImGui::GetCursorPosY() + thick * 2.0f;

                ImVec2 windowSize = ImVec2(vWidth, vHeight - y);
                ImVec2 windowOrigin = ImVec2(0.0f, y - thick);

                float ratioX = 1.0f;
                if (GenerationThread::Params->textureFont) {
                    ratioX = GenerationThread::Params->textureSize.ratioXY();
                } else {
                    ratioX = GenerationThread::Params->texMaxSize.ratioXY();
                }

                float newX = windowSize.y * ratioX;
                float newY = windowSize.x / ratioX;
                if (newX < windowSize.x) {
                    m_ImageSize.x = newX;
                    m_ImageSize.y = windowSize.y;
                    m_ImageOrigin.x = windowOrigin.x + (windowSize.x - newX) * 0.5f;
                    m_ImageOrigin.y = windowOrigin.y;
                } else {
                    m_ImageSize.x = windowSize.x;
                    m_ImageSize.y = newY;
                    m_ImageOrigin.x = windowOrigin.x;
                    m_ImageOrigin.y = windowOrigin.y + (windowSize.y - newY) * 0.5f;
                }

                if (GenerationThread::Params->textureFont > 0) {
                    ImVec2 cursorPos = ct::toImVec2(m_ImageOrigin);
                    ImVec2 imageSize = ct::toImVec2(m_ImageSize);

                    ImGui::SetCursorPos(cursorPos);

                    ImTextureID textureId = (ImTextureID)GenerationThread::Params->textureFont;

                    if (m_Font_RenderPack->GetShader() != 0) {
                        int id = m_Font_RenderPack->GetTextureId(m_PreviewBufferId);
                        if (id >= 0)
                            textureId = (ImTextureID)id;
                    }

                    cursorPos = ImGui::GetCursorScreenPos();

                    // image
                    DrawFontTexture(textureId, cursorPos, imageSize, TRANSPARENCY_DAMIER_ZOOM);

                    ImVec2 getMousePosValue = ImGui::GetMousePos();

                    ct::fvec2 mp = getMousePosValue - cursorPos;
                    m_LastMouseRect.x = mp.x;
                    m_LastMouseRect.y = mp.y;
                    m_LastMouseRect.w = imageSize.x;
                    m_LastMouseRect.h = imageSize.y;

                    CalcCamera((getMousePosValue - cursorPos) / imageSize);

                    // selection
                    if (ImGui::IsMouseHoveringWindow()) {
                        if (ImGui::IsMouseClicked(0)) {
                            ct::fvec2 mousePos = (getMousePosValue - cursorPos) / imageSize;

                            bool found = false;
                            for (auto it = GenerationThread::Params->Glyphs.begin(); it != GenerationThread::Params->Glyphs.end(); ++it) {
                                auto glyph = &it->second;

                                ct::fvec4 rc = glyph->rc;
                                if (mousePos.x > rc.x && mousePos.y > rc.y && mousePos.x < rc.z && mousePos.y < rc.w) {
                                    if (ImGui::GetIO().KeyCtrl) {
                                        if (m_MultiSelection.find(glyph->c) != m_MultiSelection.end()) {
                                            m_MultiSelection.erase(glyph->c);
                                        } else {
                                            if (m_CurrentSelectedChar != 0) {
                                                m_MultiSelection[m_CurrentSelectedChar] = true;
                                                m_CurrentSelectedChar = 0;
                                            }
                                            m_MultiSelection[glyph->c] = true;
                                        }
                                    } else {
                                        m_MultiSelection.clear();

                                        m_CurrentSelectedChar = glyph->c;

                                        ct::fvec2 cellSize = rc.zw() - rc.xy();
                                        ct::fvec2 mp = (mousePos - rc.xy());
                                        ct::fvec2 uv = mp / cellSize;

                                        if (m_CanTuneUVOffset) {
                                            glyph->shaderCenterOffset = uv;
                                            UpdateGlyphCenterOffsets();
                                        }
                                    }

                                    found = true;
                                }
                            }

                            if (found == false && !ImGui::GetIO().KeyCtrl) {
                                ResetSelection();
                            }
                        }

                        if (ImGui::IsMouseDown(0) && m_CurrentSelectedChar != 0) {
                            auto glyph = GenerationThread::Params->GetGlyph(m_CurrentSelectedChar);
                            if (glyph != 0) {
                                ct::fvec2 mousePos = (ImGui::GetMousePos() - cursorPos) / imageSize;
                                ct::fvec4 rc = glyph->rc;
                                ct::fvec2 cellSize = rc.zw() - rc.xy();
                                ct::fvec2 mp = (mousePos - rc.xy());
                                ct::fvec2 uv = mp / cellSize;

                                if (m_CanTuneUVOffset) {
                                    glyph->shaderCenterOffset = uv;
                                    UpdateGlyphCenterOffsets();
                                }
                            }
                        }
                    }
                } else {
                    if (m_CurrentNotLoadedFont.size() > 0) {
                        ImGui::Text("Fail to Load this font, but maybe MsdfGenn can work !");
                        ImGui::Text("Try to Generate the Sdf Font However");
                    } else {
                        // ImGuiStyle& style = ImGui::GetStyle();
                        ImFontAtlas* atlas = ImGui::GetIO().Fonts;

                        ImFont* font = atlas->Fonts[0];

                        if (atlas->Fonts.Size > 1)
                            font = atlas->Fonts[1];

                        ImGui::PushFont(font);

                        ImGui::Text(charSetBuffer);

                        ImGui::PopFont();

                        // Display all glyphs of the fonts in separate pages of 256 characters
                        // Forcefully/dodgily make FindGlyph() return NULL on fallback, which isn't the default behavior.
                        const ImFontGlyph* glyph_fallback = font->FallbackGlyph;
                        font->FallbackGlyph = NULL;
                        for (int base = 0; base < 0x10000; base += 256) {
                            int count = 0;
                            for (int n = 0; n < 256; n++)
                                count += font->FindGlyph((ImWchar)(base + n)) ? 1 : 0;
                            if (count > 0) {
                                float cell_spacing = 1;
                                ImVec2 cell_size(vWidth / 17, vWidth / 17);
                                ImVec2 base_pos = ImGui::GetCursorScreenPos();
                                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                                for (int n = 0; n < 256; n++) {
                                    ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size.x + cell_spacing), base_pos.y + (n / 16) * (cell_size.y + cell_spacing));
                                    ImVec2 cell_p2(cell_p1.x + cell_size.x, cell_p1.y + cell_size.y);
                                    const ImFontGlyph* glyph = font->FindGlyph((ImWchar)(base + n));
                                    ;
                                    draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
                                    // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion
                                    // functions available to generate a string.
                                    font->RenderChar(draw_list, cell_size.x, cell_p1, ImGui::GetColorU32(ImGuiCol_Text), (ImWchar)(base + n));
                                }
                                ImGui::Dummy(ImVec2((cell_size.x + cell_spacing) * 16, (cell_size.y + cell_spacing) * 16));
                            }
                        }
                        font->FallbackGlyph = glyph_fallback;
                    }
                }

                ImGui::EndTabItem();
            }

            if (GenerationThread::Params->textureFont > 0) {
                if (ImGui::BeginTabItem("Font Test", 0, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton)) {
                    UpdateFontTexFromFBO();

                    ImGui::ColorEdit3("Font Test BackGround", &m_FontTestBackgroundColor.x, ImGuiColorEditFlags_NoInputs);

                    bool res =
                        ImGui::InputInt(vWidth - 80, "Base Height", &GenerationThread::Params->baseHeight, 1, 2, GenerationThread::Params->defaultBaseHeight);
                    res |=
                        ImGui::InputInt(vWidth - 80, "Line Height", &GenerationThread::Params->lineHeight, 1, 2, GenerationThread::Params->defaultLineHeight);
                    if (res) {
                        LoadBitmapFontIntoImguiFont();
                    }
                    ImGui::SliderFloat(vWidth - 80, "Font Scale", &m_TestFont->Scale, 0.1f, 10.0f, 1.0f);

                    ImGui::InputTextMultiline("##TestBuffer", fontTestBuffer, 1024, ImVec2(-1, 100));

                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(m_FontTestBackgroundColor.x, m_FontTestBackgroundColor.y, m_FontTestBackgroundColor.z, 1.0f));

                    ImGui::BeginChild("##FontTestChildWindow", ImVec2(-1, -1));

                    ImGui::PushFont(m_TestFont);

                    ImGui::TextWrapped(fontTestBuffer);

                    ImGui::PopFont();

                    ImGui::EndChild();

                    ImGui::PopStyleColor();

                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

        ImGui::PopStyleColor();
    }*/
}

void FontDesigner::ComputeKey(ShaderKeyWeak vShaderkey) {
    auto ptr = vShaderkey.lock();
    if (ptr != nullptr) {
        auto exist = ptr->IsShaderStageNameExist("FONT");
        if (exist) {
            // we need to insert the font stage in the fragment stage
            // for the compilation suceeed
            auto* codeTreePtr = ptr->puParentCodeTree.get();
            if (codeTreePtr != nullptr) {
                //codeTreePtr->AddOrUpdateFromString("")
            }
        }
        enable(exist);
    }
}

std::string FontDesigner::InitRenderPack(const GuiBackend_Window& vWin, CodeTreePtr vCodeTree) {
    if (vCodeTree) {
        auto shader_string = m_GetBaseShaderString();
        m_BaseKeyPtr = vCodeTree->LoadFromString("FontDesignerBaseCode", shader_string, "FontDesignerBaseCode.glsl", "", KEY_TYPE_Enum::KEY_TYPE_SHADER);
        m_BaseRenderPackPtr =
            RenderPack::createBufferWithFileWithoutLoading(vWin, "FontDesignerBase", ct::ivec3(1024, 1024, 0), m_BaseKeyPtr, false, true);
    }

    return {};
}

bool FontDesigner::LoadRenderPack() {
    if (m_BaseRenderPackPtr) {
        return m_BaseRenderPackPtr->Load();
    }
    return false;
}

void FontDesigner::SaveRenderPack() {

}

void FontDesigner::FinishRenderPack() {
    if (m_BaseRenderPackPtr) {
        m_BaseRenderPackPtr->Finish(false);
    }
}

void FontDesigner::DestroyRenderPack() {
    m_BaseRenderPackPtr.reset();
}

RenderPackWeak FontDesigner::GetRenderPack() {
    return m_BaseRenderPackPtr;
}

ShaderKeyPtr FontDesigner::GetShaderKey() {
    return m_BaseKeyPtr;
}

void FontDesigner::m_displaySystemFontExplorer() {
    if (ImGui::CollapsingHeader("Font Selection")) {
        if (ImGui::BeginTabBar("##font_tabs", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
            if (ImGui::BeginTabItem("Font From System", 0, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton)) {
                GenerationThread::Params->FontLoadingMode = FontLoadingModeEnum::FONT_LOADING_FROM_SYSTEM;
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Font From File", 0, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton)) {
                GenerationThread::Params->FontLoadingMode = FontLoadingModeEnum::FONT_LOADING_FROM_FILE;
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        if (GenerationThread::Params->FontLoadingMode == FontLoadingModeEnum::FONT_LOADING_FROM_SYSTEM) {
            m_FontFilePathName = "";

            if (m_CurrentSelectedFont.size() == 0) {
                if (m_FontExplorerPtr->GetSysFontsDBSize() > 0) {
                    auto iter = m_FontExplorerPtr->begin();
                    std::string fontToLoad = iter->first;
                    SetFontToLoad(fontToLoad);
                }
            }

            if (ImGui::ContrastedButton("Goto Selected Font")) {
                m_GotoSelectedFontName = true;
            }

            ImGui::Separator();

            ImGui::Checkbox("Only Font with Existing Config", &m_showOnlyFontWithExistingConfig);

            ImGui::Separator();

            m_SearchInput.DisplayInputText(0.0f, "Search", "");

            ImGui::Separator();

            ImGui::Text("System Fonts :");

            if (m_FontExplorerPtr->GetSysFontsDBSize() > 0) {
                ImGui::BeginChild("##Font List");

                for (const auto& font : *m_FontExplorerPtr.get()) {
                    std::string fontName = font.first;
                    //			std::string fontPath = itFonts->second;

                    bool showItem = false;

                    if (m_SearchInput.GetText().size() > 0) {
                        if (fontName.find(m_SearchInput.GetText()) != std::string::npos) {
                            showItem = true;
                        }
                    } else {
                        showItem = true;
                    }

                    if (m_showOnlyFontWithExistingConfig) {
                        if (!font.second.confExist)
                            showItem = false;
                    }

                    if (showItem == true) {
                        if (m_GotoSelectedFontName && fontName == m_CurrentSelectedFont) {
                            ImGui::SetScrollHereY(0.25f);
                            m_GotoSelectedFontName = false;
                        }

                        if (font.second.confExist) {
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.25f, 0.85f, 1.00f));
                        }

                        if (ImGui::Selectable(fontName.c_str(), (fontName == m_CurrentSelectedFont))) {
                            SetFontToLoad(fontName);
                        }

                        if (font.second.confExist) {
                            ImGui::PopStyleColor();
                        }
                    }
                }
                ImGui::EndChild();
            }
        } else if (GenerationThread::Params->FontLoadingMode == FontLoadingModeEnum::FONT_LOADING_FROM_FILE) {
            /*m_FontToLoad = "";
            if (ImGui::ContrastedButton("Open Font File")) {
                ImGuiFileDialog::Instance()->OpenDialog("OpenFontFileDialog", "New File", ".ttf\0.otf\0\0", m_FilePath, m_FileName);
            }*/
        }
    }
}

void FontDesigner::m_diplayGenerationThread() {
    if (ImGui::CollapsingHeader("Generation")) {
        GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

        if (!m_GenerationThread.IsJoinable()) {
            if (ImGui::ContrastedButton("Generate SDF Font Map [F5]") || ImGui::IsKeyReleased(ImGuiKey_F5)) {
                m_ThreadToStart = true;
                m_ThreadToStop = false;
                m_JustUpdate = false;
            }

            if (GenerationThread::Params->Glyphs.size() > 0 && m_NeedRegeneration == false) {
                ImGui::SameLine();
                if (ImGui::ContrastedButton("Update Chars [F6]") || ImGui::IsKeyReleased(ImGuiKey_F6)) {
                    m_ThreadToStart = true;
                    m_ThreadToStop = false;
                    m_JustUpdate = true;
                }
            }
        } else {
            if (ImGui::ContrastedButton("Stop Generation [F4]") || ImGui::IsKeyReleased(ImGuiKey_F4)) {
                m_ThreadToStop = true;
                m_ThreadToStart = false;
            }

            ImGui::SameLine();
            ImGui::ProgressBar(GenerationThread::Progress, ImVec2(200, 0));
        }

        if (GenerationThread::Params->charSetNotPacked.size() > 0) {
            ImGui::Separator();

            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Chars not packed : %s", GenerationThread::Params->charSetNotPacked.c_str());
        }
    }
}

void FontDesigner::m_displayCharSet() {
    if (ImGui::CollapsingHeader("Char Set")) {
        if (ImGui::ContrastedButton("Clear")) {
            m_CharSetInput.Clear();
            GenerationThread::Params->IsFontLoaded = false;
            m_NeedRegeneration = true;
        }
        ImGui::SameLine();
        if (ImGui::ContrastedButton("A..Z")) {
            m_CharSetInput.AddText("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            GenerationThread::Params->IsFontLoaded = false;
            m_NeedRegeneration = true;
        }
        ImGui::SameLine();
        if (ImGui::ContrastedButton("a..z")) {
            m_CharSetInput.AddText("abcdefghijklmnopqrstuvwxyz");
            GenerationThread::Params->IsFontLoaded = false;
            m_NeedRegeneration = true;
        }
        ImGui::SameLine();
        if (ImGui::ContrastedButton("0..9")) {
            m_CharSetInput.AddText("0123456789");
            GenerationThread::Params->IsFontLoaded = false;
            m_NeedRegeneration = true;
        }
        ImGui::SameLine();
        if (ImGui::ContrastedButton("?..@")) {
            m_CharSetInput.AddText("?.*+-/_'()&:,=!\"[ ]\\{}%@");
            GenerationThread::Params->IsFontLoaded = false;
            m_NeedRegeneration = true;
        }

        if (m_CharSetInput.DisplayInputText(0.0f, "CharSet", "", true)) {
            GenerationThread::Params->IsFontLoaded = false;
            m_NeedRegeneration = true;
        }
    }

    GenerationThread::Params->fontName = m_CurrentSelectedFont;
}

void FontDesigner::m_displayParams() {
    /*if (ImGui::CollapsingHeader("Params")) {
        bool res = false;

        ImGui::Separator();

        res |= ImGui::InputFloat(281.0f, " Scale", &GenerationThread::Params->scale, defaultparams.scale, "%.3f", "%.3f", true, 0.1f, 0.5f);

        if (GenerationThread::Params->scale < 0.001f)
            GenerationThread::Params->scale = 0.001f;

        ImGui::Separator();

        res |= ImGui::RadioButtonLabeled("Auto Frame", 0, &GenerationThread::Params->autoFrame);

        if (GenerationThread::Params->autoFrame == false) {
            res |= ImGui::InputInt(305.0f, " Translate X", &GenerationThread::Params->translate.x, 1, 1, defaultparams.translate.x);
            res |= ImGui::InputInt(305.0f, " Translate Y", &GenerationThread::Params->translate.y, 1, 1, defaultparams.translate.y);
        }

        ImGui::Separator();

        res |= ImGui::RadioButtonLabeled("Auto Size", 0, &GenerationThread::Params->autoSize);

        if (GenerationThread::Params->autoSize == false) {
            res |= ImGui::InputInt(305.0f, " Glyph Width", &GenerationThread::Params->glyphSize.x, 1, 1, defaultparams.glyphSize.x);
            res |= ImGui::InputInt(305.0f, " Glyph Height", &GenerationThread::Params->glyphSize.y, 1, 1, defaultparams.glyphSize.y);

            ImGui::Separator();
        }

        res |= ImGui::InputInt(305.0f, " Tex Max Width", &GenerationThread::Params->texMaxSize.x, 1, 1, defaultparams.texMaxSize.x);
        res |= ImGui::InputInt(305.0f, " Tex Max Height", &GenerationThread::Params->texMaxSize.y, 1, 1, defaultparams.texMaxSize.y);

        ImGui::Separator();

        res |= ImGui::InputInt(305.0f, " Glyph Padding x", &GenerationThread::Params->glyphPadding.x, 1, 1, defaultparams.glyphPadding.x);
        res |= ImGui::InputInt(305.0f, " Glyph Padding y", &GenerationThread::Params->glyphPadding.y, 1, 1, defaultparams.glyphPadding.y);

        // ImGui::Separator();

        // res |= ImGui::InputInt(305.0f, " Texture Padding x", &GenerationThread::Params->texPadding.x, 1, 1, defaultparams.texPadding.x);
        // res |= ImGui::InputInt(305.0f, " Texture Padding y", &GenerationThread::Params->texPadding.y, 1, 1, defaultparams.texPadding.y);

        ImGui::Separator();

        ImGui::Text("Algo (Use of MsdfGen) :");

        if (ImGui::RadioButtonLabeled("Sdf", 0, GenerationThread::Params->algo == AlgoEnum::ALGO_SDF))
            res |= true, GenerationThread::Params->algo = AlgoEnum::ALGO_SDF;
        ImGui::SameLine();
        if (ImGui::RadioButtonLabeled("PSdf", 0, GenerationThread::Params->algo == AlgoEnum::ALGO_PSDF))
            res |= true, GenerationThread::Params->algo = AlgoEnum::ALGO_PSDF;
        ImGui::SameLine();
        if (ImGui::RadioButtonLabeled("MSdf", 0, GenerationThread::Params->algo == AlgoEnum::ALGO_MSDF))
            res |= true, GenerationThread::Params->algo = AlgoEnum::ALGO_MSDF;

        ImGui::Separator();

        if (GenerationThread::Params->algo == AlgoEnum::ALGO_MSDF) {
            res |= ImGui::SliderFloat(281.0f,
                                      " MSDF Edge Color Angle",
                                      &GenerationThread::Params->msdfColoringAngle,
                                      0.0f,
                                      360.0f,
                                      defaultparams.msdfColoringAngle,
                                      1.0f,
                                      10.0f,
                                      "%.0f",
                                      0,
                                      true);
        }

        res |= ImGui::InputFloat(281.0f, " Sdf Range", &GenerationThread::Params->range, defaultparams.range, "%.1f", "%.1f", 0.1f, 1.0f);

        if (GenerationThread::Params->range < 0.001f)
            GenerationThread::Params->range = 0.001f;

        ImGui::Separator();

        res |= ImGui::RadioButtonLabeled("Invert Y", 0, &GenerationThread::Params->InvertY);
    }*/
}

void FontDesigner::m_displayGlyphSelection() {
    /*if (m_MultiSelection.size() > 0) {
        if (ImGui::CollapsingHeader("Multi Selection", -1.0f, true)) {
            if (ImGui::ContrastedButton("Reset Selection")) {
                m_MultiSelection.clear();
            }

            ImGui::Text("UV Offset :");

            bool resetUVCenters = ImGui::ContrastedButton("Reset UV Offset of Selection");

            if (resetUVCenters) {
                auto uniLst = m_Font_RenderPack->GetShaderKey()->GetUniformsByWidget("glyphcenter");
                if (uniLst) {
                    for (auto itSel = m_MultiSelection.begin(); itSel != m_MultiSelection.end(); ++itSel) {
                        auto glyph = GenerationThread::Params->GetGlyph(itSel->first);
                        if (glyph) {
                            if (resetUVCenters) {
                                glyph->shaderCenterOffset = GlyphStruct().shaderCenterOffset;
                            }
                        }
                    }

                    if (resetUVCenters) {
                        UpdateGlyphCenterOffsets();
                    }
                }
            }

            ImGui::Text("Invert the SDF One Time Via Uniforms:");

            bool invertAllOneTime = ImGui::ContrastedButton("Invert all selection");
            ImGui::SameLine();
            bool resetInversion = ImGui::ContrastedButton("Reset Inversion");

            if (resetInversion || invertAllOneTime) {
                auto uniLst = m_Font_RenderPack->GetShaderKey()->GetUniformsByWidget("glyphinversions");
                if (uniLst) {
                    for (auto itSel = m_MultiSelection.begin(); itSel != m_MultiSelection.end(); ++itSel) {
                        auto glyph = GenerationThread::Params->GetGlyph(itSel->first);
                        if (glyph) {
                            if (invertAllOneTime) {
                                glyph->dfInverted = !glyph->dfInverted;
                            } else if (resetInversion) {
                                glyph->dfInverted = false;
                            }

                            for (auto itUniLst = uniLst->begin(); itUniLst != uniLst->end(); itUniLst++) {
                                auto v = *itUniLst;
                                if (v) {
                                    if (glyph->dfInverted)
                                        v->uFloatArr[glyph->idx] = 1.0f;
                                    else
                                        v->uFloatArr[glyph->idx] = 0.0f;
                                }
                            }
                        }
                    }
                }
            }

            ImGui::Text("Char Set :");

            bool removeFromCharSet = ImGui::ContrastedButton("Remove from Char Set");

            ImGui::Text("Need a new generation, for reflect chnages");

            if (removeFromCharSet) {
                std::string _charSet = std::string(charSetBuffer);

                for (auto itSel = m_MultiSelection.begin(); itSel != m_MultiSelection.end(); ++itSel) {
                    auto glyph = GenerationThread::Params->GetGlyph(itSel->first);
                    if (glyph) {
                        // on supprime les char selectionn� du charset
                        ct::replaceString(_charSet, ct::toStr(glyph->c), "");
                    }
                }

                // on applique le nouveau charset
                ct::ResetBuffer(charSetBuffer);
                ct::AppendToBuffer(charSetBuffer, MAX_CHARSET_SIZE, _charSet);
                GenerationThread::Params->IsFontLoaded = false;
                m_NeedRegeneration = true;
            }
        }
    }*/

    /////////////////////////////////////////////////////////////////////////
    //// SELECTED GLYPH /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////

    /*else {
        if (GenerationThread::Params->Glyphs.size() > 0 && m_CurrentSelectedChar != 0) {
            if (ImGui::CollapsingHeader("Selected Glyph", -1.0f, true)) {
                ImGui::ColorEdit4("Modification Color", &m_ModificationColor.x);
                ImGui::ColorEdit4("Selection Color", &m_SelectionColor.x);

                if (ImGui::ContrastedButton("Reset Selection")) {
                    m_CurrentSelectedChar = 0;
                }

                if (m_CurrentSelectedChar != 0) {
                    auto glyph = GenerationThread::Params->GetGlyph(m_CurrentSelectedChar);
                    if (glyph) {
                        ImGui::Text("Selected char : %c", glyph->c);

                        ImGui::NewLine();

                        ImGui::Text("Shader UV Center offset : %.4f, %.4f", glyph->shaderCenterOffset.x, glyph->shaderCenterOffset.y);
                        if (ImGui::ContrastedButton("Reset UV Offset")) {
                            glyph->shaderCenterOffset = GlyphStruct().shaderCenterOffset;
                            UpdateGlyphCenterOffsets();
                        }

                        ImGui::NewLine();

                        ImGui::Text("Invert the Glyph (no update needed) :");

                        bool invertAllOneTime = ImGui::ContrastedButton("Invert all selection");
                        ImGui::SameLine();
                        bool resetInversion = ImGui::ContrastedButton("Reset Inversion");

                        if (resetInversion || invertAllOneTime) {
                            if (invertAllOneTime) {
                                glyph->dfInverted = !glyph->dfInverted;
                            } else if (resetInversion) {
                                glyph->dfInverted = false;
                            }

                            auto uniLst = m_Font_RenderPack->GetShaderKey()->GetUniformsByWidget("glyphinversions");
                            if (uniLst) {
                                for (auto itUniLst = uniLst->begin(); itUniLst != uniLst->end(); itUniLst++) {
                                    auto v = *itUniLst;
                                    if (v) {
                                        if (glyph->dfInverted)
                                            v->uFloatArr[glyph->idx] = 1.0f;
                                        else
                                            v->uFloatArr[glyph->idx] = 0.0f;
                                    }
                                }
                            }
                        }

                        ImGui::Text("Or You can Select Contours you want to invert (need chars Update) :");

                        bool res = false;
                        int idToPush = 457954654;
                        auto* itGlyphs = &glyph->contourSdfSigns;
                        if (itGlyphs) {
                            for (auto itInv = itGlyphs->begin(); itInv != itGlyphs->end(); ++itInv) {
                                if (itInv != itGlyphs->begin())
                                    ImGui::SameLine();
                                bool b = *itInv;
                                ImGui::PushID(idToPush++);
                                res |= ImGui::Checkbox("##checkinvsdf", &b);
                                ImGui::PopID();
                                if (res) {
                                    GenerationThread::Params->ModifiedGlyphs[glyph->c] = true;
                                }
                                (*itInv) = b;
                            }
                        }
                        ImGui::Text(
                            "If not Suffiscient for correct the font map,\n try invert the texture cell color\ninside the shader by using [[char]] for conditionnal on a "
                            "particular char");

                        ImGui::Text("Char Set :");

                        bool removeFromCharSet = ImGui::ContrastedButton("Remove from Char Set");

                        ImGui::Text("Need a new generation, for reflect chnages");

                        if (removeFromCharSet) {
                            std::string _charSet = std::string(charSetBuffer);

                            // on supprime les char selectionn� du charset
                            ct::replaceString(_charSet, ct::toStr(glyph->c), "");

                            // on applique le nouveau charset
                            ct::ResetBuffer(charSetBuffer);
                            ct::AppendToBuffer(charSetBuffer, MAX_CHARSET_SIZE, _charSet);
                            GenerationThread::Params->IsFontLoaded = false;
                            m_NeedRegeneration = true;
                        }
                    }
                }
            }
        }
    }*/
}

void FontDesigner::m_displayGlyphInfos() {
    /*if (ImGui::CollapsingHeader("Params")) {
        if (GenerationThread::Params->Glyphs.size() > 0) {
            if (ImGui::CollapsingHeader("Infos")) {
                ImGui::Text("Font : name:%s", GenerationThread::Params->fontName.c_str());
                ImGui::Text("Font : path:%s", GenerationThread::Params->fontPath.c_str());

                int lh = GenerationThread::Params->lineHeight * GenerationThread::Params->scale;
                int bh = GenerationThread::Params->baseHeight * GenerationThread::Params->scale;
                ImGui::Text("Font : lineHeight:%i baseHeight:%i", lh, bh);

                int w = GenerationThread::Params->textureSize.x;
                int h = GenerationThread::Params->textureSize.y;
                ImGui::Text("Tex : w:%i h:%i", w, h);

                static int ShowGlyphMode = false;
                if (ImGui::RadioButtonLabeled("Glyph Rects", 0, ShowGlyphMode == 0))
                    ShowGlyphMode = 0;
                ImGui::SameLine();
                if (ImGui::RadioButtonLabeled("Glyph Infos", 0, ShowGlyphMode == 1))
                    ShowGlyphMode = 1;
                ImGui::SameLine();
                if (ImGui::RadioButtonLabeled("Glyph Select", 0, ShowGlyphMode == 2))
                    ShowGlyphMode = 2;

                int idx = 0;
                for (auto itGlyphs = GenerationThread::Params->Glyphs.begin(); itGlyphs != GenerationThread::Params->Glyphs.end(); ++itGlyphs) {
                    if (ShowGlyphMode == 0) {
                        ImGui::Text("%3i %c (%i) l:%.3f b:%.3f r:%.3f t:%.3f",
                                    idx++,
                                    itGlyphs->second.c,
                                    (int)itGlyphs->second.c,
                                    itGlyphs->second.rc.x,
                                    itGlyphs->second.rc.y,
                                    itGlyphs->second.rc.z,
                                    itGlyphs->second.rc.w);
                    } else if (ShowGlyphMode == 1) {
                        int px = itGlyphs->second.rc.x * w;
                        int py = itGlyphs->second.rc.y * h;
                        int psx = (itGlyphs->second.rc.z - itGlyphs->second.rc.x) * w;
                        int psy = (itGlyphs->second.rc.w - itGlyphs->second.rc.y) * h;
                        int sx = itGlyphs->second.size.x * GenerationThread::Params->scale;
                        int sy = itGlyphs->second.size.y * GenerationThread::Params->scale;

                        int ox = itGlyphs->second.offset.x * w;
                        int oy = itGlyphs->second.offset.y * h;
                        int hbx = itGlyphs->second.horiBearing.x * GenerationThread::Params->scale;
                        int hby = itGlyphs->second.horiBearing.y * GenerationThread::Params->scale;

                        int advx = (itGlyphs->second.advx + GenerationThread::Params->glyphPadding.x) * GenerationThread::Params->scale;

                        ImGui::Text("%3i %c (%3i) sx:%3i sy:%3i hbx:%3i hby:%3i adv:%3i", idx++, itGlyphs->second.c, (int)itGlyphs->second.c, sx, sy, hbx, hby, advx);
                    } else if (ShowGlyphMode == 2) {
                        if (idx % 15 > 0)
                            ImGui::SameLine();

                        static char selectCharBuffer[1000];
                        snprintf(selectCharBuffer, 1000, "%c##%3i", itGlyphs->second.c, idx++);
                        if (ImGui::RadioButtonLabeled(selectCharBuffer, 0, m_CurrentSelectedChar == itGlyphs->second.c)) {
                            m_CurrentSelectedChar = itGlyphs->second.c;
                        }
                    }
                }
            }
        }
    }*/
}

void FontDesigner::m_displayTexture() {
    if (GenerationThread::Params->textureFont > 0) {
        if (ImGui::CollapsingHeader("Font Texture")) {
            const auto w = ImGui::GetContentRegionAvail().x * 0.95f;
            const float h = w * GenerationThread::Params->textureSize.ratioYX<float>();
            ImGui::Image((ImTextureID)GenerationThread::Params->textureFont, ImVec2(w, h));
        }
    }
}

void FontDesigner::m_displayAllGlyphsOfFont(ImFont* vFontPtr, const float& vWidth) {
    if (vFontPtr != nullptr) {
        // Display all glyphs of the fonts in separate pages of 256 characters
        // Forcefully/dodgily make FindGlyph() return NULL on fallback, which isn't the default behavior.
        const ImFontGlyph* glyph_fallback = vFontPtr->FallbackGlyph;
        vFontPtr->FallbackGlyph = NULL;
        for (int base = 0; base < 0x10000; base += 256) {
            int count = 0;
            for (int n = 0; n < 256; n++)
                count += vFontPtr->FindGlyph((ImWchar)(base + n)) ? 1 : 0;
            if (count > 0) {
                float cell_spacing = 1;
                ImVec2 cell_size(vWidth / 17, vWidth / 17);
                ImVec2 base_pos = ImGui::GetCursorScreenPos();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                for (int n = 0; n < 256; n++) {
                    ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size.x + cell_spacing), base_pos.y + (n / 16) * (cell_size.y + cell_spacing));
                    ImVec2 cell_p2(cell_p1.x + cell_size.x, cell_p1.y + cell_size.y);
                    const ImFontGlyph* glyph = vFontPtr->FindGlyph((ImWchar)(base + n));
                    ;
                    draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
                    // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion
                    // functions available to generate a string.
                    vFontPtr->RenderChar(draw_list, cell_size.x, cell_p1, ImGui::GetColorU32(ImGuiCol_Text), (ImWchar)(base + n));
                }
                ImGui::Dummy(ImVec2((cell_size.x + cell_spacing) * 16, (cell_size.y + cell_spacing) * 16));
            }
        }
        vFontPtr->FallbackGlyph = glyph_fallback;
    }
}

std::string FontDesigner::m_GetBaseShaderString() {
    return u8R"(
@FRAMEBUFFER 

//FORMAT(float or byte)
//COUNT(1 to 8)
//WRAP(clamp or repeat or mirror)
//FILTER(linear or nearest)
//MIPMAP(false or true)
//SIZE(800,600 or picture:file.jpeg)
//RATIO(1.5 or picture:file.jpeg)

@UNIFORMS

uniform(texture) vec2(sdf) uAtlasSize; // sdf texture size
uniform(glyph) int(glyphcount) uCountGlyphs; // count glyphs
uniform(glyph) vec4(glyphrects) uGlyphRects[glyphcount]; // glyph rects : left, bottom, right, top
uniform(glyph) vec2(glyphcenter) uGlyphCenterOffsets[glyphcount]; // glyph center offset : x,y on range 0,0 to 1,1, default is center 0.5,0.5

@FRAGMENT

layout(location = 0) out vec4 fragColor;

[[FONT_CODE]]

void mainFontMap(vec2 fragCoord) {
	vec2 uv = fragCoord / uAtlasSize;
	for (int i = 0; i < uCountGlyphs; i++) {
		vec4 rc = uGlyphRects[i];			
		if (uv.x >= rc.x && uv.y >= rc.y && uv.x <= rc.z && uv.y <= rc.w) {
			vec2 glyphSize = rc.zw - rc.xy;
			vec2 glyphCoord = mod(uv - rc.xy, glyphSize) - glyphSize * 0.5;
			glyphCoord -= glyphSize * 0.5 * (uGlyphCenterOffsets[i] * 2.0 - 1.0);			
			fragColor = mainGlyph(i, glyphCoord, glyphSize, fragCoord, uAtlasSize);
			// glyph found so we keep gpu cycles if we break here
			break;
		}
	}
}

void main(void) {
	mainFontMap(gl_FragCoord.xy);
}
)";
}

void FontDesigner::SetFontToLoad(std::string vFontName) {
    m_FontToLoad = vFontName;
    m_GotoSelectedFontName = true;
    GenerationThread::Params->IsFontLoaded = false;
}

void FontDesigner::GoToSelectedFont() {
    m_GotoSelectedFontName = true;
}

bool FontDesigner::LoadFontFileOnDemand(std::string vFontFilePathName, float vFontSize) {
    GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

    GenerationThread::Params->IsFontLoaded = false;

    ResetSelection();

    ImFontConfig config;
    config.OversampleH = 1;
    config.OversampleV = 1;
    config.GlyphExtraSpacing.x = 1.0f;

    ImGuiIO& io = ImGui::GetIO();

    // clear the atlas for recreate it after
    io.Fonts->Clear();
    m_TestFont = 0;

    ImFont* font = 0;
    if (FileHelper::Instance()->IsFileExist("comicbd.ttf")) {
        font = io.Fonts->AddFontFromFileTTF("comicbd.ttf", 17.0f, &config);
    }
    if (font == 0)
        io.Fonts->AddFontDefault();

    std::string fontPath = vFontFilePathName;
    if (fontPath.size() > 0) {
        ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), vFontSize, &config);
        if (font != 0) {
            // on efface la texture de msdfgen

            if (GenerationThread::Params->textureFont > 0) {
                glDeleteTextures(1, &GenerationThread::Params->textureFont);
                GenerationThread::Params->textureFont = 0;
            }

            return true;
        }
    }

    return false;
}

bool FontDesigner::LoadFontOnDemand(std::string vFontName, float vFontSize) {
    GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

    GenerationThread::Params->IsFontLoaded = false;

    ResetSelection();

    ImFontConfig config;
    config.OversampleH = 1;
    config.OversampleV = 1;
    config.GlyphExtraSpacing.x = 1.0f;

    ImGuiIO& io = ImGui::GetIO();

    // clear the atlas for recreate it after
    io.Fonts->Clear();
    m_TestFont = 0;

    ImFont* font = 0;
    if (FileHelper::Instance()->IsFileExist("comicbd.ttf")) {
        font = io.Fonts->AddFontFromFileTTF("comicbd.ttf", 17.0f, &config);
    }
    if (font == 0)
        io.Fonts->AddFontDefault();

    std::string fontPath = m_FontExplorerPtr->GetFontFilePathName(vFontName);
    if (fontPath.size() > 0) {
        ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), vFontSize, &config);
        if (font != 0) {
            // on efface la texture de msdfgen
            if (GenerationThread::Params->textureFont > 0) {
                glDeleteTextures(1, &GenerationThread::Params->textureFont);
                GenerationThread::Params->textureFont = 0;
            }

            return true;
        }
    }

    return false;
}

void FontDesigner::LoadPendingFonts() {
    GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

    if (m_FontToLoad.size() > 0 || m_FontFilePathName.size() > 0) {
        GenerationThread::Params->IsFontLoaded = false;

        if (m_FontToLoad.size() > 0) {
            if (LoadFontOnDemand(m_FontToLoad, 17.0f /*GenerationThread::Params->fontSize*/) == true) {
                m_CurrentSelectedFont = m_FontToLoad;
                m_CurrentNotLoadedFont.clear();
            }

            m_FontToLoad.clear();
        } else if (m_FontFilePathName.size() > 0) {
            if (LoadFontFileOnDemand(m_FontFilePathName, 17.0f /*GenerationThread::Params->fontSize*/) == true) {
                std::size_t lastSlash = m_FontFilePathName.find_last_of('\\');
                //			std::string filePath = m_FontFilePathName.substr(0, lastSlash);
                std::string fileNameExt = m_FontFilePathName.substr(lastSlash + 1);
                std::size_t lastPoint = fileNameExt.find_last_of('.');
                std::string fileName = fileNameExt.substr(0, lastPoint);
                //			std::string fileExt = fileNameExt.substr(lastPoint + 1);

                m_CurrentSelectedFont = fileName;
                m_CurrentNotLoadedFont.clear();
                GenerationThread::Params->FontFileLoaded = m_FontFilePathName;
            }

            m_FontFilePathName.clear();
        }

        if (m_CurrentSelectedFont.size() > 0) {
            bool res = ReGenerateFontTexture();
            if (res == false) {
                m_CurrentNotLoadedFont = m_CurrentSelectedFont;

                // on efface la texture de msdfgen
                if (GenerationThread::Params->textureFont > 0) {
                    glDeleteTextures(1, &GenerationThread::Params->textureFont);
                    GenerationThread::Params->textureFont = 0;
                }

                // atlas fail, reload normal texture for avoid crash
                ImGuiIO& io = ImGui::GetIO();
                io.Fonts->Clear();
                ImFontConfig config;
                config.OversampleH = 1;
                config.OversampleV = 1;
                config.GlyphExtraSpacing.x = 1.0f;
                // ImFont* font = io.Fonts->AddFontFromFileTTF("comicbd.ttf", 17.0f, &config);
                // if (font == 0)
                io.Fonts->AddFontDefault();
                res = ReGenerateFontTexture();
                assert(res == true);
            }
        }
    }
}

bool FontDesigner::ReGenerateFontTexture() {
    GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

    // recreate atlas
    ImGuiIO& io = ImGui::GetIO();

    if (m_FontTextureID) {
        glDeleteTextures(1, &m_FontTextureID);
        io.Fonts->TexID = 0;
    }

    // Build texture atlas
    unsigned char* pixels;
    int width, height;
    // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be
    // compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a
    // GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    if (pixels) {
        // Upload texture to graphics system
        GLint last_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGenTextures(1, &m_FontTextureID);
        glBindTexture(GL_TEXTURE_2D, m_FontTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        // Store our identifier
        io.Fonts->TexID = (void*)(intptr_t)m_FontTextureID;

        // Restore state
        glBindTexture(GL_TEXTURE_2D, last_texture);

        return true;
    }

    return false;
}

void FontDesigner::UpdateFontHeight() {
    if (m_TestFont) {
        int pdx = GenerationThread::Params->glyphPadding.x;
        int pdy = GenerationThread::Params->glyphPadding.x;

        int lh = (GenerationThread::Params->lineHeight) * GenerationThread::Params->scale;
        int bh = (GenerationThread::Params->baseHeight) * GenerationThread::Params->scale;

        m_TestFont->FontSize = lh;

        m_TestFont->Ascent = bh;
        m_TestFont->Descent = bh - lh;

        int w = GenerationThread::Params->textureSize.x;
        int h = GenerationThread::Params->textureSize.y;

        /*
        m_TestFontAtlas.TexHeight = h;
        m_TestFontAtlas.TexGlyphPadding = pdx;
        m_TestFontAtlas.TexWidth = w;
        m_TestFontAtlas.TexDesiredWidth = w;
        m_TestFontAtlas.TexUvScale = ImVec2(1.0f, 1.0f);
        */

        for (auto it = GenerationThread::Params->Glyphs.begin(); it != GenerationThread::Params->Glyphs.end(); ++it) {
            auto c = it->first;
            GlyphStruct* glyph = &(it->second);

            int px = glyph->rc.x * w;
            int py = glyph->rc.y * h;
            int psx = (glyph->rc.z - glyph->rc.x) * w;
            int psy = (glyph->rc.w - glyph->rc.y) * h;
            int sx = glyph->size.x * GenerationThread::Params->scale;
            int sy = glyph->size.y * GenerationThread::Params->scale;

            int ox = glyph->offset.x * w;
            int oy = glyph->offset.y * h;
            int hbx = glyph->horiBearing.x * GenerationThread::Params->scale;
            int hby = glyph->horiBearing.y * GenerationThread::Params->scale;

            int advx = (glyph->advx) * GenerationThread::Params->scale + pdx;

            m_TestFont->AddGlyph(&m_TestFontConfig, (ImWchar)c, hbx, bh - hby, hbx + sx, bh - hby + sy, glyph->rc.x, glyph->rc.y, glyph->rc.z, glyph->rc.w, advx);
        }

        m_TestFont->BuildLookupTable();
    }
}

void FontDesigner::LoadBitmapFontIntoImguiFont() {
    if (!m_TestFont) {
        // just pour pas que AddFont merde
        m_TestFontConfig.FontData = (void*)1;
        m_TestFontConfig.FontDataSize = 1;
        m_TestFontConfig.SizePixels = 1.0f;

        // c'est els test qui font merder la fonction
        // IM_ASSERT(font_cfg->FontData != NULL && font_cfg->FontDataSize > 0);
        // IM_ASSERT(font_cfg->SizePixels > 0.0f);

        m_TestFont = m_TestFontAtlas.AddFont(&m_TestFontConfig);
        if (m_TestFont) {
            ImGui::GetIO().Fonts->Fonts.push_back(m_TestFont);
        }
    }
    if (m_TestFont) {
        m_TestFontConfig.FontData = 0;
        m_TestFontConfig.FontDataSize = 0;
        m_TestFontConfig.SizePixels = 0.0f;

        m_TestFont->ClearOutputData();

        m_TestFont->FontSize = 17.0f;
        m_TestFont->ConfigData = &m_TestFontConfig;
        m_TestFont->ConfigDataCount = 1;

        UpdateFontTexFromFBO();

        m_TestFont->ContainerAtlas = &m_TestFontAtlas;

        UpdateFontHeight();
    }
}

void FontDesigner::UpdateFontTexFromFBO() {
    // m_TestFontAtlas.TexID = (ImTextureID)m_Font_RenderPack->GetTextureId(m_PreviewBufferId);
}

void FontDesigner::ExportToFntFile() {
    GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

    if (m_FilePathNameForFntExport.size() > 0 && GenerationThread::Params->textureFont != 0) {
        ///////////////////////////////////////////////////////
        // extract filePath, fileName, fileExt from FilePathName
        ///////////////////////////////////////////////////////

        std::size_t lastSlash = m_FilePathNameForFntExport.find_last_of('\\');
        std::string filePath = m_FilePathNameForFntExport.substr(0, lastSlash);
        std::string fileNameExt = m_FilePathNameForFntExport.substr(lastSlash + 1);
        std::size_t lastPoint = fileNameExt.find_last_of('.');
        std::string fileName = fileNameExt.substr(0, lastPoint);
        //	std::string fileExt = fileNameExt.substr(lastPoint + 1);

        ///////////////////////////////////////////////////////
        // png image writing
        ///////////////////////////////////////////////////////

        std::string image_FilePathNameExt = filePath + "\\" + fileName + ".png";

        /*if (GenerationThread::Params->ExportTexType == FontTextureTypeEnum::FONT_TEXTURE_TYPE_BASE)
        {
            SaveToPictureFile(image_FilePathNameExt, GenerationThread::Params->SubSamples);
        }
        else if (GenerationThread::Params->ExportTexType == FontTextureTypeEnum::FONT_TEXTURE_TYPE_RENDERED)
        {*/
        SaveToPictureFile(image_FilePathNameExt, GenerationThread::Params->exportFlipY, GenerationThread::Params->SubSamples);
        //}

        if (ImGuiFileDialog::Instance()->GetCurrentFilter() == ".fnt") {
            ///////////////////////////////////////////////////////
            // fnt font file writing
            ///////////////////////////////////////////////////////

            std::string fnt_FilePathNameExt = filePath + "\\" + fileName + ".fnt";

            std::string fileStream;

            std::string faceName = m_CurrentSelectedFont;
            ct::replaceString(faceName, " ", "_");
            fileStream += "info face=" + faceName + " size=72 bold=0 italic=0 charset= unicode= stretchH=100 smooth=1 aa=1 padding=2,2,2,2 spacing=0,0 outline=0\n";

            int pdx = GenerationThread::Params->glyphPadding.x;
            int pdy = GenerationThread::Params->glyphPadding.x;

            int lh = (GenerationThread::Params->lineHeight) * GenerationThread::Params->scale;
            int bh = (GenerationThread::Params->baseHeight) * GenerationThread::Params->scale;

            int w = GenerationThread::Params->textureSize.x;
            int h = GenerationThread::Params->textureSize.y;

            fileStream += "common lineHeight=" + ct::toStr(lh) + " base=" + ct::toStr(bh) + " scaleW=" + ct::toStr(w) + " scaleH=" + ct::toStr(h) + " pages=1 packed=0\n";
            fileStream += "page id=0 file=\"" + fileName + ".png\"\n";
            fileStream += "chars count=" + ct::toStr(GenerationThread::Params->Glyphs.size()) + "\n";

            // float range = GenerationThread::Params->range / GenerationThread::Params->scale;

            for (auto itGlyphs = GenerationThread::Params->Glyphs.begin(); itGlyphs != GenerationThread::Params->Glyphs.end(); ++itGlyphs) {
                auto glyph = itGlyphs->second;

                int px = glyph.rc.x * w;
                int py = glyph.rc.y * h;
                int psx = (glyph.rc.z - glyph.rc.x) * w;
                int psy = (glyph.rc.w - glyph.rc.y) * h;
                int sx = glyph.size.x * GenerationThread::Params->scale;
                int sy = glyph.size.y * GenerationThread::Params->scale;

                int ox = glyph.offset.x * w;
                int oy = glyph.offset.y * h;
                int hbx = glyph.horiBearing.x * GenerationThread::Params->scale;
                int hby = glyph.horiBearing.y * GenerationThread::Params->scale;

                int advx = (glyph.advx) * GenerationThread::Params->scale + pdx;

                fileStream += "char id=" + ct::toStr((int)glyph.c);
                fileStream += " str=\"" + ct::toStr(glyph.c) + "\"";
                fileStream += " x=" + ct::toStr(px);
                fileStream += " y=" + ct::toStr(py);
                fileStream += " width=" + ct::toStr(psx);
                fileStream += " height=" + ct::toStr(psy);
                fileStream += " xoffset=" + ct::toStr(0);
                fileStream += " yoffset=" + ct::toStr(bh - hby);
                fileStream += " xadvance=" + ct::toStr(advx);
                fileStream += " page=0 chnl=15\n";
            }

            ///////////////////////////////////////////////////////

            std::ofstream fileWriter(fnt_FilePathNameExt, std::ios::out);
            if (fileWriter.bad() == false) {
                fileWriter << fileStream;
                fileWriter.close();
            }
        }

        m_FilePathNameForFntExport = "";
    }
}

void FontDesigner::ResetSelection() {
    // cancel selection
    m_CurrentSelectedChar = 0;
    m_MultiSelection.clear();
}

void FontDesigner::RefreshSelectionFromNewFontMap() {
    GuiBackend::Instance()->MakeContextCurrent(GenerationThread::sGenerationThread);

    // GenerationThread::Params->ModifiedGlyphs.clear();

    // on va maj les coordonn�es du char
    // car au rehcragment du sdf, le pointeur sur le glyph peut pointer sur un autre esapce memoire
    // hors zone allou� ou tout bonnement sur le mauvais glyph
    // on avait stock� le char selectionn� pour ca, pour pouvoir rerooter
    if (m_CurrentSelectedChar != 0) {
        auto glyph = GenerationThread::Params->GetGlyph(m_CurrentSelectedChar);
        if (glyph != 0) {
        } else {
            // si le char ne fait plus parti de cette font map il est null, donc on va reset la selection
            ResetSelection();
        }
    }
}

void FontDesigner::DrawFontTexture(ImTextureID vId, ImVec2 vPos, ImVec2 vSize, float vDamierZoom) {
    // grid
    if (m_Damier.getBack()) {
        ImGui::GetCurrentWindow()->DrawList->AddImage(
            (ImTextureID)m_Damier.getBack()->glTex, vPos, vPos + vSize, ImVec2(-vDamierZoom, -vDamierZoom), ImVec2(vDamierZoom, vDamierZoom));
    }

    // font texture
    ImVec2 _pos = vPos;
    ImVec2 _size = vSize;
    ImVec2 _uv_a = ImVec2(0.0f, 0.0f);
    ImVec2 _uv_b = ImVec2(1.0f, 1.0f);

    if (m_OneBigCharMode) {
        if (m_CurrentSelectedChar != 0) {
            auto glyph = GenerationThread::Params->GetGlyph(m_CurrentSelectedChar);
            if (glyph) {
                _size = ImVec2(glyph->rc.z - glyph->rc.x, glyph->rc.w - glyph->rc.y);
                _uv_a = ImVec2(glyph->rc.x, glyph->rc.y);
                _uv_b = ImVec2(glyph->rc.z, glyph->rc.w);

                float ratioX = _size.x / _size.y;

                float newX = vSize.y * ratioX;
                float newY = vSize.x / ratioX;
                if (newX < vSize.x) {
                    _size.x = newX;
                    _size.y = vSize.y;
                    _pos.x = vPos.x + (vSize.x - newX) * 0.5f;
                    _pos.y = vPos.y;
                } else {
                    _size.x = vSize.x;
                    _size.y = newY;
                    _pos.x = vPos.x;
                    _pos.y = vPos.y + (vSize.y - newY) * 0.5f;
                }
            }
        }
    }

    ImGui::GetCurrentWindow()->DrawList->AddImage(vId, _pos, _pos + _size, _uv_a, _uv_b);

    if (m_OneBigCharMode) {
        if (m_CurrentSelectedChar != 0) {
            ImGui::GetCurrentWindow()->DrawList->AddRect(
                _pos, _pos + _size, ImColor(m_SelectionColor.x, m_SelectionColor.y, m_SelectionColor.z, m_SelectionColor.w), 10.0f, 15, 5.0f);
        }
    }

    // modified
    for (auto it = GenerationThread::Params->ModifiedGlyphs.begin(); it != GenerationThread::Params->ModifiedGlyphs.end(); ++it) {
        auto glyph = GenerationThread::Params->GetGlyph(it->first);
        if (glyph) {
            ct::fvec4 rc = glyph->rc;
            ImVec2 a = vPos + ImVec2(rc.x, rc.y) * vSize;
            ImVec2 b = vPos + ImVec2(rc.z, rc.w) * vSize;
            ImGui::GetCurrentWindow()->DrawList->AddRect(
                a, b, ImColor(m_ModificationColor.x, m_ModificationColor.y, m_ModificationColor.z, m_ModificationColor.w), 10.0f, 15, 5.0f);
        }
    }

    if (m_MultiSelection.size() > 0) {
        for (auto itSel = m_MultiSelection.begin(); itSel != m_MultiSelection.end(); ++itSel) {
            auto glyph = GenerationThread::Params->GetGlyph(itSel->first);
            if (glyph) {
                ct::fvec4 rc = glyph->rc;
                ImVec2 a = vPos + ImVec2(rc.x, rc.y) * vSize;
                ImVec2 b = vPos + ImVec2(rc.z, rc.w) * vSize;
                ImGui::GetCurrentWindow()->DrawList->AddRect(
                    a, b, ImColor(m_SelectionColor.x, m_SelectionColor.y, m_SelectionColor.z, m_SelectionColor.w), 10.0f, 15, 5.0f);
            }
        }
    } else {
        // selected glyph
        if (m_CurrentSelectedChar != 0) {
            auto glyph = GenerationThread::Params->GetGlyph(m_CurrentSelectedChar);
            if (glyph) {
                ct::fvec4 rc = glyph->rc;
                ImVec2 a = vPos + ImVec2(rc.x, rc.y) * vSize;
                ImVec2 b = vPos + ImVec2(rc.z, rc.w) * vSize;
                ImGui::GetCurrentWindow()->DrawList->AddRect(
                    a, b, ImColor(m_SelectionColor.x, m_SelectionColor.y, m_SelectionColor.z, m_SelectionColor.w), 10.0f, 15, 5.0f);
            }
        }
    }
}

void FontDesigner::UpdateGlyphCenterOffsets() {
    /*if (m_GlyphCenterOffsets) {
        std::string _charSet = GenerationThread::Params->charSet;
        ct::replaceString(_charSet, "\n", "");
        ct::replaceString(_charSet, "\t", "");

        if (m_GlyphCenterOffsets->uVec2Arr != 0) {
            int idx = 0;
            for (auto itChar = _charSet.begin(); itChar != _charSet.end(); ++itChar) {
                char c = *itChar;
                auto glyph = GenerationThread::Params->GetGlyph(c);
                if (glyph != 0) {
                    assert(glyph->c == c);

                    if (idx < m_GlyphCenterOffsets->count)
                        m_GlyphCenterOffsets->uVec2Arr[idx] = glyph->shaderCenterOffset;
                    else
                        break;
                }
                idx++;
            }
        }
    }*/
}

///////////////////////////////////////////////////////
//// WORKER THREAD CONTROL ////////////////////////////
///////////////////////////////////////////////////////

void FontDesigner::StartWorkerThread(bool vJustUpdate) {
    /*if (!m_GenerationThread.StopWorkerThread()) {
        GenerationThread::Params->charSet = std::string(charSetBuffer);

        m_Font_RenderPack->GetShaderKey()->SaveRenderPackConfig(CONFIG_TYPE_Enum::CONFIG_TYPE_ALL);

        m_GenerationThread.CreateThread(std::bind(&MainFrame::WorkerThreadStoppedOrFinished, this), vJustUpdate);
    }*/
}

void FontDesigner::StopWorkerThread() {
    // m_GenerationThread.StopWorkerThread();
}

void FontDesigner::WorkerThreadStoppedOrFinished() {
    // on charge les rects
    /*DoShaderCodeUpdate(true);

    RefreshSelectionFromNewFontMap();

    m_NeedOneRenderingPass = true;

    m_NeedRegeneration = false;

    LoadBitmapFontIntoImguiFont();

    UpdateViewPort(m_DisplayQuality);*/
}

void FontDesigner::UpdateUniforms(RenderPackWeak vRenderPack,
                                  UniformVariantPtr vUniPtr,
                                  DisplayQualityType vDisplayQuality,
                                  MouseInterface* vMouse,
                                  CameraInterface* vCamera) {
    /*if (vRenderPack && vUni) {
        UniformVariant* v = vUni;
        if (v) {
            if (v->widget == "sdf") {
                if (v->glslType == UniformTypeEnum::U_SAMPLER2D) {
                    v->uSampler2D = GenerationThread::Params->textureFont;
                    if (GenerationThread::Params->textureSize.y > 0)
                        v->ratioXY = (float)((double)GenerationThread::Params->textureSize.x / (double)GenerationThread::Params->textureSize.y);

                } else if (v->glslType == UniformTypeEnum::U_VEC2) {
                    if (vDisplayQuality > 0.0f) {
                        v->x = GenerationThread::Params->textureSize.x / vDisplayQuality;
                        v->y = GenerationThread::Params->textureSize.y / vDisplayQuality;
                        v->constant = true;
                    }
                }
            } else if (v->widget == "glyphpadding") {
                if (v->glslType == UniformTypeEnum::U_VEC2) {
                    v->x = GenerationThread::Params->glyphPadding.x / GenerationThread::Params->scale;
                    v->y = GenerationThread::Params->glyphPadding.y / GenerationThread::Params->scale;
                    v->constant = true;
                }
            } else if (v->widget == "glyphcenter") {
                if (v->glslType == UniformTypeEnum::U_VEC2_ARRAY) {
                    if (v->count != m_CountGlyphs || v->uVec2Arr == 0) {
                        SAFE_DELETE_ARRAY(v->uVec2Arr);
                        v->count = m_CountGlyphs;
                        if (v->uVec2Arr == 0) {
                            v->uVec2Arr = new ct::fvec2[v->count];
                            v->ownVec2Arr = true;
                            for (int i = 0; i < v->count; i++)
                                v->uVec2Arr[i] = 0.0f;
                        }
                    }

                    if (v->uVec2Arr) {
                        int idx = 0;
                        for (auto itChar = m_charSet.begin(); itChar != m_charSet.end(); ++itChar) {
                            char c = *itChar;
                            auto glyph = GenerationThread::Params->GetGlyph(c);
                            if (glyph != 0) {
                                assert(glyph->c == c);

                                if (idx < m_CountGlyphs)
                                    v->uVec2Arr[idx] = glyph->shaderCenterOffset;
                                else
                                    break;
                            }
                            idx++;
                        }
                    }

                    m_GlyphCenterOffsets = v;
                }
            } else if (v->widget == "glyphrects") {
                if (v->glslType == UniformTypeEnum::U_VEC4_ARRAY) {
                    if (v->count != m_CountGlyphs || v->uVec4Arr == 0) {
                        SAFE_DELETE_ARRAY(v->uVec4Arr);
                        v->count = m_CountGlyphs;
                        if (v->uVec4Arr == 0) {
                            v->uVec4Arr = new ct::fvec4[v->count];
                            v->ownVec4Arr = true;
                            for (int i = 0; i < v->count; i++)
                                v->uVec4Arr[i] = 0.0f;
                        }

                        if (v->uVec4Arr) {
                            int idx = 0;
                            for (auto itChar = m_charSet.begin(); itChar != m_charSet.end(); ++itChar) {
                                char c = *itChar;
                                auto glyph = GenerationThread::Params->GetGlyph(c);
                                if (glyph != 0) {
                                    assert(glyph->c == c);

                                    if (idx < m_CountGlyphs)
                                        v->uVec4Arr[idx] = glyph->rc;
                                    else
                                        break;
                                }
                                idx++;
                            }
                        }
                    }
                }
            } else if (v->widget == "glyphinversions") {
                if (v->glslType == UniformTypeEnum::U_FLOAT_ARRAY) {
                    if (v->count != m_CountGlyphs || v->uFloatArr == 0) {
                        SAFE_DELETE_ARRAY(v->uFloatArr);
                        v->count = m_CountGlyphs;
                        if (v->uFloatArr == 0) {
                            v->uFloatArr = new float[v->count];
                            v->ownFloatArr = true;
                            for (int i = 0; i < v->count; i++)
                                v->uFloatArr[i] = 0.0f;
                        }
                    }

                    if (v->uFloatArr) {
                        int idx = 0;
                        for (auto itChar = m_charSet.begin(); itChar != m_charSet.end(); ++itChar) {
                            char c = *itChar;
                            auto glyph = GenerationThread::Params->GetGlyph(c);
                            if (glyph != 0) {
                                assert(glyph->c == c);

                                if (idx < m_CountGlyphs) {
                                    if (glyph->dfInverted)
                                        v->uFloatArr[idx] = 1.0f;
                                    else
                                        v->uFloatArr[idx] = 0.0f;
                                    glyph->idx = idx;
                                } else
                                    break;
                            }
                            idx++;
                        }
                    }
                }
            } else if (v->widget == "glyphcount") {
                if (v->glslType == UniformTypeEnum::U_INT) {
                    v->ix = m_CountGlyphs;
                    v->constant = true;
                }
            }
        }
    }*/
}

void FontDesigner::SaveToPictureFile(std::string vFilePathName, bool vFlipY, int vCountSamples) {
    /*if (m_Font_RenderPack) {
        int maxTextureSize = 0;

        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        if (maxTextureSize > 4096) {
            maxTextureSize = 4096;
        }

        GenerationThread::Params->renderingQuality =
            (float)(ct::mini<double>((double)GenerationThread::Params->textureSize.x, (double)GenerationThread::Params->textureSize.y) / (double)maxTextureSize);

        // on active le AA pour l'export
        DoCodeReplacement(vCountSamples);
        m_Font_RenderPack->UpdateShaderChanges(true);

        if (vFilePathName.find(".png") != std::string::npos) {
            m_Font_RenderPack->SaveFBOToPng(vFilePathName, vFlipY, 0, ct::ivec2(0), m_PreviewBufferId);
        } else if (vFilePathName.find(".bmp") != std::string::npos) {
            m_Font_RenderPack->SaveFBOToBmp(vFilePathName, vFlipY, 0, ct::ivec2(0), m_PreviewBufferId);
        } else if (vFilePathName.find(".tga") != std::string::npos) {
            m_Font_RenderPack->SaveFBOToTga(vFilePathName, vFlipY, 0, ct::ivec2(0), m_PreviewBufferId);
        } else if (vFilePathName.find(".hdr") != std::string::npos) {
            m_Font_RenderPack->SaveFBOToHdr(vFilePathName, vFlipY, 0, ct::ivec2(0), m_PreviewBufferId);
        }

        // on retablit le precedent
        DoShaderCodeUpdate(true);
    }*/
}