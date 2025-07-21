#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "Card.h"
#include "SuitPile.h"


void playSuit_Follow() {

    #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
    DEBUG_PRINT(F("Suit - P"));
    DEBUG_PRINT(this->playerNumber);
    DEBUG_PRINTLN(F(" -----------------------"));
    #endif

    uint8_t partnerIdx = (this->getPlayerNumber() + 2) % 4;
    uint8_t playerBeforeIdx = (this->getPlayerNumber() + 3) % 4;
    uint8_t playerAfterIdx = (this->getPlayerNumber() + 1) % 4;

    Suit trumps = this->gameRound->winningBid_Suit();
    Card largestTrump = this->gameRound->getLargestTrumpInPlay(trumps);
    Card cardLed = this->gameRound->getCardLed();

    if (this->gameRound->hasPlayedCard(partnerIdx)) {   

        #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
            DEBUG_PRINTLN(F("1. Partner has played."));
        #endif

        if (this->gameRound->isPlayerWinningHand(partnerIdx)) {

            #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                DEBUG_PRINTLN(F("2. Partner is winning hand."));
            #endif
                    
            if (this->gameRound->isLastPlayer(this->getPlayerNumber())) {
            
                #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                    DEBUG_PRINTLN(F("3. Player is last."));
                #endif
                
                if (this->gameRound->hasTrumpsBeenLed()) {
            
                    #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                    DEBUG_PRINTLN(F("4. Trump has been led."));
                    #endif

                    if (this->playLowest_Trump(trumps))                                                         return; // Do not over trump ..
                    else this->playLowest_NonTrump_AllSuit(trumps);                                             return; // Throw rubbish ..

                }
                else {
            
                    #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                        DEBUG_PRINTLN(F("5. Trump has not been led."));
                    #endif

                    this->playLowest_NonTrump_AllSuit(trumps);                                                  return; // Throw rubbish ..

                }

            }
            else { // Player is not last player ..
            
                #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                    DEBUG_PRINTLN(F("6. Player is not last."));
                #endif

                if (this->gameRound->hasTrumpsBeenLed()) {
                
                    #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                        DEBUG_PRINTLN(F("7. Trump has been led."));
                    #endif

                    TriState canNextPlayerFollowSuit = this->gameRound->canPlayerFollowSuit(trumps, playerAfterIdx);

                    if (canNextPlayerFollowSuit == TriState::True || canNextPlayerFollowSuit == TriState::Maybe) {

                        #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                            DEBUG_PRINTLN(F("8. Next player can follow suit."));
                        #endif
                                        
                        uint8_t noOfUnplayedTrumps = this->numberOfUnplayedCards_InSuit(trumps, largestTrump.getRank(trumps)); 

                        #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                            DEBUG_PRINT(F("9. No of unplayed trumps "));
                            DEBUG_PRINTLN(noOfUnplayedTrumps);
                        #endif
                        
                        if (noOfUnplayedTrumps > 3) {

                            if (this->playHighest_Trump_LargerThan(trumps, largestTrump.getRank()))             return; // Win or at least force the other player to play big ..  
                            else if (this->playLowest_Trump(trumps))                                            return; // Follow suit ..
                            else this->playLowest_NonTrump_AllSuit(trumps);                                     return;
                        
                        }
                        else {

                            if (this->playLowest_Trump(trumps))                                                 return; // Follow suit ..
                            else this->playLowest_NonTrump_AllSuit(trumps);                                     return;

                        }

                    }
                    else {

                        #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                            DEBUG_PRINTLN(F("10. Next player can follow suit."));
                        #endif

                        if (this->playNextHighest_Trump_LargerThan(trumps, largestTrump.getRank()))             return; // Win hand with lowest possible trump ..
                        else if (this->playLowest_Trump(trumps))                                                return; // Follow suit ..
                        else this->playLowest_NonTrump_AllSuit(trumps);                                         return; // Throw rubbish ..
                    
                    }

                }
                else {
                        
                    if (this->gameRound->hasHandBeenTrumped()) {

                        #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                            DEBUG_PRINTLN(F("11. Hand has been trumped."));
                        #endif

                        if (this->playLowest_NonTrump_InSuit(cardLed.getSuit()))                                    return; // Follow suit if we can ..
                        else if (this->playNextHighest_Trump_LargerThan(trumps, largestTrump.getRank()))            return; // Over trump of we can..
                        else this->playLowest_NonTrump_AllSuit(trumps);                                             return; // Throw rubbish ..

                    }
                    else {

                        #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                            DEBUG_PRINTLN(F("12. Hand has not been trumped."));
                        #endif

                        uint8_t countOfUnplayed = this->gameRound->getUnplayedCountofHigherCards_InSuit(cardLed);

                        if (countOfUnplayed == 0) {

                            #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                                DEBUG_PRINTLN(F("13. Partner has played top card."));
                            #endif

                                if (this->playLowest_NonTrump_InSuit(cardLed.getSuit()))                            return; // Follow suit with lowest card ..
                                else if (this->playLowest_NonTrump_AllSuit(trumps))                                 return; // Throw rubbish ..
                                else this->playLowest_Trump(trumps);                                                return; // Trump the hand with a small one ..

                        }
                        else {

                            uint8_t noOfUnplayedTrumps = this->numberOfUnplayedCards_InSuit(cardLed.getSuit(), largestTrump.getRank());
                            
                            #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                                DEBUG_PRINT(F("14. No of unplayed cards in suit "));
                                DEBUG_PRINT_SUIT(cardLed.getSuit());
                                DEBUG_PRINT_SPACE();
                                DEBUG_PRINTLN(noOfUnplayedTrumps);
                            #endif
                            
                            if (noOfUnplayedTrumps > 3) {

                                if (this->playHighest_NonTrump_InSuit(cardLed.getSuit()))                           return; // Plenty out there so throw a large card ..
                                else if (this->playLowest_NonTrump_InSuit(cardLed.getSuit()))                       return; // Follow suit with lowest card ..
                                else if (this->playLowest_Trump(trumps))                                            return; // Trump the hand with a small one ..
                                else this->playLowest_NonTrump_AllSuit(trumps);                                     return; // Throw rubbish ..

                            }
                            else {

                                if (this->playLowest_NonTrump_InSuit(cardLed.getSuit()))                            return; // Follow suit with lowest card ..
                                else if (this->playLowest_Trump(trumps))                                            return; // Trump the hand with a small one ..
                                else this->playLowest_NonTrump_AllSuit(trumps);                                     return; // Throw rubbish ..

                            }

                        }

                    }

                }

            }

        }
        else { // Partner is not winning hand ..

            if (this->gameRound->hasTrumpsBeenLed()) {

                #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                    DEBUG_PRINTLN(F("15. Trump has been led."));
                #endif

                if (this->playNextHighest_Trump_LargerThan(trumps, largestTrump.getRank()))                     return; // Win hand with lowest possible trump ..
                else if (this->playLowest_Trump(trumps))                                                        return; // Follow suit ..
                else this->playLowest_NonTrump_AllSuit(trumps);                                                 return; // Throw rubbish ..

            }
            else {

                #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                    DEBUG_PRINTLN(F("16. Trump has not been led."));
                #endif
                    
                if (this->gameRound->hasHandBeenTrumped()) {

                    #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                        DEBUG_PRINTLN(F("17. Hand has been trumped."));
                    #endif

                    if (this->playLowest_NonTrump_InSuit(cardLed.getSuit()))                                    return; // Win hand  ..
                    else if (this->playNextHighest_Trump_LargerThan(trumps, largestTrump.getRank()))            return; // Over trump ..
                    else this->playLowest_NonTrump_AllSuit(trumps);                                             return; // Throw rubbish ..

                }
                else {

                    #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                        DEBUG_PRINTLN(F("18. Hand has not been trumped."));
                    #endif

                    if (this->playHighest_NonTrump_InSuit(cardLed.getSuit()))                                   return; // Win hand  ..
                    else if (this->playLowest_Trump(trumps))                                                    return; // Trump the hand with a small one ..
                    else this->playLowest_NonTrump_AllSuit(trumps);                                             return; // Throw rubbish ..

                }

            }

        }

    }
    else { // Partner has not played a card ..

        #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
            DEBUG_PRINTLN(F("19. Partner has not played."));
        #endif

        if (this->gameRound->hasTrumpsBeenLed()) {

            #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                DEBUG_PRINTLN(F("20. Trump has been led."));
            #endif

            if (this->playTop_Trump(trumps))                                                                    return; // If we have the winning trump then play it ..
            else if (this->playNextHighest_Trump_LargerThan(trumps, largestTrump.getRank()))                    return; // Win hand with lowest possible trump ..
            else if (this->playLowest_Trump(trumps))                                                            return; // Follow suit ..
            else this->playLowest_NonTrump_AllSuit(trumps);                                                     return; // Throw rubbish ..

        }
        else {

            #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                DEBUG_PRINTLN(F("21. Trump has not been led."));
            #endif

            if (this->gameRound->hasHandBeenTrumped()) {

                #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                    DEBUG_PRINTLN(F("22. Hand has been trumped."));
                #endif
                
                if (this->playLowest_NonTrump_InSuit(cardLed.getSuit()))                                        return; // Follow suit  ..
                else if (this->playNextHighest_Trump_LargerThan(trumps, largestTrump.getRank()))                return; // Over trump ..
                else this->playLowest_NonTrump_AllSuit(trumps);                                                 return; // Throw rubbish ..

            }
            else {

                #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_FOLLOW)
                    DEBUG_PRINTLN(F("23. Hand has not been trumped."));
                #endif

                Card largestCard = this->gameRound->getLargestCardInPlay(cardLed.getSuit());

                if (this->playNextHighest_NonTrump_LargerThan(cardLed.getSuit(), largestCard.getRank()))        return; // Play a slighlty higher card ..
                else if (this->playLowest_NonTrump_InSuit(cardLed.getSuit()))                                   return; // Trump the hand with a small one ..
                else if (this->playLowest_Trump(trumps))                                                        return; // Trump the hand with a small one ..
                else this->playLowest_NonTrump_AllSuit(trumps);                                                 return; // Throw rubbish ..

            }

        }
    
    }

}

void playSuit_Lead() {

    #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
        DEBUG_PRINT(F("Suit - P"));
        DEBUG_PRINT(this->playerNumber);
        DEBUG_PRINTLN(F(" -----------------------"));
    #endif

    uint8_t partnerIdx = (this->getPlayerNumber() + 2) % 4;
    uint8_t playerBeforeIdx = (this->getPlayerNumber() + 3) % 4;
    uint8_t playerAfterIdx = (this->getPlayerNumber() + 1) % 4;

    Suit trumps = this->gameRound->winningBid_Suit();
    Card cardLed = this->gameRound->getCardLed();

    Suit suit1 = this->gameRound->getHand(playerBeforeIdx)->getSuit(); // played by competitor 1
    Suit suit2 = this->gameRound->getHand(playerAfterIdx)->getSuit(); // played by competitor 2

    if (this->gameRound->weWonCall(this->getPlayerNumber(), partnerIdx)) {
    
        #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
            DEBUG_PRINTLN(F("1. We won the call."));
        #endif

        if (this->gameRound->isFirstRound()) {

            #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
                DEBUG_PRINTLN(F("2. First round."));
            #endif

            if (this->playJoker()) {                                                                                    // Conventional lead .. 
                this->gameRound->setJokerSuit(trumps); 
                return; 
            } 

            else if (this->playLowest_Trump_LowerThan(trumps, Rank::Ten))                                       return; // Defer to partner with little trump ..
            else if (this->playTop_NonTrump_AllSuit(trumps))                                                    return; // Play a wining ace ..
            else if (this->playLowest_NonTrump_WithSecondHighest_AllSuit(trumps))                               return; // Play a small card from any other suit where you hold the second largest card in that suit.
            else if (this->playLowest_NonTrump_ExcludeSuit(trumps, suit1, suit2))                               return; // Play a small card from any other suit that the other team did not call.
            else this->playLowest_NonTrump_AllSuit(trumps);                                                     return; // Play a small card from any other suit.
        }
        else {

            #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
                DEBUG_PRINTLN(F("2. Not first round."));
            #endif

            TriState playerBeforeHasTrumps = this->gameRound->hasTrumps(trumps, playerBeforeIdx);
            TriState playerAfterHasTrumps  = this->gameRound->hasTrumps(trumps, playerAfterIdx);
            TriState partnerHasTrumps      = this->gameRound->hasTrumps(trumps, partnerIdx);
            
            if (playerBeforeHasTrumps != TriState::False || playerAfterHasTrumps != TriState::False) {                  // If either of your opponents have trumps ..

                #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
                    DEBUG_PRINTLN(F("3. Competitor(s) may have trumps."));
                #endif
                
                bool hasJoker = this->getCard(Rank::Joker);                                                             // If you have the joker then play it ..

                if (this->playJoker())  { 

                    if (hasJoker) {
                        this->gameRound->setJokerSuit(cardLed.getSuit()); 
                        
                    }
                    return; 

                } 
                else if (this->playTop_Trump(trumps))                                                           return; // Play the top trump if you have it ..
                else if (partnerHasTrumps != TriState::False && 
                         this->playLowest_Trump_LowerThan(trumps, Rank::Ten))                                   return; // If your partner has trumps, play a small trump (< 10) otherwise.
                else if (this->playTop_NonTrump_AllSuit(trumps))                                                return; // Play a wining ace ..
                else if (this->playLowest_NonTrump_WithSecondHighest_AllSuit(trumps))                           return; // Play a small card from any other suit where you hold the second largest card in that suit.
                else if (this->playLowest_NonTrump_ExcludeSuit(trumps, suit1, suit2))                           return; // Play a small card from any other suit that the other team did not call.
                else this->playLowest_NonTrump_AllSuit(trumps);                                                 return; // Play a small card from any other suit.

            }
            else {                                                                                                      // Neither of your opponents have trumps ..

                #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
                    DEBUG_PRINTLN(F("3. Competitors do not have trumps."));
                #endif

                if (this->playTop_NonTrump_AllSuit(trumps))                                                     return; // Play a wining ace or highest card in suit..
                else if (this->playLowest_NonTrump_WithSecondHighest_AllSuit(trumps))                           return; // Play a small card from any other suit where you hold the second largest card in that suit.
                else if (this->playLowest_NonTrump_ExcludeSuit(trumps, suit1, suit2))                           return; // Play a small card from any other suit that the other team did not call.
                else if (partnerHasTrumps != TriState::False && 
                         this->playLowest_Trump_LowerThan(trumps, Rank::Ten))                                   return; // If your partner has trumps, play a small trump (< 10) otherwise.
                else {

                    for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {                                      // Play a small card from any other suit where your partner may have cards.

                        TriState partnerHasSuit = this->gameRound->canPlayerFollowSuit(suit, partnerIdx);

                        if (partnerHasSuit == TriState::True || partnerHasSuit == TriState::Maybe) {

                            if (this->playLowest_NonTrump_InSuit(suit))                                         return;

                        }

                    }

                }

                if (this->playLowest_NonTrump_AllSuit(trumps))                                                  return; // Play a small card from any other suit.
                else this->playLowest_Trump(trumps);                                                            return;

            }
        }

    }
    else {                                                                                                              // We did not win call 

        #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
            DEBUG_PRINTLN(F("4. We did not win call."));
        #endif

        if (this->playJoker())  {                                                                                       // If you have the top trump, play it.

            #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
                DEBUG_PRINTLN(F("5. Play Joker."));
            #endif
            
            this->gameRound->setJokerSuit(cardLed.getSuit()); 
            return; 

        } 
                                                                                                                        

        #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
            DEBUG_PRINTLN(F("6. Can not play Joker."));
        #endif

        for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {                                                  // If you have the top card in another suit and your competitor has / maybe the suits, play it.

            TriState hasSuit1 = this->gameRound->getHasSuitInHand(playerBeforeIdx, suit);
            TriState hasSuit2 = this->gameRound->getHasSuitInHand(playerAfterIdx, suit);

            if (hasSuit1 != TriState::False || hasSuit2 != TriState::False) {
                
                #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
                    DEBUG_PRINT(F("8. Play top where comp holds "));
                    DEBUG_PRINT_SUIT(suit);
                #endif
                
                if (this->playTop_NonTrump_InSuit(suit)) {

                    #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
                        DEBUG_PRINTLN(F(", success."));
                    #endif
                
                    return; 

                }
                else {

                    #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
                        DEBUG_PRINTLN(F(", fail."));
                    #endif

                }

            }

        }


        for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {                                              // If you have the top card in any suit play it ..                                      

            TriState hasSuit1 = this->gameRound->getHasSuitInHand(playerBeforeIdx, suit);
            TriState hasSuit2 = this->gameRound->getHasSuitInHand(playerAfterIdx, suit);

            if (hasSuit1 == TriState::False && hasSuit2 == TriState::False) {
                
                #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
                    DEBUG_PRINT(F("9. Play top "));
                    DEBUG_PRINT_SUIT(suit);
                #endif
                
                if (this->playTop_NonTrump_InSuit(suit)) {

                    #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
                        DEBUG_PRINTLN(F(", success."));
                    #endif
                
                    return; 

                }
                else {

                    #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
                        DEBUG_PRINTLN(F(", fail."));
                    #endif

                }

            }

        }
    

        if (this->gameRound->suitPlayerCalled(partnerIdx) != Suit::None) {                                              // If your partner called something and they are not short suited in that, play a small card.

            #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
                DEBUG_PRINT(F("10. Partner called "));
                DEBUG_PRINT_SUIT(this->gameRound->suitPlayerCalled(partnerIdx));
                DEBUG_PRINTLN();
            #endif
            
            TriState partnerHasSuit = this->gameRound->canPlayerFollowSuit(this->gameRound->suitPlayerCalled(partnerIdx), partnerIdx);

            if (partnerHasSuit == TriState::True || partnerHasSuit == TriState::Maybe) {

                if (this->playLowest_NonTrump_InSuit(this->gameRound->suitPlayerCalled(partnerIdx)))            return;

            }

        }


        #if defined(DEBUG) && defined(DEBUG_PLAYSUIT_LEAD)
            DEBUG_PRINTLN(F("11. Default play of cards."));
        #endif
        
        if (this->playLowest_NonTrump_WithSecondHighest_AllSuit(trumps))                                        return; // Play a small card from any other suit where you hold the second largest card in that suit.
        else if (this->playLowest_NonTrump_AllSuit(trumps))                                                     return; // Play a small card from any other suit.
        else this->playLowest_Trump(trumps);                                                                    return; // Play your smallest trump.

    }

}
