#ifndef VIEWS_H
#define VIEWS_H

#include "imgui.h"
#include "structs.h"


class View
{
public:
    static void main_menu();
    static void options_menu(Options &options);
};

#endif
