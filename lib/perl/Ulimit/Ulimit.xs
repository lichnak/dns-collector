#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include <sys/resource.h>
#include <unistd.h>


MODULE = Sherlock::Ulimit		PACKAGE = Sherlock::Ulimit		

PROTOTYPES: ENABLED

int
setlimit(IN resource, IN soft, IN hard)
	int resource
	int soft
	int hard
CODE:
	struct rlimit rl;
	int r;

	switch(resource) {
	case 0:
		r = RLIMIT_CPU; break;
	case 1:
		r = RLIMIT_FSIZE; break;
	case 2:
		r = RLIMIT_DATA; break;
	case 3:
		r = RLIMIT_STACK; break;
	case 4:
		r = RLIMIT_CORE; break;
	case 5:
		r = RLIMIT_RSS; break;
	case 6:
		r = RLIMIT_NPROC; break;
	case 7:
		r = RLIMIT_NOFILE; break;
	case 8:
		r = RLIMIT_MEMLOCK; break;
	case 9:
		r = RLIMIT_AS; break;
	}
	rl.rlim_cur = soft;
	rl.rlim_max = hard;
	RETVAL = setrlimit(r, &rl);
OUTPUT:
	RETVAL


int
getlimit(IN resource, OUT soft, OUT hard)
	int resource
	int soft
	int hard
CODE:
	struct rlimit rl;
	int r;

	switch(resource) {
	case 0:
		r = RLIMIT_CPU; break;
	case 1:
		r = RLIMIT_FSIZE; break;
	case 2:
		r = RLIMIT_DATA; break;
	case 3:
		r = RLIMIT_STACK; break;
	case 4:
		r = RLIMIT_CORE; break;
	case 5:
		r = RLIMIT_RSS; break;
	case 6:
		r = RLIMIT_NPROC; break;
	case 7:
		r = RLIMIT_NOFILE; break;
	case 8:
		r = RLIMIT_MEMLOCK; break;
	case 9:
		r = RLIMIT_AS; break;
	}

	RETVAL = getrlimit(r, &rl);
	soft = rl.rlim_cur;
	hard = rl.rlim_max;
OUTPUT:
	RETVAL


