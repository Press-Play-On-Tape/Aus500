
#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "../utils/Utils.h"
#include "Card.h"
#include "SuitPile.h"
#include "GameRound.h"


void clearSelection() {

    for (uint8_t i = 0; i < this->cardCount; i++) {

        Card &card = this->cards[i];
        card.setSelected(false);

    }
    
}


uint8_t getCountOfSuit(Suit suit) {

    uint8_t count = 0;

    for (uint8_t i = 0; i < this->cardCount; i++) {
    
        Card &card = this->cards[i];

        if (card.getSuit() == suit) {
        
            count++;
            
        }

    }

    return count;

}


Suit canShortSuit() {

    for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {

        uint8_t count = this->getCountOfSuit(suit);

        if (count == 1) {

            return suit;

        }

    }

    return Suit::None;

}

void playCard(uint8_t idx) {

    Card &card = this->cards[idx];
    Suit trumps = this->gameRound->winningBid_Suit();

    if (card.getRank() == Rank::Joker) {
        card.setSuit(this->gameRound->getJokerSuit());
    }

    this->cardJustPlayed.setSuit(card.getSuit());
    this->cardJustPlayed.setRank(card.getRank());

    this->gameRound->markCardPlayed(card.getSuit(), card.getRank());
    this->gameRound->getHand(this->gameRound->getCurrentPlayer())->setSuit(card.getSuit());
    this->gameRound->getHand(this->gameRound->getCurrentPlayer())->setRank(card.getRank());
    
    card.reset();
    this->sort();

}

bool hasTrumps(Suit trumps) {

    for (uint8_t i = 0; i < this->cardCount; i++) {
    
        Card &card = this->cards[i];

        if (card.getSuit() == trumps || card.isLeftBower(trumps)) {
        
            return true;
            
        }

    }

    return false;

}

bool hasSuit(Suit suit) {

    return hasSuit(suit, Suit::None);

}


bool hasSuit(Suit suit, Suit trump) {

    for (uint8_t i = 0; i < this->cardCount; i++) {
    
        Card &card = this->cards[i];

        switch (card.getRank(trump)) {

            case Rank::Four ... Rank::Ten:
            case Rank::Queen ... Rank::Ace:
            case Rank::Right_Bower:

                if (card.getSuit() == suit ) {
                
                    return true;
                    
                }

                break;

            case Rank::Left_Bower:
        
                if (getTrump_AltSuit(card.getSuit()) == suit) {
                
                    return true;
                    
                }

                break;
        
        }

    }

    return false;

}


// Are we hold the nominated card?

uint8_t getCard(Rank rank) {

    for (uint8_t i = 0; i < this->cardCount; i++) {
    
        Card &card = this->cards[i];

        if ((rank == Rank::Joker && card.getRank() == Rank::Joker) || card.getRank() == rank) {
        
            return i;

        }

    }

    return Constants::No_Card;

}


// Are we hold the nominated card?

uint8_t getCard(Suit suit, Rank rank) {

    for (uint8_t i = 0; i < this->cardCount; i++) {
    
        Card &card = this->cards[i];

        if ((rank == Rank::Joker && card.getRank() == Rank::Joker) || (card.getSuit() == suit && card.getRank() == rank)) {
        
            return i;

        }

    }

    return Constants::No_Card;

}


// Trump cards ----------------------------------------------------------------------------------------


// Return the highest trump in the player's hand ..

uint8_t getHighestTrump(Suit trumps) {

    return this->getHighestTrump(trumps, Rank::None);

}


// Return the highest trump in the player's hand which is above the specified rank ..

uint8_t getHighestTrump(Suit trumps, Rank highestRank) {

    uint8_t foundIdx = Constants::No_Card;

    for (uint8_t i = 0; i < this->cardCount; i++) {
    
        Card &card = this->cards[i];

        if (card.getRank(trumps) == Rank::Joker) {

            highestRank = Rank::Joker;
            foundIdx = i;
            break;

        }

        if (card.getSuit() == trumps || card.isLeftBower(trumps)) {

            if (card.getRank(trumps) > highestRank) {

                highestRank = card.getRank(trumps);
                foundIdx = i;

            }                

        }

    }

    return foundIdx;

}


// Return the lowest trump in the player's hand ..

uint8_t getLowestTrump(Suit trumps) {

   return this->getLowestTrump(trumps, Rank::Joker);

}


// Return the lowest trump in the player's hand which is above the specified rank ..

uint8_t getLowestTrump(Suit trumps, Rank lowestRank) {

    uint8_t foundIdx = Constants::No_Card;

    for (uint8_t i = 0; i < this->cardCount; i++) {
    
        Card &card = this->cards[i];

        if (card.getRank(trumps) == Rank::Joker) {

            lowestRank = Rank::Joker;
            foundIdx = i;

        }

        if (card.getSuit() == trumps || card.isLeftBower(trumps)) {

            if (card.getRank(trumps) < lowestRank) {

                lowestRank = card.getRank(trumps);
                foundIdx = i;

            }                

        }

    }

    return foundIdx;

}



// Return the next highest trump in the player's hand from specified rank ..

uint8_t getNextHighestTrump(Suit trumps, Rank specifiedRank) {

    Rank foundRank = Rank::None;
    uint8_t foundIdx = Constants::No_Card;
    Card specifiedCard;

    specifiedCard.setRank(specifiedRank);
    specifiedCard.setSuit(trumps);

    for (uint8_t i = 0; i < this->cardCount; i++) {

        Card &card = this->cards[i];

        if (card.getSuit() == trumps || card.isLeftBower(trumps)) {

            #if defined(DEBUG) && defined(DEBUG_GETNEXTHIGHESTTRUMP)
                DEBUG_PRINT(F("Found card "));
                DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                DEBUG_PRINTLN();
            #endif

            if (card.getRank(trumps) > specifiedCard.getRank(trumps)) {

                if (foundRank == Rank::None) {
                
                    foundRank = card.getRank(trumps);
                    foundIdx = i;

                }
                else if (card.getRank(trumps) < foundRank) {

                    foundRank = card.getRank(trumps);
                    foundIdx = i;

                }

            }                

        }

    }

    return foundIdx;

}




// Non trump cards ------------------------------------------------------------------------------------


// Get the lowest card in the any suit ..

uint8_t getLowest_NonTrump_AllSuit() {

    return getLowest_NonTrump_AllSuit(Suit::None);

}


// Get the lowest card in the any non trump suit ..

uint8_t getLowest_NonTrump_AllSuit(Suit excludeTrumps) {

    uint8_t returnIdx = Constants::No_Card;
    Rank rank = Rank::TopCard;

    for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {
    
        if (suit != excludeTrumps) {

            uint8_t idx = this->getLowest_NonTrump_InSuit(suit);

            if (idx != Constants::No_Card)  {

                Card &card = this->cards[idx];

                if (card.getRank() < rank) {
                    
                    returnIdx = idx;
                    rank = card.getRank();

                }

            }

        }

    }

    return returnIdx;

}

// Get the lowest non-trump card in suit ..

uint8_t getLowest_NonTrump_InSuit(Suit suit) {

    return this->getLowest_NonTrump_InSuit(suit, Rank::TopCard);

}


// Get the lowest non-trump card in suit which is lower than the nominated rank..

uint8_t getLowest_NonTrump_InSuit(Suit suit, Rank rank) {

    uint8_t idx = Constants::No_Card;

    for (int j = 0; j < this->cardCount; j++) {

        Card &card = cards[j];

        if (card.getSuit() == suit && card.getRank() < rank) {
            idx = j;
            rank = this->cards[j].getRank();
        }

    }

    return idx;

}


// Get the highest non-trump card in suit ..

uint8_t getHighest_NonTrump_InSuit(Suit suit) {

    return this->getHighest_NonTrump_InSuit(suit, Rank::None);

}


// Get the highest non-trump card in suit which is higher than the nominated rank..

uint8_t getHighest_NonTrump_InSuit(Suit suit, Rank rank) {

    uint8_t idx = Constants::No_Card;

    for (int j = 0; j < this->cardCount; j++) {

        if (cards[j].getSuit() == suit && this->cards[j].getRank() > rank) {
            idx = j;
            rank = this->cards[j].getRank();
        }

    }

    return idx;

}


// Get the highest card in the any suit ..

uint8_t getHighest_NonTrump_AllSuit() {

    return getHighest_NonTrump_AllSuit(Suit::None);

}


// Get the highest card in the any non trump suit ..

uint8_t getHighest_NonTrump_AllSuit(Suit excludeTrumps) {

    uint8_t returnIdx = Constants::No_Card;
    Rank rank = Rank::None;

    for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {
    
        if (suit != excludeTrumps) {

            uint8_t idx = this->getHighest_NonTrump_InSuit(suit);

            if (idx != Constants::No_Card)  {

                Card &card = this->cards[idx];

                if (card.getRank() > rank) {
                    
                    returnIdx = idx;
                    rank = card.getRank();

                }

            }

        }

    }

    return returnIdx;

}


// Get the next highest non-trump card in suit which is higher than the nominated rank..

uint8_t getNextHighest_NonTrump_InSuit(Suit suit, Rank specifiedRank) {

    Rank foundRank = Rank::TopCard;
    uint8_t foundIdx = Constants::No_Card;

    #if defined(DEBUG) && defined(DEBUG_GETNEXTHIGHESTCARD_NOTRUMP_INSUIT)
        DEBUG_PRINT(F("getNextHighestCard_NonTrump_InSuit("));
        DEBUG_PRINT_SUIT(suit);
        DEBUG_PRINT(F(","));
        DEBUG_PRINT_RANK(specifiedRank);
        DEBUG_PRINT(F(") - "));
    #endif

    for (uint8_t i = 0; i < this->cardCount; i++) {
    
        Card &card = this->cards[i];

        #if defined(DEBUG) && defined(DEBUG_GETNEXTHIGHESTCARD_NOTRUMP_INSUIT)
            DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
            DEBUG_PRINT_SPACE();
        #endif


        if (card.getSuit() == suit) {

            if (card.getRank() > specifiedRank && card.getRank() < foundRank) {
            
                foundRank = card.getRank();
                foundIdx = i;

            } 

        }

    }


    if (foundIdx != Constants::No_Card) {

        Card &card = this->cards[foundIdx];

        #if defined(DEBUG) && defined(DEBUG_GETNEXTHIGHESTCARD_NOTRUMP_INSUIT)
            DEBUG_PRINT(F(" = "));
            DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
            DEBUG_PRINTLN();
        #endif

    }
    else {

        #if defined(DEBUG) && defined(DEBUG_GETNEXTHIGHESTCARD_NOTRUMP_INSUIT)
            DEBUG_PRINTLN(F(" = No_Card."));
        #endif
    
    }

    return foundIdx;

}


// Do we have the top unplayed card in the nominated suit ..

uint8_t getTop_NonSuit(Suit suitToFollow) {

    #if defined(DEBUG) && defined(DEBUG_GETTOP_NONSUIT)
        DEBUG_PRINT(F("getTop "));
        DEBUG_PRINT_SUIT(suitToFollow);
        DEBUG_PRINT(F(": "));
    #endif
    

    // What is the players highest trump?

    uint8_t idx = this->getHighest_NonTrump_InSuit(suitToFollow);

    if (idx == Constants::No_Card)  {

        #if defined(DEBUG) && defined(DEBUG_GETTOP_NONSUIT)
            DEBUG_PRINTLN(F("player has none."));
        #endif

        return Constants::No_Card;

    }


    Card &playersHighestCard = this->cards[idx];

    #if defined(DEBUG) && defined(DEBUG_GETTOP_NONSUIT)

        DEBUG_PRINT(F("player holds "));
        DEBUG_PRINT_CARD(playersHighestCard.getSuit(), playersHighestCard.getRank());
        DEBUG_PRINT(F(": "));

    #endif


    // What is the highest unplayed card in this suit?

    Rank highestUnplayedCard = playersHighestCard.getRank();

    for (Rank rank = Rank::Ace; rank >= playersHighestCard.getRank(); rank--) {

        if (!this->gameRound->hasCardBeenPlayed(suitToFollow, rank)) {

            #if defined(DEBUG) && defined(DEBUG_GETTOP_NONSUIT)

                DEBUG_PRINT_CARD(suitToFollow, rank);
                DEBUG_PRINT(F(" not played. "));

            #endif

            highestUnplayedCard = rank; 
            break;       

        } 
        else {

            #if defined(DEBUG) && defined(DEBUG_GETTOP_NONSUIT)

                DEBUG_PRINT_CARD(suitToFollow, rank);
                DEBUG_PRINT(F(" played. "));

            #endif

        }

    }


    // So are we holding the top card?

    #if defined(DEBUG) && defined(DEBUG_GETTOP_NONSUIT)
        DEBUG_PRINT(F("Highest unplayed "));
        DEBUG_PRINT_CARD(suitToFollow, highestUnplayedCard);
        DEBUG_PRINTLN(F("."));
    #endif

    if (highestUnplayedCard == playersHighestCard.getRank()) {

        return idx;

    }

    return Constants::No_Card;

}


// Do we have the top unplayed card in the nominated suit below the nominated rank ..

uint8_t getTop_NonSuit(Suit suitToFollow, Rank topRank) {


    // What is the players highest trump?

    uint8_t idx = this->getHighest_NonTrump_InSuit(suitToFollow);
    if (idx == Constants::No_Card)  return Constants::No_Card;

    Card &playersHighestCard = this->cards[idx];


    // What is the highest unplayed card in this suit?

    Rank highestUnplayedCard = Rank::None;

    for (Rank rank = Rank::Four; rank <= topRank; rank++) {

        if (!this->gameRound->hasCardBeenPlayed(suitToFollow, rank)) {

            highestUnplayedCard = rank;        

        } 

    }

    // So are we holding the top card?

    if (highestUnplayedCard == playersHighestCard.getRank()) {

        return idx;

    }

    return Constants::No_Card;

}


// Get the count of card in hand of a given suit..

uint8_t getNumberOfCards_InSuit(Suit suit) {

    return this->getNumberOfCards_InSuit(suit, Rank::None);

}

// Get the count of card in hand of a given suit higher than LowestRank..

uint8_t getNumberOfCards_InSuit(Suit suit, Rank lowestRank) {

    uint8_t numberOfCards = 0;

    for (int j = 0; j < this->cardCount; j++) {

        Card &card = cards[j];

        if (card.getSuit() == suit && card.getRank() >= lowestRank) {
            numberOfCards++;
        }

    }

    return numberOfCards;

}




// Get the basic score of card in hand of a given suit..

uint8_t getScoreOfCards_InSuit(Suit suit, BidType bidType) {

    return this->getScoreOfCards_InSuit(suit, Rank::None, bidType);

}


// Get the basic score of card in hand of a given suit..

uint8_t getScoreOfCards_InSuit(Suit suit, Rank lowestRank, BidType bidType) {

    uint8_t scoreOfCards = 0;

    for (int j = 0; j < this->cardCount; j++) {

        Card &card = cards[j];

        if (card.getSuit() == suit && card.getRank() >= lowestRank) {

            switch (bidType) {
            
                case BidType::Suit:
                case BidType::Misere:
                #ifdef OPEN_MISERE
                case BidType::Open_Misere:
                #endif

                    scoreOfCards = scoreOfCards + Constants::ScoreType_Basic[static_cast<uint8_t>(card.getRank())];
                    break;
            
                case BidType::No_Trumps:

                    scoreOfCards = scoreOfCards + Constants::ScoreType_Trumps[static_cast<uint8_t>(card.getRank())];
                    break;
            
            }

        }

    }

    return scoreOfCards;

}


uint8_t discardAll_InSuit(Suit suit) {

    return this->discardAll_InSuit(suit, Rank::None);

}


// Discard all cards in hand with a rank of 'lowestRank' or above ..

uint8_t discardAll_InSuit(Suit suit, Rank lowestRank) {

    uint8_t cardsDiscarded = 0;

    #if defined(DEBUG) && defined(DEBUG_DISCARDALL_INSUIT)
        DEBUG_PRINT(F("discardAll_InSuit("));
        DEBUG_PRINT_SUIT(suit);
        DEBUG_PRINT(F(","));
        DEBUG_PRINT_RANK(lowestRank);
        DEBUG_PRINTLN(F(")"));
    #endif

    for (Rank rank = lowestRank; rank <= Rank::Ace; rank++) {

        for (int j = 0; j < this->cardCount; j++) {

            Card *card = &cards[j];

            if (card->getSuit() == suit && card->getRank() == rank) {
                
                this->cardCount--;
                cardsDiscarded++;
                this->gameRound->addKitty(card);

                #if defined(DEBUG) && defined(DEBUG_DISCARDALL_INSUIT)
                    DEBUG_PRINT(F(" discard "));
                    DEBUG_PRINT_CARD(card->getSuit(), card->getRank());
                    DEBUG_PRINT_SPACE();
                #endif

                card->reset();
                this->sort();

            }

            if (this->cardCount == 10) break;

        }

        if (this->cardCount == 10) break;

    }

    this->sort();

    return cardsDiscarded;

}


void discardCard(Card &discardCard) {

    uint8_t cardsDiscarded = 0;

    for (int j = 0; j < this->cardCount; j++) {

        Card *card = &cards[j];

        if (card->getSuit() == discardCard.getSuit() && card->getRank() == discardCard.getRank()) {
            
            this->cardCount--;
            cardsDiscarded++;
            this->gameRound->addKitty(card);

            #if defined(DEBUG) && defined(DEBUG_DISCARDCARD)
                DEBUG_PRINT(F(" discard "));
                DEBUG_PRINT_CARD(card->getSuit(), card->getRank());
                DEBUG_PRINT_SPACE();
            #endif

            card->reset();
            break;

        }

    }

    this->sort();

    return cardsDiscarded;

}


void sort() {

    BidType bidType = BidType::Pass;
    Suit trumps = this->gameRound->winningBid_Suit();

    if (this->gameRound->getWinningBid_Idx() != Constants::WinningBid_None) {
        bidType = this->gameRound->getWinningBid().getBidType();
    }



    // What sequence do we want?

    uint8_t seq[] = {0, 2, 1, 3 };

    bool spades     = this->hasSuit(Suit::Spades);
    bool clubs      = this->hasSuit(Suit::Clubs);
    bool diamonds   = this->hasSuit(Suit::Diamonds);
    bool hearts     = this->hasSuit(Suit::Hearts);


    if (spades && clubs && diamonds && hearts) {
        if (trumps == Suit::Spades)                             { seq[0] = 3; seq[1] = 1; seq[2]= 0; seq[3] = 2; }
        if (trumps == Suit::Clubs)                              { seq[0] = 1; seq[1] = 3; seq[2]= 0; seq[3] = 2; }
        if (trumps == Suit::Diamonds)                           { seq[0] = 0; seq[1] = 2; seq[2]= 3; seq[3] = 1; }
        if (trumps == Suit::Hearts)                             { seq[0] = 0; seq[1] = 2; seq[2]= 1; seq[3] = 3; }
    }

    if (!spades && clubs && diamonds && hearts) {
        if (trumps == Suit::Spades)                             { seq[0] = 3; seq[1] = 1; seq[2]= 0; seq[3] = 2; } 
        if (trumps == Suit::Clubs)                              { seq[0] = 3; seq[1] = 1; seq[2]= 0; seq[3] = 2; } 
        if (trumps == Suit::Diamonds)                           { seq[0] = 3; seq[1] = 1; seq[2]= 2; seq[3] = 0; }
        if (trumps == Suit::Hearts)                             { seq[0] = 3; seq[1] = 1; seq[2]= 0; seq[3] = 2; }
    }

    if (spades && !clubs && diamonds && hearts) {
        if (trumps == Suit::Spades)                             { seq[0] = 1; seq[1] = 3; seq[2]= 0; seq[3] = 2; }
        if (trumps == Suit::Clubs)                              { seq[0] = 1; seq[1] = 3; seq[2]= 0; seq[3] = 2; }
        if (trumps == Suit::Diamonds)                           { seq[0] = 1; seq[1] = 3; seq[2]= 2; seq[3] = 0; }
        if (trumps == Suit::Hearts)                             { seq[0] = 1; seq[1] = 3; seq[2]= 0; seq[3] = 2; }
    }

    if (spades && clubs && !diamonds && hearts) {
        if (trumps == Suit::Spades)                             { seq[0] = 2; seq[1] = 0; seq[2]= 3; seq[3] = 1; }
        if (trumps == Suit::Clubs)                              { seq[0] = 0; seq[1] = 2; seq[2]= 3; seq[3] = 1; }
        if (trumps == Suit::Diamonds)                           { seq[0] = 0; seq[1] = 2; seq[2]= 3; seq[3] = 1; }
        if (trumps == Suit::Hearts)                             { seq[0] = 0; seq[1] = 2; seq[2]= 3; seq[3] = 1; }
    }

    if (spades && clubs && diamonds && !hearts) {
        if (trumps == Suit::Spades)                             { seq[0] = 2; seq[1] = 0; seq[2]= 1; seq[3] = 3; }
        if (trumps == Suit::Clubs)                              { seq[0] = 0; seq[1] = 2; seq[2]= 1; seq[3] = 3; }
        if (trumps == Suit::Diamonds)                           { seq[0] = 0; seq[1] = 2; seq[2]= 1; seq[3] = 3; }
        if (trumps == Suit::Hearts)                             { seq[0] = 0; seq[1] = 2; seq[2]= 1; seq[3] = 3; }
    }

    if (!spades && !clubs && diamonds && hearts) {
        if (trumps == Suit::Diamonds)                           { seq[0] = 0; seq[1] = 1; seq[2]= 3; seq[3] = 2; }
        if (trumps == Suit::Hearts)                             { seq[0] = 0; seq[1] = 1; seq[2]= 2; seq[3] = 3; }
    }

    if (spades && clubs && !diamonds && !hearts) {
        if (trumps == Suit::Spades)                             { seq[0] = 3; seq[1] = 2; seq[2]= 0; seq[3] = 1; }
        if (trumps == Suit::Clubs)                              { seq[0] = 2; seq[1] = 3; seq[2]= 0; seq[3] = 1; }
    }


    #if defined(DEBUG) && defined(DEBUG_SORT)
        
        if (this->playerNumber == 3) {

            DEBUG_PRINT(F("\n"));
            DEBUG_PRINT(spades);
            DEBUG_PRINT_SPACE();
            DEBUG_PRINT(clubs);
            DEBUG_PRINT_SPACE();
            DEBUG_PRINT(diamonds);
            DEBUG_PRINT_SPACE();
            DEBUG_PRINT(hearts);
            DEBUG_PRINT_SPACE();
            DEBUG_PRINT_SUIT(trumps);
            DEBUG_PRINT_SPACE();
        
            for(uint8_t i = 0; i< 4; i++) {
                DEBUG_PRINT(seq[i]);
            }

            DEBUG_PRINTLN();
        
        }

    #endif


    // Do a bubble sort ..

    for (uint8_t i = 0; i < 13; i++) {

        for (uint8_t j = i + 1; j < 13; j++) {

            if (this->cards[j].getSortValue(bidType, trumps, seq) < cards[i].getSortValue(bidType, trumps, seq)) {

                Card card;
                card.setSuit(cards[i].getSuit());
                card.setRank(cards[i].getRank());
                card.setSelected(cards[i].isSelected());

                this->cards[i] = this->cards[j];
                this->cards[j].setSuit(card.getSuit());
                this->cards[j].setRank(card.getRank());
                this->cards[j].setSelected(card.isSelected());

            }
            
        }

    }


    this->cardCount = 0;
    
    for (uint8_t i = 0; i < 13; i++) {

        if (this->cards[i].getSuit() != Suit::None) {
            
            this->cardCount++;

        }
        else {
            
            break;

        }

    }

}


void addCard(Card *card)   { 

    #if defined(DEBUG) && defined(DEBUG_PLAY_ADDCARD)
        DEBUG_PRINT(F("Card "));
        DEBUG_PRINT(cardCount);
        DEBUG_PRINT_SPACE();
        DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
        DEBUG_PRINTLN();
    #endif

    this->cards[this->cardCount].setRank(card->getRank()); 
    this->cards[this->cardCount].setSuit(card->getSuit()); 
    this->cardCount++;

}


uint8_t numberOfUnplayedCards_InSuit(Suit suit, Rank cardsAbove) {

    uint8_t count = this->gameRound->numberOfUnplayedCards_InSuit(suit, cardsAbove);


    // Remove those in the players hand ..

    for (int j = 0; j < this->cardCount; j++) {

        Card &card = this->cards[j];

        if (card.getSuit() == suit && card.getRank() > cardsAbove) {

            count--;

        }

    }


    // Remove any cards in kitty ..

    if (this->getPlayerNumber() == this->gameRound->getWinningBid_Idx()) {
        
        for (int i = 1; i < 3; i++) {

            Card *card = this->gameRound->getKitty(i);

            if (card->getSuit() == suit && card->getRank() > cardsAbove) {

                count--;

            }

        }
    
    }

    return count;

}




// Do we have the top unplayed card in the nominated suit ..

uint8_t getBottom_NonSuit(Suit suitToFollow) {


    #if defined(DEBUG) && defined(DEBUG_GETBOTTOM_NONSUIT)
        DEBUG_PRINT(F("getBottom "));
        DEBUG_PRINT_SUIT(suitToFollow);
        DEBUG_PRINT(F(": "));
    #endif
    

    // What is the players lowest trump?

    uint8_t idx = this->getLowest_NonTrump_InSuit(suitToFollow);

    if (idx == Constants::No_Card)  {

        #if defined(DEBUG) && defined(DEBUG_GETBOTTOM_NONSUIT)
            DEBUG_PRINTLN(F("player has none."));
        #endif

        return Constants::No_Card;

    }


    Card &playersLowestCard = this->cards[idx];

    #if defined(DEBUG) && defined(DEBUG_GETBOTTOM_NONSUIT)

        DEBUG_PRINT(F("player holds "));
        DEBUG_PRINT_CARD(playersLowestCard.getSuit(), playersLowestCard.getRank());
        DEBUG_PRINT(F(": "));

    #endif


    // What is the lowest unplayed card in this suit?

    Rank lowestUnplayedCard = playersLowestCard.getRank();

    for (Rank rank = Rank::Four; rank < static_cast<Rank>(static_cast<uint8_t>(playersLowestCard.getRank()) + 1); rank++) {

        if (!this->gameRound->hasCardBeenPlayed(suitToFollow, rank)) {

            #if defined(DEBUG) && defined(DEBUG_GETBOTTOM_NONSUIT)

                DEBUG_PRINT_CARD(suitToFollow, rank);
                DEBUG_PRINT(F(" not played. "));

            #endif

            lowestUnplayedCard = rank;     
            break;   

        } 
        else {

            #if defined(DEBUG) && defined(DEBUG_GETBOTTOM_NONSUIT)

                DEBUG_PRINT_CARD(suitToFollow, rank);
                DEBUG_PRINT(F(" played. "));

            #endif

        }

    }


    // So are we holding the top card?

    #if defined(DEBUG) && defined(DEBUG_GETBOTTOM_NONSUIT)
        DEBUG_PRINT(F("Lowest unplayed "));
        DEBUG_PRINT_CARD(suitToFollow, lowestUnplayedCard);
        DEBUG_PRINTLN(F("."));
    #endif

    if (lowestUnplayedCard == playersLowestCard.getRank()) {

        return idx;

    }

    return Constants::No_Card;

}


// Do we have the bottom unplayed card in the nominated suit below the nominated rank ..

uint8_t getBottom_NonSuit(Suit suitToFollow, Rank bottomRank) {


    // What is the players highest trump?

    uint8_t idx = this->getLowest_NonTrump_InSuit(suitToFollow);
    if (idx == Constants::No_Card)  return Constants::No_Card;

    Card &playersLowestCard = this->cards[idx];


    // What is the lowest unplayed card in this suit?

    Rank lowestUnplayedCard = Rank::TopCard;

    for (Rank rank = Rank::Four; rank <= bottomRank; rank++) {

        if (!this->gameRound->hasCardBeenPlayed(suitToFollow, rank)) {

            lowestUnplayedCard = rank;        

        } 

    }

    // So are we holding the bottom card?

    if (lowestUnplayedCard == playersLowestCard.getRank()) {

        return idx;

    }

    return Constants::No_Card;

}


uint8_t getNextLowest_NonTrump_InSuit(Suit suit, Rank specifiedRank) {

    Rank foundRank = Rank::None;
    uint8_t foundIdx = Constants::No_Card;

    #if defined(DEBUG) && defined(DEBUG_GETNEXTLOWEST_NONTRUMP_INSUIT)
        DEBUG_PRINT(F("getNextLowest_NonTrump_InSuit("));
        DEBUG_PRINT_SUIT(suit);
        DEBUG_PRINT(F(","));
        DEBUG_PRINT_RANK(specifiedRank);
        DEBUG_PRINT(F(") - "));
    #endif

    for (uint8_t i = 0; i < this->cardCount; i++) {
    
        Card &card = this->cards[i];

        #if defined(DEBUG) && defined(DEBUG_GETNEXTLOWEST_NONTRUMP_INSUIT)
            DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
            DEBUG_PRINT_SPACE();
        #endif


        if (card.getSuit() == suit) {

            if (card.getRank() > foundRank && card.getRank() < specifiedRank) {
            
                foundRank = card.getRank();
                foundIdx = i;

            }                

        }

    }


    if (foundIdx != Constants::No_Card) {

        Card &card = this->cards[foundIdx];

        #if defined(DEBUG) && defined(DEBUG_GETNEXTLOWEST_NONTRUMP_INSUIT)
            DEBUG_PRINT(F(" = "));
            DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
            DEBUG_PRINTLN();
        #endif

    }
    else {

        #if defined(DEBUG) && defined(DEBUG_GETNEXTLOWEST_NONTRUMP_INSUIT)
            DEBUG_PRINTLN(F(" = No_Card."));
        #endif
    
    }

    return foundIdx;

}