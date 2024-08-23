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
#include <ImGuiPack/ImGuiPack.h>
#ifdef WIN32
#include <windows.h>
#include <Metrics/Win/KGpuMon.h>
#endif

class MetricSystem
{
private:
	bool m_ApiInitialized = false;
#ifdef USE_GPU_METRIC
	KGpuMon m_KGpuMon;
#endif

private:
	ct::ActionTime puActionTime_GPUChecker;
	ct::ActionTime puActionTime_MemoryChecker;

public:
	static MetricSystem* Instance()
	{
		static MetricSystem _instance;
		return &_instance;
	}

protected:
	MetricSystem(); // Prevent construction
	MetricSystem(const MetricSystem&) = default; // Prevent construction by copying
	MetricSystem& operator =(const MetricSystem&) { return *this; }; // Prevent assignment
	~MetricSystem(); // Prevent unwanted destruction

public:
	void Init();
	void Unit();

	void DrawImGui();
	void DrawTooltip(ImVec2 vPos);
	void CaptureDatas(long vDelayInMs = 500);
	bool GetAppMemoryString(int vDelay, std::string* vStringToDisplay);
	bool IsOK() { return m_ApiInitialized; }

private:
#ifdef USE_GPU_METRIC

	void CaptureAndLogDatas();
	void DrawGpu(const GpuInfoStruct& vGpuInfoStruct);
	void DrawDisplayDeviceInfos(size_t vDisplayDeviceIdx);
	void DrawMemoryInfos(const GpuInfoStruct& vGpuInfoStruct);
	void DrawGpuInfos(const GpuInfoStruct& vGpuInfoStruct);
#endif
	std::string GetMemoryUsage();
};
