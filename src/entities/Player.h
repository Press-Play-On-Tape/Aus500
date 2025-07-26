#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "../utils/Utils.h"
#include "Card.h"
#include "SuitPile.h"
#include "GameRound.h"

struct Player {

    public:

        GameRound *gameRound;
        Card cardJustPlayed;

    private:
    
        uint8_t playerNumber;
        uint8_t cardCount;
        Card cards[13];
        bool playing = true;

    public:

        uint8_t getPlayerNumber()                       { return this->playerNumber; }
        uint8_t getCardCount()                          { return this->cardCount; }
        bool isPlaying()                                { return this->playing; }
        Card &getCard(uint8_t idx)                      { return this->cards[idx]; }

        void setPlayerNumber(uint8_t val)               { this->playerNumber = val; }
        void setCardCount(uint8_t val)                  { this->cardCount = val; }
        void setPlaying(bool val)                       { this->playing = val; }
        void setCard(uint8_t idx, Card &card)           { this->cards[idx] = card; }

        void reset() {
        
            for (uint8_t i = 0; i < 13; i++) {
            
                this->cards[i].reset();

            }

            this->cardCount = 0;
            this->playing = true;

        }

        uint8_t getSelectedCount() {
        
            uint8_t count = 0;

            for (uint8_t i = 0; i < this->cardCount; i++) {

                if (this->cards[i].isSelected()) count++;
            }

            return count;

        }

        #include "Player_DecideBid.hpp"
        #include "Player_DecideBid_SuitHand.hpp"
        #include "Player_DecideBid_TrumpHand.hpp"
        #include "Player_DecideBid_Misere.hpp"
        #include "Player_PlayCard.hpp"
        #include "Player_Utils.hpp"
        #include "Player_HandleKitty.hpp"
        #include "Player_PlaySuit.hpp"
        #include "Player_PlayNoTrumps.hpp"
        #include "Player_PlayMisere.hpp"

};
