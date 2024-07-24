// MenuRouter.cpp
#include "MenuRouter.h"

MenuRouter::MenuRouter() : m_currentMenu(nullptr), m_rootMenu(nullptr) {}

void MenuRouter::createMenu(const String& menuId, const String& title, const String& parentId) {
    Menu* newMenu = new Menu(title);
    m_menuMap[menuId] = newMenu;

    if (parentId.isEmpty()) {
        if (!m_rootMenu) {
            m_rootMenu = newMenu;
            m_currentMenu = newMenu;
        }
    } else {
        Menu* parentMenu = findMenu(parentId);
        if (parentMenu) {
            newMenu->setParentMenu(parentMenu);
        }
    }
}

void MenuRouter::addMenuItem(const String& menuId, const MenuItem& item) {
    Menu* menu = findMenu(menuId);
    if (menu) {
        menu->addItem(item);
    }
}

void MenuRouter::navigateToMenu(const String& menuId) {
    Menu* menu = findMenu(menuId);
    if (menu) {
        m_currentMenu = menu;
        // Trigger any necessary updates or redraws
        updateDisplay();
    }
}

void MenuRouter::navigateUp() {
    if (m_currentMenu) {
        m_currentMenu->navigateUp();
    }
}

void MenuRouter::navigateDown() {
    if (m_currentMenu) {
        m_currentMenu->navigateDown();
    }
}

void MenuRouter::selectCurrentItem() {
    Serial.println("Selecting current item");
    Serial.println(m_currentMenu->getSelectedIndex());
    if (m_currentMenu) {
        int selectedIndex = m_currentMenu->getSelectedIndex();
        const std::vector<MenuItem>& items = m_currentMenu->getItems();
        Serial.println(selectedIndex);
        if (selectedIndex >= 0 && selectedIndex < items.size()) {
            Serial.println("Selected item: " + items[selectedIndex].getName());
            const MenuItem& selectedItem = items[selectedIndex];
            switch (selectedItem.getActionType()) {
                case ActionType::EXECUTE_CALLBACK:
                    selectedItem.execute();
                    break;
                case ActionType::OPEN_SUBMENU:
                    navigateToMenu(selectedItem.getLinkedMenuId());
                    break;
                case ActionType::CHANGE_APP_STATE:
                    // This will be handled in AppStateManager
                    break;
            }
        }
    }
}

Menu* MenuRouter::getCurrentMenu() const {
    return m_currentMenu;
}

bool MenuRouter::isAtRootMenu() const {
    return m_currentMenu == m_rootMenu;
}

Menu* MenuRouter::findMenu(const String& menuId) const {
    auto it = m_menuMap.find(menuId);
    return (it != m_menuMap.end()) ? it->second : nullptr;
}

void MenuRouter::navigateToParentMenu()
{
    if (m_currentMenu && m_currentMenu->getParentMenu())
    {
        m_currentMenu = m_currentMenu->getParentMenu();
    }
    else if (m_currentMenu != m_rootMenu)
    {
        // If there's no parent menu but we're not at the root, go to the root menu
        m_currentMenu = m_rootMenu;
    }
}
void MenuRouter::updateDisplay() {
    // Placeholder for any necessary updates or redraws
    // This should be implemented based on your display and UI update logic
    Serial.println("Updating display for menu: " + m_currentMenu->getTitle());
}
