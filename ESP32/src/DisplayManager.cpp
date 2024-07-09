#include "DisplayManager.h"
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <vector>

const GFXfont DisplayManager::smallFont = FreeSans9pt7b;
const GFXfont DisplayManager::largeFont = FreeSansBold24pt7b;

DisplayManager::DisplayManager(Adafruit_SH1106G &display) : display(display) {}

const GFXfont *DisplayManager::getFontForSize(FontSize size) const
{
    return (size == FontSize::LARGE) ? &largeFont : &smallFont;
}

void DisplayManager::drawText(const String &text, int16_t x, int16_t y, const TextStyle &style)
{
    const GFXfont *font = getFontForSize(style.fontSize);
    display.setFont(font);
    display.setTextColor(style.color);

    if (style.alignment == TextAlignment::CENTER)
    {
        x = (display.width() - calculateTextWidth(text, font)) / 2;
    }
    else if (style.alignment == TextAlignment::RIGHT)
    {
        x = display.width() - calculateTextWidth(text, font) - DISPLAY_SIDE_MARGIN;
    }

    display.setCursor(x, y);
    display.print(text);
}

int16_t DisplayManager::calculateTextWidth(const String &text, const GFXfont *font)
{
    int16_t x, y;
    uint16_t w, h;
    display.setFont(font);
    display.getTextBounds(text.c_str(), 0, 0, &x, &y, &w, &h);
    return w;
}

int16_t DisplayManager::calculateTextHeight(const String &text, const GFXfont *font)
{
    int16_t x, y;
    uint16_t w, h;
    display.setFont(font);
    display.getTextBounds(text.c_str(), 0, 0, &x, &y, &w, &h);
    return h;
}

void DisplayManager::draw8BitImage(int16_t x, int16_t y, const tImage &image)
{
    for (int16_t j = 0; j < image.height; j++)
    {
        for (int16_t i = 0; i < image.width; i++)
        {
            if (image.data[j * image.width + i] == 0xff)
            {
                display.drawPixel(x + i, y + j, MONOOLED_WHITE);
            }
            else
            {
                display.drawPixel(x + i, y + j, MONOOLED_BLACK);
            }
        }
    }
}

void DisplayManager::displayTemperature(float temperature)
{
    display.clearDisplay();

    String temperatureString = String(temperature, 1);
    TextStyle tempStyle(FontSize::LARGE, MONOOLED_WHITE, TextAlignment::CENTER);
    drawText(temperatureString, 0, TEMPERATURE_FONT_HEIGHT, tempStyle);

    int16_t unitStartX = display.width() - celciusIcon.width - DISPLAY_SIDE_MARGIN;
    draw8BitImage(unitStartX, DISPLAY_SIDE_MARGIN, celciusIcon);

    display.drawFastHLine(30, TEMPERATURE_FONT_HEIGHT + FONT_VERTICAL_PADDING, display.width() - 60, MONOOLED_WHITE);
}

void DisplayManager::displayIconBottomLeft(const tImage &icon)
{
    draw8BitImage(DISPLAY_SIDE_MARGIN,
                  display.height() - icon.height - DISPLAY_SIDE_MARGIN,
                  icon);
}

void DisplayManager::displayIconBottomRight(const tImage &icon)
{
    draw8BitImage(display.width() - icon.width - DISPLAY_SIDE_MARGIN,
                  display.height() - icon.height - DISPLAY_SIDE_MARGIN,
                  icon);
}

void DisplayManager::displayBottomCenterText(const String &text)
{
    TextStyle style(FontSize::SMALL, MONOOLED_WHITE, TextAlignment::CENTER);
    drawText(text, 0, display.height() - DISPLAY_SIDE_MARGIN, style);
}

void DisplayManager::displayOff()
{
    display.clearDisplay();
}

void DisplayManager::render()
{
    display.display();
}

void DisplayManager::displayCentre(const String lines[], int numLines, const TextStyle &style)
{
    display.clearDisplay();
    int16_t textHeight = calculateTextHeight("W", getFontForSize(style.fontSize));
    int16_t totalHeight = textHeight * numLines;
    int16_t startY = (display.height() - totalHeight) / 2 + textHeight;

    for (int i = 0; i < numLines; i++)
    {
        drawText(lines[i], 0, startY + i * (textHeight + 5), style);
    }
}

void DisplayManager::displaySettingsMenu(const Menu &menu)
{
    display.clearDisplay();

    int16_t titleHeight = displayMenuTitle(menu.getTitle());

    const int16_t itemHeight = 16;
    const int16_t itemSpacing = 2;
    const int16_t textHeight = 12;
    const int16_t verticalOffset = (itemHeight - textHeight) / 2;
    const int maxVisibleItems = 2;

    int selectedIndex = menu.getSelectedIndex();
    int numItems = menu.getNumItems();
    int startIdx = std::max(0, std::min(selectedIndex - 1, numItems - maxVisibleItems));

    int16_t startY = titleHeight + 4;

    // Display menu items
    for (int i = 0; i < maxVisibleItems && (startIdx + i) < numItems; i++)
    {
        int itemIndex = startIdx + i;
        int16_t itemY = startY + i * (itemHeight + itemSpacing);

        bool isSelected = (itemIndex == selectedIndex);
        bool isActive = (itemIndex == menu.getActiveIndex());
        displayMenuItem(menu.getItems()[itemIndex], isSelected, isActive, itemY, verticalOffset, textHeight);
    }

    // Draw scroll indicators
    const int16_t indicatorWidth = 6;
    const int16_t indicatorHeight = 4;
    const int16_t indicatorMargin = 2;

    // Top indicator (upward-pointing triangle)
    if (startIdx > 0)
    {
        display.fillTriangle(
            display.width() - indicatorMargin - indicatorWidth, titleHeight + indicatorHeight + indicatorMargin,
            display.width() - indicatorMargin - (indicatorWidth / 2), titleHeight + indicatorMargin,
            display.width() - indicatorMargin, titleHeight + indicatorHeight + indicatorMargin,
            MONOOLED_WHITE);
    }

    // Bottom indicator (downward-pointing triangle)
    if (startIdx + maxVisibleItems < numItems)
    {
        display.fillTriangle(
            display.width() - indicatorMargin - indicatorWidth, display.height() - indicatorHeight - indicatorMargin,
            display.width() - indicatorMargin - (indicatorWidth / 2), display.height() - indicatorMargin,
            display.width() - indicatorMargin, display.height() - indicatorHeight - indicatorMargin,
            MONOOLED_WHITE);
    }
}

int16_t DisplayManager::displayMenuTitle(const String &title)
{
    TextStyle titleStyle(FontSize::SMALL, MONOOLED_WHITE, TextAlignment::CENTER);
    int16_t titleHeight = calculateTextHeight(title, getFontForSize(titleStyle.fontSize));
    drawText(title, 0, titleHeight + 2, titleStyle);

    int16_t lineY = titleHeight + 4;
    display.drawFastHLine(30, lineY, display.width() - 60, MONOOLED_WHITE);

    return lineY + 2;
}

void DisplayManager::displayMenuItem(const String &item, bool selected, bool active, int16_t y, int16_t verticalOffset, int16_t textHeight)
{
    if (selected)
    {
        display.fillRect(0, y, display.width() - 8, textHeight + 2 * verticalOffset, MONOOLED_WHITE);
        display.setTextColor(MONOOLED_BLACK);
    }
    else
    {
        display.setTextColor(MONOOLED_WHITE);
    }

    display.setCursor(2, y + verticalOffset + textHeight);
    display.print(item);

    if (active)
    {
        display.fillCircle(display.width() - 15, y + (textHeight + 2 * verticalOffset) / 2, 2, selected ? MONOOLED_BLACK : MONOOLED_WHITE);
    }
}