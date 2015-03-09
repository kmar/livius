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

#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QtGui>
#include <QWidget>
#include "chess/chess.h"

namespace config
{
class ConfigVarBase;
}

class PieceSet;

class ChessBoard : public QWidget
{
    Q_OBJECT

    cheng4::Board board;

    // initialize internal structure
    void init();

    // render board + pieces
    void renderBoard( QPainter &p, const QRectF &rc, bool flip = 0,
        const std::vector<cheng4::Square> *excludeSquares = 0 );

public:
    explicit ChessBoard(QWidget *parent = 0);

	// set board
	void setBoard(const cheng4::Board &b);
	// set board (from fen)
	void setFEN(const QString &fen);
	// get FEN
	QString getFEN() const;
	// get turn (stm)
	cheng4::Color getTurn() const;
	// get board
	const cheng4::Board &getBoard() const;
	// set flipped
	void setFlipped( bool flip );
	// is flipped?
	bool isFlipped() const;
	// do move
	bool doMove( cheng4::Move move );
	// set move number
	void setMoveNumber( cheng4::uint num );
	// increment move number
	void incMoveNumber();
	// set highlight
	void setHighlight( cheng4::Move move = cheng4::mcNone );

    void setPieceSet( PieceSet *pset );

	// set border flag
    void setBorder( bool flag );
	// has border?
	bool hasBorder() const;

    // events
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *evt);
    void mousePressEvent( QMouseEvent *evt );
    void mouseReleaseEvent( QMouseEvent *evt );
    void mouseMoveEvent( QMouseEvent *evt );
    void wheelEvent( QWheelEvent *evt );

    void leftButtonDown(QMouseEvent *evt);
    void leftButtonUp(QMouseEvent *evt);
    void rightButtonDown(QMouseEvent *evt);
    void rightButtonUp(QMouseEvent *evt);

	// change these directly
	static QColor lightColor, darkColor, borderColor, highlightColor, backgroundColor, letterColor;
	// compute border color from light and dark (average)
	static void computeBoderColor();

	// add config vars for ChessBoard
	static bool addConfig( config::ConfigVarBase *parent );
signals:

public slots:

private:
	QSize size;
	PieceSet *pieceSet;
	bool border;
	bool flipped;
	cheng4::Move highlight;
};

#endif // CHESSBOARD_H
