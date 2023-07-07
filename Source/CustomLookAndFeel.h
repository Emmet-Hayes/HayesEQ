#pragma once
#include "CentredLabel.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:

    void drawComboBox(juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override
    {
        auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 5.0f;
        juce::Rectangle<int> boxBounds(0, 0, width, height);

        g.setColour(juce::Colours::darkmagenta);
        g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);
        g.setColour(juce::Colours::silver);
        g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

        juce::Path path;
        path.startNewSubPath(buttonX + buttonW * 0.5f, buttonY + buttonH * 0.3f);
        path.lineTo(buttonX + buttonW * 0.1f, buttonY + buttonH * 0.7f);
        path.lineTo(buttonX + buttonW * 0.9f, buttonY + buttonH * 0.7f);
        path.closeSubPath();

        g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
        g.fillPath(path);
    }

    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override
    {
        g.fillAll(juce::Colours::darkmagenta);
    }

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted,
                           bool isTicked, bool hasSubMenu, const juce::String& text,
                           const juce::String& shortcutKeyText, const juce::Drawable* icon,
                           const juce::Colour* textColourToUse) override
    {
        if (isHighlighted)
            g.fillAll(juce::Colours::silver);

        g.setColour(isHighlighted ? juce::Colours::black : juce::Colours::white);
        g.setFont(juce::Font(16.0f * windowScale));
        g.drawText(text, area, juce::Justification::centred);
    }
    
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        g.fillAll (label.findColour (juce::Label::backgroundColourId));
        
        if (! label.isBeingEdited())
        {
            auto alpha = label.isEnabled() ? 1.0f : 0.5f;
            const juce::Font font (getLabelFont (label).withHeight(16.0f * windowScale));
            
            g.setColour (label.findColour (juce::Label::textColourId).withMultipliedAlpha (alpha));
            g.setFont (font);
            juce::Rectangle<int> textArea (label.getBorderSize().subtractedFrom (label.getLocalBounds()));
            g.drawFittedText (label.getText(), textArea, label.getJustificationType(),
                              jmax (1, (int) (textArea.getHeight() / font.getHeight())),
                              label.getMinimumHorizontalScale());
            g.setColour (label.findColour (juce::Label::outlineColourId).withMultipliedAlpha (alpha));
        }
        else if (label.isEnabled())
        {
            g.setColour (label.findColour (juce::Label::outlineColourId));
        }
        
        g.drawRect (label.getLocalBounds());
    }

    juce::Label* createSliderTextBox(juce::Slider& slider) override
    {
        auto* l = new CentredLabel();

        l->setColour(juce::Label::textColourId, slider.findColour(juce::Slider::textBoxTextColourId));
        l->setColour(juce::Label::backgroundColourId,
            (slider.getSliderStyle() == juce::Slider::LinearBar || slider.getSliderStyle() == juce::Slider::LinearBarVertical)
            ? juce::Colours::transparentBlack
            : slider.findColour(juce::Slider::textBoxBackgroundColourId));
        l->setColour(juce::Label::outlineColourId, slider.findColour(juce::Slider::textBoxOutlineColourId));
        l->setColour(juce::TextEditor::textColourId, slider.findColour(juce::Slider::textBoxTextColourId));
        l->setColour(juce::TextEditor::backgroundColourId,
            slider.findColour(juce::Slider::textBoxBackgroundColourId)
            .withAlpha((slider.isMouseOverOrDragging() && slider.getSliderStyle() != juce::Slider::SliderStyle::LinearBar
                        && slider.getSliderStyle() != juce::Slider::SliderStyle::LinearBarVertical) ? 0.7f : 0.3f));
        l->setColour(juce::TextEditor::outlineColourId, slider.findColour(juce::Slider::textBoxOutlineColourId));
        l->setFont(juce::Font(16.0f * windowScale));
        l->setJustificationType(juce::Justification::centred);
        
        return l;
    }

    juce::Font getLabelFont(juce::Label& label) override
    {
        return juce::Font(16.0f * windowScale);
    }

    void setWindowScale(const float& newScale) { windowScale = newScale; }

private:
    float windowScale = 1.0f;
};
