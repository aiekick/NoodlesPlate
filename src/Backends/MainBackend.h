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

#pragma once

#include <ctools/cTools.h>
#include <ctools/ConfigAbstract.h>
#include <PixelDebug/PixelDebug.h>
#include <Headers/RenderPackHeaders.h>
#include <Gui/GuiBackend.h>
#include <Renderer/RenderPack.h>
#include <Interfaces/MouseInterface.h>
#include <Interfaces/CameraInterface.h>

#include <unordered_set>
#include <string>
#include <memory>

typedef std::vector<std::string> URLLoadingErrorMessages;

class CodeTree;
class ShaderKey;
class FrameBuffer;
class UniformVariant;
class UniformsMultiLoc;
class FrameBuffersPipeLine;

struct GLFWmonitor;
class MainBackend : public conf::ConfigAbstract
{
public:
    static GuiBackend_Window sMainThread;
	static std::string sCurrentFileLoaded;
	static int sFullScreenWindowedMonitorIndex;
	static bool sDecorationWindowed;
	static bool sFullScreenWindowed;
	static ct::ivec2 sFullScreenWindowSize;
	static ct::ivec2 sNormalWindowSize;
	static ct::ivec2 sNormalWindowPos;
	static int sCurrentMonitorIndex;

public: // editor
	FrameBuffersPipeLinePtr puMainPipeLine = nullptr;
	ShaderKeyPtr puTriDAxisKey = nullptr;
	RenderPackPtr pu3dAxis_RenderPack = nullptr;
	ShaderKeyPtr puTriDGridKey = nullptr;
	RenderPackPtr pu3dGrid_RenderPack = nullptr;
	ShaderKeyPtr puMeshKey = nullptr;
	RenderPackPtr puMesh_RenderPack = nullptr;
	ShaderKeyPtr puBPPlanesKey = nullptr;
	RenderPackPtr puMain_RenderPack = nullptr;
	RenderPackWeak puDisplay_RenderPack;
	CodeTreePtr puCodeTree = nullptr;
	
public: // maps
	std::unordered_map<ShaderKeyPtr, RenderPackWeak> puShaderKeys;
	std::map<std::string, GuiBackend_Monitor> puMonitorsName;
	std::vector<GuiBackend_Monitor> puMonitors;

public:
	ct::ivec4 puAppRectIfNotmaximized;
	int puPreviewBufferId = 0;		// le buffer affiche actuellment [0 a 7]
	bool puShow3DSpace = false;		// montre les 3d grid et 3d axis
	bool puShowMesh = false;		// montre le shader de mesh
	bool puUseFXAA = false;			// on utilise la msaa x2 pour le smoothing des mesh
	bool puPlayCounterTimers = true;
	bool puCanWeTuneMouse = false;
	bool puCanWeTuneVR = false;
	bool puCanWeTuneCamera = false;
	bool puJustRecoveredFocus = false;
	bool puMouseDrag = false;
	bool puConsoleVisiblity = false;
	bool puNeedMidiUpdate = false;
	GLuint m_AppIconGLTextureID = 0;

public:
	ct::ivec2 puScreenSize;	// real screen size
	ct::fvec2 puDisplaySize; // display size (according to display quality and screensize)
	float puDisplayQuality = 1.0f; // display quality
	int puCountFXAASamples = 2;
	std::unordered_set<std::string> puFileListingExceptions;
	
	ImVec4 puBackgroundColor = ImVec4(0.15f, 0.16f, 0.17f, 1.0f);
	bool puCanWeRender = true;
	bool puNeedRefresh = true;	// permet le refesh de la vue quand l'ui a ?t? maj (widgets, settings,etc)

	// mouse
	MouseInterface puMouseInterface;
	ct::fvec4 puMouseFrameSize;
	ct::fvec2 puMousePos;
	ct::fvec2 puLastNormalizedMousePos;
	ct::fvec2 puNormalizedMousePos;

	PixelDebug puPixelDebug;

private: // resize needed // temporary vars
	ct::ivec2 prNewSize;
	float prNewQuality = 1.0f;
	bool prClearingIsNeeded = false;
	bool prResizeISForced = false;
	bool prResizeISNeeded = false;

private:
	URLLoadingErrorMessages prUrlErrorMessages;
	std::string prMainThreadWindowTitle;
	ct::ivec2 prPipeSize; // current pipe size
	int64_t prLastTimeMeasure = 0;
	float prDeltaTime = 0.0f;

public:
	static MainBackend* Instance()
	{
		static MainBackend _instance;
		return &_instance;
	}

protected:
	MainBackend(); // Prevent construction
	MainBackend(const MainBackend&) {}; // Prevent construction by copying
	MainBackend& operator =(const MainBackend&) { return *this; }; // Prevent assignment
	~MainBackend(); // Prevent unwanted destruction

public:
	void CreateWindows();
	void UpdateWindowStatus();
	bool Init(const ct::ivec2& vScreenSize);
	bool Load(const std::string& vFileToLoad);
	void Unit();

	bool IsLoaded();

	void NewFrame();
	void clearBG();
	void Render();
	void EndFrame();
	
	bool NewVRFrame();
	void RenderVRFrame();
	void RenderVRFBO();
	void EndVRFrame();
	void StartOrStopVR();

	void RefreshOneFrame();
	void NeedRefresh(bool vFlag = true);
	
	void Focus(bool vFocused);
	void GainFocus();
	void LooseFocus();
	
	void SetWindowPos(const GuiBackend_Window& vWindow, const ct::ivec2& vPos);
	void JustDropFiles(const GuiBackend_Window& vWindow, std::vector<std::string> cPaths);
	void SetFullScreen_Windowed(bool vFullScreen);
	
	void NeedColorClearing();
	void SetRenderSize(const ct::ivec2& vNewSize);
	void SetRenderQuality(const float& vNewQuality);
	void NeedResize(const ct::ivec2& vNewSize, const float& vQuality);
	
	void SetCustomWidgetsOfRenderPack(RenderPackWeak vRenderPack);
	bool UpdateFXAA();

	bool ForceReCompilation();
	bool DoShaderCodeUpdate(std::set<std::string> vFiles = std::set<std::string>(), bool vForceUpdate = false);
	void InitFilesTracker();
	void DoFileChanges(const std::set<std::string>& vFiles);
	void CheckIfTheseAreSomeFileChanges();
		
	void UpdateUniforms(
		RenderPackWeak vRenderPack, 
		UniformVariantPtr vUniPtr, 
		DisplayQualityType vDisplayQuality, 
		MouseInterface* vMouse, 
		CameraInterface* vCamera);
	
	ct::ivec4 GetWindowSize();
	ImVec2 GetWindowMousePos();

	void UpdateDeltaTime();

	void UpdateMouseDatas(bool vCanUseMouse);
	void UpdateCameraAndMouse(); // update camera / mouse / refresh ui if move
	
	void UpdateSound();
	void UpdateMidi();

	void FinishThreadsIfRequired();
	
	void SetPlayPause(bool vPlay);
	void ReSetPlayPause();
	void ResetFrame(); 
	void ResetTime();

	void SaveAsTemplate(const std::string& vFileName, const std::string& vFilePath);
	
	std::string ForkToFile(const std::string& vFilePathName);

	URLLoadingErrorMessages LoadUrl();
	void FinalizeUrlLoading();
	void FinalizeUrlLoading_By_CreateFiles();
	void FinalizeUrlLoading_By_CreateOneFile();

	std::string FinalizeShaderImport_By_CreateFiles(std::string vPath, std::list<ShaderInfos> vShaderInfos);
	std::string FinalizeShaderImport_By_CreateOneFile(std::string vPath, std::list<ShaderInfos> vShaderInfos);
	
	void LoadToClipBoard(std::string vString);
	void SetConsoleVisibility(bool vShow);
	
	void DrawMenu_DisplayQuality();
	void DrawMenu_FXAA();
	void DrawMenu_Edit();
	void ShowEditFileMenuKey(ShaderKeyPtr vKey, ct::fvec4 vLURD);
	
	bool LoadFilePathName(const std::string& vFilePathName, const std::string& vType = "", bool vDontLoadRenderPack = false);
	void NewFilePathName(const std::string& vFilePathName, const std::string& vType, const std::string& vFilePath);
	void NewFilePathNameFromTemplate(const std::string& vFileName, const std::string& vFilePath);

private:
	void SetPlayPause(bool vPlay, ShaderKeyPtr vKey);
	void ReSetPlayPause(ShaderKeyPtr vKey);
	
	void ExportFrame(FrameBuffersPipeLinePtr vPipe);
	
	void DoRendering();
	void DoRenderingVR();
	void DoRenderingOfSceneRenderPacks(bool vOnlyForSpace3DPipeLine);
	void DoRenderingOfRenderPacks();
	
	void BlitFrameBuffer();
	void BlitFrameBuffer(FrameBuffersPipeLinePtr vPipe);
	void BlitFrameBuffer(RenderPackWeak vRenderPack);
	void BlitFrameBufferFromTo(FrameBufferPtr vFrom, int vFromAttachmentId, FrameBufferPtr vTo, int vToAttachmentId);
	
	void ResizeIfNeeded();
    void UpdateCamera(const bool& vForce = false);
	bool ReSize(const ct::ivec2& vNewSize, const float& vQuality, bool vForceResize);
	void ApplyColorClearing();

	bool DoShaderCodeUpdateOfRenderPack(RenderPackWeak vRenderPack, bool vForceUpdate, std::set<std::string>* vUpdatedFiles);

public:
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;
};
