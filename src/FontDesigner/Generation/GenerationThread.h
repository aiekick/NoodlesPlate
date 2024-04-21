#pragma once

#include <ctools/cTools.h>
#include <Headers/Globals.h>

#include <FontDesigner/Generation/GenerationThreadParams.h>

#include <Gui/GuiBackend.h>

#include <functional>
#include <future>
#include <thread>
#include <atomic>
#include <string>
#include <vector>
#include <mutex>
#include <list>
using namespace std;

class GenerationThread
{
public:
    static GuiBackend_Window sGenerationThread;
	static GenerationThreadParams *Params;
	static std::mutex workerThread_Mutex;
	static std::atomic<float> Progress;
	static std::atomic<bool> Working;
	static std::atomic<GLuint> CurrentTexture;
	static std::atomic< float > GenerationTime;

private:
	std::thread m_WorkerThread;
	float m_GenerationTime;
	std::function<void()> m_FinishFunc;
	
public:
	GenerationThread();
	~GenerationThread();

	void CreateThread(std::function<void()> vFinishFunc, bool vJustUpdate);
	
	bool StopWorkerThread();
	
	bool IsJoinable();
	void Join();
	void FinishIfRequired();
};
