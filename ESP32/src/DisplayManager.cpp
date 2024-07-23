#include "DisplayManager.h"
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

const GFXfont DisplayManager::mediumFont = FreeSans9pt7b;
const GFXfont DisplayManager::largeFont = FreeSansBold24pt7b;

DisplayManager::DisplayManager(Adafruit_SH1106G &display)
    : display(display),
      loader(display, display.width() / 2, display.height() * 0.25, 10, 3, MONOOLED_WHITE)
{
}

void DisplayManager::drawText(const String &text, int16_t x, int16_t y, const TextStyle &style)
{
    setFont(style.fontSize);
    display.setTextColor(style.color);

    if (style.alignment != TextAlignment::LEFT)
    {
        int16_t textWidth = calculateTextWidth(text);
        if (style.alignment == TextAlignment::CENTER)
        {
            x = (display.width() - textWidth) / 2;
        }
        else
        { // RIGHT
            x = display.width() - textWidth - DISPLAY_SIDE_MARGIN;
        }
    }

    display.setCursor(x, y);
    display.print(text);
}

void DisplayManager::setFont(FontSize size)
{
    switch (size)
    {
    case FontSize::MEDIUM:
        display.setFont(&mediumFont);
        break;
    case FontSize::LARGE:
        display.setFont(&largeFont);
        break;
    default:
        display.setFont();
    }
}

int16_t DisplayManager::calculateTextWidth(const String &text)
{
    int16_t x, y;
    uint16_t w, h;
    display.getTextBounds(text.c_str(), 0, 0, &x, &y, &w, &h);
    return w;
}

int16_t DisplayManager::calculateTextHeight(const String &text)
{
    int16_t x, y;
    uint16_t w, h;
    display.getTextBounds(text.c_str(), 0, 0, &x, &y, &w, &h);
    return h;
}

void DisplayManager::draw8BitImage(int16_t x, int16_t y, const tImage &image)
{
    for (int16_t j = 0; j < image.height; j++)
    {
        for (int16_t i = 0; i < image.width; i++)
        {
            display.drawPixel(x + i, y + j, image.data[j * image.width + i] == 0xff ? MONOOLED_WHITE : MONOOLED_BLACK);
        }
    }
}

String formatTemperature(float temperature)
{
    if (temperature >= 100 || temperature <= -10)
    {
        return String(round(temperature));
    }
    else
    {
        return String(temperature, 1);
    }
}

void DisplayManager::displayTemperature(float temperature, tImage tempIcon)
{
    display.clearDisplay();

    TextStyle tempStyle(FontSize::LARGE, MONOOLED_WHITE, TextAlignment::CENTER);
    drawText(formatTemperature(temperature), 0, TEMPERATURE_FONT_HEIGHT, tempStyle);

    draw8BitImage(display.width() - tempIcon.width - DISPLAY_SIDE_MARGIN, DISPLAY_SIDE_MARGIN, tempIcon);

    display.drawFastHLine(30, TEMPERATURE_FONT_HEIGHT + FONT_VERTICAL_PADDING, display.width() - 60, MONOOLED_WHITE);
}

void DisplayManager::displayIconBottomLeft(const tImage &icon)
{
    draw8BitImage(DISPLAY_SIDE_MARGIN, display.height() - icon.height - DISPLAY_SIDE_MARGIN, icon);
}

void DisplayManager::displayIconBottomRight(const tImage &icon)
{
    draw8BitImage(display.width() - icon.width - DISPLAY_SIDE_MARGIN, display.height() - icon.height - DISPLAY_SIDE_MARGIN, icon);
}

void DisplayManager::displayBottomCenterText(const String &text)
{
    TextStyle style(FontSize::MEDIUM, MONOOLED_WHITE, TextAlignment::CENTER);
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

void DisplayManager::displayCenteredWrappedText(const String &text)
{
    TextStyle style(FontSize::MEDIUM, MONOOLED_WHITE, TextAlignment::CENTER);

    setFont(style.fontSize);
    display.setTextColor(style.color);

    int16_t lineHeight = calculateTextHeight("Tg") + 2; // Add a small line spacing
    int16_t maxWidth = display.width() - 2 * DISPLAY_SIDE_MARGIN;

    std::vector<String> lines;
    String currentLine = "";
    int16_t currentLineWidth = 0;

    // Split text into words
    int start = 0, end = text.indexOf(' ');
    while (start < text.length())
    {
        if (end == -1)
            end = text.length();
        String word = text.substring(start, end);
        int16_t wordWidth = calculateTextWidth(word + " ");

        if (currentLineWidth + wordWidth > maxWidth)
        {
            if (!currentLine.isEmpty())
            {
                lines.push_back(currentLine);
                currentLine = "";
                currentLineWidth = 0;
            }
            // If a single word is longer than maxWidth, split it
            while (wordWidth > maxWidth)
            {
                int splitIndex = 1;
                while (calculateTextWidth(word.substring(0, splitIndex) + "-") <= maxWidth)
                {
                    splitIndex++;
                }
                splitIndex--;
                lines.push_back(word.substring(0, splitIndex) + "-");
                word = word.substring(splitIndex);
                wordWidth = calculateTextWidth(word + " ");
            }
        }

        currentLine += word + " ";
        currentLineWidth += wordWidth;

        start = end + 1;
        end = text.indexOf(' ', start);
    }

    if (!currentLine.isEmpty())
    {
        lines.push_back(currentLine);
    }

    // Calculate starting Y position
    int16_t totalTextHeight = lines.size() * lineHeight;
    int16_t startY = 16 + (display.height() - totalTextHeight) / 2;

    // Draw each line
    for (const String &line : lines)
    {
        int16_t lineWidth = calculateTextWidth(line);
        int16_t x = (display.width() - lineWidth) / 2;
        drawText(line, x, startY, style);
        startY += lineHeight;
    }
}

void DisplayManager::displaySettingsMenu(const Menu &menu)
{
    display.clearDisplay();

    displayMenuTitle(menu.getTitle());

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
    for (int i = 0; i < maxVisibleItems && (startIdx + i) < numItems; i++)
    {
        int itemIndex = startIdx + i;
        int16_t itemY = startY + i * (itemHeight + itemSpacing);
        displayMenuItem(menu.getItems()[itemIndex].getName(), itemIndex == selectedIndex, itemIndex == menu.getSelectedIndex(), itemY, verticalOffset, textHeight);
    }

    // Draw scroll indicators
    if (startIdx > 0)
    {
        display.fillTriangle(display.width() - 8, titleHeight, display.width() - 5, titleHeight - 4, display.width() - 2, titleHeight, MONOOLED_WHITE);
    }
    if (startIdx + maxVisibleItems < numItems)
    {
        display.fillTriangle(display.width() - 8, display.height() - 6, display.width() - 5, display.height() - 2, display.width() - 2, display.height() - 6, MONOOLED_WHITE);
    }
}

void DisplayManager::displayMenuTitle(const String title)
{
    TextStyle titleStyle(FontSize::SMALL, MONOOLED_WHITE, TextAlignment::CENTER);
    int16_t titleHeight = 16;
    drawText(title, 0, 2, titleStyle);

    display.drawFastHLine(30, titleHeight - 5, display.width() - 60, MONOOLED_WHITE);
}

String DisplayManager::cutoffText(const String &text, int16_t maxWidth)
{
    if (calculateTextWidth(text) <= maxWidth)
        return text;

    String cutoff;
    for (char c : text)
    {
        if (calculateTextWidth(cutoff + c) > maxWidth)
            break;
        cutoff += c;
    }
    return cutoff;
}

void DisplayManager::displayMenuItem(const String &item, bool selected, bool active, int16_t y, int16_t verticalOffset, int16_t textHeight)
{
    int16_t maxWidth = display.width() - 10;

    setFont(FontSize::MEDIUM);

    int16_t totalItemHeight = textHeight + 4 * verticalOffset;

    int16_t textY = y + verticalOffset + textHeight;

    if (selected)
    {
        // Highlight the entire menu item area
        display.fillRect(0, y, display.width() - 8, totalItemHeight, MONOOLED_WHITE);
        display.setTextColor(MONOOLED_BLACK);

        int16_t itemWidth = calculateTextWidth(item);
        if (itemWidth > maxWidth)
        {
            if (item != scroll.currentItem)
            {
                scroll.reset(item);
            }
            unsigned long currentTime = millis();
            if (currentTime - scroll.startTime > ScrollInfo::DELAY)
            {
                scroll.offset = ((currentTime - scroll.startTime - ScrollInfo::DELAY) / ScrollInfo::SPEED) % (itemWidth + maxWidth);
            }
            display.setCursor(2 - scroll.offset, textY);
            display.print(item + " " + item);
        }
        else
        {
            display.setCursor(2, textY);
            display.print(item);
        }
    }
    else
    {
        display.setTextColor(MONOOLED_WHITE);
        display.setCursor(2, textY);
        display.print(cutoffText(item, maxWidth));
    }

    if (active)
    {
        display.fillCircle(display.width() - 15, y + totalItemHeight / 2, 2, selected ? MONOOLED_BLACK : MONOOLED_WHITE);
    }
}

void DisplayManager::showLoader()
{
    loader.update();
    loader.draw();
}

void DisplayManager::showLoaderWithText(const String &text)
{

    showLoader();
    TextStyle style(FontSize::MEDIUM, MONOOLED_WHITE, TextAlignment::CENTER);
    drawText(text, 0, display.height() * 0.75, style);
}

void DisplayManager::displayLabelAndContent(const String &label, const String &content, int16_t y)
{
    const int16_t LABEL_MARGIN = 2; // Space between label and content
    const int16_t CONTENT_X = 5;    // X position for content (slight indent)

    // Draw label
    TextStyle labelStyle(FontSize::SMALL, MONOOLED_WHITE, TextAlignment::LEFT);
    drawText(label, 0, y, labelStyle);

    // Calculate Y position for content
    int16_t contentY = y + 20 + LABEL_MARGIN;

    // Draw content
    TextStyle contentStyle(FontSize::MEDIUM, MONOOLED_WHITE, TextAlignment::LEFT);
    drawText(content, CONTENT_X, contentY, contentStyle);
}