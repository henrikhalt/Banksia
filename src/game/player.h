/*
 This file is part of Banksia, distributed under MIT license.
 
 Copyright (c) 2019 Nguyen Hong Pham
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */


#ifndef player_hpp
#define player_hpp

#include <stdio.h>

#include "../chess/chess.h"
#include "time.h"

namespace banksia {
    enum class PlayerType {
        human, engine, none
    };
    
    enum class PlayerState {
        none, starting, ready, playing, stalling, stopping, stopped
    };
    
    enum class EngineComputingState {
        idle, thinking, pondering
    };
    
    class Player : public Obj, public Tickable
    {
    public:
        Player();
        Player(const std::string& name, PlayerType type);
        virtual ~Player() {}
        
        virtual bool isValid() const override;
        virtual std::string toString() const override;
        virtual const char* className() const override { return "Player"; }
        
        void addMoveReceiver(void* parent, std::function<void(const std::string&, const std::string&, double, EngineComputingState)>);
        
    public:
        virtual bool isHuman() const = 0;
        
        virtual bool kickStart() = 0;
        virtual bool stopThinking() = 0;
        virtual bool quit() = 0;
        virtual bool kill() = 0;
        
        virtual void newGame() {}
        
        void setup(const ChessBoard*, const GameTimeController*);
        bool isAttachedToGame() const;
        
        virtual bool goPonder(const Move& pondermove);
        virtual bool go();
        
    public:
        std::string name;
        PlayerType type;
        
        PlayerState getState() const { return state; }
        void setState(PlayerState st) { state = st; }
    protected:
        PlayerState state;
        
        std::function<void(const std::string& moveString, const std::string& ponderMoveString, double, EngineComputingState)> moveReceiver = nullptr;
        
        
        const ChessBoard* board = nullptr;
        const GameTimeController* timeController = nullptr;
    };
    
    /////////////////////////////////////
    class Human : public Player
    {
    public:
        Human() : Player("", PlayerType::human) {}
        Human(const std::string& name) : Player(name, PlayerType::human) {}
        
        virtual const char* className() const override { return "Human"; }
        
        virtual void tick() override {}
        
        virtual bool isHuman() const override {
            return true;
        }
        
    public:
        virtual bool kickStart() override;
        virtual bool stopThinking() override;
        
        virtual bool quit() override;
        virtual bool kill() override;
    };
    
} // namespace banksia


#endif /* player_hpp */

