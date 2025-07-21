#include <ArduboyFX.h>  


void renderPlayerHands(uint8_t currentPlane, uint8_t color) {

    uint8_t cardCount = game.players[Constants::HumanPlayer].getCardCount();

    uint8_t xStart = 48 - (cardCount * 4);

    for (uint8_t i = 0; i < cardCount; i++) {

        Rank rank = game.players[Constants::HumanPlayer].getCard(i).getRank();

        if (rank != Rank::None) {

            if (rank != Rank::Joker) {
                SpritesU::drawPlusMaskFX(xStart + (i * 8), 47, Images::Cards_Bottom, game.players[Constants::HumanPlayer].getCard(i).getCardIndex() + currentPlane);
            }
            else {
                SpritesU::drawPlusMaskFX(xStart + (i * 8), 47,  Images::Cards_Bottom, currentPlane);
            }

        }

    }

    if (color == LIGHT_GREY) {

        if (game.players[2].getCardCount() > 0) {
            SpritesU::drawOverwriteFX(0, 10, Images::Hand_Left, (game.players[2].getCardCount() * 3) + currentPlane);
        }

        if (game.players[3].getCardCount() > 0) {
            SpritesU::drawOverwriteFX(31, -3, Images::Hand_Top, (game.players[3].getCardCount() * 3) + currentPlane);
        }

        if (game.players[0].getCardCount() > 0) {
            SpritesU::drawOverwriteFX(100, 10, Images::Hand_Right, (game.players[0].getCardCount() * 3) + currentPlane);
        }

    }
    else {

        if (game.players[2].getCardCount() > 0) {
            SpritesU::drawOverwriteFX(0, 10, Images::Hand_Left, ((game.players[2].getCardCount() + 11) * 3) + currentPlane);
        }

        if (game.players[3].getCardCount() > 0) {
            SpritesU::drawOverwriteFX(31, -3, Images::Hand_Top, ((game.players[3].getCardCount() + 11) * 3) + currentPlane);
        }

        if (game.players[0].getCardCount() > 0) {
            SpritesU::drawOverwriteFX(100, 10, Images::Hand_Right, ((game.players[0].getCardCount() + 11) * 3) + currentPlane);
        }

    }

}

// void renderPlayerHands_ValidOptions(uint8_t currentPlane) {

//     uint8_t playerCardCount = game.players[Constants::HumanPlayer].getCardCount();
//     uint8_t xStart = 48 - (playerCardCount * 4);

//     if (playerCardCount == 1) {

//         // game.setSelectedCard(0);
//         renderHumanHand_LightGrey(currentPlane);

//     }
//     else {

//         uint8_t tableCardCount = game.gameRound->getTableCardCount();

//         if (tableCardCount == 0) {

//             renderHumanHand_LightGrey(currentPlane);

//         }
//         else {

//             // bool hasSuit = game.players[Constants::HumanPlayer].hasSuit(game.gameRound->getHand(game.getFirstPlayer()).getSuit());
//             // Card &firstCard = game.gameRound->getHand(game.getFirstPlayer());

//             // if (hasSuit) {

//             //     for (uint8_t i = 0; i < playerCardCount; i++) {

//             //         Card &card = game.players[Constants::HumanPlayer].getCard(i);

//             //         if (card.getSuit() != Suit::None) {

//             //             if (card.getSuit() == firstCard.getSuit()) {

//             //                 if (game.getSelectedCard() == Constants::NoCard || game.getSelectedCard() == i) {

//             //                     game.setSelectedCard(i);
//             //                     SpritesU::drawPlusMaskFX(xStart + (i * 8), 43, Images::Cards_Bottom, card.getCardIndex() + currentPlane);

//             //                 }
//             //                 else {

//             //                     SpritesU::drawPlusMaskFX(xStart + (i * 8), 47, Images::Cards_Bottom, card.getCardIndex() + currentPlane);

//             //                 }

//             //                 game.setValidCard(i, true);

//             //             }
//             //             else {

//             //                 game.setValidCard(i, false);
//             //                 SpritesU::drawPlusMaskFX(xStart + (i * 8), 47, Images::Cards_Bottom_Grey, card.getCardIndex() + currentPlane);

//             //             }

//             //         }

//             //     }

//             // }
//             // else {

//                 renderHumanHand_LightGrey(currentPlane);

//             // }

//         }

//     }

//     SpritesU::drawOverwriteFX(0, 10, Images::Hand_Left, (game.players[2].getCardCount() * 3) + currentPlane);
//     SpritesU::drawOverwriteFX(31, -3, Images::Hand_Top, (game.players[3].getCardCount() * 3) + currentPlane);
//     SpritesU::drawOverwriteFX(100, 10, Images::Hand_Right, (game.players[0].getCardCount() * 3) + currentPlane);


// }

// void renderHumanHand_LightGrey(uint8_t currentPlane) {
  
//     uint8_t cardCount = game.players[Constants::HumanPlayer].getCardCount();
//     uint8_t xStart = 48 - (cardCount * 4);

//     for (uint8_t i = 0; i < cardCount; i++) {

//         Card &card = game.players[Constants::HumanPlayer].getCard(i);

//         if (card.getSuit() != Suit::None) {

// //            game.setValidCard(i, true);

//             if (game.getSelectedCard() == Constants::NoCard || game.getSelectedCard() == i) {

//                 game.setSelectedCard(i);
//                 SpritesU::drawPlusMaskFX(xStart + (i * 8), 43, Images::Cards_Bottom, card.getCardIndex() + currentPlane);

//             }
//             else {

//                 SpritesU::drawPlusMaskFX(xStart + (i * 8), 47, Images::Cards_Bottom, card.getCardIndex() + currentPlane);

//             }

//         }

//     }

// }

void renderKitty(uint8_t currentPlane) {
    
    for (uint8_t i = 0; i < 3; i++) {
    
        if (game.gameRound->getKitty(i)->getRank() != Rank::None) {
            SpritesU::drawOverwriteFX(38 + (i * 6), 16, Images::Hand_Full, currentPlane); 
        }

    }

}


void renderBids(uint8_t currentPlane) {
    

    { // Player 0

        Bid &bid = game.gameRound->getBid(0);

        switch (bid.getBidType()) {

            case BidType::Suit:
                {
                    uint8_t idx = ((10 - bid.getLevel()) * 6) + static_cast<uint8_t>(bid.getSuit());
                    SpritesU::drawPlusMaskFX(91, 15, Images::Bid_Right, (idx * 3) + currentPlane); 
                }
                break;

            case BidType::No_Trumps:
                {
                    uint8_t idx = (((10 - bid.getLevel()) * 6) + 5);                          
                    SpritesU::drawPlusMaskFX(91, 15, Images::Bid_Right, (idx * 3) + currentPlane); 
                }
                break;

            case BidType::Pass:
                SpritesU::drawPlusMaskFX(91, 15, Images::Bid_Right, (23 * 3) + currentPlane); 
                break;
        
            case BidType::Misere:
                SpritesU::drawPlusMaskFX(91, 15, Images::Bid_Right, (29 * 3) + currentPlane); 
                break;


        }


    }

    { // Player 1

        Bid &bid = game.gameRound->getBid(1);

        switch (bid.getBidType()) {

            case BidType::Suit:
                {
                    uint8_t idx = ((static_cast<uint8_t>(bid.getSuit()) * 5) + (bid.getLevel() - 6));
                    SpritesU::drawPlusMaskFX(14, 40, Images::Bid_Bottom, (idx * 3) + currentPlane); 
                }
                break;

            case BidType::No_Trumps:
                {
                    uint8_t idx = (20 + (bid.getLevel() - 6));
                    SpritesU::drawPlusMaskFX(14, 40, Images::Bid_Bottom, (idx * 3) + currentPlane); 
                }
                break;

            case BidType::Pass:
                SpritesU::drawPlusMaskFX(14, 40, Images::Bid_Bottom, (26 * 3) + currentPlane); 
                break;
        
            case BidType::Misere:
                SpritesU::drawPlusMaskFX(14, 40, Images::Bid_Bottom, (25 * 3) + currentPlane); 
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
                }
                break;

            case BidType::No_Trumps:
                {
                    uint8_t idx = (((bid.getLevel() - 6) * 6) + 1);
                    SpritesU::drawPlusMaskFX(6, 16, Images::Bid_Left, (idx * 3) + currentPlane); 
                }
                break;

            case BidType::Pass:
                SpritesU::drawPlusMaskFX(6, 16, Images::Bid_Left, (6 * 3) + currentPlane); 
                break;
        
            case BidType::Misere:
                SpritesU::drawPlusMaskFX(6, 16, Images::Bid_Left, currentPlane); 
                break;


        }


    }

    { // Player 3

        Bid &bid = game.gameRound->getBid(3);

        switch (bid.getBidType()) {

            case BidType::Suit:
                {
                    uint8_t idx = ((static_cast<uint8_t>(bid.getSuit()) * 5) + (bid.getLevel() - 6));
                    SpritesU::drawPlusMaskFX(64, 6, Images::Bid_Bottom, (idx * 3) + currentPlane); 
                }
                break;

            case BidType::No_Trumps:
                {
                    uint8_t idx = (20 + (bid.getLevel() - 6));
                    SpritesU::drawPlusMaskFX(64, 6, Images::Bid_Bottom, (idx * 3) + currentPlane); 
                }
                break;

            case BidType::Pass:
                SpritesU::drawPlusMaskFX(64, 6, Images::Bid_Bottom, (26 * 3) + currentPlane); 
                break;
        
            case BidType::Misere:
                SpritesU::drawPlusMaskFX(64, 6, Images::Bid_Bottom, (25 * 3) + currentPlane); 
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