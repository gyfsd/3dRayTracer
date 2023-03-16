#include <stdio.h>
#include <SDL/SDL.h>
#include <unistd.h>
#include <math.h>
#include "gamelib.hpp"
#include "GameImages.c"
#define PI 3.14
class gmaterial{
	private:
	
	public:
	struct texture{
		int imgx;
		int imgy;
		uint32_t *img;
	};
	struct gradient_{
		uint32_t c1 = 0;
		uint32_t c2 = 0xffffffff;
	};
	struct random_{
		bool enabled = 0;
	};
	struct color_{
		uint32_t color;
	};
	struct material_{
		struct gradient_ *gradient = NULL;
		struct random_ *random = NULL;
		struct color_ *color = NULL;
	};
};
class game_objects_{//contains a set of data about all 3d objects in the game
	private:
	
	public:
	struct sphere_{//object parameters: sphere
		double radius;
		double x;
		double y;
		double z;
		struct gmaterial::material_ material;
	};
	struct cube_{//object parameters: cube
		double x,y,z;
		double rotx,roty;
		double size; //cube size x,y,z
	};
	struct object{//object type and parameters
		int type;
		struct sphere_ sphere;
		struct cube_ cube;
	};
	
};
class game_camera_{
	private:
	public:
	struct camera_data{//Camera data (position, direction)
		double x;
		double y;
		double z;
		double rotx;
		double roty;
	};
};
class ray_{
	private:
	public:
	struct ray_data{
		double x;
		double y;
		double z;
	};
};
class game_objects_render_{//is responsible for the color returned by the ray, the color of the object and also for checking if the ray hits the object
	private:
	public:
	struct result{
		uint32_t col = 0;
		bool hit = 0;
	};
	//render functions
	struct result render_sphere(struct game_objects_::sphere_ sphere,struct ray_::ray_data camd){
		struct result ret;
		double range = sqrt(pow(fabs(sphere.x-camd.x),2)+pow(fabs(sphere.y-camd.y),2)+pow(fabs(sphere.z-camd.z),2));
		ret.hit = 0;
		ret.col = 0;
		if(range < sphere.radius){
			ret.hit = 1;
			ret.col = 0;
			if(sphere.material.color != NULL){
				ret.col = sphere.material.color[0].color;
			}
			if(sphere.material.random[0].enabled == 1){
				ret.col = rand();
			}
			
		}
		return ret;
	}
	struct result render_cube(struct game_objects_::cube_ cube,struct ray_::ray_data camd){
		struct result ret;
		ret.hit = 0;
		ret.col = 0;
		if(camd.x > cube.x-(cube.size/2) & camd.y > cube.y-(cube.size/2) & camd.z > cube.z-(cube.size/2) & camd.x < cube.x+(cube.size/2) & camd.y < cube.y+(cube.size/2) & camd.z < cube.z+(cube.size/2)){
			ret.hit = 1;
			ret.col = 0xffffffff;
		}
		return ret;
	}
	//check all objects
	
	struct result checkall(struct game_objects_::object objs[],int objs_count,struct ray_::ray_data rayd){
		struct result ret;
		int obj_cnt = 0;//object counter
		while(obj_cnt < objs_count){//check in sequence
			if(objs[obj_cnt].type == 0){ //cube
				ret = render_cube(objs[obj_cnt].cube,rayd);
				if(ret.hit == 1){
				return ret;}
			}
			if(objs[obj_cnt].type == 1){ //sphere
				ret = render_sphere(objs[obj_cnt].sphere,rayd);
				if(ret.hit == 1){
				return ret;}
			}
			obj_cnt++;
		}
		return ret;
	}
	
};
class game_trace_{//responsible for ray tracing
	private:
	class draw_ draw;
	class game_objects_render_ gor;
	class game_objects_ go;
	class ray_ ray;
	public:
	double fovx = 180;
	double fovy = 180;
	
	int trace(struct game_objects_::object objs[],int objcount,struct game_camera_::camera_data camd,SDL_Surface *scr){
		printf("tracing...\n");
		uint32_t *p = (uint32_t * )scr->pixels;
		float ax,ay,az;
		float cx,cy,cz;
		float dx,dy,dz;
		float fovx_mat,fovy_mat;
		float camrrx = 3.14/180*camd.rotx;
		float camrry = 3.14/180*camd.roty;
		float afovx = fovx / (float)scr->w;
		float afovy = fovy / (float)scr->h;
		float cfovy = 0;
		float cfovx;
		struct ray_::ray_data rayd;
		struct game_objects_render_::result res;
		while(cfovy < fovy){
			cfovx = 0;
			while(cfovx < fovx){
				/*	here I have fixed some formulas that will help me write the "trace()" function
				rx = x cos α - y sin α
				ry = y cos α + x sin α
				*/
				//================================================
				fovx_mat = (cfovx/fovx*2)-1;
				fovy_mat = (cfovy/fovy*2)-1;
				//=============calculating vector=================
				//ay = fovy_mat*cos(camrry); bad try
				//ax = ((fovx_mat*cos(camrrx) - sin(camrrx)));//*sin(camrry));
				//az = (((cos(camrrx)) + fovx_mat*sin(camrrx)));//*sin(camrry));
				//good try
				ay = (0 + (fovy_mat*cos(camrrx)) - (1*sin(camrrx)) ) * (0+1+0);
				ax = (1+0+0) * ((fovx_mat*cos(camrry)) + 0 + (1*sin(camrry)));
				az = (0 + (fovy_mat*sin(camrrx)) + (1*cos(camrrx))) * ((-1*fovx_mat*sin(camrry)) + 0 + (1*cos(camrry)));
				//================================================
				float r = 0;
				dx = camd.x;
				dy = camd.y;
				dz = camd.z;
				//================================================
				while(r < 50){
					//draw.setpixel(100+r,100+dy,0xff,scr);
					//============================================
					rayd.x = dx;
					rayd.y = dy;
					rayd.z = dz;
					//===========================================
					res = gor.checkall(objs,objcount,rayd);
					//============================================
					if(res.hit == 1){goto skip;}
					//===========================================
					dx+=ax;
					dy+=ay;
					dz+=az;
					r+=1;//sqrt(pow(ax,2)+pow(ay,2)+pow(az,2)); wrong!
				}
				skip:
				if(res.hit == 1){draw.setpixel(cfovx/afovx,cfovy/afovy,res.col,scr);}
				//===============================================
				cfovx+=afovx;
			}
			cfovy+=afovy;
		}
		
	}
	
};
class game_main{
	private:
	class game_trace_ gt;
	class draw_ draw;
	class font_ font;
	public:
	struct game_objects_::object *init_objs(){
		//struct game_objects_::sphere_ sphere;
		struct game_objects_::cube_ cube;
		struct game_objects_::cube_ cube2;
		struct game_objects_::object *objs = (struct game_objects_::object *)malloc(4096);
		cube.x = 0;
		cube.y = 0;
		cube.z = 20;
		cube.size = 5;
		cube2.x = -20;
		cube2.y = 0;
		cube2.z = 20;
		cube2.size = 5;
		/*sphere.x = 0;
		sphere.y = 0;
		sphere.z = 20;
		sphere.radius = 5;
		struct gmaterial::material_ smat;
		struct gmaterial::color_  color;
		struct gmaterial::random_  rnd;
		smat.color = &color;
		smat.random = &rnd;
		smat.color[0].color = 0xffff;
		smat.random[0].enabled = 1;
		sphere.material = smat;
		objs[1].sphere = sphere;
		objs[1].type = 1;*/
		objs[0].cube = cube;
		objs[0].type = 0;
		return objs;
	}
	int start(class sdl_ sdl,class event_ event){
		printf("game_main::start()\n");
		struct game_objects_::object *objs = init_objs();
		struct game_camera_::camera_data camd;
		camd.x = 0;
		camd.y = 0;
		camd.z = 0;
		camd.rotx = 0;
		camd.roty = 0;
		draw.fill(0,sdl.scr);
		struct event_::keyboarddata keyd;
		while(1){
			keyd = event.getkey();
			event.poll();
			if(keyd.type == SDL_KEYDOWN){
				if(keyd.code == SDLK_w){
					camd.x += sin(camd.rotx/3.14*180)*2;
					camd.y += sin(camd.roty/3.14*180)*2;
					camd.z += cos(camd.rotx/3.14*180)*2;
				}
				if(keyd.code == SDLK_s){
					camd.x -= sin(camd.rotx/3.14*180)*2;
					camd.y -= sin(camd.roty/3.14*180)*2;
					camd.z -= cos(camd.rotx/3.14*180)*2;
				}
				if(keyd.code == SDLK_t){
					camd.y += 2;
				}
				if(keyd.code == SDLK_r){
					camd.y -= 2;
				}
				if(keyd.code == SDLK_a){
					camd.roty += 20;
				}
				if(keyd.code == SDLK_d){
					camd.roty -= 20;
				}
				if(keyd.code == SDLK_e){
					camd.rotx += 20;
				}
				if(keyd.code == SDLK_q){
					camd.rotx -= 20;
				}
				while(keyd.type == SDL_KEYUP){event.poll();}
				draw.fill(0,sdl.scr);
				gt.trace(objs,2,camd,sdl.scr);
				sdl.flip();
			}
		}
	}
};
class menu_{//game menu
	private:
	class draw_ draw;
	class vectors_ vec;
	class game_main gm;
	public:
	int start(class sdl_ sdl,class event_ event){//run menu
		class interface_ inter;
		inter.createbutton(0,200,200,200,100,SDL_BUTTON_LEFT);
		//After creating a separate header file with images, you no longer need
		/*uint32_t *btn_0_img = (uint32_t *)malloc(100000);
		FILE* btn_0_file = fopen("a.data","rb");
		fread(btn_0_img,80,1000,btn_0_file);*/
		inter.set_image(0,(uint32_t * )menu_button_0_image);
		inter.draw_button(0,sdl.scr);
		sdl.flip();
		gm.start(sdl,event);
		while(1){
			event.poll();
			if(inter.checkbutton(0,event) == 1){
				printf("menu button: start pressed\n");
				gm.start(sdl,event);
			}
		}
	}
};
class bg_process{
	private:
	public:
	void exitcheck(SDL_Event *e){
		SDL_PollEvent(e);
		if(e->type == SDL_QUIT){
			exit(1);
		}
	}
	void loop(void *args){
		SDL_Event *e = (SDL_Event * )args;
		while(1){
		exitcheck(e);
		}
	}
};
class main_{
	private:
	class draw_ draw;
	class menu_ menu;
	class bg_process bg;
	public:
	int main(int argc,char* argv[]){
		class sdl_ sdl;
		class event_ event;
		sdl.init_screen();
		menu.start(sdl,event);
	}
};
class test_prog{//for testing functions
	private:
	
	public:
	int test_trace(){
		/*
		rx = x cos α - y sin α
		ry = y cos α + x sin α*/
		
	}
	int test(){
		class game_objects_render_ gor;
		struct game_objects_::sphere_ sphere;
		struct game_objects_::cube_ cube;
		struct ray_::ray_data camd;
		
	}
};
int WinMain(int argc,char* argv[]){
	class main_ m;
	class test_prog test;
	//return test.test();
	return m.main(argc,argv);
}
