// MenuRouter.h
#pragma once

#include <Arduino.h>
#include <map>
#include "Menu.h"

class MenuRouter {
public:
    MenuRouter();

    void createMenu(const String& menuId, const String& title, const String& parentId = "");
    void addMenuItem(const String& menuId, const MenuItem& item);
    void navigateToMenu(const String& menuId);
    void navigateUp();
    void navigateDown();
    void selectCurrentItem();
    Menu* getCurrentMenu() const;
    void navigateToParentMenu();
    
private:
    std::map<String, Menu*> m_menuMap;
    Menu* m_currentMenu;
    Menu* m_rootMenu;

    Menu* findMenu(const String& menuId) const;
};