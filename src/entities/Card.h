#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "../utils/Utils.h"
#include "Card.h"

struct Card {

    public:

        Suit suit;
        Rank rank;
        bool selected = false;

        Suit getSuit()                      { return this->suit; }
        Rank getRank()                      { return this->rank; }
        bool isSelected()                   { return this->selected; }

        void setSuit(Suit val)              { this->suit = val; }
        void setRank(Rank val)              { this->rank = val; }
        void setSelected(bool val)          { this->selected = val; }

        void reset() {
        
            this->setSuit(Suit::None);
            this->setRank(Rank::None);
            this->setSelected(false);
            
        }

        uint8_t getCardIndex() {

            if (this->getRank() == Rank::Joker) {

                switch (this->getSuit()) {

                    case Suit::Spades ... Suit::Hearts:
                        return (44 + static_cast<uint8_t>(this->getSuit())) * 3;
                        break;

                    default:
                        return 0;

                }

            }

            return (3 * ((static_cast<uint8_t>(this->getSuit()) * 11) + (static_cast<uint8_t>(this->getRank()) - 4)));
            
        }

        Suit getSuit(Suit trumps) {
        
            if (this->getRank() == Rank::Joker) return trumps;

            if (isLeftBower(trumps))    { 
           
                if (trumps == Suit::Spades)     return Suit::Clubs;
                if (trumps == Suit::Clubs)      return Suit::Spades;
                if (trumps == Suit::Diamonds)   return Suit::Hearts;
                if (trumps == Suit::Hearts)     return Suit::Diamonds;
            
            }

            return this->suit; 
         
        }

        Rank getRank(Suit trumps) {
        
            if (isRightBower(trumps))   { return Rank::Right_Bower; }
            if (isLeftBower(trumps))    { return Rank::Left_Bower; }

            return this->rank; 
         
        }

        uint8_t getSortValue(BidType bidType, Suit trumps, uint8_t seq[4]) {

            #if defined(DEBUG) && defined(DEBUG_SORT) && defined(DEBUG_SORT_DETAIL)
                DEBUG_PRINT(F("Card "));
                DEBUG_PRINT_CARD(this.getRanks(), this.getSuit);
                DEBUG_PRINT_SPACE();
            #endif

            switch (bidType) {
            
                case BidType::None:
                case BidType::Pass:
                case BidType::No_Trumps:
                case BidType::Misere:
                #ifdef OPEN_MISERE
                case BidType::Open_Misere:
                #endif

                    if (rank == Rank::Joker) {

                        #if defined(DEBUG) && defined(DEBUG_SORT) && defined(DEBUG_SORT_DETAIL)
                            DEBUG_PRINTLN(Constants::Sort_Joker);
                        #endif

                        return Constants::Sort_Joker;

                    }

                    else if (suit == Suit::None) {

                        #if defined(DEBUG) && defined(DEBUG_SORT) && defined(DEBUG_SORT_DETAIL)
                            DEBUG_PRINTLN(Constants::Sort_None);
                        #endif

                        return Constants::Sort_None;
                        
                    }                    

                    return (seq[static_cast<uint8_t>(this->suit)] * 20) + (uint8_t)getRank(Suit::None);

                default:

                    if (rank == Rank::Joker) {

                        #if defined(DEBUG) && defined(DEBUG_SORT) && defined(DEBUG_SORT_DETAIL)
                            DEBUG_PRINTLN((seq[static_cast<uint8_t>(trumps)] * 20) + (uint8_t)getRank(trumps));
                        #endif

                        return (seq[static_cast<uint8_t>(trumps)] * 20) + (uint8_t)getRank(trumps);

                    }

                    else if (suit == Suit::None) {

                        #if defined(DEBUG) && defined(DEBUG_SORT) && defined(DEBUG_SORT_DETAIL)
                            DEBUG_PRINTLN(Constants::Sort_None);
                        #endif

                        return Constants::Sort_None;
                        
                    }                    
                    else if (isLeftBower(trumps)) {

                        #if defined(DEBUG) && defined(DEBUG_SORT) && defined(DEBUG_SORT_DETAIL)
                            DEBUG_PRINTLN((seq[static_cast<uint8_t>(getTrump_AltSuit(this->suit))] * 20) + (uint8_t)getRank(trumps));
                        #endif

                        return (seq[static_cast<uint8_t>(getTrump_AltSuit(this->suit))] * 20) + (uint8_t)getRank(trumps);

                    }
                    else {
                      
                        #if defined(DEBUG) && defined(DEBUG_SORT) && defined(DEBUG_SORT_DETAIL)
                            DEBUG_PRINTLN((seq[static_cast<uint8_t>(this->suit)] * 20) + (uint8_t)getRank(trumps));
                        #endif  

                        return (seq[static_cast<uint8_t>(this->suit)] * 20) + (uint8_t)getRank(trumps);

                    }

                    break;                

            }

        }


        bool isTrump(Suit trumps) {
        
            if (this->getSuit() == trumps) {
                return true;
            }
            else if (isLeftBower(trumps)) {
                return true;
            }
            else if (this->getRank() == Rank::Joker) {
                return true;
            }

            return false;

        }

        bool isRightBower(Suit trumpSuit) const {

            if (this->getRank() != Rank::Jack) return false;

            return (trumpSuit == this->getSuit());

        }

        bool isLeftBower(Suit trumpSuit) const {

            if (this->getRank() != Rank::Jack) return false;

            Suit cardSuit = this->getSuit();

            if (trumpSuit == Suit::Spades && cardSuit == Suit::Clubs)     return true;
            if (trumpSuit == Suit::Clubs && cardSuit == Suit::Spades)     return true;
            if (trumpSuit == Suit::Diamonds && cardSuit == Suit::Hearts)  return true;
            if (trumpSuit == Suit::Hearts && cardSuit == Suit::Diamonds)  return true;

            return false;

        }

};

