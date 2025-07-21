#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "Card.h"

struct Bid {

    private:
    
        uint8_t playerIdx = 0;
        BidType type = BidType::Pass;
        Suit suit = Suit::None;       
        uint8_t level = 0;                              // e.g., 6 for "6 Spades", 8 for "8 No Trumps"
        uint16_t bidScore = 0;                          // The internal evaluation score for the hand

    public:

        Suit getSuit()                                  { return this->suit; }
        BidType getBidType()                            { return this->type; }
        uint8_t getLevel()                              { return this->level; }
        uint8_t getPlayerIdx()                          { return this->playerIdx; }
        uint16_t getBidScore()                          { return this->bidScore; }

        void setSuit(Suit val)                          { this->suit = val; }
        void setBidType(BidType val)                    { this->type = val; }
        void setLevel(uint8_t val)                      { this->level = val; }
        void setBidScore(uint16_t val)                  { this->bidScore = val; }

        void reset(uint8_t playerIdx) {
        
            this->playerIdx = playerIdx;
            this->type = BidType::None;
            this->suit = Suit::None;
            this->level = 0;
            this->bidScore = 0;
            
        }
              
        uint16_t getScore() {
        
            switch (this->getBidType()) {
            
                case BidType::No_Trumps:
                    return (((this->getLevel() - 6) * 100) + 120);

                case BidType::Suit:

                    switch (this->getSuit()) {
                    
                        case Suit::Spades:
                            return (((this->getLevel() - 6) * 100) + 40);
                    
                        case Suit::Clubs:
                            return (((this->getLevel() - 6) * 100) + 60);
                    
                        case Suit::Diamonds:
                            return (((this->getLevel() - 6) * 100) + 80);
                    
                        case Suit::Hearts:
                            return (((this->getLevel() - 6) * 100) + 100);

                    }

                    break;

                case BidType::Misere:
                    return 250;

                #ifdef OPEN_MISERE
                    case BidType::Open_Misere:
                        return 500;
                #endif

                default:
                    return 0;

            }
            
        }


        bool isHigherThan(const Bid& other) const {

            return this->getScore() > other.getScore();

        }


        uint8_t getImageIndex() {
        
            switch (this->getBidType()) {

                case BidType::Suit:
                    return (static_cast<uint8_t>(this->suit) * 5) + (this->level - 6);

                case BidType::No_Trumps:
                    return 20 + (this->level - 6);

                case BidType::Pass:
                    return 26;

                case BidType::Misere:
                    return 27;

            }

        }

};