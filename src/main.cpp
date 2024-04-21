// NoodlesPlate Copyright (C) 2017-2024 Stephane Cuillerdier aka Aiekick
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

// SoGLSL/3rdparty
#include <ctools/FileHelper.h>
#include <ctools/GLVersionChecker.h>
#include <ctools/Logger.h>
#include <Headers/RenderPackHeaders.h>
#include <ImGuiPack.h>
#include <InAppGpuProfiler/InAppGpuProfiler.h>
#ifdef USE_SDL2
#include <ImGuiPack/3rdparty/imgui_docking/backends/imgui_impl_sdl.h>
#else
#include <ImGuiPack/3rdparty/imgui_docking/backends/imgui_impl_glfw.h>
#endif
#include <ImGuiPack/3rdparty/imgui_docking/backends/imgui_impl_opengl3.h>

#include <ImGuiPack.h>

// SoGLSL
#include <SoGLSL/src/Res/CustomFont.cpp>
#include <SoGLSL/src/Res/CustomFont2.cpp>
#include <SoGLSL/src/Res/CustomFontToolBar.cpp>
#include <SoGLSL/src/Res/Roboto_Medium.cpp>
#include <SoGLSL/src/Systems/RenderDocController.h>

// this project
#include <Config/StaticConfig.h>
#include <Gui/MainFrame.h>
#include <Backends/MainBackend.h>

#include <cstdio>

#include <Res/CustomFont.h>
#include <Res/CustomFont2.h>
#include <Panes/ConsolePane.h>

// messaging
#define MESSAGING_CODE_INFOS 0
#define MESSAGING_LABEL_INFOS ICON_NDP_EXCLAMATION_CIRCLE
#define MESSAGING_CODE_WARNINGS 1
#define MESSAGING_LABEL_WARNINGS ICON_NDP_EXCLAMATION_TRIANGLE
#define MESSAGING_CODE_ERRORS 2
#define MESSAGING_LABEL_ERRORS ICON_NDP_TIMES_CIRCLE
#define MESSAGING_TYPE_DEBUG 5
#define MESSAGING_LABEL_DEBUG ICON_NDP2_DEBUG

/////////////////////////////////////////////////////////////

#include <SoGLSL/src/Profiler/TracyProfiler.h>
#ifdef TRACY_ENABLE
void* operator new(size_t count) {
    auto ptr = std::malloc(count);
    TracyAlloc(ptr, count);
    return ptr;
}
void* operator new[](size_t count) {
    auto ptr = std::malloc(count);
    TracyAlloc(ptr, count);
    return ptr;
}
void operator delete(void* ptr) noexcept {
    TracyFree(ptr);
    std::free(ptr);
}
void operator delete[](void* ptr) noexcept {
    TracyFree(ptr);
    std::free(ptr);
}
#endif

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error %d: %s\n", error, description);
}

static void FoundBestOpenGLVersionAvailable(int* vMajor, int* vMinor) {
    // 4.5
    if (GuiBackend::Instance()->IsGlversionSupported(4, 5)) {
        *vMajor = 4;
        *vMinor = 5;
        return;
    }
    // 4.4
    if (GuiBackend::Instance()->IsGlversionSupported(4, 4)) {
        *vMajor = 4;
        *vMinor = 4;
        return;
    }
    // 4.3 => compute
    if (GuiBackend::Instance()->IsGlversionSupported(4, 3)) {
        *vMajor = 4;
        *vMinor = 3;
        return;
    }
    // 4.2
    if (GuiBackend::Instance()->IsGlversionSupported(4, 2)) {
        *vMajor = 4;
        *vMinor = 2;
        return;
    }
    // 4.1
    if (GuiBackend::Instance()->IsGlversionSupported(4, 1)) {
        *vMajor = 4;
        *vMinor = 1;
        return;
    }
    // 4.0 => tesselation
    if (GuiBackend::Instance()->IsGlversionSupported(4, 0)) {
        *vMajor = 4;
        *vMinor = 0;
        return;
    }
    // 3.3 => attrcib lcoation in core
    if (GuiBackend::Instance()->IsGlversionSupported(3, 3)) {
        *vMajor = 3;
        *vMinor = 3;
        return;
    }
    // 3.2 => attrib location in extention
    if (GuiBackend::Instance()->IsGlversionSupported(3, 2)) {
        *vMajor = 3;
        *vMinor = 2;
        return;
    }
    // on charge que les cores. ya pas de core avant 3.2
}

static void Imgui_Calc(ImGuiIO& io) {
    // maintain active, prevent user change via imgui dialog
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Viewport

    ct::ivec4 viewportRect;
    GuiBackend::Instance()->GetWindowSize(MainBackend::sMainThread, &viewportRect.z, &viewportRect.w);
    MainBackend::Instance()->SetRenderSize(viewportRect.size());

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
#ifdef USE_SDL2
    ImGui_ImplSDL2_NewFrame();
#else
    ImGui_ImplGlfw_NewFrame();
#endif
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    MainFrame::Instance()->NewFrame();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        const auto viewport = ImGui::GetMainViewport();
        if (viewport) {
            const auto pos = viewport->WorkPos;
            const auto size = viewport->WorkSize;
            viewportRect.x = (int)pos.x;
            viewportRect.y = (int)pos.y;
            viewportRect.z = (int)size.x;
            viewportRect.w = (int)size.y;
        }
    } else {
        viewportRect.x = 0;
        viewportRect.y = 0;
    }

    if (!MainBackend::sFullScreenWindowed) {
        MainBackend::sNormalWindowSize = viewportRect.size();
        MainBackend::sNormalWindowPos = viewportRect.pos();
    }

    MainFrame::Instance()->Display(viewportRect.zw());
}

static void Imgui_Prepare() {
    ImGui::Render();
}

static void Imgui_Render(const ImGuiIO& io) {
    AIGPScoped("ImGui", "%s", "Rendering");
    GuiBackend::MakeContextCurrent(MainBackend::sMainThread);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call GuiBackend::MakeContextCurrent(window) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        const auto backup_current_context = GuiBackend::Instance()->GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        GuiBackend::MakeContextCurrent(backup_current_context);
    }
}

int main(int, char** argv) {
#ifndef _DEBUG
    BLOCK_DEBUGGER(0);
#endif

    std::setlocale(LC_ALL, ".UTF8");  // this func can affect atof converions...
    std::setlocale(LC_NUMERIC, "C");

    FileHelper::Instance()->SetAppPath(std::string(argv[0]));
#ifdef _DEBUG
#ifdef _MSC_VER
    auto ps = FileHelper::Instance()->ParsePathFileName(FileHelper::Instance()->GetAppPath());
    if (ps.isOk) {
        std::string path = ps.path;
        size_t buildPos = path.find("build");
        if (buildPos != std::string::npos) {
            path = path.substr(0, buildPos) + "bin\\";
            FileHelper::Instance()->SetAppPath(path);
            FileHelper::Instance()->SetCurDirectory(path);
        }
    }
#endif
#else
    FileHelper::Instance()->SetCurDirectory(FileHelper::Instance()->GetAppPath());
#endif

    Messaging::Instance()->AddCategory(MESSAGING_CODE_INFOS, "Infos(s)", MESSAGING_LABEL_INFOS, ImVec4(0.0f, 0.8f, 0.0f, 1.0f));
    Messaging::Instance()->AddCategory(MESSAGING_CODE_WARNINGS, "Warnings(s)", MESSAGING_LABEL_WARNINGS, ImVec4(0.8f, 0.8f, 0.0f, 1.0f));
    Messaging::Instance()->AddCategory(MESSAGING_CODE_ERRORS, "Errors(s)", MESSAGING_LABEL_ERRORS, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
#ifdef _DEBUG
    Messaging::Instance()->AddCategory(MESSAGING_TYPE_DEBUG, "Debug(s)", MESSAGING_LABEL_DEBUG, ImVec4(0.8f, 0.8f, 0.0f, 1.0f));
#endif
    Messaging::Instance()->SetLayoutManager(LayoutManager::Instance());
    Logger::sStandardLogFunction = [](const int& vType, const std::string& vMessage) {
        MessageData msg_datas;
        const auto& type = vType;
        Messaging::Instance()->AddMessage(vMessage, type, false, msg_datas, {});
    };

    LogVarDebugInfo("App path : %s", FileHelper::AppPath.c_str());

    GuiBackend::Instance()->SetErrorCallback(error_callback);

    if (!GuiBackend::Instance()->Init()) {
        return -1;
    }

    StaticConfig::Instance()->LoadConfigFile("config.xml");

    GuiBackend::Instance()->SetGlobalTransparent(StaticConfig::Instance()->puUseTransparancy);
    GuiBackend::Instance()->SetGlobalDecorated(true);

    // minimum version demand�e, mais glfw peut en cree une superieure
    auto opengl_Major = 0, opengl_Minor = 0;
    FoundBestOpenGLVersionAvailable(&opengl_Major, &opengl_Minor);

    // base version
    GuiBackend::Instance()->SetGlobalOpenglVersion(opengl_Major, opengl_Minor);
    MainBackend::Instance()->CreateWindows();
    if (MainBackend::sMainThread.win) {
        GuiBackend::MakeContextCurrent(MainBackend::sMainThread);
        GuiBackend::Instance()->SwapInterval(1);

        if (gladLoadGL() == 0) {
            fprintf(stderr, "Failed to initialize OpenGL loader!\n");
            return 1;
        }

#ifdef TRACY_ENABLE
        TracyGpuContext;
        tracy::SetThreadName("Main");
#endif  // TRACY_ENABLE

#if defined(_DEBUG) && defined(_WIN32)
        ShowWindow(GetConsoleWindow(), SW_SHOW);  // hide console
#endif

        // Setup ImGui binding
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        auto& io = ImGui::GetIO();
        (void)io;
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Viewport
        io.FontAllowUserScaling = true;                      // pour le zoom du nodegraph
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
        io.ConfigViewportsNoDecoration = false;  // toujours mettre une frame au fenetre enfant
#endif
        if (StaticConfig::Instance()->puEnableDPISupport) {
            io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
            LogVarInfo("DPI : Viewport Scale Enabled");
            io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
            LogVarInfo("DPI : Font Scale Enabled");
        }

        io.FontGlobalScale = StaticConfig::Instance()->puGlobalUIScale;
        LogVarInfo("Global UI Scale %.1f", io.FontGlobalScale);

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        auto& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        const auto glslVersion = GLVersionChecker::Instance()->GetGlslVersionHeader();

#ifdef USE_SDL2
        ImGui_ImplSDL2_InitForOpenGL(MainBackend::sMainThread.win, MainBackend::sMainThread.context);
#else
        ImGui_ImplGlfw_InitForOpenGL(MainBackend::sMainThread.win, true);
#endif
        ImGui_ImplOpenGL3_Init(glslVersion.c_str());

        // load memory font file
        ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_RM, 15.0f);
        static ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        static const ImWchar icons_ranges[] = {ICON_MIN_NDP, ICON_MAX_NDP, 0};
        ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_NDP, 15.0f, &icons_config, icons_ranges);
        static const ImWchar icons_ranges2[] = {ICON_MIN_NDP2, ICON_MAX_NDP2, 0};
        ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_NDP2, 15.0f, &icons_config, icons_ranges2);

        ImGui::StyleColorsDark();

        if (MainFrame::Instance()->Init()) {
            std::string appMemoryUsage;

            const auto mainBackendPtr = MainBackend::Instance();

            while (!GuiBackend::Instance()->WindowShouldClose(MainBackend::sMainThread)) {
                RenderDocController::Instance()->StartCaptureIfResquested();

                Imgui_Calc(io);
                Imgui_Prepare();

                mainBackendPtr->NewFrame();
                {
                    AIGPNewFrame("", "GPU Frame");  // a main Zone is always needed
                    if (mainBackendPtr->NewVRFrame()) {
                        mainBackendPtr->RenderVRFrame();
                        mainBackendPtr->RenderVRFBO();
                        mainBackendPtr->EndVRFrame();
                    } else {
                        mainBackendPtr->Render();
                    }
                    Imgui_Render(io);
                    mainBackendPtr->EndFrame();
                }
                AIGPCollect;
                TracyGpuCollect;

                RenderDocController::Instance()->EndCaptureIfResquested();
            }
        }

        GuiBackend::MakeContextCurrent(MainBackend::sMainThread);

        MainFrame::Instance()->Unit();
        MainBackend::Instance()->Unit();
        iagp::InAppGpuProfiler::Instance()->Clear();

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
#ifdef USE_SDL2
        ImGui_ImplSDL2_Shutdown();
#else
        ImGui_ImplGlfw_Shutdown();
#endif
        ImGui::DestroyContext();
    }

    GuiBackend::Instance()->DestroyWindow(MainBackend::sMainThread);

    GuiBackend::Instance()->Terminate();

    return 0;
}
