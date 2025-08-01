#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "../utils/Utils.h"
#include "Card.h"
#include "SuitPile.h"
#include "GameRound.h"


uint8_t evaluateNoTrumpHand() {

    int score = 0;
    uint8_t suitLarge[4] = { 0, 0, 0, 0 };

    for (uint8_t i = 0; i < this->cardCount; i++) {
    
        Card &card = this->cards[i];

        if (card.getRank() == Rank::Joker) {
            
            #if defined(DEBUG) && defined(DEBUG_EVALUATE_NOTRUMPS)
                DEBUG_PRINT(F("Joker (10), "));
            #endif 

            score += 10; // Joker is a guaranteed trick
            continue;

        }

        // Point system based on high cards
        else if (card.getRank() == Rank::Ace) {
        
            #if defined(DEBUG) && defined(DEBUG_EVALUATE_NOTRUMPS)
                DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                DEBUG_PRINT(F(" (7), "));
            #endif 
        
            suitLarge[static_cast<uint8_t>(card.getSuit())]++;
            score += 7;

        }
        
        else if (card.getRank() == Rank::King) {
        
            #if defined(DEBUG) && defined(DEBUG_EVALUATE_NOTRUMPS)
                DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                DEBUG_PRINT(F(" (4), "));
            #endif 
        
            suitLarge[static_cast<uint8_t>(card.getSuit())]++;
            score += 4;

        }
        
        else if (card.getRank() == Rank::Queen) {
        
            #if defined(DEBUG) && defined(DEBUG_EVALUATE_NOTRUMPS)
                DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                DEBUG_PRINT(F(" (2), "));
            #endif 
        
            suitLarge[static_cast<uint8_t>(card.getSuit())]++;
            score += 2;

        }

    }
    

    // A good No Trump hand should have stoppers in multiple suits.
    // We add a small bonus for having cards in all four suits.
    
    if (suitLarge[0] > 0 && suitLarge[1] > 0 && suitLarge[2] > 0 && suitLarge[3] > 0) {

        #if defined(DEBUG) && defined(DEBUG_EVALUATE_NOTRUMPS)
            DEBUG_PRINT(F("Stopper (3), "));
        #endif 

        score += 3;

    }


    // If we strong upper cards and / or runs then up the score ..

    for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {
    
        uint8_t count = 0;
        bool hasAce = false;
        bool hasKing = false;
        bool hasQueen = false;

        for (uint8_t i = 0; i < this->cardCount; i++) {
        
            Card &card = this->cards[i];

            if (card.getSuit() == suit) {

                if (card.getSuit() == suit)
                if (card.getRank() == Rank::Ace)        { hasAce = true; }
                if (card.getRank() == Rank::King)       { hasKing = true; }
                if (card.getRank() == Rank::Queen)      { hasQueen = true; }
                count++;

            }

        }

        #if defined(DEBUG) && defined(DEBUG_EVALUATE_NOTRUMPS)
            DEBUG_PRINT(F("\n   - Suit "));
            DEBUG_PRINT_SUIT(suit);
            DEBUG_PRINT(F(", count: "));
            DEBUG_PRINT(count);
            DEBUG_PRINT(F(" "));
        #endif 

        if (hasAce && hasKing && hasQueen && count >= 4) {
            
            #if defined(DEBUG) && defined(DEBUG_EVALUATE_NOTRUMPS)
                DEBUG_PRINT(F("AKQ+3 ("));
                DEBUG_PRINT((count - 3) * 4);
                DEBUG_PRINT(F("), "));
            #endif 

            score += (count - 5) * 4;

        }
        else if (hasAce && (hasKing || hasQueen) && count >= 5) {
            
            #if defined(DEBUG) && defined(DEBUG_EVALUATE_NOTRUMPS)
                DEBUG_PRINT(F("A(KQ)+3 ("));
                DEBUG_PRINT((count - 4) * 2);
                DEBUG_PRINT(F("), "));
            #endif 
        
            score += (count - 5) * 3;

        }
        else if (!hasAce && hasKing && hasQueen && count >= 6) {
            
            #if defined(DEBUG) && defined(DEBUG_EVALUATE_NOTRUMPS)
                DEBUG_PRINT(F("!AKQ+3 ("));
                DEBUG_PRINT((count - 5) * 2);
                DEBUG_PRINT(F("), "));
            #endif 
        
            score += (count - 5) * 2;

        }

    }


    #if defined(DEBUG) && defined(DEBUG_EVALUATE_NOTRUMPS)
        DEBUG_PRINTLN();
    #endif 

    return score;

}


uint8_t getBidLevelFromScore_NoTrumps(uint8_t score) {

    if (score >= 90) return 10;
    if (score >= 75) return 9;
    if (score >= 60) return 8;
    if (score >= 40) return 7;
    if (score >= 25) return 6;
    return 0; // Not enough points to bid
}