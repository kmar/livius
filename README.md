livius
======

livius - a Qt-based TLCV-compatible live chess viewer

build instructions
------------------

$ make

Requires Qt 5, in particular:
- qmake
- qtdeclarative5-dev
- qtdeclarative5-dev-tools
- libqt5svg5-dev

OSX:
- install homebrew: https://treehouse.github.io/installation-guides/mac/homebrew
- (in the terminal):
- brew install qt5
- brew link qt (or brew link qmake, don't remember)
- make

note that the first time you run livius.app, the pieces will be missing

go to Appearance/Piece set and navigate to data/pieces2d/mine (may fix this later)

contributors
------------
Philipp Classen:
    Qt 5 fixes and a nice Makefile

Felix Braun:
    IPv6 fix

Rasmus Althoff:
    Window layout saved in config

Jost Triller:
    New chat window layout
