#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "../utils/Utils.h"
#include "Card.h"
#include "SuitPile.h"
#include "GameRound.h"


Bid bid(Bid &previousBid, Bid&partnersBid, Bid &bestInRound) {

    #if defined(DEBUG) && defined(DEBUG_EVALUATE)
        DEBUG_PRINT(F("Previous Bid "));
        DEBUG_PRINT_BID(previousBid.getBidType(), previousBid.getSuit(), previousBid.getLevel());
        DEBUG_PRINT(F(", Partner Bid "));
        DEBUG_PRINT_BID(partnersBid.getBidType(), partnersBid.getSuit(), partnersBid.getLevel());
        DEBUG_PRINT(F(", Best Bid "));
        DEBUG_PRINT_BID(bestInRound.getBidType(), bestInRound.getSuit(), bestInRound.getLevel());
        DEBUG_PRINTLN();
    #endif

    switch (partnersBid.getBidType()) {
    
        case BidType::None:
        case BidType::Pass:
        case BidType::Misere:
        #ifdef OPEN_MISERE
        case BidType::Open_Misere:
        #endif
            return decideBid(bestInRound);
        

        case BidType::Suit:
        case BidType::No_Trumps:
            {
                if (previousBid.getBidType() != BidType::None) { // We have already bid ..

                    // If partner and I are only two playing, then randomly do increase ..

                    uint8_t elev = random(partnersBid.getLevel()); // The higher up the the partners bid, the less likely we will up the bid ..
                    return increasePartnersBid(partnersBid, bestInRound, elev);

                }
                else {

                    uint8_t playing = 0;

                    for (uint8_t i = 0; i < 4; i++) {

                        if (this->gameRound->getBid(i).getBidType() != BidType::None) {

                            playing++;

                        }
                        
                    }

                    Bid elev = elevatePartnersHand(partnersBid, bestInRound);
                    Bid myBid = decideBid(bestInRound);

                    #if defined(DEBUG) && defined(DEBUG_EVALUATE)
                        DEBUG_PRINT_BID(elev.getBidType(), elev.getSuit(), elev.getLevel());
                        DEBUG_PRINT_SPACE();
                        DEBUG_PRINT(elev.getScore());
                        DEBUG_PRINT_SPACE();
                        DEBUG_PRINT_BID(myBid.getBidType(), myBid.getSuit(), myBid.getLevel());
                        DEBUG_PRINT_SPACE();
                        DEBUG_PRINTLN(myBid.getScore());
                    #endif

                    if (elev.getBidType() != BidType::Pass && myBid.getBidType() != BidType::Pass) {

                        if (playing == 2) { // Only partner and I playing ..
                        
                            if (abs(elev.getScore() - myBid.getBidScore()) < 120) { // Similar level ..

                                if (random(2) == 0) {

                                    return elev;

                                }

                            }

                        }

                        
                        if (myBid.isHigherThan(elev)) {
                            return myBid;
                        }

                        return elev;
                    
                    }

                    else if (elev.getBidType() == BidType::Pass && myBid.getBidType() != BidType::Pass) {

                        #if defined(DEBUG) && defined(DEBUG_EVALUATE)
                            DEBUG_PRINT_BID(partnersBid.getBidType(), partnersBid.getSuit(), partnersBid.getLevel());
                            DEBUG_PRINT_SPACE();
                            DEBUG_PRINTLN(partnersBid.getScore());
                            DEBUG_PRINT_BID(myBid.getBidType(), myBid.getSuit(), myBid.getLevel());
                            DEBUG_PRINT_SPACE();
                            DEBUG_PRINTLN(myBid.getScore());
                        #endif

                        if (abs(partnersBid.getScore() - myBid.getBidScore()) < 120) { // Similar level ..

                            if (random(2) == 1) {

                                myBid.setBidType(BidType::Pass);
                                return myBid;

                            }

                        }

                        return myBid;
                    
                    }

                    return elev;

                }

            }

            break;

    }


    return decideBid(bestInRound);

}

Bid elevatePartnersHand(Bid &partnersBid, Bid &bestInRound) {

    #if defined(DEBUG) && defined(DEBUG_ELEVPART_EVALUATE)
        DEBUG_PRINT(F("Player "));
        DEBUG_PRINT(this->getPlayerNumber());
        DEBUG_PRINT(F(", elevate hand? "));
        DEBUG_PRINT_SUIT(partnersBid.getSuit());
        DEBUG_PRINT_SPACE();
    #endif

    uint8_t score = 0;

    switch (partnersBid.getBidType()) {

        case BidType::Suit:
            score = evaluateSuitHand(partnersBid.getSuit());
            break;

        case BidType::No_Trumps:
            score = evaluateNoTrumpHand();
            break;

    }

    bool isBestInRoundPartners = (partnersBid.getPlayerIdx() == bestInRound.getPlayerIdx());

    #if defined(DEBUG) && defined(DEBUG_EVALUATE_NOTRUMPS)
        DEBUG_PRINT(F("   = score "));
        DEBUG_PRINTLN(score);
    #endif


    switch (score) {
    
        case 6 ... 10:
            {

                uint8_t elev = random(isBestInRoundPartners ? 8 : 4);

                #if defined(DEBUG) && defined(DEBUG_ELEVPART_EVALUATE)
                    DEBUG_PRINT(F("case 6 ... 11, "));
                    DEBUG_PRINTLN(elev);
                #endif
                
                return increasePartnersBid(partnersBid, bestInRound, elev);

            }

            break;

        case 11 ... 17:
            {
                uint8_t elev = random(isBestInRoundPartners ? 4 : 2);

                #if defined(DEBUG) && defined(DEBUG_ELEVPART_EVALUATE)
                    DEBUG_PRINT(F("case 12 ... 17, "));
                    DEBUG_PRINTLN(elev);
                #endif

                return increasePartnersBid(partnersBid, bestInRound, elev);

            }

            break;

        case 18 ... 255:
            {
                uint8_t inc = random(11 - partnersBid.getLevel());

                #if defined(DEBUG) && defined(DEBUG_ELEVPART_EVALUATE)
                    DEBUG_PRINT(F("case 18 ... 255, "));
                    DEBUG_PRINTLN(inc);
                #endif

                if (partnersBid.getLevel() < 10) {

                    Bid currentBid;
                    currentBid.setBidType(BidType::No_Trumps);
                    currentBid.setSuit(Suit::No_Trumps);
                    currentBid.setLevel(partnersBid.getLevel() + inc);
                    currentBid.setBidScore(score);

                    if (currentBid.isHigherThan(bestInRound)) {
                        return currentBid;
                    }

                }

            }

            break;
    
    }

    Bid defaultBid;
    defaultBid.setBidType(BidType::Pass);
    return defaultBid;

}


Bid decideBid(Bid &bestInRound) {

    Bid bestBid;
    bestBid.setBidType(BidType::Pass);
    bestBid.setSuit(Suit::None);
    bestBid.setLevel(0);
    bestBid.setBidScore(0);

    #if defined(DEBUG) && defined(DEBUG_EVALUATE)
        DEBUG_PRINT(F("P"));
        DEBUG_PRINT(this->playerNumber);
        DEBUG_PRINTLN(F(" ----------------------"));
    #endif


    // 1. Evaluate for Suit Bids

    for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {

        #if defined(DEBUG) && defined(DEBUG_EVALUATE_SUIT)
            DEBUG_PRINT(F(" - Suit "));
            DEBUG_PRINT_SUIT(suit);
            DEBUG_PRINT_SPACE();
        #endif

        uint8_t score = evaluateSuitHand(suit);
        uint8_t level = getBidLevelFromScore_Suits(score);

        #if defined(DEBUG) && defined(DEBUG_EVALUATE_SUIT)
            DEBUG_PRINT(F("score "));
            DEBUG_PRINT(score);
            DEBUG_PRINT(F(", tricks "));
            DEBUG_PRINTLN(level);
        #endif

        if (level >= 6) {

            Bid currentBid;
            currentBid.setBidType(BidType::Suit);
            currentBid.setSuit(suit);
            currentBid.setLevel(level);
            currentBid.setBidScore(score);

            if (currentBid.isHigherThan(bestBid) && currentBid.isHigherThan(bestInRound)) {
                bestBid = currentBid;
            }

        }

    }


    // 2. Evaluate for No Trump Bid

    #if defined(DEBUG) && defined(DEBUG_EVALUATE_NOTRUMPS)
        DEBUG_PRINT(F(" - No Trumps, score "));
    #endif

    uint8_t nt_score = evaluateNoTrumpHand();
    uint8_t nt_level = getBidLevelFromScore_NoTrumps(nt_score);

    #if defined(DEBUG) && defined(DEBUG_EVALUATE_NOTRUMPS)
        DEBUG_PRINT(F("   = score "));
        DEBUG_PRINT(nt_score);
        DEBUG_PRINT(F(", tricks "));
        DEBUG_PRINTLN(nt_level);
    #endif

    if (nt_level >= 6) {

        Bid currentBid;
        currentBid.setBidType(BidType::No_Trumps);
        currentBid.setSuit(Suit::None);
        currentBid.setLevel(nt_level);
        currentBid.setBidScore(nt_score);

        if (currentBid.isHigherThan(bestBid) && currentBid.isHigherThan(bestInRound)) {
            bestBid = currentBid;
        }
    }


    // 3. Evaluate for Misere Bid

    #if defined(DEBUG) && defined(DEBUG_EVALUATE_MISERE)
        DEBUG_PRINT(F(" - Misere? "));
    #endif

    SuitPile suitPiles[4];

    bool hasJoker = prepareSuitPiles(suitPiles);

    if (canBidMisere(hasJoker, suitPiles)) {

        #if defined(DEBUG) && defined(DEBUG_EVALUATE_MISERE)
            DEBUG_PRINTLN(F("Yes!"));
        #endif

        Bid currentBid;
        currentBid.setBidType(BidType::Misere);
        currentBid.setSuit(Suit::None);
        currentBid.setLevel(0);
        currentBid.setBidScore(0);

        if (currentBid.isHigherThan(bestBid) && currentBid.isHigherThan(bestInRound)) {
       
            bestBid = currentBid;
            return bestBid;

        }

    }
    else {

        #if defined(DEBUG) && defined(DEBUG_EVALUATE_MISERE)
            DEBUG_PRINTLN(F("No."));
        #endif

    }

    
    // 4. Evaluate for Open Misere Bid

    #ifdef OPEN_MISERE
    
        #if defined(DEBUG) && defined(DEBUG_EVALUATE_OPEN_MISERE)
            DEBUG_PRINT(F(" - Misere? "));
        #endif


        if (canBidOpenMisere(hasJoker, suitPiles)) {

            #if defined(DEBUG) && defined(DEBUG_EVALUATE_OPEN_MISERE)
                DEBUG_PRINTLN(F("Yes!"));
            #endif

            Bid currentBid;
            currentBid.setBidType(BidType::Open_Misere);
            currentBid.setSuit(Suit::None);
            currentBid.setLevel(0);
            currentBid.setBidScore(0);

            if (currentBid.isHigherThan(bestBid) && currentBid.isHigherThan(bestInRound)) {
                bestBid = currentBid;
            }

        }
        else {

            #if defined(DEBUG) && defined(DEBUG_EVALUATE_OPEN_MISERE)
                DEBUG_PRINTLN(F("No."));
            #endif

        }

    #endif


    #if defined(DEBUG) && defined(DEBUG_EVALUATE)
        DEBUG_PRINT(F("Player "));
        DEBUG_PRINT(this->getPlayerNumber());
        DEBUG_PRINT(F(" decided on "));
        DEBUG_PRINT_BID(bestBid.getBidType(), bestBid.getSuit(), bestBid.getLevel());
        DEBUG_PRINTLN();
    #endif


    if (bestBid.getBidType() == BidType::Pass) return bestBid;

    #if defined(DEBUG) && defined(DEBUG_EVALUATE)
        DEBUG_PRINTLN(F("Can we lower our bid?"));
    #endif

    switch (bestInRound.getBidType()) {
    
        case BidType::None:

            #if defined(DEBUG) && defined(DEBUG_EVALUATE)
                DEBUG_PRINTLN(F("No one else has bidded so we can reduce .."));
            #endif

            if (bestBid.getBidType() == BidType::Suit && bestBid.getLevel() > 6) {
            
                uint8_t randomReduction = random(3);

                #if defined(DEBUG) && defined(DEBUG_EVALUATE)
                    DEBUG_PRINT(F("Random reduction: "));
                    DEBUG_PRINTLN(randomReduction);
                #endif

                if (randomReduction == 0) { 

                    uint8_t level = bestBid.getLevel();

                    switch (level) {

                        case 7:

                            bestBid.setLevel(6);
                            break;

                        case 8:

                            alterBid(bestBid, static_cast<uint8_t>(random(3)));
                            break;

                        case 9:

                            alterBid(bestBid, static_cast<uint8_t>(random(6)));
                            break;

                        case 10:

                            alterBid(bestBid, static_cast<uint8_t>(random(11)));
                            break;

                    }
                
                }
            
            }
            
            return bestBid;

        default:
            {
                uint8_t randomReduction = random(2);

                #if defined(DEBUG) && defined(DEBUG_EVALUATE)
                    DEBUG_PRINT(F("Random reduction: "));
                    DEBUG_PRINTLN(randomReduction);
                #endif

                if (randomReduction == 0) { 
                    
                    switch (bestBid.getBidType()) {

                        case BidType::Suit:
                        case BidType::No_Trumps:
                            {

                                Bid testBid;
                                testBid.setBid(bestBid);
                                // testBid.setBidType(bestBid.getBidType());
                                // testBid.setSuit(bestBid.getSuit());
                                testBid.setLevel(bestBid.getLevel() - 1);
                                // testBid.setBidScore(bestBid.getBidScore());

                                if (testBid.isHigherThan(bestInRound)) {

                                    bestBid.setLevel(bestBid.getLevel() - 1);

                                }

                            }

                            break;

                        #ifdef OPEN_MISERE

                            case BidType::Open_Misere:
                                {

                                    Bid testBid;
                                    testBid.setBid(bestBid);
                                    // testBid.setBidType(BidType::Misere);
                                    // testBid.setSuit(bestBid.getSuit());
                                    // testBid.setLevel(bestBid.getLevel());
                                    // testBid.setBidScore(bestBid.getBidScore());

                                    if (testBid.isHigherThan(bestInRound)) {

                                        bestBid.setBidType(BidType::Misere);

                                    }

                                }

                                break;

                        #endif

                    }

                }

            }

            return bestBid;

    }

    return bestBid;

}

void alterBid(Bid &bestBid, uint8_t r) {

    switch (r) {

        case 6 ... 10:
            bestBid.setLevel(9);
            break;

        case 3 ... 5:
            bestBid.setLevel(8);
            break;

        case 1 ... 2:
            bestBid.setLevel(7);
            break;

        default:
            bestBid.setLevel(6);
            break;

    }

}


Bid increasePartnersBid(Bid &partnersBid, Bid &bestInRound, uint8_t elev) {

    if (partnersBid.getLevel() < 10 && elev == 0) {

        Bid currentBid;
        currentBid.setBidType(partnersBid.getBidType());
        currentBid.setSuit(partnersBid.getSuit());
        currentBid.setBidScore(0);


        // Up one?

        currentBid.setLevel(partnersBid.getLevel() + 1);

        if (currentBid.isHigherThan(bestInRound)) {

            return currentBid;

        }


        // Two ?

        if (partnersBid.getLevel() < 8) {
            
            currentBid.setLevel(partnersBid.getLevel() + 2);

            if (currentBid.isHigherThan(bestInRound)) {

                return currentBid;

            }

        }

    }


    // nothing was found so pass ..

    Bid defaultBid;
    defaultBid.setBidType(BidType::Pass);
    return defaultBid;            

}
