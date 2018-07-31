#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<unistd.h>
#include<netinet/tcp.h>
#include<arpa/inet.h>
#include<netpacket/packet.h>
#include<net/ethernet.h>
#include<linux/if_ether.h>
#include<netdb.h>
#if 0

struct ip
  {
    u_int8_t ip_tos;			/* type of service */
    u_short ip_len;			/* total length */
    u_short ip_id;			/* identification */
    u_short ip_off;			/* fragment offset field */
    u_int8_t ip_ttl;			/* time to live */
    u_int8_t ip_p;			/* protocol */
    u_short ip_sum;			/* checksum */
    struct in_addr ip_src, ip_dst;	/* source and dest address */
  };
#endif
int main()
{
    struct hostent *p = gethostbyname("www.baidu.com");
    printf("%s\n",inet_ntoa(*(struct in_addr*)(p->h_addr)));
//IP ARP
//    int sfd = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
//    if(sfd == -1)perror("socket"),exit(1);
//
//    char buf[2000];
//    while(1){
//        memset(buf,0x00,sizeof buf);
//        if(read(sfd,buf,sizeof buf) < 0)
//            break;
//        struct ethhdr *pe = (struct ethhdr*)(buf);
//        printf("%02X:%02X:%02X:%02X:%02X:%02X< - >",
//               pe->h_source[0],
//               pe->h_source[1],
//               pe->h_source[2],
//               pe->h_source[3],
//               pe->h_source[4],
//               pe->h_source[5]);
//        printf("%02X:%02X:%02X:%02X:%02X:%02X\n",
//               pe->h_dest[0],
//               pe->h_dest[1],
//               pe->h_dest[2],
//               pe->h_dest[3],
//               pe->h_dest[4],
//               pe->h_dest[5]);
//        if(ntohs(pe->h_proto) == 0x0800){
//            printf("\tIP\n");
//        }
//        if(ntohs(pe->h_proto) == 0x0806){ 
//            printf("\tARP\n");
//        }
//    }
//抓包
//    int op = 1;
//    setsockopt(sfd,IPPROTO_IP,IP_HDRINCL,&op,sizeof op);
//    
//    char buf[1500] = {};
//    while(1){
//        memset(buf,0x00,sizeof buf);
//        int r = read(sfd,buf,1500);
//        if(r <= 0)break;
//        struct ip *pip = (struct ip*)(buf);
//        printf("%s<------>",inet_ntoa(pip->ip_dst));
//        printf("%s :",inet_ntoa(pip->ip_src));
//        struct tcphdr *ptcp = (struct tcphdr*)(buf+(pip->ip_hl<<2));
//        printf("%hu : %hu\n",ntohs(ptcp->dest),ntohs(ptcp->source));
//    }
    return 0;
}
