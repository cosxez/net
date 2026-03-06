#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
	int sock=socket(AF_INET,SOCK_STREAM,0);

	std::string ip_addr;
	while (ip_addr.empty())
	{
		std::cout<<"Enter ip address: ";
		getline(std::cin,ip_addr);
	}

	std::string port;
	while (port.empty())
	{
		std::cout<<"Enter port: ";
		getline(std::cin,port);
	}

	sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(std::stoi(port));

	inet_pton(AF_INET,ip_addr.c_str(),&addr.sin_addr);
	try{connect(sock,(struct sockaddr*)&addr,sizeof(addr));}catch(std::exception &er){std::cout<<"Error: "<<er.what()<<std::endl;}

	while (true)
	{
		std::string cmd;
		while (cmd.empty())
		{
			std::cout<<'>';
			getline(std::cin,cmd);
		}
		if (cmd=="exit"){break;}
		else if(cmd=="io")
		{
			try
			{
				std::string dts;
				while (dts.empty())
				{
					std::cout<<"Enter file name: ";
					getline(std::cin,dts);
				}
				std::string endstr="GET DATA:"+dts;
				send(sock,endstr.c_str(),endstr.size(),0);

				size_t fs;
				recv(sock,reinterpret_cast<char*>(&fs),sizeof(fs),0);
				if (fs!=16360)
				{
					std::vector<char> cs(fs);
					size_t fns=0;
					while (fns<cs.size())
					{
						fns+=recv(sock,cs.data()+fns,cs.size(),0);
					}

					std::ofstream file(dts,std::ios::binary);
					file.write(reinterpret_cast<const char*>(cs.data()),cs.size());
					file.close();
				}
			}
			catch(std::exception &e){std::cout<<"Error: "<<e.what()<<std::endl;}
		}
	}
	close(sock);
	return 0;
}
