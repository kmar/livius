#include <QDateTime>
#include "liveinfo.h"
#include "ui_liveinfo.h"
// FIXME: better (because of skipSpc)
#include "tlcvclient.h"
#include "chessboard.h"

LiveInfo::LiveInfo(QWidget *parent, PieceSet *pset) :
	QWidget(parent),
	ui(new Ui::LiveInfo),
	flipped(0),
	turn(-1)
{
	ui->setupUi(this);
	ui->boardWidget->hide();
	ui->boardWidget2->hide();
	boards[cheng4::ctWhite] = new ChessBoard( ui->boardWidget );
	boards[cheng4::ctBlack] = new ChessBoard( ui->boardWidget2 );
	boards[cheng4::ctWhite]->setPieceSet(pset);
	boards[cheng4::ctBlack]->setPieceSet(pset);
	boards[cheng4::ctWhite]->setBorder(0);
	boards[cheng4::ctBlack]->setBorder(0);
	connect(ui->fenButton, SIGNAL(clicked()), this, SLOT(copyFEN()));
	remTime[ cheng4::ctWhite ] = remTime[ cheng4::ctBlack ] = 0;
	thinkTime[ cheng4::ctWhite ] = thinkTime[ cheng4::ctBlack ] = 0;
}

LiveInfo::~LiveInfo()
{
	delete ui;
}

void LiveInfo::copyFEN()
{
	sigCopyFEN();
}

// set fen string
void LiveInfo::setFEN( const QString &fen )
{
	ui->fenEdit->setText( fen );
	ui->fenEdit->setCursorPosition(0);
}

// get fen string
QString LiveInfo::getFEN() const
{
	return ui->fenEdit->text();
}

// set player name
void LiveInfo::setPlayer( int color, const QString &txt )
{
	(color == cheng4::ctWhite ? ui->engineLabel : ui->engineLabel_2)->setText( txt );
	// reset engine stats
	setDepth( color, 0 );
	setScore( color, 0 );
	setNodes( color, 0, 0 );
	setPV( color, QString() );
}

// get player name
QString LiveInfo::getPlayer( int color ) const
{
	return (color == cheng4::ctWhite ? ui->engineLabel : ui->engineLabel_2)->text();
}

// set depth
void LiveInfo::setDepth( int color, int depth )
{
	QString str;
	str.sprintf("%d", depth);
	(color == cheng4::ctWhite ? ui->depthLabel : ui->depthLabel2)->setText( str );
}

// set score
void LiveInfo::setScore( int color, int score )
{
	QString str;
	str.sprintf("%+0.2lf", (double)score/100.0);
	(color == cheng4::ctWhite ? ui->scoreLabel : ui->scoreLabel2)->setText( str );
}

// set level/moves
void LiveInfo::setLevelMoves( const QString &txt )
{
	ui->movesLabel->setText(txt);
}

// set level/time
void LiveInfo::setLevelTime( const QString &txt )
{
	ui->timeLabel->setText(txt);
}

// set level/increment
void LiveInfo::setLevelIncrement( const QString &txt )
{
	ui->incrementLabel->setText(txt);
}

// set last move text
void LiveInfo::setLastMove( const QString &txt )
{
	ui->lastLabel->setText( txt );
}

// set fifty rule counter
void LiveInfo::setFiftyRule( const QString &fifty )
{
	ui->fiftyLabel->setText( fifty );
}

static QString prettyFormat( double num )
{
	QString str;
	str.sprintf("%0.3lf M", num / 1000000.0);
	return str;
}

// set nodes
void LiveInfo::setNodes( int color, double nodes, double nps )
{
	QString str;
	str = prettyFormat(nodes);
	(color == cheng4::ctWhite ? ui->nodesLabel : ui->nodesLabel2)->setText( str );
	str = prettyFormat(nps);
	(color == cheng4::ctWhite ? ui->npsLabel : ui->npsLabel2)->setText( str );
}

// set PV
void LiveInfo::setPV( int color, QString txt, bool pretty, bool pvtip, const cheng4::Board *board )
{
	QWidget *bw = color == cheng4::ctWhite ? ui->boardWidget : ui->boardWidget2;
	if ( !pretty )
		pvtip = 0;
	if ( bw->isVisible() != pvtip )
	{
		if ( pvtip )
			bw->show();
		else
			bw->hide();
	}
	// try to make PV more readable by converting it to SAN
	if ( pretty && board )
	{
		pv[color].board = *board;
		pv[color].moves.clear();
		QString prettyPV;
		cheng4::Board tb = *board;
		QByteArray arr = txt.toUtf8();
		const char *c = arr.constData();
		const char *merge = c;
		bool first = 1;
		bool foundMove = 0;
		while ( *c )
		{
			if ( (*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') )
			{
				// SAN!
				cheng4::Move m = tb.fromSAN(c);
				if ( m == cheng4::mcNone )
					break;
				merge = c;
				foundMove = 1;
				pv[color].moves.push_back(m);
				bool ponder = first && color != tb.turn();
				first = 0;
				if ( ponder )
					prettyPV += '(';
				prettyPV += tb.toSAN(m).c_str();
				if ( ponder )
					prettyPV += ')';
				prettyPV += ' ';
				cheng4::UndoInfo ui;
				tb.doMove( m, ui, tb.isCheck(m, tb.discovered()) );
			} else c++;
		}
		pv[color].tip = tb;
		if ( foundMove )
		{
			// FIXME: HACK: only set if move is found, but I can't do anything about it unfortunately
			boards[color]->setBoard( tb );

			TLCVClient::skipSpc(merge);
			txt = prettyPV;
			txt += merge;
		}
	}
	else pv[color].moves.clear();

	QLineEdit *ed = (color == cheng4::ctWhite ? ui->pvEdit : ui->pvEdit_2);
	ed->setText( txt );
	ed->setCursorPosition(0);
}

void LiveInfo::setTime()
{
	// just fill controls
	for ( cheng4::Color c = cheng4::ctWhite; c <= cheng4::ctBlack; c++ )
	{
		qint64 rt = (qint64)remTime[c];
		if ( rt < 0 )
			rt = 0;
		rt /= 100;
		int s = (int)(rt % 60);
		int m = (int)((rt/60) % 60);
		int h = (int)((rt/3600));
		QString str;
		str.sprintf("%02d:%02d:%02d", h, m, s);
		(c == cheng4::ctWhite ? ui->remainingLabel : ui->remainingLabel_2)->setText(str);
		qint64 pt = (qint64)thinkTime[c];
		pt /= 100;
		s = (int)(pt % 60);
		m = (int)((pt/60) % 60);
		str.sprintf("%02d:%02d", m, s);
		(c == cheng4::ctWhite ? ui->actualLabel : ui->actualLabel_2)->setText(str);
	}
}

// set time
void LiveInfo::setTime( int color, double time, double otim )
{
	remTime[ color ] = time;
	remTime[ color ^ 1 ] = otim;
	setTime();
}

// set turn
void LiveInfo::setTurn( int color )
{
	if ( turn != color )
	{
		turn = color;
		stamp = QDateTime::currentMSecsSinceEpoch();
		if ( turn >= 0 )
			thinkTime[ turn ] = 0;
	}
}

// refresh (to update times)
void LiveInfo::refresh()
{
	if ( turn < 0 )
		return;
	qint64 ms = QDateTime::currentMSecsSinceEpoch();
	qint64 delta = ms - stamp;
	double deltacs = (double)delta/10;
	remTime[ turn ] -= deltacs;
	thinkTime[ turn ] += deltacs;
	stamp = ms;
	setTime();
}

// visually flip players
void LiveInfo::flipPlayers()
{
	flipped = !flipped;
	ui->playersLayout->setDirection( flipped ? QBoxLayout::BottomToTop : QBoxLayout::TopToBottom );
	for ( cheng4::Color c = cheng4::ctWhite; c <= cheng4::ctBlack; c++ )
		boards[ c ]->setFlipped( !boards[ c ]->isFlipped() );
}
