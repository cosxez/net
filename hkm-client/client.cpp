#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>
#include <thread>
#include <chrono>
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
			int sock=socket(AF_INET,SOCK_STREAM,0);
			
			sockaddr_in addr;

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
					unsigned char buffer[2048];

					if (is_ar==true)
					{
						std::string fhead="t.n.c";
						send(sock,fhead.c_str(),fhead.size(),0);
						int sb=recv(sock,buffer,sizeof(buffer)-1,0);
						buffer[sb]='\0';
						for (int i=0;i<sb;i++){std::cout<<buffer[i];}

						while (true)
						{
							std::string tcmd;
							while (tcmd.empty()){std::cout<<"ar-t-terminal>";getline(std::cin,tcmd);}
							if (tcmd=="exit"){close(sock);break;}
							if (tcmd.size()>4 && tcmd[0]=='p' && tcmd[1]=='u' && tcmd[2]=='s' && tcmd[3]=='h')
							{
								send(sock,"push",4,0);

								std::string spath;
								int tcpos;
								for (int i=5;tcmd[i]!=' ';i++){if (i>=tcmd.size()){break;};spath+=tcmd[i];tcpos=i;}
								send(sock,spath.c_str(),spath.size(),0);
								std::this_thread::sleep_for(std::chrono::seconds(1));

								std::string cpath;
								tcpos+=2;for (tcpos;tcpos<tcmd.size();tcpos++){cpath+=tcmd[tcpos];}
								
								std::ifstream file(cpath);
								if (file.is_open())
								{
									file.seekg(0,std::ios::end);
									size_t fs=file.tellg();
									file.seekg(0,std::ios::beg);
									
									std::vector<char> fld(fs);
									send(sock,&fs,sizeof(fs),0);
									std::this_thread::sleep_for(std::chrono::seconds(1));

									file.read(reinterpret_cast<char*>(fld.data()),fld.size());
									file.close();
									
									send(sock,fld.data(),fld.size(),0);
									
									sb=recv(sock,buffer,sizeof(buffer)-1,0);
									buffer[sb]='\0';
									for (int i=0;i<sb;i++){std::cout<<buffer[i];}
								}
								else{std::cout<<"file dont open\n";}
							}

							if (tcmd.size()>3 && tcmd[0]=='g' && tcmd[1]=='f' && tcmd[2]=='l')
							{
								send(sock,"gfl",3,0);
								std::string spath;
								for (int i=4;i<tcmd.size();i++){spath+=tcmd[i];}
								
								std::this_thread::sleep_for(std::chrono::seconds(1));
								send(sock,spath.c_str(),spath.size(),0);
								sb=recv(sock,buffer,sizeof(buffer)-1,0);
								std::string str;for (int i=0;i<sb;i++){str+=buffer[i];}
								std::cout<<str<<std::endl;
							}

							if (tcmd.size()>3 && tcmd[0]=='g' && tcmd[1]=='e' && tcmd[2]=='t')
							{
								send(sock,"get",3,0);

								std::string spath;
								int tcpos;
								for (int i=4;tcmd[i]!=' ';i++){if (i>=tcmd.size()){break;};spath+=tcmd[i];tcpos=i;}
								send(sock,spath.c_str(),spath.size(),0);
								std::this_thread::sleep_for(std::chrono::milliseconds(100));

								std::string cpath;
								tcpos+=2;for (tcpos;tcpos<tcmd.size();tcpos++){cpath+=tcmd[tcpos];}

								size_t fs=0;
								recv(sock,&fs,sizeof(fs),0);
								std::vector<char> fld(fs);
								sb=0;
								while (sb<fs){sb+=recv(sock,fld.data()+sb,fld.size(),0);}

								std::ofstream file(cpath);
								file.write(reinterpret_cast<const char*>(fld.data()),fld.size());
								file.close();
								std::cout<<"Done\n";
							}
						}
					}
					if (is_ar==false)
					{
						while (true)
						{
							std::string tcmd;
							while (tcmd.empty()){std::cout<<"Enter data for tcp server(tcpclose for close connect): ";getline(std::cin,tcmd);}
							if (tcmd=="tcpclose"){break;}
							send(sock,tcmd.c_str(),tcmd.size(),0);
						}
					}
				}
				catch(std::exception &e){std::cout<<"Error: "<<e.what()<<std::endl;close(sock);}
			}
			close(sock);
		}
	}
	return 0;
}
