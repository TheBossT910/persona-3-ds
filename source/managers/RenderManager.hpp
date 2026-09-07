/**
 * @file RenderManager.hpp
 * @brief Manager for hardware specific rendering functionse
 * @author Gregory Munro (ggmini)
 */

#pragma once
#include "types/RenderTypes.hpp"
#include <aegis/manager.hpp>

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

    void initialize3DView(View3DConfig config);

    void renderDisplayList(const void* list);

  private:
    friend class Singleton<RenderManager>;
    RenderManager() = default;
};
