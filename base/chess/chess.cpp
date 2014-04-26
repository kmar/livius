#include "chess.h"
#include "../core/timer.h"

ChessInit::ChessInit()
{
		cheng4::Tables::init();
		cheng4::BitOp::init();
		cheng4::Magic::init();
		cheng4::Zobrist::init();
		core::Timer::init();
}

ChessInit::~ChessInit()
{
	cheng4::Magic::done();
	core::Timer::done();
}
