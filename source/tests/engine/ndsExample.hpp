#pragma once

/**
 * @file ndsExample.hpp
 *
 * None of this is part of the engine. It exists to demonstrate the pattern
 * a game should follow: define your own component-type enum, event ids,
 * event structs, and payload structs, then hand them to `ComponentRouter` /
 * `SystemRouter` as template arguments. Delete/replace this file with your
 * actual game code.
 *
 * This example specifically walks through the FULL Pub/Sub cycle in both
 * directions:
 *   1. Component -> System: `HealthComponent::TakeDamage()` broadcasts a
 *      `StartBattleSystem` event followed by a `Damage` event.
 *   2. System -> Component: `BattleSystem` receives that `Damage` event,
 *      applies a rule, and broadcasts a `State` event back out.
 *   3. Component -> Manager: `MeshComponent` submits render data directly
 *      to `RenderManager` (one-way, not Pub/Sub, see
 *      `Component::SubmitToManager`).
 */

#include <aegis/component.hpp>
#include <aegis/engine.hpp>
#include <aegis/entity.hpp>
#include <aegis/manager.hpp>
#include <aegis/system.hpp>

#ifdef __NDS__
#include <nds.h>
#define ENGINE_TRACE(...) printf(__VA_ARGS__)
#else
#define ENGINE_TRACE(...) ((void)0)
#endif

// =============================================================================
// Game-defined component types (NOT part of the engine)
// =============================================================================

/**
 * @brief Identifies each concrete `Component` subclass this game defines.
 *        Underlying type must match `ComponentTypeID`.
 */
enum class ComponentType : ae::ComponentTypeID
{
    None = 0,
    Mesh,
    Hitbox,
    Sfx,
    Health,
};

// =============================================================================
// Game-defined event ids + event structs (NOT part of the engine)
// =============================================================================

/// Compile-time message ids for every `Event::*` struct below.
namespace EventID
{
enum : etl::message_id_t
{
    TookDamage = 0,
    PlaySound,
    ChangeState,
    StartBattleSystem,
};
} // namespace EventID

/**
 * @brief POD messages broadcast over `engineBus` via `BroadcastEvent(...)`.
 *
 *  A "Start[SystemName]System" event (see StartBattleSystem` below) is
 *  reserved for Component -> System activation.
 *
 * @note These are NOT aggregates (they inherit from `etl::message<ID>`, a
 *       polymorphic base), so brace-init like `Event::Damage{15, 0}` will
 *       fail to compile. Default-construct then assign fields instead.
 *       See `HealthComponent::TakeDamage` for the pattern.
 */
namespace Event
{
/// Component -> System: this entity just took damage.
struct Damage : public etl::message<EventID::TookDamage>
{
    std::int32_t amount;
    std::int32_t element;
};

/// Component -> System: request a sound be played.
struct Audio : public etl::message<EventID::PlaySound>
{
    std::int32_t soundID;
    std::int32_t volume;
};

/// System -> Component: change internal state (e.g. "Hurt", "Idle").
/// `appliedAmount` carries whatever numeric result the System computed
/// for this state change (e.g. final, rule-modified HP delta), its
/// meaning is defined per `newStateID`, not globally.
struct State : public etl::message<EventID::ChangeState>
{
    std::int32_t newStateID;
    std::int32_t appliedAmount;
};

/**
     * @brief Component -> System: wakes `BattleSystem` up for this frame.
     *
     * Empty payload. This event exists purely to flip `BattleSystem`'s
     * `isActive` flag on, per the HLD's "Update Frequency" rule that
     * Systems "exit update early if false" and "toggle their own flag
     * based on events received from pub/sub".
     */
struct StartBattleSystem : public etl::message<EventID::StartBattleSystem>
{
};
} // namespace Event

// =============================================================================
// Game-defined payload types (Component -> Manager, NOT part of the engine)
// =============================================================================

/// POD data submitted one-way, Component -> Manager (never on the bus).
namespace Payload
{
struct RenderMesh
{
    std::int32_t modelID;
    ae::fixed_t matrix[16];
};

struct CollisionHitbox
{
    ae::fixed_t radius;
    ae::fixed_t x;
    ae::fixed_t y;
};
} // namespace Payload

// =============================================================================
// Example concrete Manager
// =============================================================================

/**
 * @brief Example Manager: owns rendering hardware, receives mesh data
 *        directly from `MeshComponent` (Component -> Manager, one-way).
 */
class RenderManager : public ae::Manager, public ae::Singleton<RenderManager>
{
  public:
    void Init() override
    {
    }
    void Process() override
    {
    }
    void Shutdown() override
    {
    }

    /**
     * @brief Concrete, Manager-specific submission API (see `Manager`'s
     *        class docs for why this isn't a generic virtual).
     * @param data Mesh/model transform to render this frame.
     */
    void SubmitData(const Payload::RenderMesh& data)
    { /* push to VRAM, etc. */
    }

  private:
    friend class Singleton<RenderManager>;
    RenderManager() = default;
};

// =============================================================================
// Example concrete Component, demonstrates BOTH sending and receiving.
// =============================================================================

/**
 * @brief Example Component: tracks HP, and demonstrates the full two-way
 *        Pub/Sub cycle.
 *
 * - **Sends**: `TakeDamage()` broadcasts `Event::StartBattleSystem` then
 *   `Event::Damage`, an external caller (input, collision, script) drives
 *   gameplay by calling this, which is how "Component -> System" traffic
 *   usually originates in practice.
 * - **Receives**: `on_receive(const Event::State&)` reacts when
 *   `BattleSystem` broadcasts a state change back.
 */
class HealthComponent : public ae::ComponentRouter<HealthComponent, Event::State>
{
  public:
    static constexpr ae::ComponentTypeID TYPE_ID = static_cast<ae::ComponentTypeID>(ComponentType::Health);
    void Init() override
    {
        currentHP = 100;
    }
    void Update(ae::fixed_t /*dt*/) override
    {
    }
    void Destroy() override
    {
    }
    ae::ComponentTypeID GetType() const override
    {
        return TYPE_ID;
    }

    /**
     * @brief Called by external gameplay code (input/collision/script) to
     *        apply damage to this entity.
     *
     * This is where a "send" originates: broadcasting `StartBattleSystem`
     * first ensures `BattleSystem` is active before its `Update()` runs
     * this frame, then `Damage` carries the actual payload for the system
     * to process.
     * @param amount  Raw damage amount.
     * @param element Elemental type id (game-defined).
     */
    void TakeDamage(std::int32_t amount, std::int32_t element)
    {
        ENGINE_TRACE("[HealthComponent] TakeDamage(%ld) HP before=%ld\n", (long)amount, (long)currentHP);
        ae::BroadcastEvent(Event::StartBattleSystem{});

        Event::Damage msg;
        msg.amount = amount;
        msg.element = element;
        ae::BroadcastEvent(msg);
    }

    /// Debug/testing accessor — not required by the engine itself.
    std::int32_t GetCurrentHP() const
    {
        return currentHP;
    }

    /// Receives `Event::State` broadcast back by `BattleSystem` after it
    /// finishes processing a `Damage` event. `BattleSystem` owns the rule
    /// (e.g. elemental modifiers); this Component owns its own data and
    /// applies the rule's result here.
    void on_receive(const Event::State& msg)
    {
        currentHP -= msg.appliedAmount;
        currentStateID = msg.newStateID;
        ENGINE_TRACE("[HealthComponent] <- BattleSystem State: applied=%ld, newState=%ld, HP after=%ld\n",
                     (long)msg.appliedAmount,
                     (long)msg.newStateID,
                     (long)currentHP);
    }

    void on_receive_unknown(const etl::imessage&)
    {
    }

  protected:
    void SubmitToManager() override
    {
    }

  private:
    std::int32_t currentHP = 100;
    std::int32_t currentStateID = 0;
};

// =============================================================================
// Example concrete Component — submits render data to RenderManager.
// =============================================================================

/// Example Component with no Pub/Sub needs, derives `Component` directly.
class MeshComponent : public ae::Component
{
  public:
    static constexpr ae::ComponentTypeID TYPE_ID = static_cast<ae::ComponentTypeID>(ComponentType::Mesh);
    void Init() override
    {
    }
    void Update(ae::fixed_t /*dt*/) override
    {
        SubmitToManager();
    }
    void Destroy() override
    {
    }
    ae::ComponentTypeID GetType() const override
    {
        return TYPE_ID;
    }

  protected:
    void SubmitToManager() override
    {
        RenderManager::GetInstance().SubmitData(Payload::RenderMesh{modelID, {}});
    }

  private:
    std::int32_t modelID = 0;
};

// =============================================================================
// Example concrete System, demonstrates BOTH receiving and sending.
// =============================================================================

/// Fixed router id for the `BattleSystem` singleton (systems are singletons,
/// so a compile-time constant is sufficient, no auto-increment needed).
constexpr etl::message_router_id_t kBattleSystemRouterID = 0;

/**
 * @brief Example System: applies battle rules to incoming damage events.
 *
 * - **Receives**: `on_receive(const Event::StartBattleSystem&)` flips
 *   `isActive` on; `on_receive(const Event::Damage&)` queues a rule to run.
 * - **Sends**: after `Update()` processes a queued hit, it broadcasts
 *   `Event::State` so components (e.g. `HealthComponent`) can react.
 */
class BattleSystem : public ae::SystemRouter<BattleSystem, Event::Damage, Event::StartBattleSystem>,
                     public ae::Singleton<BattleSystem>
{
  public:
    void Init() override
    {
    }
    void Shutdown() override
    {
    }

    /**
     * @brief Per-frame update. Exits early if `isActive` is false, and
     *        applies the queued damage rule otherwise, then broadcasts
     *        the result.
     */
    void Update(ae::fixed_t /*dt*/) override
    {
        if (!isActive)
        {
            return;
        }

        // Apply battle rules to the queued hit (elemental modifiers, etc.)
        // — passthrough here; extend this line with real rule logic later.
        const std::int32_t appliedAmount = pendingDamageAmount;
        const std::int32_t resultingStateID = appliedAmount > 0 ? 1 /* "Hurt" */ : 0 /* "Idle" */;

        ENGINE_TRACE("[BattleSystem] Update: applying rule -> amount=%ld, newState=%ld\n",
                     (long)appliedAmount,
                     (long)resultingStateID);

        // System -> Component: broadcast the outcome back out.
        Event::State msg;
        msg.newStateID = resultingStateID;
        msg.appliedAmount = appliedAmount;
        ae::BroadcastEvent(msg);

        pendingDamageAmount = 0;
        isActive = false; // done processing; wait for the next StartBattleSystem
    }

    /// Component -> System: activation event, per the HLD's reserved
    /// "Start[SystemName]System" naming convention.
    void on_receive(const Event::StartBattleSystem&)
    {
        isActive = true;
        ENGINE_TRACE("[BattleSystem] <- StartBattleSystem (isActive=true)\n");
    }

    /// Component -> System: queues the hit for `Update()` to process.
    void on_receive(const Event::Damage& msg)
    {
        pendingDamageAmount = msg.amount;
        ENGINE_TRACE("[BattleSystem] <- Damage: amount=%ld, element=%ld\n", (long)msg.amount, (long)msg.element);
    }

    void on_receive_unknown(const etl::imessage&)
    {
    }

  private:
    friend class Singleton<BattleSystem>;
    BattleSystem() : SystemRouter(kBattleSystemRouterID)
    {
    }

    std::int32_t pendingDamageAmount = 0;
};

// =============================================================================
// Sizing and instantiating the Engine
// =============================================================================

/// Computes the pool block size/alignment needed for this game's largest
/// concrete `Component`. Extend the `sizeof`/`alignof` comparisons as new
/// Component subclasses are added.
namespace GameEngineConfig
{
constexpr std::size_t kLargestComponentSize = sizeof(HealthComponent) > sizeof(MeshComponent) ? sizeof(HealthComponent)
                                                                                              : sizeof(MeshComponent);

constexpr std::size_t kLargestComponentAlign = alignof(HealthComponent) > alignof(MeshComponent)
                                                   ? alignof(HealthComponent)
                                                   : alignof(MeshComponent);
} // namespace GameEngineConfig

/// This game's concrete `Engine` specialization — see `Engine`'s class docs.
using GameEngine = ae::Engine<GameEngineConfig::kLargestComponentSize, GameEngineConfig::kLargestComponentAlign>;

// =============================================================================
// Example hardware hooks (HAL design, see Engine::SetPollInputCallback)
// =============================================================================

/// Stand-in for reading button/touch state each frame.
void MyInputPoller()
{ /* read gamepad state */
}

/// Stand-in for the final "push everything to hardware" step each frame.
void MyComputePusher()
{ /* swap buffers, push to VRAM */
}

// =============================================================================
// Example main(), end-to-end usage
// =============================================================================

int ndsExampleMain()
{
    // 1. Instantiate the Engine (owns all pools, keep off the stack on
    //    real hardware; a static/global here is fine for this example).
    static GameEngine engine;

    // 2. Register singletons.
    engine.RegisterManager(&RenderManager::GetInstance());
    engine.RegisterSystem(&BattleSystem::GetInstance());

    // 3. Wire up platform hooks (HAL design, Engine itself stays hardware-agnostic).
    engine.SetPollInputCallback(&MyInputPoller);
    engine.SetComputeCallback(&MyComputePusher);

    // 4. Initialize everything (Systems first, then Managers).
    engine.InitAll();

    // 5. Set up initial game state.
    ae::Entity* player = engine.CreateEntity();
    HealthComponent* health = nullptr;

    if (player != nullptr)
    {
        health = engine.CreateComponent<HealthComponent>();
        MeshComponent* mesh = engine.CreateComponent<MeshComponent>();

        player->AddComponent(health);
        player->AddComponent(mesh);
    }

    // Fixed-point delta time, expressed in seconds (~16.67ms at 60 FPS).
    // On real NDS hardware this is a constant tied to VBlank, not measured.
    const ae::fixed_t dt = ae::fixed_t(1) / 60;

    // 6. The main game loop.
    bool isRunning = true;
    int frameCount = 0;

    while (isRunning)
    {
        // Simulate something (input/collision/script) triggering damage on
        // frame 0. This is the Component -> System "send" that kicks the
        // whole Pub/Sub cycle from section 1 of the file comment above.
        if (frameCount == 0 && health != nullptr)
        {
            health->TakeDamage(15, /*element=*/0);
        }

        engine.Tick(dt); // Poll Input -> Update Systems -> Update Components -> Process Managers -> Compute

        if (++frameCount >= 3)
        {
            isRunning = false; // just run a few frames for this example
        }
    }

    // 7. Cleanup memory and hardware state.
    engine.ShutdownAll();

    return 0;
}

#ifdef __NDS__
// =============================================================================
// Engine test, run in main.cpp to ensure engine is working properly
// =============================================================================
void NDSPollInputCallback()
{
    scanKeys();

    // ex. pass the states to Managers here
    // uint32_t keys_pressed = keysDown();
    // InputManager::GetInstance().Update(keys_pressed);
}

void NDSComputeCallback()
{
    //...
}
void ndsExampleTest()
{
    consoleDemoInit();
    printf("Engine test\n");

    static GameEngine engine;
    engine.SetComputeCallback(&NDSComputeCallback);
    engine.SetComputeEnabled(true);
    engine.SetPollInputCallback(&NDSPollInputCallback);
    engine.SetPollingEnabled(true);

    engine.RegisterManager(&RenderManager::GetInstance());
    engine.RegisterSystem(&BattleSystem::GetInstance());

    engine.InitAll();

    ae::Entity* e = engine.CreateEntity();
    HealthComponent* hc = engine.CreateComponent<HealthComponent>();
    e->AddComponent(hc);

    printf("Initial HP: %d\n", hc->GetCurrentHP());
    hc->TakeDamage(15, 0);
    engine.Tick(ae::fixed_t(1) / 60);
    printf("Final HP: %d\n", hc->GetCurrentHP());

    engine.DestroyComponent(hc);
    engine.DestroyEntity(e);
    engine.ShutdownAll();
}
#endif
