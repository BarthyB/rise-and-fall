//
//  CustomLookAndFeel.hpp
//  RISE&FALL - VST
//
//  Created by Barthélémy Bonhomme on 22.12.17.
//

#ifndef CustomLookAndFeel_hpp
#define CustomLookAndFeel_hpp

#include "../JuceLibraryCode/JuceHeader.h"

class CustomLookAndFeel : public LookAndFeel_V4 {
public:

    Colour COLOUR_BLACK;
    Colour COLOUR_WHITE;
    Colour COLOUR_RED;
    String DIMENSION_MS;
    String DIMENSION_PERCENT;
    String DIMENSION_TIMES;
    String DIMENSION_HERTZ;
    String DIMENSION_DECIBEL;

    CustomLookAndFeel() {
        COLOUR_BLACK = Colour(0xff181818);
        COLOUR_WHITE = Colour(0xffffffff);
        COLOUR_RED = Colour(0xffCC3333);
        DIMENSION_MS = CharPointer_UTF8(" ms");
        DIMENSION_PERCENT = CharPointer_UTF8(" %");
        DIMENSION_TIMES = CharPointer_UTF8(" x");
        DIMENSION_HERTZ = CharPointer_UTF8(" Hz");
        DIMENSION_DECIBEL = CharPointer_UTF8(" dB");
    };

    void drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, Slider &slider) override {
        const float radius = (width / 2) - 16.0f;
        const float centreX = x + width * 0.5f;
        const float centreY = centreX;
        const float rx = centreX - radius;
        const float ry = centreY - radius;
        const float rw = radius * 2.0f;
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour(COLOUR_BLACK);
        g.fillEllipse(rx, ry, rw, rw);

        // outline
        g.setColour(COLOUR_WHITE);
        g.drawEllipse(rx, ry, rw, rw, 5.0f);

        // pointer
        Path p;
        const float pointerLength = 18.0f;
        const float pointerThickness = 5.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(COLOUR_RED);
        g.fillPath(p);

        g.setColour(COLOUR_WHITE);
        const int numLines = 10;
        const float lineThickness = 4.0f;
        const float lineAngleStep = (rotaryEndAngle - rotaryStartAngle) / numLines;
        const float lineLength = pointerLength * 0.5f;
        for (int i = 1; i < numLines; i++) {
            Path line;
            const float lineAngle = rotaryStartAngle + (i * lineAngleStep);
            line.addRectangle(-lineThickness * 0.5f, -radius - lineLength - 6.0f, lineThickness, lineLength);
            line.applyTransform(AffineTransform::rotation(lineAngle).translated(centreX, centreY));

            // lines
            g.fillPath(line);
        }

        g.setFont(10.0f);
        auto maximum = (int) slider.getMinimum();

        String suffix = slider.getTextValueSuffix();
        String labelStart = String((int) slider.getMinimum()) + suffix;
        String labelEnd;
        if (maximum > 0) {
            if (suffix.equalsIgnoreCase(DIMENSION_HERTZ)) {
                labelEnd = String((int) (slider.getMaximum() / 1000)) + " kHz";
            } else if (suffix.equalsIgnoreCase(DIMENSION_MS)) {
                labelEnd = String((int) (slider.getMaximum() / 1000)) + " s";
            }
        } else {
            labelEnd = String((int) slider.getMaximum()) + suffix;
        }

        g.drawFittedText(labelStart, 0, height - 26, width, 12, Justification::left, 1);
        g.drawFittedText(labelEnd, 0, height - 26, width, 12, Justification::right, 1);

        g.setFont(12.0f);
        g.drawFittedText(slider.getName(), 0, height - 12, width, 12, Justification::centred, 1);
    }

    void drawComboBox(Graphics &g, int width, int height, bool, int, int, int, int, ComboBox &box) override {
        const Rectangle<int> boxBounds(0, 16, width, 32);
        g.setColour(COLOUR_WHITE);
        g.fillRect(boxBounds);
        g.setFont(12.0f);
        g.drawFittedText(box.getName(), 0, 0, width, 12, Justification::left, 1);

        Rectangle<int> arrowZone(width - 30, 0, 20, height);
        Path path;
        path.startNewSubPath(arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);
        path.lineTo(static_cast<float> (arrowZone.getCentreX()), arrowZone.getCentreY() + 3.0f);
        path.lineTo(arrowZone.getRight() - 3.0f, arrowZone.getCentreY() - 2.0f);

        g.setColour(COLOUR_BLACK);
        g.strokePath(path, PathStrokeType(2.0f));

        box.setColour(ComboBox::textColourId, COLOUR_BLACK);
    }

    void drawPopupMenuItem(Graphics &g, const Rectangle<int> &area, const bool isSeparator, const bool isActive,
                           const bool isHighlighted, const bool isTicked, const bool hasSubMenu, const String &text,
                           const String &shortcutKeyText, const Drawable *icon,
                           const Colour *const textColourToUse) override {
        auto textColour = (textColourToUse == nullptr ? findColour(PopupMenu::textColourId) : *textColourToUse);
        auto r = area.reduced(1);

        if (isHighlighted) {
            g.setColour(COLOUR_WHITE);
        } else {
            g.setColour(COLOUR_BLACK);
        }
        g.fillRect(r);
        g.setColour(COLOUR_BLACK);

        r.reduce(jmin(5, area.getWidth() / 20), 0);
        auto font = getPopupMenuFont();
        const auto maxFontHeight = r.getHeight() / 1.3f;

        if (font.getHeight() > maxFontHeight) {
            font.setHeight(maxFontHeight);
        }

        g.setFont(font);

        auto iconArea = r.removeFromLeft(roundToInt(maxFontHeight / 2)).toFloat();
        if (isTicked) {
            Path p;
            p.addEllipse(0, 0, 1, 1);
            if (isHighlighted) {
                g.setColour(COLOUR_BLACK);
            } else {
                g.setColour(COLOUR_WHITE);
            }
            g.fillPath(p, p.getTransformToScaleToFit(iconArea.reduced(iconArea.getWidth() / 5, 0).toFloat(), true));
        }

        r.removeFromRight(3);

        if (isHighlighted) {
            g.setColour(COLOUR_BLACK);
        } else {
            g.setColour(COLOUR_WHITE);
        }
        g.drawFittedText(text, r, Justification::centredLeft, 1);
    }


    void drawButtonBackground(Graphics &g, Button &button, const Colour &backgroundColour, bool isMouseOverButton,
                              bool isButtonDown) override {

        const auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

        auto baseColour = COLOUR_WHITE.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
                .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

        if (isButtonDown || isMouseOverButton) {
            baseColour = baseColour.contrasting(isButtonDown ? 0.2f : 0.05f);
        }

        g.setColour(baseColour);
        g.fillRect(bounds);
    }

    void getIdealPopupMenuItemSize(const String &text, const bool isSeparator, int standardMenuItemHeight,
                                   int &idealWidth, int &idealHeight) override {
        if (isSeparator) {
            idealWidth = 50;
            idealHeight = standardMenuItemHeight > 0 ? standardMenuItemHeight / 10 : 10;
        } else {
            auto font = getPopupMenuFont();
            font.setHeight(16);

            idealHeight = 32;
            idealWidth = font.getStringWidth(text) + 32;
        }
    }

    void drawLinearSlider(Graphics &g, int x, int y, int width, int height, float sliderPos,
                          float minSliderPos, float maxSliderPos, const Slider::SliderStyle style,
                          Slider &slider) override {
        auto trackWidth = jmin(6.0f, slider.isHorizontal() ? height * 0.25f : width * 0.25f);

        Point<float> startPoint(slider.isHorizontal() ? x : x + width * 0.5f,
                                slider.isHorizontal() ? y + height * 0.5f : height + y);

        Point<float> endPoint(slider.isHorizontal() ? width + x : startPoint.x,
                              slider.isHorizontal() ? startPoint.y : y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath(startPoint);
        backgroundTrack.lineTo(endPoint);
        g.setColour(COLOUR_WHITE.withMultipliedAlpha(0.5));
        g.strokePath(backgroundTrack, {trackWidth, PathStrokeType::curved, PathStrokeType::rounded});

        Path valueTrack;
        Point<float> minPoint, maxPoint, thumbPoint;

        auto kx = slider.isHorizontal() ? sliderPos : (x + width * 0.5f);
        auto ky = slider.isHorizontal() ? (y + height * 0.5f) : sliderPos;

        minPoint = startPoint;
        maxPoint = {kx, ky};

        valueTrack.startNewSubPath(minPoint);
        valueTrack.lineTo(maxPoint);
        g.setColour(COLOUR_WHITE);
        g.strokePath(valueTrack, {trackWidth, PathStrokeType::curved, PathStrokeType::rounded});

        auto thumbWidth = getSliderThumbRadius(slider);

        g.setColour(COLOUR_RED);
        g.fillEllipse(Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (thumbWidth))
                              .withCentre(maxPoint));

        g.setFont(12.0f);
        g.setColour(COLOUR_WHITE);
        g.drawFittedText(slider.getName(), 12, 0, width, 12, Justification::left, 1);
    }

};

#endif /* CustomLookAndFeel_hpp */
