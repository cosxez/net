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
						sb=recv(client,buffer,sizeof(buffer)-1,0);
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
							for (int i=0;i<fls.size();i++){imgcard+="<div class=\"img-card\"><img src=\"" + fls[i] + "\"><div class=\"img-info\"><span>cvnt</span><b>" + fls[i] + "</b></div></div>";}
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

							for (auto &c:std::filesystem::directory_iterator("hkim_data")){fls.push_back(c.path().filename().string());}

							std::string musiccard="<body><div class=\"container\"><aside><div class=\"logo\">hkim</div><nav><a href=\"#\" class=\"active\">Home</a><a href=\"#\">Search</a><a href=\"#\">Your Library</a><br><a href=\"#\">Create Playlist</a><a href=\"#\">Liked Songs</a></nav></aside><main><div class=\"section-title\">New Releases</div><div class=\"sfn-grid\">";
							
							for (int i=0;i<fls.size();i++)
							{
								std::string songname="";
								std::string authorsong="";
								unsigned short curposfn=0;
								for (int j=0;fls[i][j]!='_';j++){songname+=fls[i][j];curposfn+=1;}
								curposfn+=1;
								for (curposfn;fls[i][curposfn]!='.';curposfn++){authorsong+=fls[i][curposfn];}
								musiccard+="<div class=\"track-card\" onclick=\"playTrack(\'" + songname + "\', \'" + authorsong + "\', \'/get_audio?file=" + fls[i] + "\', this)\">" + "<img src=\"/hkim_data/" + songname + '_' + authorsong + ".jpg\">" + "<span class=\"track-name\">" + songname + "</span>" + "<span class=\"artist-name\">" + authorsong + "</span></div>";
							}
							musiccard+="</div></main><footer><div class=\"now-playing\"><img src=\"\" id=\"current-cover\" style=\"display:none;\"><div><div class=\"track-name\" id=\"current-title\">Select Track</div><div class=\"artist-name\" id=\"current-artist\">-</div></div></div><div class=\"player-controls\"><div class=\"btns\"><span onclick=\"document.getElementById('audio').currentTime -= 10\">«</span><span id=\"play-btn\" onclick=\"togglePlay()\">►</span><span onclick=\"document.getElementById('audio').currentTime += 10\">»</span></div><div class=\"progress-container\" onclick=\"seek(event)\"><div id=\"progress\"></div></div></div><div style=\"text-align: right; font-size: 10px; letter-spacing: 2px;\">VOL <input type=\"range\" min=\"0\" max=\"1\" step=\"0.1\" oninput=\"document.getElementById('audio').volume = this.value\" style=\"width: 80px; vertical-align: middle; filter: grayscale(1);\"></div></footer></div><audio id=\"audio\" ontimeupdate=\"updateProgress()\"></audio><script> const audio = document.getElementById('audio'); const playBtn = document.getElementById('play-btn'); const progress = document.getElementById('progress'); function playTrack(title, artist, src, element) { audio.src = src; document.getElementById('current-title').innerText = title; document.getElementById('current-artist').innerText = artist; const cover = document.getElementById('current-cover'); cover.src = element.querySelector('img').src; cover.style.display = 'block'; audio.play(); playBtn.innerText = '█'; } function togglePlay() { if (audio.paused) { audio.play(); playBtn.innerText = '█'; } else { audio.pause(); playBtn.innerText = '►'; } } function updateProgress() { const percent = (audio.currentTime / audio.duration) * 100; progress.style.width = percent + '%'; } function seek(e) { const width = e.target.clientWidth; const clickX = e.offsetX; const duration = audio.duration; audio.currentTime = (clickX / width) * duration; } </script></body></html>";
							std::string headhttp="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
							headhttp+=std::to_string(line_index.size() + musiccard.size()) + "\r\nConnection: close\r\n\r\n" + line_index + musiccard;
							
							send(client,headhttp.c_str(),headhttp.size(),0);
							close(client);std::cout<<"Client dissconnected\n";return;
						}
						catch(std::exception &er){std::cout<<"Error: "<<er.what()<<std::endl;}
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
