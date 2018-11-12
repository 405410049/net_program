#include"net_header.h"
#include<sys/wait.h>
#define BUF_SIZE 8096
void action(int connect_fd)
{
	//	char webpage[]="HTTP/1.1 200 OK\r\n; charset=UTF-8\r\n\r\n";
	struct {
		char *ext;
		char *filetype;
	} extensions [] = {
		{"gif", "image/gif" },
		{"jpg", "image/jpeg"},
		{"jpeg","image/jpeg"},
		{"png", "image/png" },
		{"zip", "image/zip" },
		{"gz",  "image/gz"  },
		{"tar", "image/tar" },
		{"htm", "text/html" },
		{"html","text/html" },
		{"exe","text/plain" },
		{0,0} };
	char buffer[BUF_SIZE];
	char *fstr;
	unsigned int buflen,len;
	int j,n,i,file_fd;
	n=read(connect_fd,buffer,BUF_SIZE);   /* 讀取瀏覽器要求 */
	if (n==0||n==-1) {
		/* 網路連線有問題，所以結束行程 */
		exit(3);
	}
	/* 程式技巧：在讀取到的字串結尾補空字元，方便後續程式判斷結尾 */
	if (n>0&&n<BUF_SIZE)
		buffer[n]=0;
	else
		buffer[0]=0;
	/* 移除換行*/
	for (i=0;i<n;i++) 
		if (buffer[i]=='\r'||buffer[i]=='\n')
			buffer[i] = 0;
	/* 只接受 GET 命令要求 */
	if (strncmp(buffer,"GET ",4)&&strncmp(buffer,"get ",4))
		exit(3);
	/* 我們要把 GET /index.html HTTP/1.0 後面的 HTTP/1.0 用空字元隔開 */
	for(i=4;i<BUF_SIZE;i++) {
		if(buffer[i] == ' ') {
			buffer[i] = 0;
			break;
		}
	}
	/* 檔掉回上層目錄的路徑『..』 */
	for (j=0;j<i-1;j++)
		if (buffer[j]=='.'&&buffer[j+1]=='.')
			exit(3);

	/* 當客戶端要求根目錄時讀取 index.html */
	if (!strncmp(&buffer[0],"GET /\0",6)||!strncmp(&buffer[0],"get /\0",6) )
		strcpy(buffer,"GET /index.html\0");
	/* 檢查客戶端所要求的檔案格式 */
	buflen=strlen(buffer);
	fstr=(char *)0;
	for(i=0;extensions[i].ext!=0;i++) {
		len = strlen(extensions[i].ext);
		if(!strncmp(&buffer[buflen-len], extensions[i].ext, len)) {
			fstr = extensions[i].filetype;
			break;
		}
	}
	/* 檔案格式不支援 */
	if(fstr == 0)
		fstr = extensions[i-1].filetype;
	/* 開啟檔案 */
	if((file_fd=open(&buffer[5],O_RDONLY))==-1)
		write(connect_fd, "Failed to open file", 19);
	/* 傳回瀏覽器成功碼 200 和內容的格式 */
	sprintf(buffer,"HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);
	write(connect_fd,buffer,strlen(buffer));
	/* 讀取檔案內容輸出到客戶端瀏覽器 */
	while ((n=read(file_fd, buffer, BUF_SIZE))>0)
		write(connect_fd,buffer,n);
}
int main()
{
	int connect_fd,listen_fd;
	socklen_t cli_len;
	struct sockaddr_in serv_addr,cli_addr;
	chdir("./html_dir");
	listen_fd=socket(AF_INET,SOCK_STREAM,0);
	bzero(&serv_addr,sizeof(serv_addr));

	serv_addr.sin_family      = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port        = htons(8080);
	bind(listen_fd,(SA *)&serv_addr,sizeof(serv_addr));
	listen(listen_fd,LISTENQ);
	signal(SIGCHLD,SIG_IGN);
	while(1)	
	{
		int pid;
		cli_len=sizeof(cli_addr);
		connect_fd=accept(listen_fd,(SA *)&cli_addr,&cli_len);
		if((pid=fork())==0)	//child process
		{
			close(listen_fd);
			action(connect_fd);
			exit(0);
		}
		else
			wait(NULL);	
		close(connect_fd);
	}
	return 0;
}
