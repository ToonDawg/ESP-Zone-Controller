// Menu.h
#pragma once
#include <Arduino.h>
#include <vector>

class Menu {
public:
    Menu(const String& title, const std::vector<String>& items);
    void navigateUp();
    void navigateDown();
    int getSelectedIndex() const;
    const String& getTitle() const;
    const std::vector<String>& getItems() const;

private:
    String title;
    std::vector<String> items;
    int selectedIndex;
};