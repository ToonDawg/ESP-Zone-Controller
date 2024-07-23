// Menu.cpp
#include "Menu.h"

Menu::Menu(const String& title)
    : m_title(title), m_selectedIndex(0), m_parentMenu(nullptr) {}

void Menu::addItem(const MenuItem& item) {
    m_items.push_back(item);
}

void Menu::removeItem(int index) {
    if (index >= 0 && index < m_items.size()) {
        m_items.erase(m_items.begin() + index);
        if (m_selectedIndex >= m_items.size()) {
            m_selectedIndex = m_items.size() - 1;
        }
    }
}

const String& Menu::getTitle() const {
    return m_title;
}

const std::vector<MenuItem>& Menu::getItems() const {
    return m_items;
}

int Menu::getSelectedIndex() const {
    return m_selectedIndex;
}

void Menu::setSelectedIndex(int index) {
    if (index >= 0 && index < m_items.size()) {
        m_selectedIndex = index;
    }
}

Menu* Menu::getParentMenu() const {
    return m_parentMenu;
}

void Menu::setParentMenu(Menu* parent) {
    m_parentMenu = parent;
}

void Menu::navigateUp() {
    if (m_selectedIndex > 0) {
        m_selectedIndex--;
    } else {
        m_selectedIndex = m_items.size() - 1;
    }
}

void Menu::navigateDown() {
    if (m_selectedIndex < m_items.size() - 1) {
        m_selectedIndex++;
    } else {
        m_selectedIndex = 0;
    }
}