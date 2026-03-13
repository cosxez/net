#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
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
					for (auto &c: std::filesystem::directory_iterator("inetpic_data")){fls.push_back(c.path().filename().string());}

					size_t flss=0;
					for (int i=0;i<fls.size();i++){flss+=fls[i].size();}
					send(client,&flss,sizeof(flss),0);
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					for (int i=0;i<fls.size();i++)
					{
						send(client,fls[i].c_str(),fls[i].size(),0);
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
					}
					while (true)
					{
						sb=recv(client,buffer,sizeof(buffer),0);
						buffer[sb]='\0';
						str="";
						for (int i=0;i<5;i++){str+=buffer[i];}
						if (str=="close"){close(client);std::cout<<"Client dissconected\n";return;}
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
				}
				str="";
				for (int i=0;i<21;i++){str+=buffer[i];}
				if (str=="GET /inetpic HTTP/1.1")
				{
					try
					{
						std::ifstream hindexf("inetpic.html");
						std::string lines_index="";

						while (!hindexf.eof())
						{
							std::string cl;
							getline(hindexf,cl);
							lines_index+=cl;
						}
						hindexf.close();
						std::string headhttp="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
					       	headhttp+=std::to_string(lines_index.size()) + "\r\nConnection: keep-alive\r\n\r\n" + lines_index;
						send(client,headhttp.c_str(),headhttp.size(),0);

						size_t stp=recv(client,buffer,sizeof(buffer),0);
						buffer[stp]='\0';
						str="";
						for (int i=5;buffer[i]!=' ';i++){str+=buffer[i];}
						
						std::ifstream file("inetpic_data/" + str,std::ios::binary);
						if (file.is_open())
						{
							file.seekg(0,std::ios::end);
							size_t fs=file.tellg();
							file.seekg(0,std::ios::beg);

							std::vector<char> fld(fs);
							file.read(reinterpret_cast<char*>(fld.data()),fld.size());
							file.close();
							data="HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nContent-Length: " + std::to_string(fs) + "\r\nConnection: keep-alive\r\n\r\n";
							send(client,data.c_str(),data.size(),0);
							send(client,fld.data(),fld.size(),0);
						}
						else
						{
							data="HTTP/1.1 404 Not Found\r\n\r\n";
							send(client,data.c_str(),data.size(),0);
						}

					}
					catch(std::exception &er){std::cout<<"Error: "<<er.what()<<std::endl;}
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
