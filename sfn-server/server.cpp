#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <vector>
#include <cstring>
#include <filesystem>
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
			ssize_t sb=recv(client,buffer,sizeof(buffer),0);
			if (sb>0)
			{
				buffer[sb]='\0';
				for (int i=0;i<sb;i++){str+=buffer[i];}
				if (str=="inetpic")
				{
					std::string err_url="1";
					send(client,err_url.c_str(),err_url.size(),0);
					std::vector<std::string> fls;
					for (auto &c: std::filesystem::directory_iterator("inetpic_data")){fls.push_back(c.path().filename());}
					size_t flss=fls.size();
					send(client,&flss,sizeof(flss),0);
					for (int i=0;i<fls.size();i++)
					{
						send(client,fls[i].c_str(),fls[i].size(),0);
					}
					sb=recv(client,buffer,sizeof(buffer),0);
					buffer[sb]='\0';
					str="";
					for (int i=0;i<9;i++){str+=buffer[i];}
					if (str=="GET DATA:")
					{
						data="";
						for (int i=9;buffer[i]!='\0';i++){data+=buffer[i];}
						std::ifstream file("inetpic_data/" + data,std::ios::binary);
						file.seekg(0,std::ios::end);
						size_t fsd=file.tellg();
						file.seekg(0,std::ios::beg);

						std::vector<char> fs_data(fsd);
						file.read(reinterpret_cast<char*>(fs_data.data()),fs_data.size());
						file.close();
						
						send(client,&fsd,sizeof(fsd),0);
						send(client,fs_data.data(),fs_data.size(),0);
					}
				}
				else{std::string err_url="0";send(client,err_url.c_str(),err_url.size(),0);}
			}
			else{break;}
		}
		catch(std::exception &e){std::cout<<"Error: "<<e.what()<<std::endl;}
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
