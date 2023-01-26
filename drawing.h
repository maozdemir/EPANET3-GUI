#pragma warning( disable : 4244 ) 
#include "imgui.h"
#include "implot.h"
#include "structs.h"
#include <string>
#include <vector>
#include <cmath>
#include <time.h>


class Drawing
{
public:
    static void draw_valve();
    static void draw_valve_plotter(ImDrawList* drawList, ImVec2 coords, double angle);

    static void draw_circle();
    static void draw_circle_plotter(ImDrawList* drawList, ImVec2 coords);
    
    static void draw_tank();

    static void drawSquare(ImDrawList* drawList, ImVec2 center, double size, bool is_blinking);
    static void draw_line(ImDrawList* drawList, Pipe line);
};