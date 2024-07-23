// MenuItem.cpp
#include "MenuItem.h"

MenuItem::MenuItem(const String& name, ActionType actionType, const std::function<void()>& callback)
    : m_name(name), m_actionType(actionType), m_callback(callback) {}

MenuItem::MenuItem(const String& name, ActionType actionType, const String& linkedMenuId)
    : m_name(name), m_actionType(actionType), m_linkedMenuId(linkedMenuId) {}

const String& MenuItem::getName() const {
    return m_name;
}

ActionType MenuItem::getActionType() const {
    return m_actionType;
}

const std::function<void()>& MenuItem::getCallback() const {
    return m_callback;
}

const String& MenuItem::getLinkedMenuId() const {
    return m_linkedMenuId;
}

void MenuItem::execute() const {
    if (m_callback) {
        m_callback();
    }
}

bool MenuItem::hasCallback() const {
    return static_cast<bool>(m_callback);
}

bool MenuItem::hasLinkedMenu() const {
    return !m_linkedMenuId.isEmpty();
}