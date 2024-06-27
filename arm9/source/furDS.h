#include <nds.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

extern unsigned char song[];
extern unsigned char samples[];
extern int loopPoint;
extern unsigned int songLength;
extern unsigned int songRate;
uint8_t furDSregs[0x100];
uint8_t furDSregs2[0x100];
uint8_t oldfurDSregs[0x100];
uint8_t oldfurDSregs2[0x100];

int loopStart = 0;
int songOffset = 0;
unsigned int tickDelay = 0;
int noLoop = 0;

int furDSFIFOusage = 0;

bool furDScanWrite(uint8_t reg, int i, uint8_t *regs, uint8_t *oldregs) {
	const bool canWriteLut[16] = {
		true , true , true , true ,
		false, false, false, true ,
		false, true , false, true ,
		false, false, false, true ,
	};
	if ((i&15) == 0) {
		uint32_t regval = (regs[i|0]<<0)|
                          (regs[i|1]<<8)|
                          (regs[i|2]<<16)|
                          (regs[i|3]<<24);
		uint32_t regval2 = (oldregs[i|0]<<0)|
                           (oldregs[i|1]<<8)|
                           (oldregs[i|2]<<16)|
				           (oldregs[i|3]<<24);
		memcpy(oldregs,regs,sizeof(uint8_t)*256);
		return regval != regval2;
	}
	return canWriteLut[reg&15];
}


void furDSplay();

void furDSinit() {
	songOffset = 0;
	tickDelay = 0;
	if (loopPoint == -1) {
		loopStart = 0;
		noLoop = 1;
	} else {
		loopStart = loopPoint;
		noLoop = 0;
	}
	for (int i = 0; i < 256; i++) furDSregs[i] = 0;
	for (int i = 0; i < 256; i++) furDSregs2[i] = 0;
	fifoSendValue32(FIFO_USER_01,0x7f|(0x40<<16));
	fifoSendValue32(FIFO_USER_01,0x123456);
	for (int i = 0; i < 16; i++) {
		furDSregs[i<<4] = 0x7f;
		furDSregs[i<<4|2] = 0x40;
		furDSregs2[i<<4] = 0x7f;
		furDSregs2[i<<4|2] = 0x40;
	}
	for (int frame = 0; frame < 4; frame++) {
		swiWaitForVBlank();
	}
	printf("\e[22;3HFurDS was made by AArt1256");
	timerStart(0, ClockDivider_1024, TIMER_FREQ_1024(((int)songRate)), furDSplay);
}

void furDSplay() {
	if (tickDelay == 0) {
vBlankFurDS:
		int CMD = -1;
		int songOffsetTmp = songOffset;
		int furDSFifoAmt = 0;
		bool contWhile = true;
		while (contWhile) {
			CMD = song[songOffset++];
			if (songOffset >= songLength) {
				songOffset = loopStart;
				furDSFifoAmt++;
				break;
			}
			switch (CMD) {
				case 0: {
					uint8_t reg = song[songOffset++];
					uint8_t val = song[songOffset++];
					int i = reg&0b11111100;
					furDSregs2[reg] = val;
					if (furDScanWrite(reg,i,furDSregs2,oldfurDSregs2)) furDSFifoAmt++;
					break;
				}
				case 1: {
					tickDelay = song[songOffset++];
					tickDelay |= song[songOffset++]<<8;
					contWhile = false;
					break;
				}
			}
		}
		if (furDSFifoAmt > 100) {
			furDSFIFOusage = 101;
			//printf("FIFO overrun!!!\n");
			return;
		}
		tickDelay = 0;
		songOffset = songOffsetTmp;
		int furDSFifoAmtPre = furDSFifoAmt;
		furDSFifoAmt = 0;
		contWhile = true;
		while (contWhile) {
			CMD = song[songOffset++];
			if (songOffset >= songLength) {
				songOffset = loopStart;
				if (noLoop) {
					for (int i = 0; i < 256; i++) furDSregs[i] = 0;
					for (int i = 0; i < 256; i++) furDSregs2[i] = 0;
					fifoSendValue32(FIFO_USER_01,0x7f|(0x40<<16));
					fifoSendValue32(FIFO_USER_01,0x123456);
					for (int i = 0; i < 16; i++) {
						furDSregs[i<<4] = 0x7f;
						furDSregs[i<<4|2] = 0x40;
						furDSregs2[i<<4] = 0x7f;
						furDSregs2[i<<4|2] = 0x40;
					}
				}
				furDSFifoAmt++;
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
						regval += (uint32_t)samples;
					}
					if (furDSFifoAmt > 100) {
						//printf("FIFO overrun?\n");
					}
					if (((furDSFifoAmtPre > 100 && furDSFifoAmt < 100) || (furDSFifoAmtPre < 100)) && furDScanWrite(reg,i,furDSregs,oldfurDSregs)) {
						//printf("%02x: %02x %02x: %08x\n",reg,val,i,regval);
						//if (((i&15)) == 0) printf("%08x %08x\n",regval,(regval&(~(3<<27)))|(1<<27));
						fifoSendValue32(FIFO_USER_01,regval);
						fifoSendValue32(FIFO_USER_01,i);
						furDSFifoAmt++;
					}
					break;
				}
				case 1: {
					tickDelay = song[songOffset++];
					tickDelay |= song[songOffset++]<<8;
					contWhile = false;
					break;
				}
			}
		}
		furDSFIFOusage = furDSFifoAmt;
		//printf("\e[23;28HFIFO");
		//printf("\e[23;0HOffset: %d",songOffset);
		//printf("%04d %04d\n",furDSFifoAmt,furDSFifoAmtPre);
	} else {
		furDSFIFOusage = 0;
		tickDelay--;
		if (tickDelay == 0) goto vBlankFurDS;
	}
}
