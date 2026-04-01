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
	char buffer[2048];
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
				for (int i=0;i<sb;i++){str+=buffer[i];}
				if (sb>0)
				{
					if (str=="t.n.c")
					{
						std::string tcommands="push - push <server path> <client path>\nget - get <server path> <client path>\n";
						try
						{
							send(client,tcommands.c_str(),tcommands.size(),0);
							while (true)
							{
								sb=recv(client,buffer,sizeof(buffer)-1,0);
								if (sb>0)
								{
									buffer[sb]='\0';
									str="";
									for (int i=0;i<sb;i++){str+=buffer[i];}
									if (str=="push")
									{
										sb=recv(client,buffer,sizeof(buffer)-1,0);
										buffer[sb]='\0';

										std::string spath;
										for (int i=0;i<sb;i++){spath+=buffer[i];}

										size_t fs=0;
										recv(client,&fs,sizeof(fs),0);
										std::vector<char> fld(fs);
										sb=0;
										while (sb<fs){sb+=recv(client,fld.data()+sb,fld.size(),0);}
										
										std::ofstream file(spath);
										if (file.is_open())
										{
											file.write(reinterpret_cast<const char*>(fld.data()),fld.size());
											file.close();
											send(client,"file added\n",11,0);
										}
									}
									if (str=="get")
									{
										sb=recv(client,buffer,sizeof(buffer)-1,0);
										buffer[sb]='\0';

										std::string spath;
										for (int i=0;i<sb;i++){spath+=buffer[i];}

										std::ifstream file(spath);
										if (file.is_open())
										{
											file.seekg(0,std::ios::end);
											size_t fs=file.tellg();
											file.seekg(0,std::ios::beg);

											send(client,&fs,sizeof(fs),0);
											std::vector<char> fld(fs);
											file.read(reinterpret_cast<char*>(fld.data()),fld.size());
											file.close();

											std::this_thread::sleep_for(std::chrono::milliseconds(100));
											send(client,fld.data(),fld.size(),0);
										}
									}
								}
								else{close(client);break;}
							}
						}
						catch(std::exception &er){std::cout<<"Error: "<<er.what()<<std::endl;}
					}
				}
				if (sb>0)
				{
					str="";
					for (int i=0;i<14;i++){str+=buffer[i];}
					if (str=="GET / HTTP/1.1")
					{
						try
						{
							std::ifstream hsearchf("search_main.html");
							std::string linef="";
							
							if (hsearchf.is_open())
							{
								while (!hsearchf.eof())
								{
									std::string cl;
									getline(hsearchf,cl);
									linef+=cl;
								}
							}
							hsearchf.close();

							std::vector<std::string> sites={"inetpic","hkim"};

							std::string list_s="<body><div class=\"main-frame\"><header><h1>SITES LIST</h1></header><nav class=\"nav-list\">";
							for (int i=0;i<sites.size();i++){list_s+="<a href=\"/" + sites[i] + "\" class=\"nav-item\"><b>" + sites[i] + "</b></a>";}
							list_s+="</nav></div></body></html>";
							std::string headhttp="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(linef.size() + list_s.size()) + "\r\nConnection: close\r\n\r\n" + linef + list_s;
							
							send(client,headhttp.c_str(),headhttp.size(),0);
						}
						catch(std::exception &er){std::cout<<"Error: "<<er.what();}
					}
				}
				if (sb>0)
				{
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
							
							std::vector<std::string> fls;
							for (auto &c:std::filesystem::directory_iterator("inetpic_data")){fls.push_back(c.path().filename().string());}
							
							std::string imgcard="<body><div class=\"main-frame\"><header><h1>惡̶̉̄͆̿̓̋́͊̾̈̑́̈́̾̿̌̈̕͘͝͝͠ ̸̛̛͆͗̿̔͋͑͊̾͂͂̀̀̉̃̀̀͆͘͘͝é̵̾͊͂͊͗̆̋͊͐̀͐͛̎̂̊͗̒͘̚͘̚v̸̛̈́͌̔̐̎̎̊̈́͛́̏͌́̇̃̏̚͝͠͠͠ḯ̴͗̉̒̔̅͌̂̆̑̍͗̀́̌̆̈̔́̓̐̚l̷̛̄̊̋̈́̏̾͐̇̄͛̒́̑̾̐̋̂̆͘͠͝ ̶̛̛̆͛́̓̈́͂͌͑̒̉͐̀̀͋̀̚̚̕̚͝Ų̴̲̤̭̩͔̤̗̱̙̻̹̭̐͛̽͋̐̓̇̕͝T̴̈́̒̔̒͌̓̐͗̏̊̓̀̎̒̀̏̀͗̿̚͠͠F̶̈́̍́̔͒̌͗̈́̽͒̊̋̆̉͆̾̐͑̔̚̚͝-̸̃͗̀̓̇̆̍̆͆̄͗̓̈́̆͌͐́̀̊͐͌͝8̷̎̈̃̉͑̓̆̑̉̔̑͒͒̅͋̆̃̈́̄̔͘͝惡̴͊͋͗̎̽̌̽̉͂̓̊̅͋̈̍͆͌̈́̽͘͠͝ ̸̆̈́́̂̍͛́͗̇̀̈͒̿̓̓̎̀̎̈́̚̕͝惡̶̉̄͆̿̓̋́͊̾̈̑́̈́̾̿̌̈̕͘͝͝͠ ̸̛̛͆͗̿̔͋͑͊̾͂͂̀̀̉̃̀̀͆͘͘͝é̵̾͊͂͊͗̆̋͊͐̀͐͛̎̂̊͗̒͘̚͘̚v̸̛̈́͌̔̐̎̎̊̈́͛́̏͌́̇̃̏̚͝͠͠͠ḯ̴͗̉̒̔̅͌̂̆̑̍͗̀́̌̆̈̔́̓̐̚l̷̛̄̊̋̈́̏̾͐̇̄͛̒́̑̾̐̋̂̆͘͠͝</h1></header><div class=\"gallery-grid\">";
							for (int i=0;i<fls.size();i++){imgcard+="<div class=\"img-card\"><img src=\"inetpic_data/" + fls[i] + "\"><div class=\"img-info\"><span>cvnt</span><b>" + fls[i] + "</b></div></div>";}
							imgcard+="</div></div></body></html>";
	
							std::string headhttp="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
						      	headhttp+=std::to_string(lines_index.size() + imgcard.size()) + "\r\nConnection: close\r\n\r\n" + lines_index + imgcard;
							
							send(client,headhttp.c_str(),headhttp.size(),0);
							close(client);std::cout<<"Client dissconnected\n";return;
						}
						catch(std::exception &er){std::cout<<"Error: "<<er.what()<<std::endl;}
					}
					str="";
					for (int i=0;i<18;i++){str+=buffer[i];}
					if (str=="GET /hkim HTTP/1.1")
					{
						try
						{
							std::ifstream hindexf("hkim.html");
							std::string line_index="";
							while (!hindexf.eof())
							{
								std::string cl;
								getline(hindexf,cl);
								line_index+=cl;
							}
							hindexf.close();

							std::vector<std::string> fls;
							std::vector<std::string> drs;

							for (auto &c:std::filesystem::directory_iterator("hkim_data"))
							{
								if (std::filesystem::is_directory(c))
								{
									drs.push_back(c.path().filename().string());
								}
								else
								{
									fls.push_back(c.path().filename().string());
								}
							}

							std::string musiccard="<body><div class=\"container\"><aside><div class=\"logo\">hkim</div><nav><a href=\"#\" class=\"active\">Home</a><a href=\"#\">Search</a><a href=\"#\">Your Library</a><br><a href=\"#\">Create Playlist</a><a href=\"#\">Liked Songs</a></nav></aside><main><div class=\"section-title\">New Releases</div><div class=\"sfn-grid\">";
							
							for (int i=0;i<fls.size();i++)
							{
								std::string songname="";
								std::string authorsong="";
								std::string imgname="";
								unsigned short curposfn=0;
								for (int j=0;fls[i][j]!='_';j++){songname+=fls[i][j];curposfn+=1;if (fls[i][j]=='.'){break;};if (j>=fls[i].size()){break;}}
								curposfn+=1;
								if (curposfn<fls[i].size()){for (curposfn;fls[i][curposfn]!='_';curposfn++){authorsong+=fls[i][curposfn];if (fls[i][curposfn]=='.' || fls[i][curposfn]=='_'){authorsong.pop_back();break;};if (curposfn>=fls[i].size()){break;}}}
								curposfn+=1;
								if (curposfn<fls[i].size()){for (curposfn;fls[i][curposfn]!='.';curposfn++){imgname+=fls[i][curposfn];if (curposfn>=fls[i].size()){break;}}}
									
								musiccard+="<div class=\"track-card\" onclick=\"playTrack(\'" + songname + "\', \'" + authorsong + "\', \'/get_audio?file=" + fls[i] + "\', this)\">" + "<img src=\"/hkim_data_img/" + imgname + ".jpg\">" + "<span class=\"track-name\">" + songname + "</span>" + "<span class=\"artist-name\">" + authorsong + "</span></div>";
							}
							
							for (int i=0;i<drs.size();i++)
							{
								std::string pllimg="0";
								for (auto &c:std::filesystem::directory_iterator("hkim_data/"+drs[i]))
								{
									std::string checkimg=c.path().filename().string();
									if (checkimg.size()>4 && checkimg[checkimg.size()-1]=='g' && checkimg[checkimg.size()-2]=='p' && checkimg[checkimg.size()-3]=='j' && checkimg[checkimg.size()-4]=='.')
									{
										pllimg=checkimg;
									}
								}	
								musiccard+="<div class=\"track-card\" onclick=\"window.location.href=\'hkim/playlists/"+ drs[i] + "\'\">" + "<img src=\"hkim_data/" +drs[i] + '/' + pllimg + "\">" + "<span class=\"track-name\">" + drs[i] + "</span><span class=\"artist-name\">" + "playlist" + "</span></div>";
							}	

							std::ifstream jsindexf("jshkim.html");
							while (!jsindexf.eof()){std::string cl;getline(jsindexf,cl);musiccard+=cl;}
							jsindexf.close();

							std::string headhttp="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
							headhttp+=std::to_string(line_index.size() + musiccard.size()) + "\r\nConnection: close\r\n\r\n" + line_index + musiccard;
							
							send(client,headhttp.c_str(),headhttp.size(),0);
							close(client);std::cout<<"Client dissconnected\n";return;
						}
						catch(std::exception &er){std::cout<<"Error: "<<er.what()<<std::endl;}
					}
					str="";
					for (int i=0;i<20;i++){str+=buffer[i];}
					if (str=="GET /hkim/playlists/")
					{
						for (int i=20;buffer[i]!=' ';i++){str+=buffer[i];}
						for (auto &c:std::filesystem::directory_iterator("hkim_data"))
						{
							if (std::filesystem::is_directory(c))
							{
								if ((str + " HTTP/1.1")==("GET /hkim/playlists/" + c.path().filename().string() + " HTTP/1.1"))
								{
									std::ifstream hindexf("hkim.html");
									std::string line_index="";
									while (!hindexf.eof())
									{
										std::string cl;
										getline(hindexf,cl);
										line_index+=cl;
									}
									hindexf.close();
	
									std::vector<std::string> fns;
									for (auto &b:std::filesystem::directory_iterator("hkim_data/" + c.path().filename().string()))
									{
										if (!std::filesystem::is_directory(b))
										{
											std::string is_img_e="";
											for (int i=b.path().filename().string().size()-1;i>b.path().filename().string().size()-5;i--){is_img_e+=b.path().filename().string()[i];}
											if (is_img_e!="gpj.")
											{
												fns.push_back(b.path().filename().string());
											}
										}
									}
	
									std::string plmd="<body><div class=\"container\"><aside><div class=\"logo\">hkim</div><nav><a href=\"/hkim\" class=\"active\">Home</a><a href=\"#\">Search</a><a href=\"#\">Your Library</a><br><a href=\"#\">Create Playlist</a><a href=\"#\">Liked Songs</a></nav></aside><main><div class=\"section-title\">playlist " + c.path().filename().string() + "</div><div class=\"sfn-grid\">";
	
									for (int j=0;j<fns.size();j++)
									{
										std::string songname="";
										std::string authorsong="";
										std::string imgname="";
										unsigned short curposfn=0;
										for (int u=0;fns[j][u]!='_';u++){songname+=fns[j][u];curposfn+=1;if (fns[j][u]=='.'){break;};if (u>=fns[j].size()){break;}}
										curposfn+=1;
										if (curposfn<fns[j].size()){for (curposfn;fns[j][curposfn]!='_';curposfn++){authorsong+=fns[j][curposfn];if (fns[j][curposfn]=='.' || fns[j][curposfn]=='_'){authorsong.pop_back();break;};if (curposfn>=fns[j].size()){break;}}}
										curposfn+=1;
										if (curposfn<fns[j].size()){for (curposfn;fns[j][curposfn]!='.';curposfn++){imgname+=fns[j][curposfn];if (curposfn>=fns[j].size()){break;}}}
	
										plmd+="<div class=\"track-card\" onclick=\"playTrack(\'" + songname + "\', \'" + authorsong + "\', \'/get_audio?file=" + c.path().filename().string() + '/' + fns[j] + "\', this)\">" + "<img src=\"/hkim_data_img/" + imgname + ".jpg\">" + "<span class=\"track-name\">" + songname + "</span>" + "<span class=\"artist-name\">" + authorsong + "</span></div>";
									}
									std::ifstream jsindexf("jshkim.html");
									while (!jsindexf.eof())
									{
										std::string cl;
										getline(jsindexf,cl);
										plmd+=cl;
									}
									std::string pllpage="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
	
									pllpage+=std::to_string(line_index.size() + plmd.size()) + "\r\nConnection: close\r\n\r\n" +line_index+plmd;
									send(client,pllpage.c_str(),pllpage.size(),0);
									close(client);std::cout<<"Client dissconnected\n";return;
								}
							}
						}
					}
				}
				if (sb>0)
				{
					str="";
					for (int i=0;i<20;i++){str+=buffer[i];}
					if (str=="GET /get_audio?file=")
					{
						try
						{
							str="";
							int indxpos=20;
							while (indxpos<=sb)
							{
								data="";
								for (int i=indxpos-8;i<indxpos;i++)
								{
									data+=buffer[i];
								}
								if (data=="HTTP/1.1"){break;}
								str+=buffer[indxpos];
								indxpos++;
							}
							data="";
							for (int i=0;i<str.size()-9;i++){data+=str[i];}
							if (!str.empty())
							{
								std::ifstream file("hkim_data/" + data,std::ios::binary);
								if (file.is_open())
								{
									file.seekg(0,std::ios::end);
									size_t fs=file.tellg();
									file.seekg(0,std::ios::beg);

									std::vector<char> fld(fs);
									file.read(reinterpret_cast<char*>(fld.data()),fld.size());
									file.close();
									data="HTTP/1.1 200 OK\r\nContent-Type: audio/mpeg\r\nContent-Length: " + std::to_string(fs) + "\r\nConnection: close\r\n\r\n";

									send(client,data.c_str(),data.size(),0);
									send(client,fld.data(),fld.size(),0);
									close(client);std::cout<<"Client dissconnected\n";return;
								}
								else
								{
									data="HTTP/1.1 404 Not Found\r\n\r\n";
									send(client,data.c_str(),data.size(),0);
									close(client);std::cout<<"Client dissconnected\n";return;
								}
							}
						}
						catch(std::exception &er){std::cout<<"Error: "<<er.what()<<std::endl;}
					}

					str="";
					for (int i=0;buffer[i]!=' ';i++){str+=buffer[i];}
					if (str=="GET")
					{
						try
						{
							str="";
							int indxpos=0;
							for (int i=5;buffer[i]!='/';i++){str+=buffer[i];indxpos=i;}
							
							data="";
							indxpos+=1;
							for (int i=indxpos;buffer[i]!=' ';i++){data+=buffer[i];}
							if (!str.empty())
							{
								std::ifstream file(str + data,std::ios::binary);
								if (file.is_open())
								{
									file.seekg(0,std::ios::end);
									size_t fs=file.tellg();
									file.seekg(0,std::ios::beg);
		
									std::vector<char> fld(fs);
									file.read(reinterpret_cast<char*>(fld.data()),fld.size());
									file.close();
									data="HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nContent-Length: " + std::to_string(fs) + "\r\nConnection: close\r\n\r\n";
									send(client,data.c_str(),data.size(),0);
									send(client,fld.data(),fld.size(),0);
									close(client);std::cout<<"Client dissconnected\n";return;
								}
								else
								{
									data="HTTP/1.1 404 Not Found\r\n\r\n";
									send(client,data.c_str(),data.size(),0);
									close(client);std::cout<<"Client dissconnected\n";return;
								}
							}
						}
						catch(std::exception &er){std::cout<<"Error: "<<er.what()<<std::endl;}
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
