#pragma once

#include <ImGuiPack/ImGuiPack.h>
#include <ctools/cTools.h>

#include <tinyexpr/tinyexpr.h>

#include <map>
#include <vector>
#include <string>
#include <cstdint>

class ChartSystem {
private:
    static constexpr size_t s_EXPR_MAX_LEN = 1024;
    int m_Err_x = 0, m_Err_y = 0, m_Err_z = 0;
    char m_ExprX[s_EXPR_MAX_LEN + 1] = "10.0*(y-x)";
    char m_ExprY[s_EXPR_MAX_LEN + 1] = "28.0*x-y-x*z";
    char m_ExprZ[s_EXPR_MAX_LEN + 1] = "x*y-2.66667*z";
    ct::dvec3 m_StartLocation = 0.001;
    uint32_t m_StepCount = 10000U;
    double m_StepSize = 0.01;
    std::map<std::string, double> m_VarNameValues;
    std::vector<te_variable> m_Vars;
    char m_VarToAddBuffer[s_EXPR_MAX_LEN + 1] = "";
    bool m_CloseCurve = false;
    ct::dvec3 m_CenterPoint;
    float m_ChartWidth = 0.0f;
    float m_WidgetsWidth = 0.0f;

public:
    ChartSystem() = default;
    ~ChartSystem() = default;

    void draw();

private:
    void m_AddVar(const std::string& vName, const double& vValue);
    void m_DelVar(const std::string& vName);
    bool m_DrawInputExpr(const char* vLabel,        //
                         const char* vBufferLabel,  //
                         char* vBuffer,             //
                         size_t vBufferSize,        //
                         const int& vError,         //
                         const char* vDdefaultValue);
    bool m_DrawVars();
    void m_DrawWidgets(const float vWidth);
    void m_DrawChart(const float vWidth);
    void m_Compute();

public:
    static ChartSystem* Instance() {
        static ChartSystem _instance;
        return &_instance;
    }
};
