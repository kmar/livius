livius-bin:
	( cd base && qmake && make ) && ( cd gui && qmake && make ) && ( cd livius && qmake && make ) && cp livius/livius livius-bin
