#include <ArduboyFX.h>  


void continueBidding() {

    uint8_t continueBiddingCount = 0;
    uint8_t bids = 0;
    uint8_t passes = 0;

    for (uint8_t i = 0; i < 4; i++) {

        if (gameRound.getBid(i).getBidType() != BidType::Pass) {
            continueBiddingCount++;
        }

        switch (gameRound.getBid(i).getBidType()) {

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
        while (gameRound.getBid(playerCurrentlyBidding).getBidType() == BidType::Pass);
        game.setFrameCount(0);

    }
    else if (passes == 3 && bids == 1) {

        gameState++;    
        game.setFrameCount(0);

    }
    else {

        gameState = GameState::Bid_Failed;

    }
    
}


void play_Init() { 

    game.resetFrameCount();
    gameState = GameState::Play_Shuffle;
    gameRound.setScore(0, 0);
    gameRound.setScore(1, 0);

    // #ifdef DEBUG_RAND
    uint16_t r = random(8000);
    // DEBUG_PRINT("Rand ");
    // DEBUG_PRINTLN(r);
    randomSeed(r);
    game.setRandomSeed(r);
    // #endif
    // randomSeed(2816);

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

    uint8_t dealer = gameRound.getDealer_Idx();
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

            gameRound.resetRound();
            gameRound.setRound(0);

            for (uint8_t player = 0; player < 4; player++) {
                game.players[player].reset();
            }

            gameRound.setDealer_Idx(gameRound.getDealer_Idx() + 1);
            playerCurrentlyBidding = (gameRound.getDealer_Idx() + 1) % 4;

            if (gameRound.getDealer_Idx() == 0) {
                populateRotateDetails(0);
            }

        #ifdef DEBUG_SKIP_DEAL
            gameState = GameState::Play_Deal_Catchup;
            [[fallthrough]]
        #else 

            [[fallthrough]]

            case GameState::Play_Deal_00 ... GameState::Play_Deal_11:
            case GameState::Play_Deal_30 ... GameState::Play_Deal_41:
                {

                    #ifdef DEBUG_SKIP_DEAL
                        gameState = GameState::Play_Deal_Catchup;
                    #endif

                    if (justPressed & A_BUTTON) {
                        gameState = GameState::Play_Deal_Catchup;
                        return;             
                    }

                    uint8_t dealer = gameRound.getDealer_Idx();
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
                    uint8_t dealer = gameRound.getDealer_Idx();
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

                if (gameRound.getKitty(kitty)->getRank() == Rank::None) {
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
                            Bid &highestBid = gameRound.getHighestBid();
                            Bid &playersBid = gameRound.getBid(1);
                            
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

                            gameRound.getBid(1).setBidType(BidType::Pass);
                            continueBidding();

                        }

                    }

                }
                else {
                    
                    if (game.getFrameCount() == 32) {

                        Bid &highestBid = gameRound.getHighestBid();
                        Bid &previousBid = gameRound.getBid(playerCurrentlyBidding);
                        Bid &partnerBid = gameRound.getBid((playerCurrentlyBidding + 2) % 4);

                        Bid retBid = game.players[playerCurrentlyBidding].bid(previousBid, partnerBid, highestBid);
                        previousBid.setBid(retBid);

                        #if defined(DEBUG) && defined(DEBUG_FORCE_NOBID)
                            gameRound.getBid(playerCurrentlyBidding).setBidType(BidType::Pass);
                        #endif

                        if (gameRound.getBid(playerCurrentlyBidding).getBidType() != BidType::Pass && gameRound.getBid(playerCurrentlyBidding).isHigherThan(gameRound.getHighestBid())) {
                            
                            #ifdef DEBUG
                                DEBUG_PRINT(F("Player "));
                                DEBUG_PRINT(playerCurrentlyBidding);
                                DEBUG_PRINT(F(" bids "));
                                DEBUG_PRINT_BID(previousBid.getBidType(), previousBid.getSuit(), previousBid.getLevel());
                                DEBUG_PRINTLN();
                            #endif

                            gameRound.setHighestBid(gameRound.getBid(playerCurrentlyBidding));
                            gameRound.getHighestBid().setPlayerIdx(playerCurrentlyBidding);
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
                if (game.getFrameCount() == 32) {

                    gameRound.setWinningBid_Idx(winningBidIdx);
                    gameRound.setFirstPlayer_Idx(winningBidIdx);
                    gameRound.setCurrentPlayer_Idx(winningBidIdx);

                    game.players[winningBidIdx].addCard(gameRound.getKitty(0));
                    game.players[winningBidIdx].addCard(gameRound.getKitty(1));
                    game.players[winningBidIdx].addCard(gameRound.getKitty(2));

                    game.players[winningBidIdx].getCard(10).setSelected(true);
                    game.players[winningBidIdx].getCard(11).setSelected(true);
                    game.players[winningBidIdx].getCard(12).setSelected(true);

                    #ifdef OPEN_MISERE
                    if (gameRound.getWinningBid().getBidType() == BidType::Misere || gameRound.getWinningBid().getBidType() == BidType::Open_Misere) {
                    #else
                    if (gameRound.getWinningBid().getBidType() == BidType::Misere) {
                    #endif
                        game.players[(winningBidIdx + 2) % 4].setPlaying(false);
                    }

                    for (uint8_t i = 0; i < 4; i++) {

                        for (uint8_t j = 0; j < game.players[i].getCardCount(); j++) {
                        
                            switch (gameRound.getWinningBid().getBidType()) {

                                case BidType::Misere:
                                case BidType::No_Trumps:
                                    game.players[i].getCard(j).setTrumps(Suit::None);
                                    break;

                                default:
                                    game.players[i].getCard(j).setTrumps(gameRound.getWinningBid().getSuit());
                                    break;
                            
                            }
                        }
                        game.players[i].sort();
                    }
                    
                    gameState = GameState::Handle_Kitty;
                    kittyHighlight = 64;

                }

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

            if (gameRound.getHighestBid().getPlayerIdx() == Constants::HumanPlayer) {

                if (justPressed > 0) {
                
                    if (kittyHighlight > 0) {
                        kittyHighlight = 0;
                        game.players[Constants::HumanPlayer].clearSelection();
                    }

                }

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

                    gameRound.setRound(0);
                    gameRound.clearKitty();

                    uint8_t kittyIndex = 0;

                    for (uint8_t i = 0; i < game.players[Constants::HumanPlayer].getCardCount(); i++) {

                        if (game.players[Constants::HumanPlayer].getCard(i).isSelected()) {

                            gameRound.getKitty(kittyIndex)->setSuit(game.players[Constants::HumanPlayer].getCard(i).getSuit());
                            gameRound.getKitty(kittyIndex)->setRank(game.players[Constants::HumanPlayer].getCard(i).getRank());
                            gameRound.getKitty(kittyIndex)->setOrigSuit(game.players[Constants::HumanPlayer].getCard(i).getOrigSuit());
                            gameRound.getKitty(kittyIndex)->setOrigRank(game.players[Constants::HumanPlayer].getCard(i).getOrigRank());
                            gameRound.getKitty(kittyIndex)->setSelected(false);
                            game.players[Constants::HumanPlayer].getCard(i).reset();
                            kittyIndex++;

                        }

                    }

                    game.players[Constants::HumanPlayer].sort();
                    gameRound.setRound(0);
                    highlightSuitInHand();
                    gameState++;
                
                }


                if (kittyHighlight > 0) {
                    
                    kittyHighlight--;
                    
                    if (kittyHighlight == 0) {
                        game.players[Constants::HumanPlayer].clearSelection();
                    }

                }


            }
            else {

                gameRound.setRound(0);
                gameRound.clearKitty();
                game.players[gameRound.getWinningBid_Idx()].handleKitty();
                game.players[gameRound.getWinningBid_Idx()].sort();
                highlightSuitInHand();
                gameState++;

            }

            break;

        case GameState::Play_Round_Start:
            {
            
                gameRound.incRound();

                Bid bid = gameRound.getWinningBid();

                #ifdef DEBUG_BASIC
                    DEBUG_PRINTLN(F("\n-- Start of hand -----------------"));     
                    DEBUG_PRINT(F("Bid: "));
                    DEBUG_PRINT_BID(bid.getBidType(), bid.getSuit(), bid.getLevel());
                    DEBUG_PRINTLN();

                    DEBUG_PRINT(F("Bid Winner: "));
                    DEBUG_PRINTLN(gameRound.getWinningBid_Idx());
                    DEBUG_PRINT(F("Starting Hand: "));
                    DEBUG_PRINTLN(gameRound.getCurrentPlayer_Idx());
                    DEBUG_PRINT(F("Round: "));
                    DEBUG_PRINTLN(gameRound.getRound());
                    DEBUG_PRINT_HANDS();
                #endif
    
            }


            // Select card in players hand ..

            if (selectedCard >= game.players[1].getCardCount()) selectedCard--;
            game.players[Constants::HumanPlayer].getCard(selectedCard).setSelected(true);
            game.setFrameCount(0);
            gameState++;

            break;

        case GameState::Play_01:
            {

                if (gameRound.getCurrentPlayer_Idx() != Constants::HumanPlayer) {

                    if (game.getFrameCount() == 32) {

                        switch (gameRound.winningBid_Type()) {
                        
                            case BidType::Suit:

                                if (gameRound.getFirstPlayer_Idx() == gameRound.getCurrentPlayer_Idx()) {
                                    game.players[gameRound.getCurrentPlayer_Idx()].playSuit_Lead();
                                }
                                else {
                                    game.players[gameRound.getCurrentPlayer_Idx()].playSuit_Follow();
                                }

                                break;
                        
                            case BidType::No_Trumps:

                                if (gameRound.getFirstPlayer_Idx() == gameRound.getCurrentPlayer_Idx()) {
                                    game.players[gameRound.getCurrentPlayer_Idx()].playNoTrumps_Lead();
                                }
                                else {
                                    game.players[gameRound.getCurrentPlayer_Idx()].playNoTrumps_Follow();
                                }

                                break;

                            case BidType::Misere:

                                if (gameRound.getFirstPlayer_Idx() == gameRound.getCurrentPlayer_Idx()) {
                                    game.players[gameRound.getCurrentPlayer_Idx()].playMisere_Lead();
                                }
                                else {
                                    game.players[gameRound.getCurrentPlayer_Idx()].playMisere_Follow();
                                }

                                break;

                        }

                        #ifdef DEBUG_BASIC
                            DEBUG_PRINT(F("99. Player "));
                            DEBUG_PRINT(gameRound.getCurrentPlayer_Idx());
                            DEBUG_PRINT(F(" played "));
                        #endif

                        Card &card = game.players[gameRound.getCurrentPlayer_Idx()].cardJustPlayed;

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

                }
                else {

                    if (justPressed == LEFT_BUTTON && selectedCard > 0) {
                        selectedCard--;
                        game.players[Constants::HumanPlayer].clearSelection();
                        game.players[Constants::HumanPlayer].getCard(selectedCard).setSelected(true);

                    }

                    if (justPressed == RIGHT_BUTTON && selectedCard < game.players[gameRound.getCurrentPlayer_Idx()].getCardCount() - 1) {
                        selectedCard++;
                        game.players[Constants::HumanPlayer].clearSelection();
                        game.players[Constants::HumanPlayer].getCard(selectedCard).setSelected(true);

                    }

                    if (justPressed == A_BUTTON && game.players[gameRound.getCurrentPlayer_Idx()].getCard(selectedCard).isSelected()) {

                        Suit trumps = gameRound.winningBid_Suit();
                        Card cardLed = gameRound.getCardLed();
                        Bid bid = gameRound.getWinningBid();

                        Card &cardPlayed = game.players[gameRound.getCurrentPlayer_Idx()].getCard(selectedCard);

                        switch (bid.getBidType()) {
                        
                            case BidType::Suit:

                                if (cardLed.getSuit() != cardPlayed.getSuit() && game.players[gameRound.getCurrentPlayer_Idx()].hasSuit(cardLed.getSuit())) {

                                    return;

                                }

                                break;
                        
                            case BidType::No_Trumps:

                                if (cardLed.getSuit() != Suit::None && cardLed.getSuit() != cardPlayed.getSuit() && game.players[gameRound.getCurrentPlayer_Idx()].hasSuit(cardLed.getSuit())) {
                                    
                                    return;

                                }

                                if (cardPlayed.getRank() == Rank::Joker && gameRound.getFirstPlayer_Idx() == Constants::HumanPlayer) {

                                    jokerIndex = 0;
                                    gameState = GameState::Play_02;
                                    return;

                                }
                                else if (cardPlayed.getRank() == Rank::Joker) {
                                
                                    cardPlayed.setSuit(cardLed.getSuit());

                                }

                                break;

                            case BidType::Misere:

                                if (cardLed.getSuit() != cardPlayed.getSuit() && game.players[gameRound.getCurrentPlayer_Idx()].hasSuit(cardLed.getSuit())) {
                                    
                                    return;

                                }

                                break;

                        }

                        game.players[gameRound.getCurrentPlayer_Idx()].playCard(selectedCard);
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

                game.players[Constants::HumanPlayer].getCard(selectedCard).setSuit(static_cast<Suit>(jokerIndex));
                gameState = GameState::Play_01;
                game.players[Constants::HumanPlayer].playCard(selectedCard);
                play_CardSelected();

            }

            break;

        case GameState::Play_EndOfHand:

            if (gameRound.getRound() == 10) {

                uint8_t biddingTeam = gameRound.getWinningBid_Team();
                uint8_t winningTeam = gameRound.getWinningTeam();
                int16_t winningScore_BidTeam = gameRound.getWinningScore_BidTeam(gameRound.getTeam_TrickCount(biddingTeam));
                uint8_t winningScore_Alt = gameRound.getWinningScore_AltTeam();

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

                gameRound.incScore(biddingTeam, winningScore_BidTeam);
                gameRound.incScore(biddingTeam == 1 ? 0 : 1, winningScore_Alt);

                gameState = GameState::Play_EndOfRound;

            }
            else {

                if (justPressed & A_BUTTON) {
                
                    gameRound.resetHand();
                    gameState = GameState::Play_Round_Start;

                }

            }

            break;          

        case GameState::Play_EndOfRound:

            // if ((gameRound.getScore(0) <= -500 || gameRound.getScore(0) >= 500) ||
            //     (gameRound.getScore(1) <= -500 || gameRound.getScore(1) >= 500)) {  SJH
            if ((gameRound.getScore(0) <= -50 || gameRound.getScore(0) >= 50) ||
                (gameRound.getScore(1) <= -50 || gameRound.getScore(1) >= 50)) {

                gameState = GameState::Play_EndOfGame;
                endOfGame_Y = 0;

            }
            else {

                if (justPressed & A_BUTTON) {

                    gameState = GameState::Play_Shuffle;

                }

            }

            break;            

        case GameState::Play_EndOfGame:

            if (game.getFrameCount() > 60 && game.getFrameCount() < 60 + 64) {

                endOfGame_Y++;

            }

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
            renderHUD(currentPlane, false, false);

            if (playerCurrentlyBidding == Constants::HumanPlayer) {

                SpritesU::drawPlusMaskFX(27, 15, Images::Bid_Panel, (static_cast<uint8_t>(bidInput.getMode()) * 3) + currentPlane);
                SpritesU::drawOverwriteFX(31, 24, Images::Bid_Numbers, ((bidInput.getLevel() + (bidInput.getMode() != BidMode::Level ? 5 : 0)) * 3) + currentPlane);
                SpritesU::drawOverwriteFX(40, 23, Images::Bid_Suits, ((bidInput.getSuit() + (bidInput.getMode() != BidMode::Suit ? 5 : 0)) * 3) + currentPlane);
            
            }
            else {

                renderKitty(currentPlane);
                
            }

            renderBids(currentPlane);

            break;

        case GameState::Bid_Finished:

            renderPlayerHands(currentPlane, false, false);
            renderKitty(currentPlane);
            renderBids(currentPlane);
            renderHUD(currentPlane, false, false);

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
            SpritesU::drawOverwriteFX(33, 29, Images::Bid_Result, (gameRound.getHighestBid().getImageIndex() * 3) + currentPlane);

            break;

        case GameState::Handle_Kitty:

            if (gameRound.getHighestBid().getPlayerIdx() == Constants::HumanPlayer) {
                SpritesU::drawOverwriteFX(21, 15, Images::KittyInstructions, currentPlane);
                renderPlayerHands(currentPlane, true, false);
            }
            else {
                renderDealer(currentPlane);
                renderPlayerHands(currentPlane, false, false);
            }

            renderHUD(currentPlane, false, true);

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
            SpritesU::drawOverwriteFX(28, 15, Images::ChooseSuit, (jokerIndex * 3) + currentPlane);

            break;


        case GameState::Play_EndOfHand ... GameState::Play_EndOfRound:

            renderPlayerHands(currentPlane, false, false);
            renderTableCards(currentPlane, gameRound.getWinningHand());
            renderHUD(currentPlane, false, true);

            break;

        case GameState::Play_EndOfGame:

            if (endOfGame_Y > 0) {

                renderTableCards(currentPlane, Constants::NoWinner, endOfGame_Y);

                uint8_t idx = 0;
                if ((game.getFrameCount() % 140) < 64) idx = game.getFrameCount() % 140 / 2;

                if (gameRound.getScore(Constants::HumanTeam) >= 50) {//SJH
                    SpritesU::drawOverwriteFX(16, endOfGame_Y - 55, Images::You_Won, (idx * 3) + currentPlane);
                }
                else {
                    SpritesU::drawOverwriteFX(16, endOfGame_Y - 55, Images::You_Lost, (idx * 3) + currentPlane);
                }

            }
            else {
                renderTableCards(currentPlane, gameRound.getWinningHand(), endOfGame_Y);
            }

            renderHUD(currentPlane, false, true);


            break;

    }

}
