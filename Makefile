all:
	( cd base && qmake && make ) && ( cd gui && qmake && make ) && ( cd livius && qmake && make ) && /bin/rm -rf build && mkdir -p build && cp livius/livius build && cp -R livius/data build && echo && echo "Build successful (the binary is located in the 'build' directory)"
