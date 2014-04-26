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

#include "pieceset.h"

PieceSet::PieceSet()
{
    memset(pieces, 0, sizeof(pieces));
}

PieceSet::~PieceSet()
{
    clear();
}

void PieceSet::clear()
{
    for (int i=0; i<2; i++)
        for (int j=0; j<6; j++)
        {
            delete pieces[i][j];
            pieces[i][j] = 0;
        }
}

static const char *pieceFiles[2][6] =
{
    {
        ("wp.svg"),
        ("wn.svg"),
        ("wb.svg"),
        ("wr.svg"),
        ("wq.svg"),
        ("wk.svg")
    },
    {
        ("bp.svg"),
        ("bn.svg"),
        ("bb.svg"),
        ("br.svg"),
        ("bq.svg"),
        ("bk.svg")
    }
};

// load: argument is directory to load from
bool PieceSet::load( const QString &str )
{
    bool success = 1;
    clear();
    for (int i=0; i<2; i++)
    {
        for (int j=0; j<6; j++)
        {
            QString fname = str + QString(pieceFiles[i][j]);
            QFile file(fname);
            if ( !file.open(QFile::ReadOnly) )
            {
                success = 0;
                continue;
            }
            pieces[i][j] = new QGraphicsSvgItem(fname);
			pieces[i][j]->setFlags(QGraphicsItem::ItemClipsToShape);
			pieces[i][j]->setCacheMode(QGraphicsItem::NoCache);
        }
    }
    return success;
}
