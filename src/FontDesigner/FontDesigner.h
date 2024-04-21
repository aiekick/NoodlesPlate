#pragma once

#include <memory>
#include <ctools/cTools.h>
#include <Texture/Texture2D.h>
#include <ImGuiPack/ImGuiPack.h>
#include <Interfaces/CameraInterface.h>
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

    bool m_showOnlyFontWithExistingConfig = false;

    ImWidgets::InputText m_SearchInput;
    ImWidgets::InputText m_CharSetInput;

    // custom ImGui Font
    ImFont* m_TestFont = nullptr;
    ImFontConfig m_TestFontConfig;
    ImFontAtlas m_TestFontAtlas;

    std::string m_charSet;

    char m_CurrentSelectedChar = 0;
    ct::fvec4 m_SelectionColor;
    ct::fvec4 m_ModificationColor;

    // mulit selection
    std::map<char, bool> m_MultiSelection;
    bool m_GotoSelectedFontName = false;
    bool m_ProgressChanged = false;
    bool m_NeedRegeneration = false;
    bool m_ThreadToStart = false;
    bool m_ThreadToStop = false;
    bool m_JustUpdate = false;
    bool m_CanTuneUVOffset = false;
    bool m_CanTuneMouse = false;
    bool m_CanTuneCamera = false;
    bool m_OneBigCharMode = false;

    ct::texture m_FontTexture;
    GLuint m_FontTextureID = 0;
    Texture2D m_Damier;

    bool m_isEnabled = false;

public:
    bool init();
    void unit();

    void enable(const bool& vEnabled);
    const bool& isEnabled();

    void drawPane();
    void drawOverlay();

private:
    void m_displaySystemFontExplorer();
    void m_diplayGenerationThread();
    void m_displayCharSet();
    void m_displayParams();
    void m_displayGlyphSelection();
    void m_displayGlyphInfos();
    void m_displayTexture();
    void m_displayAllGlyphsOfFont(ImFont* vFontPtr, const float& vWidth);

    void LoadPendingFonts();
    void SetFontToLoad(std::string vFontName);
    void GoToSelectedFont();
    bool LoadFontOnDemand(std::string vFontName, float vFontSize);
    bool LoadFontFileOnDemand(std::string vFontFilePathName, float vFontSize);
    bool ReGenerateFontTexture();
    void UpdateFontHeight();
    void LoadBitmapFontIntoImguiFont();
    void UpdateGlyphCenterOffsets();
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
		RenderPackWeak vRenderPack, 
		UniformVariantPtr vUniPtr, 
		DisplayQualityType vDisplayQuality, 
		MouseInterface* vMouse, 
		CameraInterface* vCamera);
	

public:
    static FontDesigner* Instance() {
        static FontDesigner _instance;
        return &_instance;
    }
};