#include"net_header.h"
#include<pthread.h>
int SW,accept_fd;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
void accept_file(void)
{
	SW=1;
	while(SW!=-1);
}
void reject_file(void)
{
	SW=0;
}
int check_account(char pat[],FILE *fd)
{
	char line[1024];
	char *ptr;
	while(fgets(line,1024,fd)!=NULL)
		if(strncmp(line,pat,strlen(pat)-1)==0)
				return 1;
	return 0;
}
static void *doit(void *arg)
{
	int file_size,remaind_data,i,cmd=0,flag=1,find,connect_fd,n,offset;
        FILE *file_fd,*send_fd;
	char *ptr,account[256],mes[1024],buf[1024],mes2[1024];
	struct stat file_stat;
	users *p;
	connect_fd = *((int *) arg);
	free(arg);
	strcpy(mes,"Please input your username and password:");
	send(connect_fd,mes,sizeof(mes),0);
	if(read(connect_fd,account,sizeof(account))<0)	//接受帳號
		fprintf(stderr,"read failed\n");	
	if((file_fd=fopen("account.txt","r+"))==NULL)
		fprintf(stderr,"file open failed\n");
	if(check_account(account,file_fd)==1) //此帳戶存在	
	{	
		printf("account exists\n");
		strcpy(mes,"Login success\nSelect the command (1)List (2)Send to all users (3)Send the message to the users who you want (4)Send the file to user (0)Exit");
		send(connect_fd,mes,sizeof(mes),0);
		ptr=strtok(account," ");	//將username取出
		pthread_mutex_lock(&mutex);
		strcat(ptr,"\n");
		add_list(ptr,connect_fd);
		pthread_mutex_unlock(&mutex);
		
		//判斷使用者輸入command,以及各指令處理
		while(flag)
		{
		read(connect_fd,buf,sizeof(buf));
		if(strncmp(buf,"Yes",3)==0)
		{
			accept_file();
			continue;
		}
		if(strncmp(buf,"No",2)==0)
		{
			reject_file();
			continue;
		}
		cmd=buf[0]-'0';
		switch(cmd)
		{
			//show all users
			case 1:
				p=head;
				bzero(buf,sizeof(buf));
				while(p!=NULL)
				{
					strcat(buf,p->username);
					p=p->next;
				}
				sprintf(mes,"All users\n%sSelect the command (1)List (2)Send to all users (3)Send the message to the users who you want (4)Send the file to user (0)Exit",buf);
				send(connect_fd,mes,sizeof(mes),0);	
				break;
			//send message to all users
			case 2:
				strcpy(mes,"Send message to all users");
				write(connect_fd,mes,sizeof(mes));
				read(connect_fd,mes2,sizeof(mes2));
				p=head;
				while(p!=NULL)
				{	
					if(p->fd!=connect_fd)
					{
						sprintf(mes,"You receive the message :%sSelect the command (1)List (2)Send to all users (3)Send the message to the users who you want (4)Send the file to user (0)Exit",mes2);
						write(p->fd,mes,sizeof(mes));
					}
					else
					{
						sprintf(mes,"You send the maeesage to all users:%sSelect the command (1)List (2)Send to all users (3)Send the message to the users who you want (4)Send the file to user (0)Exit",mes2);
						write(p->fd,mes,sizeof(mes));
					}
					p=p->next;
				}
				break;
			case 3:
				find=0;
				strcpy(mes,"Send message to the indicated users\nwhich user do you choose");
				write(connect_fd,mes,sizeof(mes));
				read(connect_fd,account,sizeof(account));//接受使用者要指定傳給哪個使用者
				p=head;
				while(p!=NULL){
					if(strcmp(p->username,account)==0){
						find=1;
						break;
					}
					p=p->next;
				}
				if(find){
					strcpy(mes,"Message content :");
					write(connect_fd,mes,sizeof(mes));
					read(connect_fd,mes2,sizeof(mes2));
					sprintf(mes,"You send the message to %sContent :%sSelect the command (1)List (2)Send to all users (3)Send the message to the users who you want (4)Send the file to user (0)Exit",p->username,mes2);
					write(connect_fd,mes,sizeof(mes));
					sprintf(mes,"You receive the message :%sSelect the command (1)List (2)Send to all users (3)Send the message to the users who you want (4)Send the file to user (0)Exit",mes2);
					write(p->fd,mes,sizeof(mes));
				}
				else{
					strcpy(mes,"No this user\nelect the command (1)List (2)Send to all users (3)Send the message to the users who you want (4)Send the file to user (0)Exit");
					write(connect_fd,mes,sizeof(mes));
				}	
				break;
			case 4:
				find=0;
				strcpy(mes,"Send file to the indicated users\nwhich user do you choose");
				write(connect_fd,mes,sizeof(mes));
				read(connect_fd,account,sizeof(account));//接受使用者要指定傳給哪個使用者
				p=head;
				while(p!=NULL){
					if(strcmp(p->username,account)==0){
						find=1;
						break;
					}
					p=p->next;
				}
				if(find)
				{	
					strcpy(mes,"Please input the file name:");
					write(connect_fd,mes,sizeof(mes));
					read(connect_fd,mes2,sizeof(mes2));	//mes2 is the file name
					mes2[strlen(mes2)-1]='\0';
					if((send_fd=fopen(mes2,"r"))==NULL){
						strcpy(mes,"No this file\nSelect the command (1)List (2)Send to all users (3)Send the message to the users who you want (4)Send the file to user (0)Exit");
						write(connect_fd,mes,sizeof(mes));
						break;
					}
					/*the user whether accepct the file*/
					strcpy(mes,"whether accept the file\nPlease input Yes or No ");
					write(p->fd,mes,sizeof(mes));
			//		SW=1;
					while(SW==-1);
			//		if(read(p->fd,buf,sizeof(buf))<0)
			//		fprintf(stderr,"read error\n");			
			//		printf("The acceptor says :%s",buf);
			//		if(strncmp(buf,"Yes",3)==0)	//對方接受檔案
					if(SW)
					{	
						sprintf(mes,"You send the file :%s\nTo :%sSelect the command (1)List (2)Send to all users (3)Send the message to the users who you want (4)Send the file to user (0)Exit",mes2,p->username);
						write(connect_fd,mes,sizeof(mes));
						fseek(send_fd,0,SEEK_END);						
						file_size=ftell(send_fd);
						printf("file_size :%d\n",file_size);
						rewind(send_fd);
						sprintf(mes,"%d",file_size);
						write(p->fd,mes,sizeof(mes));
						printf("accept the file\n");
						while(fgets(buf,sizeof(buf),send_fd)!=NULL)
						{
							printf("sending\n");
							write(p->fd,buf,sizeof(buf));
						}
						strcpy(mes,"Accept the file success \nSelect the command (1)List (2)Send to all users (3)Send the message to the users who you want (4)Send the file to user (0)Exit");
						write(p->fd,mes,sizeof(mes));

					}
					else
					{
						printf("reject the file\n");
						strcpy(mes,"The user rejects your file\nSelect the command (1)List (2)Send to all users (3)Send the message to the users who you want (4)Send the file to user (0)Exit");
						write(connect_fd,mes,sizeof(mes));
						strcpy(mes,"Select the command (1)List (2)Send to all users (3)Send the message to the users who you want (4)Send the file to user (0)Exit");
						write(p->fd,mes,sizeof(mes));


					}
					SW=-1;
					fclose(send_fd);
				}
				else{
					strcpy(mes,"No this user\nSelect the command (1)List (2)Send to all users (3)Send the message to the users who you want (4)Send the file to user (0)Exit");
					write(connect_fd,mes,sizeof(mes));
				}	
				break;	
			case 0:
				flag=0;
				break;
			default:
				strcpy(mes,"No this command");
				write(connect_fd,mes,sizeof(mes));	
		}
		}
		delete_node(head);
	} 
	else //帳戶不存在創建帳戶
	{
		strcpy(mes,"It doesn't have this account . Please add the new account :");
		write(connect_fd,mes,strlen(mes));
	} 
	pthread_detach(pthread_self());
	fclose(file_fd);
	close(connect_fd);			/* done with connected socket */
	return(NULL);
}
int main(int argc, char **argv)
{
	int i,listen_fd,*iptr;
	pthread_t tid;
	socklen_t addrlen,cli_len;
	struct sockaddr_in servaddr,cliaddr;
	head=NULL;
	SW=-1;
	listen_fd=socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));

	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(8082);
	bind(listen_fd,(SA *)&servaddr,sizeof(servaddr));
	listen(listen_fd,1024);
	for ( ; ; ) {
		cli_len=addrlen;
		iptr=malloc(sizeof(int));
		*iptr=accept(listen_fd,(SA *)&cliaddr, &cli_len);
		pthread_create(&tid,NULL,doit,iptr);
	}
	close(listen_fd);
	return 0;
}
