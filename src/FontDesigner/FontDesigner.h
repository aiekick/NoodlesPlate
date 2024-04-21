#pragma once

#include <memory>

#include <ImGuiPack/ImGuiPack.h>
#include <FontDesigner/Explorer/FontExplorer.h>
#include <FontDesigner/Generation/GenerationThread.h>

class FontDesigner {
private:
    std::unique_ptr<FontExplorer> m_FontExplorerPtr = nullptr;
    GenerationThread m_GenerationThread;

    ct::fvec3 m_FontTestBackgroundColor;
    ct::fvec3 m_FontMapBackgroundColor;
    std::string m_FontToLoad;
    std::string m_CurrentSelectedFont;
    std::string m_CurrentNotLoadedFont;
    std::string m_FilePathNameForFntExport;
    std::string m_FontFilePathName;

    // custom ImGui Font
    ImFont* m_TestFont = nullptr;
    ImFontConfig m_TestFontConfig;
    ImFontAtlas m_TestFontAtlas;

    std::string m_charSet;

    char m_CurrentSelectedChar;
    ct::fvec4 m_SelectionColor;
    ct::fvec4 m_ModificationColor;

    // mulit selection
    std::map<char, bool> m_MultiSelection;
    bool m_GotoSelectedFontName;
    bool m_ProgressChanged;
    bool m_NeedRegeneration;
    bool m_ThreadToStart;
    bool m_ThreadToStop;

public:
    bool init();
    void unit();

    void drawPane();
    void drawOverlay();

private:
    void m_displayAllGlyphsOfFont(ImFont* vFontPtr, const float& vWidth);
    void LoadPendingFonts();
    void SetFontToLoad(std::string vFontName);
    void GoToSelectedFont();
    bool LoadFontOnDemand(std::string vFontName, float vFontSize);
    bool LoadFontFileOnDemand(std::string vFontFilePathName, float vFontSize);
    bool ReGenerateFontTexture();
    void UpdateFontHeight();
    void LoadBitmapFontIntoImguiFont();
    void UpdateFontTexFromFBO();
    void ExportToFntFile();
    void ResetSelection();
    void RefreshSelectionFromNewFontMap();
    void DrawFontTexture(ImTextureID vId, ImVec2 vPos, ImVec2 vSize, float vDamierZoom);
    void SaveToPictureFile(std::string vFilePathName, bool vFlipY, int vCountSamples);
    void StartWorkerThread(bool vJustUpdate);
    void StopWorkerThread();
    void WorkerThreadStoppedOrFinished();
    void SetCharset(std::string vCharSet);
    void UpdateUniforms(
        RenderPack* vRenderPack,
        UniformVariant* vUni,
        bool vCanUpdateMouse,
        float vDisplayQuality,
        CameraInterface* vCamera,
        ct::frect* vMouseRect);

public:
    static FontDesigner* Instance() {
        static FontDesigner _instance;
        return &_instance;
    }
};