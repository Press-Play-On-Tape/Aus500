#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "Card.h"
#include "Player.h"
#include "GameRound.h"

struct Game {

    private:

        uint16_t frameCount = 0;
        uint8_t selecteCard = 255;
        GameState prevGameState = GameState::SplashScreen_Start;

    public:

        Player players[4];
        Deck deck;
        GameRound *gameRound;

        uint16_t getFrameCount()                        { return this->frameCount; }
        uint8_t getSelectedCard()                       { return this->selecteCard; }
        GameState getPrevGameState()                    { return this->prevGameState; }

        void setFrameCount(uint16_t val)                { this->frameCount = val; }
        void setSelectedCard(uint8_t val)               { this->selecteCard = val; }
        void setPrevGameState(GameState val)            { this->prevGameState = val; }


        void incFrameCount() {

            this->frameCount++;

        }

        void resetFrameCount() {

            this->frameCount = 0;
            
        }


};