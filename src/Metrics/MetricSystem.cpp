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

#include "MetricSystem.h"
#include <ImGuiPack.h>
#ifdef WIN32
#include <psapi.h> // check memory usage
#endif

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#ifdef USE_GPU_METRIC
static std::string FormatNum(size_t vNum)
{
	std::string mem = ct::toStr(vNum);
	std::string unit = "";
	double valueReduced = (double)vNum;
	if (mem.size() > 12)
	{
		valueReduced /= pow(10.0, 12.0);
		unit = " To ";
	}
	else if (mem.size() > 9)
	{
		valueReduced /= pow(10.0, 9.0);
		unit = " Go ";
	}
	else if (mem.size() > 6)
	{
		valueReduced /= pow(10.0, 6.0);
		unit = " Mo ";
	}
	else if (mem.size() > 3)
	{
		valueReduced /= pow(10.0, 3.0);
		unit = " Ko ";
	}

	valueReduced /= 1.024; // conversion en byte => / 1024 * 1000
	valueReduced = static_cast<size_t>(valueReduced * 100.0) / 100.0;

	return ct::toStr(valueReduced) + unit;
}
#endif

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

MetricSystem::MetricSystem()
{
	m_ApiInitialized = false;
}

MetricSystem::~MetricSystem()
{
	Unit();
}

void MetricSystem::Init()
{
#ifdef USE_GPU_METRIC
	m_ApiInitialized = m_KGpuMon.Init();
#endif
}

void MetricSystem::Unit()
{
#ifdef USE_GPU_METRIC
    m_KGpuMon.Unit();
#endif
}

void MetricSystem::CaptureDatas(long vDelayInMs)
{
	if (!m_ApiInitialized) return;

#ifdef USE_GPU_METRIC
	if (puActionTime_GPUChecker.IsTimeToAct(vDelayInMs, true))
	{
		m_KGpuMon.CaptureGPUDatas();
	}
#else
	UNUSED(vDelayInMs);
#endif
}

void MetricSystem::DrawImGui()
{
	if (!m_ApiInitialized) return;
	
#ifdef USE_GPU_METRIC
	if (ImGui::BeginMenu("GPU"))
	{
		CaptureDatas();

		auto device = m_KGpuMon.GetDevices();
		if (device)
		{
			for (size_t i = 0U; i < device->size(); ++i)
			{
				DrawDisplayDeviceInfos(i);
			}
		}

		ImGui::EndMenu();
	}
#endif
}

void MetricSystem::DrawTooltip(ImVec2 vPos)
{
	if (!m_ApiInitialized) return;

#ifdef USE_GPU_METRIC
	CaptureDatas();

	const float offset = 15.0f;
	const auto pad = ImGui::GetStyle().FramePadding;

	static auto sz = ImVec2();
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoInputs | 
		ImGuiWindowFlags_NoTitleBar | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoSavedSettings | 
		ImGuiWindowFlags_AlwaysAutoResize | 
		ImGuiWindowFlags_NoDocking | 
		ImGuiWindowFlags_Tooltip |
		ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
	ImGui::SetNextWindowPos(ImVec2(vPos.x, vPos.y - sz.y * 0.5f));
	const auto winColor = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);

	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);

	ImGuiWindowClass window_class;
	window_class.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
	ImGui::SetNextWindowClass(&window_class);

	ImGui::Begin("##MetricSystem", NULL, flags);
	
	auto drawList = ImGui::GetCurrentWindowRead()->DrawList;

	drawList->AddCircleFilled(ImVec2(vPos.x + offset * 1.25f, vPos.y), offset * 0.75f, ImGui::GetColorU32(ImGuiCol_Button), 4);
	
	auto p = ImGui::GetCursorScreenPos();
	
	drawList->AddRectFilled(ImVec2(p.x + offset, p.y), ImVec2(p.x + sz.x, p.y + sz.y), ImGui::GetColorU32(winColor), 0.0f);

	ImGui::SetCursorScreenPos(ImVec2(p.x + offset + pad.x, p.y + pad.y));
	
	auto device = m_KGpuMon.GetDevices();
	if (device)
	{
		if (!device->empty())
		{
			if (device->at(0).info.gpuCount)
			{
				DrawGpu(device->at(0).info.gpuInfo[0]);
			}
		}
	}

	sz = ImGui::GetWindowSize();

	drawList->AddRect(ImVec2(p.x + offset, p.y), ImVec2(p.x + sz.x - pad.x * 1.5f, p.y + sz.y - pad.y * 1.5f), ImGui::GetColorU32(ImGuiCol_Button), 0.0f, 0, pad.x);
	
	ImGui::End();

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
#else
	UNUSED(vPos);
#endif
}

bool MetricSystem::GetAppMemoryString(int vDelay, std::string* vStringToDisplay)
{
	if (vStringToDisplay)
	{
		if (puActionTime_MemoryChecker.IsTimeToAct(vDelay, true))
		{
			*vStringToDisplay = GetMemoryUsage();
			return true;
		}
	}

	return false;
}

std::string MetricSystem::GetMemoryUsage() // http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
{
	std::string res;

#ifdef USE_GPU_METRIC
#ifdef WIN32
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

	SIZE_T value = pmc.PagefileUsage;
	std::string mem = ct::toStr(value);
	const char* unit = "";
	double valueReduced = (double)value;
	if (mem.size() > 12)
	{
		valueReduced /= pow(10.0, 12.0);
		unit = " TO";
	}
	else if (mem.size() > 9)
	{
		valueReduced /= pow(10.0, 9.0);
		unit = " GO";
	}
	else if (mem.size() > 6)
	{
		valueReduced /= pow(10.0, 6.0);
		unit = " MO";
	}
	else if (mem.size() > 3)
	{
		valueReduced /= pow(10.0, 3.0);
		unit = " KO";
	}

	valueReduced = int(valueReduced * 100.0) / 100.0;

	res = ct::toStr("[Mem %.2f %s]", valueReduced, unit);
#endif
#endif
	return res;
}

#ifdef USE_GPU_METRIC

void MetricSystem::DrawGpu(const GpuInfoStruct& vGpuInfoStruct)
{
	static ImGuiTableFlags flags =
		ImGuiTableFlags_SizingFixedFit |
		//ImGuiTableFlags_RowBg |
		ImGuiTableFlags_ScrollY |
		ImGuiTableFlags_NoHostExtendY;
	if (ImGui::BeginTable("##Mems", 2, flags))
	{
		//ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
		ImGui::TableSetupColumn(" Label ", ImGuiTableColumnFlags_WidthFixed, -1, 0);
		ImGui::TableSetupColumn(" Value ", ImGuiTableColumnFlags_WidthFixed, -1, 1);
		//ImGui::TableHeadersRow();

		DrawGpuInfos(vGpuInfoStruct);
		DrawMemoryInfos(vGpuInfoStruct);

		ImGui::EndTable();
	}
}

void MetricSystem::DrawDisplayDeviceInfos(size_t vDisplayDeviceIdx)
{
	auto device = m_KGpuMon.GetDevice(vDisplayDeviceIdx);
	if (device)
	{
		char buffer[256];
		snprintf(buffer, 256, "Display %i :", (int)vDisplayDeviceIdx);
		if (ImGui::BeginMenu(buffer))
		{
			for (int n = 0; n < device->info.gpuCount; ++n)
			{
				ImGui::Text("GPU %i :", n);

				DrawGpu(device->info.gpuInfo[n]);
			}

			ImGui::EndMenu();
		}
	}
}

void MetricSystem::DrawMemoryInfos(const GpuInfoStruct& vGpuInfoStruct)
{
	if (vGpuInfoStruct.memsFound)
	{
		const size_t totalMemory = vGpuInfoStruct.mem.availableDedicatedVideoMemory;
		const size_t usedMemory = vGpuInfoStruct.mem.curAvailableDedicatedVideoMemory;
		const size_t freeMemory = totalMemory - usedMemory;
		const double fMemUtilzation = (((double)usedMemory / (double)totalMemory) * 100.0);

		if (ImGui::TableNextColumn()) ImGui::Text(" Total memory");
		if (ImGui::TableNextColumn()) ImGui::Text("%s", FormatNum(totalMemory * 1000).c_str());

		if (ImGui::TableNextColumn()) ImGui::Text(" Free memory");
		if (ImGui::TableNextColumn()) ImGui::Text("%s", FormatNum(freeMemory * 1000).c_str());

		if (ImGui::TableNextColumn()) ImGui::Text(" Used memory");
		if (ImGui::TableNextColumn()) ImGui::Text("%s", FormatNum(usedMemory * 1000).c_str());

		if (ImGui::TableNextColumn()) ImGui::Text(" Load memory");
		if (ImGui::TableNextColumn()) ImGui::Text("%.2f %%", fMemUtilzation);
	}
}

void MetricSystem::DrawGpuInfos(const GpuInfoStruct& vGpuInfoStruct)
{
	//ImGui::Text("  Core Load :         %i %%", vDisplayCardInfosStruct->info.gpuInfo[n].nUsage.Usages[2]);
	//ImGui::Text("  Memory Ctrl Load :  %i %%", vDisplayCardInfosStruct->info.gpuInfo[n].nUsage.Usages[6]);
	//ImGui::Text("  Video Engine Load : %i %%", vDisplayCardInfosStruct->info.gpuInfo[n].nUsage.Usages[10]);

	if (vGpuInfoStruct.tempsFound)
	{
		for (int i = 0; i < (int)vGpuInfoStruct.temp.count; i++)
		{
			if (ImGui::TableNextColumn()) ImGui::Text(" Temp Sensor");
			switch (vGpuInfoStruct.temp.sensor[i].controller)
			{
			case NVAPI_THERMAL_CONTROLLER_GPU_INTERNAL:
				if (ImGui::TableNextColumn()) ImGui::Text("Internal ");
				break;
			case NVAPI_THERMAL_CONTROLLER_ADM1032:
				if (ImGui::TableNextColumn()) ImGui::Text("ADM1032 ");
				break;
			case NVAPI_THERMAL_CONTROLLER_MAX6649:
				if (ImGui::TableNextColumn()) ImGui::Text("MAX6649 ");
				break;
			case NVAPI_THERMAL_CONTROLLER_MAX1617:
				if (ImGui::TableNextColumn()) ImGui::Text("MAX1617 ");
				break;
			case NVAPI_THERMAL_CONTROLLER_LM99:
				if (ImGui::TableNextColumn()) ImGui::Text("LM99 ");
				break;
			case NVAPI_THERMAL_CONTROLLER_LM89:
				if (ImGui::TableNextColumn()) ImGui::Text("LM89 ");
				break;
			case NVAPI_THERMAL_CONTROLLER_LM64:
				if (ImGui::TableNextColumn()) ImGui::Text("LM64 ");
				break;
			case NVAPI_THERMAL_CONTROLLER_ADT7473:
				if (ImGui::TableNextColumn()) ImGui::Text("ADT7473 ");
				break;
			case NVAPI_THERMAL_CONTROLLER_SBMAX6649:
				if (ImGui::TableNextColumn()) ImGui::Text("SBMAX6649 ");
				break;
			case NVAPI_THERMAL_CONTROLLER_VBIOSEVT:
				if (ImGui::TableNextColumn()) ImGui::Text("VBIOSEVT ");
				break;
			case NVAPI_THERMAL_CONTROLLER_OS:
				if (ImGui::TableNextColumn()) ImGui::Text("Os ");
				break;
			default:
				if (ImGui::TableNextColumn()) ImGui::Text("Unknow ");
				break;
			}

			if (vGpuInfoStruct.temp.sensor[i].target == NV_THERMAL_TARGET::NVAPI_THERMAL_TARGET_GPU)
			{
				if (ImGui::TableNextColumn()) ImGui::Text(" GPU Temp");
				if (ImGui::TableNextColumn()) ImGui::Text("%i Deg ", vGpuInfoStruct.temp.sensor[i].currentTemp);
			}
			else if (vGpuInfoStruct.temp.sensor[i].target == NV_THERMAL_TARGET::NVAPI_THERMAL_TARGET_MEMORY)
			{
				if (ImGui::TableNextColumn()) ImGui::Text(" Memory Temp");
				if (ImGui::TableNextColumn()) ImGui::Text("%i Deg ", vGpuInfoStruct.temp.sensor[i].currentTemp);
			}
			else if (vGpuInfoStruct.temp.sensor[i].target == NV_THERMAL_TARGET::NVAPI_THERMAL_TARGET_POWER_SUPPLY)
			{
				if (ImGui::TableNextColumn()) ImGui::Text(" Power Supply Temp");
				if (ImGui::TableNextColumn()) ImGui::Text("%i Deg ", vGpuInfoStruct.temp.sensor[i].currentTemp);
			}
			else if (vGpuInfoStruct.temp.sensor[i].target == NV_THERMAL_TARGET::NVAPI_THERMAL_TARGET_BOARD)
			{
				if (ImGui::TableNextColumn()) ImGui::Text(" Board Temp");
				if (ImGui::TableNextColumn()) ImGui::Text("%i Deg ", vGpuInfoStruct.temp.sensor[i].currentTemp);
			}
		}
	}

	if (vGpuInfoStruct.clocksFound)
	{
		if (vGpuInfoStruct.clock.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].bIsPresent)
		{
			uint32_t cc = (uint32_t)(vGpuInfoStruct.clock.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency / 1e3);
			if (ImGui::TableNextColumn()) ImGui::Text(" Graphic Clock");
			if (ImGui::TableNextColumn()) ImGui::Text("%u MHz ", cc);
		}

		if (vGpuInfoStruct.clock.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].bIsPresent)
		{
			uint32_t cc = (uint32_t)(vGpuInfoStruct.clock.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency / 1e3);
			if (ImGui::TableNextColumn()) ImGui::Text(" Memory Clock");
			if (ImGui::TableNextColumn()) ImGui::Text("%u MHz ", cc);
		}

		if (vGpuInfoStruct.clock.domain[NVAPI_GPU_PUBLIC_CLOCK_PROCESSOR].bIsPresent)
		{
			uint32_t cc = (uint32_t)(vGpuInfoStruct.clock.domain[NVAPI_GPU_PUBLIC_CLOCK_PROCESSOR].frequency / 1e3);
			if (ImGui::TableNextColumn()) ImGui::Text(" Processor Clock");
			if (ImGui::TableNextColumn()) ImGui::Text("%u MHz ", cc);
		}

		if (vGpuInfoStruct.clock.domain[NVAPI_GPU_PUBLIC_CLOCK_VIDEO].bIsPresent)
		{
			uint32_t cc = (uint32_t)(vGpuInfoStruct.clock.domain[NVAPI_GPU_PUBLIC_CLOCK_VIDEO].frequency / 1e3);
			if (ImGui::TableNextColumn()) ImGui::Text(" Video Clock");
			if (ImGui::TableNextColumn()) ImGui::Text("%u MHz ", cc);
		}
	}
	
	if (vGpuInfoStruct.tachosFound)
	{
		if (vGpuInfoStruct.tacho > 0)
		{
			if (ImGui::TableNextColumn()) ImGui::Text(" Fan");
			if (ImGui::TableNextColumn()) ImGui::Text("%i ", vGpuInfoStruct.tacho);
		}
	}
}

void MetricSystem::CaptureAndLogDatas()
{
	CaptureDatas(10);

	/*
	TracyPlotConfig("Gpu Load", tracy::PlotFormatType::Percentage);
	TracyPlotConfig("Gpu Memory", tracy::PlotFormatType::Percentage);
	//TracyPlotConfig("Gpu Video", tracy::PlotFormatType::Percentage);
	TracyPlotConfig("Mem Load", tracy::PlotFormatType::Percentage);

	//TracyPlotConfig("Mem Total", tracy::PlotFormatType::Memory);
	//TracyPlotConfig("Mem Free", tracy::PlotFormatType::Memory);
	//TracyPlotConfig("Mem Used", tracy::PlotFormatType::Memory);
	
	for (int i = 0; i < puDisplayDeviceCount; i++)
	{
		for (int n = 0; n < puInfos[i].gpuCount; n++)
		{
			TracyPlot("Gpu Load", (int64_t)puInfos[i].gpuInfo[n].nUsage.Usages[2]);
			TracyPlot("Gpu Memory", (int64_t)puInfos[i].gpuInfo[n].nUsage.Usages[6]);
			//TracyPlot("Gpu Video", (int64_t)puInfos[i].gpuInfo[n].nUsage.Usages[10]);
		}

		//size_t totalMemory = puInfos[i].mem.Values[0];
		//size_t freeMemory = puInfos[i].mem.Values[4];
		//size_t usedMemory = ct::maxi<size_t>(totalMemory - freeMemory, 0);
		//size_t loadMemory = (size_t)(100.0f * usedMemory / totalMemory);

		//TracyPlot("Mem Total", (int64_t)totalMemory);
		//TracyPlot("Mem Free", (int64_t)freeMemory);
		//TracyPlot("Mem Used", (int64_t)usedMemory);
		//TracyPlot("Mem Load", (int64_t)loadMemory);
	}
	*/
}
#endif