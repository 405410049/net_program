#include"net_header.h"
int main(int argc,char *argv[])
{
	int file_size,sockfd,n;
	FILE *fd;
	struct sockaddr_in servaddr;
	char buf[1024],mes[1024];
	if(argc!=2){
		fprintf(stderr,"usage: cli <IPaddress>\n");
		exit(1);
	}
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(8082);
	inet_pton(AF_INET,argv[1],&servaddr.sin_addr);	//argv[1] is IP address
	if(connect(sockfd,(SA *)&servaddr,sizeof(servaddr))==-1)
		perror("connect error");

	while(n=read(sockfd,mes,sizeof(mes))>0)	
	{
		printf("%s\n",mes);
		printf(">>");
		if(strstr(mes,"receive")!=NULL)
			continue;
		fgets(buf,1024,stdin);		//input
		if(strstr(mes,"whether")!=NULL)
		{
			if(strstr(buf,"No")!=NULL)
				continue;
		//	write(sockfd,buf,sizeof(buf));
			read(sockfd,buf,sizeof(buf));	//read file size
			file_size=atoi(buf);
			printf("file_size :%d\n",file_size);
			fd=fopen("recv.txt","w");
			while(file_size>0)
			{
				n=read(sockfd,buf,sizeof(buf));
				file_size-=n;
				printf("reading the file\n");
				fprintf(fd,"%s",buf);
			}
	//		read(sockfd,buf,sizeof(buf));
			continue;
		}
		if(write(sockfd,buf,strlen(buf))==-1)
			perror("write error");
	}
//	fclose(fd);
	return 0;
}
