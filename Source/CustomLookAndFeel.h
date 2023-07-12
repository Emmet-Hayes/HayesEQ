#pragma once
#include "CentredLabel.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();

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
        l->setFont(juce::Font("Lucida Console", 7.f * windowScale, juce::Font::bold));
        l->setJustificationType(juce::Justification::centred);
        
        return l;
    }

    juce::Font getLabelFont(juce::Label& label) override
    {
        return juce::Font(juce::Font("Lucida Console", 7.f * windowScale, juce::Font::bold));
    }

    void drawComboBox(juce::Graphics&, int, int, bool, int, int, int, int, juce::ComboBox&) override;
    juce::Font getComboBoxFont(juce::ComboBox&) override;
	juce::Font getPopupMenuFont() override;
	void drawPopupMenuBackground(juce::Graphics&, int, int) override;
    void drawRotarySlider(juce::Graphics&, int, int, int, int, float, const float,
		const float, juce::Slider&) override;
    void setWindowScale(const float& newScale) { windowScale = newScale; }

private:
	juce::Font getCommonMenuFont(float);
	
    float windowScale = 1.0f;
    float rotaryOutlineBrightness = 1.0f;
};
