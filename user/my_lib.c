#include "user/my_lib.h"

void exitOnError(int code, char* errorMessage) {
	if (code == -1) {
		printf("%d: %s\n", getpid(), errorMessage);
		exit(1);
	}
}

