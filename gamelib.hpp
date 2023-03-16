#include <math.h>
#define PI 3.14159265359
class sdl_{
	private:
	public:
	//SDL Init() flags
	uint32_t initflags = SDL_INIT_VIDEO; //Init this
	//SDL SetVideoMode() settings
	int sx = 500; //Window size x
	int sy = 500; //Window size y
	int bp = 32; //color type
	uint32_t modeflags = SDL_HWSURFACE | SDL_RESIZABLE;
	//scr
	SDL_Surface *scr;
	//event
	//SDL_Event event;
	//event type
	//uint32_t event_type = SDL_USEREVENT;
	//initiliaze event function
	/*void init_event(){
		
	}*/
	//initiliaze sdl function
	void init_screen(){
		SDL_Init(initflags);
		scr = SDL_SetVideoMode(sx,sy,bp,modeflags);
	}
	//flip
	void flip(){
		SDL_Flip(scr);
	}
	
};
class event_{
	private:
	
	public:
	//For comfort
	struct motiondata{
		int x;
		int y;
		uint32_t winid;
	};
	struct buttondata{
		int type;
		int clickx;
		int clicky;
		int button;
	};
	struct keyboarddata{
		int type;
		int scancode;
		int code;
	};
	SDL_Event event;
	void poll(){
		SDL_PollEvent(&event); //polls event
	}
	struct motiondata getmot(){
		struct motiondata a;
		//a.winid = event.motion.windowID;
		a.x = event.motion.x;
		a.y = event.motion.y;
		return a;
	}
	struct buttondata getclick(){
		struct buttondata a;
		a.type = event.button.type;
		a.clickx = event.button.x;
		a.clicky = event.button.y;
		a.button = event.button.button;
		return a;
	}
	struct keyboarddata getkey(){
		struct keyboarddata a;
		a.type = event.key.type;
		a.scancode = event.key.keysym.scancode;
		a.code = event.key.keysym.sym;
		return a;
	}
};

class alpha_{
	private:
	
	public:
	uint8_t mixchannel(uint8_t lowlevel,uint8_t highlevel,uint8_t alpha){
		return lowlevel+(highlevel*alpha/255);
	}
	uint32_t mixcolor(uint32_t lowlevel,uint32_t highlevel,uint8_t alpha){
		uint32_t output = 0;
		uint8_t *out = (uint8_t * )&output;
		uint8_t *hl = (uint8_t * )&highlevel;
		uint8_t *ll = (uint8_t * )&lowlevel;
		out[1] = mixchannel(ll[1],hl[1],hl[0]);
		out[2] = mixchannel(ll[2],hl[2],hl[0]);
		out[3] = mixchannel(ll[3],hl[3],hl[0]);
		return output;
	}
};
class draw_{
	private:
	class alpha_ alpha;
	public:
	void copypixel(int x,int y,int px,int py,SDL_Surface *scr){
		uint32_t *p = (uint32_t * )scr->pixels;
		if(x < scr->w & px < scr->w & y < scr->h & py < scr->h){
			if(x >= 0 & y >= 0 & px >= 0 & py >= 0){
				p[px+(py*scr->w)] = p[x+(y*scr->w)];
			}
		}
	}
	void copyrect(int sx,int sy,int ex,int ey,int px,int py,SDL_Surface *scr){
	uint32_t *p = (uint32_t * )malloc(scr->h*scr->w*4);
	uint32_t *pixels = (uint32_t * )scr->pixels;
	int cx = 0;
	int cy = 0;
	while(cx < scr->w){
		cy  = 0;
		while(cy < scr->h){
			p[(cy*scr->w)+cx] = pixels[(cy*scr->w)+cx];
			cy++;
		}
		cx++;
	}
	if(sx > -1 & sy > -1){
	int x = sx;
	int y = 0;
	while(y <= ey & x < scr->h){
	x = sx;
	while(x <= ex & x < scr->w){
	pixels[((py+y)*scr->w)+(px+x)] = p[((y)*scr->w)+(x)];
	x++;
	}
	y++;
	}
	}
	}
	void fill(uint32_t color,SDL_Surface *scr){
	int x = 0;
	int y = 0;
	uint32_t *p = (uint32_t * )scr->pixels;
	while(y < scr->h){
		x = 0;
		while(x < scr->w){
			p[((y*scr->w)+x)] = color;
			x++;
		}
		y++;
	}
	}
	void drawimage(int sx,int sy,int imgx,int imgy,SDL_Surface *scr,uint32_t *img){
		int x = 0;
		int y = 0;
		uint32_t *p = (uint32_t * )scr->pixels;
		while(y < imgy & y+sy < scr->h){
			x = 0;
			while(x < imgx & x+sx < scr->w){
				uint32_t color = img[(y*imgx)+(x)];
				uint8_t colorbuf[4];
				uint8_t *prevcol = (uint8_t *)(&*p+((y*imgx)+(x)));
				uint8_t *cb = (uint8_t *)(&*img+((y*imgx)+(x)));
				colorbuf[0] = alpha.mixchannel(prevcol[2],cb[2],cb[3]);
				colorbuf[1] = alpha.mixchannel(prevcol[1],cb[1],cb[3]);
				colorbuf[2] = alpha.mixchannel(prevcol[0],cb[0],cb[3]);
				p[(x+sx)+((y+sy) * scr->w)] = (uint32_t)&colorbuf;
				x++;
				}

			y++;
			}
	}
	void drawpicture(int sx,int sy,int imgx,int imgy,SDL_Surface *scr,uint32_t *img){
		int x = 0;
		int y = 0;
		uint32_t *p = (uint32_t * )scr->pixels;
		uint8_t *pb = (uint8_t * )scr->pixels;
		while(y < imgy & y+sy < scr->h){
			x = 0;
			while(x < imgx & x+sx < scr->w){
				uint32_t color = img[(y*imgx)+(x)];
				uint8_t *color_a = (uint8_t * )&color;
				uint32_t color_buf = color;
				uint8_t *cb = (uint8_t *)(&color_buf);
				uint8_t *prevcol = (uint8_t *)(&*p+(y*imgx)+(x));
				color_a[0] = alpha.mixchannel(prevcol[2],cb[2],cb[3]);
				color_a[1] = alpha.mixchannel(prevcol[1],cb[1],cb[3]);
				color_a[2] = alpha.mixchannel(prevcol[0],cb[0],cb[3]);
				p[(x+sx)+((y+sy) * scr->w)] = color;
				
				x++;
				}

			y++;
			}
	}
	void rect(int sx,int sy,int ex,int ey,uint32_t val,SDL_Surface *scr){
	uint32_t *p = (uint32_t * ) scr->pixels;
	if(sx > -1 & sy > -1){
	int x = sx;
	int y;
	while(x <= ex & x < scr->w){
	y = sy;
	while(y <= ey & y < scr->h){
	p[x+(y*scr->w)] = val;
	y++;
	}
	x++;
	}
	}
	}
	void line(int sx,int sy,int ex,int ey,uint32_t val,SDL_Surface *scr){
		int range = sqrt(fabs(pow(ex-sx,2))+fabs(pow(ey-sy,2)));
		float ax = range/(ex-sx);
		float ay = range/(ey-sy);
		float cx = 0;
		float cy = 0;
		uint32_t *p = (uint32_t * )scr->pixels;
		while(sqrt(fabs(pow(cx,2))+fabs(pow(cy,2))) < range & cx < scr->w & cy < scr->h & cx > -1 & cy > -1){
			p[(int)cx+((int)cy*scr->w)] = val;
			cx+=ax;
			cy+=ay;
		}
		
	}
	void drawimage_on_buffer(int sx,int sy,int imgx,int imgy,int bufx,int bufy,uint32_t *p,uint32_t *img){
		int x = 0;
		int y = 0;
		while(y < imgy & y+sy < bufy){
			x = 0;
			while(x < imgx & x+sx < bufx){
				uint32_t color = img[(y*imgx)+(x)];
				p[(x+sx)+((y+sy) * bufx)] = color;
				
				x++;
				}

			y++;
			}
	}
	void setpixel(int x,int y,uint32_t val,SDL_Surface *scr){
	uint32_t *p = (uint32_t * )scr->pixels;
	if((scr->w*scr->h) > x+(y*scr->w)){
	p[x+(y*scr->w)] = val;}
	}
	uint32_t getpixel(int x,int y,SDL_Surface *scr){
	uint32_t *p = (uint32_t * )scr->pixels;
	if((scr->w*scr->h) > x+(y*scr->w)){
	return p[(x+(y * scr->w ))];
	}
	}
	
};

class interface_{
	private:
	class draw_ d;
	public:
	struct button{
		int startx;
		int starty;
		int sizex;
		int sizey;
		int clicktype;
		uint32_t *image;
	};
	struct button buttons[100];
	int set_image(int id,uint32_t *image){
		buttons[id].image = image;
	}
	int draw_button(int id,SDL_Surface *scr){
		d.drawpicture(buttons[id].startx,buttons[id].starty,buttons[id].sizex,buttons[id].sizey,scr,buttons[id].image);
	}
	int createbutton(int id,int startx,int starty,int sizex,int sizey,int clicktype){
		//buttons[id] = (interface_::button)malloc(sizeof(struct button));
		buttons[id].startx = startx;
		buttons[id].starty = starty;
		buttons[id].sizex = sizex;
		buttons[id].sizey = sizey;
		buttons[id].clicktype = clicktype;//SDL_BUTTON_LEFT and more
	}
	int checkbutton(int id,class event_ event){
		event.poll();
		if(event.getmot().x > buttons[id].startx & event.getmot().x < (buttons[id].sizex)+buttons[id].startx & event.getmot().y > buttons[id].starty & event.getmot().y < buttons[id].sizey+buttons[id].starty & event.getclick().button == buttons[id].clicktype & event.getclick().type == SDL_MOUSEBUTTONDOWN){
			return 1;
		}
		return 0;
	};
};
class font_{
	private:
	class draw_ d;
	public:
	int csizex = 31;//symbol size x
	int csizey = 30;//symbol size y
	uint32_t *smap;//symbols picture
	int smapx = 1000;//symbols picture size x
	int smapy = 30;//symbols picture size y
	char alphabet[255] = "abcdefghijklmnopqrstuvwxyz";//symbols filter
	int charcount = 27;//symbols count
	int font_file(char* file){//opens and saves to smap font file
		FILE* font_f = fopen(file,"rb");
		uint32_t *font_i = (uint32_t * )malloc(smapx*smapy*4);
		fread(font_i,smapx,smapy*4,font_f);
		smap = font_i;
	}
	int draw_char(int sx,int sy,int id,SDL_Surface *scr){//draw one symbol
		uint32_t *cbuf = (uint32_t * )malloc(4*csizex*csizey);
		int x = 0;
		int y = 0;
		while(y < csizey){
			x = 0;
			while(x < csizex){
				cbuf[x+(y*csizex)] = smap[(x+(id*csizex))+(y*smapx)];
				x++;
			}
			y++;
		}
		d.drawpicture(sx,sy,csizex,csizey,scr,cbuf);
	}
	int text(int sx,int sy,char* text,int textsize,SDL_Surface *scr){//print text
		int symbol = 0;
		while(symbol < textsize){
			int findcur = 0;
			int b = 0;
			while(findcur < charcount & b == 0){
				if(alphabet[findcur] == text[symbol]){
					b = 1;
					findcur--;
				}
				findcur++;
			}
			if(b != 0){
				draw_char(sx+(symbol*csizex),sy,findcur,scr);	
			}
			symbol++;
		}
	}
};
class vectors_{
	//class i used in my 3d game
	public:
	float gx(float g){
  float output = 0;
  output = sin(3.14/180*g);
  return output;
  
}
float gy(float g){
	
  float output = 0;
  output = cos(3.14/180*g);
  return output;
}

};
