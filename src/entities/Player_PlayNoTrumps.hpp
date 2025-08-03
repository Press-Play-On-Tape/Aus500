
#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "Card.h"

void playNoTrumps_Follow() {

    #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
    DEBUG_PRINT(F("No Trumps - P"));
    DEBUG_PRINT(this->playerNumber);
    DEBUG_PRINTLN(F(" -----------------------"));
    #endif
    
    uint8_t partnerIdx = (this->getPlayerNumber() + 2) % 4;
    uint8_t playerBeforeIdx = (this->getPlayerNumber() + 3) % 4;
    uint8_t playerAfterIdx = (this->getPlayerNumber() + 1) % 4;

    Card &largestCardInPlay = this->gameRound->getLargestCardInPlay(Suit::None);
    Card &cardLed = this->gameRound->getCardLed();
    Suit cardLedSuit = cardLed.getSuit();

    if (this->gameRound->hasPlayedCard(partnerIdx)) {                                                                                   // Has your partner played? 

        Card *partnerCard = this->gameRound->getHand(partnerIdx);

        #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
            DEBUG_PRINT(F("1. Partner has played "));
            DEBUG_PRINT_CARD(partnerCard->getSuit(), partnerCard->getRank());
            DEBUG_PRINTLN();
        #endif

        if (this->gameRound->isPlayerWinningHand(partnerIdx)) {                                                                         // Are they winning?

            #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                DEBUG_PRINTLN(F("2. Partner is winning."));
            #endif

            if (this->hasSuit(cardLedSuit)) {

                #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                    DEBUG_PRINT(F("3. Player has suit "));
                    DEBUG_PRINT_SUIT(cardLedSuit);
                    DEBUG_PRINTLN();
                #endif

                if (this->gameRound->hasPlayedCard(playerAfterIdx)) {                                                                   // If the player after you has played, then your partner will win.

                    #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                        DEBUG_PRINT(F("4. Player small suit "));
                        DEBUG_PRINT_SUIT(cardLedSuit);
                        DEBUG_PRINTLN();
                    #endif

                    if (this->playLowest_InSuit(cardLedSuit))                                                                  return; // Follow suit ..

                }
                else {

                    uint8_t noOfLargeCards = getNumberOfCards_InSuit(partnerCard->getSuit(), partnerCard->getRank());
                    TriState nextPlayerHasSuit = this->gameRound->canPlayerFollowSuit(cardLedSuit, playerAfterIdx);

                    if (noOfLargeCards >= 1 && nextPlayerHasSuit != TriState::False) {
                        
                        #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                            DEBUG_PRINT(F("5. Competitor plays next, has suit."));
                            DEBUG_PRINT_SUIT(cardLedSuit);
                            DEBUG_PRINTLN();
                        #endif

                        if (this->playHighest_InSuit(cardLedSuit))                                                              return;
                        
                    }
                    else {
                        
                        #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                            DEBUG_PRINT(F("6. Competitor plays next, does not have suit."));
                            DEBUG_PRINT_SUIT(cardLedSuit);
                            DEBUG_PRINTLN();
                        #endif

                        if (this->playLowest_InSuit(cardLedSuit))                                                               return;       
                
                    }
               
                }

            }
            else {

                #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                    DEBUG_PRINT(F("7. Player does not have suit "));
                    DEBUG_PRINT_SUIT(cardLedSuit);
                    DEBUG_PRINTLN();
                #endif
            
                Suit shortSuit = this->canShortSuit();                                                                                  // Can you short suit yourself in any suit?                             

                #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                    DEBUG_PRINT(F("8. Player can short suit "));
                    DEBUG_PRINT_SUIT(shortSuit);
                    DEBUG_PRINTLN();
                #endif

                if (shortSuit != Suit::None) {                                                                                          
                    this->playLowest_InSuit(shortSuit);                                                                         return; // Follow suit ..
                }
                else {
                    if (this->playLowest_AllSuit())                                                                             return; // Play the lowest card you have.
                }

            }

            this->playJoker(cardLedSuit);                                                                                       return;

        }
        else {  // Partner is not winning ..

            #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                DEBUG_PRINTLN(F("9. Partner is not winning."));
            #endif

            if (this->hasSuit(cardLedSuit)) { // Can you follow suit?
            
                #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                    DEBUG_PRINT(F("10. Player has suit "));
                    DEBUG_PRINT_SUIT(cardLedSuit);
                    DEBUG_PRINTLN();                    
                #endif

                if (this->gameRound->isLastPlayer(this->getPlayerNumber())) {

                    #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                        DEBUG_PRINT(F("11. Player is last player of round "));
                        DEBUG_PRINTLN((uint8_t)cardLedSuit);
                    #endif
                    
                    if (this->playHighest_LargerThan_InSuit(cardLedSuit, largestCardInPlay.getRank()))                          return; // Win or at least force th other player to play big ..
                    else (this->playLowest_InSuit(cardLedSuit));                                                                return; // Follow suit ..

                }
                else { // Not last player ..

                    #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                        DEBUG_PRINTLN(F("12. Player is not last player of round."));
                    #endif
                    
                    #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                        DEBUG_PRINT(F("13. Count of high cards "));
                        DEBUG_PRINTLN(this->gameRound->getUnplayedCountofHigherCards_InSuit(cardLed));
                    #endif

                    if (this->gameRound->getUnplayedCountofHigherCards_InSuit(cardLed) > 4) {

                        if (this->playNextHighest_LargerThan_InSuit(cardLedSuit, largestCardInPlay.getRank()))                  return; // Yes, play a card slightly larger than current, winning card.
                        else if (this->playLowest_InSuit(cardLedSuit))                                                          return; // No, play the largest on-suit card you have.
                        else if (this->playLowest_AllSuit())                                                                    return; // Should not get here as we have the suit.

                    }
                    else {

                        if (this->playHighest_LargerThan_InSuit(cardLedSuit, largestCardInPlay.getRank()))                      return; // No, play the largest on-suit card you have bigger than winning card.
                        else if (this->playLowest_InSuit(cardLedSuit))                                                          return; // No, play the smallest on-suit card you have.
                        else if (this->playLowest_AllSuit())                                                                    return; // Should not get here as we have the suit.

                    }

                }

                this->playJoker(cardLedSuit);                                                                                   return;
                
            }
            else {                                                                                                                      // Can not follow suit.
                   
                #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                    DEBUG_PRINT(F("14. Player does not have suit "));
                    DEBUG_PRINT_SUIT(cardLedSuit);
                    DEBUG_PRINTLN();
                #endif

                if (this->playJoker(cardLedSuit))                                                                               return; // Win this hand with the Joker.
            
                Suit shortSuit = this->canShortSuit();                                                                                  // Can you short suit yourself in any suit?

                #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                    DEBUG_PRINT(F("15. Player can short suit "));
                    DEBUG_PRINT_SUIT(shortSuit);
                    DEBUG_PRINTLN();
                #endif

                if (shortSuit != Suit::None) {
                    this->playLowest_InSuit(shortSuit);                                                                         return; // Follow suit ..
                }
                else {
                    if (this->playLowest_AllSuit())                                                                             return; // Play the lowest card you have.
                }

            }

        }

        this->playJoker(cardLedSuit);                                                                                           return; // Can we even get this far?

    }
    else {                                                                                                                              // Partner has not played ..

        #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
            DEBUG_PRINTLN(F("16. Partner has not played."));
        #endif
        
        if (this->hasSuit(cardLedSuit)) {                                                                                               // Can you follow suit?

            #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                DEBUG_PRINT(F("17. Player has suit "));
                DEBUG_PRINT_SUIT(cardLedSuit);
                DEBUG_PRINTLN();
            #endif

            if (this->playNextHighest_LargerThan_InSuit(cardLedSuit, largestCardInPlay.getRank()))                              return; // Yes, play a card slightly larger than current, winning card.
            else if (this->playLowest_InSuit(cardLedSuit))                                                                      return; // No, play the largest on-suit card you have.
            else if (this->playLowest_AllSuit())                                                                                return; // No, play the smallest off-suit card you have.

        }
        else {

            #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                DEBUG_PRINT(F("18. Player does not have suit "));
                DEBUG_PRINT_SUIT(cardLedSuit);
                DEBUG_PRINTLN();
            #endif

            TriState doesPartnerHaveSuit = this->gameRound->canPlayerFollowSuit(cardLedSuit, partnerIdx);

            if (doesPartnerHaveSuit == TriState::False) {

                #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                    DEBUG_PRINT(F("19. Partner does not have suit "));
                    DEBUG_PRINT_SUIT(cardLedSuit);
                    DEBUG_PRINTLN();
                #endif

                if (this->playJoker(cardLedSuit))                                                                               return; // Win this hand with the Joker.

            }
            else {

                #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                    DEBUG_PRINT(F("20. Partner has suit "));
                    DEBUG_PRINT_SUIT(cardLedSuit);
                    DEBUG_PRINTLN();
                #endif

            }



            Suit shortSuit = this->canShortSuit();                                                                                      // Can you short suit yourself in any suit?

            if (shortSuit != Suit::None) {                                                                                              // if the short suit is the highest card in suit do not short.

                uint8_t cardIdx = this->getLowest_InSuit(shortSuit);
                uint8_t topIdx = this->getTop_InSuit(shortSuit);

                if (topIdx = cardIdx) {                                                                                                 // If the only card in the hand is also the top card then do not short suit ..

                    shortSuit = Suit::None;

                }
                
            }
            
            if (shortSuit != Suit::None) {

                #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                    DEBUG_PRINT(F("21. Player can short suit "));
                    DEBUG_PRINT_SUIT(shortSuit);
                    DEBUG_PRINTLN();
                #endif

                this->playLowest_InSuit(shortSuit);                                                                             return; // Follow suit ..
            }
            else {

                #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_FOLLOW)
                    DEBUG_PRINT(F("22. Player can not short suit."));
                #endif

                if (this->playLowest_AllSuit())                                                                                 return; // Play the lowest card you have.
                
            }        
            
        }
               
    }
           
    this->playJoker(cardLedSuit);                                                                                               return; // Can we even get this far?

}               


bool playNoTrumps_Lead() {

    #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_LEAD)
    DEBUG_PRINT(F("No Trumps - P"));
    DEBUG_PRINT(this->playerNumber);
    DEBUG_PRINTLN(F(" -----------------------"));
    #endif

    uint8_t partnerIdx = (this->getPlayerNumber() + 2) % 4;
    uint8_t playerBeforeIdx = (this->getPlayerNumber() + 3) % 4;
    uint8_t playerAfterIdx = (this->getPlayerNumber() + 1) % 4;

    Card &largestCardInPlay = this->gameRound->getLargestCardInPlay(Suit::None);

    Suit suit1 = this->gameRound->getHand(playerBeforeIdx)->getSuit(); // played by competitor 1
    Suit suit2 = this->gameRound->getHand(playerAfterIdx)->getSuit(); // played by competitor 2
    Suit partnerCalledSuit = this->gameRound->getBid(partnerIdx).getSuit();

    #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_LEAD)
        DEBUG_PRINTLN(F("1. Player has top card?"));
    #endif

    if (this->playTop_AllSuit())                                                                                                return; // Play a good lead ..

    #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_LEAD)
        DEBUG_PRINTLN(F("2. No top card played."));
    #endif

    #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_LEAD)
        DEBUG_PRINT(F("3. Check for 2nd highest and low card, partner called suit "));
        DEBUG_PRINT_SUIT(partnerCalledSuit);
        DEBUG_PRINTLN();
    #endif

    TriState doesPartnerHaveSuit = this->gameRound->canPlayerFollowSuit(partnerCalledSuit, partnerIdx);                                 // Do you have the second highest card in a suit and a small card of same suit and your partner has / maybe the suit? 

    #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_LEAD)
        DEBUG_PRINT(F("Partner called "));
        DEBUG_PRINT_SUIT(partnerCalledSuit);
        DEBUG_PRINT(F(", holds "));
        DEBUG_PRINT_TRISTATE(doesPartnerHaveSuit);
        DEBUG_PRINTLN();
    #endif

    if (partnerCalledSuit == Suit::None) {
        partnerCalledSuit = Suit::Spades;
    }

    if (doesPartnerHaveSuit == TriState::False) {
        partnerCalledSuit = Suit::Spades;
    }

    #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_LEAD)
        DEBUG_PRINT(F("3. Check for 2nd highest and low card, partner has suit "));
    #endif

    for (uint8_t s = static_cast<uint8_t>(partnerCalledSuit); s < static_cast<uint8_t>(partnerCalledSuit) + 4; s++) {                   // Do you have the second highest card in a suit and a small card of same suit and your partner has / maybe the suit? 

        Suit suit = static_cast<Suit>(s % 4);

        TriState doesPartnerHaveSuit = this->gameRound->canPlayerFollowSuit(suit, partnerIdx);

        #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_LEAD)
            DEBUG_PRINT_SUIT(suit);
            DEBUG_PRINT_TRISTATE(doesPartnerHaveSuit);
            DEBUG_PRINT_SPACE();
        #endif

        if (doesPartnerHaveSuit == TriState::True || doesPartnerHaveSuit == TriState::Maybe) {

            if (this->playLowest_WithSecondHighest_InSuit(suit)) {
                        
                #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_LEAD)
                    DEBUG_PRINTLN(F("."));
                #endif
                                                                                                                                return; // Play the card!
            }

        }

    }

    #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_LEAD)
        DEBUG_PRINTLN(F("."));
    #endif

    Suit suitPartnerCalled = this->gameRound->suitPlayerCalled(partnerIdx);

    #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_LEAD)
        DEBUG_PRINT(F("4. Partner called "));
        DEBUG_PRINT_SUIT(suitPartnerCalled);
        DEBUG_PRINTLN(F(", play low card."));
    #endif

    if (suitPartnerCalled != Suit::None) {                                                                                              // Did your partner call a suit?
    
        if (this->playLowest_InSuit(suitPartnerCalled))                                                                         return; // No, play the lowest of the suit your partner called hoping they will play back.

    }

    Suit suit = Suit::None;

    if (this->getCard(Rank::Joker) != Constants::No_Card) {                                                                             // We might play the joker if we have one. What suiit will we call it?
        
        uint8_t idx = this->getHighest_AllSuit();                                                                                       // By default call the suit where you have the highest card.

        if (idx != Constants::No_Card) {
        
            Card &card = this->getCard(idx);
            suit = card.getSuit();

            #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_LEAD)
                DEBUG_PRINT(F("5. Play the joker as suit "));
                DEBUG_PRINT_SUIT(suit);
                DEBUG_PRINTLN(F("."));          
            #endif

        }
        else {
        
            suit = static_cast<Suit>(random(0, 4));                                                                                    // Select a random suit (should never happen unless on last card).
            
            #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_LEAD)
                DEBUG_PRINT(F("6. Play the joker as random "));
                DEBUG_PRINT_SUIT(suit);
                DEBUG_PRINTLN(F("."));          
            #endif

        }
        
        this->playJoker(suit);                                                                                                          // Play Joker and Win!
        return; 

    }

    #if defined(DEBUG) && defined(DEBUG_PLAYNOTRUMPS_LEAD)
        DEBUG_PRINTLN(F("7. Play the King, Queen, Jack or small card."));
    #endif
    
    if (this->playHighest_LargerThan_AllSuit(Rank::King))                                                                       return; // Play a king if you have it ..
    else if (this->playHighest_LargerThan_AllSuit(Rank::Queen))                                                                 return; // Play a queen if you have it ..
    else if (this->playHighest_LargerThan_AllSuit(Rank::Jack))                                                                  return; // Play a Jack if you have it ..
    else if (this->playLowest_AllSuit())                                                                                        return; // No, play the largest on-suit card you have.
    else this->playJoker(static_cast<Suit>(random(4)));                                                                         return;

}