// MenuRenderer.cpp
#include "MenuRenderer.h"

MenuRenderer::MenuRenderer(DisplayManager& displayManager)
    : m_displayManager(displayManager) {}

void MenuRenderer::renderMenu(const Menu& menu) {
    m_displayManager.displayMenuTitle(menu.getTitle());

    const int16_t itemHeight = 16;
    const int16_t titleHeight = 16;
    const int16_t itemSpacing = 4;
    const int16_t textHeight = 12;
    const int16_t verticalOffset = (itemHeight - textHeight) / 2;
    const int maxVisibleItems = 2;

    int selectedIndex = menu.getSelectedIndex();
    int numItems = menu.getItems().size();
    int startIdx = std::max(0, std::min(selectedIndex - 1, numItems - maxVisibleItems));

    int16_t startY = titleHeight;

    // Display menu items
    for (int i = 0; i < maxVisibleItems && (startIdx + i) < numItems; i++) {
        int itemIndex = startIdx + i;
        int16_t itemY = startY + i * (itemHeight + itemSpacing);

        renderMenuItem(menu.getItems()[itemIndex], itemIndex == selectedIndex, false, itemY, verticalOffset, textHeight);
    }

    // Draw scroll indicators
    if (startIdx > 0) {
        m_displayManager.drawScrollIndicator(true);
    }
    if (startIdx + maxVisibleItems < numItems) {
        m_displayManager.drawScrollIndicator(false);
    }
}

void MenuRenderer::renderMenuItem(const MenuItem& item, bool selected, bool active, int16_t y, int16_t verticalOffset, int16_t textHeight) {
    m_displayManager.displayMenuItem(item.getName(), selected, active, y, verticalOffset, textHeight);
}