#include <nds.h>
#include <stdint.h>
#include "furDS.h"
#include <stdio.h>

int main(void) {
	consoleDemoInit();
	furDSinit();
	int t = 0;
	while(1) {
		swiWaitForVBlank();
		scanKeys();
		int keys = keysDown();
		if (keys & KEY_START) break;
	}
	return 0;
}
