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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QMdiSubWindow>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QFontDialog>
#include "liveinfo.h"
#include "liveframe.h"
#include "pieceset.h"
#include "connectiondialog.h"
#include "chessboard.h"
#include "resultsdialog.h"
#include "emailgamedialog.h"
#include "pgndialog.h"
#include "aboutdialog.h"
#include "debugconsoledialog.h"
#include "config/config.h"

const int defWidth  = 800;
const int defHeight = 600;

void MainWindow::initConfig()
{
	cfgRoot = new config::CVarGroup("");

	pieceSetFile = appRelative("./data/pieces2d/mine/");

	cd = new ConnectionDialog(this);
	cd->addConfig( cfgRoot );

	config::CVarGroup *group = new config::CVarGroup("Fonts");
	group->addChild( new config::CVarQString("Family", &fontFamily, config::CF_EDIT ) );
	group->addChild( new config::CVarInt("Size", &fontSize, config::CF_EDIT ) );
	group->addChild( new config::CVarInt("Weight", &fontWeight, config::CF_EDIT ) );
	group->addChild( new config::CVarBool("Bold", &fontBold, config::CF_EDIT ) );
	group->addChild( new config::CVarBool("Italic", &fontItalic, config::CF_EDIT ) );
	cfgRoot->addChild( group );
	
	config::CVarGroup *wgroup = new config::CVarGroup("Main Window");
	wgroup->addChild( new config::CVarInt("Width", &mainWidth, config::CF_EDIT ) );
	wgroup->addChild( new config::CVarInt("Height", &mainHeight, config::CF_EDIT ) );
	wgroup->addChild( new config::CVarBool("Maximized", &maxWindow, config::CF_EDIT ) );
	cfgRoot->addChild( wgroup );

	// add all configs here
	LiveFrame::addConfig( cfgRoot );
	ChessBoard::addConfig( cfgRoot );

	cfgRoot->addChild( new config::CVarQString("Piece set", &pieceSetFile, config::CF_EDIT ) );

	// load config now
	QString error;
	QString fnm = appRelative("./livius.cfg");
	bool res = config::ConfigSerialize::loadText( fnm, cfgRoot, &error );
	if ( !res )
		QMessageBox::critical(0, "Error loading config", error );

	cd->updateConfig();
	
	if ( mainWidth < defWidth )
		mainWidth = defWidth;
	if ( mainHeight < defHeight )
		mainHeight = defHeight;
	resize( QSize(mainWidth, mainHeight) );

	if ( fontSize > 0 && !fontFamily.isEmpty() )
	{
		QFont font( fontFamily, fontSize, fontWeight, fontItalic );
		font.setBold( fontBold );
		QApplication::setFont(font);
	}
}

bool MainWindow::saveConfig()
{
	// save config
	QString error;
	QString fnm = appRelative("./livius.cfg");
	maxWindow = isMaximized();
	if ( !maxWindow )
	{
		mainWidth  = width();
		mainHeight = height();
	}
	LiveFrame *lf = getLiveFrame();
	if ( lf )
		lf->updateConfig();
	bool res = config::ConfigSerialize::saveText( fnm, cfgRoot, &error );
	if ( !res )
		QMessageBox::critical(0, "Error saving config", error );
	return res;
}

MainWindow::MainWindow(const QString &appDir, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	rd(0), ed(0), cd(0), appDirectory(appDir),
	cfgRoot(0), fontSize(-1), fontWeight(-1), fontBold(0), fontItalic(0),
	maxWindow(1), mainWidth(defWidth), mainHeight(defHeight)
{
	QLocale::setDefault(QLocale::C);
	ui->setupUi(this);
	ui->mdiArea->setViewMode(QMdiArea::TabbedView);
	setCentralWidget(ui->mdiArea);

	connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
			this, SLOT(frameWindowChanged()));

	status1 = new QLabel( this );
	status2 = new QLabel( this );

	statusBar()->addWidget( status1 );
	statusBar()->addPermanentWidget( status2 );

	initConfig();

	pset = new PieceSet;
	if ( !pset->load( pieceSetFile ) )
		pset->load( appRelative( pieceSetFile ) );

	ui->mainToolBar->hide();
	statusBar()->hide();

	setMinimumHeight( defHeight );
	setMinimumWidth( defWidth );

	frameWindowChanged();
}

void MainWindow::updateMenu( const LiveFrame::MenuMap *menu )
{
	// TODO: implement...
	foreach( QAction *a, menuActions )
	{
		ui->menuTournament->removeAction(a);
	}
	menuActions.clear();
	if ( !menu )
		return;
	LiveFrame::MenuMap::const_iterator ci;
	int i = 0;
	for ( ci = menu->begin(); ci != menu->end(); ci++ )
	{
		const LiveFrame::MenuItem &mi = ci->second;
		QAction *a = ui->menuTournament->addAction(mi.name);
		// FIXME: this sucks!
		switch( i )
		{
		case 0:
			connect(a, SIGNAL(triggered()), this, SLOT(onMenuAction1()));
			break;
		case 1:
			connect(a, SIGNAL(triggered()), this, SLOT(onMenuAction2()));
			break;
		case 2:
			connect(a, SIGNAL(triggered()), this, SLOT(onMenuAction3()));
			break;
		case 3:
			connect(a, SIGNAL(triggered()), this, SLOT(onMenuAction4()));
			break;
		case 4:
			connect(a, SIGNAL(triggered()), this, SLOT(onMenuAction5()));
			break;
		case 5:
			connect(a, SIGNAL(triggered()), this, SLOT(onMenuAction6()));
			break;
		case 6:
			connect(a, SIGNAL(triggered()), this, SLOT(onMenuAction7()));
			break;
		case 7:
			connect(a, SIGNAL(triggered()), this, SLOT(onMenuAction8()));
			break;
		case 8:
			connect(a, SIGNAL(triggered()), this, SLOT(onMenuAction9()));
			break;
		case 9:
			connect(a, SIGNAL(triggered()), this, SLOT(onMenuAction10()));
			break;
		}
		menuActions.push_back(a);
		if ( ++i >= 10 )
			break;			// no more allowed
	}
}

void MainWindow::menuAction(int id)
{
	LiveFrame *lf = getLiveFrame();
	if ( !lf )
		return;
	const LiveFrame::MenuMap &menu = lf->getMenu();
	LiveFrame::MenuMap::const_iterator ci;
	int i=0;
	for ( ci = menu.begin(); ci != menu.end(); ci++ )
	{
		if ( i == id )
		{
			const LiveFrame::MenuItem &mi = ci->second;
			QDesktopServices::openUrl( QUrl(mi.url) );
			break;
		}
		i++;
	}
}

// FIXME: this sucks!
void MainWindow::onMenuAction1()
{
	menuAction(0);
}

void MainWindow::onMenuAction2()
{
	menuAction(1);
}

void MainWindow::onMenuAction3()
{
	menuAction(2);
}

void MainWindow::onMenuAction4()
{
	menuAction(3);
}

void MainWindow::onMenuAction5()
{
	menuAction(4);
}

void MainWindow::onMenuAction6()
{
	menuAction(5);
}

void MainWindow::onMenuAction7()
{
	menuAction(6);
}

void MainWindow::onMenuAction8()
{
	menuAction(7);
}

void MainWindow::onMenuAction9()
{
	menuAction(8);
}

void MainWindow::onMenuAction10()
{
	menuAction(9);
}

// convert relative filename to absolute (relative to application dir)
QString MainWindow::appRelative( const QString &fnm ) const
{
	return appDirectory.isEmpty() ? fnm : appDirectory + '/' + fnm;
}

// note: may return 0
LiveFrame *MainWindow::getLiveFrame() const
{
	QMdiSubWindow *sw = ui->mdiArea->currentSubWindow();
	if ( !sw )
		return 0;
	LiveFrame *lf = qobject_cast<LiveFrame *>(sw->widget());
	return lf;
}

void MainWindow::frameWindowChanged()
{
	LiveFrame *lf = getLiveFrame();
	// update menu now!
	ui->actionResults->setEnabled( lf != 0 );
	ui->actionE_mail_game->setEnabled( lf != 0 );
	ui->actionPGN->setEnabled( lf != 0 );
	ui->actionFlipBoard->setEnabled( lf != 0 );
	ui->actionShowDebugConsole->setEnabled( lf != 0 );
	updateMenu( lf ? &lf->getMenu() : 0 );
}

MainWindow::~MainWindow()
{
	delete ui;
	delete pset;
	delete cfgRoot;
}

void MainWindow::setStatusText( const QString &str )
{
	status2->setText( str );
}

bool MainWindow::isMaxWindow() const
{
	return maxWindow;
}

void MainWindow::onMenuChanged( LiveFrame *who, const LiveFrame::MenuMap &menu )
{
	if ( getLiveFrame() != who )
		return;			// not interested in this
	updateMenu( &menu );
}

// initiate new connection
LiveFrame *MainWindow::newConnection()
{
	LiveFrame *res = 0;
	Q_ASSERT( cd != 0 );
	if ( cd->exec() == QDialog::Accepted )
	{
		if ( cd->getPort() && !cd->getURL().isEmpty() )
		{
			LiveFrame *child = new LiveFrame(this, pset, cd->getNick(), cd->getURL(), cd->getPort(), cd->getLayoutType() );
			child->sigSetStatus.connect( this, &MainWindow::setStatusText );
			child->sigMenuChanged.connect( this, &MainWindow::onMenuChanged );
			ui->mdiArea->addSubWindow(child);
			res = child;
			child->show();
		}
	}
	return res;
}

void MainWindow::on_actionExit_triggered()
{
	close();
}

void MainWindow::on_actionConnect_triggered()
{
	newConnection();
}

void MainWindow::on_actionBoard_light_squares_triggered()
{
	QColorDialog dlg( this );
	dlg.setCurrentColor( ChessBoard::lightColor );
	if ( dlg.exec() == QDialog::Accepted )
	{
		ChessBoard::lightColor = dlg.currentColor();
		ChessBoard::computeBoderColor();
		updateBoards();
	}
}

void MainWindow::on_actionBoard_dark_squares_triggered()
{
	QColorDialog dlg( this );
	dlg.setCurrentColor( ChessBoard::darkColor );
	if ( dlg.exec() == QDialog::Accepted )
	{
		ChessBoard::darkColor = dlg.currentColor();
		ChessBoard::computeBoderColor();
		updateBoards();
	}
}

void MainWindow::on_actionPiece_set_triggered()
{
	QFileDialog fd( this );
	fd.setFileMode(QFileDialog::Directory);
	fd.setOption(QFileDialog::ShowDirsOnly);
	if ( fd.exec() == QDialog::Accepted )
	{
		QDir dir = fd.directory();
		QString path = dir.absolutePath();
		if ( !path.isEmpty() && !path.endsWith('/')  && !path.endsWith('\\') )
			path += '/';
		if ( pset->load(path) )
		{
			pieceSetFile = path;
		}
		else
		{
			// revert to old
			if ( !pset->load( pieceSetFile ) )
				pset->load( appRelative( pieceSetFile ) );
			QMessageBox::critical(this, "Error!", "Failed to load piece set");
		}
		updateBoards();
	}
}

void MainWindow::on_actionBoard_border_triggered()
{
	QColorDialog dlg( this );
	dlg.setCurrentColor( ChessBoard::borderColor );
	if ( dlg.exec() == QDialog::Accepted )
	{
		ChessBoard::borderColor = dlg.currentColor();
		updateBoards();
	}
}

void MainWindow::on_actionBoard_highlight_triggered()
{
	QColorDialog dlg( this );
	dlg.setCurrentColor( ChessBoard::highlightColor );
	if ( dlg.exec() == QDialog::Accepted )
	{
		ChessBoard::highlightColor = dlg.currentColor();
		updateBoards();
	}
}

void MainWindow::on_actionResults_triggered()
{
	LiveFrame *lf = getLiveFrame();
	Q_ASSERT( lf != 0 );
	if ( !lf )
		return;
	if ( !rd )
		rd = new ResultsDialog(this);
	rd->connection.disconnect();
	rd->connection2.disconnect();
	rd->connection = lf->sigCTAdd.connect( rd, &ResultsDialog::addCrossTable );
	rd->connection2 = lf->sigCTClear.connect( rd, &ResultsDialog::clearCrossTable );
	lf->getCrossTable();
	rd->exec();
	rd->connection.disconnect();
	rd->connection2.disconnect();
}

void MainWindow::on_actionE_mail_game_triggered()
{
	LiveFrame *lf = getLiveFrame();
	Q_ASSERT( lf );
	if ( !lf )
		return;
	if ( !ed )
	{
		ed = new EmailGameDialog(this);
		ed->setEmail( cd->getEmail() );
	}
	ed->connection.disconnect();
	ed->clearGames();
	ed->connection = lf->sigGLAdd.connect( ed, &EmailGameDialog::addGameList );
	ed->sigSendGames.disconnectAll();
	ed->sigSendGames.connect( lf, &LiveFrame::sendGames );

	lf->getGameList();
	// we need this to be modal!
	ed->exec();
	cd->setEmail( ed->getEmail() );
	ed->connection.disconnect();
	ed->sigSendGames.disconnectAll();
}

void MainWindow::updateBoards()
{
	// FIXME: this sucks, there must be a way invalidate everything...
	LiveFrame *lf = getLiveFrame();
	if ( lf )
		lf->update();
}

void MainWindow::setBoardColor( const QColor &light, const QColor &dark )
{
	ChessBoard::lightColor = light;
	ChessBoard::darkColor = dark;
	ChessBoard::computeBoderColor();
	updateBoards();
}

void MainWindow::on_actionBrown_triggered()
{
	setBoardColor( QColor(0xd7, 0xac, 0x88), QColor(0xaf, 0x69, 0x54) );
}

void MainWindow::on_actionBlue_1_triggered()
{
	setBoardColor( QColor(137, 188, 215), QColor(80, 131, 175) );
}

void MainWindow::on_actionBlue_2_triggered()
{
	setBoardColor( QColor(240, 240, 240), QColor(86, 147, 179) );
}

void MainWindow::on_actionGreen_triggered()
{
	setBoardColor( QColor(136, 215, 141), QColor(80, 175, 99) );
}

void MainWindow::on_actionTLCV_triggered()
{
	setBoardColor( QColor(239,247,198), QColor(99,173,74) );
}

void MainWindow::on_actionFritz_blue_triggered()
{
	setBoardColor( QColor(237,237,237), QColor(125,144,189) );
}

void MainWindow::on_actionShredder_triggered()
{
	setBoardColor( QColor(233,236,240), QColor(181,189,206) );
}

void MainWindow::on_actionArena_triggered()
{
	setBoardColor( QColor(224, 224, 224), QColor(119, 181, 225) );
}

void MainWindow::on_actionWinboard_triggered()
{
	setBoardColor( QColor(200,195,101), QColor(119,162,109) );
}

void MainWindow::on_actionChess_Partner_triggered()
{
	setBoardColor( QColor(227,197,140), QColor(149,95,34) );
}

void MainWindow::on_actionChess_King_triggered()
{
	setBoardColor( QColor(255,255,250), QColor(106,154,106) );
}

void MainWindow::on_actionSave_config_triggered()
{
	saveConfig();
}

void MainWindow::on_actionPGN_triggered()
{
	LiveFrame *lf = getLiveFrame();
	Q_ASSERT( lf );
	if ( !lf )
		return;		// better safe than sorry
	PGNDialog pd( this );
	pd.setPGN( lf->getPGN() );
	sig::Connection conn = lf->sigPGNChanged.connect( &pd, &PGNDialog::setPGN );
	pd.exec();
	conn.disconnect();
}

void MainWindow::on_actionFonts_triggered()
{
	bool ok = 0;
	QFont font = QFontDialog::getFont( &ok, QApplication::font(), this );
	if ( ok )
	{
		fontFamily = font.family();
		fontSize = font.pointSize();
		fontWeight = font.weight();
		fontItalic = font.italic();
		fontBold = font.bold();
		QApplication::setFont( font );
	}
}

void MainWindow::on_actionFlipBoard_triggered()
{
	LiveFrame *lf = getLiveFrame();
	Q_ASSERT( lf );
	if ( !lf )
		return;		// better safe than sorry
	lf->flipBoard();
}

void MainWindow::on_actionAbout_triggered()
{
	AboutDialog dlg( this );
	dlg.exec();
}

void MainWindow::on_actionShowDebugConsole_triggered()
{
	LiveFrame *lf = getLiveFrame();
	Q_ASSERT( lf );
	if ( !lf )
		return;		// better safe than sorry
	DebugConsoleDialog dlg( this );
	dlg.setClient( lf->getClient() );
	dlg.exec();
}
