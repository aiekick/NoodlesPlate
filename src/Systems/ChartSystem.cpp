#include "ChartSystem.h"

#include <Res/CustomFont.h>

void ChartSystem::draw() {
    m_DrawChart(m_ChartWidth);
    const auto avail = ImGui::GetContentRegionAvail();
    ImGui::SameLine();
    ImGui::Splitter(true, 3.0f, &m_ChartWidth, &m_WidgetsWidth, avail.x * 0.2f, avail.x * 0.2f, avail.y);
    ImGui::SameLine();
    m_DrawWidgets(m_WidgetsWidth);
}

void ChartSystem::m_AddVar(const std::string& vName, const double& vValue) {
    if (!vName.empty()) {
        if (m_VarNameValues.find(vName) == m_VarNameValues.end()) {
            m_VarNameValues.emplace(vName, vValue);
            const auto& it = m_VarNameValues.find(vName);
            te_variable v;
            v.name = it->first.c_str();  // ref so m_VarNames name must already exist
            v.address = &it->second;
            v.context = 0;
            v.type = 0;
            m_Vars.push_back(v);
        }
    }
}

void ChartSystem::m_DelVar(const std::string& vName) {
    auto it = m_VarNameValues.find(vName);
    if (it != m_VarNameValues.end()) {
        for (auto var_it = m_Vars.begin(); var_it != m_Vars.end(); ++var_it) {
            if (strcmp(var_it->name, it->first.c_str()) == 0) {  // is equal
                m_Vars.erase(var_it);
                m_VarNameValues.erase(vName);
                break;
            }
        }
    }
}

bool ChartSystem::m_DrawInputExpr(const char* vLabel, const char* vBufferLabel, char* vBuffer, size_t vBufferSize, const int& vError, const char* vDdefaultValue) {
    bool change = false;

    ImGui::Text(vLabel);
    ImGui::SameLine();
    if (vError)
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::CustomStyle::BadColor);
    if (ImGui::ContrastedButton(ICON_NDP_RESET)) {
        ct::ResetBuffer(vBuffer);
        ct::AppendToBuffer(vBuffer, vBufferSize, vDdefaultValue);
    }
    ImGui::SameLine();
    change |= ImGui::InputText(vBufferLabel, vBuffer, vBufferSize);
    if (vError) {
        ImGui::PopStyleColor();
        ImGui::Text("Err at pos : %i", vError);
    }

    return change;
}

bool ChartSystem::m_DrawVars() {
    static std::string var_to_erase;

    bool change = false;

    ImGui::Separator();

    if (ImGui::ContrastedButton("Add Var")) {
        m_AddVar(m_VarToAddBuffer, 0.0);
        change = true;
    }

    ImGui::SameLine();

    ImGui::InputText("##VarToAdd", m_VarToAddBuffer, s_EXPR_MAX_LEN);

    ImGui::Separator();

    for (auto& var : m_VarNameValues) {
        if (var.first == "x" || var.first == "y" || var.first == "z")
            continue;

        if (ImGui::ContrastedButton(ICON_NDP_RESET)) {
            var_to_erase = var.first;
        }
        ImGui::SameLine();
        ImGui::Text("%s", var.first.c_str());
        ImGui::SameLine();
        ImGui::PushID(&var.second);
        change |= ImGui::InputDouble("##value", &var.second);
        ImGui::PopID();
    }

    if (!var_to_erase.empty()) {
        m_DelVar(var_to_erase);
        var_to_erase.clear();
        change = true;
    }

    return change;
}

void ChartSystem::m_DrawWidgets(const float vWidth) {
    if (ImGui::CollapsingHeader("Parametric Curve Diff")) {
        bool change = false;

        change |= ImGui::InputDoubleDefault(0.0f, "Start Location x", &m_StartLocation.x, 0.001, "%f");
        change |= ImGui::InputDoubleDefault(0.0f, "Start Location Y", &m_StartLocation.y, 0.001, "%f");
        change |= ImGui::InputDoubleDefault(0.0f, "Start Location z", &m_StartLocation.z, 0.001, "%f");
        change |= ImGui::InputUIntDefault(0.0f, "Step Count", &m_StepCount, 10000U, 1, 10);
        change |= ImGui::InputDoubleDefault(0.0f, "Step Size", &m_StepSize, 0.01, "%f");

        ImGui::Separator();

        change |= m_DrawInputExpr("dx(x,y,z)", "##expr_x", m_ExprX, ChartSystem::s_EXPR_MAX_LEN, m_Err_x, "10.0*(y-x)");
        change |= m_DrawInputExpr("dy(x,y,z)", "##expr_y", m_ExprY, ChartSystem::s_EXPR_MAX_LEN, m_Err_y, "28.0*x-y-x*z");
        change |= m_DrawInputExpr("dz(x,y,z)", "##expr_z", m_ExprZ, ChartSystem::s_EXPR_MAX_LEN, m_Err_z, "x*y-2.66667*z");

        ImGui::Separator();

        change |= ImGui::CheckBoxBoolDefault("Close Curve", &m_CloseCurve, false);

        ImGui::Separator();

        change |= ImGui::ContrastedButton("Eval");

        ImGui::SameLine();

        if (ImGui::ContrastedButton("Center Model")) {
            // CommonSystem::Instance()->SetTargetXYZ(-(ct::fvec3)m_CenterPoint, true);
        }

        change |= m_DrawVars();

        if (change) {
            m_Compute();
        }
    }
}

void ChartSystem::m_DrawChart(const float vWidth) {
    static float constraints[4] = {-10, 10, 1, 20};
    static ImPlotAxisFlags flags;
    ImGui::DragFloat2("Limits Constraints", &constraints[0], 0.01f);
    ImGui::DragFloat2("Zoom Constraints", &constraints[2], 0.01f);
    if (ImPlot::BeginPlot("##AxisConstraints", ImVec2(vWidth, -1.0f))) {
        ImPlot::SetupAxes("X", "Y", flags, flags);
        ImPlot::SetupAxesLimits(-1, 1, -1, 1);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, constraints[0], constraints[1]);
        ImPlot::SetupAxisZoomConstraints(ImAxis_X1, constraints[2], constraints[3]);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, constraints[0], constraints[1]);
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y1, constraints[2], constraints[3]);
        ImPlot::EndPlot();
    }
}

void ChartSystem::m_Compute() {
    /*VerticeArray vertices;
    IndiceArray indices;

    const int verts_len = m_StepCount;
    if (verts_len < 2) {
        LogVarError("Vertex count < 2, no mesh will be generated");
        return;
    }

    vertices.resize(verts_len);
    indices.resize(m_CloseCurve ? verts_len + 1 : verts_len);

    te_expr* expr_x_ptr = te_compile(m_ExprX, m_Vars.data(), (int)m_Vars.size(), &m_Err_x);
    te_expr* expr_y_ptr = te_compile(m_ExprY, m_Vars.data(), (int)m_Vars.size(), &m_Err_y);
    te_expr* expr_z_ptr = te_compile(m_ExprZ, m_Vars.data(), (int)m_Vars.size(), &m_Err_z);

    if (expr_x_ptr && expr_y_ptr && expr_z_ptr) {
        const double step_d = m_StepSize;  // double conv

        auto& x_value = m_VarNameValues.at("x");
        x_value = m_StartLocation.x;

        auto& y_value = m_VarNameValues.at("y");
        y_value = m_StartLocation.y;

        auto& z_value = m_VarNameValues.at("z");
        z_value = m_StartLocation.z;

        m_CenterPoint = 0.0;

        double ratio = 0.0;
        double px, py, pz;
        int verts_index;
        for (verts_index = 0; verts_index < verts_len; ++verts_index) {
            ratio = (double)verts_index / (double)verts_len;

            px = te_eval(expr_x_ptr);
            py = te_eval(expr_y_ptr);
            pz = te_eval(expr_z_ptr);

            x_value += px * step_d;
            y_value += py * step_d;
            z_value += pz * step_d;

            if (!isfinite(abs(x_value)))
                x_value = 0.0;
            if (!isfinite(abs(y_value)))
                y_value = 0.0;
            if (!isfinite(abs(z_value)))
                z_value = 0.0;

            auto& v = vertices.at(verts_index);
            v.p.x = (float)x_value;
            v.p.y = (float)y_value;
            v.p.z = (float)z_value;
            v.c = (float)ratio;
            m_CenterPoint += v.p;
            indices[verts_index] = verts_index;
        }

        m_CenterPoint /= (double)verts_len;

        if (m_CloseCurve) {
            indices[verts_len] = 0;
        }

        auto sceneMeshPtr = SceneMesh<VertexStruct::P3_N3_TA3_BTA3_T2_C4>::Create(m_VulkanCore, vertices, indices);
        sceneMeshPtr->SetPrimitiveType(SceneModelPrimitiveType::SCENE_MODEL_PRIMITIVE_TYPE_CURVES);
        m_SceneModelPtr->clear();
        m_SceneModelPtr->Add(sceneMeshPtr);

        auto parentNodePtr = GetParentNode().lock();
        if (parentNodePtr) {
            parentNodePtr->SendFrontNotification(ModelUpdateDone);
        }
    }

    if (!expr_x_ptr)
        LogVarError("Parse error for params x at %d", m_Err_x);
    if (!expr_y_ptr)
        LogVarError("Parse error for params y at %d", m_Err_y);
    if (!expr_z_ptr)
        LogVarError("Parse error for params z at %d", m_Err_z);

    te_free(expr_x_ptr);
    te_free(expr_y_ptr);
    te_free(expr_z_ptr);*/
}
