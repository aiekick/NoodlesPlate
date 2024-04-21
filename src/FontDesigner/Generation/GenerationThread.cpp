// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <FontDesigner/Generation/GenerationThread.h>
#include <FontDesigner/Generation/Generator.h>
#include <SoGLSL/src/Gui/GuiBackend.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <chrono>

///////////////////////////////////////////////////////
//// WORKER THREAD ////////////////////////////////////
///////////////////////////////////////////////////////
GuiBackend_Window GenerationThread::sGenerationThread;
GenerationThreadParams* GenerationThread::Params = new GenerationThreadParams();
std::mutex GenerationThread::workerThread_Mutex;

std::atomic<float> GenerationThread::Progress(0.0f);
std::atomic<bool> GenerationThread::Working(false);
std::atomic<GLuint> GenerationThread::CurrentTexture(0);
std::atomic<float> GenerationThread::GenerationTime(0.0f);

void GenerateFullMapThread(GLFWwindow* win,
                           std::atomic<float>& vProgress,
                           std::atomic<bool>& vWorking,
                           std::atomic<GLuint>& vCurrentTexture,
                           std::atomic<float>& vGenerationTime) {
    glfwMakeContextCurrent(win);

    vProgress = 0.0f;

    vWorking = true;

    GenerationThreadParams* params = 0;

    /////////////////////////////////////////////////////////////////////
    /////// LOCK ////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    GenerationThread::workerThread_Mutex.lock();
    params = new GenerationThreadParams();
    params->Copy(GenerationThread::Params);
    GenerationThread::workerThread_Mutex.unlock();

    /////////////////////////////////////////////////////////////////////
    /////// UNLOCK //////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    Generator* generator = new Generator(win, params);

    /////////////////////////////////////////////////////////////////////
    /////// LOCK ////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    // GenerationThread::workerThread_Mutex.lock();
    // GenerationThread::workerThread_Mutex.unlock();

    /////////////////////////////////////////////////////////////////////
    /////// UNLOCK //////////////////////////////////////////////////////

    vGenerationTime = 0.0f;

    while (vWorking) {
        auto firstTimeMark = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        /////////////////////////////////////////////////////////////////////
        /////// LOCK ////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////

        GenerationThread::workerThread_Mutex.lock();
        GenerationThread::workerThread_Mutex.unlock();

        /////////////////////////////////////////////////////////////////////
        /////// UNLOCK //////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////

        generator->GenerateFullMap(std::ref(vProgress), std::ref(vWorking), std::ref(vCurrentTexture), std::ref(vGenerationTime));

        // oddly important, might need to be glFinish()
        glFlush();

        auto secondTimeMark = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        vGenerationTime = vGenerationTime + (secondTimeMark - firstTimeMark) / 1000.0f;
    }

    /////////////////////////////////////////////////////////////////////
    /////// LOCK ////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    GenerationThread::workerThread_Mutex.lock();
    GenerationThread::Params->Copy(params);
    GenerationThread::workerThread_Mutex.unlock();

    /////////////////////////////////////////////////////////////////////
    /////// UNLOCK //////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    vWorking = false;

    SAFE_DELETE(generator);

    // permet de s'assurer que la destruction du thread ne va pas detruire le contexte opengl
    // il resservira pour une autre execution du thread
    glfwMakeContextCurrent(0);
}

void UpdateCharsThread(GLFWwindow* win,
                       std::atomic<float>& vProgress,
                       std::atomic<bool>& vWorking,
                       std::atomic<GLuint>& vCurrentTexture,
                       std::atomic<float>& vGenerationTime) {
    glfwMakeContextCurrent(win);

    vProgress = 0.0f;

    vWorking = true;

    GenerationThreadParams* params = 0;

    /////////////////////////////////////////////////////////////////////
    /////// LOCK ////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    GenerationThread::workerThread_Mutex.lock();
    params = new GenerationThreadParams();
    params->Copy(GenerationThread::Params);
    GenerationThread::workerThread_Mutex.unlock();

    /////////////////////////////////////////////////////////////////////
    /////// UNLOCK //////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    Generator* generator = new Generator(win, params);

    /////////////////////////////////////////////////////////////////////
    /////// LOCK ////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    // GenerationThread::workerThread_Mutex.lock();
    // GenerationThread::workerThread_Mutex.unlock();

    /////////////////////////////////////////////////////////////////////
    /////// UNLOCK //////////////////////////////////////////////////////

    vGenerationTime = 0.0f;

    while (vWorking) {
        auto firstTimeMark = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        /////////////////////////////////////////////////////////////////////
        /////// LOCK ////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////

        GenerationThread::workerThread_Mutex.lock();
        GenerationThread::workerThread_Mutex.unlock();

        /////////////////////////////////////////////////////////////////////
        /////// UNLOCK //////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////

        generator->UpdateGhars(std::ref(vProgress), std::ref(vWorking), std::ref(vCurrentTexture), std::ref(vGenerationTime));

        // oddly important, might need to be glFinish()
        glFlush();

        auto secondTimeMark = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        vGenerationTime = vGenerationTime + (secondTimeMark - firstTimeMark) / 1000.0f;
    }

    /////////////////////////////////////////////////////////////////////
    /////// LOCK ////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    GenerationThread::workerThread_Mutex.lock();
    GenerationThread::Params->Copy(params);
    GenerationThread::Params->ModifiedGlyphs.clear();
    GenerationThread::workerThread_Mutex.unlock();

    /////////////////////////////////////////////////////////////////////
    /////// UNLOCK //////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    vWorking = false;

    SAFE_DELETE(generator);

    // permet de s'assurer que la destruction du thread ne va pas detruire le contexte opengl
    // il resservira pour une autre execution du thread
    glfwMakeContextCurrent(0);
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

GenerationThread::GenerationThread() = default;

GenerationThread::~GenerationThread() = default;

void GenerationThread::CreateThread(std::function<void()> vFinishFunc, bool vJustUpdate) {
    if (!StopWorkerThread()) {
        m_FinishFunc = vFinishFunc;

        if (vJustUpdate) {
            m_WorkerThread = std::thread(UpdateCharsThread,
                                         GenerationThread::sGenerationThread.win,
                                         std::ref(GenerationThread::Progress),
                                         std::ref(GenerationThread::Working),
                                         std::ref(GenerationThread::CurrentTexture),
                                         std::ref(GenerationThread::GenerationTime));
        } else {
            m_WorkerThread = std::thread(GenerateFullMapThread,
                                         GenerationThread::sGenerationThread.win,
                                         std::ref(GenerationThread::Progress),
                                         std::ref(GenerationThread::Working),
                                         std::ref(GenerationThread::CurrentTexture),
                                         std::ref(GenerationThread::GenerationTime));
        }
    }
}

bool GenerationThread::StopWorkerThread() {
    bool res = false;

    res = m_WorkerThread.joinable();
    if (res) {
        GenerationThread::Working = false;
        m_WorkerThread.join();
        m_FinishFunc();
    }

    return res;
}

bool GenerationThread::IsJoinable() {
    return m_WorkerThread.joinable();
}

void GenerationThread::FinishIfRequired() {
    if (m_WorkerThread.joinable() && GenerationThread::Working == false) {
        m_WorkerThread.join();
        m_FinishFunc();
    }
}

void GenerationThread::Join() {
    m_WorkerThread.join();
}