#pragma once

#include "views/BaseView.h"

// core
#include "core/enums.h"
// environments/data
#include "data/environmentDb.h"
#include "environment/Environment.h"
// components
#include "components/menu/BattleMenuComponent.h"
#include "components/menu/PauseMenuComponent.h"
#include "components/ui/DialogueScreen.h"
#include "components/ui/MenuHUDScreen.h"
// controllers
#include "controllers/AnimationController.h"
#include "controllers/CharacterController.h"
#include "controllers/DialogueController.h"
#include "controllers/GraphicsController.h"
#include "controllers/UIController.h"

#include "systems/CameraSystem.hpp"

class EnvironmentView : public BaseView
{
  public:
    /**
     * @brief One-time setup for a room
     *
     * @note Resolves a room's EnvironmentDbEntry once into
     *       dbEntry. Everything below reads from that member instead of re-deriving it or
     *       relying on a per-room generated type. If no entry can be resolved,
     *       init() logs an error and returns immediately, since nothing below
     *       this point can run without a valid entry (setupEnvironment()
     *       immediately dereferences dbEntry->name).
     */
    void init() override;

    /**
     * @brief Per-frame update for this room's view
     *
     * @note  Advances the current ViewPhase, updates Controllers,
     *        and reports whether a phase transition to a different
     *        ViewState should occur.
     *
     * @return ViewState::KEEP_CURRENT to remain on this view for another
     *         frame, or another ViewState value to signal that the caller
     *         should transition away from this view entirely.
     */
    ViewState update() override;

    /**
     * @brief Tears down everything a room's view had set up
     */
    void cleanup() override;

    /**
     * @brief Loads and uploads a room's environment geometry and textures,
     *        driven entirely by dbEntry
     *
     * @note  No per-room texture-slot code and no per-room generated class needed.
     *
     * Loads each texture slot's texture assets to build display lists and upload
     * textures to VRAM, then unloads the texture assets. Logs a message if environment
     * loading fails, since a failed load otherwise leaves environments silently
     * rendering nothing.
     */
    void setupEnvironment();

  protected:
    // Room-specific hooks
    virtual float getCameraYOffset() const
    {
        return 0.1f;
    } // default

    virtual const EnvironmentDbEntry* getEnvironmentDbEntry() = 0;

    virtual CharacterController* createPlayerController() = 0;

    virtual void setMusic() = 0;

    virtual ViewState onTileCheck(TileType tile, u32 pressed) = 0;

    virtual void onDialogueStart() = 0;

    virtual void onSetupDialogueAndUI()
    {
    }

    virtual void setCameraConfig()
    {
    }

    // -------------------------------------------------
    // Battle
    virtual void startBattle()
    {
    }

    virtual void onBattleStart()
    {
    }

    // Shared state
    touchPosition touch;

    int bgSharedSub1;
    int bgSharedSub2;
    int bgSharedSub3;

    ViewPhase phase;

    bool prevPauseState = false;
    bool prevDialogueState = false;
    bool prevEnvironmentState = false;
    bool prevBattleState = false;
    bool isBattleMenuActive = false;
    bool promptDrawn = false;

    CharacterController* playerCtrl = nullptr;

    Event::CameraPosition camPos;
    const float tileSize = 0.062500f;

    // Override fields in setCameraConfig() — same struct for all modes
    Event::ConfigureCamera camConfig;

    // -------------------------------------------------
    // Controllers
    DialogueController dialogueCtrl;
    UIController* uiCtrl = UIController::getInstance();

    AnimationController* characterAnimationCtrl = AnimationController::getInstance();
    GraphicsController* graphicsCtrl = GraphicsController::getInstance();
    MusicController* musicCtrl = MusicController::getInstance();
    DialogueScreen* dialogueScreen = DialogueScreen::getInstance();
    MenuHUDScreen* menuHUDScreen = MenuHUDScreen::getInstance();
    BattleMenuComponent* battleMenuCmpt = BattleMenuComponent::getInstance();
    PauseMenuComponent* pauseMenuCmpt = PauseMenuComponent::getInstance();

    // Environment
    Environment env;
    const EnvironmentDbEntry* dbEntry = nullptr;

    uint16_t* textVideoBuffer;
    uint16_t* textVideoBufferSub;
    Font* cosmeticaFont = nullptr;
    TextController* textCtrl = TextController::getInstance();

  private:
    // fog properties
    int shift = 1;
    // how thick (translucent) the fog is
    int mass = 1;
    // how far the fog is (0x0000 to 0x8000)
    int depth = 0x6000;
};
