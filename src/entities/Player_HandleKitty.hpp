
#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "Bid.h"
#include "Card.h"

void handleKitty() {

    uint8_t numberDiscarded = 0;    
    Suit trumps = this->gameRound->winningBid_Suit();
    Bid winningBid = this->gameRound->getBid(this->getPlayerNumber());


    // Have either oponent bidded ?  Get the suit ..

    Suit startingSuit = this->gameRound->getBid((this->getPlayerNumber() - 1 + 4) % 4).getSuit();

    if (startingSuit == Suit::None) {
        startingSuit = this->gameRound->getBid((this->getPlayerNumber() + 1) % 4).getSuit();
    }

    if (startingSuit == Suit::None) {
        startingSuit = Suit::Spades;
    }    

    switch (winningBid.getBidType()) {

        case BidType::No_Trumps:
        case BidType::Suit:

            #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_TRUMPS)
                DEBUG_PRINTLN(F("Handle Kitty - Trumps / Suit"));
                DEBUG_PRINTLN(F("Short suit?"));
            #endif

            for (uint8_t s = static_cast<uint8_t>(startingSuit); s < static_cast<uint8_t>(startingSuit) + 4; s++) {             // Can we short suit ourselves?

                Suit suit = static_cast<Suit>(s % 4);
            
                #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_TRUMPS)
                    DEBUG_PRINT(F("- suit "));
                    DEBUG_PRINT_SUIT(suit);
                #endif
 
                if (suit == trumps) {

                    #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_TRUMPS)
                        DEBUG_PRINTLN(F(" - skip trumps."));
                    #endif
                    continue;
                }

                uint8_t countOfSuit = this->getNumberOfCards_InSuit(suit);
                uint8_t scoreOfSuit = this->getScoreOfCards_InSuit(suit, winningBid.getBidType());       


                #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_TRUMPS)
                    DEBUG_PRINT(F(" count "));
                    DEBUG_PRINT(countOfSuit);
                    DEBUG_PRINT(F(", score "));
                    DEBUG_PRINT(scoreOfSuit);
                #endif

                switch (countOfSuit) {
    
                    case 1:
                        if (scoreOfSuit <= 12) {
                            
                            uint8_t cardsDiscarded = this->discardAll_InSuit(suit);
                            numberDiscarded = numberDiscarded + cardsDiscarded;

                            #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_TRUMPS)
                                DEBUG_PRINT(F(" = discard "));
                                DEBUG_PRINT(cardsDiscarded);
                                DEBUG_PRINT(F(" card."));
                            #endif

                        }
                        break;
                        
                    case 2:
                        if (scoreOfSuit <= 24) {
                            
                            uint8_t cardsDiscarded = this->discardAll_InSuit(suit);
                            numberDiscarded = numberDiscarded + cardsDiscarded;
                            
                            #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_TRUMPS)
                                DEBUG_PRINT(F(" = discard "));
                                DEBUG_PRINT(cardsDiscarded);
                                DEBUG_PRINT(F(" card."));
                            #endif

                        }
                        break;
                                    
                    case 3:
                        if (scoreOfSuit <= 34) {
                            
                            uint8_t cardsDiscarded = this->discardAll_InSuit(suit);
                            numberDiscarded = numberDiscarded + cardsDiscarded;
                            
                            #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_TRUMPS)
                                DEBUG_PRINT(F(" = discard "));
                                DEBUG_PRINT(cardsDiscarded);
                                DEBUG_PRINT(F(" card."));
                            #endif

                        }
                        break;

                }
            
                #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_TRUMPS)
                    DEBUG_PRINTLN();
                #endif

                if (numberDiscarded == 3) return;
 
            }
 

            // If we have not unloaded 3 cards, discard the lowest cards of the shortest suits ..

            #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_TRUMPS)
                DEBUG_PRINT(F("Lowest of short suits?"));
            #endif

            if (numberDiscarded < 3) {
                
                for (uint8_t i = 1; i < 10; i++) {
                                        
                    for (Rank rank = Rank::Four; rank <= Rank::Ace; rank++) {
                        
                        for (uint8_t s = static_cast<uint8_t>(startingSuit); s < static_cast<uint8_t>(startingSuit) + 4; s++) {

                            Suit suit = static_cast<Suit>(s % 4);

                            if (numberDiscarded == 3) break;

                            uint8_t countOfSuit = this->getNumberOfCards_InSuit(suit);
                            uint8_t idx = this->getLowest_NonTrump_InSuit(suit);

                            if (idx != Constants::No_Card) {

                                Card &lowestCard = this->cards[idx];

                                if (countOfSuit == i && lowestCard.getRank() == rank) {

                                    if (static_cast<uint8_t>(lowestCard.getRank()) + countOfSuit == 15) {

                                        #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_TRUMPS)
                                            DEBUG_PRINT(F("\n- "));
                                            DEBUG_PRINT_CARD(lowestCard.getSuit(), lowestCard.getRank());
                                            DEBUG_PRINT(F(" skipped."));
                                        #endif
                                    
                                    }
                                    else {

                                        #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_TRUMPS)
                                            DEBUG_PRINT(F("\n- "));
                                            DEBUG_PRINT_CARD(lowestCard.getSuit(), lowestCard.getRank());
                                            DEBUG_PRINT_SPACE();
                                        #endif

                                        this->discardCard(lowestCard);
                                        numberDiscarded = numberDiscarded + 1;

                                    }

                                }            

                            }

                        }                                         
                                                
                        if (numberDiscarded == 3) {

                            #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_TRUMPS)
                                DEBUG_PRINTLN();
                            #endif

                            return;

                        }
        
                    }

                }

            }

            this->sort();

            #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_TRUMPS)
                DEBUG_PRINTLN();
            #endif

             
            // Double check we have discarded 3 cards ..
             
            #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_TRUMPS)
                DEBUG_PRINT(F("Dicard last "));
                DEBUG_PRINT(3 - numberDiscarded);
                DEBUG_PRINTLN();
            #endif

            while (numberDiscarded < 3) {

                uint8_t idx = this->getLowest_NonTrump_AllSuit();

                if (idx != Constants::No_Card) {
                
                    Card &card = this->cards[idx];

                    this->discardCard(card);
                    numberDiscarded = numberDiscarded + 1;

                }

            }
             
            this->sort();

            break;


        case BidType::Misere:
        #ifdef OPEN_MISERE
        case BidType::Open_Misere:             
        #endif
            {
                #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_MISERE)
                    #ifdef OPEN_MISERE
                        DEBUG_PRINTLN(F("Handle Kitty - Misere / Open Misere"));
                    #else
                        DEBUG_PRINTLN(F("Handle Kitty - Misere"));
                    #endif
                #endif

                SuitPile suitPiles[4];
                prepareSuitPiles(suitPiles);
                Rank lowestRank = Rank::None;


                // If we have the joker then dicard it ..

                uint8_t jokerIdx = this->getCard(Rank::Joker);

                if (jokerIdx != Constants::No_Card) {

                    #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_MISERE)
                        DEBUG_PRINTLN(F("Dicard Joker"));
                    #endif

                    Card &joker = this->getCard(jokerIdx);
                    this->discardCard(joker);
                    numberDiscarded = 1;

                }    


                // Loop through the suit piles and check scores ..

                for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {

                    uint8_t s = static_cast<uint8_t>(suit);

                    switch (suit) {

                        case Suit::Spades:
                        case Suit::Clubs:
                            lowestRank = Rank::Seven; // Therefore keep 5 and 6
                            break;

                        case Suit::Diamonds:
                        case Suit::Hearts:
                            lowestRank = Rank::Six; // Therefore keep 4 and 5
                            break;

                    }


                    // Discard highest cards?

                    if (suitPiles[s].score == Constants::No_Card) continue;

                    if (suitPiles[s].score == 0) {

                        // uint8_t countOfSuit = this->getNumberOfCards_InSuit(suit, lowestRank);

                        if (numberDiscarded < 3) { 
                            
                            numberDiscarded = numberDiscarded + this->discardAll_InSuit(suit, lowestRank);

                            #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_MISERE)
                                DEBUG_PRINT(F("A) Dicard suit "));
                                DEBUG_PRINT_SUIT(suit);
                                DEBUG_PRINT(F(", "));
                                DEBUG_PRINTLN(numberDiscarded);
                            #endif

                            if (numberDiscarded == 3) return;

                        }
                                                         
                        if (numberDiscarded == 3) return;

                    }

                }


                // Short suit ourselves?

                for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {

                    uint8_t s = static_cast<uint8_t>(suit);


                    // Discard highest cards?

                    if (suitPiles[s].score == Constants::No_Card) continue;

                    uint8_t countOfSuit = this->getNumberOfCards_InSuit(suit, Rank::None);

                    if (numberDiscarded + countOfSuit <= 3) { 
                        
                        numberDiscarded = numberDiscarded + this->discardAll_InSuit(suit, Rank::None);

                        #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_MISERE)
                            DEBUG_PRINT(F("A) Dicard suit "));
                            DEBUG_PRINT_SUIT(suit);
                            DEBUG_PRINT(F(", "));
                            DEBUG_PRINTLN(numberDiscarded);
                        #endif

                        if (numberDiscarded == 3) return;

                    }
                                                        
                    if (numberDiscarded == 3) return;

                }

    
                // If we have not unloaded 3 cards, discard the highest cards  ..
                                    
                for (Rank rank = Rank::Ace; rank >= Rank::Four; rank--) {

                    for (uint8_t s = static_cast<uint8_t>(startingSuit); s < static_cast<uint8_t>(startingSuit) + 4; s++) {

                        Suit suit = static_cast<Suit>(s % 4);

                        if (numberDiscarded == 3) continue;
                        
                        uint8_t idx = this->getHighest_NonTrump_InSuit(suit);

                        if (idx != Constants::No_Card) {

                            Card &highestCard = this->cards[idx];
                            
                            if (highestCard.getRank() == rank) {

                                #if defined(DEBUG) && defined(DEBUG_HANDLEKITTY_MISERE)
                                    DEBUG_PRINT(F("D) Dicard card "));
                                    DEBUG_PRINT_CARD(highestCard.getSuit(), highestCard.getRank());
                                    DEBUG_PRINT(F(", "));
                                    DEBUG_PRINTLN(numberDiscarded);
                                #endif

                                this->discardCard(highestCard);
                                numberDiscarded = numberDiscarded + 1;
                                if (numberDiscarded == 3) return;

                            }        

                        }    

                    }

                }                                         
                                                        
                if (numberDiscarded == 3) return;

            }

            break;

    }

} 
 