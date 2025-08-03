#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/utils/SpritesU.hpp"



uint8_t getJustPressedButtons() {

    a.pollButtons();

    return a.justPressedButtons();

}


uint8_t getPressedButtons() {

    return a.pressedButtons();

}


void play_CardSelected() {

    gameRound.setCurrentPlayer_Idx((gameRound.getCurrentPlayer_Idx() + 1) % 4);


    // If we ae playing misere and the player is sitting out then skip ..

    #ifdef OPEN_MISERE
    if (gameRound.winningBid_Type() == BidType::Misere || gameRound.winningBid_Type() == BidType::Open_Misere) {
    #else
    if (gameRound.winningBid_Type() == BidType::Misere) {
    #endif

        if (!game.players[gameRound.getCurrentPlayer_Idx()].isPlaying()) {

            gameRound.setCurrentPlayer_Idx((gameRound.getCurrentPlayer_Idx() + 1) % 4);

        }

    }


    // Is it the end of the round?

    if (gameRound.getFirstPlayer_Idx() == gameRound.getCurrentPlayer_Idx()) {

        #ifdef DEBUG_BASIC
            DEBUG_PRINTLN(F("-----"));
            DEBUG_PRINT(F("Trick winner "));
            DEBUG_PRINTLN(gameRound.getWinningHand());
        #endif


        // Increase tricks won count and scores ..

        switch (gameRound.getWinningBid().getBidType()) {
        
            case BidType::Misere:
            #ifdef OPEN_MISERE
            case BidType::Open_Misere:
            #endif
                gameRound.incTricksWon(gameRound.getWinningHand(BidType::No_Trumps));
                break;

            default:
                gameRound.incTricksWon(gameRound.getWinningHand());
                break;
            
        }

    
        // Did anyone not follow suit?

        Suit cardLedSuit = gameRound.getCardLed().getSuit();
        Suit trumpsSuit = gameRound.winningBid_Suit();
        bool cardLedIsTrump = gameRound.getCardLed().isTrump(trumpsSuit);
        BidType bidType = gameRound.winningBid_Type();

        for (uint8_t playerIdx = 0; playerIdx < 4; playerIdx++) {

            switch (bidType) {
            
                case BidType::No_Trumps:
                case BidType::Misere:
                #ifdef OPEN_MISERE
                case BidType::Open_Misere:
                #endif

                    if (gameRound.getHand(playerIdx)->getSuit() != cardLedSuit) {
                        gameRound.setHasSuitInHand(playerIdx, cardLedSuit, TriState::False);
                    }

                    break;

                case BidType::Suit:

                    if (cardLedIsTrump) {

                        if (!gameRound.getHand(playerIdx)->isTrump(trumpsSuit)) {
                            gameRound.setHasSuitInHand(playerIdx, cardLedSuit, TriState::False);
                        }
                    
                    }
                    else {

                        if (gameRound.getHand(playerIdx)->getSuit() != cardLedSuit) {
                            gameRound.setHasSuitInHand(playerIdx, cardLedSuit, TriState::False);
                        }

                    }

                    break;

            }

        }

        // If playing misere, did the calling hand win?

        #ifdef DEBUG_BASIC
            DEBUG_PRINT(F("Highest hand "));
            DEBUG_PRINT(gameRound.getWinningHand(BidType::No_Trumps));
            DEBUG_PRINT(F(", lowest hand "));
            DEBUG_PRINT(gameRound.getWinningHand());
            DEBUG_PRINT(F(", WinningBidIdx "));
            DEBUG_PRINTLN(gameRound.getWinningBid_Idx());
            DEBUG_PRINTLN(F("-------"));
        #endif

        #ifdef OPEN_MISERE
        if (gameRound.winningBid_Type() == BidType::Misere || gameRound.winningBid_Type() == BidType::Open_Misere) {
        #else
        if (gameRound.winningBid_Type() == BidType::Misere) {
        #endif
        
            if (gameRound.getWinningBid_Idx() == gameRound.getWinningHand(BidType::No_Trumps)) {

                gameRound.setRound(10);
            
            }
            else {

                gameRound.resetHand(gameRound.getWinningHand(BidType::No_Trumps));

            }

        }
        else {

            gameRound.resetHand(gameRound.getWinningHand());

        }
        
        gameState = GameState::Play_EndOfHand;

    }

    game.setFrameCount(0);
    highlightSuitInHand();

}

void highlightSuitInHand() {

    Suit trumpsSuit = gameRound.winningBid_Suit();
    Card &cardLed = gameRound.getCardLed();

    for (uint8_t i = 0; i < game.players[Constants::HumanPlayer].getCardCount(); i++) {

        if (game.players[Constants::HumanPlayer].getCard(i).getSuit() ==  cardLed.getSuit()) {
        
            selectedCard = i;
            game.players[Constants::HumanPlayer].clearSelection();
            game.players[Constants::HumanPlayer].getCard(selectedCard).setSelected(true);            
            return;

        }

    }

    selectedCard = 0;
    game.players[gameRound.getCurrentPlayer_Idx()].clearSelection();
    game.players[gameRound.getCurrentPlayer_Idx()].getCard(selectedCard).setSelected(true);

}