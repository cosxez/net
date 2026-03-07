#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ncurses.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
	initscr();
	unsigned short w,h;
	getmaxyx(stdscr,w,h);
	clear();
	refresh();
	getch();
	endwin();
	return 0;
}
