#include <ArduboyFX.h>  


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


void play_Init() { 

    game.resetFrameCount();
    gameState = GameState::Play_Shuffle;

    #ifdef DEBUG_RAND
    uint16_t r = random(8000);
    DEBUG_PRINT("Rand ");
    DEBUG_PRINTLN(r);
    randomSeed(r);
    #endif
    randomSeed(3750);

    #ifdef DEBUG_CRASH
        game.setCardCount(9);
    #endif

}

void populateRotateDetails(uint8_t card) {
    
    dealPhysics.x = 110 * 16;
    dealPhysics.y = 1 * 16;

    dealPhysics.x2 = pgm_read_byte(Constants::DealCardCoords + (card * 2)) * 16;
    dealPhysics.y2 = pgm_read_byte(Constants::DealCardCoords + (card * 2) + 1) * 16;
    dealPhysics.rotInc = 27 * 16;


    dealPhysics.xInc = (dealPhysics.x2 - dealPhysics.x) / dealPhysics.noOfIncs;
    dealPhysics.yInc = (dealPhysics.y2 - dealPhysics.y) / dealPhysics.noOfIncs;
    dealPhysics.x = (110 * 16) + dealPhysics.xInc;
    dealPhysics.y = -dealPhysics.yInc;
    dealPhysics.rotInc = (dealPhysics.rotInc / dealPhysics.noOfIncs);
    dealPhysics.rot = dealPhysics.rotStart;

}

uint8_t getCardIdx(uint8_t player, uint8_t offset, uint8_t cardsToBeDealt) {

    uint8_t dealer = game.gameRound->getDealer_Idx();
    return (cardsToBeDealt * ((player - dealer + 3) % 4)) + offset;

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

            gameRound.setDealer_Idx(gameRound.getDealer_Idx() + 1);

            if (gameRound.getDealer_Idx() == 0) {
                populateRotateDetails(0);
            }

        #ifdef DEBUG
            gameState = GameState::Play_Deal_Catchup;
            [[fallthrough]]
        #else 

            [[fallthrough]]

            case GameState::Play_Deal_00 ... GameState::Play_Deal_11:
            case GameState::Play_Deal_30 ... GameState::Play_Deal_41:
                {

                    #ifdef DEBUG
                        gameState = GameState::Play_Deal_Catchup;
                    #endif

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
                                game.players[1].sort();
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

                            if (player == Constants::HumanPlayer) continue;
                        
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
                                game.players[1].sort();
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

                            if (player == Constants::HumanPlayer) continue;
                        
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
            
        #endif

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

            for (uint8_t i = 0; i < 4; i++) {
                game.players[i].sort();
            }

            break;

        case GameState::Bid:
            {

                if (playerCurrentlyBidding == Constants::HumanPlayer) {

                    if (justPressed & LEFT_BUTTON && bidInput.getMode() == BidMode::Suit)                   bidInput.decMode();
                    else if (justPressed & LEFT_BUTTON && bidInput.getMode() > BidMode::Suit)               bidInput.setMode(BidMode::Suit);
                    else if (justPressed & RIGHT_BUTTON && bidInput.getMode() < BidMode::Bid)               bidInput.incMode();

                    else if (justPressed & UP_BUTTON) {
                    
                        if (bidInput.getMode() == BidMode::Level && bidInput.getLevel() < 4)                bidInput.incLevel();
                        if (bidInput.getMode() == BidMode::Suit  && bidInput.getSuit() < 4)                 bidInput.incSuit();
                        if (bidInput.getMode() == BidMode::Pass || bidInput.getMode() == BidMode::Misere)   bidInput.decMode();

                    }

                    else if (justPressed & DOWN_BUTTON) {

                        if (bidInput.getMode() == BidMode::Level && bidInput.getLevel() > 0)                bidInput.decLevel();
                        if (bidInput.getMode() == BidMode::Suit && bidInput.getSuit() > 0)                  bidInput.decSuit();
                        if (bidInput.getMode() == BidMode::Bid || bidInput.getMode() == BidMode::Pass)      bidInput.incMode();

                    }

                    else if (justPressed & A_BUTTON) {

                        if (bidInput.getMode() == BidMode::Bid || bidInput.getMode() == BidMode::Misere) {

                            Bid bid;
                            Bid &highestBid = game.gameRound->getHighestBid();
                            Bid &playersBid = game.gameRound->getBid(1);
                            
                            bid.setLevel(bidInput.getLevel() + 6);

                            if (bidInput.getMode() == BidMode::Misere) {

                                bid.setBidType(BidType::Misere);

                            }
                            else {
                                
                                if (bidInput.getSuit() == 4) {

                                    bid.setSuit(Suit::No_Trumps);
                                    bid.setBidType(BidType::No_Trumps);
                                }
                                else {

                                    bid.setSuit(static_cast<Suit>(bidInput.getSuit()));
                                    bid.setBidType(BidType::Suit);
                                }

                            }

                            if (bid.isHigherThan(highestBid)) {

                                highestBid.setBid(bid);
                                highestBid.setPlayerIdx(Constants::HumanPlayer);

                                playersBid.setPlayerIdx(1);
                                playersBid.setBid(bid);

                                winningBidIdx = 1;
                                continueBidding();

                            }
                            else {

                                gameState = GameState::Bid_Error;

                            }

                        }

                        else if (bidInput.getMode() == BidMode::Pass) {

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
                        previousBid.setBid(retBid);

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
                            game.gameRound->getHighestBid().setPlayerIdx(playerCurrentlyBidding);
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
            
            if (game.getFrameCount() % 4 > 0) return;

            switch (game.players[0].getCardCount() > 0) {

                case 1 ... 10:

                    for (uint8_t i = 0; i < 4; i++) {
                        game.players[i].getCard(game.players[i].getCardCount() - 1).reset();
                        game.players[i].setCardCount(game.players[i].getCardCount() - 1);
                    }

                    break;

                case 0:
                    dealPhysics.x = 0;
                    dealPhysics.x2 = 0;
                    dealPhysics.y = 0;
                    dealPhysics.y2 = 0;
                    gameState = GameState::Play_Shuffle;
                    break;
            
            }
            
            break;

        case GameState::Bid_Error:

            if (justPressed & A_BUTTON) {
                gameState = GameState::Bid;
            }

            break;

        case GameState::Handle_Kitty:

            if (game.gameRound->getHighestBid().getPlayerIdx() == Constants::HumanPlayer) {

                if (justPressed == LEFT_BUTTON && selectedCard > 0) {

                    selectedCard--;

                }

                if (justPressed == RIGHT_BUTTON && selectedCard < game.players[Constants::HumanPlayer].getCardCount() - 1) {

                    selectedCard++;

                }

                if (justPressed == UP_BUTTON && game.players[Constants::HumanPlayer].getSelectedCount() < 3) {

                    game.players[Constants::HumanPlayer].getCard(selectedCard).setSelected(true);
                
                }

                if (justPressed == DOWN_BUTTON && game.players[Constants::HumanPlayer].getCard(selectedCard).isSelected()) {

                    game.players[Constants::HumanPlayer].getCard(selectedCard).setSelected(false);
                
                }

                if (justPressed == A_BUTTON && game.players[Constants::HumanPlayer].getSelectedCount() == 3) {

                    game.gameRound->setRound(0);
                    game.gameRound->clearKitty();

                    uint8_t kittyIndex = 0;

                    for (uint8_t i = 0; i < game.players[Constants::HumanPlayer].getCardCount(); i++) {

                        if (game.players[Constants::HumanPlayer].getCard(i).isSelected()) {

                            game.gameRound->getKitty(kittyIndex)->setSuit(game.players[Constants::HumanPlayer].getCard(i).getSuit());
                            game.gameRound->getKitty(kittyIndex)->setRank(game.players[Constants::HumanPlayer].getCard(i).getRank());
                            game.gameRound->getKitty(kittyIndex)->setSelected(false);
                            game.players[Constants::HumanPlayer].getCard(i).reset();
                            kittyIndex++;

                        }

                    }

                    game.players[Constants::HumanPlayer].sort();
                    game.gameRound->setRound(0);
                    selectedCard = 0;
                    gameState++;
                
                }

            }
            else {

                game.gameRound->setRound(0);
                game.gameRound->clearKitty();
                game.players[game.gameRound->getWinningBid_Idx()].handleKitty();
                game.players[game.gameRound->getWinningBid_Idx()].sort();
                selectedCard = 0;
                gameState++;

            }

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


            // Select card in players hand ..

            if (selectedCard >= game.players[1].getCardCount()) selectedCard--;
            game.players[1].getCard(selectedCard).setSelected(true);
            gameState++;

            break;

        case GameState::Play_01:
            {

                if (gameRound.getCurrentPlayer() != Constants::HumanPlayer) {
//if (gameRound.getRound() == 1) {
// if (gameRound.getRound() == 1 || gameRound.getRound() == 10) {
//    DEBUG_BREAK   //
//}
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

                    #ifdef DEBUG_BASIC

                        if (card.getRank() == Rank::Joker) {
                            
                            DEBUG_PRINT(F("JOKER"));

                        }
                        else {

                            DEBUG_PRINT_CARD(card.getSuit(), card.getRank());

                        }

                        DEBUG_PRINTLN();

                    #endif

                    play_CardSelected();

                }
                else {

                    if (justPressed == LEFT_BUTTON && selectedCard > 0) {
                        selectedCard--;
                        game.players[gameRound.getCurrentPlayer()].clearSelection();
                        game.players[gameRound.getCurrentPlayer()].getCard(selectedCard).setSelected(true);

                    }

                    if (justPressed == RIGHT_BUTTON && selectedCard < game.players[gameRound.getCurrentPlayer()].getCardCount() - 1) {
                        selectedCard++;
                        game.players[gameRound.getCurrentPlayer()].clearSelection();
                        game.players[gameRound.getCurrentPlayer()].getCard(selectedCard).setSelected(true);

                    }

                    if (justPressed == A_BUTTON && game.players[gameRound.getCurrentPlayer()].getCard(selectedCard).isSelected()) {

                        Suit trumps = game.gameRound->winningBid_Suit();
                        Card cardLed = game.gameRound->getCardLed();
                        Bid bid = game.gameRound->getWinningBid();

                        Card &cardPlayed = game.players[gameRound.getCurrentPlayer()].getCard(selectedCard);

                        switch (bid.getBidType()) {
                        
                            case BidType::Suit:

                                if (cardLed.getSuit(trumps) != cardPlayed.getSuit(trumps) && game.players[gameRound.getCurrentPlayer()].hasSuit(cardLed.getSuit(trumps))) {
                                    return;
                                }

                                break;
                        
                            case BidType::No_Trumps:

                                if (cardLed.getSuit() != cardPlayed.getSuit() && game.players[gameRound.getCurrentPlayer()].hasSuit(cardLed.getSuit())) {
                                    return;
                                }

                                if (cardPlayed.getRank() == Rank::Joker && game.gameRound->getFirstPlayer() == Constants::HumanPlayer) {

                                    jokerIndex = 0;
                                    gameState = GameState::Play_02;
                                    return;
                                }

                                break;

                            case BidType::Misere:

                                if (cardLed.getSuit() != cardPlayed.getSuit() && game.players[gameRound.getCurrentPlayer()].hasSuit(cardLed.getSuit())) {
                                    return;
                                }

                                break;

                        }

                        game.players[gameRound.getCurrentPlayer()].playCard(selectedCard);
                        play_CardSelected();

                    }

                }

            }

            break;

        case GameState::Play_02:

            if (justPressed == LEFT_BUTTON && jokerIndex > 0) {

                jokerIndex--;

            }

            if (justPressed == RIGHT_BUTTON && jokerIndex < 4) {

                jokerIndex++;

            }

            if (justPressed == A_BUTTON) {

                gameState = GameState::Play_01;
                game.players[Constants::HumanPlayer].playCard(selectedCard);
                game.gameRound->setJokerSuit(static_cast<Suit>(jokerIndex));
                play_CardSelected();
                game.gameRound->getHand(Constants::HumanPlayer)->setSuit(static_cast<Suit>(jokerIndex));

            }

            break;

        case GameState::Play_EndOfHand:

            if (gameRound.getRound() == 10) {

                uint8_t biddingTeam = game.gameRound->getWinningBid_Team();
                uint8_t winningTeam = game.gameRound->getWinningTeam();
                int16_t winningScore_BidTeam = game.gameRound->getWinningScore_BidTeam();
                uint8_t winningScore_Alt = game.gameRound->getWinningScore_AltTeam();

                #ifdef DEBUG_BASIC
                    DEBUG_PRINT(F("Winning Bid team "));
                    DEBUG_PRINTLN(biddingTeam);
                    DEBUG_PRINT(F("Winning team "));
                    DEBUG_PRINTLN(winningTeam);
                    DEBUG_PRINT(F("Winning score (Bid) "));
                    DEBUG_PRINTLN(winningScore_BidTeam);
                    DEBUG_PRINT(F("Winning score (Alt) "));
                    DEBUG_PRINTLN(winningScore_Alt);
                #endif

                game.gameRound->incScore(biddingTeam, winningScore_BidTeam);
                game.gameRound->incScore(biddingTeam == 1 ? 0 : 1, winningScore_Alt);

                gameState = GameState::Play_EndOfRound;

            }
            else {

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

            renderPlayerHands(currentPlane, false, false);
            renderKitty(currentPlane);
            renderDealer(currentPlane);
            renderHUD(currentPlane, true, false);

            if (dealPhysics.x > dealPhysics.x2 && dealPhysics.y < dealPhysics.y2) {
                SpritesU::drawPlusMaskFX((dealPhysics.x / 16)  - 5, (dealPhysics.y / 16) - 4, Images::Rotate, ((static_cast<uint8_t>((dealPhysics.rot / 16)) % 36) * 3) + currentPlane);
            }

            break;

        case GameState::Bid:

            renderPlayerHands(currentPlane, false, false);
            renderBids(currentPlane);
            renderHUD(currentPlane, false, false);

            if (playerCurrentlyBidding == Constants::HumanPlayer) {

                SpritesU::drawPlusMaskFX(27, 15, Images::Bid_Panel, (static_cast<uint8_t>(bidInput.getMode()) * 3) + currentPlane);
                SpritesU::drawOverwriteFX(31, 24, Images::Bid_Numbers, ((bidInput.getLevel() + (bidInput.getMode() != BidMode::Level ? 5 : 0)) * 3) + currentPlane);
                SpritesU::drawOverwriteFX(40, 23, Images::Bid_Suits, ((bidInput.getSuit() + (bidInput.getMode() != BidMode::Suit ? 5 : 0)) * 3) + currentPlane);
            
            }
            else {

                renderKitty(currentPlane);
                
            }

            break;

        case GameState::Bid_Finished:

            renderPlayerHands(currentPlane, false, false);
            renderKitty(currentPlane);
            renderBids(currentPlane);
            renderHUD(currentPlane, false, false);
            renderDealer(currentPlane);

            break;

        case GameState::Bid_Failed:

            SpritesU::drawOverwriteFX(34, 19, Images::EveryonePassed, currentPlane);
            renderPlayerHands(currentPlane, false, false);
            renderBids(currentPlane);
            renderHUD(currentPlane, false, false);

            break;

        case GameState::Bid_Error:

            renderPlayerHands(currentPlane, false, false);
            renderBids(currentPlane);
            renderHUD(currentPlane, false, false);

            SpritesU::drawPlusMaskFX(27, 15, Images::Bid_Panel, (5 * 3) + currentPlane);
            SpritesU::drawOverwriteFX(33, 29, Images::Bid_Result, (game.gameRound->getHighestBid().getImageIndex() * 3) + currentPlane);

            break;

        case GameState::Handle_Kitty:

            if (game.gameRound->getHighestBid().getPlayerIdx() == Constants::HumanPlayer) {
                SpritesU::drawOverwriteFX(21, 15, Images::KittyInstructions, currentPlane);
                renderPlayerHands(currentPlane, true, false);
            }
            else {
                renderPlayerHands(currentPlane, false, false);
            }

            renderDealer(currentPlane);
            renderHUD(currentPlane, false, true);
            renderDealer(currentPlane);

            break;

        case GameState::Play_Round_Start ... GameState::Play_01:

            renderPlayerHands(currentPlane, false, false);
            renderTableCards(currentPlane, Constants::NoWinner);
            renderHUD(currentPlane, false, true);

            break;

        case GameState::Play_02:

            renderPlayerHands(currentPlane, false, false);
            renderTableCards(currentPlane, Constants::NoWinner);
            renderHUD(currentPlane, false, true);
            SpritesU::drawOverwriteFX(21, 15, Images::ChooseSuit, (jokerIndex * 3) + currentPlane);

            break;


        case GameState::Play_EndOfHand ... GameState::Play_EndOfGame:

            renderPlayerHands(currentPlane, false, false);
            renderTableCards(currentPlane, game.gameRound->getWinningHand());
            renderHUD(currentPlane, false, true);

            break;

    }

}
