#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/utils/SpritesU.hpp"


void title_Init() {

    gameState = GameState::Title_OptPlay;
    titleCounter = 0;
    game.resetFrameCount();

}

void title_Update() {

    game.incFrameCount();

    uint8_t justPressed = getJustPressedButtons();
    uint8_t pressed = getPressedButtons();

    if ((titleCounter == 24) && (justPressed & A_BUTTON)) {

        a.initRandomSeed(); 

        gameState = GameState::Play_Init;
        game.setPrevGameState(gameState);

    }

    if (titleCounter < 24) titleCounter++;
      
}

void title(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {

    if (a.needsUpdate()) title_Update();

    uint8_t currentPlane = a.currentPlane();

    uint8_t idx = 0;
    if ((game.getFrameCount() % 220) < 64) idx = game.getFrameCount() % 220 / 2;
    SpritesU::drawOverwriteFX(0, 0, Images::Title500, (idx * 3) + currentPlane);

}
