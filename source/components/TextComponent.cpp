#include "TextComponent.hpp"

void TextComponent::Update(ae::fixed_t)
{
    if (appearingText != nullptr)
    {
        if (appearingText->cursorPos < (int)appearingText->content.size())
        {
            if (appearingText->counter <= 0)
            {
                ts.drawNextFromText(appearingText);
                appearingText->cursorPos++;
                appearingText->counter = ts.APPEAR_DELAY; // Reset the counter for the next character
            }
            else
                appearingText->counter--;
        }
        else //this text has finished appearing, so we can clear the storage
        {
            delete appearingText;
            appearingText = nullptr;
        }
    }
}

void TextComponent::Destroy()
{
    isActive = false;
}

void TextComponent::configureText(const TextConfig& config, bool loadDefaultPalette)
{
    isActive = true;
    videoBuffer = config.videoBuffer;
    if (config.fontNamePath != nullptr)
    {
        font = tm.loadFont(config.fontNamePath, config.fontSize);
    }
    else
    {
        // TODO: show error message
    }

    if (config.fontBitmapPath != nullptr)
    {
        tm.loadFontBitmap(config.fontBitmapPath);
    }
    else
    {
        // TODO: show error message
    }

    if (config.fontPalettePath != nullptr)
    {
        tm.loadPalette(config.fontPalettePath, config.isSub);
    }
    else
    {
        // TODO: show error message
    }

    if ((config.fontMetadataPath != nullptr) && (font != nullptr))
    {
        tm.loadFontMetadata(config.fontMetadataPath, font, config.isBoldBitmap);
    }
    else
    {
        // TODO: show error message
    }

    if (loadDefaultPalette)
    {
        tm.loadDefaultPalette();
    }
}

void TextComponent::drawText(const std::string& text, int x, int y, int color)
{
    ts.drawText(text, font, videoBuffer, x, y, color);
}

void TextComponent::appearText(const std::string& text, int x, int y, int color)
{
    ts.appearText(appearingText, text, font, videoBuffer, x, y, color);
}

void TextComponent::appearTextSkip()
{
    ts.appearTextSkip(appearingText);
}

bool TextComponent::appearTextDone()
{
    return ts.appearTextDone(appearingText);
}

void TextComponent::drawGlyph(const Glyph& glyph, int x, int y, int color, bool bold, bool italic, bool underline)
{
    ts.drawGlyph(glyph, font, videoBuffer, x, y, color, bold, italic, underline);
}

void TextComponent::clearArea(int x, int y, int width, int height)
{
    ts.clearArea(videoBuffer, x, y, width, height);
}

void TextComponent::clearScreen()
{
    tm.clearScreen(videoBuffer);
}

void TextComponent::testBitmap()
{
    ts.testBitmap(font, videoBuffer);
}

void TextComponent::testPalette()
{
    ts.testPalette(videoBuffer);
}
