#include<stdio.h>
#include<pcap.h>
#include<string.h>
#include<time.h>
#include<net/ethernet.h>
#include<netinet/ip.h>
#include<netinet/tcp.h>
#include<arpa/inet.h>
#define LINE_LEN 16
#define SIZE_ETHERNET 14
void dispatcher_handler(u_char *, const struct pcap_pkthdr *, const u_char *);

int main(int argc, char *argv[])
{
	pcap_t *fp;
	char errbuf[PCAP_ERRBUF_SIZE];
	int i;
	char cmd[512];
	struct bpf_program filter;	
	if(argc<2)
	{	
		printf("usage: %s filename", argv[0]);
		return -1;

	}
	//打开pcap文件
	/* Open the capture file */
	if ((fp = pcap_open_offline(argv[argc-1],errbuf)) == NULL)
	{
		fprintf(stderr,"\nUnable to open the file %s.\n", argv[1]);
		return -1;
	}
	for(i=1;i<argc-1;i++){
		strcat(cmd,argv[i]);
		strcat(cmd," ");
	}
//	printf("cmd:%s\n",cmd);
	pcap_compile(fp,&filter,cmd,1,PCAP_NETMASK_UNKNOWN);
	/* read and dispatch packets until EOF is reached */
	if(pcap_setfilter(fp,&filter)==0)
		pcap_loop(fp,0,dispatcher_handler,NULL);

	//关闭打开pcap文件的指针
	pcap_close(fp);
	return 0;
}



void dispatcher_handler(u_char *temp1,const struct pcap_pkthdr *header,const u_char *pkt_data)
{
	u_int i=0;
	u_char *ptr;
	char buf[128];
	time_t t=header->ts.tv_sec;
	struct tm *timeinfo=localtime(&t);
	struct ether_header *eptr=(struct ether_header *)pkt_data; 
	//unused variable
	temp1=(void *)temp1;

	/* print pkt data*/
	strftime(buf,128,"%Y-%m-%d %X",timeinfo);
	printf("time :%s\n",buf);
	printf("len :%dbytes\n",header->len);
	ptr = eptr->ether_dhost;  
	i = ETHER_ADDR_LEN;  
	printf("Ethernet Destination Address: ");  
	do{  
		printf("%s%x",(i == ETHER_ADDR_LEN) ? " " : ":",*ptr++);  
	}
	while(--i>0);  
		printf("\n");  	
	ptr = eptr->ether_shost;  
	i = ETHER_ADDR_LEN;  
	printf("Ethernet Source Address: ");
	do{  
		printf("%s%x",(i == ETHER_ADDR_LEN) ? " " : ":",*ptr++);  
	}
	while(--i>0);  
		printf("\n");  	
	struct ip *_ip=(struct ip *)(pkt_data+SIZE_ETHERNET);
	printf("IP Source Address: %s\n",inet_ntoa(_ip->ip_src));
	_ip=(struct ip *)(pkt_data+SIZE_ETHERNET);
	printf("IP Destination Address: %s\n",inet_ntoa(_ip->ip_dst));
	u_short size_ip=(_ip->ip_hl)*4;
	struct tcphdr *tcp=(struct tcphdr *)(pkt_data+SIZE_ETHERNET+size_ip);
	short dport=ntohs(tcp->th_dport);
	short sport=ntohs(tcp->th_sport);
	printf("Destination Port: %d\nSource Port: %d\n\n",dport,sport);

}
