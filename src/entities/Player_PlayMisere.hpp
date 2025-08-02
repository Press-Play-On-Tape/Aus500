
#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "Card.h"

void playMisere_Follow() {

    #if defined(DEBUG) && defined(DEBUG_PLAYMISERE_LEAD)
        DEBUG_PRINT(F("Misere - P"));
        DEBUG_PRINT(this->playerNumber);
        DEBUG_PRINTLN(F(" -----------------------"));
    #endif

    uint8_t partnerIdx = (this->getPlayerNumber() + 2) % 4;
    uint8_t playerBeforeIdx = (this->getPlayerNumber() + 3) % 4;
    uint8_t playerAfterIdx = (this->getPlayerNumber() + 1) % 4;

    Card *winningCard;
    Card *losingCard;

    uint8_t winningCardIdx = this->gameRound->getWinningHand(BidType::No_Trumps);
    uint8_t losingCardIdx = this->gameRound->getWinningHand();

    if (winningCardIdx != Constants::No_Card)       winningCard = this->gameRound->getHand(winningCardIdx);
    if (losingCardIdx != Constants::No_Card)        losingCard = this->gameRound->getHand(losingCardIdx);

    #if defined(DEBUG) && defined(DEBUG_PLAYMISERE_LEAD)
        DEBUG_PRINT(F("Winning card "));
        DEBUG_PRINT_CARD(winningCard->getSuit(), winningCard->getRank());
        DEBUG_PRINT(F(", Losing card "));
        DEBUG_PRINT_CARD(losingCard->getSuit(), losingCard->getRank());
        DEBUG_PRINTLN();
    #endif

    if (this->gameRound->getWinningBid_Idx() == this->getPlayerNumber()) {

        #if defined(DEBUG) && defined(DEBUG_PLAYMISERE_LEAD)
            DEBUG_PRINTLN(F("1. I called so need to lose."));
        #endif    

        if (this->playNextLowest_SmallerThan(winningCard->getSuit(), winningCard->getRank()))                                       return; // Something immediately lower than the leader ..
        else if (this->playLowest_InSuit(winningCard->getSuit()))                                                                   return; // Lowest card following suit
        else if (this->playHighest_LargerThan_AllSuit(Rank::None))                                                                  return; // Lowest card in another suit
        else if (this->playJoker())                                                                                                 return; // Play joker if you have it.

    }
    else {

        if (this->gameRound->hasPlayedCard(partnerIdx)) { 

            #if defined(DEBUG) && defined(DEBUG_PLAYMISERE_LEAD)
                DEBUG_PRINTLN(F("2. Partner has played."));
            #endif    

            if (this->gameRound->isPlayerWinningHand(partnerIdx)) {                                                                        // The single player is leaading ..

                #if defined(DEBUG) && defined(DEBUG_PLAYMISERE_LEAD)
                    DEBUG_PRINTLN(F("3. Partner is winning."));
                #endif    

                Card *biddersCard = this->gameRound->getHand(this->gameRound->getWinningBid_Idx());

                if (biddersCard->getRank() != Rank::None) {

                    #if defined(DEBUG) && defined(DEBUG_PLAYMISERE_LEAD)
                        DEBUG_PRINT(F("4. The bidder has played."));
                        DEBUG_PRINT_CARD(biddersCard->getSuit(), biddersCard->getRank());
                        DEBUG_PRINTLN();
                    #endif    
                
                    if (this->gameRound->getCardLed().getSuit() == biddersCard->getSuit()) {

                        #if defined(DEBUG) && defined(DEBUG_PLAYMISERE_LEAD)
                            DEBUG_PRINTLN(F("5. The bidder has followed suit."));
                        #endif    
                        
                        if (this->playNextLowest_SmallerThan(winningCard->getSuit(), winningCard->getRank()))                       return; // Something immediately lower than the leader ..
                        else if (this->playHighest_InSuit(winningCard->getSuit()))                                                  return; // Lowest card following suit
                        else if (this->playHighest_LargerThan_AllSuit(Rank::None))                                                  return; // Lowest card in another suit
                        else if (this->playJoker())                                                                                 return; // Play joker if you have it.

                    }
                    else {

                        #if defined(DEBUG) && defined(DEBUG_PLAYMISERE_LEAD)
                            DEBUG_PRINTLN(F("6. The bidder has not followed suit."));
                        #endif    

                        if (this->playHighest_InSuit(winningCard->getSuit()))                                                       return; // Lowest card following suit
                        else if (this->playHighest_LargerThan_AllSuit(Rank::None))                                                  return; // Lowest card in another suit
                        else if (this->playJoker())                                                                                 return; // Lowest card in another suit

                    }

                }
                else {

                    #if defined(DEBUG) && defined(DEBUG_PLAYMISERE_LEAD)
                        DEBUG_PRINTLN(F("7. The bidder has not played."));
                    #endif    

                    if (this->playNextLowest_SmallerThan(losingCard->getSuit(), losingCard->getRank()))                             return; // Something immediately lower than the leader ..
                    else if (this->playLowest_InSuit(losingCard->getSuit()))                                                        return; // Lowest card following suit
                    else if (this->playHighest_LargerThan_AllSuit(Rank::None))                                                      return; // Lowest card in another suit
                    else if (this->playJoker())                                                                                     return; // Play joker if you have it.

                }

            }
            else {
            
                #if defined(DEBUG) && defined(DEBUG_PLAYMISERE_LEAD)
                    DEBUG_PRINTLN(F("8. Partner is not winning."));
                #endif   

                if (this->playHighest_InSuit(winningCard->getSuit()))                                                               return; // Highest card following suit
                else if (this->playHighest_LargerThan_AllSuit(Rank::None))                                                          return; // Lowest card in another suit
                else if (this->playJoker())                                                                                         return; // Play joker if you have it.

            }

        }
        else {
        
            #if defined(DEBUG) && defined(DEBUG_PLAYMISERE_LEAD)
                DEBUG_PRINTLN(F("9. Partner has not played."));
            #endif    

            if (this->playNextLowest_SmallerThan(winningCard->getSuit(), winningCard->getRank()))                                   return; // Something immediately lower than the leader ..
            else if (this->playHighest_InSuit(winningCard->getSuit()))                                                              return; // Lowest card following suit
            else if (this->playHighest_LargerThan_AllSuit(Rank::None))                                                              return; // Lowest card in another suit
            else if (this->playJoker())                                                                                             return; // Play joker if you have it.

        }
    
    }

}


void playMisere_Lead() {

    #if defined(DEBUG) && defined(DEBUG_PLAYMISERE_LEAD)
        DEBUG_PRINT(F("Misere - P"));
        DEBUG_PRINT(this->playerNumber);
        DEBUG_PRINTLN(F(" -----------------------"));
    #endif

    uint8_t partnerIdx = (this->getPlayerNumber() + 2) % 4;
    uint8_t playerBeforeIdx = (this->getPlayerNumber() + 3) % 4;
    uint8_t playerAfterIdx = (this->getPlayerNumber() + 1) % 4;


    if (this->gameRound->getWinningBid_Idx() == this->getPlayerNumber()) {
    
        #if defined(DEBUG) && defined(DEBUG_PLAYMISERE_LEAD)
            DEBUG_PRINTLN(F("1. Round one, I called."));
        #endif    

        if (this->playBottom_AllSuit())                                                                                             return; // Play a bottom card if we have one ..
        else this->playLowest_AllSuit(Suit::None);                                                                                  return; // Play a small card from any other suit.
    
    }
    else {

        uint8_t idx = this->gameRound->getWinningBid_Idx() == playerBeforeIdx ? playerBeforeIdx : playerAfterIdx;


        // Try leading the bottom card of a suit that the caller has got ..

        for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {

            TriState hasSuit = this->gameRound->getHasSuitInHand(playerBeforeIdx, suit);

            if (hasSuit != TriState::False) {

                if (this->playBottom_InSuit(suit))                                                                                  return; //  Play a bottom card if we have one ..

            }

        }


        // Try leading the lowest card of a suit that the caller has got ..

        for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {

            TriState hasSuit = this->gameRound->getHasSuitInHand(playerBeforeIdx, suit);

            if (hasSuit != TriState::False) {

                if (this->playLowest_InSuit(suit))                                                                                  return; // PLay a low card in that suit

            }

        }


        // Lead the smallest card we have ..

        if (this->playLowest_AllSuit())                                                                                             return; // Try to pass control to your partner ..
        else if (this->playJoker())                                                                                                 return; // Play joker if you have it.

    }

}