/*
livius - a TLCV-compatible live chess viewer

Copyright (c) 2014 Martin Sedlak (mar)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#pragma once

#include "core/types.h"
#include "core/thread.h"
#include "chess/board.h"
#include "sig/signal.h"
#include <vector>
#include <map>
#include <QString>

struct PGNIndex
{
	core::u64		offset;		// file offset
    size_t          index;		// index (fast unsorted view)
    size_t          size;		// chunk size (including header)
    size_t          hdrSize;    // header chunk size
};

// even better would be to collect players and stuff...
// using indices
// game index
// white
// white elo
// black
// black elo
// event
// site
// round
// date
// result
// ECO
// length

// that would be:
// 12 indices * 4 = 48 bytes + 4 + 8 = 52+8 = 60 bytes per index (too much)
// so we'll have to be able to fast-parse header to do sorting

struct PGNTag
{
    QString key, value;
};

struct PGNHeader
{
    std::vector< PGNTag > tags;
};

class PGNFile;

class PGNLoadThread : public core::Thread
{
	volatile bool abortFlag;
public:
    PGNLoadThread();

    // associated file
    PGNFile *file;

	// abort scanning
	void abort();
	void work();
};

enum PGNResult
{
    prUndetermined,
    prWhiteWon,
    prBlackWon,
    prDraw
};

struct PGNNode;

struct PGNMove
{
    cheng4::Move move;                  // move
    // there can be more symbols associated with a single move!
    std::vector<cheng4::uint> nags;     // NAG sequence
    PGNNode *next;                      // leads to this node after move

    PGNMove();
    ~PGNMove();
private:
    PGNMove( const PGNMove & ) {}
};

// rework variations: variation appears AFTER parent variation move (=undo last move, continue variation)

// represents node in game tree
struct PGNNode
{
    PGNNode *parent;                    // back link to parent node
    cheng4::Board board;                // board at this move (wastes space but keeps things simple)
    QString comment;                    // comment associated with this node
    std::vector< PGNMove > variations;  // variations (variation 0 is main for this node)

    PGNNode();

private:
    PGNNode( const PGNNode & ) {}
};

// represents one PGN game
class PGNGame
{
public:
    PGNNode *root;                      // root node
    PGNHeader header;                   // game header
    PGNResult result;                   // game result

    PGNGame();
    ~PGNGame();
};

class PGNIndexParser
{
    cheng4::u32 ofs;
    cheng4::u64 line;
    cheng4::u64 column;
    cheng4::u32 count;
    std::vector< PGNIndex > &index;
    QString error;

    enum State
    {
        stScan,         // scan for header
        stHeader,       // skip header
        stGame          // parsing game
    };

    enum LexState
    {
        lsScan,         // default state
        lsComment,      // skipping comment
        lsMultiComment, // multiline comment
        lsEol,          // last char was 13, consume 10 if next
        lsCommentEol,   // multiline eol (last char was 13)
        lsString,       // string
        lsStringEscape  // string escape sequence
    };

    State state;        // current parser state
    LexState lexState;  // lexer state
    bool inTag;         // inside tag

    void flushHeader( cheng4::u64 offset );

public:
    PGNIndexParser( std::vector< PGNIndex > &indexRef );
    void reset();
    cheng4::u64 getLine() const;
    QString getError() const;
    bool parseBuffer( const char *buf, size_t sz );
    void flushIndex( cheng4::u64 offset );
};

// PGN file reader
class PGNFile
{
public:
    friend class PGNLoadThread;

	PGNFile();
	~PGNFile();
	
    // start loading PGN file (background)
    bool load( const QString &fname );

    // wait for load finished
    void wait();

    // reload PGN file (background)
    bool reload();

    // sort index (must be loaded)
    // returns 0 if not fully loaded yet
    bool sort( const QString &key, bool descending = 0 );

    // clear PGN file (+reset parser)
    void clear();

	// current game count (may change while parsing)
	size_t getCount() const;
	// get (sorted) index, returns true on success
    bool getIndex( size_t idx, PGNIndex &data );
	// load header for corresponding index
    bool loadHeader( const PGNIndex &index, PGNHeader &hdr );

    // raw low level function to load data
    bool loadRawData( cheng4::u64 offset, size_t size, std::vector< cheng4::u8 > &data );

    // called when (re)loading has started
    sig::Signal<void> onLoadStarted;
    // progress signal (called from load thread)
    // param: percent complete
    sig::Signal<void, int> onLoadProgress;
    // called when read error occurs
    sig::Signal<void> onLoadError;
    // called when parser error occurs
    // param: line, error message
    sig::Signal<void, qint64, const QString &> onParseError;
    // always called when read is complete (even in the case of an error)
    sig::Signal<void> onLoadComplete;

private:
    // kill loader thread
    void killThread();

    // parse PGN buffer (and index)
    bool parseBuffer( const char *buf, size_t sz );

	// there can be MANY (many!!) games in a PGN file, thus we implement some caching here:
	// the PGN index describes positions of games inside the file

	// index describes relative position within PGN files so they can be read relatively fast
	std::vector< PGNIndex > index;
	bool utf8;							// use UTF-8 encoding (preferred for new pgn files)
    bool loaded;                        // already loaded
    PGNLoadThread *thread;              // loader thread
    QString fileName;                   // associated filename
    PGNIndexParser parser;              // index parser
	mutable core::Mutex mutex;
};

// think of some problems:
// how to edit games?
// ... etc ... => only edit existing games, never append or delete
