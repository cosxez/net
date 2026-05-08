#pragma once

void glssa(SDL_Renderer *ren,unsigned int win_width,unsigned int win_height,unsigned short delay,unsigned char k,int r_ofst,int g_ofst,int b_ofst)
{
	extern unsigned char _binary_cvnt_whld_start[];
	extern unsigned char _binary_cvnt_whld_end[];
	unsigned char kk=0;
	while (kk<k)
	{
		for (unsigned int y=0;y<win_height;y++)
		{
			for (unsigned int x=0;x<win_width;x++)
			{
				SDL_SetRenderDrawColor(ren,_binary_cvnt_whld_start[(y*win_width+x)*3+8+r_ofst/10*kk],_binary_cvnt_whld_start[(y*win_width+x)*3+1+8+g_ofst/10*kk],_binary_cvnt_whld_start[(y*win_width+x)*3+2+8+b_ofst/10*kk],255);
				SDL_RenderDrawPoint(ren,x,y);
			}
		}
		kk+=1;
		SDL_RenderPresent(ren);
		SDL_Delay(delay);
	}
}
