#include <stdio.h>
#include <pcap.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>

void parse_SMTP_packet(const u_char *packet)
{
    struct in_addr addr;
    struct iphdr *ipptr;
    struct ether_header *eptr;
    struct tcphdr *tcpptr;
    u_char *ptr;
    char *data;
    int i;

    eptr = (struct ether_header *)packet;

    ipptr = (struct iphdr *)(packet + sizeof(struct ether_header)); //得到ip包头
    printf("the IP packets total_length is :%d\n", ipptr->tot_len);
    addr.s_addr = ipptr->daddr;
    printf("Destination IP: %s\n", inet_ntoa(addr));
    addr.s_addr = ipptr->saddr;
    printf("Source IP: %s\n", inet_ntoa(addr));
    printf("\n");

    tcpptr = (struct iphdr *)(packet + sizeof(struct ether_header) + sizeof(struct iphdr)); //得到tcp包头
    printf("Destination port : %d\n", ntohs(tcpptr->dest));
    printf("Source port : %d\n", ntohs(tcpptr->source));
    printf("the seq of packet is %d\n", ntohl(tcpptr->seq));
    //以上关于ip、tcp的结构信息请查询/usr/include/linux/ip.h | tcp.h

    data = (char *)(packet + sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct tcphdr)); //得到数据包里内容，不过一般为乱码。
    printf("\nthe content of packets is \n%s\n", data);
    printf("*******************************************************************************");
}

void callback(u_char *userarg, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    int *id = (int *)userarg;

    printf("id: %d\n", (*id)++);
    printf("length: %d\n", pkthdr->len);
    printf("Number of bytes: %d\n", pkthdr->caplen);
    printf("time: %s", ctime((const time_t *)&pkthdr->ts.tv_sec));

    // int i;
    // for (i = 0; i < pkthdr->len; ++i)
    // {
    //     printf(" %0x", packet[i]);
    //     if ((i + 1) % 16 == 0)
    //     {
    //         printf("\n");
    //     }
    // }
    parse_SMTP_packet(packet);
    printf("\n\n");
}

int main(void)
{
    pcap_t *handle;
    char errBuf[PCAP_ERRBUF_SIZE];
    struct bpf_program filter;
    struct pcap_pkthdr header;
    u_char *packet;

    handle = pcap_open_offline("./test.pcapng", errBuf);

    if (NULL != handle)
    {
        printf("open ok!\n");
    }
    else
    {
        printf("open failed!\n");
    }

    char filter_app[] = "dst port 25";

    pcap_compile(handle, &filter, filter_app, 0, 0);
    pcap_setfilter(handle, &filter);

    int id = 1;

    pcap_loop(handle, 20, callback, (u_char *)&id);
    pcap_close(handle);

    return 0;
}