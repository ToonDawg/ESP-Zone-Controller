#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "Icons.h"
#include "Menu.h"
#include <Loader.h>

class DisplayManager
{
public:
    enum class FontSize
    {
        SMALL,
        MEDIUM,
        LARGE
    };
    enum class TextAlignment
    {
        LEFT,
        CENTER,
        RIGHT
    };

    struct TextStyle
    {
        FontSize fontSize;
        uint16_t color;
        TextAlignment alignment;

        TextStyle(FontSize size, uint16_t col, TextAlignment align)
            : fontSize(size), color(col), alignment(align) {}

        TextStyle() : fontSize(FontSize::MEDIUM), color(MONOOLED_WHITE), alignment(TextAlignment::LEFT) {}
    };

    DisplayManager(Adafruit_SH1106G &display);

    void displayTemperature(float temperature, tImage tempIcon);
    void displayIconBottomLeft(const tImage &icon);
    void displayIconBottomRight(const tImage &icon);
    void displayIconBottomMiddle(const tImage &icon);
    void displayBottomCenterText(const String &text);
    void clearDisplay();
    void render();
    void displayCenteredWrappedText(const String &text);
    void displaySettingsMenu(const Menu &menu);
    void displayMenuTitle(const String title);
    void drawScrollIndicator(bool isUpIndicator);

    uint16_t getWidth() const { return display.width(); }
    uint16_t getHeight() const { return display.height(); }
    void showLoader();
    void showLoaderWithText(const String &text);
    void displayLabelAndContent(const String &label, const String &content, int16_t y);
    void displayMenuItem(const String &item, bool selected, bool active, int16_t y, int16_t verticalOffset, int16_t textHeight);


private:
    Adafruit_SH1106G &display;
    static const GFXfont mediumFont;
    static const GFXfont largeFont;
    LoaderAnimation loader;

    void drawText(const String &text, int16_t x, int16_t y, const TextStyle &style);
    void setFont(FontSize size);
    int16_t calculateTextWidth(const String &text);
    int16_t calculateTextHeight(const String &text);
    void draw8BitImage(int16_t x, int16_t y, const tImage &image);

    String cutoffText(const String &text, int16_t maxWidth);

    static constexpr int16_t DISPLAY_SIDE_MARGIN = 4;
    static constexpr int16_t TEMPERATURE_FONT_HEIGHT = 35;
    static constexpr int16_t FONT_VERTICAL_PADDING = 6;

    struct ScrollInfo
    {
        unsigned long startTime;
        int16_t offset;
        String currentItem;
        static constexpr unsigned long DELAY = 2000;
        static constexpr unsigned long SPEED = 100;

        ScrollInfo() : startTime(0), offset(0), currentItem("") {}

        void reset(const String &item)
        {
            startTime = millis();
            offset = 0;
            currentItem = item;
        }
    };

    ScrollInfo scroll;
};

#endif // DISPLAY_MANAGER_H
