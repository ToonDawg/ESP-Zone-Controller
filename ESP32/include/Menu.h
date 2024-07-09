// Menu.h
#pragma once
#include <Arduino.h>
#include <vector>

class Menu {
public:
    Menu(const String& title, const std::vector<String>& items)
        : title(title), items(items), selectedIndex(0), activeIndex(-1) {}

    const String& getTitle() const { return title; }
    const std::vector<String>& getItems() const { return items; }
    int getSelectedIndex() const { return selectedIndex; }
    void setSelectedIndex(int index) { selectedIndex = index; }
    int getNumItems() const { return items.size(); }
    void setActiveIndex(int index) { activeIndex = index; }
    int getActiveIndex() const { return activeIndex; }
    void navigateUp();
    void navigateDown();

private:
    String title;
    std::vector<String> items;
    int selectedIndex;
    int activeIndex;
};