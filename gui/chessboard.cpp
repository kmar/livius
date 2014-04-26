#include "chessboard.h"
#include "pieceset.h"
#include "config/config.h"
#include <QSvgRenderer>

ChessBoard::ChessBoard(QWidget *parent) :
	QWidget(parent), pieceSet(0)
{
    init();
	setMinimumWidth( 128 );
	setMinimumHeight( 128 );
}

/*
 * Color themes:
 *
 * brown:
 *	0xd7, 0xac, 0x88
 *	0xaf, 0x69, 0x54
 *
 * blue:
 *	137, 188, 215
 *	80, 131, 175
 *
 * blue #2:
 *	240, 240, 240
 *	86, 147, 179
 *
 * green:
 *	136, 215, 141
 *	80, 175, 99
 *
 * TLCV:
 *	239,247,198
 *	99,173,74
 *
 * Fritz blue:
 *	237,237,237
 *	125,144,189
 *	62,72,95	(outline)
 *
 * Shredder:
 *	233,236,240
 *	181,189,206
 *
 * Arena:
 *	224, 224, 224
 *	119, 181, 225
 *
 * Winboard:
 *	200,195,101
 *	119,162,109
 *
 * Chess Partner:
 *	227,197,140
 *	149,95,34
 *
 * Chess King:
 *	255,255,250
 *	106,154,106
 */

QColor ChessBoard::lightColor(0xd7, 0xac, 0x88);
QColor ChessBoard::darkColor(0xaf, 0x69, 0x54);
QColor ChessBoard::borderColor(0xaf/2, 0x69/2, 0x54/2);
QColor ChessBoard::highlightColor(255, 0, 0);
QColor ChessBoard::backgroundColor(128, 128, 128);
QColor ChessBoard::letterColor(255, 255, 255);

void ChessBoard::init()
{
    border = 1;
    flipped = 0;
    board.reset();
	highlight = cheng4::mcNone;
}

// set flipped
void ChessBoard::setFlipped( bool flip )
{
	flipped = flip;
	update();
}

// is flipped?
bool ChessBoard::isFlipped() const
{
	return flipped;
}

// set board
void ChessBoard::setBoard(const cheng4::Board &b)
{
	board = b;
	update();
}

// set board (from fen)
void ChessBoard::setFEN(const QString &fen)
{
	if ( fen.isEmpty() )
		board.reset();
	else
	{
		QByteArray arr = fen.toLatin1();
		board.fromFEN( arr.constData() );
	}
	update();
}

// get FEN
QString ChessBoard::getFEN() const
{
	return QString(board.toFEN().c_str());
}

// get turn (stm)
cheng4::Color ChessBoard::getTurn() const
{
	return board.turn();
}

// get board
const cheng4::Board &ChessBoard::getBoard() const
{
	return board;
}

// set highlight
void ChessBoard::setHighlight( cheng4::Move move )
{
	if ( highlight == move )
		return;
	highlight = move;
}

// do move
bool ChessBoard::doMove( cheng4::Move move )
{
	if ( move == cheng4::mcNone )
		return 0;
	cheng4::UndoInfo ui;
	board.doMove( move, ui, board.isCheck(move, board.discovered()) );
	return 1;
}

// set move number
void ChessBoard::setMoveNumber( cheng4::uint num )
{
	board.setMove( num );
}

// increment move number
void ChessBoard::incMoveNumber()
{
	board.incMove();
}

void ChessBoard::renderBoard( QPainter &p, const QRectF &rc, bool flip,
    const std::vector<cheng4::Square> *excludeSquares )
{
    qreal maxh = std::max( rc.width(), rc.height() )/14;
    maxh = 18;
	qreal bordersz = maxh;
	if ( !border )
		bordersz = 0;
	qreal sw = (rc.width() - 2*bordersz) / 8;
	qreal sh = (rc.height() - 2*bordersz) / 8;
    sh = sw = std::min(sw, sh);

    QColor wb[2] = { QColor(255, 255, 255, 255), QColor(0, 0, 0, 255) };

	QColor bord = borderColor;

	QBrush lightb(lightColor);
	QBrush darkb(darkColor);
    p.setPen(Qt::transparent);
    // render border...
    p.setBrush( QBrush(bord) );
	p.drawRect(QRectF(rc.left(), rc.top(), 8*sw + 2*bordersz+1, 8*sh + 2*bordersz+1));

    QPen bpen( QColor(0, 0, 0, 128) );
    bpen.setWidth(1);
    p.setPen( bpen );
    p.setBrush( Qt::transparent );
    QRectF brect;
	brect.setTopLeft( QPoint(bordersz-1, bordersz-1) );
    brect.setSize( QSizeF(8*sw+2, 8*sh+2) );
    p.drawRect( brect );

    brect.setTopLeft( QPoint(0, 0) );
	brect.setSize( QSizeF(8*sw+2*bordersz, 8*sh+2*bordersz) );
    p.drawRect( brect );

    p.setPen( Qt::transparent );

    for (cheng4::Square s = 0; s < 64; s++)
    {
        cheng4::File f = cheng4::SquarePack::file(s);
        cheng4::Rank r = cheng4::SquarePack::rank(s);
		QRectF sub(rc.left() + sw*f + bordersz, rc.top() + sh*r + bordersz, sw, sh);
        cheng4::Square sq = flip ? cheng4::SquarePack::flipH(cheng4::SquarePack::flipV(s)) : s;


        p.setBrush( ((f+r) & 1) ? darkb : lightb );
        p.drawRect( sub );
        if ( excludeSquares && std::find(excludeSquares->begin(), excludeSquares->end(), sq)
             != excludeSquares->end() )
            continue;

        cheng4::Piece pc = board.piece(sq);
        cheng4::Piece pt = cheng4::PiecePack::type(pc);
        if ( pt == cheng4::ptNone )
            continue;
        cheng4::Color c = cheng4::PiecePack::color(pc);
		if ( !pieceSet || !pieceSet->pieces[c][pt-1] )
			continue;
        pieceSet->pieces[c][pt-1]->renderer()->render(&p, sub);
	}

	// FIXME: better!!!
	for (cheng4::Square s = 0; s < 64; s++)
	{
		cheng4::File f = cheng4::SquarePack::file(s);
		cheng4::Rank r = cheng4::SquarePack::rank(s);
		QRectF sub(rc.left() + sw*f + bordersz, rc.top() + sh*r + bordersz, sw, sh);
		cheng4::Square sq = flip ? cheng4::SquarePack::flipH(cheng4::SquarePack::flipV(s)) : s;
		// now highlight!!!
		if ( highlight == cheng4::mcNone || highlight == cheng4::mcNull )
			continue;
		cheng4::Square from, to;
		from = cheng4::MovePack::from(highlight);
		to = cheng4::MovePack::to(highlight);
		if ( sq != from && sq != to )
			continue;

		QPen hpen( highlightColor );
		hpen.setWidth(2);
		p.setPen( hpen );
		p.setBrush( Qt::transparent );
		p.drawRect( sub );
		p.setPen( Qt::transparent );
	}

	if ( !border )
		return;

    // draw board letters
	p.setPen( letterColor );
    QTextOption opt;
    opt.setAlignment(Qt::AlignCenter);
    QFont font;
    font.setBold(0);
    font.setPixelSize(12);
    p.setFont(font);
    for (uint i=0; i<8; i++)
    {
        // rows:
		QPointF pt( 0, bordersz + sh*i );
        QRectF rct;
        rct.setTopLeft( pt );
		rct.setSize( QSizeF( bordersz, sh ) );
        QString text;
        int irow = 7-(int)i;
        if ( flip )
            irow = 8-1-irow;
        text.sprintf("%d", irow+1);
        p.drawText(rct, text, opt);
		pt.setX( bordersz+sw*8 );
        rct.setTopLeft( pt );
		rct.setSize( QSizeF( bordersz, sh ) );
        p.drawText(rct, text, opt);
        // cols:
        int icol = (int)i;
        if ( flip )
            icol = 8-1-i;
        text.sprintf("%c", 'a' + icol);
		pt.setX( bordersz + sw*i );
        pt.setY( 0 );
        rct.setTopLeft(pt);
		rct.setSize( QSizeF( sw, bordersz ) );
        p.drawText(rct, text, opt);
		pt.setY( bordersz+sh*8 );
        rct.setTopLeft( pt );
		rct.setSize( QSizeF( sw, bordersz ) );
        p.drawText(rct, text, opt);
    }
    // draw stm
    QRectF upr;
	upr.setTopLeft( QPointF( bordersz + sw*8 + bordersz/8, bordersz/8 ) );
	upr.setSize( QSizeF( bordersz - 2*bordersz/8, bordersz - 2*bordersz/8 ) );

    bool upper = board.turn() == (flip ? cheng4::ctWhite : cheng4::ctBlack);

    p.setPen( wb[ cheng4::flip(board.turn()) ] );
    p.setBrush( wb[ board.turn() ] );

    if ( upper )
        p.drawRect( upr );

    QRectF lwr;
	lwr.setTopLeft( QPointF( bordersz + sw*8 + bordersz/8, bordersz + sh*8 + bordersz/8 ) );
	lwr.setSize( QSizeF( bordersz - 2*bordersz/8, bordersz - 2*bordersz/8 ) );

    if ( !upper )
        p.drawRect( lwr );
}

void ChessBoard::paintEvent(QPaintEvent * /*e*/)
{
    QImage img( size, QImage::Format_RGB32 );
    QPainter p( &img );
    p.setRenderHint(QPainter::Antialiasing, 1);

	p.setPen(QColor(128, 128, 128));
	p.setBrush(QBrush( QColor(128, 128, 128) ));
    QRect full(0, 0, size.width(), size.height());
	p.drawRect( full );

    // render p
    // render board
//    QRectF rc(0, 0, 128, 128);
//    pieceSet->pieces[0][0]->renderer()->render(&p, rc);

    renderBoard( p, QRectF(full), flipped );

    QPainter p2( this );
    p2.drawImage( 0, 0, img);
}

void ChessBoard::resizeEvent(QResizeEvent *evt)
{
    size = evt->size();
    QWidget::resizeEvent(evt);
}

void ChessBoard::mousePressEvent(QMouseEvent *evt)
{
    if ( evt->button() == Qt::LeftButton )
        leftButtonDown( evt );
    else if ( evt->button() == Qt::RightButton )
        rightButtonDown( evt );
    QWidget::mousePressEvent( evt );
}

void ChessBoard::mouseReleaseEvent(QMouseEvent *evt)
{
    if ( evt->button() == Qt::LeftButton )
        leftButtonUp( evt );
    else if ( evt->button() == Qt::RightButton )
        rightButtonUp( evt );
    QWidget::mouseReleaseEvent( evt );
}

void ChessBoard::leftButtonDown(QMouseEvent *evt)
{
    (void)evt;
}

void ChessBoard::leftButtonUp(QMouseEvent *evt)
{
    (void)evt;
}

void ChessBoard::rightButtonDown(QMouseEvent *evt)
{
    (void)evt;
}

void ChessBoard::rightButtonUp(QMouseEvent *evt)
{
    (void)evt;
}

void ChessBoard::mouseMoveEvent( QMouseEvent *evt )
{
    QWidget::mouseMoveEvent( evt );
}

void ChessBoard::wheelEvent( QWheelEvent *evt )
{
    QWidget::wheelEvent( evt );
}

void ChessBoard::setPieceSet( PieceSet *pset )
{
    pieceSet = pset;
}

void ChessBoard::setBorder( bool flag )
{
    border = flag;
	update();
}

bool ChessBoard::hasBorder() const
{
    return border;
}

// compute border color from light and dark (average)
void ChessBoard::computeBoderColor()
{
	borderColor = QColor( darkColor.red()/2,
		darkColor.green()/2,
		darkColor.blue()/2);
/*	borderColor = QColor( (lightColor.red() + darkColor.red())/2,
				(lightColor.green() + darkColor.green())/2,
				(lightColor.blue() + darkColor.blue())/2 );*/
}

bool ChessBoard::addConfig( config::ConfigVarBase *parent )
{
	if ( !parent )
		return 0;
	config::CVarGroup *group = new config::CVarGroup("Board colors");
	group->addChild( new config::CVarQColor("Light squares",	&lightColor,		config::CF_EDIT) );
	group->addChild( new config::CVarQColor("Dark squares",		&darkColor,			config::CF_EDIT) );
	group->addChild( new config::CVarQColor("Border",			&borderColor,		config::CF_EDIT) );
	group->addChild( new config::CVarQColor("Background",		&backgroundColor,	config::CF_EDIT) );
	group->addChild( new config::CVarQColor("Highlight",		&highlightColor,	config::CF_EDIT) );
	group->addChild( new config::CVarQColor("Letters",			&letterColor,		config::CF_EDIT) );
	return parent->addChild( group );
}
