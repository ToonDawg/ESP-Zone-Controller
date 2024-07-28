// MenuItem.h
#pragma once

#include <Arduino.h>
#include <functional>

enum class ActionType {
    EXECUTE_CALLBACK,
    OPEN_SUBMENU,
    CHANGE_APP_STATE,
    DISPLAY_VALUE,
    SELECT_OPTION
};

class MenuItem {
public:
    MenuItem(const String& name, ActionType actionType, const std::function<void()>& callback);
    MenuItem(const String& name, ActionType actionType, const String& linkedMenuId);

    const String& getName() const;
    ActionType getActionType() const;
    const std::function<void()>& getCallback() const;
    const String& getLinkedMenuId() const;
    void execute() const;
    bool hasCallback() const;
    bool hasLinkedMenu() const;

private:
    String m_name;
    ActionType m_actionType;
    std::function<void()> m_callback;
    String m_linkedMenuId;
};