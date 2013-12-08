#include <arpa/inet.h>
#include <stdio.h>

int main(int argc,char* argv[])
{
	
	in_addr_t addr = inet_addr("192.168.1.109");
	printf("add=%u\n",ntohl(addr));
	printf("htonl=%u\n",htonl(addr));
	
	struct in_addr ipaddr;
	ipaddr.s_addr = addr;
	printf("%s\n",inet_ntoa(ipaddr));

	return 0;
}
