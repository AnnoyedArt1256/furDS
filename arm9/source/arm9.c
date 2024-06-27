#include <nds.h>
#include <stdint.h>
#include "furDS.h"
#include "furnace_logo.h"
#include "furds_text.h"
#include "furds_description.h"
#include <stdio.h>
#include <math.h>

#define OLIVEC_RED(color)   (((color)&0x000000FF)>>(8*0))
#define OLIVEC_GREEN(color) (((color)&0x0000FF00)>>(8*1))
#define OLIVEC_BLUE(color)  (((color)&0x00FF0000)>>(8*2))
#define OLIVEC_ALPHA(color) (((color)&0xFF000000)>>(8*3))
#define OLIVEC_RGBA(r, g, b, a) ((((r)&0xFF)<<(8*0)) | (((g)&0xFF)<<(8*1)) | (((b)&0xFF)<<(8*2)) | (((a)&0xFF)<<(8*3)))

u16 textures[65536];

int textureID;

float anglex = 0;
float angley = 0;

float canglex = 3.141592653;
float cangley = 0;

float zline(float x1,float y1,float z1,float anglex,float angley,float r[]) {
    float rot1[3] = {(z1*sin(anglex))+(x1*cos(anglex)),y1,(x1*sin(anglex))-(z1*cos(anglex))};
    x1 = rot1[0]; 
    y1 = rot1[1];
    z1 = rot1[2];
    float rot3[3] = {x1,(y1*cos(angley))-(z1*sin(angley)),(y1*sin(angley))+(z1*cos(angley))};
	r[0] = rot3[0];
	r[1] = rot3[1];
	r[2] = rot3[2];
}

#define FLOAT_TYPE float


struct point {
    float x, y;
};

struct zpoint {
	FLOAT_TYPE x;
	FLOAT_TYPE y;
	FLOAT_TYPE z;
};

struct quad {
	struct zpoint a;
	struct zpoint b;
	struct zpoint c;
	struct zpoint d;
};

#define cube1(x,y,z) (struct quad){(struct zpoint){-x,y,z/2},(struct zpoint){x,y,z/2},(struct zpoint){x,-y,z/2},(struct zpoint){-x,-y,z/2}}
#define cube2(x,y,z) (struct quad){(struct zpoint){-x,y,-z/2},(struct zpoint){x,y,-z/2},(struct zpoint){x,-y,-z/2},(struct zpoint){-x,-y,-z/2}}
#define cube3(x,y,z) (struct quad){(struct zpoint){x,y,z/2},(struct zpoint){x,-y,z/2},(struct zpoint){x,-y,-z/2},(struct zpoint){x,y,-z/2}}
#define cube4(x,y,z) (struct quad){(struct zpoint){-x,y,z/2},(struct zpoint){-x,-y,z/2},(struct zpoint){-x,-y,-z/2},(struct zpoint){-x,y,-z/2}}
#define cube5(x,y,z) (struct quad){(struct zpoint){-x,-y,-z/2},(struct zpoint){x,-y,-z/2},(struct zpoint){x,-y,z/2},(struct zpoint){-x,-y,z/2}}
#define cube6(x,y,z) (struct quad){(struct zpoint){-x,y,-z/2},(struct zpoint){x,y,-z/2},(struct zpoint){x,y,z/2},(struct zpoint){-x,y,z/2}}

#define MULT 256

	int t = 0;


	int textureID;

static void vga_3dtri(struct quad q, struct zpoint p, uint32_t color, float anglex, float angley, float cx, float cy, float cz, int w, int h, int tx, int ty) {
	float r[3];
	zline(q.a.x,q.a.y,q.a.z,anglex,angley,r);
	q.a.x = r[0];
	q.a.y = r[1];
	q.a.z = r[2];
	q.a.x -= p.x+cx;
	q.a.y += p.y+cy;
	q.a.z += p.z+cz;
	zline(q.a.x,q.a.y,q.a.z,canglex,cangley,r);
	q.a.x = r[0];
	q.a.y = r[1];
	q.a.z = r[2];

	zline(q.b.x,q.b.y,q.b.z,anglex,angley,r);
	q.b.x = r[0];
	q.b.y = r[1];
	q.b.z = r[2];
	q.b.x -= p.x+cx;
	q.b.y += p.y+cy;
	q.b.z += p.z+cz;
	zline(q.b.x,q.b.y,q.b.z,canglex,cangley,r);
	q.b.x = r[0];
	q.b.y = r[1];
	q.b.z = r[2];

	zline(q.c.x,q.c.y,q.c.z,anglex,angley,r);
	q.c.x = r[0];
	q.c.y = r[1];
	q.c.z = r[2];
	q.c.x -= p.x+cx;
	q.c.y += p.y+cy;
	q.c.z += p.z+cz;
	zline(q.c.x,q.c.y,q.c.z,canglex,cangley,r);
	q.c.x = r[0];
	q.c.y = r[1];
	q.c.z = r[2];

	zline(q.d.x,q.d.y,q.d.z,anglex,angley,r);
	q.d.x = r[0];
	q.d.y = r[1];
	q.d.z = r[2];
	q.d.x -= p.x+cx;
	q.d.y += p.y+cy;
	q.d.z += p.z+cz;
	zline(q.d.x,q.d.y,q.d.z,canglex,cangley,r);
	q.d.x = r[0];
	q.d.y = r[1];
	q.d.z = r[2];

#define N_WHATEVER2 10.0

	struct point a = (struct point){
		128+(q.a.x)/(q.a.z)*N_WHATEVER2,
		96+(q.a.y)/(q.a.z)*N_WHATEVER2
	};
	struct point b = (struct point){
		128+(q.b.x)/(q.b.z)*N_WHATEVER2,
		96+(q.b.y)/(q.b.z)*N_WHATEVER2
	};
	struct point c = (struct point){
		128+(q.c.x)/(q.c.z)*N_WHATEVER2,
		96+(q.c.y)/(q.c.z)*N_WHATEVER2
	};
	struct point d = (struct point){
		128+(q.d.x)/(q.d.z)*N_WHATEVER2,
		96+(q.d.y)/(q.d.z)*N_WHATEVER2
	};

#define N_WHATEVER 18.0
	q.a.x /= -N_WHATEVER;
	q.b.x /= -N_WHATEVER;
	q.c.x /= -N_WHATEVER;
	q.d.x /= -N_WHATEVER;
	q.a.y /= -N_WHATEVER;
	q.b.y /= -N_WHATEVER;
	q.c.y /= -N_WHATEVER;
	q.d.y /= -N_WHATEVER;
	q.a.z /= N_WHATEVER;
	q.b.z /= N_WHATEVER;
	q.c.z /= N_WHATEVER;
	q.d.z /= N_WHATEVER;
	if (
		((int)(q.a.x*(1<<12))) == floattov16(q.a.x) &&
		((int)(q.a.y*(1<<12))) == floattov16(q.a.y) &&
		((int)(q.a.z*(1<<12))) == floattov16(q.a.z) &&
		((int)(q.b.x*(1<<12))) == floattov16(q.b.x) &&
		((int)(q.b.y*(1<<12))) == floattov16(q.b.y) &&
		((int)(q.b.z*(1<<12))) == floattov16(q.b.z) &&
		((int)(q.c.x*(1<<12))) == floattov16(q.c.x) &&
		((int)(q.c.y*(1<<12))) == floattov16(q.c.y) &&
		((int)(q.c.z*(1<<12))) == floattov16(q.c.z) &&
		((int)(q.d.x*(1<<12))) == floattov16(q.d.x) &&
		((int)(q.d.y*(1<<12))) == floattov16(q.d.y) &&
		((int)(q.d.z*(1<<12))) == floattov16(q.d.z)
/*
		&&
		a.x > -32 && a.x < 284 && b.x > -2 && b.x < 284 &&
		c.x > -32 && c.x < 284 && d.x > -2 && d.x < 284
*/
		) {
			glBindTexture(0, textureID);
			glBegin(GL_QUAD);
			glNormal(NORMAL_PACK(0,inttov10(-1),0));

			GFX_TEX_COORD = (TEXTURE_PACK(inttot16(tx+w), inttot16(ty+h)));
			glVertex3v16(floattov16(q.a.x),floattov16(q.a.y), floattov16(q.a.z));

			GFX_TEX_COORD = (TEXTURE_PACK(inttot16(tx), inttot16(ty+h)));
			glVertex3v16(floattov16(q.b.x), floattov16(q.b.y), floattov16(q.b.z));

			GFX_TEX_COORD = (TEXTURE_PACK(inttot16(tx), inttot16(ty)));
			glVertex3v16(floattov16(q.c.x), floattov16(q.c.y), floattov16(q.c.z));

			GFX_TEX_COORD = (TEXTURE_PACK(inttot16(tx+w), inttot16(ty)));
			glVertex3v16(floattov16(q.d.x), floattov16(q.d.y), floattov16(q.d.z));

		glEnd();
	}
    //olivec_triangle3c(oc,d.x,d.y,c.x,c.y,a.x,a.y,color,color,color);
}

double fifoUsage = 0.0;

int main(void) {
	videoSetMode(MODE_5_3D);
	consoleDemoInit();

	// initialize gl
	glInit();
	
    lcdMainOnBottom();
 
	// sub background holds the top image when 3D directed to bottom
	bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

	//enable textures
	glEnable(GL_TEXTURE_2D);

	// enable antialiasing
	glEnable(GL_ANTIALIAS);

	glEnable(GL_OUTLINE);
	
	// setup the rear plane #2b3338
	glClearColor(0x22>>3,0x24>>3,0x26>>3,31); // BG must be opaque for AA to work
	glClearPolyID(0xFF); // BG must have a unique polygon ID for AA to work
    glSetOutlineColor(7,0x8000);
	glClearDepth(0x7FFF);

	//this should work the same as the normal gl call
	glViewport(0,0,255,191);



	vramSetBankA(VRAM_A_TEXTURE);
	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 128; j++) {
			uint32_t p = png_pixels[0];
			textures[j+0+((i+0)*256)] = RGB15(OLIVEC_RED(p)>>3,OLIVEC_GREEN(p)>>3,OLIVEC_BLUE(p)>>3)|0x8000;
		}
	}

	for (int i = 0; i < png2_height; i++) {
		for (int j = 0; j < png2_width; j++) {
			uint32_t p = png2_pixels[png2_width*i+j];
			textures[j+128+((i+0)*256)] = RGB15(OLIVEC_RED(p)>>3,OLIVEC_GREEN(p)>>3,OLIVEC_BLUE(p)>>3)|0x8000;
		}
	}

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			textures[j+128+((i+128)*256)] = RGB15(0x08,0x0E,0x12)|0x8000;
		}
	}

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			textures[j+192+((i+128)*256)] = RGB15(0x1F,0x0E,0x12)|0x8000;
		}
	}


	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			uint32_t p = png_pixels[64*i+j];
			textures[j+0+((i+0)*256)] = RGB15(OLIVEC_RED(p)>>3,OLIVEC_GREEN(p)>>3,OLIVEC_BLUE(p)>>3)|0x8000;
		}
	}

	for (int i = 0; i < png3_height; i++) {
		for (int j = 0; j < png3_width; j++) {
			uint32_t p = png3_pixels[png3_width*i+j];
			textures[j+0+((i+240)*256)] = RGB15(OLIVEC_RED(p)>>3,OLIVEC_GREEN(p)>>3,OLIVEC_BLUE(p)>>3)|0x8000;
		}
	}


	glGenTextures(1, &textureID);
	glBindTexture(0, textureID);
	glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_256 , TEXTURE_SIZE_256, 0, TEXGEN_TEXCOORD, (u8*)textures);
	
	vramSetBankE(VRAM_E_MAIN_BG);	 // Main bg text/8bit bg. Bank E size == 64kb, exactly enough for 8bit * 256 * 192 + text layer
	
	PrintConsole topScreen;

	// Load our custom font for the top screen
	consoleInit(&topScreen,1, BgType_Text4bpp, BgSize_T_256x256, 31,0, true, false);
	//put bg 0 at a lower priority than the text background
	bgSetPriority(0, 1);

	// Bottom Screen font
	ConsoleFont font;

	font.gfx = consoleGetDefault()->font.gfx;
	font.pal = 0;
	font.numChars = 128;
	font.numColors = 0;
	font.bpp = 4;
	font.asciiOffset = 0;
	font.convertSingleColor = false;
	

	consoleSetFont(&topScreen, &font);


	//any floating point gl call is being converted to fixed prior to being implemented
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 256.0 / 192.0, 0.1, 32);
	
	gluLookAt(	0.0, 0.0, -0.1,		//camera possition
				0.0, 0.0, 0.0,		//look at
				0.0, 1.0, 0.0);		//up


	fifoSendValue32(FIFO_PM, PM_REQ_SLEEP_DISABLE);

	BG_PALETTE[1] = RGB15(31,31,31);
	furDSinit();
	int t = 0;
	struct zpoint cam = {0,0,28};
	while(1) {
		swiWaitForVBlank();


		glPushMatrix();

		//move it away from the camera

		glMatrixMode(GL_MODELVIEW);


		glMaterialf(GL_AMBIENT, RGB15(31,31,31));
		glMaterialf(GL_DIFFUSE, RGB15(31,31,31));
		glMaterialf(GL_SPECULAR, BIT(15) | RGB15(31,31,31));
		glMaterialf(GL_EMISSION, RGB15(31,31,31));

		//ds uses a table for shinyness..this generates a half-ass one
		glMaterialShinyness();

		//not a real gl function and will likely change

		struct zpoint cam2 = {cam.x,cam.y,cam.z};

/*
		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(0));

		vga_3dtri(cube1(6,6,12),cam2,0xAF2D00C0,anglex,angley,0,2,0,64,64,0,0);
		vga_3dtri(cube2(6,6,12),cam2,0xAF2D00C0,anglex,angley,0,2,0,64,64,0,0);
		vga_3dtri(cube3(6,6,12),cam2,0xAF2D00C0,anglex,angley,0,2,0,64,64,0,0);
		vga_3dtri(cube4(6,6,12),cam2,0xAF2D00C0,anglex,angley,0,2,0,64,64,0,0);
		vga_3dtri(cube5(6,6,12),cam2,0xAF2D00C0,anglex,angley,0,2,0,64,64,0,0);
		vga_3dtri(cube6(6,6,12),cam2,0xAF2D00C0,anglex,angley,0,2,0,64,64,0,0);
*/

		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(0xFF));

		fifoUsage -= (fifoUsage-(((double)furDSFIFOusage)/5))/5.0;
		vga_3dtri(cube1(3,fifoUsage,12),cam2,0xAF2D00C0,
		                0,0,19.7,fifoUsage-16.5,0,48,48,fifoUsage>14?192:128,130);

		vga_3dtri(cube1(9.6,3,14.4),cam2,0xAF2D00C0,
		                sin(anglex)/8.0,cos(angley/1.42)/14.0,0,-2,0,77,24,128,0);
		vga_3dtri(cube1(16,1.5,8),cam2,0xAF2D00C0,
		                cos(anglex)/8.0,sin(angley)/12.0,0,4,0,180,16,0,240);

		glPopMatrix(1);

		glFlush(0);
		anglex += 0.018;
		angley += 0.025;

		scanKeys();
		int keys = keysDown();
		if (keys & KEY_START) break;
	}
	return 0;
}
