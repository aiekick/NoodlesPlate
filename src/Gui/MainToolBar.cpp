// NoodlesPlate Copyright (C) 2017-2023 Stephane Cuillerdier aka Aiekick
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MainToolBar.h"

#include <SoGLSL/src/Res/CustomFontToolBar.cpp>
#include <SoGLSL/src/Res/CustomFontToolBar.h>
#include <SoGLSL/src/Systems/GizmoSystem.h>
#include <SoGLSL/src/Systems/GamePadSystem.h>
#include <SoGLSL/src/Systems/SoundSystem.h>
#include <SoGLSL/src/VR/Backend/VRBackend.h>
#include <SoGLSL/src/VR/Gui/VRGui.h>

#include <ImGuiPack.h>

#include <Metrics/MetricSystem.h>
#include <Panes/MessagePane.h>
#include <Gui/MainFrame.h>
#include <Backends/MainBackend.h>

// Panes
#include <Panes/CodePane.h>
#include <Panes/ConfigPane.h>
#include <Panes/HelpPane.h>
#include <Panes/InfosPane.h>
#include <Panes/NodesPane.h>
#include <Panes/TimeLinePane.h>
#include <Panes/UniformsPane.h>
#include <Panes/ConsolePane.h>
#include <Panes/MessagePane.h>
#include <Panes/ProfilerPane.h>
#include <Panes/InspectorPane.h>
#include <Panes/ConfigSwitcherPane.h>
#include <Panes/BufferPreview.h>

ImFont* MainToolBar::puFont = nullptr;

static const float& font_scale_ratio = 1.0f / 3.5f;

bool MainToolBar::Init() {
    static ImFontConfig icons_config3;
    icons_config3.MergeMode              = false;
    icons_config3.PixelSnapH             = true;
    static const ImWchar icons_ranges3[] = {ICON_MIN_NDPTB, ICON_MAX_NDPTB, 0};
    const float& font_size               = 20.0f / font_scale_ratio;
    MainToolBar::puFont                  = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_NDPTB, font_size, &icons_config3, icons_ranges3);
    MainToolBar::puFont->Scale           = font_scale_ratio;
    return (MainToolBar::puFont != nullptr);
}

void MainToolBar::Unit() {
}

void MainToolBar::DrawCoolBar() {
    auto NeedOneFrameUpdate = false;

    const float& toolbar_width = 35.0f;

    static ImGui::ImCoolBarConfig _config;
    _config.normal_size  = 30.0f;
    _config.hovered_size = 75.0f;
    _config.anchor       = ImVec2(0.0f, 0.5f);
    _config.anim_step    = 0.2f;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
    bool left_bar_opened = ImGui::BeginLeftToolBar(toolbar_width);
    ImGui::PopStyleVar();

    if (left_bar_opened) {
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
        bool _opened            = ImGui::BeginCoolBar("##coolbar", ImCoolBarFlags_Vertical, _config);
        auto window  = ImGui::GetCurrentWindow();
        if (window) {
            // correct the rect of the window. maybe a bug on imgui..
            // the workrect can cause issue when clikc a timeline 
            // channel close button when close to the toolbar
            // this thing correct the issue
            const auto& rc            = window->Rect();
            window->WorkRect          = rc;
            window->OuterRectClipped  = rc;
            window->InnerRect         = rc;
            window->InnerClipRect     = rc;
            window->ParentWorkRect    = rc;
            window->ClipRect          = rc;
            window->ContentRegionRect = rc;
        }
        ImGui::PopStyleVar(2);

        if (_opened) {
            if (ImGui::CoolBarItem()) {
                const auto aw     = ImGui::GetCoolBarItemWidth();
                const auto colBtn = ImGui::ColorButton("##BackGround", MainBackend::Instance()->puBackgroundColor, 0, ImVec2(aw, aw));
                if (colBtn) {
                    // Store current color and open a picker
                    auto& g = *GImGui;
                    // const auto window = ImGui::GetCurrentWindow();
                    g.ColorPickerRef = MainBackend::Instance()->puBackgroundColor;
                    ImGui::OpenPopup("picker");
                    ImGui::SetNextWindowPos(g.LastItemData.Rect.GetBL() + ImVec2(-1, g.Style.ItemSpacing.y));
                    NeedOneFrameUpdate = true;
                }

                if (ImGui::BeginPopup("picker")) {
                    const auto picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
                    const auto picker_flags            = (0 & picker_flags_to_forward) | ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
                    auto& g                            = *GImGui;
                    NeedOneFrameUpdate                 = ImGui::ColorPicker4("##picker", &MainBackend::Instance()->puBackgroundColor.x, picker_flags, &g.ColorPickerRef.x);
                    ImGui::EndPopup();
                }
            }

            /////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////

            /*		IsThereSomeMessages(); // update puIsThereSomeShaderErrors
                    if (puIsThereSomeShaderErrors)
                        ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), ICON_NDP_TIMES_CIRCLE " Errors", "Show/Hide Errors", &puShowErrors);
                    if (puIsThereSomeShaderWarnings)
                        ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), ICON_NDP_EXCLAMATION_TRIANGLE " Warnings", "Show/Hide Warnings", &puShowWarnings);
            */

            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                ImGui::RadioButtonLabeled_BitWize<LayoutPaneFlag>(ImVec2(aw, aw),
                                                             ICON_NDPTB_VIEW_LIST "##Config",
                                                             "Config",
                                                             &LayoutManager::Instance()->pane_Shown,
                                                             ConfigPane::Instance()->GetFlag(),
                                                             false,
                                                             true,
                                                             0,
                                                             false,
                                                             MainToolBar::puFont);
            }
            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                ImGui::RadioButtonLabeled_BitWize<LayoutPaneFlag>(ImVec2(aw, aw),
                                                                  ICON_NDPTB_TUNE "##Uniforms",
                                                                  "Tuning",
                                                                  &LayoutManager::Instance()->pane_Shown,
                                                                  UniformsPane::Instance()->GetFlag(),
                                                                  false,
                                                                  true,
                                                                  0,
                                                                  false,
                                                                  MainToolBar::puFont);
            }

            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                ImGui::RadioButtonLabeled_BitWize<LayoutPaneFlag>(ImVec2(aw, aw),
                                                                  ICON_NDPTB_VECTOR_CURVE "##TimeLine",
                                                                  "TimeLine",
                                                                  &LayoutManager::Instance()->pane_Shown,
                                                                  TimeLinePane::Instance()->GetFlag(),
                                                                  false,
                                                                  true,
                                                                  0,
                                                                  false,
                                                            MainToolBar::puFont);
            }
            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                ImGui::RadioButtonLabeled_BitWize<LayoutPaneFlag>(ImVec2(aw, aw),
                                                                  ICON_NDPTB_LAYERS_TRIPLE "##TuningSwitcher",
                                                                  "Tuning Switcher",
                                                                  &LayoutManager::Instance()->pane_Shown,
                                                                  ConfigSwitcherPane::Instance()->GetFlag(),
                                                                  false,
                                                                  true,
                                                                  0,
                                                            false, MainToolBar::puFont);
            }

            // #ifdef _DEBUG
            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                ImGui::RadioButtonLabeled_BitWize<LayoutPaneFlag>(ImVec2(aw, aw),
                                                                  ICON_NDPTB_FILE_DOCUMENT_EDIT "##Code",
                                                                  "Code",
                                                                  &LayoutManager::Instance()->pane_Shown,
                                                                  CodePane::Instance()->GetFlag(),
                                                                  false,
                                                                  true,
                                                                  0,
                                                                  false,
                                                                  MainToolBar::puFont);
            }
            // #endif

            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                ImGui::RadioButtonLabeled_BitWize<LayoutPaneFlag>(ImVec2(aw, aw),
                                                                  ICON_NDPTB_FILE_DOCUMENT_BOX "##Notes",
                                                                  "Notes",
                                                                  &LayoutManager::Instance()->pane_Shown,
                                                                  InfosPane::Instance()->GetFlag(),
                                                                  false,
                                                                  true,
                                                                  0,
                                                                  false,
                                                                  MainToolBar::puFont);
            }

#ifdef USE_HELP_IN_APP
            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                ImGui::RadioButtonLabeled_BitWize<LayoutPaneFlag>(ImVec2(aw, aw),
                                                                  ICON_NDPTB_COMMENT_QUESTION "##Help",
                                                                  "Help",
                                                                  &LayoutManager::Instance()->pane_Shown,
                                                                  HelpPane::Instance()->GetFlag(),
                                                                  false,
                                                                  true,
                                                                  0,
                                                                  false,
                                                                  MainToolBar::puFont);
            }
#endif

            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                ImGui::RadioButtonLabeled_BitWize<LayoutPaneFlag>(ImVec2(aw, aw),
                                                                  ICON_NDPTB_CLIPBOARD_PULSE "##Profiler",
                                                                  "Profiler",
                                                                  &LayoutManager::Instance()->pane_Shown,
                                                                  ProfilerPane::Instance()->GetFlag(),
                                                                  false,
                                                                  true,
                                                                  0,
                                                                  false,
                                                            MainToolBar::puFont);
            }

#ifdef USE_GPU_METRIC
            if (MetricSystem::Instance()->IsOK()) {
                ImVec2 lp, np;
                static bool s_ShowMetricInToolBar = false;
                if (ImGui::CoolBarItem()) {
                    const auto aw              = ImGui::GetCoolBarItemWidth();
                    MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                    lp                         = ImGui::GetCursorScreenPos();
                    ImGui::RadioButtonLabeled(ImVec2(aw, aw), ICON_NDPTB_THERMOMETER "##Metrics", "Metrics (GPU 0)", &s_ShowMetricInToolBar, false, MainToolBar::puFont);
                    np = ImGui::GetCursorScreenPos();
                }
                if (s_ShowMetricInToolBar) {
                    auto ph = (np.y + lp.y) * 0.5f;
                    auto pw = (np.x + ImGui::GetItemRectSize().x);
                    MetricSystem::Instance()->DrawTooltip(ImVec2(pw, ph));
                }
            }
#endif

#ifdef _DEBUG
            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                ImGui::RadioButtonLabeled_BitWize<LayoutPaneFlag>(ImVec2(aw, aw),
                                                                  ICON_NDPTB_ATOM_VARIANT "##Inspector",
                                                                  "Inspector",
                                                                  &LayoutManager::Instance()->pane_Shown,
                                                                  InspectorPane::Instance()->GetFlag(),
                                                                  false,
                                                                  true,
                                                                  0,
                                                                  false,
                                                            MainToolBar::puFont);
            }
#endif
            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                ImGui::RadioButtonLabeled_BitWize<LayoutPaneFlag>(ImVec2(aw, aw),
                                                                  ICON_NDPTB_VIEW_GRID "##BufferPreview",
                                                                  "Buffers Preview",
                                                                  &LayoutManager::Instance()->pane_Shown,
                                                                  BufferPreview::Instance()->GetFlag(),
                                                                  false,
                                                                  true,
                                                                  0,
                                                                  false,
                                                            MainToolBar::puFont);
            }

            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                ImGui::RadioButtonLabeled_BitWize<LayoutPaneFlag>(ImVec2(aw, aw),
                                                                  ICON_NDPTB_COMMENT_TEXT_MULTIPLE "##Console",
                                                                  "Console",
                                                                  &LayoutManager::Instance()->pane_Shown,
                                                                  ConsolePane::Instance()->GetFlag(),
                                                                  false,
                                                                  true,
                                                                  0,
                                                                  false,
                                                            MainToolBar::puFont);
            }

            MessagePane::Instance()->DrawToolBarButtons(0.0f, MainToolBar::puFont);

            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                NeedOneFrameUpdate |= ImGui::RadioButtonLabeled(ImVec2(aw, aw), ICON_NDPTB_GRID "##3DSpace", "3D Space",
                                                            &MainBackend::Instance()->puShow3DSpace, false, MainToolBar::puFont);
            }

            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                NeedOneFrameUpdate |=
                    ImGui::RadioButtonLabeled(ImVec2(aw, aw), ICON_NDPTB_CUBE_OUTLINE "##3DMesh", "Mesh", &MainBackend::Instance()->puShowMesh,
                                                false, MainToolBar::puFont);
            }

#ifdef USE_VR
            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                if (ImGui::RadioButtonLabeled(ImVec2(aw, aw), ICON_NDPTB_GOOGLE_CARDBOARD "##VR", "Vr", VRBackend::Instance()->IsLoaded(), false, MainToolBar::puFont)) {
                    MainBackend::Instance()->StartOrStopVR();
                }
            }
#endif
            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                NeedOneFrameUpdate |= ImGui::RadioButtonLabeled(ImVec2(aw, aw), ICON_NDPTB_CAMCORDER "##Camera", "Camera",
                                                                &MainBackend::Instance()->puCanWeTuneCamera, false, MainToolBar::puFont);
            }

            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                NeedOneFrameUpdate |= ImGui::RadioButtonLabeled(ImVec2(aw, aw), ICON_NDPTB_MOUSE "##Mouse", "Mouse",
                                                                &MainBackend::Instance()->puCanWeTuneMouse, false, MainToolBar::puFont);
            }

            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                if (ImGui::RadioButtonLabeled(ImVec2(aw, aw), ICON_NDPTB_AXIS_ARROW "##Gizmo", "Gizmo", &GizmoSystem::Instance()->puActivated, false, MainToolBar::puFont)) {
                    GizmoSystem::Instance()->SetActivation(GizmoSystem::Instance()->puActivated);
                    NeedOneFrameUpdate = true;
                }
            }

            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                NeedOneFrameUpdate |= ImGui::RadioButtonLabeled(ImVec2(aw, aw), ICON_NDPTB_GOOGLE_CONTROLLER "##GamePad", "GamePad", &GamePadSystem::Instance()->puActivated, false, MainToolBar::puFont);
            }

            if (ImGui::CoolBarItem()) {
                const auto aw              = ImGui::GetCoolBarItemWidth();
                MainToolBar::puFont->Scale = font_scale_ratio * ImGui::GetCoolBarItemScale();
                NeedOneFrameUpdate |= ImGui::RadioButtonLabeled(ImVec2(aw, aw), ICON_NDPTB_MUSIC_NOTE "##Sound", "Sound", &SoundSystem::Instance()->puActivated, false, MainToolBar::puFont);
            }

            MainBackend::Instance()->NeedRefresh(NeedOneFrameUpdate);

            MainFrame::sAnyWindowsHovered |= ImGui::IsWindowHovered();

            ImGui::EndCoolBar();
        }
        ImGui::EndLeftToolBar();
    }

    MainToolBar::puFont->Scale = font_scale_ratio;
}