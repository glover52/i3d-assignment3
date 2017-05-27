#pragma once

#include "player.h"
#include "level.h"
#include "controls.h"
#include "mesh.h"
#include "camera.h"
#include "osd.h"
#include "skybox.h"

/*
 * All of the global state for our main functions is declared here
 */
typedef struct {
    Player player;
    Level level;
    Controls controls;
    Camera camera;
    DrawingFlags drawingFlags;
    bool halt;
    OSD osd;
    Skybox skybox;
} Globals;
