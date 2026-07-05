#include "PauseMenuComponent.h"
#include "core/globals.h"
#include <nds.h>
#include <string>

// sfx
#include "soundbank.h"
// dialogue
#include "dialogue/demo_dialogue.h"

PauseMenuComponent* PauseMenuComponent::instance = nullptr;

void PauseMenuComponent::create()
{
    if (instance == nullptr)
    {
        instance = new PauseMenuComponent();
    }
}

void PauseMenuComponent::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }
    instance = nullptr;
}

PauseMenuComponent* PauseMenuComponent::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }
    return instance;
}

DialogueController dialogueCtrl;

void PauseMenuComponent::loadBg(int bgIndex)
{
    if (bgIndex < 0)
        return;

    std::string bgName;
    switch (bgIndex)
    {
    case 0: // Akihiko
        bgName = "bgAkihiko";
        break;

    case 1: // Kenji
        bgName = "bgKenji";
        break;

    case 2: // Yukari
        bgName = "bgYukari";
        break;

    case 3: // YukariClose
        bgName = "bgYukariClose";
        break;

    default:
        return;
    }

    GraphicAsset bg = graphicsCtrl->loadGrit(fatBasePath + "graphics/Dialogue/backgrounds/" + bgName + "/" + bgName);
    ;
    dmaCopy(bg.tiles, bgGetGfxPtr(bgSlot), bg.tilesLen);
    dmaCopy(bg.map, bgGetMapPtr(bgSlot), bg.mapLen);

    vramSetBankH(VRAM_H_LCD);
    dmaCopy(bg.pal, &VRAM_H_EXT_PALETTE[0][0], bg.palLen);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

    graphicsCtrl->unloadGrit(bg);
}

void PauseMenuComponent::init(int iBgSlot, bool* isActive, const std::string& iPauseMessage)
{
    BaseMenu::init(iBgSlot, isActive, iPauseMessage);
    options = menuOptions;
    optionCount = MENU_OPTIONS;
}

ViewState PauseMenuComponent::update(int keys)
{
    // dialogue controller takes full control when active
    if (dialogueCtrl.isActive())
    {
        dialogueCtrl.update(keys);
        return ViewState::KEEP_CURRENT;
    }

    return BaseMenu::update(keys);
}

// menu navigation handlers

ViewState PauseMenuComponent::openDebugMenu()
{
    return changeMenu(debugOptions, DEBUG_OPTIONS);
}

ViewState PauseMenuComponent::openSkillMenu()
{
    return changeMenu(skillOptions, SKILL_OPTIONS);
}

ViewState PauseMenuComponent::openItemMenu()
{
    return changeMenu(itemOptions, ITEM_OPTIONS);
}

ViewState PauseMenuComponent::openPersonaMenu()
{
    return changeMenu(personaOptions, PERSONA_OPTIONS);
}

ViewState PauseMenuComponent::openEquipMenu()
{
    return changeMenu(equipOptions, EQUIP_OPTIONS);
}

ViewState PauseMenuComponent::openStatusMenu()
{
    return changeMenu(statsOptions, STATS_OPTIONS);
}

ViewState PauseMenuComponent::openSLinkMenu()
{
    return changeMenu(sLinkOptions, S_LINK_OPTIONS);
}

ViewState PauseMenuComponent::openSystemMenu()
{
    return changeMenu(systemOptions, SYSTEM_OPTIONS);
}

ViewState PauseMenuComponent::openCharacterAnimMenu()
{
    return changeMenu(characterAnimOptions, CHARACTER_ANIM_OPTIONS);
}

// selection handlers

ViewState PauseMenuComponent::skillOptionSelected()
{
    return changeMenu(skills, SKILLS);
}

ViewState PauseMenuComponent::itemOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::equipOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::personaOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::statsOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::sLinkOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::systemOptionSelected()
{
    return ViewState::KEEP_CURRENT;
}

ViewState PauseMenuComponent::debugOptionSelected()
{
    ViewState selectedView;
    switch (static_cast<DebugOption>(selectedOption))
    {
    case DebugOption::DISCLAIMER_VIEW:
        musicCtrl->pause();
        selectedView = ViewState::DISCLAIMER;
        break;
    case DebugOption::INTRO_VIEW:
        selectedView = ViewState::INTRO;
        break;
    case DebugOption::MAIN_MENU_VIEW:
        selectedView = ViewState::MAIN_MENU;
        break;
    case DebugOption::IWATODAI_DORM_VIEW:
        selectedView = ViewState::IWATODAI_DORM;
        break;
    case DebugOption::IWATODAI_STREETS_VIEW:
        selectedView = ViewState::IWATODAI_STREETS;
        break;
    case DebugOption::STATION_VIEW:
        selectedView = ViewState::STATION;
        break;
    case DebugOption::SIGN_CONTRACT_VIEW:
        selectedView = ViewState::SIGN_CONTRACT;
        break;
    case DebugOption::PAULOWNIA_MALL_VIEW:
        selectedView = ViewState::PAULOWNIA_MALL;
        break;
    case DebugOption::INTRO_VIDEO:
        selectedView = ViewState::INTRO_VIDEO;
        break;
    case DebugOption::CUTSCENE_1:
        selectedView = ViewState::CUTSCENE_1;
        break;
    case DebugOption::CUTSCENE_2:
        selectedView = ViewState::CUTSCENE_2;
        break;
    case DebugOption::DEBUG_DIALOGUE:
        consoleClear();
        demo_yukari_kenji_argument_load();
        dialogueCtrl.setLoader(demo_yukari_kenji_argument_load_bg);
        dialogueCtrl.start(demo_yukari_kenji_argument_first());
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case DebugOption::TOGGLE_BILLBOARDS:
        Globals::enableBillboards = !Globals::enableBillboards;
        *isActivePtr = false;
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case DebugOption::TOGGLE_DEBUG_PRINT:
        Globals::enableDebugPrint = !Globals::enableDebugPrint;
        *isActivePtr = false;
        selectedView = ViewState::KEEP_CURRENT;
        break;
    default:
        selectedView = ViewState::KEEP_CURRENT;
    }
    return selectedView;
}

ViewState PauseMenuComponent::characterAnimOptionSelected()
{
    characterAnimationCtrl->stop();

    ViewState selectedView = ViewState::KEEP_CURRENT;
    switch (static_cast<CharacterAnimOption>(selectedOption))
    {
    case CharacterAnimOption::TOGGLE_AUTO_ANIM:
        Globals::enableCharacterAnim = !Globals::enableCharacterAnim;
        break;
    case CharacterAnimOption::ANIM_1:
        characterAnimationCtrl->set(0, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_2:
        characterAnimationCtrl->set(1, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_3:
        characterAnimationCtrl->set(2, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_4:
        characterAnimationCtrl->set(3, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_5:
        characterAnimationCtrl->set(4, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_6:
        characterAnimationCtrl->set(5, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_7:
        characterAnimationCtrl->set(6, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_8:
        characterAnimationCtrl->set(7, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_9:
        characterAnimationCtrl->set(8, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_10:
        characterAnimationCtrl->set(9, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_11:
        characterAnimationCtrl->set(10, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_12:
        characterAnimationCtrl->set(11, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_13:
        characterAnimationCtrl->set(12, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_14:
        characterAnimationCtrl->set(13, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_15:
        characterAnimationCtrl->set(14, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_16:
        characterAnimationCtrl->set(15, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_17:
        characterAnimationCtrl->set(16, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_18:
        characterAnimationCtrl->set(17, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_19:
        characterAnimationCtrl->set(18, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_20:
        characterAnimationCtrl->set(19, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_21:
        characterAnimationCtrl->set(20, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_22:
        characterAnimationCtrl->set(21, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_23:
        characterAnimationCtrl->set(22, true);
        Globals::enableCharacterAnim = false;
        break;
    case CharacterAnimOption::ANIM_24:
        characterAnimationCtrl->set(23, true);
        Globals::enableCharacterAnim = false;
        break;
    default:
        break;
    }

    *isActivePtr = false;
    characterAnimationCtrl->play();
    return selectedView;
}
