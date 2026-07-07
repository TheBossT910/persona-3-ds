#include "SaveController.h"
#include "core/globals.h"
#include <stdio.h>
#include <stdlib.h>

SaveController* SaveController::instance = nullptr;

void SaveController::create()
{
    if (instance == nullptr)
    {
        instance = new SaveController();
    }
}

void SaveController::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }
    instance = nullptr;
}

SaveController* SaveController::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }
    return instance;
}

// save the current data into a .sav file (text file)
bool SaveController::write()
{
    // open file
    std::string tempPath = fatBasePath + "save/temp.sav";
    std::string savePath = fatBasePath + "save/save.sav";

    FILE* file = fopen(tempPath.c_str(), "wb");
    if (!file)
    {
        return false;
    }

    // write data
    fwrite(&saveData, sizeof(Save), 1, file);

    // close file
    fclose(file);

    // replace old save with new save
    remove(savePath.c_str());
    rename(tempPath.c_str(), savePath.c_str());

    return true;
}

// load the save data from .sav file (text file) into saveData struct
bool SaveController::read()
{
    // open file
    std::string savePath = fatBasePath + "save/save.sav";
    FILE* file = fopen(savePath.c_str(), "rb");
    if (!file)
    {
        return false;
    }

    // read data
    fread(&saveData, sizeof(Save), 1, file);

    // close file
    fclose(file);

    return true;
}
