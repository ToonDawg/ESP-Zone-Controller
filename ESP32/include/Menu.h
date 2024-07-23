// Menu.h
#pragma once

#include <Arduino.h>
#include <vector>
#include "MenuItem.h"

class Menu {
public:
    Menu(const String& title);
    
    void addItem(const MenuItem& item);
    void removeItem(int index);
    const String& getTitle() const;
    const std::vector<MenuItem>& getItems() const;
    int getSelectedIndex() const;
    void setSelectedIndex(int index);
    Menu* getParentMenu() const;
    void setParentMenu(Menu* parent);
    void navigateUp();
    void navigateDown();

private:
    String m_title;
    std::vector<MenuItem> m_items;
    int m_selectedIndex;
    Menu* m_parentMenu;
};