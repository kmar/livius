#include "types.h"

namespace core
{

struct Timer
{
	static void init();
	static void done();
	// get millisecond counter
	static i32 getMillisec();
};

}
