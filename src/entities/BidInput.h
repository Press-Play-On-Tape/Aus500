#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"

struct BidInput {

    private:
    
        BidMode mode = BidMode::Level;
        uint8_t suit = 0;
        uint8_t level = 0; 

    public:

        const BidMode getMode()                     { return this->mode; }
        const uint8_t getSuit()                     { return this->suit; }
        const uint8_t getLevel()                    { return this->level; }

        void setMode(BidMode val)                   { this->mode = val; }
        void setSuit(uint8_t val)                   { this->suit = val; }
        void setLevel(uint8_t val)                  { this->level = val; }

        void reset() {

            mode = BidMode::Level;
            suit = 0;
            level = 0; 

        }

        void incMode() {
            this->mode++;
        }

        void decMode() {
            this->mode--;
        }

        void incSuit() {
            this->suit++;
        }

        void decSuit() {
            this->suit--;
        }

        void incLevel() {
            this->level++;
        }

        void decLevel() {
            this->level--;
        }

};

