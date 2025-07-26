#include <ArduboyFX.h>  


void renderPlayerHands(uint8_t currentPlane, bool blinkSelected, bool blinkRaised) {

    uint8_t cardCount = game.players[Constants::HumanPlayer].getCardCount();
    int8_t xStart = 48 - (cardCount * 4) + (cardCount > 10 ? 3 : 0);

    for (uint8_t i = 0; i < cardCount; i++) {

        Card &card = game.players[Constants::HumanPlayer].getCard(i);
        Rank rank = card.getRank();

        if (rank != Rank::None) {

            uint8_t y = 47 - (card.isSelected() ? 4 : 0);

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

                    if (dealer == 1) {
                        SpritesU::drawOverwriteFX(39 - (bid.getLevel() == 10 ? 2 : 0), 40, Images::Dealer, currentPlane); 
                    }

                }
                break;

            case BidType::No_Trumps:
                {
                    uint8_t idx = (20 + (bid.getLevel() - 6));
                    SpritesU::drawPlusMaskFX(41, 40, Images::Bid_Bottom, (idx * 3) + currentPlane); 

                    if (dealer == 1) {
                        SpritesU::drawOverwriteFX(38 - (bid.getLevel() == 10 ? 2 : 0), 40, Images::Dealer, currentPlane); 
                    }

                }
                break;

            case BidType::Pass:

                SpritesU::drawPlusMaskFX(41, 40, Images::Bid_Bottom, (26 * 3) + currentPlane); 

                if (dealer == 1) {
                    SpritesU::drawOverwriteFX(36, 40, Images::Dealer, currentPlane); 
                }

                break;
        
            case BidType::Misere:

                SpritesU::drawPlusMaskFX(41, 40, Images::Bid_Bottom, (25 * 3) + currentPlane); 

                if (dealer == 1) {
                    SpritesU::drawOverwriteFX(34, 40, Images::Dealer, currentPlane); 
                }

                break;

            case BidType::None:

                if (dealer == 1) {
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

        if (game.gameRound->getHand(3)->getRank() != Rank::None)  {

            uint8_t idx = game.gameRound->getHand(3)->getCardIndex();
            if (game.gameRound->getHand(3)->getRank() == Rank::Joker) idx = 0;
            
            SpritesU::drawPlusMaskFX(38, 9, Images::Cards_Bottom, idx + currentPlane); 
        }

    }
        
    if (winningHand != 1 || (winningHand == 1 && game.getFrameCount(48))) {

        if (game.gameRound->getHand(1)->getRank() != Rank::None)  {

            uint8_t idx = game.gameRound->getHand(1)->getCardIndex();
            if (game.gameRound->getHand(1)->getRank() == Rank::Joker) idx = 0;

            SpritesU::drawPlusMaskFX(49, 19, Images::Cards_Bottom, idx + currentPlane); 
        }
    }
            
    if (winningHand != 0 || (winningHand == 0 && game.getFrameCount(48))) {

        if (game.gameRound->getHand(0)->getRank() != Rank::None)  {

            uint8_t idx = game.gameRound->getHand(0)->getCardIndex();
            if (game.gameRound->getHand(0)->getRank() == Rank::Joker) idx = 0;

            SpritesU::drawPlusMaskFX(60, 13, Images::Cards_Right, idx + currentPlane); 
        }

    }
        
    if (winningHand != 2 || (winningHand == 2 && game.getFrameCount(48))) {

        if (game.gameRound->getHand(2)->getRank() != Rank::None)  {

            uint8_t idx = game.gameRound->getHand(2)->getCardIndex();
            if (game.gameRound->getHand(2)->getRank() == Rank::Joker) idx = 0;

            SpritesU::drawPlusMaskFX(20, 21, Images::Cards_Left, idx + currentPlane); 
        }

    }

}