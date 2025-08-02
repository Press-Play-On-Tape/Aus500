
#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "Card.h"
#include "SuitPile.h"
#include "GameRound.h"



// Trump cards -----------------------------------------------------------------

bool playJoker() {

    for (uint8_t i = 0; i < this->cardCount; i++) {
    
        Card &card = this->cards[i];

        if (card.getRank() == Rank::Joker) {

            this->playCard(i);
            return true;

        }

    }

    return false;

}

bool isCardInTrick(Suit suit, Rank rank) {

    for (uint8_t i = 0; i < 4; i++) {
    
        Card *card = this->gameRound->getHand(i);

        if (card->getRank() == Rank::Joker || (card->getSuit() == suit && card->getRank() == rank)) {
        
            return true;

        }

    }

    return false;

}

// If the player has the top, unplayed trump then play it ..

// bool playTop_Trump(Suit trumps) {

    
//     // If we have the Joker then play it automatically ..

//     if (this->playJoker())    return true;


//     // What is the players highest trump?

//     uint8_t idx = this->getHighest_InSuit(trumps);
//     if (idx == Constants::No_Card)  return false;

//     Card &playersHighestCard = this->cards[idx];


//     // What is the highest unplayed trump?

//     Rank highestUnplayedCard = Rank::None;

//     for (Rank rank = Rank::Four; rank <= Rank::Ace; rank++) {

//         if (!this->gameRound->hasCardBeenPlayed(trumps, rank) && !this->isCardInTrick(trumps, rank)) {

//             highestUnplayedCard = rank;        

//         } 

//     }


//     // Test the left bower separately ..

//     if (!this->gameRound->hasCardBeenPlayed(getTrump_AltSuit(trumps), Rank::Jack) && !this->isCardInTrick(getTrump_AltSuit(trumps), Rank::Jack)) {

//         highestUnplayedCard = Rank::Left_Bower;        

//     } 


//     // Test the right bower separately ..

//     if (!this->gameRound->hasCardBeenPlayed(trumps, Rank::Jack) && !this->isCardInTrick(trumps, Rank::Jack)) {

//         highestUnplayedCard = Rank::Right_Bower;        

//     } 


//     // Test the joker separately ..

//     if (!this->gameRound->getPlayedJoker() && !this->isCardInTrick(trumps, Rank::Joker)) {

//         highestUnplayedCard = Rank::Joker;        

//     } 


//     // So are we holding the top card?

//     if (highestUnplayedCard == playersHighestCard.getRank()) {

//         this->playCard(idx);
//         return true;

//     }

//     return false;

// }


// // Play the lowest trump in the player's hand ..

// bool playLowest_Trump(Suit trumps) {

//     uint8_t idx = this->getLowest_InSuit(trumps);

//     if (idx == Constants::No_Card)  {
    
//         return false;

//     }
//     else {

//         this->playCard(idx);
//         return true;

//     }

// }


// Play the lowest trump in the player's hand that is below the specified rank ..

bool playLowest_LowerThan_InSuit(Suit trumps, Rank rank) {

    uint8_t idx = this->getLowest_InSuit(trumps, rank);

    if (idx == Constants::No_Card)  {
    
        return false;

    }
    else {

        this->playCard(idx);
        return true;

    }

}


// Play the highest trump in the player's hand ..

// bool playHighest_Trump(Suit trumps) {

//     uint8_t idx = this->getHighest_InSuit(trumps);

//     if (idx == Constants::No_Card)  {
    
//         return false;

//     }
//     else {

//         this->playCard(idx);
//         return true;

//     }

// }


// Play the highest trump in the player's hand that is above the specified rank ..

// bool playHighest_LargerThan_InSuit(Suit suit, Rank rank) {   

//     uint8_t idx = this->getHighest_InSuit(suit, rank);

//     if (idx == Constants::No_Card)  {
    
//         return false;

//     }
//     else {

//         this->playCard(idx);
//         return true;

//     }

// }


// Play the next highest trump above the specified one ..

// bool playNextHighest_Trump_LargerThan(Suit trumps, Rank rank) {   

//     uint8_t idx = this->getNextHighestTrump(trumps, rank);

//     if (idx == Constants::No_Card)  {
    
//         return false;

//     }
//     else {

//         this->playCard(idx);
//         return true;

//     }

// }




// Non trump cards ------------------------------------------------------------------------------------


// Play the lowest card in the specified suit ..

bool playLowest_InSuit(Suit suitToFollow) {

    uint8_t idx = this->getLowest_InSuit(suitToFollow);

    if (idx == Constants::No_Card)  {
    
        return false;

    }
    else {

        this->playCard(idx);
        return true;

    }

}


// Play the highest card in the specified suit ..

bool playHighest_InSuit(Suit suitToFollow) {

    uint8_t idx = this->getHighest_InSuit(suitToFollow);

    if (idx == Constants::No_Card)  {
    
        return false;

    }
    else {

        this->playCard(idx);
        return true;

    }

}


// Play the highest card in the any suit greater than the specified rank ..

bool playHighest_LargerThan_AllSuit(Rank rank) {
    
    return playHighest_LargerThan_AllSuit(Suit::None, rank);

}


// Play the highest card in the any non trump suit greater than the specified rank ..

bool playHighest_LargerThan_AllSuit(Suit excludeTrumps, Rank rank) {

    uint8_t returnIdx = Constants::No_Card;

    for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {
    
        if (suit != excludeTrumps) {

            uint8_t idx = this->getHighest_InSuit(suit);

            if (idx != Constants::No_Card)  {

                Card &card = this->cards[idx];

                if (card.getRank() > rank) {
                    
                    returnIdx = idx;
                    rank = card.getRank();

                }

            }

        }

    }


    // Play the best card we found ..

    if (returnIdx != Constants::No_Card)  {

        this->playCard(returnIdx);
        return true;

    }

    return false;

}


// Play the lowest card in the any non trump suit ..

bool playLowest_AllSuit() {

    return playLowest_AllSuit(Suit::None);

}


// Play the lowest card in the any non trump suit ..

bool playLowest_AllSuit(Suit excludeSuit) {

    Rank rank = Rank::Joker;
    uint8_t returnIdx = Constants::No_Card;

    for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {
    
        if (suit != excludeSuit) {

            uint8_t idx = this->getLowest_InSuit(suit);

            if (idx != Constants::No_Card)  {

                Card &card = this->cards[idx];

                if (card.getRank() < rank) {
                   
                    returnIdx = idx;
                    rank = card.getRank();

                }

            }

        }

    }


    // Play the best card we found ..

    if (returnIdx != Constants::No_Card)  {

        this->playCard(returnIdx);
        return true;

    }

    return false;

}


bool playTop_InSuit(Suit suit) { 

    uint8_t idx = this->getTop_InSuit(suit);

    if (idx != Constants::No_Card)  {

        this->playCard(idx);
        return true;

    }

    return false;

}


bool playTop_AllSuit() { 

    return playTop_AllSuit(Suit::None);

}

bool playTop_AllSuit(Suit excludeTrumps) { 

    for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {
    
        if (suit != excludeTrumps) {

            uint8_t idx = this->getTop_InSuit(suit);

            if (idx != Constants::No_Card)  {

                this->playCard(idx);
                return true;

            }

        }

    }

    return false;

}


bool playBottom_AllSuit() { 

    return playBottom_AllSuit(Suit::None);

}

bool playBottom_AllSuit(Suit excludeTrumps) { 

    for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {
    
        if (suit != excludeTrumps) {

            uint8_t idx = this->getBottom_InSuit(suit);

            if (idx != Constants::No_Card)  {

                this->playCard(idx);
                return true;

            }

        }

    }

    return false;

}


bool playBottom_InSuit(Suit suit) { 

    uint8_t idx = this->getBottom_InSuit(suit);

    if (idx != Constants::No_Card)  {

        this->playCard(idx);
        return true;

    }

    return false;

}

bool playLowest_WithSecondHighest_InSuit(Suit suitToFollow) {


    // What is the second highest unplayed card in this suit?

    Rank highestUnplayedCard = Rank::None;

    for (Rank rank = Rank::Four; rank <= Rank::Ace; rank++) {

        if (!this->gameRound->hasCardBeenPlayed(suitToFollow, rank)) {

            highestUnplayedCard = rank;        

        } 

    }


    // If the lowest card is a Red four or Black five then the highest card is also the lowest ..

    if ((suitToFollow == Suit::Diamonds || suitToFollow == Suit::Hearts) && highestUnplayedCard == Rank::Four) return false;
    if ((suitToFollow == Suit::Clubs    || suitToFollow == Suit::Spades) && highestUnplayedCard == Rank::Five) return false;


    // So are we holding the top card?

    highestUnplayedCard--;
    uint8_t haveSecondLowestCard = this->getTop_InSuit(suitToFollow, highestUnplayedCard);

    if (haveSecondLowestCard != Constants::No_Card) {

        uint8_t lowCard = this->getLowest_InSuit(suitToFollow, highestUnplayedCard);

        if (lowCard != Constants::No_Card) {

            this->playCard(lowCard);
            return true;

        }

    }

    return false;

}



bool playLowest_WithSecondHighest_AllSuit(Suit excludeTrumps) { 

    for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {
    
        if (suit != excludeTrumps) {

            bool cardPlayed = this->playLowest_WithSecondHighest_InSuit(suit);

            if (cardPlayed)  {

                return true;

            }

        }

    }

    return false;

}


// Play the lowest card in the any non trump suit ..

bool playLowest_ExcludeSuit(Suit excludeTrumps, Suit excludeSuit1, Suit excludeSuit2) {

    Rank rank = Rank::Joker;
    uint8_t returnIdx = Constants::No_Card;

    for (Suit suit = Suit::Spades; suit <= Suit::Hearts; suit++) {
    
        if (suit != excludeTrumps && suit != excludeSuit1 && suit != excludeSuit2) {

            uint8_t idx = this->getLowest_InSuit(suit);

            if (idx != Constants::No_Card)  {

                Card &card = this->cards[idx];

                if (card.getRank() < rank) {
                    
                    returnIdx = idx;
                    rank = card.getRank();

                }

            }

        }

    }


    // Play the best card we found ..

    if (returnIdx != Constants::No_Card)  {

        this->playCard(returnIdx);
        return true;

    }

    return false;

}

bool playHighest_LargerThan_InSuit(Suit suitToFollow, Rank rank) {

    uint8_t idx = this->getHighest_InSuit(suitToFollow, rank);

    if (idx != Constants::No_Card)  {

        this->playCard(idx);
        return true;

    }

    return false;
}


bool playNextHighest_LargerThan_InSuit(Suit suitToFollow, Rank rank) {

    uint8_t idx = this->getNextHighest_InSuit(suitToFollow, rank);

    if (idx != Constants::No_Card)  {

        this->playCard(idx);
        return true;

    }

    return false;
}


bool playNextLowest_SmallerThan_InSuit(Suit suitToFollow, Rank rank) {

    uint8_t idx = this->getNextLowest_InSuit(suitToFollow, rank);

    if (idx != Constants::No_Card)  {

        this->playCard(idx);
        return true;

    }

    return false;
}