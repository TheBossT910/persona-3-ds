#pragma once

#include "components/TextComponent.hpp"
#include <string>
#include <vector>

struct Dialogue;
struct DialogueSelection
{
    std::string text;
    bool isSelected;
    Dialogue* next;
};
struct Dialogue
{
    std::string characterName;
    std::string text;
    int imageId;
    Dialogue* prev;
    Dialogue* next;
    std::vector<DialogueSelection> selections;
};

/**
 * @brief A struct that holds initial config values for the DialogueComponent
 */
struct DialogueConfig
{
    Dialogue* firstLine = nullptr;
    void (*loader)(int bgIndex) = nullptr;
    TextComponent* text = nullptr;

    DialogueConfig() = default;

    DialogueConfig(Dialogue* iFirstLine, void (*iLoader)(int bgIndex), TextComponent* iText)
        : firstLine(iFirstLine), loader(iLoader), text(iText)
    {
    }
};
