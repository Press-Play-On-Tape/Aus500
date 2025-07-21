#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "../utils/Utils.h"
#include "Card.h"
#include "SuitPile.h"
#include "GameRound.h"


bool prepareSuitPiles(SuitPile suitPiles[]) {

    #if defined(DEBUG) && defined(DEBUG_PREPARE_SUIT_PILES)
        DEBUG_PRINT(F("prepareSuitPiles, P "));
        DEBUG_PRINTLN(this->playerNumber);
    #endif
    
    bool hasJoker = false;


    // Clear stacks ..

    for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {

        uint8_t s = static_cast<uint8_t>(suit);
        suitPiles[s].reset(suit);

    }


    // First, iterate through the hand and sort cards into suit piles.

    for (uint8_t i = 0; i < this->cardCount; ++i) {

        Card &card = this->cards[i];

        if (card.rank == Rank::Joker) {
            hasJoker = true;
            continue;
        }


        // Add the card to the correct pile based on its suit ..

        uint8_t suitIndex = static_cast<uint8_t>(card.getSuit());

        suitPiles[suitIndex].cards[suitPiles[suitIndex].card_count].setSuit(card.getSuit());
        suitPiles[suitIndex].cards[suitPiles[suitIndex].card_count].setRank(card.getRank());
        suitPiles[suitIndex].card_count++;

    }


    int16_t totalDangerScore = hasJoker ? 3 : 0;

    const int16_t DANGER_THRESHOLD = 5; // A tunable parameter. Lower is stricter.


    // Sort the runs ..

    for (uint8_t suit = 0; suit < 4; suit++) {

        suitPiles[suit].sort();
        suitPiles[suit].calcMisereScore();

    }

    #if defined(DEBUG) && defined(DEBUG_PREPARE_SUIT_PILES)

        for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {

            uint8_t s = static_cast<uint8_t>(suit);

            DEBUG_PRINT_SUIT(suit);
            DEBUG_PRINT_SPACE();

            for (uint8_t j = 0; j < 10; j++) {
                DEBUG_PRINT_CARD(suitPiles[s].cards[j].getSuit(), suitPiles[s].cards[j].getRank());
                DEBUG_PRINT_SPACE();
            }

            DEBUG_PRINT(F(" ("));
            DEBUG_PRINT(suitPiles[s].card_count);
            DEBUG_PRINT(F(", "));
            DEBUG_PRINT(suitPiles[s].rankScore);
            DEBUG_PRINT(F(", "));
            DEBUG_PRINT(suitPiles[s].score);
            DEBUG_PRINTLN(F(")"));

        }

    #endif

    return hasJoker;

}


bool canBidMisere(bool hasJoker, SuitPile suitPiles[]) { //SJH hasJoker not used

    #if defined(DEBUG) && defined(DEBUG_EVALUATE_MISERE)
        DEBUG_PRINT(F("canBidMisere, P "));
        DEBUG_PRINTLN(this->playerNumber);
    #endif


    // First pass, if we have any single suit with a 60 and sum greater than 80 then call misere ..

    uint8_t hasHigh = false;
    uint8_t score = 0;
    uint8_t emptySuits = 0;
    uint8_t zeroes = 0;

    for (uint8_t suit = 0; suit < 4; suit++) {

        if (suitPiles[suit].score >= 60 && suitPiles[suit].score < Constants::No_Card) hasHigh = true;
        if (suitPiles[suit].score < Constants::No_Card) score = score + suitPiles[suit].score;
        if (suitPiles[suit].score == Constants::No_Card) score = score + 20;
        if (suitPiles[suit].score == 0) zeroes++;

    }

    if (score >= 80 && zeroes == 0) {

        #if defined(DEBUG) && defined(DEBUG_EVALUATE_MISERE)
            DEBUG_PRINTLN(F("1. Score >= 80."));
        #endif
        
        return true;

    }


    // Second pass, if we have a score greater than 60 and one or more empty suits then call misere .,

    hasHigh = false;
    score = 0;
    emptySuits = 0;
    zeroes = 0;

    for (uint8_t suit = 0; suit < 4; suit++) {

        if (suitPiles[suit].score < Constants::No_Card) score = score + suitPiles[suit].score;
        if (suitPiles[suit].score == Constants::No_Card) emptySuits++;
        if (suitPiles[suit].score == 0) zeroes++;

    }

    if (score >= 60 && emptySuits >= 1 && zeroes == 0) {

        #if defined(DEBUG) && defined(DEBUG_EVALUATE_MISERE)
            DEBUG_PRINTLN(F("2. Score > 60, 1+ empty suits."));
        #endif
        
        return true;

    }


    // Third pass, if we have a score greater than 50 and two or more empty suits then call misere .,

    hasHigh = false;
    score = 0;
    emptySuits = 0;
    zeroes = 0;

    for (uint8_t suit = 0; suit < 4; suit++) {

        if (suitPiles[suit].score < Constants::No_Card) score = score + suitPiles[suit].score;
        if (suitPiles[suit].score == Constants::No_Card) emptySuits++;
        if (suitPiles[suit].score == 0) zeroes++;

    }

    if (score >= 50 && emptySuits >= 2 && zeroes == 0) {

        #if defined(DEBUG) && defined(DEBUG_EVALUATE_MISERE)
            DEBUG_PRINTLN(F("3. Score > 50, 2+ empty suits."));
        #endif
        
        return true;

    }

    #if defined(DEBUG) && defined(DEBUG_EVALUATE_MISERE)
        DEBUG_PRINTLN(F("4. No misere!"));
    #endif

    return false;
    
}


#ifdef OPEN_MISERE
    bool canBidOpenMisere(bool hasJoker, SuitPile suitPiles[]) { //SJH hasJoker not used

        #if defined(DEBUG) && defined(DEBUG_EVALUATE_OPEN_MISERE)
            DEBUG_PRINT(F("canBidMisere, P "));
            DEBUG_PRINTLN(this->playerNumber);
        #endif


        // First pass, if we have any single suit with a 70 and sum greater than 100 then call misere ..

        uint8_t hasHigh = false;
        uint8_t score = 0;
        uint8_t emptySuits = 0;
        uint8_t zeroes = 0;

        for (uint8_t suit = 0; suit < 4; suit++) {

            if (suitPiles[suit].score >= 60 && suitPiles[suit].score < Constants::No_Card) hasHigh = true;
            if (suitPiles[suit].score < Constants::No_Card) score = score + suitPiles[suit].score;
            if (suitPiles[suit].score == Constants::No_Card) score = score + 20;
            if (suitPiles[suit].score == 0) zeroes++;

        }

        if (score >= 80 && zeroes == 0) {

            #if defined(DEBUG) && defined(DEBUG_EVALUATE_OPEN_MISERE)
                DEBUG_PRINTLN(F("1. Score >= 100."));
            #endif
            
            return true;

        }


        // Second pass, if we have a score greater than 80 and one or more empty suits then call misere .,

        hasHigh = false;
        score = 0;
        emptySuits = 0;

        for (uint8_t suit = 0; suit < 4; suit++) {

            if (suitPiles[suit].score < Constants::No_Card) score = score + suitPiles[suit].score;
            if (suitPiles[suit].score == Constants::No_Card) emptySuits++;
            if (suitPiles[suit].score == 0) zeroes++;

        }

        if (score >= 60 && emptySuits >= 1 && zeroes == 0) {

            #if defined(DEBUG) && defined(DEBUG_EVALUATE_OPEN_MISERE)
                DEBUG_PRINTLN(F("2. Score > 80, 1+ empty suits."));
            #endif
            
            return true;

        }


        // Third pass, if we have a score greater than 60 and two or more empty suits then call misere .,

        hasHigh = false;
        score = 0;
        emptySuits = 0;

        for (uint8_t suit = 0; suit < 4; suit++) {

            if (suitPiles[suit].score < Constants::No_Card) score = score + suitPiles[suit].score;
            if (suitPiles[suit].score == Constants::No_Card) emptySuits++;
            if (suitPiles[suit].score == 0) zeroes++;

        }

        if (score >= 50 && emptySuits >= 2 && zeroes == 0) {

            #if defined(DEBUG) && defined(DEBUG_EVALUATE_OPEN_MISERE)
                DEBUG_PRINTLN(F("3. Score > 60, 2+ empty suits."));
            #endif
            
            return true;

        }

        #if defined(DEBUG) && defined(DEBUG_EVALUATE_OPEN_MISERE)
            DEBUG_PRINTLN(F("4. No misere!"));
        #endif

        return false;
        
    }
#endif
