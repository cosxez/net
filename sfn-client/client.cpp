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
				std::string url_s;
				if (is_ar==true)
				{
					while (url_s.empty()){std::cout<<"Enter url: ";getline(std::cin,url_s);}
				}

				memset(&addr,0,sizeof(addr));
				try
				{
					addr.sin_family=AF_INET;
					addr.sin_port=htons(std::stoi(port));
					inet_pton(AF_INET,ip_addr.c_str(),&addr.sin_addr);
					connect(sock,(struct sockaddr*)&addr,sizeof(addr));
					
					if (is_ar==true && !url_s.empty())
					{
						send(sock,url_s.c_str(),url_s.size(),0);
						char buffer[1024];
						size_t stp=recv(sock,buffer,sizeof(buffer),0);
						buffer[stp]='\0';
						if (stp==1 && buffer[0]=='1')
						{
							stp=recv(sock,buffer,sizeof(buffer),0);
							std::cout<<stp<<std::endl;
							for (int i=0;i<stp;i++)
							{
								size_t ltp=recv(sock,buffer,sizeof(buffer),0);
								buffer[ltp]='\0';
								for (int j=0;j<ltp;j++){std::cout<<buffer[j];}
							}
						}
						if (stp==1 && buffer[0]=='0'){std::cout<<"URL error: site doesnt exist\n";}
					}
				}
				catch(std::exception &e){std::cout<<"Error: "<<e.what()<<std::endl;}
				shutdown(sock,SHUT_RDWR);
			}
		}
	}
	close(sock);
	return 0;
}
