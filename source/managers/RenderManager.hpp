/**
 * @file RenderManager.hpp
 * @brief Manager for hardware specific rendering functions
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include <aegis/manager.hpp>

class RenderManager : public ae::Manager, public ae::Singleton<RenderManager>
{
  public:
    void init() override
    {
    }

    void process() override
    {
    }

    void shutdown() override
    {
    }

    /**
     * @brief Shows the specified background
     *
     * @param bgId background layer to show
     */
    void showBg(int bgId);

    /**
     * @brief Hides the specified background
     *
     * @param bgId background layer to hide
     */
    void hideBg(int bgId);

  private:
    friend class Singleton<RenderManager>;
    RenderManager() = default;
};
