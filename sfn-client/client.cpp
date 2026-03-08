#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void ulb_clear()
{
	std::cout<<"\033c";
}

int main()
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	
	sockaddr_in addr;
	

	ulb_clear();
	bool is_ar=true;
	std::cout<<"Mode: "<<(is_ar ? "auto request" : "manual request")<<std::endl;
	while (true)
	{
		std::string cmd;
		while (cmd.empty()){std::cout<<'>';getline(std::cin,cmd);}
		if (cmd=="exit"){break;}
		if (cmd=="crm"){is_ar=is_ar ? false : true; ulb_clear();std::cout<<"Mode: "<<(is_ar ? "auto request":"manual request")<<std::endl;continue;}
		if (cmd=="help"){std::cout<<"crm - change requests mode (auto request or manual)\n"<<"conn - tcp connect\n";continue;}
		if (cmd.size()==4)
		{
			std::string ft;
			for (int i=0;i<4;i++)
			{
				ft+=cmd[i];
			}
			if (ft=="conn")
			{
				std::string ip_addr;
				while (ip_addr.empty()){std::cout<<"Enter ip address: ";getline(std::cin,ip_addr);}
				std::string port;
				while (port.empty()){std::cout<<"Enter port: ";getline(std::cin,port);}
				
				memset(&addr,0,sizeof(addr));
				try
				{
					addr.sin_family=AF_INET;
					addr.sin_port=htons(std::stoi(port));
					inet_pton(AF_INET,ip_addr.c_str(),&addr.sin_addr);
					connect(sock,(struct sockaddr*)&addr,sizeof(addr));
					std::cout<<"Connected\n";
				}
				catch(std::exception &e){std::cout<<"Error: "<<e.what()<<std::endl;}
				shutdown(sock,SHUT_RDWR);
			}
		}
	}
	close(sock);
	return 0;
}
