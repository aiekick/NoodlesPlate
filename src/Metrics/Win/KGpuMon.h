#pragma once
#pragma warning(disable : 4702)
 
/* 
   Not found the orioginla code
   so dont know any infos about the licensing
*/
 
#ifdef USE_GPU_METRIC

#include <Windows.h>
#include <set>
#include <array>
#include <vector>
#include <NVApi_R470/nvapi.h>

#define MAX_GPU_NUM 4L
#define MAX_DISPLAY_CARDS 4 

struct GpuInfoStruct
{
	NvPhysicalGpuHandle nvGpuHandle = nullptr;
	NV_DISPLAY_DRIVER_MEMORY_INFO mem;
	bool memsFound = false;
	NV_GPU_THERMAL_SETTINGS temp;
	bool tempsFound = false;
	NV_GPU_CLOCK_FREQUENCIES clock;
	bool clocksFound = false;
	int tacho = 0;
	bool tachosFound = false;
};

struct DisplayInfosStruct
{
	int	gpuCount = 0;
	std::array<GpuInfoStruct, MAX_GPU_NUM> gpuInfo;
};

struct DisplayCardInfosStruct
{
	NvDisplayHandle nvDisplayHandle = nullptr;
	DisplayInfosStruct info;
};

class KGpuMon
{
private:
	std::vector<DisplayCardInfosStruct> pupDisplayDevices;
	std::set<NvDisplayHandle> puNvDisplayHandles;

public:
    KGpuMon(void);
    ~KGpuMon(void);
 
    bool Init();
    bool Unit();
 
	void CaptureGPUDatas();
	std::vector<DisplayCardInfosStruct>* GetDevices();
	DisplayCardInfosStruct* GetDevice(size_t vDeviceId);

private:
	size_t EnupuDisplayDevices();
	bool GetGpuHandles(const NvDisplayHandle nvDisplayHandle, DisplayCardInfosStruct& vDisplayDeviceInfo);
	bool GetDisplayDeviceGpuUsages(const NvDisplayHandle nvDisplayHandle, DisplayCardInfosStruct& vDisplayDeviceInfo);
	bool GetDisplayDeviceMemoryInfo(const NvDisplayHandle nvDisplayHandle, DisplayCardInfosStruct& vDisplayDeviceInfo);
	bool GetDisplayDeviceGpuTemperatures(const NvDisplayHandle nvDisplayHandle, DisplayCardInfosStruct& vDisplayDeviceInfo);
	bool GetDisplayDeviceGpuTachometers(const NvDisplayHandle nvDisplayHandle, DisplayCardInfosStruct& vDisplayDeviceInfo);
	bool GetDisplayDeviceGpuClock(const NvDisplayHandle nvDisplayHandle, DisplayCardInfosStruct& vDisplayDeviceInfo);
	bool GetDisplayDeviceGpuCooler(const NvDisplayHandle nvDisplayHandle, DisplayCardInfosStruct& vDisplayDeviceInfo);

};
 
#endif