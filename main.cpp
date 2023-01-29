#pragma warning( disable : 4244 ) 
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include <vector>
#include <algorithm>
#include <thread>

#include "implot.h"
#include "IconsForkAwesome.h"

// #include "main_menu_items.h"
#include "drawing.h"
#include "structs.h"
#include "parseinp.h"
#include "saveinp.h"
#include "views.h"
#include "epanet-ga.h"


#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

struct FrameContext
{
    ID3D12CommandAllocator *CommandAllocator;
    UINT64 FenceValue;
};

// Data
static int const NUM_FRAMES_IN_FLIGHT = 3;
static FrameContext g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
static UINT g_frameIndex = 0;

static int const NUM_BACK_BUFFERS = 3;
static ID3D12Device *g_pd3dDevice = NULL;
static ID3D12DescriptorHeap *g_pd3dRtvDescHeap = NULL;
static ID3D12DescriptorHeap *g_pd3dSrvDescHeap = NULL;
static ID3D12CommandQueue *g_pd3dCommandQueue = NULL;
static ID3D12GraphicsCommandList *g_pd3dCommandList = NULL;
static ID3D12Fence *g_fence = NULL;
static HANDLE g_fenceEvent = NULL;
static UINT64 g_fenceLastSignaledValue = 0;
static IDXGISwapChain3 *g_pSwapChain = NULL;
static HANDLE g_hSwapChainWaitableObject = NULL;
static ID3D12Resource *g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
static D3D12_CPU_DESCRIPTOR_HANDLE g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForLastSubmittedFrame();
FrameContext *WaitForNextFrameResources();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define IMGUI_ENABLE_FREETYPE

// Main code
int main(int, char **)
{
    Project project;
    Parser::parseInputFile("EPA3-hk-large-peaked-intermediate.inp", project);
    printf("NODES SIZE= %d\n", project.nodes.size());
    printf("LINES SIZE= %d\n", project.pipes.size());
    printf("FLOW UNIT ENUM= %d\n", project.options.flow_unit);
    WNDCLASSEXW wc = {sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"EPANET3-TEST", NULL};
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"EPANET3-TEST", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
                        DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
                        g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
                        g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

    io.Fonts->AddFontFromFileTTF("resources/Roboto-Medium.ttf", 16.0f);

    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF("resources/" FONT_ICON_FILE_NAME_FAS, 16.0f, &icons_config, icons_ranges);
    io.Fonts->AddFontFromFileTTF("resources/" FONT_ICON_FILE_NAME_FAR, 16.0f, &icons_config, icons_ranges);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    double DRAWING_X_COORD = 0;
    double DRAWING_Y_COORD = 0;
    int SELECTOR_FIRST_ELEMENT_POSITION = -1;

    BOOL IS_CURSOR_SELECTED = true;
    BOOL IS_PAN_SELECTED = false;
    BOOL IS_ZOOM_SELECTED = false;
    BOOL IS_VALVE_SELECTED = false;
    BOOL IS_ANNOTATION_SELECTED = false;
    BOOL IS_JUNCTION_SELECTED = false;
    BOOL IS_LINE_DRAWING_SELECTED = false;

    double ZOOM_LEVEL = 0.0f;

    std::vector<ImVec2> valveVector;
    std::vector<ImVec2> annotationVector;
    std::vector<ImVec2> junctionVector;
    std::vector<Pipe> lines;
    // Main loop
    bool done = false;
    Saver::saveInputFile("EPA3-hk-large-peaked-intermediate_SAVED.inp", project);
    //GeneticAlgorithm ga;
    //ga.run();

    //GeneticAlgorithm *ga_ptr = &ga;

    //std::thread thread1(&GeneticAlgorithm::run, ga_ptr);
    //thread1.join();
    //thread1.detach();

    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        if (show_demo_window)
        // ImGui::ShowDemoWindow(&show_demo_window);

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("PERF");

            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        View::main_menu();
        View::options_menu(project.options);

        bool unused_open = true;
        if (ImGui::BeginPopupModal("options_menu", &unused_open))
        {
            ImGui::Text("Hello from Stacked The Second!");
            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        {
            ImGuiViewportP *viewport = (ImGuiViewportP *)(void *)ImGui::GetMainViewport();
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
            float height = ImGui::GetFrameHeight();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2({0, 10.0f}));
            {
                if (ImGui::BeginViewportSideBar("##SecondaryMenuBar", viewport, ImGuiDir_Up, height, window_flags))
                {
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 15.0f));
                    if (ImGui::BeginMenuBar())
                    {
                        if (ImGui::MenuItem(ICON_FA_COMPUTER_MOUSE, 0, IS_CURSOR_SELECTED, !IS_CURSOR_SELECTED))
                        {
                            IS_CURSOR_SELECTED = true;
                            IS_PAN_SELECTED = false;
                            IS_ZOOM_SELECTED = false;
                            IS_VALVE_SELECTED = false;
                            IS_JUNCTION_SELECTED = false;
                            IS_ANNOTATION_SELECTED = false;
                            IS_LINE_DRAWING_SELECTED = false;
                        }
                        if (ImGui::MenuItem(ICON_FA_ARROWS_LEFT_RIGHT_TO_LINE, 0, IS_PAN_SELECTED, !IS_PAN_SELECTED))
                        {
                            IS_CURSOR_SELECTED = false;
                            IS_PAN_SELECTED = true;
                            IS_ZOOM_SELECTED = false;
                            IS_VALVE_SELECTED = false;
                            IS_ANNOTATION_SELECTED = false;
                            IS_LINE_DRAWING_SELECTED = false;
                        }
                        if (ImGui::MenuItem(ICON_FA_MAGNIFYING_GLASS, 0, IS_ZOOM_SELECTED, !IS_ZOOM_SELECTED))
                        {
                            IS_CURSOR_SELECTED = false;
                            IS_PAN_SELECTED = false;
                            IS_ZOOM_SELECTED = true;
                            IS_VALVE_SELECTED = false;
                            IS_JUNCTION_SELECTED = false;
                            IS_ANNOTATION_SELECTED = false;
                            IS_LINE_DRAWING_SELECTED = false;
                        }

                        ImGui::PushID("VALVE_DRAW");
                        Drawing::draw_valve();
                        if (ImGui::MenuItem("   ", 0, IS_VALVE_SELECTED, !IS_VALVE_SELECTED))
                        {
                            IS_CURSOR_SELECTED = false;
                            IS_PAN_SELECTED = false;
                            IS_ZOOM_SELECTED = false;
                            IS_VALVE_SELECTED = true;
                            IS_JUNCTION_SELECTED = false;
                            IS_ANNOTATION_SELECTED = false;
                            IS_LINE_DRAWING_SELECTED = false;
                        }
                        ImGui::PopID();

                        ImGui::PushID("JUNCTION_DRAW");
                        Drawing::draw_circle();
                        if (ImGui::MenuItem("   ", 0, IS_JUNCTION_SELECTED, !IS_JUNCTION_SELECTED))
                        {
                            IS_CURSOR_SELECTED = false;
                            IS_PAN_SELECTED = false;
                            IS_ZOOM_SELECTED = false;
                            IS_VALVE_SELECTED = false;
                            IS_JUNCTION_SELECTED = true;
                            IS_ANNOTATION_SELECTED = false;
                            IS_LINE_DRAWING_SELECTED = false;
                        }
                        ImGui::PopID();

                        ImGui::PushID("JUNCTION_DRAW2");
                        Drawing::draw_tank();
                        if (ImGui::MenuItem("   ", 0, IS_JUNCTION_SELECTED, !IS_JUNCTION_SELECTED))
                        {
                            IS_CURSOR_SELECTED = false;
                            IS_PAN_SELECTED = false;
                            IS_ZOOM_SELECTED = false;
                            IS_VALVE_SELECTED = false;
                            IS_JUNCTION_SELECTED = true;
                            IS_ANNOTATION_SELECTED = false;
                            IS_LINE_DRAWING_SELECTED = false;
                        }
                        ImGui::PopID();

                        if (ImGui::MenuItem(ICON_FA_FONT, 0, IS_ANNOTATION_SELECTED, !IS_ANNOTATION_SELECTED))
                        {
                            IS_CURSOR_SELECTED = false;
                            IS_PAN_SELECTED = false;
                            IS_ZOOM_SELECTED = false;
                            IS_VALVE_SELECTED = false;
                            IS_JUNCTION_SELECTED = false;
                            IS_ANNOTATION_SELECTED = true;
                            IS_LINE_DRAWING_SELECTED = false;
                        }
                        if (ImGui::MenuItem(ICON_FA_MARKER, 0, IS_LINE_DRAWING_SELECTED, !IS_LINE_DRAWING_SELECTED))
                        {
                            IS_CURSOR_SELECTED = false;
                            IS_PAN_SELECTED = false;
                            IS_ZOOM_SELECTED = false;
                            IS_VALVE_SELECTED = false;
                            IS_ANNOTATION_SELECTED = false;
                            IS_JUNCTION_SELECTED = false;
                            IS_LINE_DRAWING_SELECTED = true;
                        }

                        ImGui::EndMenuBar();
                    }
                    ImGui::PopStyleVar();
                    ImGui::End();
                }
            }
            ImGui::PopStyleVar();

            // ImPlot::ShowDemoWindow();

            {

                if (ImGui::BeginViewportSideBar("##MainStatusBar", viewport, ImGuiDir_Down, height, window_flags))
                {
                    if (ImGui::BeginMenuBar())
                    {
                        ImGui::Text("AL_");
                        ImGui::Text("GPM_");
                        ImGui::Text("ZOOM: %f", ZOOM_LEVEL);
                        ImGui::Text("X,Y: %f,%f", DRAWING_X_COORD, DRAWING_Y_COORD);
                        ImGui::EndMenuBar();
                    }
                    ImGui::End();
                }
            }
        }
        static int prop_window_selected_node = -1;
        static int prop_window_selected_pipe = -1;
        {
            // ImGui::ShowDemoWindow();
            if (ImGui::Begin("Nodes"))
            {

                // Create the combo box for selecting the node type
                const char *items[] = {"Junctions", "Reservoirs", "Tanks", "Pipes"};
                static const char *current_item = "Junctions";
                if (ImGui::BeginCombo("Node Type##type_sel", current_item))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                    {
                        bool is_selected = (current_item == items[n]);
                        if (ImGui::Selectable(items[n], is_selected))
                            current_item = items[n];
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }

                if (strcmp(current_item, "Junctions") == 0)
                {
                    if (ImGui::BeginListBox("##item_sel", ImVec2(-FLT_MIN, 20 * ImGui::GetTextLineHeightWithSpacing())))
                    {

                        for (int n = 0; std::cmp_less(n, project.nodes.size()); n++)
                        {

                            // if (nodes[n].type == NodeType::JUNCTION)
                            {
                                bool is_selected = (prop_window_selected_node == n);
                                if (ImGui::Selectable(project.nodes[n].id.c_str(), is_selected))
                                    prop_window_selected_node = n;
                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndListBox();
                    }
                }
                if (strcmp(current_item, "Pipes") == 0)
                {
                    if (ImGui::BeginListBox("##item_sel", ImVec2(-FLT_MIN, 20 * ImGui::GetTextLineHeightWithSpacing())))
                    {
                        for (int n = 0; std::cmp_less(n , project.pipes.size()); n++)
                        {
                            if (project.pipes[n].lineType == LineType::PIPE)
                            {
                                bool is_selected = (prop_window_selected_pipe == n);
                                char *label;
                                sprintf_s(label,50, "##pipe-%s", std::to_string(project.pipes[n].id).c_str());
                                if (ImGui::Selectable(label, is_selected))
                                    prop_window_selected_pipe = n;
                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndListBox();
                    }
                }
            }

            ImGui::End();
        }

        {
            if (ImGui::Begin("Node Properties"))
            {
                /*if (prop_window_selected_node >= 0 && prop_window_selected_node < nodes.size() && nodes[prop_window_selected_node].type == NodeType::JUNCTION)
                {
                    ImGui::BeginTable("##node_props", 2);
                    ImGui::TableSetupColumn("Property");
                    ImGui::TableSetupColumn("Value");

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("ID");
                    ImGui::TableNextColumn();
                    //char* id = nodes[prop_window_selected_node].id;
                    /*if (ImGui::InputText("##id", &id))
                        nodes[prop_window_selected_node].id = id;*/

                    /*ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Elevation");
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(nodes[prop_window_selected_node].elevation).c_str());

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Demand");
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(nodes[prop_window_selected_node].demand).c_str());

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Pattern");
                    ImGui::TableNextColumn();
                    ImGui::Text(nodes[prop_window_selected_node].pattern.c_str());

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Type");
                    ImGui::TableNextColumn();
                    ImGui::Text((nodes[prop_window_selected_node].type == NodeType::JUNCTION) ? "Junction" : "Reservoir");

                    ImGui::EndTable();
                }*/
            }
            ImGui::End();
        }

        {
            if (ImGui::Begin("Pipe Properties"))
            {
                if (prop_window_selected_pipe >= 0 && std::cmp_less(prop_window_selected_pipe, project.nodes.size()))
                {
                    ImGui::BeginTable("##pipe_props", 2);
                    ImGui::TableSetupColumn("Property");
                    ImGui::TableSetupColumn("Value");

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("ID");
                    ImGui::TableNextColumn();
                    int id = project.pipes[prop_window_selected_pipe].id;
                    if (ImGui::InputInt("##id", &id))
                        project.pipes[prop_window_selected_pipe].id = id;

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Length");
                    ImGui::TableNextColumn();
                    double length = (double)project.pipes[prop_window_selected_pipe].length;
                    if (ImGui::InputDouble("##len", &length))
                        project.pipes[prop_window_selected_pipe].length = length;

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Diameter");
                    ImGui::TableNextColumn();
                    double diameter = (double)project.pipes[prop_window_selected_pipe].diameter;
                    if (ImGui::InputDouble("##dia", &diameter))
                        project.pipes[prop_window_selected_pipe].diameter = diameter;

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Roughness");
                    ImGui::TableNextColumn();
                    double roughness = (double)project.pipes[prop_window_selected_pipe].roughness;
                    if (ImGui::InputDouble("##roughness", &roughness))
                        project.pipes[prop_window_selected_pipe].roughness = roughness;

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Minor Loss");
                    ImGui::TableNextColumn();
                    double minor_loss = (double)project.pipes[prop_window_selected_pipe].minor_loss;
                    if (ImGui::InputDouble("##min_loss", &minor_loss))
                        project.pipes[prop_window_selected_pipe].minor_loss = minor_loss;

                    ImGui::EndTable();
                }
            }
            ImGui::End();
        }

        {
            if (ImGui::Begin("Network Map"))
            {
                static float mk_size = 5.0f;
                static float mk_weight = 2.0f;

                if (ImPlot::BeginPlot("##MarkerStyles", ImVec2(-1, -1), ImPlotFlags_CanvasOnly))
                {

                    ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
                    ImPlot::SetupAxesLimits(0, 15, 0, 10);

                    if (ImPlot::IsPlotHovered())
                    {
                        ImPlotPoint pt = ImPlot::GetPlotMousePos();
                        DRAWING_Y_COORD = pt.y;
                        DRAWING_X_COORD = pt.x;

                        /*if (IS_VALVE_SELECTED && ImGui::IsMouseClicked(0))
                        {
                            long double JUNCTION_RADIUS = 7.5;
                            if (ImGui::IsMouseClicked(0))
                            {
                                for (int i = 0; i < nodes.size(); i++)
                                {
                                    ImVec2 node_coords = ImVec2(nodes[i].x_coord, nodes[i].y_coord);
                                    ImVec2 node_pixels = ImPlot::PlotToPixels(node_coords);
                                    int junction_circle_radius = 10;
                                    ImVec2 mouse_position = ImGui::GetMousePos();
                                    long double x_coord = mouse_position.x;
                                    long double y_coord = mouse_position.y;
                                    ;
                                    float distance = sqrt((x_coord - node_pixels.x) * (x_coord - node_pixels.x) + (y_coord - node_pixels.y) * (y_coord - node_pixels.y));
                                    if (distance < junction_circle_radius)
                                    {
                                        if (SELECTOR_FIRST_ELEMENT_POSITION == -1)
                                        {
                                            SELECTOR_FIRST_ELEMENT_POSITION = i;
                                            break;
                                        }
                                        else
                                        {
                                            pipe_vector.push_back(Pipe(&nodes[SELECTOR_FIRST_ELEMENT_POSITION], &nodes[i], (int)(lines.size() - 1), LineType::VALVE, 0, 0, 0, 0));
                                            SELECTOR_FIRST_ELEMENT_POSITION = -1;
                                            break;
                                        }
                                        break;
                                    }
                                }
                            }
                        }*/

                        if (IS_JUNCTION_SELECTED && ImGui::IsMouseClicked(0))
                        {
                            junctionVector.push_back(ImVec2(DRAWING_X_COORD, DRAWING_Y_COORD));
                        }

                        if (IS_ANNOTATION_SELECTED)
                        {
                            ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
                            if (ImGui::IsMouseClicked(0))
                            {
                                annotationVector.push_back(ImVec2(DRAWING_X_COORD, DRAWING_Y_COORD));
                            }
                        }
                        if (IS_LINE_DRAWING_SELECTED)
                        {
                            long double JUNCTION_RADIUS = 7.5;
                            if (ImGui::IsMouseClicked(0))
                            {
                                for (int i = 0; std::cmp_less(i,project.nodes.size()); i++)
                                {
                                    ImVec2 node_coords = ImVec2(project.nodes[i].x_coord, project.nodes[i].y_coord);
                                    ImVec2 node_pixels = ImPlot::PlotToPixels(node_coords);
                                    int junction_circle_radius = 10;
                                    ImVec2 mouse_position = ImGui::GetMousePos();
                                    long double x_coord = mouse_position.x;
                                    long double y_coord = mouse_position.y;

                                    float distance = sqrt((x_coord - node_pixels.x) * (x_coord - node_pixels.x) + (y_coord - node_pixels.y) * (y_coord - node_pixels.y));
                                    if (distance < junction_circle_radius)
                                    {
                                        if (SELECTOR_FIRST_ELEMENT_POSITION == -1)
                                        {
                                            SELECTOR_FIRST_ELEMENT_POSITION = i;
                                            break;
                                        }
                                        else
                                        {

                                            project.pipes.push_back(Pipe(&project.nodes[SELECTOR_FIRST_ELEMENT_POSITION], &project.nodes[i], (int)(lines.size() - 1), LineType::PIPE, 0, 0, 0, 0, PipeStatus::OPEN));

                                            break;
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    for (int i = 0; std::cmp_less(i, junctionVector.size()); i++)
                    {
                        Drawing::draw_circle_plotter(ImPlot::GetPlotDrawList(), ImPlot::PlotToPixels(ImPlotPoint(ImVec2(junctionVector[i].x, junctionVector[i].y))));
                    }
                    for (int i = 0; std::cmp_less(i , project.nodes.size()); i++)
                    {
                        if (project.nodes[i].type == NodeType::JUNCTION)
                        {
                            ImPlot::PlotScatter("Junctions", new double[1]{(double)project.nodes[i].x_coord}, new double[1]{(double)project.nodes[i].y_coord}, 1);
                            Drawing::draw_circle_plotter(ImPlot::GetPlotDrawList(), ImPlot::PlotToPixels(ImPlotPoint(ImVec2(project.nodes[i].x_coord, project.nodes[i].y_coord))));
                        }
                    }

                    for (int i = 0; std::cmp_less(i,project.pipes.size()); i++)
                    {
                        Drawing::draw_line(ImPlot::GetPlotDrawList(), project.pipes[i]);
                    }

                    Drawing::drawSquare(ImPlot::GetPlotDrawList(), ImPlot::PlotToPixels(ImPlotPoint(ImVec2(0, 0))), 50, true);

                    ImVec2 plot_size = ImPlot::GetPlotSize();
                    ImPlotRect plotLimits = ImPlot::GetPlotLimits();
                    long double diff_y = abs(plotLimits.Min().y - plotLimits.Max().y);
                    long double diff_x = abs(plotLimits.Min().x - plotLimits.Max().x);
                    ZOOM_LEVEL = round(max(plot_size.x / diff_x, plot_size.y / diff_y));

                    for (int i = 0; std::cmp_less(i,annotationVector.size()); i++)
                    {
                        ImPlot::PlotText("TEXT", annotationVector[i].x, annotationVector[i].y);
                    }

                    ImPlot::EndPlot();
                }
            }

            ImGui::End();
        }

        ImGui::Render();

        {
            FrameContext *frameCtx = WaitForNextFrameResources();
            UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
            frameCtx->CommandAllocator->Reset();

            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
            g_pd3dCommandList->Reset(frameCtx->CommandAllocator, NULL);
            g_pd3dCommandList->ResourceBarrier(1, &barrier);

            const float clear_color_with_alpha[4] = {clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w};
            g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, NULL);
            g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
            g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            g_pd3dCommandList->ResourceBarrier(1, &barrier);
            g_pd3dCommandList->Close();

            g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList *const *)&g_pd3dCommandList);

            g_pSwapChain->Present(1, 0);

            UINT64 fenceValue = g_fenceLastSignaledValue + 1;
            g_pd3dCommandQueue->Signal(g_fence, fenceValue);
            g_fenceLastSignaledValue = fenceValue;
            frameCtx->FenceValue = fenceValue;
        }
    }

    WaitForLastSubmittedFrame();

    // Cleanup
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC1 sd;
    {
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = NUM_BACK_BUFFERS;
        sd.Width = 0;
        sd.Height = 0;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.Stereo = FALSE;
    }

    // [DEBUG] Enable debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
    ID3D12Debug *pdx12Debug = NULL;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
        pdx12Debug->EnableDebugLayer();
#endif

    // Create device
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
        return false;

        // [DEBUG] Setup debug interface to break on any warnings/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
    if (pdx12Debug != NULL)
    {
        ID3D12InfoQueue *pInfoQueue = NULL;
        g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        pInfoQueue->Release();
        pdx12Debug->Release();
    }
#endif

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = NUM_BACK_BUFFERS;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;
        if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
            return false;

        SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        {
            g_mainRenderTargetDescriptor[i] = rtvHandle;
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
            return false;
    }

    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 1;
        if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
            return false;
    }

    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
            return false;

    if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
        g_pd3dCommandList->Close() != S_OK)
        return false;

    if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
        return false;

    g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (g_fenceEvent == NULL)
        return false;

    {
        IDXGIFactory4 *dxgiFactory = NULL;
        IDXGISwapChain1 *swapChain1 = NULL;
        if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
            return false;
        if (dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK)
            return false;
        if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
            return false;
        swapChain1->Release();
        dxgiFactory->Release();
        g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
        g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
    }

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain)
    {
        g_pSwapChain->SetFullscreenState(false, NULL);
        g_pSwapChain->Release();
        g_pSwapChain = NULL;
    }
    if (g_hSwapChainWaitableObject != NULL)
    {
        CloseHandle(g_hSwapChainWaitableObject);
    }
    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (g_frameContext[i].CommandAllocator)
        {
            g_frameContext[i].CommandAllocator->Release();
            g_frameContext[i].CommandAllocator = NULL;
        }
    if (g_pd3dCommandQueue)
    {
        g_pd3dCommandQueue->Release();
        g_pd3dCommandQueue = NULL;
    }
    if (g_pd3dCommandList)
    {
        g_pd3dCommandList->Release();
        g_pd3dCommandList = NULL;
    }
    if (g_pd3dRtvDescHeap)
    {
        g_pd3dRtvDescHeap->Release();
        g_pd3dRtvDescHeap = NULL;
    }
    if (g_pd3dSrvDescHeap)
    {
        g_pd3dSrvDescHeap->Release();
        g_pd3dSrvDescHeap = NULL;
    }
    if (g_fence)
    {
        g_fence->Release();
        g_fence = NULL;
    }
    if (g_fenceEvent)
    {
        CloseHandle(g_fenceEvent);
        g_fenceEvent = NULL;
    }
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = NULL;
    }

#ifdef DX12_ENABLE_DEBUG_LAYER
    IDXGIDebug1 *pDebug = NULL;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
    {
        pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
        pDebug->Release();
    }
#endif
}

void CreateRenderTarget()
{
    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
    {
        ID3D12Resource *pBackBuffer = NULL;
        g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, g_mainRenderTargetDescriptor[i]);
        g_mainRenderTargetResource[i] = pBackBuffer;
    }
}

void CleanupRenderTarget()
{
    WaitForLastSubmittedFrame();

    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        if (g_mainRenderTargetResource[i])
        {
            g_mainRenderTargetResource[i]->Release();
            g_mainRenderTargetResource[i] = NULL;
        }
}

void WaitForLastSubmittedFrame()
{
    FrameContext *frameCtx = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue == 0)
        return; // No fence was signaled

    frameCtx->FenceValue = 0;
    if (g_fence->GetCompletedValue() >= fenceValue)
        return;

    g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
    WaitForSingleObject(g_fenceEvent, INFINITE);
}

FrameContext *WaitForNextFrameResources()
{
    UINT nextFrameIndex = g_frameIndex + 1;
    g_frameIndex = nextFrameIndex;

    HANDLE waitableObjects[] = {g_hSwapChainWaitableObject, NULL};
    DWORD numWaitableObjects = 1;

    FrameContext *frameCtx = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue != 0) // means no fence was signaled
    {
        frameCtx->FenceValue = 0;
        g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
        waitableObjects[1] = g_fenceEvent;
        numWaitableObjects = 2;
    }

    WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

    return frameCtx;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            WaitForLastSubmittedFrame();
            CleanupRenderTarget();
            HRESULT result = g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
            assert(SUCCEEDED(result) && "Failed to resize swapchain.");
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}