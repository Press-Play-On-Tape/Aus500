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


void saveCookie(bool enableLEDs) {

    #ifdef USE_LED
    if (enableLEDs) {
        #ifndef MICROCADE
            a.setRGBled(RED_LED, 32);
        #else
            a.setRGBledGreenOff();
            a.setRGBledBlueOff();
        #endif
    }
    #endif

    // FX::saveGameState(cookie);

    #ifdef USE_LED
    if (enableLEDs) {
        #ifndef MICROCADE
            a.setRGBled(RED_LED, 0);
            a.setRGBled(GREEN_LED, 32);
        #else
            a.setRGBledRedOff();
            a.setRGBledGreenOn();
        #endif
    }
    #endif

}


void deactivateLEDs() {

    #ifdef USE_LED
        #ifndef MICROCADE
            a.setRGBled(0, 0, 0);
        #else
            a.setRGBledRedOff();
            a.setRGBledGreenOff();
            a.setRGBledBlueOff();
        #endif
    #endif

}


void play_CardSelected() {

    gameRound.setCurrentPlayer((gameRound.getCurrentPlayer() + 1) % 4);


    // If we ae playing misere and the player is sitting out then skip ..

    #ifdef OPEN_MISERE
    if (gameRound.winningBid_Type() == BidType::Misere || gameRound.winningBid_Type() == BidType::Open_Misere) {
    #else
    if (gameRound.winningBid_Type() == BidType::Misere) {
    #endif

        if (!game.players[gameRound.getCurrentPlayer()].isPlaying()) {

            gameRound.setCurrentPlayer((gameRound.getCurrentPlayer() + 1) % 4);

        }

    }


    // Is it the end of the round?

    if (gameRound.getFirstPlayer() == gameRound.getCurrentPlayer()) {

        #ifdef DEBUG_BASIC
            DEBUG_PRINTLN(F("-----"));
            DEBUG_PRINT(F("Trick winner "));
            DEBUG_PRINTLN(game.gameRound->getWinningHand());
        #endif


        // Increase tricks won count and scores ..

        switch (game.gameRound->getWinningBid().getBidType()) {
        
            case BidType::Misere:
            #ifdef OPEN_MISERE
            case BidType::Open_Misere:
            #endif
                game.gameRound->incTricksWon(game.gameRound->getWinningHand(BidType::No_Trumps));
                break;

            default:
                game.gameRound->incTricksWon(game.gameRound->getWinningHand());
                break;
            
        }

    
        // Did anyone not follow suit?

        Suit cardLedSuit = game.gameRound->getCardLed().getSuit();
        Suit trumpsSuit = game.gameRound->winningBid_Suit();
        bool cardLedIsTrump = game.gameRound->getCardLed().isTrump(trumpsSuit);
        BidType bidType = game.gameRound->winningBid_Type();

        for (uint8_t playerIdx = 0; playerIdx < 4; playerIdx++) {

            switch (bidType) {
            
                case BidType::No_Trumps:
                case BidType::Misere:
                #ifdef OPEN_MISERE
                case BidType::Open_Misere:
                #endif

                    if (game.gameRound->getHand(playerIdx)->getSuit() != cardLedSuit) {
                        game.gameRound->setHasSuitInHand(playerIdx, cardLedSuit, TriState::False);
                    }

                    break;

                case BidType::Suit:

                    if (cardLedIsTrump) {

                        if (!game.gameRound->getHand(playerIdx)->isTrump(trumpsSuit)) {
                            game.gameRound->setHasSuitInHand(playerIdx, cardLedSuit, TriState::False);
                        }
                    
                    }
                    else {

                        if (game.gameRound->getHand(playerIdx)->getSuit() != cardLedSuit) {
                            game.gameRound->setHasSuitInHand(playerIdx, cardLedSuit, TriState::False);
                        }

                    }

                    break;

            }

        }

        // If playing misere, did the calling hand win?

        #ifdef DEBUG_BASIC
            DEBUG_PRINT(F("Highest hand "));
            DEBUG_PRINT(game.gameRound->getWinningHand(BidType::No_Trumps));
            DEBUG_PRINT(F(", lowest hand "));
            DEBUG_PRINT(game.gameRound->getWinningHand());
            DEBUG_PRINT(F(", WinningBidIdx "));
            DEBUG_PRINTLN(game.gameRound->getWinningBid_Idx());
            DEBUG_PRINTLN(F("-------"));
        #endif

        #ifdef OPEN_MISERE
        if (gameRound.winningBid_Type() == BidType::Misere || gameRound.winningBid_Type() == BidType::Open_Misere) {
        #else
        if (gameRound.winningBid_Type() == BidType::Misere) {
        #endif
        
            if (game.gameRound->getWinningBid_Idx() == game.gameRound->getWinningHand(BidType::No_Trumps)) {

                gameRound.setRound(10);
                gameState = GameState::Play_EndOfHand;// SJH should be end of round but results written on end of hand
            
            }
            else {

                game.gameRound->resetHand(game.gameRound->getWinningHand(BidType::No_Trumps));
                gameState = GameState::Play_EndOfHand;

            }

        }
        else {

            game.gameRound->resetHand(game.gameRound->getWinningHand());
            gameState = GameState::Play_EndOfHand;

        }
        
    }

}