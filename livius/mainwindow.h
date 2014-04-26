#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "liveframe.h"

class QLabel;
class PieceSet;

namespace Ui {
class MainWindow;
}

namespace config
{
class ConfigVarBase;
}

class LiveFrame;
class ResultsDialog;
class EmailGameDialog;
class ConnectionDialog;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(const QString &appDir, QWidget *parent = 0);
	~MainWindow();

	// initiate new connection
	LiveFrame *newConnection();

	void setStatusText( const QString &str );

	LiveFrame *getLiveFrame() const;

	// convert relative filename to absolute (relative to application dir)
	QString appRelative( const QString &fnm ) const;

	// save configuration
	bool saveConfig() const;

private slots:
	void frameWindowChanged();

	void on_actionExit_triggered();

	void on_actionConnect_triggered();

	void on_actionBoard_light_squares_triggered();

	void on_actionBoard_dark_squares_triggered();

	void on_actionPiece_set_triggered();

	void on_actionBoard_border_triggered();

	void on_actionBoard_highlight_triggered();

	void on_actionResults_triggered();

	void on_actionE_mail_game_triggered();

	void on_actionBrown_triggered();

	void on_actionBlue_1_triggered();

	void on_actionBlue_2_triggered();

	void on_actionGreen_triggered();

	void on_actionTLCV_triggered();

	void on_actionFritz_blue_triggered();

	void on_actionShredder_triggered();

	void on_actionArena_triggered();

	void on_actionWinboard_triggered();

	void on_actionChess_Partner_triggered();

	void on_actionChess_King_triggered();

	void on_actionSave_config_triggered();

	// FIXME: this sucks... until I figure out how to fix that (capturing actions?)
	void onMenuAction1();
	void onMenuAction2();
	void onMenuAction3();
	void onMenuAction4();
	void onMenuAction5();
	void onMenuAction6();
	void onMenuAction7();
	void onMenuAction8();
	void onMenuAction9();
	void onMenuAction10();

	void on_actionPGN_triggered();

	void on_actionFonts_triggered();

	void on_actionFlipBoard_triggered();

	void on_actionAbout_triggered();

	void on_actionShowDebugConsole_triggered();

private:
	void setBoardColor( const QColor &light, const QColor &dark );
	void updateBoards();
	void initConfig();
	void onMenuChanged( LiveFrame *who, const LiveFrame::MenuMap &menu );
	void updateMenu( const LiveFrame::MenuMap *menu );
	void menuAction( int id );

	QString pieceSetFile;

	std::vector< QAction * > menuActions;

	Ui::MainWindow *ui;
	QLabel *status1, *status2;
	PieceSet *pset;
	ResultsDialog *rd;
	EmailGameDialog *ed;
	ConnectionDialog *cd;
	QString appDirectory;
	config::ConfigVarBase *cfgRoot;

	// fonts (TODO: use struct instead)
	QString fontFamily;
	int fontSize;
	int fontWeight;
	bool fontBold;
	bool fontItalic;
};

#endif // MAINWINDOW_H
