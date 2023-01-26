#include "views.h"

void View::main_menu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("ASD"))
        {
            if (ImGui::MenuItem("Options"))
            {
                ImGui::OpenPopup("options_menu");
            }
            ImGui::EndMenu();
        }
        /*MainMenuItems::fileMenu();
        MainMenuItems::editMenu();
        MainMenuItems::viewMenu();
        MainMenuItems::projectMenu();
        MainMenuItems::reportMenu();
        MainMenuItems::windowMenu();
        MainMenuItems::helpMenu();*/
        ImGui::EndMainMenuBar();
    }
}

void View::options_menu(Options &options)
{

    bool unused_open = true;
    if (ImGui::BeginPopupModal("options_menu", &unused_open))
    {
        ImGui::Text("Hello from Stacked The Second!");
        printf("Option menu triggered");
        if (ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}
