#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

void client_conn(int client_sock,std::ofstream &file,std::ofstream &ser)
{
	char buffer[4096];
	while (true)
	{
		try
		{
			size_t size_buff=recv(client_sock,buffer,sizeof(buffer),0);
			if (size_buff<=0){break;}
			
			if (size_buff<4096)
			{
				buffer[size_buff]='\0';
			}
			if (size_buff>8)
			{
				std::string check;
				for (uint8_t i=0;i<9;i++)
				{
					check+=buffer[i];
				}
				if (check=="GET DATA:")
				{
					std::string data;
					for (uint8_t i=9;buffer[i]!='\0';i++)
					{
						data+=buffer[i];
					}
					if (data.size()>0)
					{
						std::ifstream sf(data,std::ios::binary);
						if (!sf.is_open()){std::cout<<"File dont exist\n";ser<<"File dont exist"<<std::endl;}
						else
						{
							sf.seekg(0,std::ios::end);
							size_t size_f=sf.tellg();
							std::vector<char> cs(size_f);
							
							sf.seekg(0,std::ios::beg);
							sf.read(reinterpret_cast<char*>(cs.data()),cs.size());
							sf.close();
						
							send(client_sock,(const char*) &size_f,size_f,0);
							send(client_sock,cs.data(),cs.size(),0);
						}
					}
				}
			}
		}
		catch(std::exception &e){std::cout<<"Error client: "<<e.what()<<std::endl;file<<"Error client: "<<e.what()<<std::endl;}
	}
	close(client_sock);
	std::cout<<"Client disconnected\n";
	ser<<"Client disconnected"<<std::endl;
}

void terminal(bool &run)
{
	while (true)
	{
		std::string cmd;
		std::cout<<'>';
		getline(std::cin,cmd);
		if (cmd=="exit"){run=false;break;}
	}
}

int main()
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(3580);
	addr.sin_addr.s_addr=INADDR_ANY;
	
	bind(sock,(struct sockaddr*) &addr,sizeof(addr));

	listen(sock,8);
	
	bool run=true;
	std::ofstream errs("err.txt",std::ios::app);
	std::ofstream server_work("server_work.txt");
	std::cout<<"Server start\n";
	server_work<<"Server start"<<std::endl;

	while (run)
	{
		try
		{
			int client_acc=accept(sock,nullptr,nullptr);
			std::cout<<"Client connected\n";server_work<<"Client connected"<<std::endl;
			std::thread(client_conn,client_acc,ref(errs),ref(server_work)).detach();
		}
		catch(std::exception &err){std::cout<<"Error client conn: "<<err.what()<<std::endl;errs<<"Error client conn: "<<err.what()<<std::endl;}
	}
	close(sock);
	std::cout<<"Server was stopped\n";
	server_work<<"Server was stopped"<<std::endl;
	errs.close();
	server_work.close();
	return 0;
}
