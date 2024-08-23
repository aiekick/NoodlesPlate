#pragma once

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

#include <ImGuiPack/ImGuiPack.h>
#include <EzExpr/EzExpr.hpp>
#include <unordered_map>

class OneExpr;
typedef std::shared_ptr<OneExpr> OneExprPtr;
typedef std::weak_ptr<OneExpr> OneExprWeak;

enum class WidgetType { WIDGET_INPUT = 0, WIDGET_SLIDER, Count };

class ChartSystem;
class OneExpr {
public:
    static constexpr int32_t s_COUNT_POINTS = 1000U;
    typedef std::array<double, s_COUNT_POINTS> Points;
    static OneExprPtr create(const std::string& vLabel, const int32_t& vIdx);

private:
    OneExprWeak m_This;
    Points m_axisX{};
    Points m_axisY{};
    ez::Expr m_Expr;
    ez::ErrorCode m_ErrorCode = ez::ErrorCode::NONE;
    std::string m_ErrorMsg;
    std::string m_Label;
    int32_t m_Idx;
    ImWidgets::InputText m_ExprInput;
    std::unordered_map<ez::String, WidgetType> m_WidgetTypes;
    ImPlotRect m_ChartLimits;

public:
    bool init(const std::string& vLabel, const int32_t& vIdx);
    void drawCurve();
    void drawWidgets();
    void reComputeIfNeeded(const ImPlotRect& vRect);
    Points& getAxisXRef();
    Points& getAxisYRef();

private:
    bool m_computeExpr();
};

class ChartSystem {
public:
    typedef ImPlotRect CharLimits;

private:
    std::vector<OneExprPtr> m_Exprs;
    ImPlotRect m_ChartLimits;

public:
    bool init();
    void unit();
    void drawChart();
    void drawExpr();

public:
    static ChartSystem* Instance() {
        static ChartSystem _instance;
        return &_instance;
    }
};
