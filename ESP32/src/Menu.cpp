// Menu.cpp
#include "Menu.h"

void Menu::navigateUp() {
    if (!items.empty()) {
        selectedIndex = (selectedIndex - 1 + items.size()) % items.size();
    }
}

void Menu::navigateDown() {
    if (!items.empty()) {
        selectedIndex = (selectedIndex + 1) % items.size();
    }
}

void Menu::addItem(const String &item) {
    items.push_back(item);
}