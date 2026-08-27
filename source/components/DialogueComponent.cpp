#include "DialogueComponent.hpp"
#include "core/globals.hpp"

void DialogueComponent::Init()
{
    isActive = false;

    cornerBlue = genericGraphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::CORNER);
    edgeBlue = genericGraphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::EDGE);
    cornerGreen = genericGraphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::CORNER_GREEN);
    edgeGreen = genericGraphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::EDGE_GREEN);
}

void DialogueComponent::Update(ae::fixed_t)
{
    if (!isActive || current == nullptr)
    {
        isActive = false;
        return;
    }

    // animation
    if (!text->appearTextDone())
    {
        /// swap the background exactly once per dialogue line, on the first
        /// frame, and only if the image has actually changed
        if (bgLoader && current->imageId != loadedImageId)
        {
            bgLoader(current->imageId);
            loadedImageId = current->imageId;
        }
        else
        {
            optionCount = (int)current->selections.size();

            if (optionCount > 0)
            {
                // render the full text + option list now that animation ended
                doRenderOptions = true;
            }
        }
    }
    else if (doRenderOptions)
    {
        renderOptions();
        doRenderOptions = false;
    }

    // input
    u32 pressed = systemKeysHeld & ~prevKeys;
    prevKeys = systemKeysHeld;

    if (pressed & KEY_START)
    {
        end();
        return;
    }

    if (optionCount > 0)
    {
        if (text->appearTextDone())
        {
            // switch palette to green
            dmaCopy(cornerGreen.pal, SPRITE_PALETTE_SUB + (2 * 16), 16 * sizeof(u16));
            dmaCopy(edgeGreen.pal, SPRITE_PALETTE_SUB + (3 * 16), 16 * sizeof(u16));

            // selection dialogue
            if (pressed & KEY_DOWN)
            {
                selectedOption = (selectedOption + 1) % optionCount;
                renderOptions();
            }
            else if (pressed & KEY_UP)
            {
                selectedOption = (selectedOption + optionCount - 1) % optionCount;
                renderOptions();
            }
        }
        if (pressed & KEY_A)
        {
            if (!text->appearTextDone())
            {
                text->appearTextSkip();
                return;
            }
            Dialogue* next = current->selections[selectedOption].next;
            if (next == nullptr)
            {
                end();
                return;
            }

            // switch palette to blue
            dmaCopy(cornerBlue.pal, SPRITE_PALETTE_SUB + (2 * 16), 16 * sizeof(u16));
            dmaCopy(edgeBlue.pal, SPRITE_PALETTE_SUB + (3 * 16), 16 * sizeof(u16));

            advanceTo(next);
        }
    }
    else
    {
        // linear dialogue
        if (pressed & KEY_A)
        {
            if (!text->appearTextDone())
            {
                text->appearTextSkip();
                return;
            }
            Dialogue* next = current->next;
            if (next == nullptr)
            {
                end();
                return;
            }
            advanceTo(next);
        }
        else if (pressed & KEY_B)
        {
            if (!text->appearTextDone())
            {
                text->appearTextSkip();
                return;
            }
            Dialogue* next = current->prev;
            if (next == nullptr)
            {
                end();
                return;
            }
            advanceTo(next);
        }
    }
}

void DialogueComponent::Destroy()
{
    isActive = false;
}

void DialogueComponent::configureDialogue(const DialogueConfig& config)
{
    bgLoader = config.loader;
    text = config.text;
    loadedImageId = -1; // force a bg load for the very first line
    prevKeys = 0;
    advanceTo(config.firstLine);
}

void DialogueComponent::start()
{
    prevKeys = systemKeysHeld;
    isActive = true;
}

void DialogueComponent::end()
{
    text->clearScreen();
    isActive = false;
}

void DialogueComponent::advanceTo(Dialogue* next)
{
    current = next;
    doRenderOptions = false;
    optionCount = 0;
    selectedOption = 0;
    text->clearScreen();
    renderAnimFrame();
}

void DialogueComponent::renderAnimFrame()
{
    text->drawText(current->characterName, 8, 133, TextColor::RichBlue);
    text->appearText(current->text, 8, 154, TextColor::Black);
}

void DialogueComponent::renderOptions()
{
    /// reprint the complete line then list choices below it
    text->clearScreen();
    text->drawText(current->characterName, 8, 133, TextColor::DualGreen);
    text->drawText(current->text, 8, 154, TextColor::Black);
    //TODO: The options are currently drawn outside the textbox, do we want to add an extra overlay for them similar to the actual game?
    int textSize = text->getFontSize();
    for (int i = 0; i < optionCount; i++)
    {
        if (i == selectedOption)
        {
            text->drawText(current->selections[i].text, 128, 50 + textSize * i, TextColor::Blue);
        }
        else
        {
            text->drawText(current->selections[i].text, 128, 50 + textSize * i, TextColor::White);
        }
    }
}
