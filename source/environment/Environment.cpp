#include "Environment.h"
#include "core/globals.h"
#include <math.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

/**
 * @brief Converts a raw texture dimension in pixels to the corresponding
 *        libnds TEXTURE_SIZE_* enum value.
 *
 * @param size Texture width/height in pixels. Expected to be one of the
 *             power-of-two values 8, 16, 32, 64, 128, 256, 512, or 1024.
 * @return The matching TEXTURE_SIZE_* constant, or TEXTURE_SIZE_8 as a
 *         fallback if @p size does not match a supported value (a message
 *         is also printed to the debug console in that case).
 */
static int textureSizeEnum(int size)
{
    switch (size)
    {
    case 8:
        return TEXTURE_SIZE_8;
    case 16:
        return TEXTURE_SIZE_16;
    case 32:
        return TEXTURE_SIZE_32;
    case 64:
        return TEXTURE_SIZE_64;
    case 128:
        return TEXTURE_SIZE_128;
    case 256:
        return TEXTURE_SIZE_256;
    case 512:
        return TEXTURE_SIZE_512;
    case 1024:
        return TEXTURE_SIZE_1024;
    default:
        printf("Invalid texture size %d\n", size);
        return TEXTURE_SIZE_8;
    }
}

Environment::Environment() : dbEntry(nullptr)
{
    for (int i = 0; i < MAX_ENVIRONMENT_TEXTURES; i++)
    {
        displayLists[i] = nullptr;
        dlSizes[i] = 0;
        textureIDs[i] = 0;
    }
}

bool Environment::load(const EnvironmentDbEntry* entry,
                       std::array<const unsigned int*, MAX_ENVIRONMENT_TEXTURES> bitmaps)
{
    cleanup();

    // Guard against a missing/oversized db entry before touching it
    if (!entry || entry->textureCount > MAX_ENVIRONMENT_TEXTURES)
    {
        printf("EnvironmentDbEntry textures exceeds MAX_ENVIRONMENT_TEXTURES");
        return false;
    }

    dbEntry = entry;

    const std::string fullBinaryPath = fatBasePath + "environments/" + entry->name + "/" + entry->binaryFile;

    if (Globals::enableDebugPrint)
    {
        printf("Environment::load opening '%s'\n", fullBinaryPath.c_str());
    }

    FILE* file = fopen(fullBinaryPath.c_str(), "rb");
    if (!file)
    {
        cleanup();
        return false;
    }

    char magic[4];
    if (fread(magic, 1, 4, file) != 4)
    {
        fclose(file);
        cleanup();
        return false;
    }

    if (magic[0] != 'E' || magic[1] != 'N' || magic[2] != 'V' || magic[3] != '1')
    {
        fclose(file);
        cleanup();
        return false;
    }

    u32 groupCount = 0;
    if (fread(&groupCount, sizeof(u32), 1, file) != 1)
    {
        fclose(file);
        cleanup();
        return false;
    }

    if (groupCount > (u32)entry->textureCount)
    {
        fclose(file);
        cleanup();
        return false;
    }

    // Display list load
    for (u32 i = 0; i < groupCount; i++)
    {
        if (fread(&dlSizes[i], sizeof(u32), 1, file) != 1)
        {
            fclose(file);
            cleanup(); // frees any displayLists[0..i) already allocated this call
            return false;
        }

        displayLists[i] = nullptr;

        if (dlSizes[i] > 0)
        {
            displayLists[i] = (u32*)malloc((dlSizes[i] + 1) * sizeof(u32));

            if (!displayLists[i])
            {
                fclose(file);
                cleanup();
                return false;
            }

            displayLists[i][0] = dlSizes[i];

            if (fread(&displayLists[i][1], sizeof(u32), dlSizes[i], file) != dlSizes[i])
            {
                fclose(file);
                cleanup();
                return false;
            }
        }
    }

    fclose(file);

    // Texture upload
    for (int i = 0; i < entry->textureCount; i++)
    {
        textureIDs[i] = 0;

        if (bitmaps.empty() || !bitmaps[i])
            continue;

        glGenTextures(1, &textureIDs[i]);
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     textureSizeEnum(entry->textures[i].width),
                     textureSizeEnum(entry->textures[i].height),
                     0,
                     TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
                     bitmaps[i]);
    }

    return true;
}

void Environment::draw()
{
    if (!dbEntry)
        return;

    for (int i = 0; i < dbEntry->textureCount; i++)
    {
        if (!textureIDs[i])
            continue;

        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        if (displayLists[i])
        {
            // Guard against corrupted DL pointers
            glCallList(displayLists[i]);
        }

        while (GFX_BUSY)
            ;
    }
}

void Environment::drawBillboards(bool faceCamera, float camX, float camY, float camZ)
{
    if (!dbEntry || dbEntry->billboardCount == 0)
        return;

    int currentSlot = -1;
    bool inQuads = false;

    for (int i = 0; i < dbEntry->billboardCount; i++)
    {
        const auto& bb = dbEntry->billboards[i];

        if (bb.texSlot >= dbEntry->textureCount)
            continue;

        if (!textureIDs[bb.texSlot])
            continue;

        if (bb.texSlot != currentSlot)
        {
            if (inQuads)
            {
                glEnd();
                inQuads = false;
            }

            while (GFX_BUSY)
                ;

            glBindTexture(GL_TEXTURE_2D, textureIDs[bb.texSlot]);
            currentSlot = bb.texSlot;
        }

        if (!inQuads)
        {
            glBegin(GL_QUADS);
            inQuads = true;
        }

        v16 rX = 4096, rY = 0, rZ = 0;
        v16 uX = 0, uY = 4096, uZ = 0;

        if (faceCamera)
        {
            float bx = (float)bb.x / 4096.0f;
            float bz = (float)bb.z / 4096.0f;

            float dx = camX - bx;
            float dz = camZ - bz;

            float dist = sqrtf(dx * dx + dz * dz);

            if (dist > 0.001f)
            {
                dx /= dist;
                dz /= dist;
            }

            rX = (v16)(dz * 4096.0f);
            rZ = (v16)(-dx * 4096.0f);
        }

        v16 rx = mulf32(rX, bb.halfWidth);
        v16 ry = mulf32(rY, bb.halfWidth);
        v16 rz = mulf32(rZ, bb.halfWidth);

        v16 ux = mulf32(uX, bb.halfHeight);
        v16 uy = mulf32(uY, bb.halfHeight);
        v16 uz = mulf32(uZ, bb.halfHeight);

        glTexCoord2t16(bb.u0, bb.v1);
        glVertex3v16(bb.x - rx - ux, bb.y - ry - uy, bb.z - rz - uz);

        glTexCoord2t16(bb.u1, bb.v1);
        glVertex3v16(bb.x + rx - ux, bb.y + ry - uy, bb.z + rz - uz);

        glTexCoord2t16(bb.u1, bb.v0);
        glVertex3v16(bb.x + rx + ux, bb.y + ry + uy, bb.z + rz + uz);

        glTexCoord2t16(bb.u0, bb.v0);
        glVertex3v16(bb.x - rx + ux, bb.y - ry + uy, bb.z - rz + uz);
    }

    if (inQuads)
        glEnd();
}

void Environment::cleanup()
{
    for (int i = 0; i < MAX_ENVIRONMENT_TEXTURES; i++)
    {
        if (displayLists[i])
        {
            free(displayLists[i]);
            displayLists[i] = nullptr;
        }

        dlSizes[i] = 0;

        if (textureIDs[i])
        {
            // Previously this only zeroed the id without ever releasing the
            // underlying GPU texture slot, leaking VRAM texture memory on
            // every single room transition.
            glDeleteTextures(1, &textureIDs[i]);
            textureIDs[i] = 0;
        }
    }

    dbEntry = nullptr;
}
