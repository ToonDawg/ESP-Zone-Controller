#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Wire.h>
#include "Icons.h"
#include <vector>
#include "Menu.h"


class DisplayManager {
public:
    enum class TextAlignment { LEFT, CENTER, RIGHT };
    enum class FontSize { SMALL, LARGE };
    
    struct TextStyle {
        FontSize fontSize;
        uint16_t color;
        TextAlignment alignment;
        
        TextStyle(FontSize size = FontSize::SMALL, 
                  uint16_t col = MONOOLED_WHITE, 
                  TextAlignment align = TextAlignment::LEFT)
            : fontSize(size), color(col), alignment(align) {}
    };

    DisplayManager(Adafruit_SH1106G& display);

    // Main display functions
    void displayTemperature(float temperature);
    void displayIconBottomLeft(const tImage& icon);
    void displayIconBottomRight(const tImage& icon);
    void displayBottomCenterText(const String& text);

    // Additional display functions
    void displayOff();
    void render();
    void displayCentre(const String lines[], int numLines, const TextStyle& style = TextStyle());
    void displaySettingsMenu(const Menu &menu);

    // Utility functions
    uint16_t getWidth() const { return display.width(); }
    uint16_t getHeight() const { return display.height(); }

protected:
    Adafruit_SH1106G& display;
    static const GFXfont smallFont;
    static const GFXfont largeFont;

    // Drawing helper functions
    void drawText(const String& text, int16_t x, int16_t y, const TextStyle& style);
    int16_t calculateTextWidth(const String& text, const GFXfont* font);
    int16_t calculateTextHeight(const String& text, const GFXfont* font);
    void draw8BitImage(int16_t x, int16_t y, const tImage& image);
    
    // Menu helper functions
    int16_t displayMenuTitle(const String& title);
    int16_t getStringWidth(const String &str);
    String cutoffText(const String &text, int16_t maxWidth);
    void displayMenuItem(const String &item, bool selected, bool active, int16_t y, int16_t verticalOffset, int16_t textHeight);

    // Font helper function
    const GFXfont* getFontForSize(FontSize size) const;

    // Constants
    static constexpr int16_t DISPLAY_SIDE_MARGIN = 4;
    static constexpr int16_t TEMPERATURE_FONT_HEIGHT = 35;
    static constexpr int16_t FONT_VERTICAL_PADDING = 6;

    unsigned long scrollStartTime = 0;
    int16_t scrollOffset = 0;
    String currentScrollingItem;
    const unsigned long SCROLL_DELAY = 2000;
    const unsigned long SCROLL_SPEED = 100;
};

#endif // DISPLAY_MANAGER_H