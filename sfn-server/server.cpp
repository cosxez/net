#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void client_conn(int client)
{
	char buffer[1024];
	std::string str;
	std::string data;
	while (true)
	{
		try
		{
			ssize_t sb=recv(client,buffer,sizeof(buffer)-1,0);
			if (sb>0)
			{
				buffer[sb]='\0';
				for (int i=0;i<9;i++){str+=buffer[i];}
				if (str=="GET DATA:")
				{
					for (int i=9;buffer[i]!='\0';i++){data+=buffer[i];}
					std::ifstream file(data,std::ios::binary);
					if (!file.is_open()){uint16_t ierr=16360;send(client,reinterpret_cast<const char*>(&ierr),sizeof(ierr),0);}
					else
					{
						file.seekg(0,std::ios::end);
						size_t fs=file.tellg();
						file.seekg(0,std::ios::beg);
						
						std::vector<char> cs(fs);
						file.read(reinterpret_cast<char*>(cs.data()),cs.size());
						file.close();
	
						send(client,reinterpret_cast<const char*>(&fs),sizeof(fs),0);
						send(client,cs.data(),cs.size(),0);
					}
				}
			}
			else{break;}
		}
		catch(std::exception &e){std::cout<<"Error: "<<e.what()<<std::endl;}
		memset(buffer,0,sizeof(buffer));
		str.clear();
		data.clear();
	}
	close(client);
	std::cout<<"Client dissconeted\n";
}

int main()
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	
	sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(3580);
	addr.sin_addr.s_addr=INADDR_ANY;

	bind(sock,(struct sockaddr*)&addr,sizeof(addr));
	listen(sock,16);

	std::cout<<"Server work\n";

	while (true)
	{
		try
		{
			int cl=accept(sock,nullptr,nullptr);
			std::cout<<"Client connected\n";
			std::thread(client_conn,cl).detach();
		}
		catch(std::exception &e){std::cout<<"Error: "<<e.what()<<std::endl;}
	}
	close(sock);
	return 0;
}
