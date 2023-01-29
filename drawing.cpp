#include "drawing.h"
#define M_PI 3.14159265358979323846

void Drawing::draw_valve()
{
    ImDrawList *pDrawList = ImGui::GetWindowDrawList();
    const ImVec2 p = ImGui::GetCursorScreenPos();
    float x = p.x + 3.0f;
    float y = p.y + 10.0f;
    static float sz = 15.0f;
    static ImVec4 colf = ImVec4(0, 0, 0, 3.0f);
    const ImU32 col = ImColor(colf);
    float th = 2.0f;
    pDrawList->AddTriangle(ImVec2(x, y), ImVec2(x + sz * 0.5f, y + sz * 0.5f), ImVec2(x, y + sz - 0.5f), col, th); // ust,sag,alt
    x = x + sz * 0.5f;
    pDrawList->AddTriangle(ImVec2(x + sz * 0.5f, y), ImVec2(x, y + sz * 0.5f), ImVec2(x + sz * 0.5f, y + sz - 0.5f), col, th); // ust,sol,alt
}

void Drawing::drawSquare(ImDrawList *drawList, ImVec2 center, double size, bool is_blinking)
{
    static ImVec4 colf = ImVec4(0, 0, 0, 3.0f);
    static clock_t last_time = 0;
    clock_t current_time = clock();
    if (is_blinking)
    {
        double time_elapsed = (current_time - last_time) / (double)CLOCKS_PER_SEC;
        if (time_elapsed >= 0.25)
        {
            colf.x = 1 - colf.x;
            colf.y = 1 - colf.y;
            colf.z = 1 - colf.z;
            last_time = current_time;
        }
    }
    float thickness = 2;
    ImVec2 topLeft = ImVec2(center.x - size / 2, center.y - size / 2);
    ImVec2 midLeft = ImVec2(center.x - size / 2, center.y);
    ImVec2 topRight = ImVec2(center.x + size / 2, center.y - size / 2);
    ImVec2 midRight = ImVec2(center.x + size / 2, center.y);
    ImVec2 bottomRight = ImVec2(center.x + thickness / 2 + size / 2, center.y + size / 2);

    ImU32 color = ImColor(colf);

    ImPlot::PushPlotClipRect();
    drawList->AddLine(topLeft, midLeft, color, thickness);
    drawList->AddLine(topRight, midRight, color, thickness);
    drawList->AddRect(midLeft, bottomRight, color, 0, 0, thickness);
    drawList->AddRectFilled(midLeft, bottomRight, color);
    ImPlot::PopPlotClipRect();
}

void Drawing::draw_valve_plotter(ImDrawList *drawList, ImVec2 coords, double angle)
{
    angle = M_PI - angle;
    static float sz = 15.0f;
    static ImVec4 colf = ImVec4(0, 0, 0, 255.0f);
    const ImU32 col = ImColor(colf);
    float th = 2.0f;
    ImVec2 valveCenter = coords;
    ImVec2 rotation = ImVec2(std::cos(angle), std::sin(angle));
    ImVec2 leftTriangleTop = ImVec2(valveCenter.x + (rotation.x * (-sz * 0.5f)) - (rotation.y * (-sz * 0.5f)), valveCenter.y + (rotation.y * (-sz * 0.5f)) + (rotation.x * (-sz * 0.5f)));
    ImVec2 leftTriangleBottom = ImVec2(valveCenter.x + (rotation.x * (-sz * 0.5f)) - (rotation.y * (sz * 0.5f)), valveCenter.y + (rotation.y * (-sz * 0.5f)) + (rotation.x * (sz * 0.5f)));
    ImVec2 rightTriangleTop = ImVec2(valveCenter.x + (rotation.x * (sz * 0.5f)) - (rotation.y * (-sz * 0.5f)), valveCenter.y + (rotation.y * (sz * 0.5f)) + (rotation.x * (-sz * 0.5f)));
    ImVec2 rightTriangleBottom = ImVec2(valveCenter.x + (rotation.x * (sz * 0.5f)) - (rotation.y * (sz * 0.5f)), valveCenter.y + (rotation.y * (sz * 0.5f)) + (rotation.x * (sz * 0.5f)));

    ImPlot::PushPlotClipRect();
    drawList->AddTriangle(valveCenter, leftTriangleTop, leftTriangleBottom, col);
    drawList->AddTriangle(valveCenter, rightTriangleTop, rightTriangleBottom, col);
    ImPlot::PopPlotClipRect();
}

void Drawing::draw_circle()
{

    ImDrawList *pDrawList = ImGui::GetWindowDrawList();
    const ImVec2 p = ImGui::GetCursorScreenPos();
    static float sz = 15.0f;

    float x = p.x + 5.0f + ImGui::CalcTextSize("   ").x / 2;
    float y = p.y + 10.0f + ImGui::CalcTextSize("   ").y / 2;
    static ImVec4 colf = ImVec4(0.0f, 0.0f, 0.0f, 255.0f);
    float th = 2.0f;
    ImVec2 circleCenter = ImVec2(x, y);
    pDrawList->AddCircle(circleCenter, sz / 2, ImColor(colf), 0, th);
}

void Drawing::draw_circle_plotter(ImDrawList *drawList, ImVec2 coords)
{

    static float sz = 15.0f;
    static ImVec4 colf = ImVec4(0.0f, 0.0f, 0.0f, 255.0f);
    float th = 1.0f;
    ImVec2 circleCenter = coords;
    ImPlot::PushPlotClipRect();
    drawList->AddCircle(circleCenter, sz / 2, ImColor(colf), 0, th);
    ImPlot::PopPlotClipRect();
}

void Drawing::draw_tank()
{

    ImDrawList *pDrawList = ImGui::GetWindowDrawList();
    const ImVec2 p = ImGui::GetCursorScreenPos();
    static float sz = 15.0f;

    float x = p.x + 5.0f + ImGui::CalcTextSize("   ").x / 2;
    float y = p.y + 5.0f + ImGui::CalcTextSize("   ").y / 2;
    static ImVec4 colf = ImVec4(0.0f, 0.0f, 0.0f, 255.0f);
    float th = 1.0f;
    ImVec2 circleCenter = ImVec2(x, y);
    pDrawList->AddRect(ImVec2(x, y), ImVec2(x + 14, y + 8), ImColor(colf), 1.0f, 0, th);
    pDrawList->AddRect(ImVec2(x + 2, y + 7), ImVec2(x + 2 + 10, y + 7 + 7), ImColor(colf), 0, 0, th);
}

void Drawing::draw_line(ImDrawList *drawList, Pipe line)
{
    std::string id_str = std::to_string(line.id);
    const char *id_cstr = id_str.c_str();
    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 1.0f);
    static ImVec4 colf = ImVec4(0.0f, 0.0f, 0.0f, 255.0f);
    ImPlot::PushStyleColor(ImPlotCol_Line, colf);
    std::vector<double> x{(double)line.start->x_coord, (double)line.end->x_coord};
    std::vector<double> y{(double)line.start->y_coord, (double)line.end->y_coord};

    switch (line.lineType)
    {
    case LineType::PUMP:
        // Draw the pump symbol at the center point of the line
        ImPlot::PlotLine("Line 2", x.data(), y.data(), x.size());
        break;
    case LineType::PIPE:
        // Draw the pipe symbol at the center point of the line
        ImPlot::PlotLine("Line 2", x.data(), y.data(), x.size());
        break;
    case LineType::VALVE:
        // Draw the rotated valve symbol parallel to the line
        draw_valve_plotter(drawList, ImPlot::PlotToPixels(ImPlotPoint(line.getCenterPoint())), line.getAngle());
        ImPlot::PlotLine("Line 2", x.data(), y.data(), x.size());
        break;
    }

    ImPlot::PopStyleVar();
    ImPlot::PopStyleColor();
}
