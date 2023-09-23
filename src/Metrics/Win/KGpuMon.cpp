/* 
   Not found the orioginla code
   so dont know any infos about the licensing
*/
 
// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifdef USE_GPU_METRIC

#include "KGpuMon.h"
#include <ctools/cTools.h>
#include <ctools/Logger.h>
#include <ImGuiPack.h>

KGpuMon::KGpuMon(void) = default;
KGpuMon::~KGpuMon(void)
{
    Unit();
}

// display information about the calling function and related error
void ShowErrorDetails(const NvAPI_Status nvRetVal, const char* pFunctionName)
{
    switch (nvRetVal)
    {
    case NVAPI_ERROR:
        LogVarError("[%s] ERROR: NVAPI_ERROR", pFunctionName);
        break;
    case NVAPI_INVALID_ARGUMENT:
        LogVarError("[%s] ERROR: NVAPI_INVALID_ARGUMENT - pDynamicPstatesInfo is NULL ", pFunctionName);
        break;
    case NVAPI_HANDLE_INVALIDATED:
        LogVarError("[%s] ERROR: NVAPI_HANDLE_INVALIDATED", pFunctionName);
        break;
    case NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE:
        LogVarError("[%s] ERROR: NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE", pFunctionName);
        break;
    case NVAPI_INCOMPATIBLE_STRUCT_VERSION:
        LogVarError("[%s] ERROR: NVAPI_INCOMPATIBLE_STRUCT_VERSION", pFunctionName);
        break;
    case NVAPI_NOT_SUPPORTED:
        LogVarError("[%s] ERROR: NVAPI_NOT_SUPPORTED", pFunctionName);
        break;
    default:
        LogVarError("[%s] ERROR: 0x%x", pFunctionName, nvRetVal);
        break;
    }
}

bool KGpuMon::Init()
{
    // Before any of the NVAPI functions can be used NvAPI_Initialize() must be called
    NvAPI_Status nvRetValue = NvAPI_Initialize();
    if (NVAPI_OK != nvRetValue)
    {
        ShowErrorDetails(nvRetValue, "NvAPI_Initialize");
        return false;
    }

    // Get the number of GPUs and actual GPU handles
    NvU32 uiNumGPUs = 0;
    NvPhysicalGpuHandle nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS];
    nvRetValue = NvAPI_EnumPhysicalGPUs(nvGPUHandle, &uiNumGPUs);
    if (NVAPI_OK != nvRetValue)
    {
        ShowErrorDetails(nvRetValue, "NvAPI_EnumPhysicalGPUs");
        NvAPI_Unload();
        return false;
    }

    // In the case that no GPUs were detected
    if (0 == uiNumGPUs)
    {
        LogVarError("No NVIDIA GPUs were detected.\r\n");
        NvAPI_Unload();
        return false;
    }

    if (EnupuDisplayDevices())
    {
        for (auto& device : pupDisplayDevices)
        {
            if (!GetGpuHandles(device.nvDisplayHandle, device))
                return false;
        }
    }

    return true;
}

bool KGpuMon::Unit()
{
    pupDisplayDevices.clear();
    NvAPI_Unload();

    return true;
}

void KGpuMon::CaptureGPUDatas()
{
    for (auto& device : pupDisplayDevices)
    {
        GetDisplayDeviceGpuUsages(device.nvDisplayHandle, device);
        GetDisplayDeviceMemoryInfo(device.nvDisplayHandle, device);
        GetDisplayDeviceGpuTemperatures(device.nvDisplayHandle, device);
        GetDisplayDeviceGpuTachometers(device.nvDisplayHandle, device);
        GetDisplayDeviceGpuClock(device.nvDisplayHandle, device);
        GetDisplayDeviceGpuCooler(device.nvDisplayHandle, device);
    }
}

std::vector<DisplayCardInfosStruct>* KGpuMon::GetDevices()
{
    return &pupDisplayDevices;
}

DisplayCardInfosStruct* KGpuMon::GetDevice(size_t vDeviceId)
{
    if (pupDisplayDevices.size() > vDeviceId)
        return &pupDisplayDevices[vDeviceId];
    return nullptr;
}

size_t KGpuMon::EnupuDisplayDevices()
{
    NvAPI_Status nvResult = NVAPI_ERROR;
    NvDisplayHandle nvDisplayDeviceHandle = 0;

    for (int nIndex = 0; nIndex < MAX_DISPLAY_CARDS; ++nIndex)
    {
        nvResult = NvAPI_EnumNvidiaDisplayHandle(nIndex, &nvDisplayDeviceHandle);
        if (nvResult == NVAPI_OK)
        {
            if (puNvDisplayHandles.find(nvDisplayDeviceHandle) == puNvDisplayHandles.end()) // non trouvé
            {
                puNvDisplayHandles.emplace(nvDisplayDeviceHandle);

                DisplayCardInfosStruct infos;
                infos.nvDisplayHandle = nvDisplayDeviceHandle;
                pupDisplayDevices.push_back(infos);
            }
        }
    }

    return pupDisplayDevices.size();
}

bool KGpuMon::GetGpuHandles(const NvDisplayHandle nvDisplayHandle, DisplayCardInfosStruct& vDisplayDeviceInfo)
{
    bool bResult = false;

    NvAPI_Status nvStatus;
    NvPhysicalGpuHandle* pnvHandles = 0;

    int nIndex = 0;
    NvU32 uiGpuCount = 0;

    pnvHandles = new NvPhysicalGpuHandle[NVAPI_MAX_PHYSICAL_GPUS];
    nvStatus = NvAPI_GetPhysicalGPUsFromDisplay(nvDisplayHandle, pnvHandles, &uiGpuCount);
    if (nvStatus == NVAPI_OK)
    {
        vDisplayDeviceInfo.info.gpuCount = ct::mini<uint32_t>(uiGpuCount, MAX_GPU_NUM);
        for (nIndex = 0; nIndex < vDisplayDeviceInfo.info.gpuCount; ++nIndex)
        {
            vDisplayDeviceInfo.info.gpuInfo[nIndex].nvGpuHandle = pnvHandles[nIndex];
        }

        bResult = true;
    }

    delete[]pnvHandles;
    pnvHandles = 0;

    return bResult;
}

bool KGpuMon::GetDisplayDeviceGpuUsages(const NvDisplayHandle nvDisplayHandle, DisplayCardInfosStruct& vDisplayDeviceInfo)
{
    UNUSED(nvDisplayHandle);
    UNUSED(vDisplayDeviceInfo);
    
    bool bResult = false;

    /*
    int nIndex = 0;

    NvAPI_Status nvStatus = NVAPI_ERROR;

    NvUsages pnvUsages;
    pnvUsages.Version = GPU_USAGES_VER;
    for (nIndex = 0; nIndex < vDisplayDeviceInfo->info.gpuCount; ++nIndex)
    {
        nvStatus = NvAPI_GPU_GetUsages(vDisplayDeviceInfo->info.gpuInfo[nIndex].nvGpuHandle, &pnvUsages);
        if (enumNvStatus_OK == nvStatus)
        {
            vDisplayDeviceInfo->info.gpuInfo[nIndex].nUsage = pnvUsages;
        }
    }

    bResult = (enumNvStatus_OK == nvStatus) ? true : false;
    */

    return bResult;
}

bool KGpuMon::GetDisplayDeviceMemoryInfo(const NvDisplayHandle nvDisplayHandle, DisplayCardInfosStruct& vDisplayDeviceInfo)
{
    UNUSED(nvDisplayHandle);

    if (vDisplayDeviceInfo.info.gpuCount)
    {
        NV_DISPLAY_DRIVER_MEMORY_INFO deviceMemoryStatus;
        deviceMemoryStatus.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER;
        for (size_t nIndex = 0; nIndex < vDisplayDeviceInfo.info.gpuCount; ++nIndex)
        {
            vDisplayDeviceInfo.info.gpuInfo[nIndex].memsFound = false;
            auto nvRetValue = NvAPI_GPU_GetMemoryInfo(vDisplayDeviceInfo.info.gpuInfo[nIndex].nvGpuHandle, (NV_DISPLAY_DRIVER_MEMORY_INFO*)&deviceMemoryStatus);
            if (nvRetValue == NVAPI_OK)
            {
                vDisplayDeviceInfo.info.gpuInfo[nIndex].mem = deviceMemoryStatus;
                vDisplayDeviceInfo.info.gpuInfo[nIndex].memsFound = true;
            }
        }

        return true;
    }
    

    return false;
}

bool KGpuMon::GetDisplayDeviceGpuTemperatures(const NvDisplayHandle nvDisplayHandle, DisplayCardInfosStruct& vDisplayDeviceInfo)
{
    UNUSED(nvDisplayHandle);

    NV_GPU_THERMAL_SETTINGS temperatureInfo;
    temperatureInfo.version = NV_GPU_THERMAL_SETTINGS_VER_2;
    for (size_t nIndex = 0U; nIndex < vDisplayDeviceInfo.info.gpuCount; ++nIndex)
    {
        vDisplayDeviceInfo.info.gpuInfo[nIndex].tempsFound = false;
        auto nvRetValue = NvAPI_GPU_GetThermalSettings(vDisplayDeviceInfo.info.gpuInfo[nIndex].nvGpuHandle, NVAPI_THERMAL_TARGET_ALL, (NV_GPU_THERMAL_SETTINGS *)&temperatureInfo);
        if (nvRetValue == NVAPI_OK)
        {
            vDisplayDeviceInfo.info.gpuInfo[nIndex].temp = temperatureInfo;
            vDisplayDeviceInfo.info.gpuInfo[nIndex].tempsFound = true;
        }

        return true;
    }

    return false;
}

bool KGpuMon::GetDisplayDeviceGpuTachometers(const NvDisplayHandle nvDisplayHandle, DisplayCardInfosStruct& vDisplayDeviceInfo)
{
    UNUSED(nvDisplayHandle);

    NvU32 nTacho = 0;
    for (size_t nIndex = 0U; nIndex < vDisplayDeviceInfo.info.gpuCount; ++nIndex)
    {
        vDisplayDeviceInfo.info.gpuInfo[nIndex].tachosFound = false;
        auto nvRetValue = NvAPI_GPU_GetTachReading(vDisplayDeviceInfo.info.gpuInfo[nIndex].nvGpuHandle, &nTacho);
        if (nvRetValue == NVAPI_OK)
        {
            vDisplayDeviceInfo.info.gpuInfo[nIndex].tacho = nTacho;
            vDisplayDeviceInfo.info.gpuInfo[nIndex].tachosFound = true;
        }

        return true;
    }

    return false;
}

bool KGpuMon::GetDisplayDeviceGpuClock(const NvDisplayHandle nvDisplayHandle, DisplayCardInfosStruct& vDisplayDeviceInfo)
{
    UNUSED(nvDisplayHandle);

    NV_GPU_CLOCK_FREQUENCIES clockInfo;
    clockInfo.version = NV_GPU_CLOCK_FREQUENCIES_VER;
    clockInfo.ClockType = NV_GPU_CLOCK_FREQUENCIES_CURRENT_FREQ;
    if (vDisplayDeviceInfo.info.gpuCount > 0) {
        for (size_t nIndex = 0U; nIndex < vDisplayDeviceInfo.info.gpuCount; ++nIndex) {
            vDisplayDeviceInfo.info.gpuInfo[nIndex].clocksFound = false;
            auto nvRetValue =
                NvAPI_GPU_GetAllClockFrequencies(vDisplayDeviceInfo.info.gpuInfo[nIndex].nvGpuHandle, (NV_GPU_CLOCK_FREQUENCIES*)&clockInfo);
            if (nvRetValue == NVAPI_OK) {
                vDisplayDeviceInfo.info.gpuInfo[nIndex].clock = clockInfo;
                vDisplayDeviceInfo.info.gpuInfo[nIndex].clocksFound = true;
            }
            return true;
        }
    }

    return false;
}

bool KGpuMon::GetDisplayDeviceGpuCooler(const NvDisplayHandle nvDisplayHandle, DisplayCardInfosStruct& vDisplayDeviceInfo)
{
    UNUSED(nvDisplayHandle);
    UNUSED(vDisplayDeviceInfo);

    return false;

    /*
    NvAPI_Status nvStatus = NVAPI_ERROR;
    NvGPUCoolerSettings sCoolerInfo;


    sCoolerInfo.Version = GPU_COOLER_SETTINGS_VER;

    for (nIndex = 0; nIndex < vDisplayDeviceInfo->info.gpuCount; ++nIndex)
    {
        nvStatus = NvAPI_GPU_GetCoolerSettings(vDisplayDeviceInfo->info.gpuInfo[nIndex].nvGpuHandle, 0, &sCoolerInfo);
        if (enumNvStatus_OK == nvStatus)
        {
            vDisplayDeviceInfo->info.gpuInfo[nIndex].nCooler = sCoolerInfo;
        }
    }

    bResult = (enumNvStatus_OK == nvStatus) ? true : false;
    */
}

#endif