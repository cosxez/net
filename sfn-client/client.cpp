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
							size_t flss;
							stp=recv(sock,&flss,sizeof(flss),0);
							std::vector<std::string> fls;

							stp=0;
							while (stp<flss)
							{
								size_t crtp=recv(sock,buffer,sizeof(buffer),0);
								stp+=crtp;
								buffer[crtp]='\0';
								std::string lspw;
								for (int j=0;j<crtp;j++){lspw+=buffer[j];}
								fls.push_back(lspw);
							}
							for (auto &c:fls){std::cout<<c<<std::endl;}
							

							std::string fln;
							while (true)
							{
								std::cout<<"Enter filename(if you want download, \"exit\" - for close connect): ";
								getline(std::cin,fln);
								
								if (fln=="exit"){std::string cl_conn="close";send(sock,cl_conn.c_str(),cl_conn.size(),0);break;}
								if (!fln.empty())
								{
									bool file_ex=false;
									for (int i=0;i<fls.size();i++){if (fln==fls[i]){file_ex=true;}}
									if (file_ex==true)
									{
										std::string endline="GET DATA:"+fln;
										send(sock,endline.c_str(),endline.size(),0);
										size_t fs;
										recv(sock,&fs,sizeof(fs),0);
										std::vector<char> fs_data(fs);
										size_t fsds=0;
										while (fsds<fs_data.size())
										{
											fsds+=recv(sock,fs_data.data()+fsds,fs_data.size(),0);
										}
	
										std::ofstream file(fln,std::ios::binary);
										file.write(reinterpret_cast<const char*>(fs_data.data()),fs_data.size());
										file.close();
									}
								}
								else{std::string cl_conn="close";send(sock,cl_conn.c_str(),cl_conn.size(),0);}
							}
						}
						if (stp==1 && buffer[0]=='0'){std::cout<<"URL error: site doesnt exist\n";}
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
				catch(std::exception &e){std::cout<<"Error: "<<e.what()<<std::endl;}
				shutdown(sock,SHUT_RDWR);
			}
		}
	}
	close(sock);
	return 0;
}
