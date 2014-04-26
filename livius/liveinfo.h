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

#ifndef LIVEINFO_H
#define LIVEINFO_H

#include <QWidget>
#include <vector>
#include "sig/signal.h"
#include "chess/chess.h"

namespace Ui {
class LiveInfo;
}

class PieceSet;
class ChessBoard;

class LiveInfo : public QWidget
{
	Q_OBJECT

public:
	explicit LiveInfo(QWidget *parent = 0, PieceSet *pset = 0);
	~LiveInfo();

	// set fen string
	void setFEN( const QString &fen );
	// get fen string
	QString getFEN() const;

	// set player name
	void setPlayer( int color, const QString &txt );
	// get player name
	QString getPlayer( int color ) const;
	// set depth
	void setDepth( int color, int depth );
	// set score (cp)
	void setScore( int color, int score );
	// set nodes
	void setNodes( int color, double nodes, double nps );
	// set PV
	// pretty PV: parse and replace
	void setPV( int color, QString txt, bool pretty = 0, bool pvtip = 0, const cheng4::Board *board = 0 );
	// set time
	void setTime( int color, double time, double otim );
	// set turn
	void setTurn( int color );
	// set level/moves
	void setLevelMoves( const QString &txt );
	// set level/time
	void setLevelTime( const QString &txt );
	// set level/increment
	void setLevelIncrement( const QString &txt );
	// set last move text
	void setLastMove( const QString &txt );
	// set fifty rule counter
	void setFiftyRule( const QString &fifty );

	// visually flip players
	void flipPlayers();

	// refresh (to update times)
	void refresh();

	Ui::LiveInfo *getUI() const
	{
		return ui;
	}

	sig::Signal< void > sigCopyFEN;

private slots:
	void copyFEN();

private:
	void setTime();

	struct PVInfo
	{
		// board is startpos, tip is endpos
		// both are valiad only if moves isn't empty
		cheng4::Board board, tip;
		std::vector< cheng4::Move > moves;
	};

	Ui::LiveInfo *ui;
	// players flipped flag
	bool flipped;
	// remaining time for each player (cs)
	double remTime[ cheng4::ctMax ];
	// thinking time for each player (cs)
	double thinkTime[ cheng4::ctMax ];
	// PV for each player
	PVInfo pv[ cheng4::ctMax ];
	// side to move (-1 = none)
	int turn;
	// time stamp
	qint64 stamp;
	ChessBoard *boards[ cheng4::ctMax ];
};

#endif // LIVEINFO_H
