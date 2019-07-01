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

#ifndef base_h
#define base_h

#include <stdio.h>

#include "comm.h"

namespace banksia {
    
    class Result {
    public:
        Result() {
            reset();
        }
        Result(ResultType _result, ReasonType _reason = ReasonType::noreason, std::string _comment = "") {
            result = _result;
            reason = _reason;
            comment = _comment;
        }
        
        void reset() {
            result = ResultType::noresult;
            reason = ReasonType::noreason;
            comment = "";
        }
        
        ResultType result;
        ReasonType reason;
        std::string comment;
        
        bool isNone() const {
            return result == ResultType::noresult;
        }
        
        std::string reasonString() const {
            return reasonStrings[static_cast<int>(reason)];
        }
        
        std::string toShortString() const {
            switch (result) {
                case ResultType::draw:
                    return "1/2-1/2";
                case ResultType::win:
                    return "1-0";
                case ResultType::loss:
                    return "0-1";
                case ResultType::noresult:
                default:
                    return "*";
                    break;
            }
            
            return "";
        }
    };
    
    class Piece {
    public:
        PieceType type;
        Side side;
        
    public:
        Piece() {}
        Piece(PieceType _type, Side _side) {
            set(_type, _side);
        }
        
        void set(PieceType _type, Side _side) {
            type = _type;
            side = _side;
            assert(isValid());
        }
        
        void setEmpty() {
            set(PieceType::empty, Side::none);
        }
        
        bool isEmpty() const {
            return type == PieceType::empty;
        }
        
        bool isPiece(PieceType _type, Side _side) const {
            return type == _type && side == _side;
        }
        
        bool isValid() const {
            return (side == Side::none && type == PieceType::empty) || (side != Side::none && type != PieceType::empty);
        }
        
        static std::string toString(const PieceType type, const Side side) {
            int k = static_cast<int>(type);
            char ch = pieceTypeName[k];
            if (side == Side::white) {
                ch += 'A' - 'a';
            }
            return std::string(1, ch);
        }
        
        std::string toString() const {
            return toString(type, side);
        }
    };
    
    
    class Move {
    public:
        int from, dest;
        PieceType promotion;
        Piece piece;
        
        Move() {}
        Move(Piece piece, int _from, int _dest, PieceType _promote = PieceType::empty) {
            set(piece, _from, _dest, _promote);
        }
        
        static Move illegalMove;
        
        Move(int _from, int _dest, PieceType _promote = PieceType::empty) {
            from = _from;
            dest = _dest;
            promotion = _promote;
        }
        
        void set(Piece _piece, int _from, int _dest, PieceType _promote = PieceType::empty) {
            piece = _piece;
            from = _from;
            dest = _dest;
            promotion = _promote;
        }
        
        void set(int _from, int _dest, PieceType _promote) {
            from = _from;
            dest = _dest;
            promotion = _promote;
        }
        
        bool isValid() const {
            return isValid(from, dest);
        }
        
        static bool isValid(int from, int dest) {
            return from != dest  && from >= 0 && from < 64 && dest >= 0 && dest < 64;
        }
        
        bool operator == (const Move& other) const {
            return from == other.from && dest == other.dest && promotion == other.promotion;
        }
        
        std::string toString() const {
            std::ostringstream stringStream;
            stringStream << posToCoordinateString(from) << posToCoordinateString(dest);
            if (promotion != PieceType::empty) {
                stringStream << "(" << Piece(promotion, Side::white).toString() << ")";
            }
            return stringStream.str();
        }
        
        std::string toCoordinateString() const {
            auto s = posToCoordinateString(from) + posToCoordinateString(dest);
            if (promotion > PieceType::king && promotion < PieceType::pawn) s += pieceTypeName[static_cast<int>(promotion)];
            return s;
        }
    };
    
    class MoveList {
    public:
        static const int MaxMoveNumber = 250;
        
        Move list[MaxMoveNumber];
        int end;
        
        MoveList() {
            reset();
        }
        
        void reset() {
            end = 0;
        }
        
        bool isEmpty() const {
            return end == 0;
        }
        
        bool isFull() const {
            return end >= MaxMoveNumber - 2;
        }
        
        void add(const Move& move) {
            list[end] = move;
            end++;
        }
        
        void add(Piece piece, int from, int dest, PieceType promotion = PieceType::empty) {
            list[end].set(piece, from, dest, promotion);
            end++;
        }
        
        bool isValid() const {
            return end >= 0 && end < MaxMoveNumber;
        }
        
        std::string toString() const {
            std::ostringstream stringStream;
            
            for (int i = 0; i < end; i++) {
                stringStream << i + 1 << ") " << list[i].toString() << " ";
            }
            return stringStream.str();
        }
    };
    
    class Hist {
    public:
        Move move;
        Piece movep, cap;
        int enpassant, status;
        int8_t castleRights[2];
        u64 hashKey;
        int quietCnt;
        double elapsed;
        std::string moveString;
        
        void set(const Move& _move) {
            move = _move;
        }
        
        bool isValid() const {
            return move.isValid() && cap.isValid();
        }
    };
    
    class BoardCore : public Obj {
    protected:
        std::vector<Piece> pieces;
        
    public:
        Side side;
        std::vector<Hist> histList;
        
        int _status;
        Result result;
        
    public:
        void reset() {
            for (auto && p : pieces) {
                p.setEmpty();
            }
        }
        
        virtual bool isPositionValid(int pos) const {
            return pos >= 0 && pos < pieces.size();
        }
        
        void setPiece(int pos, Piece piece) {
            assert(isPositionValid(pos));
            pieces[pos] = piece;
        }
        
        Piece getPiece(int pos) const {
            assert(isPositionValid(pos));
            return pieces.at(pos);
        }
        
        bool isEmpty(int pos) const {
            assert(isPositionValid(pos));
            return pieces[pos].type == PieceType::empty;
        }
        
        bool isPiece(int pos, PieceType type, Side side) const {
            assert(isPositionValid(pos));
            auto p = pieces[pos];
            return p.type == type && p.side == side;
        }
        
        void setEmpty(int pos) {
            assert(isPositionValid(pos));
            pieces[pos].setEmpty();
        }
        
    public:
        BoardCore();
        
        virtual void setFen(const std::string& fen) = 0;
        virtual std::string getFen(int halfCount = 0, int fullMoveCount = 1) const = 0;
        
        
        virtual u64 key() const {
            return hashKey;
        }
        virtual u64 initHashKey() const;
        
    public:
        bool fromOriginPosition() const;
        std::string getStartingFen() const;
        
        void newGame(std::string fen = "");
        
        Move createMove(int from, int dest, PieceType promote) const;
        static PieceType charactorToPieceType(char ch);
        static Move moveFromCoordiateString(const std::string& moveString);
        
        bool isValidPromotion(PieceType promotion, Side side) const;
        
        virtual Result rule() = 0;
        
    protected:
        virtual bool istHashKeyValid() const;
        virtual u64 xorHashKey(int pos) const;
        
        int quietCnt;
        u64 hashKey;
        
        static u64 hashForSide;
        static std::vector<u64> hashTable;
        
        std::string startFen;
    };
    
} // namespace banksia

#endif /* board_hpp */

