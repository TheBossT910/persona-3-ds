#pragma once

#include "components/TextComponent.hpp"
#include "types/GraphicsTypes.hpp"
#include <etl/span.h>
#include <string>
#include <vector>

class DialogueScreen;
struct Dialogue;

/**
 * @brief Holds dialogue selection choice data
 */
struct DialogueSelection
{
    std::string text;
    bool isSelected;
    Dialogue* next;
};

/**
 * @brief Holds dialogue content and branching data
 */
struct Dialogue
{
    // content
    std::string name;
    std::string text;

    // bust
    etl::span<SpritePayload> spritePayload;

    // branching
    Dialogue* prev;
    Dialogue* next;
    etl::vector<DialogueSelection, 3> selections;
};

/**
 * @brief A struct that holds initial config values for the DialogueComponent
 */
struct DialogueConfig
{
    Dialogue* firstLine = nullptr;
    TextComponent* text = nullptr;
    TextComponent* textAlt = nullptr;
    DialogueScreen* screen = nullptr;

    DialogueConfig() = default;

    DialogueConfig(Dialogue* iFirstLine, TextComponent* iText, TextComponent* iTextAlt, DialogueScreen* iScreen)
        : firstLine(iFirstLine), text(iText), textAlt(iTextAlt), screen(iScreen)
    {
    }

    // config only for debug purposes
    DialogueConfig(Dialogue* iFirstLine, TextComponent* iText) : firstLine(iFirstLine), text(iText)
    {
        textAlt = text;
    }
};
