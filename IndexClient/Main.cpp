#include "Client.h"

#define QUERY_COUNT 50
#define QUERY_SIZE 1
#define VARIANT 1

int main(int argc, char* argv[]) {
	Client c;
#if VARIANT == 0
	c.run();
#elif VARIANT == 1
	c.testPerfomance(QUERY_SIZE, QUERY_COUNT);
#endif
}