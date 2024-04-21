#pragma once

#include <ctools/cTools.h>
#include <Headers/Globals.h>

#include <FontDesigner/Generation/GenerationThreadParams.h>

#include <future>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <list>
using namespace std;

class GenerationThread
{
public:
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
