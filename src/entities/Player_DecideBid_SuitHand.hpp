#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "../utils/Utils.h"
#include "Card.h"
#include "SuitPile.h"
#include "GameRound.h"


uint8_t evaluateSuitHand(Suit trumpSuit) {

    uint8_t score = 0;
    uint8_t trumpCount = 0;

    for (const auto& card : this->cards) {

        // High value for the Joker ..

        if (card.getRank() == Rank::Joker) {

            #if defined(DEBUG) && defined(DEBUG_EVALUATE_SUIT)
                DEBUG_PRINT(F("Joker (16), "));
            #endif 

            score += 16;
            trumpCount++;
            continue;

        }

        Suit currentSuit = card.getSuit();
        Rank currentRank = card.getRank();

        if (card.isLeftBower(trumpSuit) || card.isTrump(trumpSuit)) {

            trumpCount++;

            if (card.isRightBower(trumpSuit)) { // Right Bower

                #if defined(DEBUG) && defined(DEBUG_EVALUATE_SUIT)
                    DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                    DEBUG_PRINT(F(" (14), "));
                #endif 

                score += 14; 

            }
            else if (card.isLeftBower(trumpSuit)) { // Left Bower

                #if defined(DEBUG) && defined(DEBUG_EVALUATE_SUIT)
                    DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                    DEBUG_PRINT(F(" (12), "));
                #endif 
                
                score += 12; 
            } 
            else if (currentRank == Rank::Ace) { 
            
                #if defined(DEBUG) && defined(DEBUG_EVALUATE_SUIT)
                    DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                    DEBUG_PRINT(F(" (8), "));
                #endif 

                score += 8; 
            
            }
            else if (currentRank == Rank::King) { 
            
                #if defined(DEBUG) && defined(DEBUG_EVALUATE_SUIT)
                    DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                    DEBUG_PRINT(F(" (6), "));
                #endif 

                score += 6; 
            
            }  
            else if (currentRank == Rank::Queen) { 
            
                #if defined(DEBUG) && defined(DEBUG_EVALUATE_SUIT)
                    DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                    DEBUG_PRINT(F(" (5), "));
                #endif 

                score += 5; 
            
            }                                
            else if (currentRank == Rank::Ten) { 
            
                #if defined(DEBUG) && defined(DEBUG_EVALUATE_SUIT)
                    DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                    DEBUG_PRINT(F(" (4), "));
                #endif 

                score += 4; 
            
            }                                
            else if (currentRank > Rank::Six) { 
                
                #if defined(DEBUG) && defined(DEBUG_EVALUATE_SUIT)
                    DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                    DEBUG_PRINT(F(" (2), "));
                #endif 
            
                score += 2; 
                
            } 
            else { // Any other trump
                
                #if defined(DEBUG) && defined(DEBUG_EVALUATE_SUIT)
                    DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                    DEBUG_PRINT(F(" (1), "));
                #endif 
            
                score += 1; 
                
            } 

        } 

    }


    // If we have a decent hand than look for Aces and Kings ..

    if (score > 25) {
        
        for (const auto& card : this->cards) {

            Rank currentRank = card.getRank();

            if (!card.isTrump(trumpSuit)) {

                // Point system for off-suit cards

                if (currentRank == Rank::Ace) { 

                    #if defined(DEBUG) && defined(DEBUG_EVALUATE_SUIT)
                        DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                        DEBUG_PRINT(F(" (4), "));
                    #endif 

                    score += 4; 
                    
                }
                else if (currentRank == Rank::King) { 

                    #if defined(DEBUG) && defined(DEBUG_EVALUATE_SUIT)
                        DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                        DEBUG_PRINT(F(" (2), "));
                    #endif 

                    score += 2; 
                }

            }          

        }

    }


    // Bonus points for holding many trumps / lower if holding too few trumps ..

    if (trumpCount >= 6) {
        score += (trumpCount - 5) * 3;
    }
    else if (trumpCount <= 4) {
        score = score * 2 / 3;
    }

    return score;

}


uint8_t getBidLevelFromScore_Suits(uint8_t score) {

    if (score >= 65) return 10;
    if (score >= 54) return 9;
    if (score >= 43) return 8;
    if (score >= 28) return 7;
    if (score >= 15) return 6;
    return 0; // Not enough points to bid

}