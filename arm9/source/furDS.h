#include <nds.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

extern unsigned char song[];
extern unsigned char samples[];
extern int loopPoint;
extern unsigned int songLength;
extern unsigned int songRate;
uint8_t furDSregs[0x200];

int loopStart = 0;
int songOffset = 0;
unsigned int tickDelay = 0;
int noLoop = 0;

bool furDScanWrite(uint8_t reg, int i) {
	if ((i&15) == 0) return true;
	if ((reg&15) == 0x09) return true;
	if ((reg&15) == 0x0B) return true;
	if ((reg&3) == 3) return true;
	return false;
}

void furDSplay();

void furDSinit() {
	songOffset = 0;
	tickDelay = 0;
	if (loopPoint == -1) {
		loopStart = 0;
		noLoop = 1;
	} else {
		loopStart = loopPoint-3;
		noLoop = 0;
	}
	for (int i = 0; i < 256; i++) furDSregs[i] = 0;
	fifoSendValue32(FIFO_USER_01,0x7f|(0x40<<16));
	fifoSendValue32(FIFO_USER_01,0x123456);
	for (int i = 0; i < 16; i++) {
		furDSregs[i<<4] = 0x7f;
		furDSregs[i<<4|2] = 0x40;
	}
	for (int frame = 0; frame < 4; frame++) {
		swiWaitForVBlank();
	}
	timerStart(0, ClockDivider_1024, TIMER_FREQ_1024(((int)songRate)), furDSplay);
}

int furDSFifoAmt = 0;

void furDSplay() {
	if (tickDelay == 0) {
vBlankFurDS:
		int CMD = -1;
		int songOffsetTmp = songOffset;
		int furDSFifoAmt = 0;
		while (tickDelay == 0) {
			CMD = song[songOffset++];
			if (songOffset >= songLength) {
				songOffset = loopStart+6;
				furDSFifoAmt++;
				break;
			}
			switch (CMD) {
				case 0: {
					uint8_t reg = song[songOffset++];
					uint8_t val = song[songOffset++];
					int i = reg&0b11111100;
					if (furDScanWrite(reg,i)) furDSFifoAmt++;
					break;
				}
				case 1: {
					tickDelay = song[songOffset++];
					tickDelay |= song[songOffset++]<<8;
					break;
				}
			}
		}
		if (furDSFifoAmt > 100) {
			printf("FIFO overrun!!!\n");
			return;
		}
		tickDelay = 0;
		songOffset = songOffsetTmp;
		int furDSFifoAmtPre = furDSFifoAmt;
		furDSFifoAmt = 0;
		while (tickDelay == 0) {
			CMD = song[songOffset++];
			if (songOffset >= songLength) {
				songOffset = loopStart+6;
				if (noLoop) {
					for (int i = 0; i < 256; i++) furDSregs[i] = 0;
					fifoSendValue32(FIFO_USER_01,0x7f|(0x40<<16));
					fifoSendValue32(FIFO_USER_01,0x123456);
					for (int i = 0; i < 16; i++) {
						furDSregs[i<<4] = 0x7f;
						furDSregs[i<<4|2] = 0x40;
					}
				}
				break;
			}
			switch (CMD) {
				case 0: {
					uint8_t reg = song[songOffset++];
					uint8_t val = song[songOffset++];
					furDSregs[reg] = val;
					int i = reg&0b11111100;
					uint32_t regval = (furDSregs[i|0]<<0)|
									  (furDSregs[i|1]<<8)|
									  (furDSregs[i|2]<<16)|
				           			  (furDSregs[i|3]<<24);
					if ((i&15) == 4) {
						printf("%08lx ",regval);
						printf("%08lx ",samples);
						regval += (uint32_t)samples;
						printf("%08lx\n",regval);
					}
					if (furDSFifoAmt > 100) {
						printf("FIFO overrun?\n");
					}
					if (((furDSFifoAmtPre > 100 && furDSFifoAmt < 100) || !(furDSFifoAmtPre > 100)) && furDScanWrite(reg,i)) {
						fifoSendValue32(FIFO_USER_01,regval);
						fifoSendValue32(FIFO_USER_01,i);
						furDSFifoAmt++;
					}
					break;
				}
				case 1: {
					tickDelay = song[songOffset++];
					tickDelay |= song[songOffset++]<<8;
					break;
				}
			}
		}
		//printf("\e[0;0H%d        \n",furDSFifoAmt);
	} else {
		tickDelay--;
		if (tickDelay == 0) goto vBlankFurDS;
	}
}
