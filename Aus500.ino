#define ABG_IMPLEMENTATION
#define ABG_TIMER1
#define ABG_SYNC_PARK_ROW
#define SPRITESU_IMPLEMENTATION

#include <ArduboyFX.h>  
#include <FixedPointsCommon.h>
#include "src/utils/ArduboyG.h"
#include "src/utils/Enums.h"
#include "src/utils/Constants.h"
#include "src/utils/Utils.h"
#include "src/entities/Card.h"
#include "src/entities/Deck.h"
#include "src/entities/Bid.h"
#include "src/entities/GameRound.h"
#include "src/entities/Player.h"
#include "src/entities/Game.h"
#include "fxdata/fxdata.h"
#include "fxdata/images/Images.h"
// #include "src/entities/Entities.h"
#include "src/utils/Random.h"

#define SPRITESU_OVERWRITE
#define SPRITESU_PLUSMASK
#define SPRITESU_RECT
#define SPRITESU_FX

#include "src/utils/SpritesU.hpp"

extern ArduboyGBase_Config<ABG_Mode::L4_Triplane> a;
decltype(a) a;

#ifndef DEBUG_SOUND
    #define SYNTHU_IMPLEMENTATION
    #define SYNTHU_NUM_CHANNELS 6
    #define SYNTHU_UPDATE_EVERY_N_FRAMES 3
    #define SYNTHU_ENABLE_SFX 1
    #define SYNTHU_FX_READDATABYTES_FUNC FX::readDataBytes
    #include "src/utils/SynthU.hpp"
    #define ABG_TIMER1
#endif

#include <stdio.h>
#include <stdlib.h>
#include "time.h"

GameRound gameRound;
Game game;

uint8_t winningBidIdx;
uint8_t playerCurrentlyBidding;

// SoundSettings &soundSettings = cookie.soundSettings;
#if not defined(DEBUG) && not defined(DEBUG_BASIC)
GameState gameState = GameState::SplashScreen_Start;
#else
GameState gameState = GameState::Title_Init;
#endif

uint8_t titleCounter = 0;
int16_t instructions_Y = 0;

// Font3x5 font3x5 = Font3x5();
DealPhysics dealPhysics;

#ifdef DEBUG_FIXED_RAND
uint16_t seed = 72;
#endif

BidMode bidMode = BidMode::Level;
uint8_t bidSuit = 0;
uint8_t bidLevel = 0; 

void setup() {

    a.boot();
    abg_detail::send_cmds_prog<0xDB, 0x20>();
    a.startGray();
    
    FX::begin(FX_DATA_PAGE, FX_SAVE_PAGE);
    // FX::loadGameState((uint8_t*)&cookie, sizeof(cookie));
    #ifndef DEBUG_SOUND
    audioInit();
    setAudioOn();
    #endif


    for (uint8_t i = 0; i < 4; i++) {
        game.players[i].setPlayerNumber(i);
        game.players[i].gameRound = &gameRound;
    }

    game.gameRound = &gameRound;

    game.setFrameCount(0);

}

void loop() {

    FX::enableOLED();
    a.waitForNextPlane(BLACK);
    FX::disableOLED();

    switch (gameState) {

        #if not defined(DEBUG) && not defined(DEBUG_BASIC)
            case GameState::SplashScreen_Start ... GameState::SplashScreen_End:
                splashScreen(a);
                break;
        #endif

        case GameState::Title_Init:
            title_Init();
            [[fallthrough]];

        case GameState::Title_Start ... GameState::Title_End:
            title(a);
            break;

        case GameState::Play_Init:
            play_Init();
            [[fallthrough]];

        case GameState::Play_Start ... GameState::Play_End:
            play(a);
            break;


    }

    #ifndef DEBUG_SOUND
    audioUpdate();
    #endif

}


void dealCard(uint8_t player) {

    game.players[player].addCard(game.deck.getCard());

}

void dealKitty() {

  game.gameRound->addKitty(game.deck.getCard());

}


void DEBUG_PRINT_HAND(uint8_t player) {

    #ifdef DEBUG_BASIC
        
        switch (player) {

            case 0 ... 3:

                DEBUG_PRINT(F("P"));
                DEBUG_PRINT(player);
                DEBUG_PRINT_SPACE();

                for (uint8_t i = 0; i < game.players[player].getCardCount(); i++) {
                    if (game.players[player].getCard(i).getRank() != Rank::Ten) DEBUG_PRINT_SPACE();
                    DEBUG_PRINT_CARD(game.players[player].getCard(i).getSuit(), game.players[player].getCard(i).getRank());
                    DEBUG_PRINT_SPACE();
                }

                DEBUG_PRINTLN();
                break;

            case 4:

                DEBUG_PRINT(F("K  "));

                for (uint8_t i = 0; i < game.gameRound->getKittyPointer(); i++) {
                    if (game.gameRound->getKitty(i)->getRank() != Rank::Ten) DEBUG_PRINT_SPACE();
                    DEBUG_PRINT_CARD(game.gameRound->getKitty(i)->getSuit(), game.gameRound->getKitty(i)->getRank());
                    DEBUG_PRINT_SPACE();
                }

                DEBUG_PRINTLN();
                break;

        }

    #endif

}

void DEBUG_PRINT_HANDS() {

    #ifdef DEBUG_BASIC 

        DEBUG_PRINTLN(F("      1   2   3   4   5   6   7   8   9  10"));

        for (uint8_t i = gameRound.getCurrentPlayer(); i < gameRound.getCurrentPlayer() + 4; i++) {
            
            uint8_t playerIdx = i % 4;

            if (game.players[playerIdx].isPlaying()) {

                if (gameRound.getWinningBid_Idx() == playerIdx) {
                    DEBUG_PRINT(F("*"));
                }
                else {
                    DEBUG_PRINT_SPACE();
                }

                DEBUG_PRINT_HAND(playerIdx);
            }

        }

        DEBUG_PRINT_SPACE();
        DEBUG_PRINT_HAND(4);
        DEBUG_PRINTLN();     

    #endif

}