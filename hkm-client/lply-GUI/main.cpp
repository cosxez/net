#include <SDL2/SDL.h>
#include <string>
#include <fstream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "cfg_pars.h"
#include "sdh.h"
#include "network.h"
#include "font.h"

struct mp_attrs
{
	unsigned short w;
	unsigned short h;
	unsigned char bg_attr;
}__attribute__((packed));

int main()
{
	std::ifstream file("config");
	if (!file.is_open()){return -1;}
	
	char buffer_f_cfg[2048];
	unsigned short bfg_idx=0;
	while (!file.eof()){if (bfg_idx==2048){break;}file.get(buffer_f_cfg[bfg_idx]);bfg_idx+=1;}
	file.close();
	buffer_f_cfg[bfg_idx]='\0';
	
	std::string ip;
	unsigned short port;
	cfg_pars(buffer_f_cfg,&ip,&port);

	int sock=socket(AF_INET,SOCK_DGRAM,0);
	
	struct sockaddr_in addr;
	struct sockaddr_in faddr;
	
	if (lply_sbatt(&sock,&addr,&faddr,ip,port)<0){sclose(&sock);return -2;}

	if (lply_connect(&sock,&faddr)<0){sclose(&sock);return -3;}

	mp_attrs mpt;
	if (lply_read(&sock,&faddr,&mpt,sizeof(mpt))<1){sclose(&sock);return -4;}
	
	size_t bgrm;
	if (lply_read(&sock,&faddr,&bgrm,sizeof(bgrm))<1){sclose(&sock);return -4;}
	std::vector<unsigned char> bgv(bgrm);
	size_t crpr=0;
	while (crpr<bgrm)
	{
		size_t csb=lply_read(&sock,&faddr,(bgv.data()+crpr),bgrm);
		if (csb<1){sclose(&sock);break;return -4;};
		crpr+=csb;
	}

	size_t mlistrm;
	if (lply_read(&sock,&faddr,&mlistrm,sizeof(mlistrm))<1){sclose(&sock);return -4;}
	//std::string mlist;mlist.reserve(mlistrm);
	char mlist[mlistrm];
	if (lply_read(&sock,&faddr,&mlist,mlistrm)<1){sclose(&sock);return -4;}

	extern unsigned char _binary_cvnt_whld_start[];
	extern unsigned char _binary_cvnt_whld_end[];

	int win_width=720;
	int win_height=480;
	
	SDL_Window *win=SDL_CreateWindow("Lain player",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,win_width,win_height,SDL_WINDOW_SHOWN);
	SDL_Renderer *ren=SDL_CreateRenderer(win,-1,0);

	char run=1;
	SDL_Event ev;
	
	unsigned char k=0;
	while (k<11)
	{
		for (unsigned int y=0;y<win_height;y++)
		{
			for (unsigned int x=0;x<win_width;x++)
			{
				SDL_SetRenderDrawColor(ren,_binary_cvnt_whld_start[(y*win_width+x)*3+8]/10*k,_binary_cvnt_whld_start[(y*win_width+x)*3+1+8]/10*k,_binary_cvnt_whld_start[(y*win_width+x)*3+2+8]/10*k,255);
				SDL_RenderDrawPoint(ren,x,y);
			}
		}
		k+=1;
		SDL_RenderPresent(ren);
		SDL_Delay(20);
	}

	win_width=mpt.w;win_height=mpt.h;
	SDL_SetWindowSize(win,win_width,win_height);
	//SDL_SetWindowResizable(win,SDL_TRUE);
	
	if (mpt.bg_attr=='c' && bgrm==4){SDL_SetRenderDrawColor(ren,bgv[1],bgv[2],bgv[3],bgv[0]);for (unsigned int y=0;y<win_height;y++){for (unsigned int x=0;x<win_width;x++){SDL_RenderDrawPoint(ren,x,y);}}}
	if (mpt.bg_attr=='i' && bgrm>0)
	{
		unsigned int imgw=*(unsigned int*)(bgv.data());
		unsigned int imgh=*(unsigned int*)(bgv.data()+4);
		for (unsigned int y=0;y<imgh;y++)
		{
			for (unsigned int x=0;x<imgw;x++)
			{
				SDL_SetRenderDrawColor(ren,bgv[(y*imgw+x)*3+8],bgv[(y*imgw+x)*3+8+1],bgv[(y*imgw+x)*3+8+2],255);
				SDL_RenderDrawPoint(ren,x,y);
			}
		}
	}

	unsigned char font_size=1;
	unsigned int lx=0;unsigned int ly=14*font_size;
	unsigned int mlisti=0;
	for (mlisti;mlisti<mlistrm;mlisti++)
	{
		if (ly>=(win_height-14*font_size)){break;}
		if (mlist[mlisti]=='\n'){ly+=14*font_size;lx=0;}
		else
		{
			if (lx<(win_width-24*font_size)){printc(mlist[mlisti],font_size,255,255,255,ren,lx,ly);lx+=12*font_size;}
		}
	}

	while (run)
	{
		while (SDL_PollEvent(&ev)){if (ev.type==SDL_QUIT){run=0;}}

		SDL_RenderPresent(ren);
		SDL_Delay(20);
	}
	sclose(&sock);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
