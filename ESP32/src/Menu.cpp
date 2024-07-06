#include "Menu.h"

Menu::Menu(const String& title, const std::vector<String>& items)
    : title(title), items(items), selectedIndex(0) {}

void Menu::navigateUp() {
    selectedIndex = (selectedIndex - 1 + items.size()) % items.size();
}

void Menu::navigateDown() {
    selectedIndex = (selectedIndex + 1) % items.size();
}

int Menu::getSelectedIndex() const {
    return selectedIndex;
}

const String& Menu::getTitle() const {
    return title;
}

const std::vector<String>& Menu::getItems() const {
    return items;
}