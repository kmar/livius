#pragma once

#include <QtGui>
#include <QtSvg/QGraphicsSvgItem>

class PieceSet
{
public:
	QGraphicsSvgItem *pieces[2][6];	// [color][type-1]

    PieceSet();
    virtual ~PieceSet();

    // clean up
    void clear();

	// load: argument is directory to load from
	bool load( const QString &str );
};
