#pragma once
#include "core/structs.h"

class SaveController
{
  public:
    static void create();
    static void destroy();
    static SaveController* getInstance();

    // returns true/false on success/fail
    bool write();
    bool read();

  private:
    SaveController() {};
    ~SaveController() {};
    static SaveController* instance;
};
