#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	std::string ip_addr;
	std::cout<<"Enter ip address(default 192.168.1.160): ";
	getline(std::cin,ip_addr);

	std::string port;
	std::cout<<"Enter port(default 3580): ";
	getline(std::cin,port);

	sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(std::stoi(port));
	
	inet_pton(AF_INET,ip_addr.c_str(),&addr.sin_addr);

	std::cout<<"Connection\n";
	try{connect(sock,(struct sockaddr*)&addr,sizeof(addr));}catch(std::exception &er){std::cout<<"Error connect: "<<er.what()<<std::endl;}
	std::cout<<"Connected\n";
	while (true)
	{
		std::string cmd;
		std::cout<<'>';
		getline(std::cin,cmd);
		if (cmd=="exit"){break;}
		else if (cmd=="io")
		{
			std::string id;
			std::cout<<"Enter file id: ";
			getline(std::cin,id);

			std::string req="GET DATA:"+id;
			send(sock,req.c_str(),req.size(),0);
			char size_fs[128];
			memset(size_fs,0,sizeof(size_fs));
			int si=recv(sock,size_fs,sizeof(size_fs),0);
			size_fs[si]='\0';
			std::cout<<si<<' '<<size_fs<<std::endl;
			std::string sz="0";
			for (int i=0;size_fs[i]!='\0';i++){sz+=size_fs[i];}			

			std::vector<char> cs(std::stoi(sz));
			recv(sock,cs.data(),cs.size(),0);
			std::ofstream file(id+".jpg",std::ios::binary);
			file.write(reinterpret_cast<const char*>(cs.data()),cs.size());
			file.close();
		}
		else
		{
			if (cmd.size()<4096)
			{
				try
				{
					if (send(sock,cmd.c_str(),cmd.size(),0)<0){perror("Error: ");}
				}
				catch(std::exception &e){std::cout<<"Error: "<<e.what()<<std::endl;}
			}
		}
	}
	close(sock);
	return 0;
}
