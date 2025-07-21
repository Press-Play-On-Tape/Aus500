#pragma once
#include "../../fxdata/fxdata.h"
#include "../../fxdata/images/Images.h"

#define DEBUG_PRINT    Serial.print
#define DEBUG_PRINTLN  Serial.println
#define DEBUG_BREAK    asm volatile("break\n");

#define DEBUG
#define DEBUG_BASIC
#define DEBUG_SOUND


// Bidding

#define _DEBUG_EVALUATE
#define _DEBUG_EVALUATE_SUIT
#define _DEBUG_EVALUATE_NOTRUMPS
#define _DEBUG_EVALUATE_MISERE
#ifdef OPEN_MISERE
#define _DEBUG_EVALUATE_OPEN_MISERE
#endif
#define _DEBUG_PREPARE_SUIT_PILES
#define _DEBUG_CALC_MISERE_SCORE
#define _DEBUG_ELEVPART_EVALUATE

//Kitty

#define DEBUG_HANDLEKITTY_TRUMPS
#define _DEBUG_HANDLEKITTY_MISERE

//Play

#define _DEBUG_PLAYER_WINNING_HAND
#define _DEBUG_PLAYNOTRUMPS_LEAD
#define _DEBUG_PLAYNOTRUMPS_FOLLOW
#define _DEBUG_PLAYSUIT_LEAD;
#define _DEBUG_PLAYSUIT_FOLLOW;
#define _DEBUG_PLAYMISERE_LEAD
#define _DEBUG_PLAYMISERE_FOLLOW
#define _DEBUG_MARKCARDPLAYED
#define _DEBUG_HASCARDBEENPLAYED
#define _DEBUG_GETTOP_NONSUIT
#define _DEBUG_SORT
#define _DEBUG_SORT_DETAIL
#define _DEBUG_PLAY_ADDCARD
#define _DEBUG_DISCARDALL_INSUIT
#define _DEBUG_DISCARDCARD
#define _DEBUG_GETBOTTOM_NONSUIT
#define _DEBUG_GETNEXTLOWEST_NONTRUMP_INSUIT
#define _DEBUG_NUMBER_OF_UNPLAYED_CARDS


#ifndef DEBUG_SOUND
namespace Music {

    constexpr uint24_t Songs[] = { Music::Song_MainTheme };

    constexpr uint24_t SFXs[] =  { Music::SFX_Death, 
                                   Music::SFX_Victory,
                                   Music::SFX_XPGain,
                                   Music::SFX_Three_Inc,
                                   Music::SFX_Three_Dec,
                                   Music::SFX_PlayerBlip,
                                   Music::SFX_EnemyBlip,
                                   Music::SFX_Explosion,
                                   Music::SFX_DoubleBeep,
                                   Music::SFX_DoubleBeepDown,
                                   Music::SFX_Bet1,
                                   Music::SFX_Bet2,
                                   Music::SFX_Bet3,
                                   Music::SFX_Bet4,
                                   Music::SFX_Bet5,
                                   Music::SFX_Bet6,
                                   Music::SFX_Click,
                                   Music::SFX_SmallBlind,
                                   Music::SFX_BigBlind,
                                };

}
#endif


namespace Constants {

    const uint16_t DealCardCoords[] PROGMEM = {
      // x,  y,  noOfIncs  StartingRot, numberofRots
        44, 54,  24,       12,          27,  // 01
        48, 48,  24,       12,          27,  // 02
        52, 48,  24,       12,          27,  // 03
        56, 48,  24,       12,          27,  // 04
        60, 48,  24,       12,          27,  // 05
        64, 48,  24,       12,          27,  // 06
        68, 48,  24,       12,          27,  // 07
        72, 48,  24,       12,          27,  // 08
        76, 48,  24,       12,          27,  // 09
        80, 48,  24,       12,          27,  // 10
    };

    const int8_t Bounce[] PROGMEM = { -36, -30, -24, -16, -12, -6, 0, 4, 8, 12, 16, 20, 24, 28, 32, 28, 25, 22, 21, 21, 22, 25, 28, 32, 29, 27, 26, 26, 27, 28, 32, 30, 29, 30, 32, 31, 32 };

    constexpr uint8_t NoPlayer = 255;
    constexpr uint8_t NoCard = 255;
    constexpr uint8_t NoBid = 255;
    constexpr uint8_t NoWinner = 255;
    constexpr uint8_t PlayerCount = 4;
    constexpr uint8_t CardsInDeck = 52;
    constexpr uint8_t HumanPlayer = 1;

    constexpr uint8_t Delay = 12;
    constexpr uint8_t Title_Start_Action = 60;
    

	static constexpr uint8_t Player_Count = 4;
	static constexpr uint8_t Suit_Count = 4;
	static constexpr uint8_t Team_Count = 2;

	static constexpr uint8_t Deal_Delay = 2;
	static constexpr uint8_t No_Card = 255;
	static constexpr uint8_t No_Player = 255;
	static constexpr uint8_t WinningBid_None = 255;

	static constexpr uint8_t Sort_Joker = 254;
	static constexpr uint8_t Sort_None = 255;

	//													   						        J   Q   K   A
	static constexpr uint8_t ScoreType_Basic[] = 	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
	static constexpr uint8_t ScoreType_Trumps[] = 	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 15, 21, 50 };

};

struct DealPhysics {

    float x = 0;
    float y = 0;

    float xInc = 0;
    float yInc = 0;
    float x2 = 0;
    float y2 = 0;
    float rot = 0;
    float rotStart = 0;
    float rotInc = 0;
    int8_t noOfIncs = 0;

};