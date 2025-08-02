#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "Card.h"
#include "Bid.h"

struct GameRound {

    private:

        Bid highestBid; // Used to track bidding only.
        Bid bid[4];
        Card hand[4];
        Card kitty[3];

        uint8_t tricksWon[Constants::Player_Count];
        int16_t score[Constants::Team_Count];
        uint16_t playedCards[Constants::Suit_Count];
        TriState hasSuitInHand[Constants::Player_Count][Constants::Suit_Count];  // [player][suit];

        Suit jokerSuit = Suit::None;

        uint8_t kittyPointer = 0;
        uint8_t round = 0;
        uint8_t firstPlayer = 0;
        uint8_t currentPlayer = 0;
        uint8_t handCount = 10;
        uint8_t dealerIdx = 0;
        uint8_t winningBidIdx = Constants::WinningBid_None;   // Index to bid array ..

        bool playedJoker = false;

    public:

        Card *getHand(uint8_t playerIdx)                            { return &this->hand[playerIdx]; }
        Card *getKitty(uint8_t idx)                                 { return &this->kitty[idx]; }
        Suit getJokerSuit()                                         { return this->jokerSuit; }
        Bid &getBid(uint8_t playerIdx)                              { return this->bid[playerIdx]; }
        Bid &getHighestBid()                                        { return this->highestBid; }
        uint8_t getKittyPointer()                                   { return this->kittyPointer; }
        int16_t getScore(uint8_t teamIdx)                           { return this->score[teamIdx]; }
        uint8_t getRound()                                          { return this->round; }
        uint8_t getFirstPlayer_Idx()                                { return this->firstPlayer; }
        uint8_t getCurrentPlayer_Idx()                              { return this->currentPlayer; }
        uint8_t getHandCount()                                      { return this->handCount; }
        uint8_t getWinningBid_Idx()                                 { return this->winningBidIdx; }
        uint8_t getDealer_Idx()                                     { return this->dealerIdx; }
        bool getPlayedJoker()                                       { return this->playedJoker; }

        void setJokerSuit(Suit val)                                 { this->jokerSuit = val; }
        void setKitty(uint8_t idx, Card &kitty)                     { this->kitty[idx].setSuit(kitty.getSuit()); this->kitty[idx].setRank(kitty.getRank()); }
        void setKittyPointer(uint8_t idx)                           { this->kittyPointer = idx; }
        void setScore(uint8_t teamIdx, int16_t val)                 { this->score[teamIdx] = val; }
        void setRound(uint8_t val)                                  { this->round = val; }
        void setFirstPlayer_Idx(uint8_t val)                        { this->firstPlayer = val; }
        void setCurrentPlayer_Idx(uint8_t val)                      { this->currentPlayer = val; }
        void setHandCount(uint8_t val)                              { this->handCount = val; }
        void setWinningBid_Idx(uint8_t val)                         { this->winningBidIdx = val; }
        void setDealer_Idx(uint8_t val)                             { this->dealerIdx = val % 4; }
        void sgtPlayedJoker(bool val)                               { this->playedJoker = val; }


        uint8_t getTableCardCount() {

            uint8_t count = 0;

            if (this->hand[0].getSuit() != Suit::None) count++;
            if (this->hand[1].getSuit() != Suit::None) count++;
            if (this->hand[2].getSuit() != Suit::None) count++;
            if (this->hand[3].getSuit() != Suit::None) count++;
            
            return count;

        }

        void setHighestBid(Bid &bid) {
        
            this->highestBid.setBidScore(bid.getBidScore());
            this->highestBid.setBidType(bid.getBidType());
            this->highestBid.setLevel(bid.getLevel());
            this->highestBid.setSuit(bid.getSuit());
            
        }
 
        TriState getHasSuitInHand(uint8_t playerIdx, Suit suitIdx) {

            return this->hasSuitInHand[playerIdx][static_cast<uint8_t>(suitIdx)];

        }

        void setHasSuitInHand(uint8_t playerIdx, Suit suitIdx, TriState val) {

            this->hasSuitInHand[playerIdx][static_cast<uint8_t>(suitIdx)] = val;
            
        }

        void incScore(uint8_t teamIdx, int16_t val) { 

            this->score[teamIdx] = this->score[teamIdx] + val; 

        }

        void incRound() { 

            this->round++;
            
        }

        Bid &getWinningBid() {

            return this->bid[this->winningBidIdx];

        }


        uint8_t getWinningTeam() {
        
            Bid bid = this->getWinningBid();

            switch (bid.getBidType()) {
            
                case BidType::No_Trumps:
                case BidType::Suit:

                    switch (getWinningBid_Team()) {
                    
                        case 0:
                            return (this->tricksWon[0] + this->tricksWon[2] >= bid.getLevel()) ? 0 : 1;

                        case 1:
                            return (this->tricksWon[1] + this->tricksWon[3] >= bid.getLevel()) ? 1 : 0;

                    }

                    break;

                case BidType::Misere:
                #ifdef OPEN_MISERE
                case BidType::Open_Misere:
                #endif

                    switch (getWinningBid_Team()) {
                    
                        case 0:
                            return (this->tricksWon[0] + this->tricksWon[2] == 0) ? 0 : 1;

                        case 1:
                            return (this->tricksWon[1] + this->tricksWon[3] == 0) ? 1 : 0;

                    }

                    break;

            }

            return 0;

        }


        uint8_t getTeam_TrickCount(uint8_t teamIdx) {
        
            switch (teamIdx) {
            
                case 0:
                case 2:
                    return this->tricksWon[0] + this->tricksWon[2];

                default:
                    return this->tricksWon[1] + this->tricksWon[3];
                
            }

        }


        uint8_t getWinningBid_Team() {
        
            switch (this->winningBidIdx) {
            
                case 0:
                case 2:
                    return 0;

                default:
                    return 1;
                
            }

        }


        int16_t getWinningScore_BidTeam() {
      
            Bid winningBid = this->getWinningBid();
            return winningBid.getScore() * (this->getWinningTeam() == this->getWinningBid_Team() ? 1 : -1);

        }


        uint8_t getWinningScore_AltTeam() {
        
            Bid winningBid = this->getWinningBid();

            switch (winningBid.getBidType()) {
            
                case BidType::No_Trumps:
                case BidType::Suit:

                    return this->getTeam_TrickCount(this->getWinningBid_Team() == 0 ? 1 : 0) * 10;

                case BidType::Misere:
                #ifdef OPEN_MISERE
                case BidType::Open_Misere:
                #endif
                    return 0;

            }

        }


        void incTricksWon(uint8_t playerIdx) {

            this->tricksWon[playerIdx]++;

        }


        void clearKitty() {
        
            for (uint8_t i = 0; i < 3; i++) {

                this->kitty[i].setRank(Rank::None);
                this->kitty[i].setSuit(Suit::None);

            }

            this->kittyPointer = 0;
            
        }


        void addKitty(Card *card) {

            this->kitty[this->kittyPointer].setRank(card->getRank());
            this->kitty[this->kittyPointer].setSuit(card->getSuit());
            this->kittyPointer++;
            
        }


        void resetRound() {
        
            this->winningBidIdx = 255;
            this->firstPlayer = 0;
            this->currentPlayer = 0;
            this->jokerSuit = Suit::None;
            this->playedJoker = false;
            this->handCount = 10;
            this->highestBid.reset(Constants::No_Player);

            for (uint8_t i = 0; i < 4; i++) {

                this->bid[i].reset(i);
                this->hand[i].reset();
                this->playedCards[i] = 0;
                this->tricksWon[i] = 0;

                for (uint8_t playerIdx = 0; playerIdx < 4; playerIdx++) {
                    this->hasSuitInHand[playerIdx][i] = TriState::Maybe; 
                }

            }

            for (uint8_t i = 0; i < 3; i++) {

                this->kitty[i].reset();

            }

            this->kittyPointer = 0;

        }


        void resetHand(uint8_t winningPlayer) {
        
            this->firstPlayer = winningPlayer;
            this->currentPlayer = winningPlayer;
            this->handCount--;

        }

        void resetHand() {

            for (uint8_t i = 0; i < 4; i++) {
                this->hand[i].reset();
            }

        }


        bool hasPlayedCard(uint8_t playerIdx) {

            Card *card = this->getHand(playerIdx);

            return card->getRank() != Rank::None;

        }
        

        Card &getCardLed() {
        
            return this->hand[this->firstPlayer];

        }


        bool hasCardBeenPlayed(Suit suit, Rank rank) {

            uint16_t mask = 1 << static_cast<uint8_t>(rank);

            if (rank == Rank::Joker) {

                #if defined(DEBUG) && defined(DEBUG_HASCARDBEENPLAYED)
                    DEBUG_PRINT(F("Has card been played? "));
                    DEBUG_PRINT_RANK(rank);
                    DEBUG_PRINT_SPACE();
                    DEBUG_PRINTLN(playedJoker);
                #endif
                            
                return playedJoker;

            }
            else {

                #if defined(DEBUG) && defined(DEBUG_HASCARDBEENPLAYED)
                    DEBUG_PRINT(F("Has card been played? "));
                    DEBUG_PRINT_RANK(rank);
                    DEBUG_PRINT_SUIT(suit);
                    DEBUG_PRINT_SPACE();
                    DEBUG_PRINTLN(this->playedCards[static_cast<uint8_t>(suit)] & mask);
                #endif

                if ((this->playedCards[static_cast<uint8_t>(suit)] & mask) > 0) {

                    return true;        

                } 

            }

            return false;

        }


        void markCardPlayed(Suit suit, Rank rank) {

            #if defined(DEBUG) && defined(DEBUG_MARKCARDPLAYED)
                DEBUG_PRINT(F("Mark Card "));
                DEBUG_PRINT_RANK(rank);
                DEBUG_PRINT_SUIT(suit);
                DEBUG_PRINTLN();
            #endif
            
            if (rank == Rank::Joker) {
            
                this->playedJoker = true;

            }
            else {

                uint16_t mask = 1 << static_cast<uint8_t>(rank);
                this->playedCards[static_cast<uint8_t>(suit)] = this->playedCards[static_cast<uint8_t>(suit)] | mask;

            }

        }


        Suit winningBid_Suit() {

            if (this->winningBidIdx == Constants::WinningBid_None) { 
                return Suit::None;
            }

            return this->bid[this->winningBidIdx].getSuit();
            
        }


        BidType winningBid_Type() {

            return this->bid[this->winningBidIdx].getBidType();
            
        }


        Suit suitPlayerCalled(uint8_t playerIdx) {

            return this->bid[playerIdx].getSuit();

        }


        bool isLastPlayer(uint8_t playerIdx) {

            return ((this->firstPlayer + 3) % 4 == (playerIdx % 4));

        }


        TriState hasTrumps(Suit trumps, uint8_t playerIdx) {

            return canPlayerFollowSuit(winningBid_Suit(), playerIdx);

        }


        TriState canPlayerFollowSuit(Suit suitToFollow, uint8_t playerIdx) {
//SJH should consider what is in the current players hand.  
            Suit trumps = winningBid_Suit();
            bool hasSuit = false;

            if (suitToFollow == trumps) {
                
                if (this->hasSuitInHand[playerIdx][static_cast<uint8_t>(suitToFollow)] == TriState::False) {
                    return TriState::False;
                }
                else if (this->hasSuitInHand[playerIdx][static_cast<uint8_t>(suitToFollow)] == TriState::Maybe) {
                    return TriState::Maybe;
                }
                else if (!this->playedJoker) {
                    return TriState::Maybe;
                }
                else if (this->hasCardBeenPlayed(getTrump_AltSuit(suitToFollow), Rank::Jack)) {
                    return TriState::Maybe;
                }

                return TriState::False;
            
            }
            else {
            
                return this->hasSuitInHand[playerIdx][static_cast<uint8_t>(suitToFollow)];

            }

        }


        uint8_t numberOfUnplayedCards_InSuit(Suit suit, Rank cardsAbove) {

            uint8_t count = 0;
            Suit trumps = winningBid_Suit();
            
            // Reposition to bottom of the deck if no 'cardsAbove' provided ..

            if (cardsAbove == Rank::None) {

                switch (suit) {
                
                    case Suit::Spades:
                    case Suit::Clubs:
                        cardsAbove = Rank::Four;
                        break;
                
                    case Suit::Diamonds:
                    case Suit::Hearts:
                        cardsAbove = Rank::Three;
                        break;
              
                }

            }

            #if defined(DEBUG) && defined(DEBUG_NUMBER_OF_UNPLAYED_CARDS)
                DEBUG_PRINT(F("numberOfUnplayedCards_InSuit("));
                DEBUG_PRINT_SUIT(suit);
                DEBUG_PRINT(F(", "));
                DEBUG_PRINT_RANK(cardsAbove);
                DEBUG_PRINT(F(") "));
            #endif

            switch (suit) {
            
                case Suit::Spades:
                case Suit::Clubs:

                    for (Rank rank = cardsAbove; rank <= Rank::Ace; rank++) {

                        if (rank != cardsAbove && !this->hasCardBeenPlayed(suit, rank)) {

                            #if defined(DEBUG) && defined(DEBUG_NUMBER_OF_UNPLAYED_CARDS)
                                DEBUG_PRINT_CARD(suit, rank);
                                DEBUG_PRINT(F(", "));
                            #endif

                            count++;
                        }

                    }

                    break;
            
                case Suit::Hearts:
                case Suit::Diamonds:

                    for (Rank rank = cardsAbove; rank <= Rank::Ace; rank++) {

                        if (rank != cardsAbove && !this->hasCardBeenPlayed(suit, rank)) {

                            #if defined(DEBUG) && defined(DEBUG_NUMBER_OF_UNPLAYED_CARDS)
                                DEBUG_PRINT_CARD(suit, rank);
                                DEBUG_PRINT(F(", "));
                            #endif

                            count++;
                        }

                    }

                    break;
                    
            }

            if (suit == trumps) { 
            
                // Left bower ..

                if (cardsAbove < Rank::Left_Bower && !this->hasCardBeenPlayed(getTrump_AltSuit(suit), Rank::Jack)) {

                    #if defined(DEBUG) && defined(DEBUG_NUMBER_OF_UNPLAYED_CARDS)
                        DEBUG_PRINT(F("LB, "));
                    #endif

                    count++;
                }
            
                // Joker ..

                if (cardsAbove < Rank::Joker && !this->hasCardBeenPlayed(Suit::None, Rank::Joker)) {

                    #if defined(DEBUG) && defined(DEBUG_NUMBER_OF_UNPLAYED_CARDS)
                        DEBUG_PRINT(F("Joker, "));
                    #endif

                    count++;
                }
            
            }


            //SJH remove count of any coards in players hand or in the kitty (if player is winning bid)

            #if defined(DEBUG) && defined(DEBUG_NUMBER_OF_UNPLAYED_CARDS)
                DEBUG_PRINT(F(" = "));
                DEBUG_PRINTLN(count);
            #endif

            return count;

        }


        bool hasTrumpsBeenLed() {

            BidType bidType = winningBid_Type();
            Suit trumps = winningBid_Suit();

            #ifdef OPEN_MISERE
            if (bidType == BidType::Misere || bidType == BidType::Open_Misere || bidType == BidType::No_Trumps) return false;
            #else
            if (bidType == BidType::Misere || bidType == BidType::No_Trumps) return false;
            #endif

            if (this->firstPlayer == this->currentPlayer) return false;     // No one has played a card ..

            return this->hand[this->firstPlayer].isTrump(trumps);

        }


        bool hasHandBeenTrumped() {

            BidType bidType = winningBid_Type();
            Suit trumps = winningBid_Suit();

            #ifdef OPEN_MISERE
            if (bidType == BidType::Misere || bidType == BidType::Open_Misere || bidType == BidType::No_Trumps) return false;
            #else
            if (bidType == BidType::Misere || bidType == BidType::No_Trumps) return false;
            #endif

            for (uint8_t i = 0; i < 4; i++) {

                Card &card = this->hand[i];

                if (this->hand[i].isTrump(trumps)) {

                    return true;

                }

            }

            return false;

        }


        bool weWonCall(uint8_t playerIdx, uint8_t partnerIdx) {

            return (this->winningBidIdx == playerIdx || this->winningBidIdx == partnerIdx);

        }


        bool isFirstRound() {
            return this->round == 1;
        }


        uint8_t getUnplayedCountofHigherCards_InSuit(Card &card) {

            uint8_t count = 0;

            for (Rank rank = static_cast<Rank>(static_cast<uint8_t>(card.getRank()) + 1); rank <= Rank::Ace; rank++) {

                if (!this->hasCardBeenPlayed(card.getSuit(), rank)) {
                    count++;
                }

            }
            
            return count;

        }


        Card &getLargestCardInPlay(Suit trumps) {

            Suit cardLedSuit = this->hand[this->firstPlayer].getSuit();
            uint8_t returnIdx = Constants::No_Card;
            Rank rank = Rank::None;
            bool hasBeenTrumped = this->hasHandBeenTrumped();

            #if defined(DEBUG) && defined(DEBUG_GETLARGESTCARDINPLAY)
                DEBUG_PRINT(F("getLargestCardInPlay() trumps "));
                DEBUG_PRINT_SUIT(trumps);
                DEBUG_PRINT(F(", suit led "));
                DEBUG_PRINT_SUIT(cardLedSuit);
            #endif

            if (hasBeenTrumped) {

                for (uint8_t i = 0; i < 4; i++) {

                    Card &card = this->hand[i];

                    #if defined(DEBUG) && defined(DEBUG_GETLARGESTCARDINPLAY)
                        DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                        DEBUG_PRINT_SPACE();
                    #endif

                    if (card.getSuit() == trumps && card.getRank() > rank) {

                        rank = card.getRank();
                        returnIdx = i;

                    }

                }

            }
            else {

                for (uint8_t i = 0; i < 4; i++) {

                    Card &card = this->hand[i];

                    #if defined(DEBUG) && defined(DEBUG_GETLARGESTCARDINPLAY)
                        DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                        DEBUG_PRINT_SPACE();
                    #endif

                    if (card.getSuit() == cardLedSuit && card.getRank() > rank) {

                        rank = card.getRank();
                        returnIdx = i;

                    }

                }

            }

            if (returnIdx != Constants::No_Card) {

                #if defined(DEBUG) && defined(DEBUG_GETLARGESTCARDINPLAY)
                    Card &card = this->hand[returnIdx];
                    DEBUG_PRINT(F(" = "));
                    DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                    DEBUG_PRINTLN(F(" "));
                #endif

                return this->hand[returnIdx];

            }
            else {

                #if defined(DEBUG) && defined(DEBUG_GETLARGESTCARDINPLAY)
                    DEBUG_PRINTLN(F(" = (not found)"));
                #endif

                return this->hand[this->currentPlayer];            // Return current players card which should be a Rank::None.

            }

        }


        Card &getLargestTrumpInPlay(Suit trumps) {

            Rank rank = Rank::None;
            uint8_t returnIdx = Constants::No_Card;

            for (uint8_t i = 0; i < 4; i++) {

                Card &card = this->hand[i];

                if (card.isTrump(trumps) && card.getRank() > rank) {

                    rank = card.getRank();
                    returnIdx = i;

                }

            }

            if (returnIdx != Constants::No_Card) {

                return this->hand[returnIdx];

            }
            else {
            
                return this->hand[this->currentPlayer];            // Return current players card which should be a Rank::None.
            }

        }


        bool isPlayerWinningHand(uint8_t playerIdx) {

            uint8_t winningHand = this->getWinningHand();
            return (playerIdx == winningHand);

        }


        uint8_t getWinningHand() {

            return this->getWinningHand(this->bid[this->winningBidIdx].getBidType());

        }


        uint8_t getWinningHand(BidType bidType) {

            #if defined(DEBUG) && defined(DEBUG_PLAYER_WINNING_HAND)

                DEBUG_PRINT(F("getWinningHand() "));

                for (uint8_t i = 0; i < 4; i++) {
                    Card &card = this->hand[i];
                    DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                    DEBUG_PRINT_SPACE();
                }

                DEBUG_PRINT_SPACE();

            #endif

            uint8_t returnIdx = Constants::No_Card;
            Rank rank = Rank::None;

            Suit trumps = winningBid_Suit();
            Suit cardLedSuit = this->hand[this->firstPlayer].getSuit();

            #if defined(DEBUG) && defined(DEBUG_PLAYER_WINNING_HAND)

                DEBUG_PRINT(F("Card led suit: "));            
                DEBUG_PRINT_SUIT(cardLedSuit);
                DEBUG_PRINTLN();

            #endif

            switch (bidType) {
            
                case BidType::Suit:

                    if (this->hasHandBeenTrumped()) {

                        #if defined(DEBUG) && defined(DEBUG_PLAYER_WINNING_HAND)
                            DEBUG_PRINTLN(F("Trick has been trumped."));            
                        #endif

                        for (uint8_t i = 0; i < 4; i++) {

                            Card &card = this->hand[i];

                            if (card.isTrump(trumps) && card.getRank() > rank) {

                                rank = card.getRank();
                                returnIdx = i;

                            }

                        }       

                    }
                    else {

                        #if defined(DEBUG) && defined(DEBUG_PLAYER_WINNING_HAND)
                            DEBUG_PRINTLN(F("Trick has not been trumped."));            
                        #endif

                        for (uint8_t i = 0; i < 4; i++) {

                            Card &card = this->hand[i];

                            if (card.getSuit() == cardLedSuit && card.getRank() > rank) {

                                rank = card.getRank();
                                returnIdx = i;

                            }

                        }       
                    
                    }
                               
                    #if defined(DEBUG) && defined(DEBUG_PLAYER_WINNING_HAND)

                        DEBUG_PRINT(F(", BidType::Suit Win:"));
                        DEBUG_PRINTLN(returnIdx);

                    #endif

                    return returnIdx;

                case BidType::Misere:
                #ifdef OPEN_MISERE
                case BidType::Open_Misere:
                #endif

                    rank = Rank::TopCard;

                    for (uint8_t i = 0; i < 4; i++) {

                        Card &card = this->hand[i];

                        if ((card.getSuit() == cardLedSuit && card.getRank() < rank) || card.getRank() == Rank::Joker) {

                            rank = card.getRank();
                            returnIdx = i;

                        }

                    }       
                               
                    #if defined(DEBUG) && defined(DEBUG_PLAYER_WINNING_HAND)

                        DEBUG_PRINT(F(", BidType::Misere Win:"));
                        DEBUG_PRINTLN(returnIdx);

                    #endif

                    return returnIdx;

                case BidType::No_Trumps:

                    for (uint8_t i = 0; i < 4; i++) {

                        Card &card = this->hand[i];

                        if ((card.getSuit() == cardLedSuit && card.getRank() > rank) || card.getRank() == Rank::Joker) {

                            rank = card.getRank();
                            returnIdx = i;

                        }

                    }       

                    #if defined(DEBUG) && defined(DEBUG_PLAYER_WINNING_HAND)

                        DEBUG_PRINT(F(", BidType::No_Trumps Win:"));
                        DEBUG_PRINTLN(returnIdx);

                    #endif

                    return returnIdx;
            
            }

        }

};
