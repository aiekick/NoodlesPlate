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

#include <Gui/MainFrame.h>

// SoGlSl/3rdparty
#include <ctools/FileHelper.h>
#include <ctools/Logger.h>
#include <ImGuiPack.h>
#include <ctools/GLVersionChecker.h>

// SoGLSL
#include <SoGLSL/src/Gui/CustomGuiWidgets.h>
#include <SoGLSL/src/Helper/InterfacePanes.h>
#include <SoGLSL/src/Importer/NetCodeRetriever.h>
#include <SoGLSL/src/Manager/HelpManager.h>
#include <SoGLSL/src/Renderer/RenderPack.h>
#include <SoGLSL/src/Res/CustomFont.h>
#include <SoGLSL/src/Res/CustomFont2.h>
#include <SoGLSL/src/Texture/Texture2D.h>
#include <SoGLSL/src/Systems/CameraSystem.h>
#include <SoGLSL/src/Systems/GamePadSystem.h>
#include <SoGLSL/src/Systems/MidiSystem.h>
#include <SoGLSL/src/Systems/SoundSystem.h>
#include <SoGLSL/src/Systems/GizmoSystem.h>
#include <SoGLSL/src/Systems/RecentFilesSystem.h>
#include <SoGLSL/src/Systems/TimeLineSystem.h>
#include <SoGLSL/src/Profiler/TracyProfiler.h>
#include <SoGLSL/src/Buffer/FrameBuffer.h>
#include <SoGLSL/src/Buffer/FrameBuffersPipeLine.h>
#include <SoGLSL/src/Importer/ShadertoyBackupFileImportDlg.h>

#include <ImGuiPack.h>

#ifdef USE_VR
#include <SoGLSL/src/VR/Gui/VRGui.h>
#include <Panes/VRPane.h>
#endif

// this project
#include <Headers/NoodlesPlateBuild.h>
#include <Config/SettingsDlg.h>
#include <Config/StaticConfig.h>
#include <Metrics/MetricSystem.h>
#include <Backends/MainBackend.h>
#include <Systems/PictureExportSystem.h>
#include <Systems/TemplateSystem.h>
#include <ImGuiThemeHelper.h>
#include <Systems/UrlLibrarySystem.h>
#include <Systems/VersionSystem.h>
#include <Systems/PathSystem.h>
#include <Messaging.h>

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
#include <Panes/ChartPane.h>

#include <Res/CustomFont.h>

#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__WIN64__) || defined(WIN64) || defined(_WIN64) || defined(_MSC_VER)
#include <ShlObj.h> // for quick acces places
#else
#endif

using namespace std::placeholders;

///////////////////////////////////////////////////////
//// STATIC ///////////////////////////////////////////
///////////////////////////////////////////////////////

bool MainFrame::sAnyWindowsHovered = false;
bool MainFrame::sCentralWindowHovered = false;
std::string MainFrame::puFileToLoad = std::string();

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

MainFrame::MainFrame() {  
}

MainFrame::~MainFrame() {
}

void MainFrame::Unit() {
    ZoneScoped;

    SaveConfigFile("config.xml");
    LayoutManager::Instance()->UnitPanes();
    puMainToolBar.Unit();
    m_ShadowTexturePtr.reset();
}

bool MainFrame::Init() {
    ZoneScoped;

    auto res = false;

    ImGui::CustomStyle::Init();

    m_build_themes();

    ImGui::SetPUSHID(4577);
    puMainToolBar.Init();

    puOpenFileDialog.SetType("OpenFileDialog");
    puNewFileDialog.SetType("NewFileDialog");
    puSavePictureDialog.SetType("SaveFileDialog");
    puForkFileDialog.SetType("ForkFileDialog");

    if (MainBackend::Instance()->Init(500)) {
        HelpManager::Instance()->Init("NOODLESPLATE");
        UrlLibrarySystem::Instance()->Init();

        m_CreateDialogPlaces();

        LoadConfigFile("config.xml");

        PathSystem::Instance()->Save();  // apply paths who was loaded before in LoadConfigFile

        LayoutManager::Instance()->SetPaneDisposalRatio("LEFT", 0.3f);
        LayoutManager::Instance()->SetPaneDisposalRatio("RIGHT", 0.3f);
        LayoutManager::Instance()->SetPaneDisposalRatio("BOTTOM", 0.3f);

        LayoutManager::Instance()->Init(ICON_NDP2_DESKTOP_MAC " Layouts", "Default Layout");
        LayoutManager::Instance()->AddPane(ConfigPane::Instance(), ICON_NDP2_VIEW_LIST " Config", "", "LEFT", 0.3f, false, true);
        LayoutManager::Instance()->AddPane(UniformsPane::Instance(), ICON_NDP2_TUNE " Tuning", "", "RIGHT", 0.3f, false, true);
        LayoutManager::Instance()->AddPane(TimeLinePane::Instance(), ICON_NDP2_VECTOR_CURVE " TimeLine", "", "BOTTOM", 0.3f, false, true);
        LayoutManager::Instance()->AddPane(ConfigSwitcherPane::Instance(), ICON_NDP2_LAYERS_TRIPLE " Tuning Switcher", "", "RIGHT", 0.3f, false, false);
        LayoutManager::Instance()->AddPane(InfosPane::Instance(), ICON_NDP2_FILE_DOCUMENT_BOX " Notes", "", "CENTRAL", 0.3f, false, false);
#ifdef USE_HELP_IN_APP
        LayoutManager::Instance()->AddPane(HelpPane::Instance(), ICON_NDP2_COMMENT_QUESTION " Help", "", "CENTRAL", 0.3f, false, false);
#endif
        LayoutManager::Instance()->AddPane(ProfilerPane::Instance(), ICON_NDP2_CLIPBOARD_PULSE " Profiler", "", "BOTTOM", 0.3f, false, false);
        LayoutManager::Instance()->AddPane(ConsolePane::Instance(), ICON_NDP2_COMMENT_TEXT_MULTIPLE " Console", "", "BOTTOM", 0.3f, false, false);
        LayoutManager::Instance()->AddPane(MessagePane::Instance(), ICON_NDP2_COMMENT_TEXT " Message", "", "CENTRAL", 0.3f, false, false);
        LayoutManager::Instance()->AddPane(CodePane::Instance(), ICON_NDP2_FILE_DOCUMENT_EDIT " Code", "", "LEFT", 0.3f, false, false);
#ifdef _DEBUG
        LayoutManager::Instance()->AddPane(InspectorPane::Instance(), ICON_NDP2_ORBIT " Inspector", "", "BOTTOM", 0.3f, false, false);
#endif
        LayoutManager::Instance()->AddPane(BufferPreview::Instance(), ICON_NDP2_VIEW_GRID " Buffers Preview", "", "RIGHT", 0.3f, false, false);
        LayoutManager::Instance()->AddPane(ChartPane::Instance(), ICON_NDP2_CHART_BELL_CURVE " Chart", "", "CENTRAL", 0.0f, false, false);

#ifdef USE_VR
        VRGui::Instance()->SetCodeTree(MainBackend::Instance()->puCodeTree);
#endif
        LayoutManager::Instance()->InitPanes();

        Messaging::sMessagePaneId = ConsolePane::Instance()->GetFlag();
    

        if (MainBackend::Instance()->Load(puFileToLoad)) {
            SaveConfigFile("config.xml");
            res = true;
        } else {
            // on va quitter l'app
        }
    }

    return res;
}

void MainFrame::Display(ct::ivec2 vSize) {
    ZoneScoped;

    const auto context_ptr = ImGui::GetCurrentContext();
    if (context_ptr) {
        MainFrame::sCentralWindowHovered = (context_ptr->HoveredWindow == nullptr);
        ImGui::SetPUSHID(4577);

        if (puShowUI) {
            puMainToolBar.DrawCoolBar();

            if (ImGui::BeginMainMenuBar()) {
                DrawMainMenuBar();

                // ImGui Infos
                auto io = ImGui::GetIO();
                const auto label = ct::toStr("Dear ImGui %s (Docking)", ImGui::GetVersion());
                const auto size = ImGui::CalcTextSize(label.c_str());
                ImGui::Spacing(ImGui::GetContentRegionAvail().x - size.x - ImGui::GetStyle().FramePadding.x * 2.0f);
                ImGui::Text("%s", label.c_str());

                MainFrame::sAnyWindowsHovered |= ImGui::IsWindowHovered();

                ImGui::EndMainMenuBar();
            }

            if (ImGui::BeginMainStatusBar()) {
                if (!DrawImportBar()) {
                    Messaging::Instance()->DrawStatusBar();

                    // ImGui Infos
                    const auto io = ImGui::GetIO();
                    const auto fps = ct::toStr("%.1f ms/frame (%.1f fps)", 1000.0f / io.Framerate, io.Framerate);
                    const auto size = ImGui::CalcTextSize(fps.c_str());
                    ImGui::Spacing(ImGui::GetContentRegionAvail().x - size.x - ImGui::GetStyle().FramePadding.x * 2.0f);
                    ImGui::Text("%s", fps.c_str());
                }

                MainFrame::sAnyWindowsHovered |= ImGui::IsWindowHovered();

                ImGui::EndMainStatusBar();
            }

            if (LayoutManager::Instance()->BeginDockSpace(ImGuiDockNodeFlags_PassthruCentralNode)) {
                if (GizmoSystem::Instance()->Use()) {
                    // puGizmoSystem.DrawTooltips()
                    const auto viewport = ImGui::GetMainViewport();
                    ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);
                    ImGuizmo::SetRect(viewport->Pos.x, viewport->Pos.y, viewport->Size.x, viewport->Size.y);
                    MainBackend::Instance()->NeedRefresh(                   //
                        GizmoSystem::Instance()->DrawTooltips(              //
                            MainBackend::Instance()->puDisplay_RenderPack,  //
                            MainBackend::Instance()->puDisplayQuality,      //
                            CameraSystem::Instance(),                       //
                            MainBackend::Instance()->puScreenSize));        //
                }

                if (MainBackend::Instance()->puPixelDebug.IsActive()) {
                    MainBackend::Instance()->puPixelDebug.DrawTooltips(  //
                        MainBackend::Instance()->puDisplay_RenderPack,   //
                        MainBackend::Instance()->puDisplayQuality,       //
                        CameraSystem::Instance());                       //
                }

                LayoutManager::Instance()->EndDockSpace();
            }

            LayoutManager::Instance()->DrawPanes(0U);
        }

        DrawFileDialogs(vSize);

        const auto& size = ImVec2((float)vSize.x, (float)vSize.y);
        LayoutManager::Instance()->InitAfterFirstDisplay(size);

        VersionSystem::Instance()->CheckVersionAtStart();
        SettingsDlg::Instance()->DrawDialog();
        VersionSystem::Instance()->DrawNewVersionDialog();
        ShadertoyBackupFileImportDlg::Instance()->DrawDialog();
        LayoutManager::Instance()->DrawDialogsAndPopups(0U, ImRect(ImVec2(), size));

        DrawErrorDialog();
        DoAbout();
        if (!MainBackend::Instance()->puDisplay_RenderPack.expired()) {
            auto displayShaderPtr = MainBackend::Instance()->puDisplay_RenderPack.lock();
            if (displayShaderPtr) {
                MainBackend::Instance()->NeedRefresh(displayShaderPtr->DrawDialogsAndPopups(vSize));
            }
        }

        if (puShowImGui) {
            ImGui::ShowDemoWindow();
        }
        if (puShowImPlot) {
            ImPlot::ShowDemoWindow();
        }
        if (puShowMetric) {
            ImGui::ShowMetricsWindow(&puShowMetric);
        }

        // on update la mouse apres l'affichage complet
        CanMouseAffectRendering();
    }
}

void MainFrame::NewFrame() {
    sAnyWindowsHovered = false;
    sCentralWindowHovered = false;

    // UI Visibility
    if (ImGui::IsKeyReleased(GuiBackend_KEY_F10)) {
        ImGuiContext& g = *GImGui;
        if ((int)puShowUIFrame != g.FrameCount) {
            puShowUI = !puShowUI;
            puShowUIFrame = g.FrameCount;
        }
    }
}

void MainFrame::CanMouseAffectRendering() {
    const auto& CanUpdateMouse = MainFrame::sCentralWindowHovered &&  // only update mouse when hovering the central node
        !ImGuizmo::IsUsing() &&                                       // only update mouse if no gizmo is moving
        ImGui::GetTopMostAndVisiblePopupModal() == nullptr;           // only update mouse if no modal dialog is opened
    MainBackend::Instance()->UpdateMouseDatas(CanUpdateMouse);
}

void MainFrame::ShowAboutDialog() {
    puShowAboutDialog = true;
}

void MainFrame::m_CreateDialogPlaces() {
    const char* system_group_name = ICON_NDP_STAR " System";
    ImGuiFileDialog::Instance()->AddPlacesGroup(system_group_name, 1, false);
    auto system_places_ptr = ImGuiFileDialog::Instance()->GetPlacesGroupPtr(system_group_name);
    if (system_places_ptr != nullptr) {
#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__WIN64__) || defined(WIN64) || defined(_WIN64) || defined(_MSC_VER)

        //////////////////////////////////////////////////
        //// SYSTEM QUICK ACCESS /////////////////////////
        //////////////////////////////////////////////////

#define addKnownFolderAsPlace(knownFolder, folderLabel, folderIcon)      \
    {                                                                    \
        PWSTR path = NULL;                                               \
        HRESULT hr = SHGetKnownFolderPath(knownFolder, 0, NULL, &path);  \
        if (SUCCEEDED(hr)) {                                             \
            IGFD::FileStyle style;                                       \
            style.icon = folderIcon;                                     \
            auto place_path = IGFD::Utils::UTF8Encode(path);             \
            system_places_ptr->AddPlace(folderLabel, place_path, false, style); \
        }                                                                \
        CoTaskMemFree(path);                                             \
    }
        addKnownFolderAsPlace(FOLDERID_Desktop, "Desktop", ICON_NDP2_DESKTOP_MAC);
        addKnownFolderAsPlace(FOLDERID_Startup, "Startup", ICON_NDP2_HOME);
        addKnownFolderAsPlace(FOLDERID_Downloads, "Downloads", ICON_NDP2_DOWNLOADS);
        addKnownFolderAsPlace(FOLDERID_Pictures, "Pictures", ICON_NDP_PICTURE_O);
        addKnownFolderAsPlace(FOLDERID_Music, "Music", ICON_NDP_MUSIC);
        addKnownFolderAsPlace(FOLDERID_Videos, "Videos", ICON_NDP_VIDEO);
#undef addKnownFolderAsPlace

        //////////////////////////////////////////////////
        //// SYSTEM DEVICES //////////////////////////////
        //////////////////////////////////////////////////

        const DWORD mydrives = 2048;
        char lpBuffer[2048];
#define mini(a, b) (((a) < (b)) ? (a) : (b))
        const DWORD countChars = mini(GetLogicalDriveStringsA(mydrives, lpBuffer), 2047);
#undef mini
        if (countChars > 0U && countChars < 2049U) {
            std::string var = std::string(lpBuffer, (size_t)countChars);
            IGFD::Utils::ReplaceString(var, "\\", "");
            auto arr = IGFD::Utils::SplitStringToVector(var, '\0', false);
            wchar_t szVolumeName[2048];
            std::string path;
            std::string name;
            for (auto& a : arr) {
                path = a;
                name.clear();
                std::wstring wpath = IGFD::Utils::UTF8Decode(a);
                if (GetVolumeInformationW(wpath.c_str(), szVolumeName, 2048, NULL, NULL, NULL, NULL, 0)) {
                    name = IGFD::Utils::UTF8Encode(szVolumeName);
                }
                IGFD::FileStyle style;
                style.icon = ICON_NDP_DRIVES;
                system_places_ptr->AddPlace(path + " " + name, path + IGFD::Utils::GetPathSeparator(), false, style);
            }
        }
#else
#endif
        system_places_ptr = nullptr;
    }

    const char* local_group_name = "Local";
    ImGuiFileDialog::Instance()->AddPlacesGroup(local_group_name, 2, false);
    auto local_places_ptr = ImGuiFileDialog::Instance()->GetPlacesGroupPtr(local_group_name);
    if (local_places_ptr != nullptr) {
        IGFD::FileStyle style;
        style.icon = ICON_NDP_PICTURE_O;
        local_places_ptr->AddPlace("Assets", "assets", false, style);
        style.icon = ICON_NDP_FILE_TEXT_O;
        local_places_ptr->AddPlace("Scripts", "scripts", false, style);
        local_places_ptr = nullptr;
    }

    const char* recent_group_name = "Recent";
    ImGuiFileDialog::Instance()->AddPlacesGroup(recent_group_name, 3, false);
}

void MainFrame::DrawSettingsMenu_Global() {
    if (ImGui::MenuItem("Settings Dialog", "")) {
        SettingsDlg::Instance()->OpenDialog();
    }

    ImGui::Separator();

    ImGuiThemeHelper::Instance()->DrawMenu();

    MainBackend::Instance()->DrawMenu_DisplayQuality();

    if (ImGui::BeginMenu("FullScreen")) {
        auto idx = 0;
        for (auto it = MainBackend::Instance()->puMonitorsName.begin(); it != MainBackend::Instance()->puMonitorsName.end(); ++it) {
            if (ImGui::RadioButton(it->first.c_str(), idx == MainBackend::Instance()->sFullScreenWindowedMonitorIndex)) {
                MainBackend::Instance()->sFullScreenWindowedMonitorIndex = idx;
            }

            idx++;
        }

        ImGui::EndMenu();
    }

    MainBackend::Instance()->DrawMenu_FXAA();
}

void MainFrame::OpenImportBar(std::string vUrl) {
#ifdef USE_NETWORK
    puShowImportBar = true;
    if (!vUrl.empty()) {
        NetCodeRetriever::Instance()->SetUrl(vUrl);
    }
#endif  // #ifdef USE_NETWORK
}

bool MainFrame::DrawImportBar() {
#ifdef USE_NETWORK
    if (puShowImportBar) {
        /*const auto y = ImGui::GetCursorPosY();
        ImGui::SetNextWindowPos(ImVec2(10, y));
        ImGui::SetNextWindowSize(ImVec2(-1, -1));
        const auto flags =
            ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoCollapse
            //| ImGuiWindowFlags_NoBringToFrontOnFocus
            | ImGuiWindowFlags_NoFocusOnAppearing
            ;

        ImGui::Begin("#ImportBar", nullptr, flags);*/

        const auto load = NetCodeRetriever::Instance()->drawUrl(&puShowImportBar, true, true);

        // ImGui::End();

        if (load) {
            auto msgs = MainBackend::Instance()->LoadUrl();
            if (msgs.empty()) {
                SaveConfigFile("config.xml");
                puShowImportBar = false;
            } else {
                puShowErrorDialog = true;
                puErrorDialogTitle = "Import Error";
                puErrorDialogMsg.clear();
                for (const auto& msg : msgs) {
                    puErrorDialogMsg += msg;
                }
            }
        }
    }
#endif  // #ifdef USE_NETWORK
    return puShowImportBar;
}

bool MainFrame::DrawErrorDialog() {
    if (puShowErrorDialog) {
        ImGui::Begin(puErrorDialogTitle.c_str(), (bool*)nullptr, ImGuiWindowFlags_NoCollapse);

        MainFrame::sAnyWindowsHovered |= ImGui::IsWindowHovered();

        ImGui::Text("Error : %s", puErrorDialogMsg.c_str());

        if (ImGui::ContrastedButton("OK", nullptr)) {
            puShowErrorDialog = false;
        }

        ImGui::End();
    }

    return puShowErrorDialog;
}

bool MainFrame::DoAbout() {
    auto res = false;

    if (puShowAboutDialog) {
        ImGui::Begin("About NoodlesPlate", &puShowAboutDialog);

        ImGui::Image((ImTextureID)(void*)(size_t)MainBackend::Instance()->m_AppIconGLTextureID, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));

        // VERSION
        ImGui::Text("Version : %s %s %s build %s", "Beta",
#ifdef _DEBUG
                    "Debug",
#else
                    "Release",
#endif

#ifdef VERSION_X64
                    "x64",
#else
                    "x86",
#endif
                    NoodlesPlate_BuildId);

        ImGui::Text("License : ");
        ImGui::Indent();
        ImGui::Text("Copyright 2017-2019 Stephane Cuillerdier / AieKick");
        ImGui::Text("Free to use without limitations\n");
        ImGui::Text("Credit to the software is appreciated by ex : 'designed with NoodlesPlate'\n");
        ImGui::Text("Selling or modify the binary of this software is disallowed\n");
        ImGui::Text("The Software is provided 'as is', without warranty of any kind, express or implied.\n");
        ImGui::Text("In no event shall the authors or copyright holders X be liable for any claim, damages or other liability\n");
        ImGui::Text("whether in an action of contract, tort or otherwise, arising from, out of or in connection with the software\n");
        ImGui::Text("or the use or other dealings in the Software. \n");
        ImGui::Unindent();
        ImGui::Separator();
        ImGui::ClickableTextUrl("NoodlesPlate : Updates / Help / tutorials", "https://www.funparadigm.com/noodlesplate");
        ImGui::Separator();

        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "My Accounts :");
        ImGui::Indent();
        {
            ImGui::ClickableTextUrl("Twitter", "https://twitter.com/aiekick");
            ImGui::SameLine();
            ImGui::ClickableTextUrl("Instagram", "https://www.instagram.com/aiekick/");
            ImGui::SameLine();
            ImGui::ClickableTextUrl("Sketchfab", "https://sketchfab.com/Aiekick");
        }
        ImGui::Unindent();
        ImGui::Indent();
        {
            ImGui::ClickableTextUrl("Github", "https://github.com/aiekick");
            ImGui::SameLine();
            ImGui::ClickableTextUrl("Shadertoy", "https://www.shadertoy.com/user/aiekick");
            ImGui::SameLine();
            ImGui::ClickableTextUrl("Web", "https://www.funparadigm.com");
        }
        ImGui::Unindent();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Frameworks / Libraries / Api's used :");
        ImGui::Indent();
        {
            // glfw3
            ImGui::ClickableTextUrl("Glfw 3.2.1 (ZLIB)", "http://www.glfw.org/");
            // ImGui / version
            ImGui::ClickableTextUrl("ImGui (MIT)", "https://github.com/ocornut/imgui");
            ImGui::SameLine();
            ImGui::Text("by");
            ImGui::SameLine();
            ImGui::ClickableTextUrl("Omar Cornut @Ocornut", "https://twitter.com/ocornut");
            // Curl
            ImGui::ClickableTextUrl("Curl 7.54.0 (MIT)", "https://curl.haxx.se/");
            // cWalk
            ImGui::ClickableTextUrl("cWalk (MIT)", "https://github.com/likle/cwalk");
            // glad
            ImGui::ClickableTextUrl("Glad 0.1.14a0 (MIT)", "https://github.com/Dav1dde/glad");
            // glm
            ImGui::ClickableTextUrl("Glm (MIT & Happy Bunny License)", "https://glm.g-truc.net/0.9.9/index.html");
            // stb
            ImGui::ClickableTextUrl("Stb (MIT)", "https://github.com/nothings/stb");
            ImGui::SameLine();
            ImGui::Text("by");
            ImGui::SameLine();
            ImGui::ClickableTextUrl("Sean Barrett @Nothings", "https://twitter.com/nothings");
            // tinyxml2
            ImGui::ClickableTextUrl("tinyxml2 (ZLIB)", "https://github.com/leethomason/tinyxml2");
            // dirent
            ImGui::ClickableTextUrl("dirent (MIT)", "https://github.com/tronkko/dirent/blob/master/include/dirent.h");
            // filewatcher
            ImGui::ClickableTextUrl("FileWatcher (MIT)", "https://github.com/apetrone/simplefilewatcher");
            // shadertoy api
            ImGui::ClickableTextUrl("Shadertoy.com API", "https://www.shadertoy.com/api");
            // ImGuizmo
            ImGui::ClickableTextUrl("ImGuizmo (MIT)", "https://github.com/CedricGuillemet/ImGuizmo");
            ImGui::SameLine();
            ImGui::Text("by");
            ImGui::SameLine();
            ImGui::ClickableTextUrl("Cedric Guillemet @skaven_  (MIT)", "https://twitter.com/skaven_");
            // ImGui MArkDown
            ImGui::ClickableTextUrl("imgui_markdown (Zlib)", "https://github.com/juliettef/imgui_markdown");
            ImGui::SameLine();
            ImGui::Text("by");
            ImGui::SameLine();
            ImGui::ClickableTextUrl("Juliette Foucaut @juulcat  (MIT)", "https://twitter.com/juulcat");
            // Remotery
#ifdef PROFILER
            ImGui::ClickableTextUrl("Remotery (Apache-2.0)", "https://github.com/Celtoys/Remotery");
#endif
            ImGui::ClickableTextUrl("AlphaNum algo from David Koelle (MIT)", "http://www.davekoelle.com/alphanum.html");
        }
        ImGui::Unindent();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Inspirations :");
        ImGui::Indent();
        {
            // Shadertoy
            ImGui::ClickableTextUrl("Shadertoy (webgl)", "https://www.shadertoy.com/");
            ImGui::SameLine();
            ImGui::Text("by");
            ImGui::SameLine();
            ImGui::ClickableTextUrl("@Beauty_pi", "https://twitter.com/beauty_pi");
            // Vertexshaderart
            ImGui::ClickableTextUrl("Vertexshaderart (webgl)", "https://www.vertexshaderart.com/");
            ImGui::SameLine();
            ImGui::Text("by");
            ImGui::SameLine();
            ImGui::ClickableTextUrl("@Greggman", "https://twitter.com/greggman");
            // GlslSandbox
            ImGui::ClickableTextUrl("GlslSandbox (webgl)", "http://www.glslsandbox.com/");
        }
        ImGui::Unindent();

        ImGui::End();
    }

    return res;
}

////////////////////////////////////////////////
//// PRIVATE ///////////////////////////////////
////////////////////////////////////////////////

void MainFrame::DrawFileDialogs(const ct::ivec2& vScreenSize) {
    ImVec2 min = ImVec2(0, 0);
    ImVec2 max = ImVec2(FLT_MAX, FLT_MAX);
    if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)) {
        max = ImVec2((float)vScreenSize.x, (float)vScreenSize.y);
        min = max * 0.5f;
    }

    if (ImGuiFileDialog::Instance()->Display("OpenFileDialog", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking, min, max)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            puOpenFileDialog.filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            puOpenFileDialog.filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            if (MainBackend::Instance()->LoadFilePathName(puOpenFileDialog.filePathName, ".glsl")) {
                SaveConfigFile("config.xml");
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("OpenShaderToyBackupJsonFile", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking, min, max)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            puOpenFileDialog.filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            puOpenFileDialog.filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            ShadertoyBackupFileImportDlg::Instance()->OpenDialog(puOpenFileDialog.filePathName);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("NewFileDialog", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking, min, max)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            puNewFileDialog.filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            puNewFileDialog.filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            std::string mode;
            if (ImGuiFileDialog::Instance()->GetUserDatas())
                mode = std::string((const char*)ImGuiFileDialog::Instance()->GetUserDatas());

            if (mode == "createfromtemplate") {
                const auto fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                MainBackend::Instance()->NewFilePathNameFromTemplate(fileName, puNewFileDialog.filePath);
            } else if (mode == "saveastemplate") {
                const auto fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                MainBackend::Instance()->SaveAsTemplate(fileName, puNewFileDialog.filePath);
            } else {
                MainBackend::Instance()->NewFilePathName(puNewFileDialog.filePathName, puNewFileDialog.tgt_ext, puNewFileDialog.filePath);
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // pour le moment la fonction d'ecriture de jpg merde bien comme il faut donc on la desactive, on verra plus tard
    // if (ImGuiFileDialog::Instance()->Display("Save Current Buffer to Picture File", ".png\0.tga\0.bmp\0.jpg\0.hdr\0\0", puFilePicturePath,
    // puFilePicturePathName))
    if (ImGuiFileDialog::Instance()->Display("SavePictureDialog", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking, min, max)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            const auto filepathname = ImGuiFileDialog::Instance()->GetFilePathName();

            const auto ext = filepathname.find_last_of('.');
            if (ext != std::string::npos) {
                puSavePictureDialog.filePathName = filepathname.substr(0, ext);
            } else {
                puSavePictureDialog.filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            }

            puSavePictureDialog.filePathName += ImGuiFileDialog::Instance()->GetCurrentFilter();

            puSavePictureDialog.filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            if (!MainBackend::Instance()->puDisplay_RenderPack.expired()) {
                if (MainBackend::Instance()->puShow3DSpace) {
                    PictureExportSystem::Instance()->ExportFBOToPictureFile(MainBackend::Instance()->puMainPipeLine->puBackBuffer, filepathname,
                                                                            MainBackend::Instance()->puPreviewBufferId);
                } else {
                    PictureExportSystem::Instance()->ExportRenderPackToPictureFile(MainBackend::Instance()->puDisplay_RenderPack, filepathname,
                                                                                   MainBackend::Instance()->puPreviewBufferId);
                }
            }

            puSavePictureDialog.filePathName = ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("ForkFileDialog", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking, min, max)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            puForkFileDialog.filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            puForkFileDialog.filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            MainBackend::Instance()->Load(MainBackend::Instance()->ForkToFile(puForkFileDialog.filePathName));
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void MainFrame::DrawMainMenuBar() {
    MainFrame::sAnyWindowsHovered |= ImGui::IsWindowHovered();

    if (ImGui::BeginMenu(ICON_NDP2_FILE_DOCUMENT " File")) {
        if (ImGui::BeginMenu(ICON_NDP2_FILE_DOCUMENT " New")) {
            if (ImGui::BeginMenu("Quad")) {
                if (ImGui::MenuItem("Base")) {
                    CreateNewShader("Shader_Quad", "Quad_Shader.glsl");
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Points")) {
                if (ImGui::MenuItem("Base 2D")) {
                    CreateNewShader("Shader_Points_2D", "Points_Shader_2D.glsl");
                }
                if (ImGui::MenuItem("Base 3D Lines")) {
                    CreateNewShader("Shader_Points_3D_Lines", "Points_Shader_3D.glsl");
                }
                if (ImGui::MenuItem("Base 3D Triangles")) {
                    CreateNewShader("Shader_Points_3D_Triangles", "Points_Shader_3D.glsl");
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Mesh")) {
                if (ImGui::MenuItem("Base")) {
                    CreateNewShader("Shader_Mesh_Base", "Mesh_Shader.glsl");
                }
                if (ImGui::MenuItem("Tesselation Shader")) {
                    CreateNewShader("Shader_Mesh_Tesselation", "Mesh_Shader.glsl");
                }
                if (ImGui::MenuItem("Geometry Shader")) {
                    CreateNewShader("Shader_Mesh_Geometry", "Mesh_Shader.glsl");
                }
                if (ImGui::MenuItem("Mesh Capture (Transform Feedback)")) {
                    CreateNewShader("Shader_Mesh_Capture", "Mesh_Shader.glsl");
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("Uniforms")) {
                    CreateNewShader("Shader_Uniforms_Help", "Uniforms.glsl");
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();

            TemplateSystem::Instance()->DrawMenu();  // il met un separator si le menu est affihc�

            if (ImGui::BeginMenu("Features Sample")) {
                if (ImGui::MenuItem("Gizmo Demo")) {
                    CreateNewShader("Shader_Demo_Gizmo", "Gizmo_Basis_Scene.glsl");
                }

                if (ImGui::MenuItem("Space3D Demo")) {
                    CreateNewShader("Shader_Demo_Space3D", "Space3D_Sample.glsl");
                }

                if (ImGui::MenuItem("Gizmo Culling Demo")) {
                    CreateNewShader("Shader_Demo_Gizmo_Culling", "Gizmo_Culling_Sample.glsl");
                }

                if (ImGui::MenuItem("Buffer Attachments Demo")) {
                    CreateNewShader("Shader_Demo_Buffer_Attachments", "Buffer_Attachments_Sample.glsl");
                }

                if (ImGui::BeginMenu("Multipass One File Demo")) {
                    if (ImGui::MenuItem("2 Buffers")) {
                        CreateNewShader("Shader_Demo_Multipass_One_File_2_buffers", "Multipass_One_File_2_Buffers.glsl");
                    }

                    if (ImGui::MenuItem("3 Buffers")) {
                        CreateNewShader("Shader_Demo_Multipass_One_File_3_buffers", "Multipass_One_File_3_Buffers.glsl");
                    }

                    if (ImGui::MenuItem("4 Buffers")) {
                        CreateNewShader("Shader_Demo_Multipass_One_File_4_buffers", "Multipass_One_File_4_Buffers.glsl");
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::MenuItem("Compute Shader Demo")) {
                    CreateNewShader("Shader_Demo_Compute", "Compute.glsl");
                }

                if (ImGui::MenuItem("Instanced Mesh Demo")) {
                    CreateNewShader("Shader_Demo_Instanced_Mesh", "Instanced_Mesh.glsl");
                }

                if (ImGui::MenuItem("Scene Demo")) {
                    CreateNewShader("Shader_Demo_Scene", "Scene_Shader.glsl");
                }

                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::BeginMenu("For ShaderToy")) {
                if (ImGui::MenuItem("Image")) {
                    CreateNewShader("Shader_ShaderToy_Image", "Shadertoy_Shader_Image.glsl");
                }
                if (ImGui::MenuItem("Buffer")) {
                    CreateNewShader("Shader_ShaderToy_Buffer", "Shadertoy_Shader_Buffer.glsl");
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("For GlslSandbox")) {
                if (ImGui::MenuItem("Base")) {
                    CreateNewShader("Shader_GlslSandbox_Base", "GlslSandbox_Base.glsl");
                }
                if (ImGui::MenuItem("MultiPass")) {
                    CreateNewShader("Shader_GlslSandbox_MultiPass", "GlslSandbox_MultiPass.glsl");
                }
                if (ImGui::MenuItem("SurfacePosition")) {
                    CreateNewShader("Shader_GlslSandbox_SurfacePosition", "GlslSandbox_SurfacePosition.glsl");
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("For VertexShaderArt")) {
                if (ImGui::MenuItem("Base")) {
                    CreateNewShader("Shader_VertexShaderArt_Base", "VertexShaderArt_Base.glsl");
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem(ICON_NDP2_FOLDER " Open")) {
            IGFD::FileDialogConfig config;
            config.path = puOpenFileDialog.filePath;
            config.filePathName = puOpenFileDialog.filePathName;
            config.countSelectionMax = 1;
            config.flags = ImGuiFileDialogFlags_DisableThumbnailMode | ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("OpenFileDialog", "Open File", ".glsl", config);
        }

        if (MainBackend::Instance()->IsLoaded()) {
            /* if (ImGui::MenuItem(ICON_NDP_CODE_FORK " Fork"))
            {
                ImGuiFileDialog::Instance()->OpenModal("ForkFileDialog", "Fork to File", ".glsl",
                    puForkFileDialog.filePath, puForkFileDialog.filePathName, 1, nullptr,
                    ImGuiFileDialogFlags_DisableThumbnailMode | ImGuiFileDialogFlags_Modal);
            }
            */

            if (ImGui::MenuItem(ICON_NDP_BAN " Close")) {
                MainBackend::Instance()->puMain_RenderPack->Finish(false);
                const auto mp = MainBackend::Instance()->puFileListingExceptions;
                MainBackend::Instance()->puCodeTree->Clear(mp);
                MainBackend::Instance()->puMain_RenderPack->SetShaderKey(nullptr);
            }
        }

        if (RecentFilesSystem::Instance()->DrawMenu("Recent Files")) {
            if (MainBackend::Instance()->LoadFilePathName(RecentFilesSystem::Instance()->GetSelectedFile(), ".glsl")) {
                SaveConfigFile("config.xml");
            }
        }

        if (ImGui::BeginMenu(ICON_NDP2_IMPORT " Import")) {
            if (ImGui::BeginMenu("From File Type")) {
                if (ImGui::MenuItem(".json (ShaderToy Backup File)")) {
                    IGFD::FileDialogConfig config;
                    config.path = puOpenFileDialog.filePath;
                    config.filePathName = puOpenFileDialog.filePathName;
                    config.countSelectionMax = 1;
                    config.flags = ImGuiFileDialogFlags_DisableThumbnailMode | ImGuiFileDialogFlags_Modal;
                    ImGuiFileDialog::Instance()->OpenDialog("OpenShaderToyBackupJsonFile", "Open ShaderToy Backup File", ".json", config);
                }

                if (ImGui::MenuItem(".xsha (wxShade)")) {
                    CTOOL_DEBUG_BREAK;
                }

                if (ImGui::MenuItem(".isf (Interactive Shader Format)")) {
                    CTOOL_DEBUG_BREAK;
                }

                ImGui::EndMenu();
            }

#ifdef USE_NETWORK
            if (ImGui::MenuItem("From Custom Url")) {
                OpenImportBar();
            }

            UrlLibrarySystem::Instance()->DrawImGui_Library();
#endif  // #ifdef USE_NETWORK

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(ICON_NDP2_UPLOAD " Export")) {
            if (ImGui::BeginMenu("Buffer")) {
                if (ImGui::MenuItem("To Picture File")) {
                    IGFD::FileDialogConfig config;
                    config.path = puSavePictureDialog.filePath;
                    config.filePathName = puSavePictureDialog.filePathName;
                    config.countSelectionMax = 1;
                    config.sidePane = std::bind(&PictureExportSystem::ExportPictureDialogOptions, PictureExportSystem::Instance(), _1, _2, _3);
                    config.flags = ImGuiFileDialogFlags_Modal;
                    ImGuiFileDialog::Instance()->OpenDialog(
                        "SavePictureDialog", "Save Current Buffer to Picture File", ".png,.tga,.bmp,.hdr", config);
                }

                if (ImGui::BeginMenu("After Each Modif of Code")) {
                    PictureExportSystem::Instance()->DrawMenu();

                    ImGui::EndMenu();
                }

                /*
                if (ImGui::MenuItem("To Video File"))
                {
                    //puShowOpenDialog = true;
                }
                */

                ImGui::EndMenu();
            }

            /*
            if (ImGui::BeginMenu("Code"))
            {
                if (ImGui::BeginMenu("Get Stages To File"))
                {
                    if (ImGui::MenuItem("Vertex"))
                    {

                    }
                    if (ImGui::MenuItem("Tesselation Control"))
                    {

                    }
                    if (ImGui::MenuItem("Tesselation Evaluation"))
                    {

                    }
                    if (ImGui::MenuItem("Geometry"))
                    {

                    }
                    if (ImGui::MenuItem("Fragment"))
                    {

                    }
                    if (ImGui::MenuItem("Compute"))
                    {

                    }
                    if (ImGui::MenuItem("All"))
                    {

                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
            */

            ImGui::EndMenu();
        }

        ImGui::Separator();

        auto displayShaderPtr = MainBackend::Instance()->puDisplay_RenderPack.lock();
        if (displayShaderPtr && displayShaderPtr->GetShaderKey()) {
            if (ImGui::MenuItem(ICON_NDP_FLOPPY_O " Save As Template")) {
                IGFD::FileDialogConfig config;
                config.path = ImGuiFileDialog::Instance()->GetCurrentPath();
                config.filePathName = displayShaderPtr->GetShaderKey()->puKey;
                config.countSelectionMax = 1;
                config.userDatas = IGFDUserDatas("saveastemplate");
                config.flags = ImGuiFileDialogFlags_DisableThumbnailMode | ImGuiFileDialogFlags_Modal;
                ImGuiFileDialog::Instance()->OpenDialog("NewFileDialog", "Save As Template", ".glsl", config);
            }

            ImGui::Separator();
        }

#ifdef USE_NETWORK
        if (ImGui::MenuItem("Check New Version", "")) {
            VersionSystem::Instance()->CheckLastVersion(true);
        }
#endif

        ImGui::Separator();

        if (ImGui::MenuItem(ICON_NDP_QUESTION " Help")) {
            FileHelper::Instance()->OpenUrl("https://github.com/aiekick/NoodlesPlate/wiki");
        }

#ifdef USE_NETWORK
        if (ImGui::MenuItem(ICON_NDP_INFO " ChangeLog")) {
            VersionSystem::Instance()->ShowVersionChangeLog();
        }
#endif

        ImGui::Separator();

        if (ImGui::MenuItem(ICON_NDP_INFO " About")) {
            ShowAboutDialog();
        }

        ImGui::EndMenu();
    }

    auto& screenSize = MainBackend::Instance()->puScreenSize;
    LayoutManager::Instance()->DisplayMenu(ImVec2((float)screenSize.x, (float)screenSize.y));

    if (ImGui::BeginMenu(ICON_NDP_COG " Settings")) {
        DrawSettingsMenu_Global();

        ImGui::Separator();

        // ImGui::MenuItem("Show 3D Space", "", &MainBackend::Instance()->puShow3DSpace);
        // ImGui::MenuItem("Show Mesh", "", &MainBackend::Instance()->puShowMesh);
        // ImGui::MenuItem("Show BluePrints Planes", "", &MainBackend::Instance()->puShowBPPlanes);

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(ICON_NDP2_DEBUG " Debug")) {
        if (ImGui::BeginMenu("Pixel Debugging")) {
            MainBackend::Instance()->puPixelDebug.DrawMenu();

            ImGui::EndMenu();
        }

        ImGui::MenuItem("Debug Mode", "", &puShowDebug);

        if (puShowDebug) {
            ImGui::Separator();

            ImGui::MenuItem("Show ImGUi", "", &puShowImGui);
            ImGui::MenuItem("Show ImPlot", "", &puShowImPlot);
            ImGui::MenuItem("Show Metric/Debug", "", &puShowMetric);
        }

        ImGui::EndMenu();
    }

    MainBackend::Instance()->NeedRefresh(GizmoSystem::Instance()->DrawMenu());
    MainBackend::Instance()->NeedRefresh(GamePadSystem::Instance()->DrawMenu());
    MainBackend::Instance()->NeedRefresh(SoundSystem::Instance()->DrawMenu());

    if (ImGui::BeginMenu(ICON_NDP_INFO " Infos")) {
        GLVersionChecker::Instance()->m_OpenglInfosStruct.drawImGui();

        MetricSystem::Instance()->DrawImGui();

        ImGui::EndMenu();
    }

    ImGui::Spacing();

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

    ImGui::Spacing();

    PictureExportSystem::Instance()->DrawButton_Shot();

    ImGui::Spacing();

    if (ImGui::ContrastedButton(ICON_NDP_COGS "##Compil", "Force Re Compilation")) {
        MainBackend::Instance()->NeedRefresh(MainBackend::Instance()->ForceReCompilation());
    }

    ImGui::Spacing();

    if (ImGui::ContrastedButton(ICON_NDP_REFRESH, "Clear Buffers")) {
        MainBackend::Instance()->NeedColorClearing();
    }

    if (ImGui::ContrastedButton(ICON_NDP_STEP_BACKWARD, "Reset All (Counters / Timers / Players)")) {
        MainBackend::Instance()->ReSetPlayPause();

        MainBackend::Instance()->ResetFrame();
        MainBackend::Instance()->ResetTime();

        MainBackend::Instance()->NeedRefresh();
    }

    ImGui::ToggleContrastedButton(ICON_NDP_PAUSE, ICON_NDP_PLAY, &MainBackend::Instance()->puCanWeRender, "Play/Pause Rendering");

    if (!MainBackend::Instance()->puCanWeRender) {
        if (ImGui::ContrastedButton(ICON_NDP_STEP_FORWARD, "Render Next Frame")) {
            MainBackend::Instance()->RefreshOneFrame();
        }
    }

    if (ImGui::ToggleContrastedButton(ICON_NDP_PAUSE, ICON_NDP_PLAY, &MainBackend::Instance()->puPlayCounterTimers,
                                      "Play/Pause  All (Counters / Timers / Players)")) {
        MainBackend::Instance()->SetPlayPause(MainBackend::Instance()->puPlayCounterTimers);
    }

    ImGui::Spacing();

    // ImGui::Text("Buffers");

    auto displayShaderPtr = MainBackend::Instance()->puDisplay_RenderPack.lock();
    if (displayShaderPtr) {
        MainBackend::Instance()->puPreviewBufferId = ct::mini<int>(MainBackend::Instance()->puPreviewBufferId, displayShaderPtr->puZeroBasedMaxSliceBufferId);

        char bufferRecord[80];
        char bufferRecordHelp[80];
        const auto& arr = displayShaderPtr->puFragColorNames;
        for (auto i = 0; i <= displayShaderPtr->puZeroBasedMaxSliceBufferId; i++) {
            snprintf(bufferRecord, 80, "%i##record", i);
            if (!arr.empty() && !arr[i].empty()) {
                snprintf(bufferRecordHelp, 80, "Show Buffer %i (%s)", i, arr[i].c_str());
            } else {
                snprintf(bufferRecordHelp, 80, "Show Buffer %i", i);
            }
            if (ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), bufferRecord, bufferRecordHelp, MainBackend::Instance()->puPreviewBufferId == i)) {
                MainBackend::Instance()->puPreviewBufferId = i;
                MainBackend::Instance()->NeedRefresh();
            }
            if (!ImGui::IsAnyItemActive()) {
                if (ImGui::IsKeyReleased((ImGuiKey)(GuiBackend_KEY_0 + i)) || ImGui::IsKeyReleased((ImGuiKey)(GuiBackend_KEY_KP_0 + i))) {
                    MainBackend::Instance()->puPreviewBufferId = i;
                    MainBackend::Instance()->NeedRefresh();
                }
            }
        }

        ImGui::Spacing();
    }

    MainBackend::Instance()->DrawMenu_Edit();
}

void MainFrame::CreateNewShader(std::string vType, const std::string& vFilePathName) {
    const auto filePathName = FileHelper::Instance()->CorrectSlashTypeForFilePathName(vFilePathName);
    puNewFileDialog.tgt_ext = vType;
    puNewFileDialog.filePathName = filePathName;
    IGFD::FileDialogConfig config;
    config.path = puNewFileDialog.filePath;
    config.filePathName = puNewFileDialog.filePathName;
    config.countSelectionMax = 1;
    config.flags = ImGuiFileDialogFlags_DisableThumbnailMode | ImGuiFileDialogFlags_Modal;
    ImGuiFileDialog::Instance()->OpenDialog("NewFileDialog", "New File", ".glsl", config);
}

///////////////////////////////////////////////////////
//// CONFIGURATION ////////////////////////////////////
///////////////////////////////////////////////////////

std::string MainFrame::getXml(const std::string& vOffset, const std::string& vUserDatas) {
    UNUSED(vUserDatas);

    std::string str;

    // checked and used when the app is loading
    str += StaticConfig::Instance()->getXml(vOffset, vUserDatas);

    // the rest
    str += puOpenFileDialog.getXml(vOffset, vUserDatas);
    str += puNewFileDialog.getXml(vOffset, vUserDatas);
    str += puSavePictureDialog.getXml(vOffset, vUserDatas);
    str += puForkFileDialog.getXml(vOffset, vUserDatas);
    str += vOffset + "<debug>" + ct::toStr(puShowDebug ? "true" : "false") + "</debug>\n";
    str += vOffset + "<consoleguivisibility>" + ct::toStr(puConsoleGuiVisiblity ? "true" : "false") + "</consoleguivisibility>\n";
    str += vOffset + "<igfd_places>" + ImGuiFileDialog::Instance()->SerializePlaces() + "</igfd_places>\n";
    str += vOffset + "<consoleverbose>" + ct::toStr(Logger::Instance()->ConsoleVerbose ? "true" : "false") + "</consoleverbose>\n";
    str += vOffset + "<ShaderFile>" + MainBackend::sCurrentFileLoaded + "</ShaderFile>\n";

    str += LayoutManager::Instance()->getXml(vOffset, vUserDatas);
    str += PictureExportSystem::Instance()->getXml(vOffset, vUserDatas);
    str += CameraSystem::Instance()->getXml(vOffset, vUserDatas);
    str += RecentFilesSystem::Instance()->getXml(vOffset, vUserDatas);
    str += MainBackend::Instance()->getXml(vOffset, vUserDatas);

#ifdef USE_NETWORK
    str += NetCodeRetriever::Instance()->getXml(vOffset, vUserDatas);
#endif

    str += SoundSystem::Instance()->getXml(vOffset, vUserDatas);
    str += MidiSystem::Instance()->getXml(vOffset, vUserDatas);
    str += GamePadSystem::Instance()->getXml(vOffset, vUserDatas);
    str += UrlLibrarySystem::Instance()->getXml(vOffset, vUserDatas);
    str += VersionSystem::Instance()->getXml(vOffset, vUserDatas);
    str += TimeLineSystem::Instance()->getXml(vOffset, vUserDatas);
    str += ImGuiThemeHelper::Instance()->getXml(vOffset, vUserDatas);
    str += PathSystem::Instance()->getXml(vOffset, vUserDatas);

#ifdef USE_OPTIMIZER_SYSTEM
    str += OptimizerSystem::Instance()->getXml(vOffset);
#endif

    return str;
}

bool MainFrame::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) {
    UNUSED(vUserDatas);

    // The value of this child identifies the name of this element
    std::string strName = "";
    std::string strValue = "";
    std::string strParentName = "";

    strName = vElem->Value();
    if (vElem->GetText())
        strValue = vElem->GetText();
    if (vParent != nullptr)
        strParentName = vParent->Value();

    // check� et utilis� au chargement de l'app
    if (strName == "useTransparancy")
        StaticConfig::Instance()->puUseTransparancy = ct::ivariant(strValue).GetB();

    if (strParentName == "config") {
        if (strName == "consoleguivisibility")
            puConsoleGuiVisiblity = ct::ivariant(strValue).GetB();
        if (strName == "consoleverbose")
            Logger::Instance()->ConsoleVerbose = ct::ivariant(strValue).GetB();
        if (strName == "ShaderFile")
            puFileToLoad = ct::fvariant(strValue).GetS();
        if (strName == "debug")
            puShowDebug = ct::ivariant(strValue).GetB();
        if (strName == "igfd_places")
            ImGuiFileDialog::Instance()->DeserializePlaces(strValue);
    }

    LayoutManager::Instance()->setFromXml(vElem, vParent, vUserDatas);
    PictureExportSystem::Instance()->setFromXml(vElem, vParent, vUserDatas);

    puOpenFileDialog.setFromXml(vElem, vParent, vUserDatas);
    puNewFileDialog.setFromXml(vElem, vParent, vUserDatas);
    puSavePictureDialog.setFromXml(vElem, vParent, vUserDatas);
    puForkFileDialog.setFromXml(vElem, vParent, vUserDatas);

    CameraSystem::Instance()->setFromXml(vElem, vParent, vUserDatas);
    MainBackend::Instance()->setFromXml(vElem, vParent, vUserDatas);
    RecentFilesSystem::Instance()->setFromXml(vElem, vParent, vUserDatas);

#ifdef USE_NETWORK
    NetCodeRetriever::Instance()->setFromXml(vElem, vParent, vUserDatas);
#endif

    SoundSystem::Instance()->setFromXml(vElem, vParent, vUserDatas);
    MidiSystem::Instance()->setFromXml(vElem, vParent, vUserDatas);
    GamePadSystem::Instance()->setFromXml(vElem, vParent, vUserDatas);
    UrlLibrarySystem::Instance()->setFromXml(vElem, vParent, vUserDatas);
    TimeLineSystem::Instance()->setFromXml(vElem, vParent, vUserDatas);
    ImGuiThemeHelper::Instance()->setFromXml(vElem, vParent, vUserDatas);
    PathSystem::Instance()->setFromXml(vElem, vParent, vUserDatas);

#ifdef USE_OPTIMIZER_SYSTEM
    OptimizerSystem::Instance()->setFromXml(vElem, vParent, vUserDatas);
#endif

    return true;  // continue for explore childs. need to return false if we want explore child ourselves
}
