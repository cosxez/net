#pragma once

void cfg_pars(char str[],std::string *ip_addr, unsigned short *port)
{
	for (unsigned short i=1;i<2048;i++)
	{
		if (str[i]=='p' &&str[i-1]=='i')
		{
			i+=2;
			unsigned char j=0;
			for (j;str[i]!='\n';i++)
			{
				if (i>=2048){break;}
				if (j>=64){break;}
				*ip_addr+=str[i];
				j++;
			}
		}
		if (str[i]=='t'&&str[i-1]=='p')
		{
			i+=2;
			unsigned char j=0;
			std::string npt;
			for (j;str[i]!='\n';i++)
			{
				if (i>=2048){break;}
				if (j>=6){break;}
				npt+=str[i];
				j++;
			}
			*port=std::stoi(npt);
		}
	}
}
