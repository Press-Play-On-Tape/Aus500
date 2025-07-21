#include <ArduboyFX.h>  


uint8_t bid = 0;


void play_Init() { 

    game.resetFrameCount();
    // bid = 0;

    gameState = GameState::Play_Shuffle;

    // game.fillCardsPile();
    // game.shuffleCardsPile();
    // game.players[Constants::HumanPlayer].sort();


//    game.setFirstPlayer((game.getDealer() + 1) % 4);
//    game.resetEOR();

    // for (uint8_t i = 0; i < Constants::PlayerCount; i++) {

    //     game.players[i].resetEOR();
    //     game.getTableCard(i).setSuit(Suits::None);

    // }


    #ifdef DEBUG_CRASH
        game.setCardCount(9);
    #endif

}

void populateRotateDetails(uint8_t card) {
    
    dealPhysics.x = 110;
    dealPhysics.y = 1;

    dealPhysics.x2 = pgm_read_byte(Constants::DealCardCoords + (card * 5));
    dealPhysics.y2 = pgm_read_byte(Constants::DealCardCoords + (card * 5) + 1);
    dealPhysics.noOfIncs = pgm_read_byte(Constants::DealCardCoords + (card * 5) + 2);
    dealPhysics.rotStart = pgm_read_byte(Constants::DealCardCoords + (card * 5) + 3);
    dealPhysics.rotInc = pgm_read_byte(Constants::DealCardCoords +( card * 5) + 4);

    dealPhysics.xInc = (dealPhysics.x2 - dealPhysics.x) / dealPhysics.noOfIncs;
    dealPhysics.yInc = (dealPhysics.y2 - dealPhysics.y) / dealPhysics.noOfIncs;
    dealPhysics.x = 110 + dealPhysics.xInc;
    dealPhysics.y = -dealPhysics.yInc;
    dealPhysics.rotInc = (dealPhysics.rotInc / dealPhysics.noOfIncs);
    dealPhysics.rot = dealPhysics.rotStart;

}

uint8_t getCardIdx(uint8_t player, uint8_t offset, uint8_t cardsToBeDealt) {

    uint8_t dealer = game.gameRound->getDealer_Idx();
    return (cardsToBeDealt * ((player - dealer + 4) % 4)) + offset;

}

// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void play_Update() { 

    uint8_t justPressed = getJustPressedButtons();

    game.incFrameCount();

    switch (gameState) {

        case GameState::Play_Shuffle:  

            game.deck.createDeck();
            game.deck.shuffleDeck();
            game.setFrameCount(0);
            gameState = GameState::Play_Deal_00;

            game.gameRound->resetRound();
            game.gameRound->setRound(0);

            for (uint8_t player = 0; player < 4; player++) {
                game.players[player].reset();
            }

            gameRound.setDealer_Idx(gameRound.getDealer_Idx() + 3);

            if (gameRound.getDealer_Idx() == 0) {
                populateRotateDetails(0);
            }

            [[fallthrough]]
    
        case GameState::Play_Deal_00 ... GameState::Play_Deal_11:
        case GameState::Play_Deal_30 ... GameState::Play_Deal_41:
            {

                if (justPressed & A_BUTTON) {
                    gameState = GameState::Play_Deal_Catchup;
                    return;             
                }

                uint8_t dealer = game.gameRound->getDealer_Idx();
                uint8_t card = static_cast<uint8_t>(gameState) - static_cast<uint8_t>(GameState::Play_Deal_00);
                uint8_t round = (gameState <= GameState::Play_Deal_11 ? 0 : 30);

                if (card >= getCardIdx(1, round, 3) && card <= getCardIdx(1, round, 3) + 2) {

                    uint8_t player = 1;

                    if (dealPhysics.x > dealPhysics.x2 && dealPhysics.y < dealPhysics.y2) {

                        dealPhysics.x = dealPhysics.x + dealPhysics.xInc;
                        dealPhysics.y = dealPhysics.y + dealPhysics.yInc;
                        dealPhysics.rot = dealPhysics.rot + dealPhysics.rotInc;

                        if (dealPhysics.x <= dealPhysics.x2 || dealPhysics.y >= dealPhysics.y2) {

                            dealCard(player);
                            gameState++;
                            game.setFrameCount(0);

                            if (card < getCardIdx(1, round, 3) + 2) {
                                populateRotateDetails(game.players[1].getCardCount());
                            }

                        }

                    }

                }
                else {

                    for (uint8_t player = 0; player < 4; player++) {

                        if (player == 1) continue;
                    
                        if (card >= getCardIdx(player, round, 3) && card <= getCardIdx(player, round, 3) + 2) {

                            if (game.getFrameCount() == 6) {

                                dealCard(player);

                                gameState++;
                                game.setFrameCount(0);

                                if (card + 1 == getCardIdx(1, round, 3)) {
                                    populateRotateDetails(game.players[1].getCardCount());
                                }

                            }

                        }

                    }

                }

            }

            break;

        case GameState::Play_Deal_12:
        case GameState::Play_Deal_29:

            if (justPressed & A_BUTTON) {
                gameState = GameState::Play_Deal_Catchup;
                return;             
            }

            gameState++;
            dealKitty();
            if (gameRound.getDealer_Idx() == 0) {
                populateRotateDetails(game.players[1].getCardCount());
            }
            break;
    
        case GameState::Play_Deal_13 ... GameState::Play_Deal_28:
            {
                uint8_t dealer = game.gameRound->getDealer_Idx();
                uint8_t card = static_cast<uint8_t>(gameState) - static_cast<uint8_t>(GameState::Play_Deal_00);

                if (justPressed & A_BUTTON) {
                    gameState = GameState::Play_Deal_Catchup;
                    return;             
                }

                if (card >= getCardIdx(1, 13, 4) && card <= getCardIdx(1, 13, 4) + 3) {

                    uint8_t player = 1;

                    if (dealPhysics.x > dealPhysics.x2 && dealPhysics.y < dealPhysics.y2) {

                        dealPhysics.x = dealPhysics.x + dealPhysics.xInc;
                        dealPhysics.y = dealPhysics.y + dealPhysics.yInc;
                        dealPhysics.rot = dealPhysics.rot + dealPhysics.rotInc;

                        if (dealPhysics.x <= dealPhysics.x2 || dealPhysics.y >= dealPhysics.y2) {

                            dealCard(player);
                            gameState++;
                            game.setFrameCount(0);

                            if (card < getCardIdx(1, 13, 4) + 3) {
                                populateRotateDetails(game.players[1].getCardCount());
                            }

                        }

                    }

                }
                else {

                    for (uint8_t player = 0; player < 4; player++) {

                        if (player == 1) continue;
                    
                        if (card >= getCardIdx(player, 13, 4) && card <= getCardIdx(player, 13, 4) + 3) {

                            if (game.getFrameCount() == 6) {

                                dealCard(player);

                                gameState++;
                                game.setFrameCount(0);

                                if (card + 1 == getCardIdx(1, 13, 4)) {
                                    populateRotateDetails(game.players[1].getCardCount());
                                }

                            }

                        }

                    }

                }

            }

            break;

        case GameState::Play_Deal_42:

            game.setFrameCount(0);
            gameState = GameState::Bid;
            dealKitty();
            break;

        case GameState::Play_Deal_Catchup:

            for (uint8_t player = 0; player < 4; player++) {

                while (game.players[player].getCardCount() < 10) {
                    dealCard(player);
                }

            }

            for (uint8_t kitty = 0; kitty < 3; kitty++) {

                if (game.gameRound->getKitty(kitty)->getRank() == Rank::None) {
                    dealKitty();
                }

            }

            game.setFrameCount(0);
            gameState = GameState::Bid;
        
            break;

        case GameState::Bid:
            {

                if (playerCurrentlyBidding == 1) {

                    if (justPressed & LEFT_BUTTON && bidMode == BidMode::Suit)          bidMode--;
                    else if (justPressed & LEFT_BUTTON && bidMode > BidMode::Suit)      bidMode = BidMode::Suit;
                    else if (justPressed & RIGHT_BUTTON && bidMode < BidMode::Bid)      bidMode++;

                    else if (justPressed & UP_BUTTON) {
                    
                        if (bidMode == BidMode::Level && bidLevel < 4)                  bidLevel++;
                        if (bidMode == BidMode::Suit  && bidSuit < 4)                   bidSuit++;
                        if (bidMode == BidMode::Pass || bidMode == BidMode::Misere)     bidMode--;

                    }

                    else if (justPressed & DOWN_BUTTON) {

                        if (bidMode == BidMode::Level && bidLevel > 0)                  bidLevel--;
                        if (bidMode == BidMode::Suit && bidSuit > 0)                    bidSuit--;
                        if (bidMode == BidMode::Bid || bidMode == BidMode::Pass)        bidMode++;

                    }

                    else if (justPressed & A_BUTTON) {

                        if (bidMode == BidMode::Bid || bidMode == BidMode::Misere) {

                            Bid bid;
                            Bid &highestBid = game.gameRound->getHighestBid();
                            Bid &playersBid = game.gameRound->getBid(1);
                            
                            bid.setLevel(bidLevel + 6);

                            if (bidMode == BidMode::Misere) {

                                bid.setBidType(BidType::Misere);

                            }
                            else {
                                
                                if (bidSuit == 4) {

                                    bid.setSuit(Suit::No_Trumps);
                                    bid.setBidType(BidType::No_Trumps);
                                }
                                else {

                                    bid.setSuit(static_cast<Suit>(bidSuit));
                                    bid.setBidType(BidType::Suit);
                                }

                            }

                            if (bid.isHigherThan(highestBid)) {

                                highestBid.setLevel(bid.getLevel());
                                highestBid.setSuit(bid.getSuit());
                                highestBid.setBidScore(bid.getBidScore());
                                highestBid.setBidType(bid.getBidType());

                                playersBid.setLevel(bid.getLevel());
                                playersBid.setSuit(bid.getSuit());
                                playersBid.setBidScore(bid.getBidScore());
                                playersBid.setBidType(bid.getBidType());

                                continueBidding();

                            }
                            else {

                                gameState = GameState::Bid_Error;

                            }

                        }

                        else if (bidMode == BidMode::Pass) {

                            game.gameRound->getBid(1).setBidType(BidType::Pass);
                            continueBidding();

                        }

                    }

                }
                else {
                    
                    if (game.getFrameCount() == 32) {

                        Bid &highestBid = game.gameRound->getHighestBid();
                        Bid &previousBid = game.gameRound->getBid(playerCurrentlyBidding);
                        Bid &partnerBid = game.gameRound->getBid((playerCurrentlyBidding + 2) % 4);

                        Bid retBid = game.players[playerCurrentlyBidding].bid(previousBid, partnerBid, highestBid);
                        previousBid.setBidScore(retBid.getBidScore());
                        previousBid.setBidType(retBid.getBidType());
                        previousBid.setLevel(retBid.getLevel());
                        previousBid.setSuit(retBid.getSuit());

                        #if defined(DEBUG) && defined(DEBUG_FORCE_NOBID)
                            game.gameRound->getBid(playerCurrentlyBidding).setBidType(BidType::Pass);
                        #endif

                        if (game.gameRound->getBid(playerCurrentlyBidding).getBidType() != BidType::Pass && game.gameRound->getBid(playerCurrentlyBidding).isHigherThan(game.gameRound->getHighestBid())) {
                            
                            #ifdef DEBUG
                                DEBUG_PRINT(F("Player "));
                                DEBUG_PRINT(playerCurrentlyBidding);
                                DEBUG_PRINT(F(" bids "));
                                DEBUG_PRINT_BID(previousBid.getBidType(), previousBid.getSuit(), previousBid.getLevel());
                                DEBUG_PRINTLN();
                            #endif

                            game.gameRound->setHighestBid(game.gameRound->getBid(playerCurrentlyBidding));
                            winningBidIdx = playerCurrentlyBidding;
                        }
                        else {

                            #ifdef DEBUG
                                DEBUG_PRINT(F("Player "));
                                DEBUG_PRINT(playerCurrentlyBidding);
                                DEBUG_PRINTLN(F(" passes"));
                            #endif

                        }

                        continueBidding();

                    }

                }

            }

            break;

        case GameState::Bid_Finished:
            {

                for (uint8_t i = 0; i < 4; i++) {
                    game.players[i].sort();
                }

                
                game.gameRound->setWinningBid_Idx(winningBidIdx);
                game.gameRound->setFirstPlayer(winningBidIdx);
                game.gameRound->setCurrentPlayer(winningBidIdx);

                game.players[winningBidIdx].addCard(game.gameRound->getKitty(0));
                game.players[winningBidIdx].addCard(game.gameRound->getKitty(1));
                game.players[winningBidIdx].addCard(game.gameRound->getKitty(2));

                #ifdef OPEN_MISERE
                if (game.gameRound->getWinningBid().getBidType() == BidType::Misere || game.gameRound->getWinningBid().getBidType() == BidType::Open_Misere) {
                #else
                if (game.gameRound->getWinningBid().getBidType() == BidType::Misere) {
                #endif
                    game.players[(winningBidIdx + 2) % 4].setPlaying(false);
                }

                gameState = GameState::Handle_Kitty;

            }

            break;

        case GameState::Bid_Failed:
            gameState = GameState::Play_Shuffle;
            break;

        case GameState::Bid_Error:

            if (justPressed & A_BUTTON) {
                gameState = GameState::Bid;
            }

            break;

        case GameState::Handle_Kitty:
            game.gameRound->setRound(0);
            game.gameRound->clearKitty();
            game.players[game.gameRound->getWinningBid_Idx()].handleKitty();

            //#include "GameOfArds_DEBUG.hpp"

            for (uint8_t i = 0; i < 4; i++) {
                game.players[i].sort();
            }

            gameState++;
            break;

        case GameState::Play_Round_Start:
            {
            
                game.gameRound->incRound();




                Bid bid = game.gameRound->getWinningBid();

                #ifdef DEBUG_BASIC
                    DEBUG_PRINTLN(F("\n-- Start of hand -----------------"));     
                    DEBUG_PRINT(F("Bid: "));
                    DEBUG_PRINT_BID(bid.getBidType(), bid.getSuit(), bid.getLevel());
                    DEBUG_PRINTLN();

                    DEBUG_PRINT(F("Bid Winner: "));
                    DEBUG_PRINTLN(gameRound.getWinningBid_Idx());
                    DEBUG_PRINT(F("Starting Hand: "));
                    DEBUG_PRINTLN(gameRound.getCurrentPlayer());
                    DEBUG_PRINT(F("Round: "));
                    DEBUG_PRINTLN(gameRound.getRound());
                    DEBUG_PRINT_HANDS();
                #endif
    
            }

            gameState++;

            break;

        case GameState::Play_00:

            gameState++;

            break;

        case GameState::Play_01:
            {

// if (gameRound.getRound() >= 10) {
// if (gameRound.getRound() == 1 || gameRound.getRound() == 10) {
    DEBUG_BREAK   
// }
                switch (gameRound.winningBid_Type()) {
                
                    case BidType::Suit:

                        if (gameRound.getFirstPlayer() == gameRound.getCurrentPlayer()) {
                            game.players[gameRound.getCurrentPlayer()].playSuit_Lead();
                        }
                        else {
                            game.players[gameRound.getCurrentPlayer()].playSuit_Follow();
                        }

                        break;
                
                    case BidType::No_Trumps:

                        if (gameRound.getFirstPlayer() == gameRound.getCurrentPlayer()) {
                            game.players[gameRound.getCurrentPlayer()].playNoTrumps_Lead();
                        }
                        else {
                            game.players[gameRound.getCurrentPlayer()].playNoTrumps_Follow();
                        }

                        break;

                    case BidType::Misere:

                        if (gameRound.getFirstPlayer() == gameRound.getCurrentPlayer()) {
                            game.players[gameRound.getCurrentPlayer()].playMisere_Lead();
                        }
                        else {
                            game.players[gameRound.getCurrentPlayer()].playMisere_Follow();
                        }

                        break;

                }

                #ifdef DEBUG_BASIC
                    DEBUG_PRINT(F("99. Player "));
                    DEBUG_PRINT(gameRound.getCurrentPlayer());
                    DEBUG_PRINT(F(" played "));
                #endif

                Card &card = game.players[gameRound.getCurrentPlayer()].cardJustPlayed;

                if (card.getRank() == Rank::Joker) {

                    #ifdef DEBUG_BASIC
                        DEBUG_PRINT(F("JOKER"));
                    #endif

                }
                else {

                    #ifdef DEBUG_BASIC
                        DEBUG_PRINT_CARD(card.getSuit(), card.getRank());
                    #endif
                }

                Serial.print("\n");

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

                    Serial.println("---------------------------------");
                    Serial.print("Trick winner ");
                    Serial.println(game.gameRound->getWinningHand());


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


Serial.print("Highest hand ");
Serial.print(game.gameRound->getWinningHand(BidType::No_Trumps));
Serial.print(", lowest hand ");
Serial.print(game.gameRound->getWinningHand());
Serial.print(", WinningBidIdx ");
Serial.println(game.gameRound->getWinningBid_Idx());
Serial.println("-------");

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

            break;

        case GameState::Play_02:

            break;

        case GameState::Play_EndOfHand:

            if (gameRound.getRound() == 10) {

                uint8_t biddingTeam = game.gameRound->getWinningBid_Team();
                uint8_t winningTeam = game.gameRound->getWinningTeam();
                int16_t winningScore_BidTeam = game.gameRound->getWinningScore_BidTeam();
                uint8_t winningScore_Alt = game.gameRound->getWinningScore_AltTeam();

                Serial.print("Winning Bid team ");
                Serial.println(biddingTeam);
                Serial.print("Winning team ");
                Serial.println(winningTeam);
                Serial.print("Winning score (Bid) ");
                Serial.println(winningScore_BidTeam);
                Serial.print("Winning score (Alt) ");
                Serial.println(winningScore_Alt);

                game.gameRound->incScore(biddingTeam, winningScore_BidTeam);
                game.gameRound->incScore(biddingTeam == 1 ? 0 : 1, winningScore_Alt);

                gameState = GameState::Play_EndOfRound;

            }
            else {

                // if (gameRound.getRound() != 10 || (justPressed & A_BUTTON)) {
                if (justPressed & A_BUTTON) {
                
                    game.gameRound->resetHand();
                    gameState = GameState::Play_Round_Start;

                }

            }

            break;          

        case GameState::Play_EndOfRound:

            if ((gameRound.getScore(0) <= -500 || gameRound.getScore(0) >= 500) ||
                (gameRound.getScore(1) <= -500 || gameRound.getScore(1) >= 500)) {

                gameState = GameState::Play_EndOfGame;

            }
            else {

                if (justPressed & A_BUTTON) {

                    gameState = GameState::Play_Shuffle;

                }

            }

            break;            

        case GameState::Play_EndOfGame:

            if (justPressed & A_BUTTON) {

                gameState = GameState::Title_Init;

            }

            break;       
    }

}

void play(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {

    uint8_t currentPlane = a.currentPlane();
    if (a.needsUpdate()) play_Update();

    switch (gameState) {

        case GameState::Play_Deal_00 ... GameState::Play_Deal_42:

            SpritesU::drawOverwriteFX(105, 0, Images::HUD, currentPlane);
            renderPlayerHands(currentPlane, LIGHT_GREY);
            renderKitty(currentPlane);
            SpritesU::drawOverwriteFX(109, 1, Images::Hand_Full, currentPlane);

            if (dealPhysics.x > dealPhysics.x2 && dealPhysics.y < dealPhysics.y2) {
                SpritesU::drawPlusMaskFX(dealPhysics.x - 5, dealPhysics.y - 4, Images::Rotate, ((static_cast<uint8_t>(dealPhysics.rot) % 36) * 3) + currentPlane);
            }

            break;

        case GameState::Bid:

            SpritesU::drawOverwriteFX(105, 0, Images::HUD, currentPlane);
            renderPlayerHands(currentPlane, LIGHT_GREY);
            renderBids(currentPlane);

            if (playerCurrentlyBidding == 1) {

                SpritesU::drawPlusMaskFX(27, 15, Images::Bid_Panel, (static_cast<uint8_t>(bidMode) * 3) + currentPlane);
                SpritesU::drawOverwriteFX(31, 24, Images::Bid_Numbers, ((bidLevel + (bidMode != BidMode::Level ? 5 : 0)) * 3) + currentPlane);
                SpritesU::drawOverwriteFX(40, 23, Images::Bid_Suits, ((bidSuit + (bidMode != BidMode::Suit ? 5 : 0)) * 3) + currentPlane);
            }

            break;

        case GameState::Bid_Finished ... GameState::Bid_Failed:

            SpritesU::drawOverwriteFX(105, 0, Images::HUD, currentPlane);
            renderPlayerHands(currentPlane, LIGHT_GREY);
            renderKitty(currentPlane);
            renderBids(currentPlane);

            break;

        case GameState::Bid_Error:

            SpritesU::drawOverwriteFX(105, 0, Images::HUD, currentPlane);
            renderPlayerHands(currentPlane, LIGHT_GREY);
            renderBids(currentPlane);

            SpritesU::drawPlusMaskFX(27, 15, Images::Bid_Panel, (5 * 3) + currentPlane);
            SpritesU::drawOverwriteFX(33, 29, Images::Bid_Result, (game.gameRound->getHighestBid().getImageIndex() * 3) + currentPlane);

            break;

        case GameState::Handle_Kitty ... GameState::Play_EndOfGame:

            SpritesU::drawOverwriteFX(105, 0, Images::HUD, currentPlane);
            renderPlayerHands(currentPlane, LIGHT_GREY);
            renderTableCards(currentPlane, 2);
            // renderBids(currentPlane);

            break;




    }

}

void continueBidding() {

    uint8_t continueBiddingCount = 0;
    uint8_t bids = 0;
    uint8_t passes = 0;

    for (uint8_t i = 0; i < 4; i++) {

        if (game.gameRound->getBid(i).getBidType() != BidType::Pass) {
            continueBiddingCount++;
        }

        switch (game.gameRound->getBid(i).getBidType()) {

            case BidType::Suit:
            case BidType::No_Trumps:
            case BidType::Misere:
            #ifdef OPEN_MISERE
            case BidType::Open_Misere:
            #endif
                bids++;
                break;

            case BidType::Pass:     
                passes++;
                break;

        }

    }


    if (passes < 3 || (passes == 3 && bids == 0)) {

        do {    // Skip to the next non-passed player ..

            playerCurrentlyBidding = (playerCurrentlyBidding + 1) % 4;

        }
        while (game.gameRound->getBid(playerCurrentlyBidding).getBidType() == BidType::Pass);
        game.setFrameCount(0);

    }
    else if (passes == 3 && bids == 1) {

        gameState++;     

    }
    else {

        gameState = GameState::Bid_Failed;

    }
    
}