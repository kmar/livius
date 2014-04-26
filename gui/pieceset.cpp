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
