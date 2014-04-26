#include "pgn.h"
#include <QFile>

using namespace std;

// PGNMove

PGNMove::PGNMove() : next(0)
{
}

PGNMove::~PGNMove()
{
    delete next;
}

// PGNNode
PGNNode::PGNNode() : parent(0)
{
}

// PGNGame

PGNGame::PGNGame() : root(0), result(prUndetermined)
{
}

PGNGame::~PGNGame()
{
    delete root;
}

// PGNLoadThread

PGNLoadThread::PGNLoadThread() : abortFlag(0), file(0)
{
}

void PGNLoadThread::abort()
{
    abortFlag = 1;
}

void PGNLoadThread::work()
{
    QFile lfile( file->fileName );
    char buf[16384];
    file->onLoadStarted();
    file->clear();
    qint64 pos = 0;
    qint64 fsize = lfile.size();
    if ( !lfile.open(QFile::ReadOnly) )
    {
        file->onLoadError();
        abortFlag = 1;
    }
    while (!abortFlag)
    {
        file->onLoadProgress( fsize ? (int)(pos*100 / fsize) : 100 );
        // index pgn
        // this will require parsing tokens actually!
        qint64 nr = lfile.read(buf, sizeof(buf));
        if ( nr <= 0 )
        {
            if ( nr < 0 )
                file->onLoadError();
            break;
        }
        // feed parser with buffer...
        if ( !file->parser.parseBuffer(buf, (size_t)nr) )
        {
            file->onParseError( (qint64)file->parser.getLine(), file->parser.getError() );
            break;
        }
        pos += nr;
    }
    file->parser.flushIndex((cheng4::u64)pos);
    file->onLoadComplete();
}

// PGNIndexParser

PGNIndexParser::PGNIndexParser( vector< PGNIndex > &indexRef ) : index(indexRef)
{
    reset();
}

void PGNIndexParser::reset()
{
    column = line = 1;
    state = stScan;
    lexState = lsScan;
    inTag = 0;
    ofs = 0;
    index.clear();
    error = "no error";
}

cheng4::u64 PGNIndexParser::getLine() const
{
    return line;
}

QString PGNIndexParser::getError() const
{
    return error;
}

void PGNIndexParser::flushIndex( cheng4::u64 offset )
{
    if ( index.empty() )
        return;
    PGNIndex &idx = index.back();
    if ( idx.size )
        return;             // already set
    idx.size = offset - idx.offset;
    if ( !idx.size )
        index.pop_back();
}

void PGNIndexParser::flushHeader( cheng4::u64 offset )
{
    if ( index.empty() )
        return;
    PGNIndex &idx = index.back();
    if ( idx.size )
        return;             // already set
    idx.hdrSize = offset - idx.offset;
}

bool PGNIndexParser::parseBuffer( const char *buf, size_t sz )
{
    Q_ASSERT(buf);
    const char *src = buf;
    const char *top = buf + sz;
    while ( buf < top )
    {
        switch( lexState )
        {
        case lsStringEscape:
            lexState = lsString;
            column++; buf++;
            continue;
        case lsString:
            if ( *buf == '"')
                lexState = lsScan;
            else if ( *buf == '\\' )
                lexState = lsStringEscape;
            column++; buf++;
            continue;
        case lsEol:
            if ( *buf == 10 )
                buf++;
            lexState = lsScan;
            continue;
        case lsCommentEol:
            if ( *buf == 10 )
                buf++;
            lexState = lsMultiComment;
            continue;
        case lsComment:
            if (*buf == 10 || *buf == 13)
            {
                // end of lsComment
                lexState = (*buf == 13) ? lsEol : lsScan;
                column = 1; line++;
            } else column++;
            buf++;
            continue;
        case lsMultiComment:
            if (*buf == 10 || *buf == 13)
            {
                // end of lsComment
                if ( *buf == 13 )
                    lexState = lsCommentEol;
                column = 1; line++;
            } else column++;
            if ( *buf == '}' )
                lexState = lsScan;
            buf++;
            continue;
        case lsScan:
            switch( *buf )
            {
            case '%':
                if ( column == 1 )
                {
                    lexState = lsComment; column++; buf++;
                    continue;
                }
                // unexpected
                error = "unexpected `%' character";
                return 0;
            case '[':
                inTag = 1;
                if ( state != stHeader )
                {
                    cheng4::u64 offset = ofs + (cheng4::u64)(buf - src);
                    flushIndex( offset );
                    // add index
                    PGNIndex idx;
                    idx.offset = offset;
                    idx.index = (cheng4::u32)index.size();
                    idx.size = 0;
                    idx.hdrSize = 0;
                    index.push_back( idx );
                    state = stHeader;
                }
                column++; buf++;
                continue;
            case ']':
                if ( !inTag )
                {
                    error = "unexpected `]'";
                    return 0;
                }
                column++; buf++;
                inTag = 0;
                continue;
            case '"':
                column++; buf++;
                lexState = lsString;
                continue;
            case ';':
                column++; buf++;
                lexState = lsComment;
                continue;
            case 13:
            case 10:
                // EOL
                lexState = (*buf == 13) ? lsEol : lsScan;
                column = 1; line++; buf++;
                continue;
            case '{':
                column++; buf++;
                lexState = lsMultiComment;
                continue;
            default:
                // moves and stuff OR spaces
                if ( *buf <= 32 )
                {
                    column++; buf++;
                    continue;       // ignore spaces/specials
                }
                // ok special stuff now
                if ( state == stHeader )
                {
                    if ( !inTag )
                    {
                        cheng4::u64 offset = ofs + (cheng4::u64)(buf - src);
                        flushHeader( offset );
                        state = stGame;
                    }
                }
                column++; buf++;
            }
        }
    }
    ofs += sz;
    return 1;
}

// PGNFile

PGNFile::PGNFile() : utf8(0), loaded(0), thread(0), parser(index)
{
}

PGNFile::~PGNFile()
{
    killThread();
}

void PGNFile::clear()
{
	core::MutexLock _(mutex);
    parser.reset();
}

void PGNFile::killThread()
{
	core::MutexLock _(mutex);
    if ( thread )
    {
        thread->abort();
        thread->kill();
        thread = 0;
    }
}

bool PGNFile::load( const QString &fname )
{
    killThread();
    QFile f(fname);
    if ( !f.exists() )
        return 0;       // no such file
    f.close();
    fileName = fname;
    thread = new PGNLoadThread;
    thread->file = this;
    thread->run();
    return 1;
}

bool PGNFile::reload()
{
    QString fnm = fileName;
    return load(fnm);
}

// wait for indexing to finish
void PGNFile::wait()
{
    {
		core::MutexLock _(mutex);
        if ( !thread )
           return;       // already finished
    }
    thread->wait();
}

size_t PGNFile::getCount() const
{
	core::MutexLock _(mutex);
    return index.size();
}

// get (sorted) index, returns true on success
bool PGNFile::getIndex( size_t idx, PGNIndex &data )
{
    if ( idx >= index.size())
        return 0;
    data = index[idx];
    return 1;
}

// raw low level function to load data
bool PGNFile::loadRawData( cheng4::u64 offset, size_t size, std::vector< cheng4::u8 > &data )
{
    data.clear();
    QFile f(fileName);
    if ( !f.open(QFile::ReadOnly) )
        return 0;
    if ( !f.seek( (qint64)offset ) )
        return 0;
    data.resize(size+1);
    if ( size && f.read( (char *)&data.front(), (qint64)size ) != (qint64)size )
        return 0;
    data[size] = 0;
    return 1;
}
