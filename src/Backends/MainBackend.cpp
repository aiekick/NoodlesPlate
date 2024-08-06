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


#include <Backends/MainBackend.h>

// 3rdparty
#include <ctools/FileHelper.h>
#include <Headers/RenderPackHeaders.h>
#include <glad/glad.h>
#include <ImGuiPack.h>
#include <iagp/iagp.h>
#include <ctools/Logger.h>
#include <ctools/GLVersionChecker.h>
#include <ImGuiPack.h>

// SoGLSL
#include <SoGLSL/src/Gui/GuiBackend.h>
#include <SoGLSL/src/CodeTree/CodeTree.h>
#include <SoGLSL/src/Buffer/FrameBuffer.h>
#include <SoGLSL/src/CodeTree/ShaderKey.h>
#include <SoGLSL/src/Systems/MidiSystem.h>
#include <SoGLSL/src/Systems/SoundSystem.h>
#include <SoGLSL/src/Renderer/RenderPack.h>
#include <SoGLSL/src/Systems/GizmoSystem.h>
#include <SoGLSL/src/Systems/CameraSystem.h>
#include <SoGLSL/src/Texture/TextureSound.h>
#include <SoGLSL/src/VR/Backend/VRBackend.h>
#include <SoGLSL/src/Systems/GamePadSystem.h>
#include <SoGLSL/src/Helper/InterfacePanes.h>
#include <SoGLSL/src/Uniforms/UniformHelper.h>
#include <SoGLSL/src/Profiler/TracyProfiler.h>
#include <SoGLSL/src/Systems/TimeLineSystem.h>
#include <SoGLSL/src/Uniforms/UniformVariant.h>
#include <SoGLSL/src/Importer/ShaderUrlLoader.h>
#include <SoGLSL/src/Importer/NetCodeRetriever.h>
#include <SoGLSL/src/Systems/RecentFilesSystem.h>
#include <SoGLSL/src/Mesh/Operations/MeshSaver.h>
#include <SoGLSL/src/Mesh/Operations/MeshLoader.h>
#include <SoGLSL/src/Buffer/FrameBuffersPipeLine.h>
#include <SoGLSL/src/Systems/RenderDocController.h>
#include <SoGLSL/src/CodeTree/ShaderKeyConfigSwitcherUnified.h>
#include <SoGLSL/src/Res/CustomFont.h>
#include <SoGLSL/src/Res/CustomFont2.h>

// This Project
#include <Generator/CodeGenerator.h>
#include <Systems/PictureExportSystem.h>
#include <Headers/NoodlesPlateBuild.h>
#include <Metrics/MetricSystem.h>
#include <Systems/TemplateSystem.h>
#include <Systems/UrlLibrarySystem.h>
#include <filesystem>

#include <SoGLSL/src/Importer/ShadertoyBackupFileImportDlg.h>

#include <algorithm>

#define MAIN_RENDERPACK_KEY "main"
#define AXIS_RENDERPACK_KEY "3d Axis"
#define GRID_RENDERPACK_KEY "3d Grid"
#define MESH_RENDERPACK_KEY "3d Mesh"
#define BPPLANES_RENDERPACK_KEY "Blueprint Planes"

///////////////////////////////////////////////////////
//// STATIC ///////////////////////////////////////////
///////////////////////////////////////////////////////

GuiBackend_Window MainBackend::sMainThread;
int MainBackend::sFullScreenWindowedMonitorIndex = 0;
std::string MainBackend::sCurrentFileLoaded = std::string();
bool MainBackend::sDecorationWindowed = false;
bool MainBackend::sFullScreenWindowed = false;
ct::ivec2 MainBackend::sFullScreenWindowSize = 0;
ct::ivec2 MainBackend::sNormalWindowSize = ct::ivec2(1280, 720);
ct::ivec2 MainBackend::sNormalWindowPos = 0;
int MainBackend::sCurrentMonitorIndex = 0;

MainBackend::MainBackend() {
    puMouseFrameSize = 0;
}

MainBackend::~MainBackend() {
    puCodeTree->Clear();
    puCodeTree.reset();
}

//////////////////////////////////////////////////////////////////////////

static void window_focus_callback(const GuiBackend_Window& /*window*/, int focused) {
    MainBackend::Instance()->Focus(focused != 0);
}

static void window_pos_callback(const GuiBackend_Window& window, int x, int y) {
    MainBackend::Instance()->SetWindowPos(window, ct::ivec2(x, y));
}

static void glfw_window_drop_callback(const GuiBackend_Window& window, std::vector<std::string> paths) {
    MainBackend::Instance()->JustDropFiles(window, paths);
}

void MainBackend::CreateWindows() {
    const std::string cpuVersion = (sizeof(size_t) > 4U) ? "x64" : "x86";

    static char buffer[1024 + 1];
    snprintf(buffer, 1024, "NoodlesPlate Beta%s %s %s [F10/F11/F12 : Toggle UI/FullScreen/Decoration]",
#ifdef _DEBUG
             " Debug",
#else
             "",
#endif
             cpuVersion.c_str(), NoodlesPlate_BuildId);

    prMainThreadWindowTitle = buffer;

    MainBackend::sMainThread =
        GuiBackend::Instance()->CreateGuiBackendWindow_Visible(sNormalWindowSize.x, sNormalWindowSize.y, prMainThreadWindowTitle.c_str());
    if (MainBackend::sMainThread.win) {
        GuiBackend::MakeContextCurrent(MainBackend::sMainThread);

        GuiBackend::Instance()->SwapInterval(1);  // Enable vsync
        GuiBackend::Instance()->SetWindowFocusCallback(MainBackend::sMainThread, window_focus_callback);
        GuiBackend::Instance()->SetWindowPosCallback(MainBackend::sMainThread, window_pos_callback);
        GuiBackend::Instance()->SetDropCallback(MainBackend::sMainThread, glfw_window_drop_callback);

        GuiBackend::Instance()->SetEmbeddedIconApp(MainBackend::sMainThread, "IDI_ICON1");
    }
}

void MainBackend::UpdateWindowStatus() {
    std::string appMemoryUsage;
    if (MetricSystem::Instance()->GetAppMemoryString(1000, &appMemoryUsage)) {
#ifdef _DEBUG
        const auto title = ct::toStr("%s %s [%i x %i] [Uni %i]", prMainThreadWindowTitle.c_str(), appMemoryUsage.c_str(), puScreenSize.x,
                                     puScreenSize.y, UniformVariant::counter);
#else
        const auto title = ct::toStr("%s %s [%i x %i]", prMainThreadWindowTitle.c_str(), appMemoryUsage.c_str(), puScreenSize.x, puScreenSize.y);
#endif
        GuiBackend::Instance()->SetWindowTitle(MainBackend::sMainThread, title.c_str());
    }

    if (ImGui::IsKeyReleased(GuiBackend_KEY_F11)) {
        sFullScreenWindowed = !sFullScreenWindowed;
        SetFullScreen_Windowed(sFullScreenWindowed);
    }

    if (ImGui::IsKeyReleased(GuiBackend_KEY_F12)) {
        sDecorationWindowed = !sDecorationWindowed;
        GuiBackend::Instance()->SetDecorated(MainBackend::sMainThread, sDecorationWindowed);
    }

    // if (ImGui::IsKeyReleased(GLFW_KEY_PAGE_UP))
    //{
    //	opacityWindowed = ct::clamp(opacityWindowed+0.1f, 0.1f, 1.0f);
    //	glfwSetWindowOpacity(MainBackend::sMainThread, opacityWindowed);
    // }
    //
    // if (ImGui::IsKeyReleased(GLFW_KEY_PAGE_DOWN))
    //{
    //	opacityWindowed = ct::clamp(opacityWindowed-0.1f, 0.1f, 1.0f);
    //	glfwSetWindowOpacity(MainBackend::sMainThread, opacityWindowed);
    // }

    // c'est un changement de moniteur
    if (sCurrentMonitorIndex != sFullScreenWindowedMonitorIndex) {
        SetFullScreen_Windowed(sFullScreenWindowed);
    }
}

//////////////////////////////////////////////////////////////////////////

void MainBackend::Unit() {
    TracyGpuZone("MainBackend::Unit");

    RenderDocController::Instance()->Unit();

    puCodeTree->SaveConfigIncludeFiles();
    puMain_RenderPack->SaveRenderPackConfig(CONFIG_TYPE_Enum::CONFIG_TYPE_ALL);
    pu3dAxis_RenderPack->SaveRenderPackConfig(CONFIG_TYPE_Enum::CONFIG_TYPE_ALL);
    pu3dGrid_RenderPack->SaveRenderPackConfig(CONFIG_TYPE_Enum::CONFIG_TYPE_ALL);
    puMesh_RenderPack->SaveRenderPackConfig(CONFIG_TYPE_Enum::CONFIG_TYPE_ALL);
    GizmoSystem::Instance()->SaveRenderPack();

#ifdef USE_VR
    VRBackend::Instance()->Unit();
#endif

    TextureSound::Release();
    MetricSystem::Instance()->Unit();
    GamePadSystem::Instance()->Unit();
    ;
    SoundSystem::Instance()->Unit();
    MidiSystem::Instance()->Unit();

    puMain_RenderPack->Finish(false);
    pu3dAxis_RenderPack->Finish(false);
    pu3dGrid_RenderPack->Finish(false);
    puMesh_RenderPack->Finish(false);
    GizmoSystem::Instance()->FinishRenderPack();
    SoundSystem::Instance()->FinishRenderPack();

    puMain_RenderPack.reset();
    pu3dAxis_RenderPack.reset();
    pu3dGrid_RenderPack.reset();
    puMesh_RenderPack.reset();
    GizmoSystem::Instance()->DestroyRenderPack();
    SoundSystem::Instance()->DestroyRenderPack();

    puMainPipeLine.reset();

    if (m_AppIconGLTextureID) {
        glDeleteTextures(GL_TEXTURE_2D, &m_AppIconGLTextureID);
        m_AppIconGLTextureID = 0U;
    }
}

bool MainBackend::Init(const ct::ivec2& vScreenSize) {
    TracyGpuZone("MainBackend::Init");

    puCodeTree = CodeTree::Create(); // need imgui

#ifdef USE_THUMBNAILS
    ImGuiFileDialog::Instance()->SetCreateThumbnailCallback([](IGFD_Thumbnail_Info* vThumbnail_Info) {
        if (vThumbnail_Info && vThumbnail_Info->isReadyToUpload && vThumbnail_Info->textureFileDatas) {
            GLuint textureId = 0;
            glGenTextures(1, &textureId);
            vThumbnail_Info->textureID = (void*)(size_t)textureId;

            glBindTexture(GL_TEXTURE_2D, textureId);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)vThumbnail_Info->textureWidth, (GLsizei)vThumbnail_Info->textureHeight, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, vThumbnail_Info->textureFileDatas);
            glBindTexture(GL_TEXTURE_2D, 0);

            delete[] vThumbnail_Info->textureFileDatas;
            vThumbnail_Info->textureFileDatas = nullptr;

            vThumbnail_Info->isReadyToUpload = false;
            vThumbnail_Info->isReadyToDisplay = true;
        }
    });
    ImGuiFileDialog::Instance()->SetDestroyThumbnailCallback([](IGFD_Thumbnail_Info* vThumbnail_Info) {
        if (vThumbnail_Info) {
            GLuint texID = (GLuint)(size_t)vThumbnail_Info->textureID;
            glDeleteTextures(1, &texID);
        }
    });
#endif  // USE_THUMBNAILS

    bool res = true;

    MetricSystem::Instance()->Init();

    FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_SCRIPT, "scripts");
    FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_GENERAL, "assets");
    FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_TEXTURE_2D, "assets");
    FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_TEXTURE_3D, "assets");
    FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_CUBEMAP, "assets");
    FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_SOUND, "assets");
    FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_MESH, "assets");
    FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_EXPORT, "exports");
    FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_CONF, "conf");
    FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_DEBUG, "debug");
    FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_SHAPES, "shapes");
    FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_IMPORT, "imports");
    FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_SCRIPT_INCLUDE, "");
    FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_APP, "");

    FileHelper::Instance()->puSearchPaths.emplace_back("predefined");
    FileHelper::Instance()->puSearchPaths.emplace_back("predefined" + FileHelper::Instance()->puSlashType + "shaders");
    FileHelper::Instance()->puSearchPaths.emplace_back("predefined" + FileHelper::Instance()->puSlashType + "helpers");

    puPixelDebug.Init();

    m_AppIconGLTextureID = GuiBackend::Instance()->ExtractEmbeddedImageToGLTexture("IDB_BMP1");

    MeshLoader::Instance(MainBackend::sMainThread);
    MeshSaver::Instance(MainBackend::sMainThread);
    TextureSound::Init();

    RenderDocController::Instance()->Init();

#ifdef USE_NETWORK
    ShaderUrlLoader::InitCurl();
#endif  // #ifdef USE_NETWORK

    GizmoSystem::Instance()->Init(nullptr);
    GamePadSystem::Instance()->Init(nullptr);
    SoundSystem::Instance()->Init(nullptr);
    MidiSystem::Instance()->Init(nullptr);
    TemplateSystem::Instance()->Init();
    CameraSystem::Instance()->NeedCamChange();

#ifdef USE_NETWORK
    NetCodeRetriever::Instance()->SetShaderToyApiKey("ApiKey");
    NetCodeRetriever::Instance()->SetUrl("type the url here");
    NetCodeRetriever::Instance()->SetProxyPath("ip:port");
    NetCodeRetriever::Instance()->SetProxyUserPwd("user:paswword");
#endif  // #ifdef USE_NETWORK

    ShadertoyBackupFileImportDlg::Instance()->SetFunction_For_CreateManyFilesShader(
        std::bind(&MainBackend::FinalizeShaderImport_By_CreateFiles, this, std::placeholders::_1, std::placeholders::_2));

    ShadertoyBackupFileImportDlg::Instance()->SetFunction_For_CreateOneFileShader(
        std::bind(&MainBackend::FinalizeShaderImport_By_CreateOneFile, this, std::placeholders::_1, std::placeholders::_2));

    SetConsoleVisibility(false);

    puMonitorsName = GuiBackend::Instance()->GetMonitors();

    puScreenSize = vScreenSize;

    ShaderKeyPtr mainKey = nullptr;  // puCodeTree->AddShaderKey("");
    puMain_RenderPack = RenderPack::createBufferWithFileWithoutLoading(
        MainBackend::sMainThread,    // thread
        MAIN_RENDERPACK_KEY,         // tag
        ct::ivec3(puScreenSize, 0),  // fbo size
        mainKey,                     // script
        true,                        // zbuffer // pour l'opprotunite de charger du points ou du mesh il seviera pas si c'est du quads
        true,                        // fbo pour le multipass
        false,                       // renderbuffer
        false                        // floatbuffer
    );

    if (puMain_RenderPack) {
        puMain_RenderPack->puShowBlendingButton = true;
        puMain_RenderPack->puShowCullingButton = true;
        puMain_RenderPack->puShowZBufferButton = true;
        puMain_RenderPack->puShowTransparentButton = false;
        puMain_RenderPack->puShowOpenModelButton = true;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////

    auto path = FileHelper::Instance()->GetExistingFilePathForFile("space3d.glsl", true);
    if (path.empty()) {
        path = "predefined/helpers/space3d.glsl";
        CodeGenerator::Instance()->CreateFilePathName(path, "Helper_Space3d", "", false);
    }
    puFileListingExceptions.emplace(path);

    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////

    path = FileHelper::Instance()->GetExistingFilePathForFile("3dAxis.glsl", true);
    if (path.empty()) {
        path = "predefined/shaders/3dAxis.glsl";
        CodeGenerator::Instance()->CreateFilePathName(path, "Shader_3dAxis", "", false);
    }
    puFileListingExceptions.emplace(path);
    puTriDAxisKey = puCodeTree->LoadFromFile(path, KEY_TYPE_Enum::KEY_TYPE_SHADER);
    pu3dAxis_RenderPack = RenderPack::createBufferWithFileWithoutLoading(MainBackend::sMainThread, AXIS_RENDERPACK_KEY, ct::ivec3(puScreenSize, 0),
                                                                         puTriDAxisKey, true, false);
    if (pu3dAxis_RenderPack) {
        pu3dAxis_RenderPack->puShowZBufferButton = true;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////

    path = FileHelper::Instance()->GetExistingFilePathForFile("3dGrid.glsl", true);
    if (path.empty()) {
        path = "predefined/shaders/3dGrid.glsl";
        CodeGenerator::Instance()->CreateFilePathName(path, "Shader_3dGrid", "", false);
    }
    puFileListingExceptions.emplace(path);
    puTriDGridKey = puCodeTree->LoadFromFile(path, KEY_TYPE_Enum::KEY_TYPE_SHADER);
    pu3dGrid_RenderPack = RenderPack::createBufferWithFileWithoutLoading(MainBackend::sMainThread, GRID_RENDERPACK_KEY, ct::ivec3(puScreenSize, 0),
                                                                         puTriDGridKey, true, false);
    if (pu3dGrid_RenderPack) {
        pu3dGrid_RenderPack->puShowZBufferButton = true;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////

    path = FileHelper::Instance()->GetExistingFilePathForFile("mesh.glsl", true);
    if (path.empty()) {
        path = "predefined/shaders/mesh.glsl";
        CodeGenerator::Instance()->CreateFilePathName(path, "Shader_Mesh", "", false);
    }
    puFileListingExceptions.emplace(path);
    puMeshKey = puCodeTree->LoadFromFile(path, KEY_TYPE_Enum::KEY_TYPE_SHADER);
    puMesh_RenderPack = RenderPack::createBufferWithFileWithoutLoading(MainBackend::sMainThread, MESH_RENDERPACK_KEY, ct::ivec3(puScreenSize, 0),
                                                                       puMeshKey, true, false);
    if (puMesh_RenderPack) {
        puMesh_RenderPack->puShowBlendingButton = true;
        puMesh_RenderPack->puShowCullingButton = true;
        puMesh_RenderPack->puShowZBufferButton = true;
        puMesh_RenderPack->puShowTransparentButton = false;
        puMesh_RenderPack->puShowOpenModelButton = true;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////

    path = GizmoSystem::Instance()->InitRenderPack(MainBackend::sMainThread, puCodeTree);
    path = SoundSystem::Instance()->InitRenderPack(MainBackend::sMainThread, puCodeTree);
    puFileListingExceptions.emplace(path);

    if (!puMain_RenderPack ||                                  //
        !pu3dAxis_RenderPack ||                                //
        !pu3dGrid_RenderPack ||                                //
        !puMesh_RenderPack ||                                  //
        GizmoSystem::Instance()->GetRenderPack().expired() ||  //
        SoundSystem::Instance()->GetRenderPack().expired())    //
        res = false;

    PreCalc_Menu_DisplayQuality();

    return res;
}

bool MainBackend::Load(const std::string& vFileToLoad) {
    TracyGpuZone("MainBackend::Load");

    auto res = true;

    UniformHelper::FBOSizeForMouseUniformNormalization = ct::convert(puScreenSize) / puDisplayQuality;

    puMainPipeLine = FrameBuffersPipeLine::Create(MainBackend::sMainThread, ct::ivec3(puScreenSize, 0), "float", puUseFXAA, puCountFXAASamples, true);
    if (!puMainPipeLine) {
        return false;
    }

    // on applique musefxaa avant le chargement des renderpacks
    puMain_RenderPack->puUseFXAA = puUseFXAA;
    puMain_RenderPack->puCountFXAASamples = puCountFXAASamples;

    pu3dAxis_RenderPack->puUseFXAA = puUseFXAA;
    pu3dAxis_RenderPack->puCountFXAASamples = puCountFXAASamples;

    pu3dGrid_RenderPack->puUseFXAA = puUseFXAA;
    pu3dGrid_RenderPack->puCountFXAASamples = puCountFXAASamples;

    puMesh_RenderPack->puUseFXAA = puUseFXAA;
    puMesh_RenderPack->puCountFXAASamples = puCountFXAASamples;

    GizmoSystem::Instance()->SetRenderPackFXAA(puUseFXAA, puCountFXAASamples);

    puDisplay_RenderPack.reset();

    SetCustomWidgetsOfRenderPack(puMain_RenderPack);
    SetCustomWidgetsOfRenderPack(pu3dAxis_RenderPack);
    SetCustomWidgetsOfRenderPack(pu3dGrid_RenderPack);
    SetCustomWidgetsOfRenderPack(puMesh_RenderPack);
    SetCustomWidgetsOfRenderPack(GizmoSystem::Instance()->GetRenderPack());
    SetCustomWidgetsOfRenderPack(SoundSystem::Instance()->GetRenderPack());

    res &= puMain_RenderPack->Load();
    res &= pu3dAxis_RenderPack->Load();
    res &= pu3dGrid_RenderPack->Load();
    res &= puMesh_RenderPack->Load();
    res &= GizmoSystem::Instance()->LoadRenderPack();
    res &= SoundSystem::Instance()->LoadRenderPack();
    res &= puMainPipeLine->load();

    InitFilesTracker();

    if (!vFileToLoad.empty()) {
        LoadFilePathName(vFileToLoad, ".glsl", false);
    }

    DoShaderCodeUpdate();
    puCodeTree->FillIncludeFileList();
    puDisplay_RenderPack = puMain_RenderPack;

    CameraSystem::Instance()->NeedCamChange();
    MainBackend::Instance()->NeedRefresh();

    PreCalc_Menu_DisplayQuality();

    return res;
}

bool MainBackend::IsLoaded() {
    return (puMain_RenderPack->GetShaderKey() != nullptr);
}

void MainBackend::NewFrame() {
    ZoneScoped;
    GuiBackend::MakeContextCurrent(MainBackend::sMainThread);
    FrameMark;

    UpdateDeltaTime();
    UpdateCameraAndMouse();
    UpdateSound();
    UpdateMidi();
    CheckIfTheseAreSomeFileChanges();
    UpdateWindowStatus();
    FinishThreadsIfRequired();
    ResizeIfNeeded();

#ifdef USE_THUMBNAILS
    ImGuiFileDialog::Instance()->ManageGPUThumbnails();
#endif
}

void MainBackend::EndFrame() {
    MidiSystem::Instance()->updateMidiNeeded = false;
    // SoundSystem::Instance()->updateMidiNeeded = false;
    PictureExportSystem::Instance()->ScreenGrabbIfNeeded(MainBackend::sMainThread);
    {
        AIGPScoped("Opengl", "%s", "Swap buffers");
        GuiBackend::Instance()->SwapBuffers(MainBackend::sMainThread);
    }
}

void MainBackend::StartOrStopVR() {
#ifdef USE_VR
    if (!VRBackend::Instance()->IsLoaded()) {
        if (VRBackend::Instance()->Init(MainBackend::Instance()->puCodeTree)) {
            // things to do after start
        }
    } else {
        VRBackend::Instance()->Unit();
    }
#endif
}

bool MainBackend::NewVRFrame() {
#ifdef USE_VR
    return VRBackend::Instance()->NewFrame();
#else
    return false;
#endif  // USE_VR
}

void MainBackend::RenderVRFrame() {
    DoRenderingVR();
}

void MainBackend::RenderVRFBO() {
#ifdef USE_VR
    VRBackend::Instance()->RenderFBO(puScreenSize, puBackgroundColor);
#endif  // USE_VR
}

void MainBackend::EndVRFrame() {
#ifdef USE_VR
    VRBackend::Instance()->EndFrame();
#endif  // USE_VR
}

void MainBackend::Focus(bool vFocused) {
    if (vFocused) {
        GainFocus();
    } else {
        LooseFocus();
    }
}

void MainBackend::SetWindowPos(const GuiBackend_Window& vWindow, const ct::ivec2& vPos) {
    if (vWindow.win == MainBackend::sMainThread.win) {
        const auto isMaximized = (bool)GuiBackend::Instance()->IsMaximized(MainBackend::sMainThread);
        if (!isMaximized) {
            puAppRectIfNotmaximized.x = vPos.x;
            puAppRectIfNotmaximized.y = vPos.y;
        }
    }
}

void MainBackend::JustDropFiles(const GuiBackend_Window& vWindow, std::vector<std::string> paths) {
    if (vWindow.win == MainBackend::sMainThread.win) {
        for (const auto& file : paths) {
            auto ps = FileHelper::Instance()->ParsePathFileName(file);
            if (ps.isOk) {
                if (ps.ext == "glsl") {
                    LoadFilePathName(file);
                    return;
                }
            }
        }
    }
}

void MainBackend::SetFullScreen_Windowed(bool vFullScreen) {
    ct::ivec2 newSize;
    ct::ivec2 newPos;

    if (vFullScreen) {
        auto mon = GuiBackend::Instance()->GetPrimaryMonitor();
        if (sFullScreenWindowedMonitorIndex < (int)puMonitors.size())
            if (puMonitors[sFullScreenWindowedMonitorIndex].id)
                mon = puMonitors[sFullScreenWindowedMonitorIndex];

        const auto vidMode = GuiBackend::Instance()->GetVideoMode(mon);

        sFullScreenWindowSize.x = vidMode.width;
        sFullScreenWindowSize.y = vidMode.height;

        newSize = sFullScreenWindowSize;

        int monX, monY;
        GuiBackend::Instance()->GetMonitorPos(mon, &monX, &monY);
        newPos = ct::ivec2(monX, monY);
    } else {
        newSize = sNormalWindowSize;
        newPos = sNormalWindowPos;
    }

    GuiBackend::Instance()->SetWindowSize(MainBackend::sMainThread, newSize.x, newSize.y);
    GuiBackend::Instance()->SetWindowPos(MainBackend::sMainThread, newPos.x, newPos.y);

    sCurrentMonitorIndex = sFullScreenWindowedMonitorIndex;
}

void MainBackend::SetRenderSize(const ct::ivec2& vNewSize) {
    if (puScreenSize != vNewSize) {
        NeedResize(vNewSize, puDisplayQuality);
    }
}

void MainBackend::SetRenderQuality(const float& vNewQuality) {
    NeedResize(puScreenSize, vNewQuality);
}

void MainBackend::NeedColorClearing()  // will clear the fbos
{
    prClearingIsNeeded = true;
}

void MainBackend::NeedResize(const ct::ivec2& vNewSize, const float& vNewQuality) {
    prNewSize = vNewSize;
    prNewQuality = vNewQuality;
    prResizeISNeeded = true;
}

void MainBackend::ResizeIfNeeded() {
    if (prResizeISNeeded) {
        ReSize(prNewSize, prNewQuality, prResizeISForced);
        prResizeISNeeded = false;
        prResizeISForced = false;
        PreCalc_Menu_DisplayQuality();
    }
    if (prClearingIsNeeded) {
        ApplyColorClearing();
        prClearingIsNeeded = false;
    }
}

void MainBackend::UpdateCamera(const bool& vForce) {
    prPipeSize = puScreenSize;
    if ((puShow3DSpace || puUseFXAA || puShowMesh  //
         || GizmoSystem::Instance()->UseCulling()  //
         ) &&
        puMain_RenderPack->puCanBeIntegratedInExternalPipeline) {
        prPipeSize = puMainPipeLine->size.xy();
    } else {
        const auto& s = puMain_RenderPack->puMeshRect.Size();
        prPipeSize = ct::ivec2((int)s.x, (int)s.y);
    }
    if (vForce) {
        CameraSystem::Instance()->NeedCamChange();
    }
    CameraSystem::Instance()->UpdateIfNeeded(prPipeSize);
}

bool MainBackend::ReSize(const ct::ivec2& vNewSize, const float& vQuality, bool vForceResize) {
    auto res = false;

    auto newSize = vNewSize;

    if (!newSize.emptyAND()) {
        auto FBOSize = ct::convert(newSize) / vQuality;
        const auto maxTexSize = GLVersionChecker::Instance()->m_OpenglInfosStruct.maxTextureSize;
        if (FBOSize > 0.0f && FBOSize <= (float)maxTexSize) {
            auto fboSize = ct::ivec3((int)FBOSize.x, (int)FBOSize.y, 0);
            res = puMain_RenderPack->Resize(fboSize, vForceResize);
            res &= pu3dAxis_RenderPack->Resize(fboSize, vForceResize);  // pour update le meshRect pour les interaction camera
            res &= pu3dGrid_RenderPack->Resize(fboSize, vForceResize);  // pour update le meshRect pour les interaction camera
            res &= puMesh_RenderPack->Resize(fboSize, vForceResize);    // pour update le meshRect pour les interaction camera
            res &= puMainPipeLine->Resize(fboSize);

            if (!res)  // on revient comme avant
            {
                FBOSize = puDisplaySize / puDisplayQuality;
                fboSize = ct::ivec3((int)FBOSize.x, (int)FBOSize.y, 0);
                puMain_RenderPack->Resize(fboSize, vForceResize);
                pu3dAxis_RenderPack->Resize(fboSize, vForceResize);  // pour update le meshRect pour les interaction camera
                pu3dGrid_RenderPack->Resize(fboSize, vForceResize);  // pour update le meshRect pour les interaction camera
                puMesh_RenderPack->Resize(fboSize, vForceResize);    // pour update le meshRect pour les interaction camera
                puMainPipeLine->Resize(fboSize);
                puPixelDebug.Resize(puMain_RenderPack, puDisplayQuality, CameraSystem::Instance());
            }
        } else {
            if (FBOSize.x > maxTexSize || FBOSize.y > maxTexSize)
                LogVarError("Can't Create FBO size more than %i =>", maxTexSize, FBOSize.string().c_str());
            else if (FBOSize.x < 0.0f || FBOSize.y < 0.0f)
                LogVarError("The Calculated new FBO Size is negative : %s", FBOSize.string().c_str());
        }

        if (res) {
            UniformHelper::FBOSizeForMouseUniformNormalization = (puDisplaySize / puDisplayQuality);
            UniformHelper::FBOSize = FBOSize;
            puCodeTree->ReScaleMouseUniforms(UniformHelper::FBOSize);

            puScreenSize = newSize;
            puDisplaySize = ct::convert(newSize);
            puDisplayQuality = vQuality;

            puPixelDebug.Resize(puMain_RenderPack, puDisplayQuality, CameraSystem::Instance());

            UpdateCamera();

            const auto isMaximized = (bool)GuiBackend::Instance()->IsMaximized(MainBackend::sMainThread);
            if (!isMaximized) {
                puAppRectIfNotmaximized = GetWindowSize();
            }
            NeedRefresh(true);
        }
    }

    return res;
}

void MainBackend::ApplyColorClearing() {
    puMain_RenderPack->ClearColor();
    pu3dAxis_RenderPack->ClearColor();
    pu3dGrid_RenderPack->ClearColor();
    puMesh_RenderPack->ClearColor();
    puMainPipeLine->ClearColor();
    NeedRefresh(true);
}

void MainBackend::SetCustomWidgetsOfRenderPack(RenderPackWeak vRenderPack) {
    auto rpPtr = vRenderPack.lock();
    if (rpPtr) {
        rpPtr->AddCustomWidgetNameAndPropagateToChilds(uType::uTypeEnum::U_FLOAT, "usemsaa", 0);
        rpPtr->AddCustomWidgetNameAndPropagateToChilds(uType::uTypeEnum::U_FLOAT, "show3dSpace", 0);
        rpPtr->AddCustomWidgetNameAndPropagateToChilds(uType::uTypeEnum::U_INT, "currentattachment", 0);
        rpPtr->AddCustomWidgetNameAndPropagateToChilds(uType::uTypeEnum::U_SAMPLER2D, "desktop", 0);
        rpPtr->AddCustomWidgetNameAndPropagateToChilds(uType::uTypeEnum::U_VEC4, "bgcolor", 0);
    }
}

bool MainBackend::UpdateFXAA() {
    const auto res = false;

    puMainPipeLine->SetFXAAUse(puUseFXAA, puCountFXAASamples);
    puMain_RenderPack->SetFXAAUse(puUseFXAA, puCountFXAASamples);
    pu3dAxis_RenderPack->SetFXAAUse(puUseFXAA, puCountFXAASamples);
    pu3dGrid_RenderPack->SetFXAAUse(puUseFXAA, puCountFXAASamples);
    puMesh_RenderPack->SetFXAAUse(puUseFXAA, puCountFXAASamples);

    auto gizmoPtr = GizmoSystem::Instance()->GetRenderPack().lock();
    if (gizmoPtr) {
        gizmoPtr->SetFXAAUse(puUseFXAA, puCountFXAASamples);
    }

    return res;
}

bool MainBackend::ForceReCompilation() {
    return DoShaderCodeUpdate(std::set<std::string>(), true);
}

bool MainBackend::DoShaderCodeUpdateOfRenderPack(RenderPackWeak vRenderPack, bool vForceUpdate, std::set<std::string>* vUpdatedFiles) {
    auto codeChange = false;

    TracyGpuZone("MainBackend::DoShaderCodeUpdateOfRenderPack");

    auto rpPtr = vRenderPack.lock();
    if (rpPtr) {
        auto key = rpPtr->GetShaderKey();
        if (key) {
            if (vUpdatedFiles) {
                // on va check si la key utilise les fichier de vUpdatedFiles
                // si c'est le cas, on force l'update
                for (const auto& file : key->puUsedFileNames) {
                    for (auto filename : file.second) {
                        // ce code a ni queue ni tete, pouruqoi cette boucle si c'est pour tester key->puKey
                        if (vUpdatedFiles->find(filename) != vUpdatedFiles->end())  // non trouvé
                        {
                            vForceUpdate = true;
                            break;
                        }
                    }
                }
            }

            if (vForceUpdate) {
                codeChange |= key->UpdateIfChange(true, false, true);
            } else {
                if (vUpdatedFiles) {
                    if (vUpdatedFiles->find(key->puKey) == vUpdatedFiles->end())  // non trouvé
                    {
                        return codeChange;
                    }
                }
            }

            codeChange |= rpPtr->UpdateShaderChanges(vForceUpdate);
        }
    }

    return codeChange;
}

bool MainBackend::DoShaderCodeUpdate(std::set<std::string> vFiles, bool vForceUpdate) {
    auto codeChange = false;

    TracyGpuZone("MainBackend::DoShaderCodeUpdate");

    std::set<std::string> glslFilesToUpdatePossibly;
    for (const auto& file : vFiles) {
        auto ps = FileHelper::Instance()->ParsePathFileName(file);
        if (ps.isOk) {
            if (ps.ext == "glsl") {
                glslFilesToUpdatePossibly.emplace(file);
            }
        }
    }

    // if (glslFilesToUpdatePossibly.empty())
    //	CTOOL_DEBUG_BREAK;

    if (!glslFilesToUpdatePossibly.empty() || vForceUpdate) {
        // auto countUniforms = UniformVariant::counter;
        codeChange |= DoShaderCodeUpdateOfRenderPack(puMain_RenderPack, vForceUpdate, &glslFilesToUpdatePossibly);
        for (auto it : puMain_RenderPack->puBuffers) {
            codeChange |= DoShaderCodeUpdateOfRenderPack(it, vForceUpdate, &glslFilesToUpdatePossibly);
        }
        for (auto it : puMain_RenderPack->puSceneBuffers) {
            auto itPtr = it.lock();
            if (itPtr) {
                codeChange |= DoShaderCodeUpdateOfRenderPack(it, vForceUpdate, &glslFilesToUpdatePossibly);
                for (auto& sit : itPtr->puBuffers) {
                    codeChange |= DoShaderCodeUpdateOfRenderPack(sit, vForceUpdate, &glslFilesToUpdatePossibly);
                }
            }
        }

         // countUniforms = UniformVariant::counter;
        codeChange |= DoShaderCodeUpdateOfRenderPack(pu3dAxis_RenderPack, vForceUpdate, &glslFilesToUpdatePossibly);
        for (auto it : pu3dAxis_RenderPack->puBuffers) {
            codeChange |= DoShaderCodeUpdateOfRenderPack(it, vForceUpdate, &glslFilesToUpdatePossibly);
        }
        // countUniforms = UniformVariant::counter;
        codeChange |= DoShaderCodeUpdateOfRenderPack(pu3dGrid_RenderPack, vForceUpdate, &glslFilesToUpdatePossibly);
        for (auto it : pu3dGrid_RenderPack->puBuffers) {
            codeChange |= DoShaderCodeUpdateOfRenderPack(it, vForceUpdate, &glslFilesToUpdatePossibly);
        }
        // countUniforms = UniformVariant::counter;
        codeChange |= DoShaderCodeUpdateOfRenderPack(puMesh_RenderPack, vForceUpdate, &glslFilesToUpdatePossibly);
        for (auto it : puMesh_RenderPack->puBuffers) {
            codeChange |= DoShaderCodeUpdateOfRenderPack(it, vForceUpdate, &glslFilesToUpdatePossibly);
        }
        if (GizmoSystem::Instance()->UseCulling()) {
            // countUniforms = UniformVariant::counter;
            auto gizmoRP = GizmoSystem::Instance()->GetRenderPack().lock();
            if (gizmoRP) {
                codeChange |= DoShaderCodeUpdateOfRenderPack(gizmoRP, vForceUpdate, &glslFilesToUpdatePossibly);
                for (auto it : gizmoRP->puBuffers) {
                    codeChange |= DoShaderCodeUpdateOfRenderPack(it, vForceUpdate, &glslFilesToUpdatePossibly);
                }
            }
        }

        // codeChange |= DoShaderCodeUpdateOfRenderPack(SoundSystem::Instance()->GetRenderPack(), vForceUpdate, &glslFilesToUpdatePossibly);

        if (codeChange) {
            //countUniforms = UniformVariant::counter;
            puCodeTree->FillIncludeFileList();
            //countUniforms = UniformVariant::counter;

            NeedRefresh(true);
            CameraSystem::Instance()->NeedCamChange();

            // pourquoi commuter la pause ?
            // SetPlayPause(puPlayCounterTimers);

            InterfacePanes::Instance()->puForceRefreshCode = true;

            PictureExportSystem::Instance()->TakeScreenShotForEachModif(false);

        } else {
            // CTOOL_DEBUG_BREAK;
            // LogVar("DoShaderCodeUpdate fail to update. paths not corresponds with shaderkey paths");
        }
    } else {
        // CTOOL_DEBUG_BREAK;
    }

    NeedRefresh(codeChange);

    return codeChange;
}

void MainBackend::InitFilesTracker() {
    // add a watch to the system
    // the file watcher doesn't manage the pointer to the listener - so make sure you don't just
    // allocate a listener here and expect the file watcher to manage it - there will be a leak!

    std::set<std::string> set_files;

    for (int i = 0; i < (int)FILE_LOCATION_Enum::FILE_LCOATION_Count; ++i) {
        set_files.emplace(FileHelper::Instance()->GetRegisteredPath(i));
    }

    std::list<std::string> files;
    for (auto f : set_files)
        files.emplace_back(f);

    for (auto it = FileHelper::Instance()->puSearchPaths.begin(); it != FileHelper::Instance()->puSearchPaths.end(); ++it) {
        files.emplace_front(*it);
    }

    puCodeTree->InitFilesTracker(std::bind(&MainBackend::DoFileChanges, this, std::placeholders::_1), files);
}

void MainBackend::DoFileChanges(const std::set<std::string>& vFiles) {
    if (puMain_RenderPack->IsLoaded()) {
        NeedRefresh(DoShaderCodeUpdate(vFiles));
    }
}

void MainBackend::CheckIfTheseAreSomeFileChanges() {
    puCodeTree->CheckIfTheseAreSomeFileChanges();
}

ct::ivec4 MainBackend::GetWindowSize() {
    ct::ivec4 vSize;
    GuiBackend::Instance()->GetWindowPos(MainBackend::sMainThread, &vSize.x, &vSize.y);
    GuiBackend::Instance()->GetWindowSize(MainBackend::sMainThread, &vSize.z, &vSize.w);
    return vSize;
}

void MainBackend::GainFocus() {
    puJustRecoveredFocus = true;
}

void MainBackend::LooseFocus() {
}

void MainBackend::clearBG() {
    glClearColor(puBackgroundColor.x, puBackgroundColor.y, puBackgroundColor.z, puBackgroundColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
}

void MainBackend::FinishThreadsIfRequired() {
    GuiBackend::MakeContextCurrent(MainBackend::sMainThread);

    NeedRefresh(MeshLoader::Instance()->FinishIfRequired());
    NeedRefresh(MeshSaver::Instance()->FinishIfRequired());
}

ImVec2 MainBackend::GetWindowMousePos() {
    double mouse_x, mouse_y;
    GuiBackend::Instance()->GetCursorPos(MainBackend::sMainThread, &mouse_x, &mouse_y);
    return ImVec2((float)mouse_x, (float)mouse_y);
}

void MainBackend::UpdateMouseDatas(bool vCanUseMouse) {
    for (auto mb = 0; mb < 3; mb++) {
        const auto imd = (GuiBackend::Instance()->GetMouseButton(MainBackend::sMainThread, mb) == GuiBackend_MOUSE_PRESS);
        puMouseInterface.buttonDownLastFrame[mb] = puMouseInterface.buttonDown[mb];
        puMouseInterface.buttonDown[mb] = imd && vCanUseMouse;
    }

    puMouseInterface.canUpdateMouse =
        puCanWeTuneMouse && (puMouseInterface.buttonDown[0] || puMouseInterface.buttonDown[1] || puMouseInterface.buttonDown[2]);

    if (vCanUseMouse) {
        // puMouseInterface pos is needed for camera and mouse
        // is will be needed to clarify that.
        // normally the mouse interface is only for mouse
        const auto p = GetWindowMousePos();
        puMouseInterface.px = p.x;
        puMouseInterface.py = p.y;
    }
}

void MainBackend::UpdateDeltaTime() {
    ZoneScoped;

    int64_t timeMeasure = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    if (prLastTimeMeasure) {
        prDeltaTime = (timeMeasure - prLastTimeMeasure) * 0.001f;
    }

    prLastTimeMeasure = timeMeasure;
}

void MainBackend::UpdateCameraAndMouse() {
    ZoneScoped;

    // update camera of renderpack
    GamePadSystem::Instance()->Update();
    if (GamePadSystem::Instance()->WasChanged()) {
        CameraSystem::Instance()->m_NeedCamChange = true;
        GamePadSystem::Instance()->ResetChange();
    }

    if (!puJustRecoveredFocus && !ImGuizmo::IsUsing())  // permet d'eviter de creer un gouffre entre normalizedMousePos et puLastNormalizedMousePos
    {
        auto rpPtr = puDisplay_RenderPack.lock();
        if (rpPtr) {
            if (rpPtr && puDisplayQuality >= 0.0f) {
                const auto mousePos = ct::fvec2(puMouseInterface.px, puMouseInterface.py) / puDisplayQuality;
                auto meshRect = rpPtr->puMeshRect;

                if (ImGui::IsKeyPressed(GuiBackend_KEY_UP)) {
                    if (CameraSystem::Instance()->m_CameraSettings.m_CameraMode == CAMERA_MODE_Enum::CAMERA_MODE_FREE) {
                        CameraSystem::Instance()->IncFlyingPosition(CameraSystem::Instance()->m_CameraSettings.m_SpeedFactor);
                    }
                } else if (ImGui::IsKeyPressed(GuiBackend_KEY_DOWN)) {
                    if (CameraSystem::Instance()->m_CameraSettings.m_CameraMode == CAMERA_MODE_Enum::CAMERA_MODE_FREE) {
                        CameraSystem::Instance()->IncFlyingPosition(-CameraSystem::Instance()->m_CameraSettings.m_SpeedFactor);
                    }
                }

                if (puMouseInterface.buttonDown[0])  // left mouse rotate
                {
                    if (puCanWeTuneCamera || puCanWeTuneMouse) {
                        if (meshRect.IsContainPoint(mousePos)) {
                            puNormalizedMousePos.x = (mousePos.x - meshRect.x) / meshRect.w;
                            puNormalizedMousePos.y = (mousePos.y - meshRect.y) / meshRect.h;
                        }

                        if (!puMouseDrag)
                            puLastNormalizedMousePos = puNormalizedMousePos;
                        puMouseDrag = true;
                    }

                    auto diff = puNormalizedMousePos - puLastNormalizedMousePos;

                    if (puCanWeTuneCamera)
                        CameraSystem::Instance()->IncRotateXYZ(ct::fvec3(diff * 6.28318f, 0.0f));

                    if (puCanWeTuneCamera || puCanWeTuneMouse) {
                        NeedRefresh(!diff.emptyAND());
                        puLastNormalizedMousePos = puNormalizedMousePos;
                    }
                } else if (puMouseInterface.buttonDown[1])  // right mouse zoom y and rotate x
                {
                    if (puCanWeTuneCamera || puCanWeTuneMouse) {
                        if (meshRect.IsContainPoint(mousePos)) {
                            puNormalizedMousePos.x = (mousePos.x - meshRect.x) / meshRect.w;
                            puNormalizedMousePos.y = (mousePos.y - meshRect.y) / meshRect.h;
                        }

                        if (!puMouseDrag)
                            puLastNormalizedMousePos = puNormalizedMousePos;
                        puMouseDrag = true;
                    }

                    auto diff = puNormalizedMousePos - puLastNormalizedMousePos;

                    if (puCanWeTuneCamera) {
                        CameraSystem::Instance()->IncZoom(diff.y * 50.0f);
                        CameraSystem::Instance()->IncRotateXYZ(ct::fvec3(0.0f, 0.0f, diff.x * 6.28318f));
                    }

                    if (puCanWeTuneCamera || puCanWeTuneMouse) {
                        NeedRefresh(!diff.emptyAND());
                        puLastNormalizedMousePos = puNormalizedMousePos;
                    }
                } else if (puMouseInterface.buttonDown[2])  // middle mouse, translate
                {
                    if (puCanWeTuneCamera || puCanWeTuneMouse) {
                        if (meshRect.IsContainPoint(mousePos)) {
                            puNormalizedMousePos.x = (mousePos.x - meshRect.x) / meshRect.w;
                            puNormalizedMousePos.y = (mousePos.y - meshRect.y) / meshRect.h;
                        }

                        if (!puMouseDrag)
                            puLastNormalizedMousePos = puNormalizedMousePos;
                        puMouseDrag = true;
                    }

                    auto diff = puNormalizedMousePos - puLastNormalizedMousePos;

                    if (puCanWeTuneCamera)
                        CameraSystem::Instance()->IncTranslateXY(diff * 10.0f);

                    if (puCanWeTuneCamera || puCanWeTuneMouse) {
                        NeedRefresh(!diff.emptyAND());
                        puLastNormalizedMousePos = puNormalizedMousePos;
                    }
                } else {
                    puMouseDrag = false;
                }
            }
        } else {
            puMouseDrag = false;
        }
    } else {
        puJustRecoveredFocus = false;
    }

    UpdateCamera();
}

void MainBackend::UpdateSound() {
    SoundSystem::Instance()->Update();
}

void MainBackend::UpdateMidi() {
    MidiSystem::Instance()->Update();
    NeedRefresh(MidiSystem::Instance()->updateMidiNeeded);
}

void MainBackend::Render() {
    glViewport(0, 0, puScreenSize.x, puScreenSize.y);
    clearBG();
    DoRendering();
    BlitFrameBuffer();
}

void MainBackend::RefreshOneFrame() {
    NeedRefresh(true);
    DoRendering();
}

void MainBackend::NeedRefresh(bool vFlag) {
    puNeedRefresh |= vFlag;
}

void MainBackend::DoRendering() {
    GuiBackend::MakeContextCurrent(MainBackend::sMainThread);

    TracyGpuZone("MainBackend::DoRendering");
    AIGPScoped("MainBackend", "%s", "Rendering");

    if (puPixelDebug.NeedRefresh()) {
        puPixelDebug.Capture(puDisplay_RenderPack, puDisplayQuality, CameraSystem::Instance());
    }

    auto displayShaderPtr = puDisplay_RenderPack.lock();
    if (displayShaderPtr) {
        NeedRefresh(TimeLineSystem::Instance()->DoAnimation_WithoutUiTriggering(displayShaderPtr->GetShaderKey()));
        NeedRefresh(ShaderKeyConfigSwitcherUnified::Instance()->IsRendering());
    }

    if (puCanWeRender || puNeedRefresh || CameraSystem::Instance()->m_NeedCamChange) {
        puNeedRefresh = false;  // reset

        GuiBackend::MakeContextCurrent(MainBackend::sMainThread);

        if (puUseFXAA) {
            glEnable(GL_MULTISAMPLE);
            LogGlError();
        }

        glClearDepth(1.0f);

        DoRenderingOfRenderPacks();

        if (puShow3DSpace || puUseFXAA || puShowMesh  //
            || GizmoSystem::Instance()->UseCulling()) {
            TracyGpuZone("MainBackend::DrawScene");

            // scene buffers
            /*for (auto &it : puMain_RenderPack->puSceneBuffers)
            {
                if (!it.second->puCanBeIntegratedInExternalPipeline)
                {
                    it.second->UpdateTimeWidgets();
                    it.second->UpdateUniforms(std::bind(&MainFrame::UpdateUniforms, this,
                        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5),
                        canUpdateMouse, puDisplayQuality, CameraSystem::Instance(), puScreenSize);
                    it.second->RenderNode();
                }
            }*/

            if (puMainPipeLine->bind()) {
                GuiBackend::MakeContextCurrent(MainBackend::sMainThread);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // glDisable(GL_CULL_FACE);
                // glFrontFace(GL_CCW);

                glEnable(GL_DEPTH_TEST);
                glDepthRange(0.0, 1.0);

                // glDepthFunc(puDepthFunc);
                // glEnable(GL_BLEND);
                // glBlendFunc(puBlendSFactor, puBlendDFactor);
                // glBlendEquation(puBlendEquation);

                if (displayShaderPtr) {
                    auto pipe = displayShaderPtr->GetPipe();
                    if (pipe) {
                        auto fbo = pipe->getBackBuffer();
                        if (fbo) {
                            const auto size = pipe->size.xy();
                            const ct::ivec2 p0 = 0;
                            const auto p1 = puMainPipeLine->size.xy();

                            GuiBackend::MakeContextCurrent(MainBackend::sMainThread);

                            // from
                            glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->getFboID());
                            LogGlError();

                            glReadBuffer(GL_COLOR_ATTACHMENT0 + puPreviewBufferId);
                            LogGlError();

                            // to
                            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, puMainPipeLine->getFrontFboID());
                            LogGlError();

                            glDrawBuffer(GL_COLOR_ATTACHMENT0);
                            LogGlError();

                            glBlitFramebuffer(0, 0, size.x, size.y, p0.x, p0.y, p1.x, p1.y, GL_COLOR_BUFFER_BIT,
                                              puMainPipeLine->puTexParams.magFilter);
                            LogGlError();

                            // seulement GL_NEAREST permet de merger le depth et les sdf
                            glBlitFramebuffer(0, 0, size.x, size.y, p0.x, p0.y, p1.x, p1.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
                            LogGlError();
                        }
                    }
                }

                if (puShow3DSpace) {
                    TracyGpuZone("MainBackend::Draw3DSpace");

                    if (pu3dAxis_RenderPack && puTriDAxisKey->puShaderGlobalSettings.showFlag) {
                        pu3dAxis_RenderPack->UpdateTimeWidgets(prDeltaTime);
                        pu3dAxis_RenderPack->UpdateUniforms(
                            std::bind(&MainBackend::UpdateUniforms, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                      std::placeholders::_4, std::placeholders::_5),
                            puDisplayQuality, &puMouseInterface, CameraSystem::Instance(), puScreenSize);
                        pu3dAxis_RenderPack->RenderNode(nullptr, puMainPipeLine);
                    }

                    if (pu3dGrid_RenderPack && puTriDGridKey->puShaderGlobalSettings.showFlag) {
                        pu3dGrid_RenderPack->UpdateTimeWidgets(prDeltaTime);
                        pu3dGrid_RenderPack->UpdateUniforms(
                            std::bind(&MainBackend::UpdateUniforms, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                      std::placeholders::_4, std::placeholders::_5),
                            puDisplayQuality, &puMouseInterface, CameraSystem::Instance(), puScreenSize);
                        pu3dGrid_RenderPack->RenderNode(nullptr, puMainPipeLine);
                    }
                }

                if (puShowMesh) {
                    TracyGpuZone("MainBackend::DrawMesh");

                    if (puMesh_RenderPack && puMeshKey->puShaderGlobalSettings.showFlag) {
                        puMesh_RenderPack->UpdateTimeWidgets(prDeltaTime);
                        puMesh_RenderPack->UpdateUniforms(std::bind(&MainBackend::UpdateUniforms, this, std::placeholders::_1, std::placeholders::_2,
                                                                    std::placeholders::_3, std::placeholders::_4, std::placeholders::_5),
                                                          puDisplayQuality, &puMouseInterface, CameraSystem::Instance(), puScreenSize);
                        puMesh_RenderPack->RenderNode(nullptr, puMainPipeLine);
                    }
                }

                if (GizmoSystem::Instance()->UseCulling()) {
                    TracyGpuZone("MainBackend::DrawCullingMesh");

                    auto rp = GizmoSystem::Instance()->GetRenderPack().lock();
                    if (rp && rp->GetShaderKey() && rp->GetShaderKey()->puShaderGlobalSettings.showFlag) {
                        rp->UpdateTimeWidgets(prDeltaTime);
                        rp->UpdateUniforms(std::bind(&MainBackend::UpdateUniforms, this, std::placeholders::_1, std::placeholders::_2,
                                                     std::placeholders::_3, std::placeholders::_4, std::placeholders::_5),
                                           puDisplayQuality, &puMouseInterface, CameraSystem::Instance(), puScreenSize);
                        rp->RenderNode(nullptr, puMainPipeLine);
                    }
                }

                {
                    // scene buffers
                    TracyGpuZone("MainBackend::DrawSceneShaders");

                    for (auto it : puMain_RenderPack->puSceneBuffers) {
                        auto itPtr = puDisplay_RenderPack.lock();
                        if (itPtr) {
                            itPtr->UpdateTimeWidgets(prDeltaTime);
                            itPtr->UpdateUniforms(std::bind(&MainBackend::UpdateUniforms, this, std::placeholders::_1, std::placeholders::_2,
                                                            std::placeholders::_3, std::placeholders::_4, std::placeholders::_5),
                                                  puDisplayQuality, &puMouseInterface, CameraSystem::Instance(), puScreenSize);
                            itPtr->RenderNode(nullptr, puMainPipeLine);
                        }
                    }
                }

                GuiBackend::MakeContextCurrent(MainBackend::sMainThread);

                glDisable(GL_BLEND);
                glDisable(GL_DEPTH_TEST);

                // glEnable(GL_CULL_FACE);
                // glCullFace(GL_BACK);

                puMainPipeLine->unbind();
                puMainPipeLine->switchBuffers();
            }

            ExportFrame(puMainPipeLine);
        } else {
            TracyGpuZone("MainBackend::DrawFullScreenShader");

            if (displayShaderPtr) {
                ExportFrame(displayShaderPtr->GetPipe());
            }

            auto renderPackToRender = displayShaderPtr;
            if (renderPackToRender->puMainRenderPack)
                renderPackToRender = renderPackToRender->puMainRenderPack;

            // scene buffers
            for (auto it : puMain_RenderPack->puSceneBuffers) {
                auto itPtr = it.lock();
                if (itPtr && itPtr != renderPackToRender) {
                    const auto s = itPtr->puMeshRect.Size();
                    itPtr->UpdateTimeWidgets(prDeltaTime);
                    itPtr->UpdateUniforms(std::bind(&MainBackend::UpdateUniforms, this, std::placeholders::_1, std::placeholders::_2,
                                                    std::placeholders::_3, std::placeholders::_4, std::placeholders::_5),
                                          puDisplayQuality, &puMouseInterface, CameraSystem::Instance(), ct::ivec2((int)s.x, (int)s.y));
                    itPtr->RenderNode();
                }
            }
        }

        if (puUseFXAA) {
            glDisable(GL_MULTISAMPLE);
            LogGlError();
        }
    }
}

void MainBackend::DoRenderingVR() {
#ifdef USE_VR
    GuiBackend::MakeContextCurrent(MainBackend::sMainThread);

    TracyGpuZone("MainBackend::DoRenderingVR");
    AIGPScoped("MainBackend", "VR Rendering");

    glClearDepth(1.0f);

    auto const vrbPtr = VRBackend::Instance();
    auto const camPtr = CameraSystem::Instance();
    auto const gizmoPtr = GizmoSystem::Instance();

    if (vrbPtr->BeginXRFrame()) {
        auto viewCount = vrbPtr->GetViewCount();
        for (uint32_t viewIdx = 0U; viewIdx < viewCount; ++viewIdx) {
            if (vrbPtr->StartXRFrameRendering(viewIdx)) {
                ct::ivec2 fboSize = vrbPtr->GetFBOSize();

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glEnable(GL_DEPTH_TEST);
                glDepthRange(0.0, 1.0);

                auto displayShaderPtr = puDisplay_RenderPack.lock();
                if (displayShaderPtr) {
                    displayShaderPtr->UpdateTimeWidgets(prDeltaTime);
                    displayShaderPtr->UpdateUniforms(std::bind(&MainBackend::UpdateUniforms, this, std::placeholders::_1, std::placeholders::_2,
                                                               std::placeholders::_3, std::placeholders::_4, std::placeholders::_5),
                                                     puDisplayQuality, &puMouseInterface, camPtr, fboSize);
                    displayShaderPtr->RenderNode(nullptr, nullptr, nullptr, nullptr, true);
                }

                if (puShow3DSpace) {
                    if (pu3dAxis_RenderPack && puTriDAxisKey->puShaderGlobalSettings.showFlag) {
                        pu3dAxis_RenderPack->UpdateTimeWidgets(prDeltaTime);
                        pu3dAxis_RenderPack->UpdateUniforms(
                            std::bind(&MainBackend::UpdateUniforms, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                      std::placeholders::_4, std::placeholders::_5),
                            puDisplayQuality, &puMouseInterface, camPtr, fboSize);
                        pu3dAxis_RenderPack->RenderShader(nullptr, nullptr);
                    }

                    if (pu3dGrid_RenderPack && puTriDGridKey->puShaderGlobalSettings.showFlag) {
                        pu3dGrid_RenderPack->UpdateTimeWidgets(prDeltaTime);
                        pu3dGrid_RenderPack->UpdateUniforms(
                            std::bind(&MainBackend::UpdateUniforms, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                      std::placeholders::_4, std::placeholders::_5),
                            puDisplayQuality, &puMouseInterface, camPtr, fboSize);
                        pu3dGrid_RenderPack->RenderShader(nullptr, nullptr);
                    }
                }

                if (gizmoPtr->UseCulling()) {
                    auto rp = gizmoPtr->GetRenderPack().lock();
                    if (rp && rp->GetShaderKey() && rp->GetShaderKey()->puShaderGlobalSettings.showFlag) {
                        rp->UpdateTimeWidgets(prDeltaTime);
                        rp->UpdateUniforms(std::bind(&MainBackend::UpdateUniforms, this, std::placeholders::_1, std::placeholders::_2,
                                                     std::placeholders::_3, std::placeholders::_4, std::placeholders::_5),
                                           puDisplayQuality, &puMouseInterface, CameraSystem::Instance(), fboSize);
                        rp->RenderShader(nullptr, nullptr);
                    }
                }

                glDisable(GL_DEPTH_TEST);

                vrbPtr->EndXRFrameRendering(viewIdx);
            }
        }

        vrbPtr->EndXRFrame();
    }
#endif  // USE_VR
}

void MainBackend::DoRenderingOfSceneRenderPacks(bool vOnlyForSpace3DPipeLine) {
    UNUSED(vOnlyForSpace3DPipeLine);

    TracyGpuZone("MainBackend::DoRenderingOfSceneRenderPacks");
    if (!puMain_RenderPack->puSceneBuffers.empty()) {
        AIGPScoped("MainBackend", "Scene");

        // scene buffers
        for (auto it : puMain_RenderPack->puSceneBuffers) {
            auto itPtr = it.lock();
            if (itPtr) {
                if (itPtr->puCanBeIntegratedInExternalPipeline) {
                    itPtr->UpdateTimeWidgets(prDeltaTime);
                    itPtr->UpdateUniforms(std::bind(&MainBackend::UpdateUniforms, this, std::placeholders::_1, std::placeholders::_2,
                                                    std::placeholders::_3, std::placeholders::_4, std::placeholders::_5),
                                          puDisplayQuality, &puMouseInterface, CameraSystem::Instance(), puScreenSize);
                    itPtr->RenderNode(nullptr, nullptr);
                }
            }
        }
    }
}

void MainBackend::DoRenderingOfRenderPacks() {
    if (!puDisplay_RenderPack.expired()) {
        TracyGpuZone("MainBackend::DoRenderingOfRenderPacks");
        // AIGPScoped("MainBackend", "DoRenderingOfRenderPacks");

        // puMain_RenderPack->UpdatePreDefinedUniforms(puDefaultUniforms);

        // histo texture
        SoundSystem::Instance()->Render(prDeltaTime);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///// ici on utilise pas le display_renderpack //////////////////////////////////////////////////////////////
        ///// car la maj d'un des buffers de main renderpack doit dependre du main Renderpack ///////////////////////
        ///// pour avoir la bonne maj des sous buffers il faut tout rendre et donc passer par le main renderpack ////
        ///// MAJ 31/05/2020 ////////////////////////////////////////////////////////////////////////////////////////
        ///// on part sur maj le display renderpack /////////////////////////////////////////////////////////////////
        ///// mais s'il y a un parent, ca veut dire que c'est un child //////////////////////////////////////////////
        ///// et dans ce cas on selectionne le parent a rendre //////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // selection du main renderpack pour demarrer le rendu etagé
        auto renderPackToRender = puDisplay_RenderPack.lock();
        if (renderPackToRender->puMainRenderPack)
            renderPackToRender = renderPackToRender->puMainRenderPack;

        prPipeSize = puScreenSize;
        if ((puShow3DSpace || puUseFXAA || puShowMesh  //
             || GizmoSystem::Instance()->UseCulling()  //
             ) &&
            renderPackToRender->puCanBeIntegratedInExternalPipeline) {
            prPipeSize = puMainPipeLine->size.xy();
        } else {
            const auto& s = renderPackToRender->puMeshRect.Size();
            prPipeSize = ct::ivec2((int)s.x, (int)s.y);
        }

        renderPackToRender->UpdateTimeWidgets(prDeltaTime);
        renderPackToRender->UpdateUniforms(std::bind(&MainBackend::UpdateUniforms, this, std::placeholders::_1, std::placeholders::_2,
                                                     std::placeholders::_3, std::placeholders::_4, std::placeholders::_5),
                                           puDisplayQuality, &puMouseInterface, CameraSystem::Instance(), prPipeSize);
        renderPackToRender->RenderNode();
    } else {
        prPipeSize = puScreenSize;
    }
}

void MainBackend::BlitFrameBufferFromTo(FrameBufferPtr vFrom, int vFromAttachmentId, FrameBufferPtr vTo, int vToAttachmentId) {
    TracyGpuZone("MainBackend::BlitFrameBufferFromTo");

    if (vFrom && vFromAttachmentId > -1) {
        GuiBackend::MakeContextCurrent(MainBackend::sMainThread);

        const auto fromSize = vFrom->getSize().xy();
        ct::ivec2 p0, p1;

        glBindFramebuffer(GL_READ_FRAMEBUFFER, vFrom->getFboID());
        LogGlError();

        glReadBuffer(GL_COLOR_ATTACHMENT0 + vFromAttachmentId);
        LogGlError();

        if (vTo && vToAttachmentId > -1) {
            p1 = ct::convert(ct::convert(vTo->getSize().xy()) * puDisplayQuality);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, vTo->getFboID());
            LogGlError();

            glDrawBuffer(GL_COLOR_ATTACHMENT0 + vToAttachmentId);
            LogGlError();
        } else {
            p1 = ct::convert(ct::convert(fromSize) * puDisplayQuality);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            LogGlError();

            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            LogGlError();
        }

        glBlitFramebuffer(0, 0, fromSize.x, fromSize.y, p0.x, p0.y, p1.x, p1.y, GL_COLOR_BUFFER_BIT, vFrom->puTexParams.magFilter);
        LogGlError();
    }
}

void MainBackend::BlitFrameBuffer() {
    TracyGpuZone("MainBackend::BlitFrameBuffer");
    AIGPScoped("MainBackend", "BlitFrameBuffer");

    if (puShow3DSpace || puUseFXAA || puShowMesh  //
        || GizmoSystem::Instance()->UseCulling()  //
        || GizmoSystem::Instance()->UseCulling()  //
    ) {
        BlitFrameBuffer(puMainPipeLine);
    } else {
        BlitFrameBuffer(puDisplay_RenderPack);
    }
}

void MainBackend::BlitFrameBuffer(FrameBuffersPipeLinePtr vPipe) {
    TracyGpuZone("MainBackend::BlitFrameBuffer");

    if (vPipe) {
        auto fbo = vPipe->getBackBuffer();
        if (fbo) {
            GuiBackend::MakeContextCurrent(MainBackend::sMainThread);

            const auto size = fbo->getSize().xy();
            const ct::ivec2 p0 = 0;
            const auto p1 = ct::convert(ct::convert(size) * puDisplayQuality);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            LogGlError();

            glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->getFboID());
            LogGlError();

            glReadBuffer(GL_COLOR_ATTACHMENT0);
            LogGlError();

            glDrawBuffer(GL_BACK);
            LogGlError();

            glBlitFramebuffer(0, 0, size.x, size.y, p0.x, p0.y, p1.x, p1.y, GL_COLOR_BUFFER_BIT, fbo->puTexParams.magFilter);
            LogGlError();
        }
    }
}

void MainBackend::BlitFrameBuffer(RenderPackWeak vRenderPack) {
    TracyGpuZone("MainBackend::BlitFrameBuffer");

    auto rpPtr = vRenderPack.lock();
    if (rpPtr) {
        if (rpPtr->GetPipe()) {
            auto fbo = rpPtr->GetPipe()->getBackBuffer();
            if (fbo) {
                GuiBackend::MakeContextCurrent(MainBackend::sMainThread);

                const auto size = fbo->getSize().xy();
                const auto p0 = ct::convert(rpPtr->puMeshRect.leftBottom() * puDisplayQuality);
                const auto p1 = ct::convert(rpPtr->puMeshRect.rightTop() * puDisplayQuality);

                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                LogGlError();

                glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->getFboID());
                LogGlError();

                glReadBuffer(GL_COLOR_ATTACHMENT0 + puPreviewBufferId);
                LogGlError();

                glDrawBuffer(GL_BACK);
                LogGlError();

                glBlitFramebuffer(0, 0, size.x, size.y, p0.x, p0.y, p1.x, p1.y, GL_COLOR_BUFFER_BIT, fbo->puTexParams.magFilter);
                LogGlError();
            }
        }
    }
}

///////////////////////////////////////////////////////
//// DEFAULT UNIFORMS /////////////////////////////////
///////////////////////////////////////////////////////

void MainBackend::UpdateUniforms(RenderPackWeak vRenderPack, UniformVariantPtr vUniPtr, DisplayQualityType vDisplayQuality, MouseInterface* vMouse,
                                 CameraInterface* vCamera) {
    UNUSED(vRenderPack);
    UNUSED(vDisplayQuality);
    UNUSED(vMouse);
    UNUSED(vCamera);

    if (vUniPtr->widget == "show3dSpace" && vUniPtr->glslType == uType::uTypeEnum::U_FLOAT) {
        vUniPtr->x = (puShow3DSpace ? 1.0f : 0.0f);
    }
    if (vUniPtr->widget == "currentattachment" && vUniPtr->glslType == uType::uTypeEnum::U_INT) {
        vUniPtr->ix = puPreviewBufferId;
    }
    if (vUniPtr->widget == "usemsaa" && vUniPtr->glslType == uType::uTypeEnum::U_FLOAT) {
        vUniPtr->x = (puUseFXAA ? 1.0f : 0.0f);
    }
    if (vUniPtr->widget == "bgcolor" && vUniPtr->glslType == uType::uTypeEnum::U_VEC4) {
        vUniPtr->x = puBackgroundColor.x;
        vUniPtr->y = puBackgroundColor.y;
        vUniPtr->z = puBackgroundColor.z;
        vUniPtr->w = puBackgroundColor.w;
    }
#ifdef USE_DESKTOP_TEXTURE
    if (vUniPtr->widget == "desktop" && vUniPtr->glslType == uType::uTypeEnum::U_SAMPLER2D) {
        if (vUniPtr->loc > -1) {
            if (DesktopTexture::Instance()->GetPipe()) {
                vUniPtr->uSampler2D = DesktopTexture::Instance()->GetTexId();
            }
        }
    }
#endif
}
//////////////// IMGUI ///////////////////////////////////////////////////

void MainBackend::PreCalc_Menu_DisplayQuality() {
    m_QualityDisplayMenu.clear();
    auto dPtr = puDisplay_RenderPack.lock();
    if (dPtr != nullptr && dPtr->GetPipe()!=nullptr) {
        const auto& dpSize = puScreenSize;
        for (const auto q : m_DisplayQualities) {
            const auto newSize = ct::fvec2(dpSize) / q;
            const auto s = ct::toStr("%f (%i x %i)", q, (int32_t)newSize.x, (int32_t)newSize.y);
            m_QualityDisplayMenu[s] = q;
        }
    } else {
        for (const auto q : m_DisplayQualities) {
            const auto s = ct::toStr("%f", q);
            m_QualityDisplayMenu[s] = q;
        }    
    }
}

void MainBackend::DrawMenu_DisplayQuality() {
    if (ImGui::BeginMenu("Quality")) {
        for (const auto& p : m_QualityDisplayMenu) {
            if (ImGui::MenuItem(p.first.c_str(), "", IS_FLOAT_EQUAL(puDisplayQuality, p.second))) {
                SetRenderQuality(p.second);
            }
        }

        ImGui::EndMenu();
    }
}

void MainBackend::DrawMenu_FXAA() {
    if (ImGui::MenuItem("Use MSAA 2X for Mesh's", "", &puUseFXAA)) {
        puCountFXAASamples = 2;
        UpdateFXAA();
    }
}

void MainBackend::DrawMenu_Edit() {
    if (ImGui::BeginMenu(ICON_NDP2_PENCIL " Edit Shaders")) {
        auto& isp = ImGui::GetStyle().ItemSpacing;
        const auto spacing_L = (float)(int)(isp.x * 0.5f);
        const auto spacing_U = (float)(int)(isp.y * 0.5f);
        const auto spacing_R = isp.x - spacing_L;
        const auto spacing_D = isp.y - spacing_U;

        const auto spacing_LURD = ct::fvec4(spacing_L, spacing_U, spacing_R, spacing_D);

        for (auto it = puCodeTree->puFilesUsedFromLastShadersConstruction.begin(); it != puCodeTree->puFilesUsedFromLastShadersConstruction.end();
             ++it) {
            for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
                const auto key = it2->first;

                if (puFileListingExceptions.find(key->puKey) == puFileListingExceptions.end())  // not found
                {
                    if (it != puCodeTree->puFilesUsedFromLastShadersConstruction.begin())
                        ImGui::Separator();

                    ShowEditFileMenuKey(key, spacing_LURD);
                }
            }
        }

        // si je veut les editer je peux faire dans le pane uniforms

        if (puShow3DSpace) {
            ImGui::Separator();

            ShowEditFileMenuKey(puTriDAxisKey, spacing_LURD);
            ShowEditFileMenuKey(puTriDGridKey, spacing_LURD);
        }

        if (puShowMesh) {
            ImGui::Separator();

            ShowEditFileMenuKey(puMeshKey, spacing_LURD);
        }

        /*if (puShowBPPlanes)
        {
            ImGui::Separator();

            ShowEditFileMenuKey(puBPPlanesKey, spacing_LURD);
        }*/

        /*if (GizmoSystem::Instance()->UseCulling())
        {
            ImGui::Separator();

            ShowEditFileMenuKey(GizmoSystem::Instance()->GetShaderKey(), spacing_LURD);
        }*/

        ImGui::EndMenu();
    }
}

void MainBackend::ShowEditFileMenuKey(ShaderKeyPtr vKey, ct::fvec4 vLURD) {
    if (vKey) {
        auto _colorPushed = false;

        if (vKey->puCompilationSuccess == ShaderMsg::SHADER_MSG_ERROR) {
            const auto p_min = ImGui::GetCursorScreenPos() - ImVec2(vLURD.x, vLURD.y);
            const auto p_max = ImVec2(p_min.x + ImGui::GetContentRegionAvail().x, p_min.y + ImGui::GetFrameHeight()) - ImVec2(vLURD.z, vLURD.w);
            ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, IM_COL32(255, 50, 50, 255));
        } else if (vKey->puCompilationSuccess == ShaderMsg::SHADER_MSG_WARNING) {
            const auto p_min = ImGui::GetCursorScreenPos() - ImVec2(vLURD.x, vLURD.y);
            const auto p_max = ImVec2(p_min.x + ImGui::GetContentRegionAvail().x, p_min.y + ImGui::GetFrameHeight()) - ImVec2(vLURD.z, vLURD.w);
            ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, IM_COL32(255, 127, 50, 255));

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

            _colorPushed = true;
        }

        if (ImGui::MenuItem(vKey->puMainSection->relativeFile.c_str())) {
            vKey->OpenFileKey();
        }

        if (_colorPushed) {
            ImGui::PopStyleColor();
        }
    }
}

//////////////// CONFIGURATION XML //////////////////////////////////

std::string MainBackend::getXml(const std::string& vOffset, const std::string& vUserDatas) {
    UNUSED(vUserDatas);

    std::string str;

    str += vOffset + "<MainBackend>\n";
    str += vOffset + "\t<useMSAA2X>" + ct::toStr(puUseFXAA ? "true" : "false") + "</useMSAA2X>\n";
    str += vOffset + "\t<quality>" + ct::toStr(puDisplayQuality) + "</quality>\n";
    str += vOffset + "\t<appMaximized>" + ct::toStr(((bool)GuiBackend::Instance()->IsMaximized(MainBackend::sMainThread)) ? "true" : "false") +
        "</appMaximized>\n";
    str += vOffset + "\t<appRect>" + puAppRectIfNotmaximized.string() + "</appRect>\n";
    str += vOffset + "\t<consolevisibility>" + ct::toStr(puConsoleVisiblity ? "true" : "false") + "</consolevisibility>\n";
    str += vOffset + "\t<colorbg value=\"" + ct::fvec4(puBackgroundColor.x, puBackgroundColor.y, puBackgroundColor.z, puBackgroundColor.w).string() + "\"/>\n";
    str += vOffset + "\t<autoplay>" + (puPlayCounterTimers ? "true" : "false") + "</autoplay>\n";
    str += vOffset + "\t<canWeRender>" + (puCanWeRender ? "true" : "false") + "</canWeRender>\n";
    str += vOffset + "\t<worldview>" + (puShow3DSpace ? "true" : "false") + "</worldview>\n";
    str += vOffset + "\t<showmesh>" + (puShowMesh ? "true" : "false") + "</showmesh>\n";
    // str += vOffset + "\t<showbpplanes>" + (puShowBPPlanes ? "true" : "false") + "</showbpplanes>\n";
    str += vOffset + "\t<preview_buffer>" + ct::toStr(puPreviewBufferId) + "</preview_buffer>\n";
    str += vOffset + "\t<canWeTuneCamera>" + (puCanWeTuneCamera ? "true" : "false") + "</canWeTuneCamera>\n";
    str += vOffset + "\t<canWeTuneMouse>" + (puCanWeTuneMouse ? "true" : "false") + "</canWeTuneMouse>\n";
    str += puPixelDebug.getXml(vOffset + "\t", vUserDatas);
    str += GizmoSystem::Instance()->getXml(vOffset + "\t", vUserDatas);
    str += SoundSystem::Instance()->getXml(vOffset + "\t", vUserDatas);
    str += ShaderKeyConfigSwitcherUnified::Instance()->getXml(vOffset + "\t", vUserDatas);
    str += vOffset + "</MainBackend>\n";

    return str;
}

bool MainBackend::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) {
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

    if (strParentName == "MainBackend") {
        if (strName == "appRect") {
            const auto isMaximized = (bool)GuiBackend::Instance()->IsMaximized(MainBackend::sMainThread);
            if (isMaximized)
                GuiBackend::Instance()->RestoreWindow(MainBackend::sMainThread);
            puAppRectIfNotmaximized = ct::ivariant(strValue).GetV4();
            GuiBackend::Instance()->SetWindowPos(MainBackend::sMainThread, puAppRectIfNotmaximized.x, puAppRectIfNotmaximized.y);
            GuiBackend::Instance()->SetWindowSize(MainBackend::sMainThread, puAppRectIfNotmaximized.z, puAppRectIfNotmaximized.w);
            if (isMaximized) {
                GuiBackend::Instance()->MaximizeWindow(MainBackend::sMainThread);
            }
        }
        if (strName == "appMaximized") {
            const auto isMaximized = ct::ivariant(strValue).GetB();
            if (isMaximized) {
                GuiBackend::Instance()->MaximizeWindow(MainBackend::sMainThread);
            }
        }

        if (strName == "consolevisibility")
            SetConsoleVisibility(ct::ivariant(strValue).GetB());

        if (strName == "colorbg") {
            const auto att = vElem->FirstAttribute();
            if (att) {
                strName = att->Name();
                if (strName == "value") {
                    strValue = att->Value();
                    auto v4 = ct::fvariant(strValue).GetV4();
                    puBackgroundColor = ImVec4(v4.x, v4.y, v4.z, v4.w);
                }
            }
        }
        if (strName == "autoplay")
            puPlayCounterTimers = ct::ivariant(strValue).GetB();
        if (strName == "canWeRender")
            puCanWeRender = ct::ivariant(strValue).GetB();
        if (strName == "canWeTuneCamera")
            puCanWeTuneCamera = ct::ivariant(strValue).GetB();
        if (strName == "canWeTuneMouse")
            puCanWeTuneMouse = ct::ivariant(strValue).GetB();
        if (strName == "useMSAA2X")
            puUseFXAA = ct::ivariant(strValue).GetB();
        if (strName == "quality")
            puDisplayQuality = ct::fvariant(strValue).GetF();
        if (strName == "useMSAA2X")
            puUseFXAA = ct::ivariant(strValue).GetB();
        if (strName == "worldview")
            puShow3DSpace = ct::ivariant(strValue).GetB();
        if (strName == "showmesh")
            puShowMesh = ct::ivariant(strValue).GetB();
        // if (strName == "showbpplanes")
        //	puShowBPPlanes = ct::ivariant(strValue).GetB();

        puPixelDebug.setFromXml(vElem, vParent, vUserDatas);
    }

    GizmoSystem::Instance()->setFromXml(vElem, vParent, vUserDatas);
    SoundSystem::Instance()->setFromXml(vElem, vParent, vUserDatas);
    ShaderKeyConfigSwitcherUnified::Instance()->setFromXml(vElem, vParent, vUserDatas);

    return true;
}

void MainBackend::ExportFrame(FrameBuffersPipeLinePtr vPipe) {
    if (vPipe && TimeLineSystem::Instance()->IsRendering()) {
        if (TimeLineSystem::Instance()->GetRenderingMode() == RenderingModeEnum::RENDERING_MODE_PICTURES) {
            PictureExportSystem::Instance()->ExportFBOToPictureFile(vPipe->getBackBuffer(),  //
                                                                    TimeLineSystem::Instance()->GetRenderingFilePathNameForCurrentFrame(),
                                                                    puPreviewBufferId);  //
        }
    }

    if (vPipe && ShaderKeyConfigSwitcherUnified::Instance()->IsRendering()) {
        if (ShaderKeyConfigSwitcherUnified::Instance()->GetRenderingMode() == RenderingModeEnum::RENDERING_MODE_PICTURES) {
            PictureExportSystem::Instance()->ExportFBOToPictureFile(
                vPipe->getBackBuffer(),                                                                                     //
                ShaderKeyConfigSwitcherUnified::Instance()->GetRenderingFilePathNameForCurrentFrame(), puPreviewBufferId);  //
        }
    }
}

void MainBackend::SaveAsTemplate(const std::string& vFileName, const std::string& vFilePath) {
    TemplateSystem::Instance()->CreateTemplateFromRenderPack(vFileName, vFilePath, puMain_RenderPack);
}

std::string MainBackend::ForkToFile(const std::string& vFilePathName) {
    std::string res;

    // il va falloir copier le code dnas des nouveaus fichier
    // et remplacer les noms des fichier includes, buffer et scene buffers au prealable.
    // en evitant de remplacer les include predefined

    CTOOL_DEBUG_BREAK;

    if (IsLoaded() && !vFilePathName.empty()) {
        std::string main_buffer_fpn = FileHelper::Instance()->GetPathRelativeToApp(puMain_RenderPack->GetShaderKey()->puMainSection->absoluteFile);
        std::string main_code = puMain_RenderPack->GetShaderKey()->puMainSection->code;

        auto src_ps = FileHelper::Instance()->ParsePathFileName(main_buffer_fpn);
        auto tgt_ps = FileHelper::Instance()->ParsePathFileName(vFilePathName);
        if (src_ps.isOk && tgt_ps.isOk) {
            using spss_t = std::pair<std::string, std::string>;

            spss_t _main_buffer = spss_t(FileHelper::Instance()->GetPathRelativeToApp(src_ps.GetFPNE()),
                                         FileHelper::Instance()->GetPathRelativeToApp(tgt_ps.GetFPNE()));
            std::string _main_code = puMain_RenderPack->GetShaderKey()->puMainSection->code;

            std::map<spss_t, std::string> child_buffers_code;
            for (auto child_buffer : puMain_RenderPack->puBuffers) {
                auto rpPtr = child_buffer.lock();
                if (rpPtr) {
                    if (rpPtr && rpPtr->GetShaderKey()) {
                        child_buffers_code[spss_t(FileHelper::Instance()->GetPathRelativeToApp(rpPtr->GetShaderKey()->puMainSection->absoluteFile),
                                                  "")] = rpPtr->GetShaderKey()->puMainSection->code;
                    }
                }
            }

            std::map<spss_t, std::string> scene_buffers_code;
            for (auto scene_buffer : puMain_RenderPack->puSceneBuffers) {
                auto rpPtr = scene_buffer.lock();
                if (rpPtr) {
                    if (rpPtr && rpPtr->GetShaderKey()) {
                        scene_buffers_code[spss_t(FileHelper::Instance()->GetPathRelativeToApp(rpPtr->GetShaderKey()->puMainSection->absoluteFile),
                                                  "")] = rpPtr->GetShaderKey()->puMainSection->code;
                    }
                }
            }

            std::map<spss_t, std::string> includes_code;
            for (const auto& include : puMain_RenderPack->GetShaderKey()->puIncludeFileNames) {
                if (!include.second.empty()) {
                    auto incKey = puCodeTree->GetKey(include.second);
                    if (incKey) {
                        scene_buffers_code[spss_t(FileHelper::Instance()->GetPathRelativeToApp(incKey->puMainSection->absoluteFile), "")] =
                            incKey->puMainSection->code;
                    }
                }
            }
        }
    }

    return res;
}

void MainBackend::SetPlayPause(bool vPlay, ShaderKeyPtr vKey) {
    if (vKey) {
        auto lst = vKey->GetUniformsByWidget("time");
        if (lst) {
            for (auto itLst = lst->begin(); itLst != lst->end(); ++itLst) {
                const auto v = *itLst;
                if (v) {
                    v->bx = vPlay;
                }
            }
        }

        /*lst = vKey->GetUniformsByWidget("sound");
        if (lst)
        {
            for (auto itLst = lst->begin(); itLst != lst->end(); ++itLst)
            {
                auto v = *itLst;
                if (v && v->loc > -1)
                {
                    if (v->sound_histo_ptr)
                    {
                        if (vPlay && v->bx)
                        {
                            v->sound->Play();
                        }
                        else
                        {
                            v->sound->Pause();
                        }
                    }
                }
            }
        }*/
    }
}

void MainBackend::SetPlayPause(bool vPlay) {
    const auto key = puMain_RenderPack->GetShaderKey();
    if (key) {
        SetPlayPause(vPlay, key);

        for (auto it : puMain_RenderPack->puBuffers) {
            auto itPtr = it.lock();
            if (itPtr) {
                const auto subkey = itPtr->GetShaderKey();
                if (subkey) {
                    SetPlayPause(vPlay, subkey);
                }
            }
        }

        for (auto it : puMain_RenderPack->puSceneBuffers) {
            auto itPtr = it.lock();
            if (itPtr) {
                SetPlayPause(vPlay, itPtr->GetShaderKey());

                for (auto sit : itPtr->puBuffers) {
                    auto sitPtr = sit.lock();
                    if (sitPtr) {
                        SetPlayPause(vPlay, sitPtr->GetShaderKey());
                    }
                }
            }
        }
    }
}

void MainBackend::ReSetPlayPause(ShaderKeyPtr vKey) {
    if (vKey) {
        auto lst = vKey->GetUniformsByWidget("time");
        if (lst) {
            for (auto itLst = lst->begin(); itLst != lst->end(); ++itLst) {
                const auto v = *itLst;
                if (v) {
                    v->x = 0.0f;
                }
            }
        }

        /*lst = vKey->GetUniformsByWidget("sound");
        if (lst)
        {
            for (auto itLst = lst->begin(); itLst != lst->end(); ++itLst)
            {
                auto v = *itLst;
                if (v)
                {
                    if (v->sound)
                    {
                        v->sound->Reset();
                    }
                    v->x = 0.0f;
                }
            }
        }*/
    }
}

void MainBackend::ReSetPlayPause() {
    const auto key = puMain_RenderPack->GetShaderKey();
    if (key) {
        ReSetPlayPause(key);

        for (auto it : puMain_RenderPack->puBuffers) {
            auto itPtr = it.lock();
            if (itPtr) {
                ReSetPlayPause(itPtr->GetShaderKey());
            }
        }

        for (auto it : puMain_RenderPack->puSceneBuffers) {
            auto itPtr = it.lock();
            if (itPtr) {
                ReSetPlayPause(itPtr->GetShaderKey());
            }

            for (auto sit : puMain_RenderPack->puBuffers) {
                auto sitPtr = sit.lock();
                if (sitPtr) {
                    ReSetPlayPause(sitPtr->GetShaderKey());
                }
            }
        }
    }
}

void MainBackend::ResetFrame() {
    puMain_RenderPack->ResetFrame();

    SoundSystem::Instance()->ResetFrame();
}

void MainBackend::ResetTime() {
    puMain_RenderPack->ResetTime();

    SoundSystem::Instance()->ResetTime();
}

URLLoadingErrorMessages MainBackend::LoadUrl() {
    URLLoadingErrorMessages res;
#ifdef USE_NETWORK
    NetCodeRetriever::Instance()->RetrieveWithoutThread(std::bind(&MainBackend::FinalizeUrlLoading, MainBackend::Instance()));
    // NetCodeRetriever::Instance()->CreateThread(std::bind(&GlobalSystem::LoadUrl, this));

    if (  // NetCodeRetriever::sUrlLoadingStatus != UrlLoadingStatus::URL_LOADING_STATUE_OK ||
        !prUrlErrorMessages.empty()) {
        res = prUrlErrorMessages;
    }
#endif  // #ifdef USE_NETWORK
    return res;
}

void MainBackend::FinalizeUrlLoading_By_CreateFiles() {
#ifdef USE_NETWORK
    // c'est ca qu'il faut multithreader
    // on charge l'url
    auto file_name = FinalizeShaderImport_By_CreateFiles("", NetCodeRetriever::sShaders);

    if (!file_name.empty()) {
        LoadFilePathName(FileHelper::Instance()->GetAbsolutePathForFileLocation(file_name, (int)FILE_LOCATION_Enum::FILE_LOCATION_SCRIPT), ".glsl");
        ReSetPlayPause();
        SetPlayPause(true);
    }
#endif
}

void MainBackend::FinalizeUrlLoading_By_CreateOneFile() {
#ifdef USE_NETWORK
    // ici on est certain d'etre sur une plateform ShaderToy
    //
    // c'est ca qu'il faut multithreader
    // on charge l'url
    auto file_name = FinalizeShaderImport_By_CreateOneFile("", NetCodeRetriever::sShaders);

    if (!file_name.empty()) {
        LoadFilePathName(FileHelper::Instance()->GetAbsolutePathForFileLocation(file_name, (int)FILE_LOCATION_Enum::FILE_LOCATION_SCRIPT), ".glsl");
        ReSetPlayPause();
        SetPlayPause(true);
    }
#endif
}

std::string MainBackend::FinalizeShaderImport_By_CreateFiles(std::string vPath, std::list<ShaderInfos> vShaderInfos) {
    if (!vShaderInfos.empty()) {
        std::string mainBufferName;
        //auto mainPlatform = ShaderPlaform::SPF_UNKNOW;  // on cree les fichier
        std::string file_string;
        for (auto& infos : vShaderInfos) {
            file_string.clear();
            if (!infos.error.empty()) {
                prUrlErrorMessages.push_back(infos.error);
            } else {
                file_string += infos.framebuffer + "\n";
                file_string += infos.common_uniforms;
                file_string += infos.specific_uniforms;
                file_string += infos.note;
                file_string += infos.common;
                file_string += infos.vertex;
                file_string += infos.fragment;

                auto name = infos.name;

                if (infos.platform == ShaderPlaform::SPF_VERTEXSHADERART) {
                    puBackgroundColor.x = infos.color.r;
                    puBackgroundColor.y = infos.color.g;
                    puBackgroundColor.z = infos.color.b;

                    name = infos.name;

                    UrlLibrarySystem::Instance()->AddUrl("VertexShaderArt", infos.user, infos.name, infos.url);
                } else if (infos.platform == ShaderPlaform::SPf_SHADERTOY) {
                    if (infos.IsMain) {
                        UrlLibrarySystem::Instance()->AddUrl("ShaderToy", infos.user, infos.name, infos.url);
                    }
                } else if (infos.platform == ShaderPlaform::SPF_GLSLSANDBOX) {
                    UrlLibrarySystem::Instance()->AddUrl("GlslSandBox", infos.user, infos.name, infos.url);
                }

                ct::replaceString(name, ".", "_");
                ct::replaceString(name, " ", "_");
                ct::replaceString(name, "?", "");
                ct::replaceString(name, "<", "");
                ct::replaceString(name, ">", "");
                ct::replaceString(name, ":", "");
                ct::replaceString(name, "|", "");
                ct::replaceString(name, "*", "");
                ct::replaceString(name, ":", "");

                if (!infos.shader_id.empty())
                    name += "_" + infos.shader_id;

                auto filename = name + ".glsl";

                mainBufferName = filename;
                //mainPlatform = infos.platform;

                if (vPath.empty()) {
                    FileHelper::Instance()->SaveToFile(file_string, filename, (int)FILE_LOCATION_Enum::FILE_LOCATION_SCRIPT);
                } else {
                    PathStruct ps;
                    FileHelper::Instance()->SaveStringToFile(file_string, ps.GetFPNE_WithPathName(vPath, filename));
                }
            }
        }

        return mainBufferName;
    }

    return "";
}

std::string MainBackend::FinalizeShaderImport_By_CreateOneFile(std::string vPath, std::list<ShaderInfos> vShaderInfos) {
    if (!vShaderInfos.empty()) {
        std::string mainBufferName;
        std::string filename;

        std::string framebuffer_string;
        std::string uniforms_string;
        std::string note_string;
        std::string vertex_string;
        std::string common_string;
        std::string fragment_string;

        bool common_uniforms_already_set = false;
        for (const auto& infos : vShaderInfos) {
            if (!infos.error.empty()) {
                prUrlErrorMessages.push_back(infos.error);
            } else {
                framebuffer_string += infos.framebuffer;

                if (!common_uniforms_already_set && !infos.common_uniforms.empty()) {
                    common_uniforms_already_set = true;
                    uniforms_string += infos.common_uniforms;
                }

                uniforms_string += infos.specific_uniforms;
                fragment_string += infos.fragment;

                if (note_string.empty())
                    note_string += infos.note;
                if (vertex_string.empty())
                    vertex_string += infos.vertex;
                if (common_string.empty())
                    common_string += infos.common;

                if (infos.IsMain) {
                    UrlLibrarySystem::Instance()->AddUrl("ShaderToy", infos.user, infos.name, infos.url);

                    auto name = infos.name;

                    ct::replaceString(name, ".", "_");
                    ct::replaceString(name, " ", "_");

                    if (!infos.shader_id.empty())
                        name += "_" + infos.shader_id;

                    filename = name + ".glsl";
                }
            }
        }

        std::string file_string = note_string + framebuffer_string + "\n" + common_string + uniforms_string + vertex_string + fragment_string;

        if (vPath.empty()) {
            FileHelper::Instance()->SaveToFile(file_string, filename, (int)FILE_LOCATION_Enum::FILE_LOCATION_SCRIPT);
        } else {
            PathStruct ps;
            FileHelper::Instance()->SaveStringToFile(file_string, ps.GetFPNE_WithPathName(vPath, filename));
        }

        return filename;
    }

    return "";
}

void MainBackend::FinalizeUrlLoading() {
#ifdef USE_NETWORK
    prUrlErrorMessages.clear();

    // l'improt en 1 fichier est seulement pour shadertoy
    ShaderPlaform platform = ShaderPlaform::SPF_UNKNOW;
    if (!NetCodeRetriever::sShaders.empty()) {
        platform = NetCodeRetriever::sShaders.begin()->platform;
    }

    if (NetCodeRetriever::sInportInOneFile && platform == ShaderPlaform::SPf_SHADERTOY) {
        FinalizeUrlLoading_By_CreateOneFile();
    } else {
        FinalizeUrlLoading_By_CreateFiles();
    }
#endif  // #ifdef USE_NETWORK
}

void MainBackend::LoadToClipBoard(std::string vString) {
    if (!vString.empty()) {
        GuiBackend::Instance()->SetClipboardString(sMainThread, vString.c_str());
    }
}

void MainBackend::SetConsoleVisibility(bool vShow) {
    puConsoleVisiblity = vShow;

    if (puConsoleVisiblity) {
        // on cache la console
        // on l'affichera au besoin comme blender fait
#ifdef WIN32
        ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif
    } else {
        // on cache la console
        // on l'affichera au besoin comme blender fait
#ifdef WIN32
        ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    }
}

bool MainBackend::LoadFilePathName(const std::string& vFilePathName, const std::string& vType, bool vDontLoadRenderPack) {
    UNUSED(vType);

    auto res = false;

    if (puMain_RenderPack) {
        const auto filePathName = FileHelper::Instance()->CorrectSlashTypeForFilePathName(vFilePathName);
        auto relativeFilePathName = FileHelper::Instance()->GetPathRelativeToApp(filePathName);
        if (FileHelper::Instance()->IsFileExist(relativeFilePathName, true)) {
            // tentative de simplifier relativeFilePathName
            try {
                auto basePath = FileHelper::Instance()->GetAppPath();
                auto absPath = std::filesystem::canonical(relativeFilePathName);
                auto relPath = std::filesystem::relative(absPath, basePath).string();
                if (relPath.empty()) {
                    relativeFilePathName = absPath.string();
                } else {
                    relativeFilePathName = relPath;
                }
            } catch (const std::exception& ex) {
                LogVarError("%s", ex.what());
            }

            const auto mp = puFileListingExceptions;
            puMain_RenderPack->Finish(false);
            puCodeTree->Clear(mp);
            auto key = puCodeTree->AddOrUpdateFromFileAndGetKey(relativeFilePathName, true, true, false);
            if (key) {
                key = puCodeTree->GetShaderKey(relativeFilePathName);
                if (key) {
                    sCurrentFileLoaded = relativeFilePathName;

                    if (vDontLoadRenderPack) {
                        puMain_RenderPack->SetShaderKey(key, true);
                    } else {
                        // pour eviter un crash si un switcher etait deja affiche
                        puCodeTree->CloseUniformsConfigSwitcher();
                        SetCustomWidgetsOfRenderPack(puMain_RenderPack);
                        puMain_RenderPack->SetShaderKey(key, true);
                        puDisplay_RenderPack = puMain_RenderPack;
                        if (puMain_RenderPack->Load()) {
                            puCodeTree->FillIncludeFileList();
                            // NodeGraphSystem::Instance()->Parse(ShaderTypeEnum::SHADER_TYPE_FRAGMENT, puCodeTree, puDisplay_RenderPack);
                            puPreviewBufferId = ct::mini<int>(puPreviewBufferId, puMain_RenderPack->puZeroBasedMaxSliceBufferId);
                            auto modelPtr = puMain_RenderPack->GetModel().lock();
                            if (modelPtr) {
                                RecentFilesSystem::Instance()->AddFile(sCurrentFileLoaded, modelPtr->GetMeshType());
                                res = true;
                            }
                        }
                    }

                    if (ShaderKeyConfigSwitcherUnified::Instance()->IsActivated()) {
                        // on provoque le refresh si le panel est deja ouvert
                        ShaderKeyConfigSwitcherUnified::Instance()->Activate(true, puCodeTree, puMain_RenderPack, true);
                    }

                    CameraSystem::Instance()->NeedCamChange();
                    NeedRefresh(true);
                }
            }

            if (!key) {
                puMain_RenderPack->SetShaderKey(nullptr);
            }
        } else {
            LogVarLightInfo("Path not found : %s", relativeFilePathName.c_str());
        }
    }

    return res;
}

void MainBackend::NewFilePathName(const std::string& vFilePathName, const std::string& vType, const std::string& vFilePath) {
    const auto filePathName = FileHelper::Instance()->CorrectSlashTypeForFilePathName(vFilePathName);

    if (CodeGenerator::Instance()->CreateFilePathName(filePathName, vType, vFilePath, false)) {
        LoadFilePathName(vFilePathName, ".glsl");
    }
}

void MainBackend::NewFilePathNameFromTemplate(const std::string& vFileName, const std::string& vFilePath) {
    std::string _filePathName;
    if (TemplateSystem::Instance()->CreateFileFromSelectedTemplate(vFileName, vFilePath, &_filePathName)) {
        LoadFilePathName(_filePathName, ".glsl");
    }
}
