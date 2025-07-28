#include <ArduboyFX.h>  


void renderPlayerHands(uint8_t currentPlane, bool blinkSelected, bool blinkRaised) {

    uint8_t cardCount = game.players[Constants::HumanPlayer].getCardCount();
    int8_t xStart = 48 - (cardCount * 4) + (cardCount > 10 ? 3 : 0);

    for (uint8_t i = 0; i < cardCount; i++) {

        Card &card = game.players[Constants::HumanPlayer].getCard(i);
        Rank rank = card.getRank();

        if (rank != Rank::None) {

            // uint8_t y = 47 - (card.isSelected() ? 4 : 0);
            uint8_t y = 47;


            // Raise cards only when the player is throwing cards from the kitty or its his turn ..

            switch (gameState){
            
                case GameState::Play_01:

                    if (gameRound.getCurrentPlayer() == Constants::HumanPlayer) {
                        
                        y = 47 - (card.isSelected() ? 4 : 0);

                    }

                    break;

                case GameState::Handle_Kitty:

                    if (game.gameRound->getHighestBid().getPlayerIdx() == Constants::HumanPlayer) {
                        
                        y = 47 - (card.isSelected() ? 4 : 0);

                    }

                    break;

            }

            if ((blinkRaised && card.isSelected() || (blinkSelected && i == selectedCard)) && game.getFrameCount(48)) {
            
                if (rank != Rank::Joker) {
                    SpritesU::drawPlusMaskFX(xStart + (i * 8), y, Images::Cards_Bottom_Grey, game.players[Constants::HumanPlayer].getCard(i).getCardIndex() + currentPlane);
                }
                else {
                    SpritesU::drawPlusMaskFX(xStart + (i * 8), y,  Images::Cards_Bottom_Grey, currentPlane);
                }

            }
            else {

                if (rank != Rank::Joker) {
                    SpritesU::drawPlusMaskFX(xStart + (i * 8), y, Images::Cards_Bottom, game.players[Constants::HumanPlayer].getCard(i).getCardIndex() + currentPlane);
                }
                else {
                    SpritesU::drawPlusMaskFX(xStart + (i * 8), y,  Images::Cards_Bottom, currentPlane);
                }

            }

        }

    }


    if (game.players[2].getCardCount() > 0) {
        SpritesU::drawOverwriteFX(0, 4, Images::Hand_Left, (game.players[2].getCardCount() * 3) + currentPlane);
    }

    if (game.players[3].getCardCount() > 0) {
        SpritesU::drawOverwriteFX(31, -3, Images::Hand_Top, (game.players[3].getCardCount() * 3) + currentPlane);
    }

    if (game.players[0].getCardCount() > 0) {
        SpritesU::drawOverwriteFX(100, 4, Images::Hand_Right, (game.players[0].getCardCount() * 3) + currentPlane);
    }

}


void renderKitty(uint8_t currentPlane) {
    
    for (uint8_t i = 0; i < 3; i++) {
    
        if (game.gameRound->getKitty(i)->getRank() != Rank::None) {
            SpritesU::drawOverwriteFX(38 + (i * 6), 16, Images::Hand_Full, currentPlane); 
        }

    }

}


void renderDealer(uint8_t currentPlane) {

    uint8_t dealer = game.gameRound->getDealer_Idx();
    
    switch (dealer) {

        case 0:
            SpritesU::drawOverwriteFX(91, 23, Images::Dealer, currentPlane); 
            break;

        case 1:
            SpritesU::drawOverwriteFX(52, 40, Images::Dealer, currentPlane); 
            break;

        case 2:
            SpritesU::drawOverwriteFX(8, 23, Images::Dealer, currentPlane);                  
            break;

        case 3:
            SpritesU::drawOverwriteFX(52, 6, Images::Dealer, currentPlane); 
            break;

    }

}


void renderBids(uint8_t currentPlane) {
    

    uint8_t dealer = game.gameRound->getDealer_Idx();

    { // Player 0

        Bid &bid = game.gameRound->getBid(0);

        switch (bid.getBidType()) {

            case BidType::Suit:
                {
                    uint8_t idx = ((10 - bid.getLevel()) * 6) + static_cast<uint8_t>(bid.getSuit());
                
                    SpritesU::drawPlusMaskFX(91, 15, Images::Bid_Right, (idx * 3) + currentPlane); 
                
                    if (dealer == 0) {
                        SpritesU::drawOverwriteFX(92, 35 + (bid.getLevel() == 10 ? 2 : 0), Images::Dealer, currentPlane); 
                    }
                
                }
                break;

            case BidType::No_Trumps:
                {
                    uint8_t idx = (((10 - bid.getLevel()) * 6) + 4);                          
                
                    SpritesU::drawPlusMaskFX(91, 15, Images::Bid_Right, (idx * 3) + currentPlane); 
                
                    if (dealer == 0) {
                        SpritesU::drawOverwriteFX(92, 36 + (bid.getLevel() == 10 ? 2 : 0), Images::Dealer, currentPlane); 
                    }
                
                }
                break;

            case BidType::Pass:
                
                SpritesU::drawPlusMaskFX(91, 15, Images::Bid_Right, (23 * 3) + currentPlane); 
                
                if (dealer == 0) {
                    SpritesU::drawOverwriteFX(92, 38, Images::Dealer, currentPlane); 
                }

                break;
        
            case BidType::Misere:
                
                SpritesU::drawPlusMaskFX(91, 15, Images::Bid_Right, (29 * 3) + currentPlane); 
                
                if (dealer == 0) {
                    SpritesU::drawOverwriteFX(92, 40, Images::Dealer, currentPlane); 
                }

                break;

            case BidType::None:
                
                if (dealer == 0) {
                    SpritesU::drawOverwriteFX(91, 23, Images::Dealer, currentPlane); 
                }

                break;

        }


    }

    { // Player 1

        Bid &bid = game.gameRound->getBid(1);

        switch (bid.getBidType()) {

            case BidType::Suit:
                {
                    uint8_t idx = ((static_cast<uint8_t>(bid.getSuit()) * 5) + (bid.getLevel() - 6));
                    SpritesU::drawPlusMaskFX(41, 40, Images::Bid_Bottom, (idx * 3) + currentPlane); 

                    if (dealer == Constants::HumanPlayer) {
                        SpritesU::drawOverwriteFX(39 - (bid.getLevel() == 10 ? 2 : 0), 40, Images::Dealer, currentPlane); 
                    }

                }
                break;

            case BidType::No_Trumps:
                {
                    uint8_t idx = (20 + (bid.getLevel() - 6));
                    SpritesU::drawPlusMaskFX(41, 40, Images::Bid_Bottom, (idx * 3) + currentPlane); 

                    if (dealer == Constants::HumanPlayer) {
                        SpritesU::drawOverwriteFX(38 - (bid.getLevel() == 10 ? 2 : 0), 40, Images::Dealer, currentPlane); 
                    }

                }
                break;

            case BidType::Pass:

                SpritesU::drawPlusMaskFX(41, 40, Images::Bid_Bottom, (26 * 3) + currentPlane); 

                if (dealer == Constants::HumanPlayer) {
                    SpritesU::drawOverwriteFX(36, 40, Images::Dealer, currentPlane); 
                }

                break;
        
            case BidType::Misere:

                SpritesU::drawPlusMaskFX(41, 40, Images::Bid_Bottom, (25 * 3) + currentPlane); 

                if (dealer == Constants::HumanPlayer) {
                    SpritesU::drawOverwriteFX(34, 40, Images::Dealer, currentPlane); 
                }

                break;

            case BidType::None:

                if (dealer == Constants::HumanPlayer) {
                    SpritesU::drawOverwriteFX(52, 40, Images::Dealer, currentPlane); 
                }

                break;

        }


    }

    { // Player 2

        Bid &bid = game.gameRound->getBid(2);

        switch (bid.getBidType()) {

            case BidType::Suit:
                {
                    uint8_t idx = (((bid.getLevel() - 6) * 6) + (5 - static_cast<uint8_t>(bid.getSuit())));
                    SpritesU::drawPlusMaskFX(6, 16, Images::Bid_Left, (idx * 3) + currentPlane); 

                    if (dealer == 2) {
                        SpritesU::drawOverwriteFX(8, 13 - (bid.getLevel() == 10 ? 2 : 0), Images::Dealer, currentPlane);                  
                    }

                }
                break;

            case BidType::No_Trumps:
                {
                    uint8_t idx = (((bid.getLevel() - 6) * 6) + 1);
                    SpritesU::drawPlusMaskFX(6, 16, Images::Bid_Left, (idx * 3) + currentPlane); 

                    if (dealer == 2) {
                        SpritesU::drawOverwriteFX(8, 12 - (bid.getLevel() == 10 ? 2 : 0), Images::Dealer, currentPlane);                  
                    }

                }
                break;

            case BidType::Pass:

                SpritesU::drawPlusMaskFX(6, 16, Images::Bid_Left, (6 * 3) + currentPlane); 

                if (dealer == 2) {
                    SpritesU::drawOverwriteFX(8, 10, Images::Dealer, currentPlane);                  
                }

                break;
        
            case BidType::Misere:

                SpritesU::drawPlusMaskFX(6, 16, Images::Bid_Left, currentPlane); 

                if (dealer == 2) {
                    SpritesU::drawOverwriteFX(8, 8, Images::Dealer, currentPlane);                  
                }

                break;
        
            case BidType::None:

                if (dealer == 2) {
                    SpritesU::drawOverwriteFX(8, 23, Images::Dealer, currentPlane);                  
                }

                break;

        }

    }

    { // Player 3

        Bid &bid = game.gameRound->getBid(3);

        switch (bid.getBidType()) {

            case BidType::Suit:
                {
                    uint8_t idx = ((static_cast<uint8_t>(bid.getSuit()) * 5) + (bid.getLevel() - 6));
                    SpritesU::drawPlusMaskFX(41, 6, Images::Bid_Bottom, (idx * 3) + currentPlane); 

                    if (dealer == 3) {
                        SpritesU::drawOverwriteFX(61 + (bid.getLevel() == 10 ? 1 : 0), 6, Images::Dealer, currentPlane); 
                    }
                    
                }
                break;

            case BidType::No_Trumps:
                {
                    uint8_t idx = (20 + (bid.getLevel() - 6));
                    SpritesU::drawPlusMaskFX(41, 6, Images::Bid_Bottom, (idx * 3) + currentPlane); 

                    if (dealer == 3) {
                        SpritesU::drawOverwriteFX(62 + (bid.getLevel() == 10 ? 2 : 0), 6, Images::Dealer, currentPlane); 

                    }

               }
                break;

            case BidType::Pass:

                SpritesU::drawPlusMaskFX(41, 6, Images::Bid_Bottom, (26 * 3) + currentPlane); 

                if (dealer == 3) {
                    SpritesU::drawOverwriteFX(64, 6, Images::Dealer, currentPlane); 
                }

                break;
        
            case BidType::Misere:

                SpritesU::drawPlusMaskFX(41, 6, Images::Bid_Bottom, (25 * 3) + currentPlane); 

                if (dealer == 3) {
                    SpritesU::drawOverwriteFX(66, 6, Images::Dealer, currentPlane); 
                }

                break;

            case BidType::None:

                if (dealer == 3) {
                    SpritesU::drawOverwriteFX(52, 6, Images::Dealer, currentPlane); 
                }

                break;

        }

    }

}

void renderTableCards(uint8_t currentPlane, uint8_t winningHand) {
        
    if (winningHand != 3 || (winningHand == 3 && game.getFrameCount(48))) {

        Card *card = game.gameRound->getHand(3);

        if (card->getRank() != Rank::None)  {

            uint8_t idx = card->getCardIndex();

            // if (card->getRank() == Rank::Joker) {

            //     switch (card->getSuit()) {

            //         case Suit::Spades ... Suit::Hearts:
            //             idx = (44 + static_cast<uint8_t>(card->getSuit())) * 3;
            //             break;

            //         default:
            //             idx = 0;

            //     }

            // }
            
            SpritesU::drawPlusMaskFX(38, 9, Images::Cards_Bottom, idx + currentPlane); 
        }

    }
        
    if (winningHand != 1 || (winningHand == 1 && game.getFrameCount(48))) {

        Card *card = game.gameRound->getHand(1);

        if (card->getRank() != Rank::None)  {

            uint8_t idx = card->getCardIndex();

            // if (card->getRank() == Rank::Joker) {

            //     switch (card->getSuit()) {

            //         case Suit::Spades ... Suit::Hearts:
            //             idx = (44 + static_cast<uint8_t>(card->getSuit())) * 3;
            //             break;

            //         default:
            //             idx = 0;

            //     }

            // }

            SpritesU::drawPlusMaskFX(49, 19, Images::Cards_Bottom, idx + currentPlane); 
        }

    }
            
    if (winningHand != 0 || (winningHand == 0 && game.getFrameCount(48))) {

        Card *card = game.gameRound->getHand(0);

        if (card->getRank() != Rank::None)  {

            uint8_t idx = card->getCardIndex();

            // if (card->getRank() == Rank::Joker) {

            //     switch (card->getSuit()) {

            //         case Suit::Spades ... Suit::Hearts:
            //             idx = (44 + static_cast<uint8_t>(card->getSuit())) * 3;
            //             break;

            //         default:
            //             idx = 0;

            //     }

            // }

            SpritesU::drawPlusMaskFX(60, 13, Images::Cards_Right, idx + currentPlane); 
        }

    }
        
    if (winningHand != 2 || (winningHand == 2 && game.getFrameCount(48))) {

        Card *card = game.gameRound->getHand(2);

        if (card->getRank() != Rank::None)  {

            uint8_t idx = card->getCardIndex();

            // if (card->getRank() == Rank::Joker) {

            //     switch (card->getSuit()) {

            //         case Suit::Spades ... Suit::Hearts:
            //             idx = (44 + static_cast<uint8_t>(card->getSuit())) * 3;
            //             break;

            //         default:
            //             idx = 0;

            //     }

            // }

            SpritesU::drawPlusMaskFX(20, 21, Images::Cards_Left, idx + currentPlane); 

        }

    }

}



void renderHUD(uint8_t currentPlane, bool displayCard, bool displayWinningBid) {

    if (displayCard && !displayWinningBid) {

        renderHUD_Top(currentPlane, 0, false, false);
        renderHUD_Bottom(currentPlane, 6, true, true);

    }
    else if (!displayCard && !displayWinningBid) {

        renderHUD_Top(currentPlane, 3, false, false);
        renderHUD_Bottom(currentPlane, 6, true, true);

    }
    else if (!displayCard && displayWinningBid) {

        switch (gameState) {

            case GameState::Play_EndOfHand:
            case GameState::Play_EndOfRound:
            case GameState::Play_EndOfGame:

                if (game.gameRound->getWinningHand() == 1 || game.gameRound->getWinningHand() == 3) {

                    if (game.getFrameCount(48)) {

                        renderHUD_Top(currentPlane, 6, true, true);
    
                    }
                    else {
                    
                        renderHUD_Top(currentPlane, 9, false, true);

                    }
                
                }
                else {

                    if (game.getFrameCount(48)) {

                        renderHUD_Top(currentPlane, 6, true, true);
    
                    }
                    else {

                        renderHUD_Top(currentPlane, 12, true, false);

                    }


                }

                if (gameState != GameState::Play_EndOfHand) {

                    if (game.gameRound->getWinningBid_Team() == Constants::HumanTeam) {

                        if (game.getFrameCount(48)) {

                            renderHUD_Bottom(currentPlane, 6, true, true);
        
                        }
                        else {
                        
                            renderHUD_Bottom(currentPlane, 9, false, true);

                        }
                    
                    }
                    else {

                        if (game.getFrameCount(48)) {

                            renderHUD_Bottom(currentPlane, 6, true, true);
        
                        }
                        else {

                            renderHUD_Bottom(currentPlane, 12, true, false);

                        }


                    }

                }
                else {

                    renderHUD_Bottom(currentPlane, 6, true, true);

                }

                break;

            default:
                renderHUD_Top(currentPlane, 6, true, true);
                renderHUD_Bottom(currentPlane, 6, true, true);
                break;

        }


        SpritesU::drawOverwriteFX(107, 29, Images::Bid_Result_Narrow, (game.gameRound->getHighestBid().getImageIndex() * 3) + currentPlane);

        // int16_t score = game.gameRound->getScore(1);
        // uint16_t idx = abs(score) / 10;
        // if (idx > 99) idx = 99;

        // if (score < 0) idx = idx + 100;
        // SpritesU::drawOverwriteFX(113, 51, Images::HUD_Game_Score, (idx * 3) + currentPlane);

        // score = game.gameRound->getScore(0);
        // idx = abs(score) / 10;
        // if (idx > 99) idx = 99;

        // if (score < 0) idx = idx + 100;
        // SpritesU::drawOverwriteFX(113, 58, Images::HUD_Game_Score, (idx * 3) + currentPlane);

    }

}

void renderHUD_Top(uint8_t currentPlane, uint8_t backgroundIdx, bool showTeam0, bool showTeam1) {

    SpritesU::drawOverwriteFX(105, 0, Images::HUD_Top, backgroundIdx + currentPlane);
    if (showTeam0)    SpritesU::drawOverwriteFX(118, 8, Images::HUD_Trick_Score, (game.gameRound->getTeam_TrickCount(1) * 3) + currentPlane);
    if (showTeam1)    SpritesU::drawOverwriteFX(118, 15, Images::HUD_Trick_Score, (game.gameRound->getTeam_TrickCount(0) * 3) + currentPlane);

}


void renderHUD_Bottom(uint8_t currentPlane, uint8_t backgroundIdx, bool showTeam0, bool showTeam1) {

    SpritesU::drawOverwriteFX(105, 32, Images::HUD_Bottom, backgroundIdx + currentPlane);

    int16_t score = game.gameRound->getScore(1);
    uint16_t idx = abs(score) / 10;
    if (idx > 99) idx = 99;

    if (score < 0) idx = idx + 100;
    if (showTeam0) SpritesU::drawOverwriteFX(114, 51, Images::HUD_Game_Score, (idx * 3) + currentPlane);

    score = game.gameRound->getScore(0);
    idx = abs(score) / 10;
    if (idx > 99) idx = 99;

    if (score < 0) idx = idx + 100;
    if (showTeam1) SpritesU::drawOverwriteFX(114, 58, Images::HUD_Game_Score, (idx * 3) + currentPlane);

}