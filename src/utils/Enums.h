#pragma once

enum class BidMode : uint8_t { 
    Level, 
    Suit, 
    Bid, 
    Pass, 
    Misere,
};

inline BidMode &operator++(BidMode &c) {
    c = static_cast<BidMode>( static_cast<uint8_t>(c) + 1 );
    return c;
}

inline BidMode operator++(BidMode &c, int) {
    BidMode result = c;
    ++c;
    return result;
}

inline BidMode &operator--(BidMode &c) {
    c = static_cast<BidMode>( static_cast<uint8_t>(c) - 1 );
    return c;
}

inline BidMode operator--(BidMode &c, int) {
    BidMode result = c;
    --c;
    return result;
}

enum class Suit : uint8_t { 
    Spades, 
    Clubs, 
    Diamonds, 
    Hearts, 
    No_Trumps = 254, 
    None = 255, 
};

inline Suit &operator++(Suit &c) {
    c = static_cast<Suit>( static_cast<uint8_t>(c) + 1 );
    return c;
}

inline Suit operator++(Suit &c, int) {
    Suit result = c;
    ++c;
    return result;
}

enum class Rank : uint8_t { 
    None = 0, 
    Three = 3, 
    Four, 
    Five, 
    Six, 
    Seven, 
    Eight, 
    Nine, 
    Ten, 
    Jack, 
    Queen, 
    King, 
    Ace, 
    Left_Bower = 15, 
    Right_Bower = 16, 
    Joker = 17,
    TopCard = 255 
};

inline Rank &operator++(Rank &c) {
    c = static_cast<Rank>( static_cast<uint8_t>(c) + 1 );
    return c;
}

inline Rank operator++(Rank &c, int) {
    Rank result = c;
    ++c;
    return result;
}

inline Rank &operator--(Rank &c) {
    c = static_cast<Rank>( static_cast<uint8_t>(c) - 1 );
    return c;
}

inline Rank operator--(Rank &c, int) {
    Rank result = c;
    --c;
    return result;
}

enum class BidType : uint8_t { 
    Pass, 
    Suit, 
    No_Trumps, 
    Misere, 
    #ifdef OPEN_MISERE
    Open_Misere,
    #endif
    None
};

enum class TriState : uint8_t {
    False,
    True,
    Maybe
};

enum class GameState : uint8_t {

    SplashScreen_Start,
        SplashScreen_00 = SplashScreen_Start,
        SplashScreen_01,
        SplashScreen_02,
        SplashScreen_03,
    SplashScreen_End,

    Title_Init,
    Title_Start,
        Title_OptPlay = Title_Start,
        Title_Assist,
    Title_End,

    Play_Init,
    Play_Start,
        Play_Shuffle = Play_Start,
        Play_Deal_00, 
        Play_Deal_01, 
        Play_Deal_02, 
        Play_Deal_03, 
        Play_Deal_04, 
        Play_Deal_05, 
        Play_Deal_06, 
        Play_Deal_07, 
        Play_Deal_08, 
        Play_Deal_09, 
        Play_Deal_10, 
        Play_Deal_11, 
        Play_Deal_12, 
        Play_Deal_13, 
        Play_Deal_14, 
        Play_Deal_15, 
        Play_Deal_16, 
        Play_Deal_17, 
        Play_Deal_18, 
        Play_Deal_19, 
        Play_Deal_20, 
        Play_Deal_21, 
        Play_Deal_22, 
        Play_Deal_23, 
        Play_Deal_24, 
        Play_Deal_25, 
        Play_Deal_26, 
        Play_Deal_27, 
        Play_Deal_28, 
        Play_Deal_29, 
        Play_Deal_30, 
        Play_Deal_31, 
        Play_Deal_32, 
        Play_Deal_33, 
        Play_Deal_34, 
        Play_Deal_35, 
        Play_Deal_36, 
        Play_Deal_37, 
        Play_Deal_38, 
        Play_Deal_39, 
        Play_Deal_40, 
        Play_Deal_41, 
        Play_Deal_42, 
        Play_Deal_Catchup, 
        Bid, 
        Bid_Player, 
        Bid_Finished,
        Bid_Failed,
        Bid_Error,
        Handle_Kitty,
        Play_Round_Start,
        Play_Hand,
        Play_PlayerInput,
        Play_SelectSuitForJoker,
        Play_EndOfHand,
        Play_EndOfRound,
        Play_EndOfGame,
    Play_End = Play_EndOfGame,
    
};

inline GameState &operator++(GameState &c) {
    c = static_cast<GameState>( static_cast<uint8_t>(c) + 1 );
    return c;
}

inline GameState operator++(GameState &c, int) {
    GameState result = c;
    ++c;
    return result;
}

inline GameState &operator--(GameState &c) {
    c = static_cast<GameState>( static_cast<uint8_t>(c) - 1 );
    return c;
}

inline GameState operator--(GameState &c, int) {
    GameState result = c;
    --c;
    return result;
}

enum Direction : uint8_t { 
    Down,
    Up
};
