#include "RenderManager.hpp"
#include <nds.h>

void RenderManager::initialize3DView(View3DConfig config)
{
    glInit();

    glEnable(config.settings);
    glViewport(config.viewportLeft, config.viewportTop, config.viewportRight, config.viewportBottom);
    glClearColor(config.clearColorRed, config.clearColorGreen, config.clearColorBlue, config.clearColorAlpha);
    glClearDepth(config.clearDepth.raw_value());

    glClearPolyID(config.clearPolyID);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(config.fov, config.aspect.raw_value(), config.nearPlane.raw_value(), config.farPlane);

    if (config.outlineColor != -1)
    {
        glSetOutlineColor(0, config.outlineColor);
    }

    if (config.fogRed != -1)
    {
        glFogColor(config.fogRed, config.fogGreen, config.fogBlue, config.fogAlpha);

        // How much depth difference there is between table entries
        glFogShift(config.shift);
        // Depth at which the fog starts
        glFogOffset(config.depth);

        // generate linear density table
        uint8_t density = 0;
        for (uint8_t i = 0; i < 32; ++i) // it has 32 steps
        {
            glFogDensity(i, density);
            // exponentially increase mass the furthur back the fog is
            density += (config.mass * i) >> 2;

            // entries are 7 bit, so cap the density to 127
            if (density > 127)
            {
                density = 127;
            }
        }
    }

    glPolyFmt(config.polyParams);

    glMatrixMode(GL_MODELVIEW);
}

void RenderManager::renderDisplayList(const void* list)
{
    glCallList(list);
}
