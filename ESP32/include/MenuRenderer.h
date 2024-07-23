// MenuRenderer.h
#pragma once

#include "DisplayManager.h"
#include "Menu.h"

class MenuRenderer {
public:
    MenuRenderer(DisplayManager& displayManager);
    void renderMenu(const Menu& menu);

private:
    DisplayManager& m_displayManager;
    void renderMenuItem(const MenuItem& item, bool selected, bool active, int16_t y, int16_t verticalOffset, int16_t textHeight);
};