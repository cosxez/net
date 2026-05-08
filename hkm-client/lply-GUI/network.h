#pragma once

void sclose(int *sock)
{
	if (*sock<0){return;}
	else{close(*sock);*sock=-1;}
}
short lply_sbatt(int *sock,struct sockaddr_in *addr,struct sockaddr_in *faddr,std::string ip,unsigned short port)
{
	try
	{
		addr->sin_family=AF_INET;
		addr->sin_port=htons(port);
		addr->sin_addr.s_addr=INADDR_ANY;
	
		bind(*sock,(struct sockaddr*)addr,sizeof(*addr));
	
		faddr->sin_family=AF_INET;
		faddr->sin_port=htons(port);
	
		inet_pton(AF_INET,ip.c_str(),&(faddr->sin_addr));
	}
	catch(std::exception &e){return -1;}
	return 0;
}

int lply_read(int *sock,struct sockaddr_in *faddr,void* pbuff,size_t sbuff)
{
	struct sockaddr_in ffaddr;
	socklen_t ips=sizeof(ffaddr);
	int sb=-1;
	for (unsigned short i=0;i<32;i++)
	{
		sb=recvfrom(*sock,pbuff,sbuff,0,(struct sockaddr*)&ffaddr,&ips);
		if (sb>0 && ffaddr.sin_addr.s_addr==faddr->sin_addr.s_addr){break;}
	}
	unsigned short mgc_chk=*(unsigned short*)pbuff;
	if (sb==2 && mgc_chk==0xe1dd){sb=-4;}
	return sb;
}

void lply_lconn(int *sock,struct sockaddr_in *faddr,char *is_conn)
{
	struct sockaddr_in ffaddr;
	socklen_t ips=sizeof(ffaddr);
	for (unsigned int i=0;i<32;i++)
	{
		unsigned short mgc;
		recvfrom(*sock,&mgc,sizeof(mgc),0,(struct sockaddr*)&ffaddr,&ips);
		if (ffaddr.sin_addr.s_addr==faddr->sin_addr.s_addr &&mgc==0xAC5F){*is_conn=1;break;return;}
	}
}

short lply_connect(int *sock,struct sockaddr_in *faddr)
{
	char is_conn=0;
	std::thread(lply_lconn,sock,faddr,&is_conn).detach();
	unsigned short mgc=0xC00F;
	for (unsigned short i=0;i<512;i++)
	{
		if (is_conn==1){break;}
		sendto(*sock,&mgc,sizeof(mgc),0,(struct sockaddr*)faddr,sizeof(*faddr));
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	if (is_conn==1){return 0;}
	return -1;
}
