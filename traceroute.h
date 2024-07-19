// Paweł Borek 324938

#include <netinet/in.h>

/* Receive up to 3 ICMP packets of either ICMP_ECHOREPLY
type with matching Identifier and Sequence or ICMP_TIME_EXCEEDED type containing
the ICMP_ECHO packet with matching Identifier and Sequence in the data of
original datagram. Store their source IP addresses in 'addr_buff'. If 3 packets
are received store their average round trip time in 'avg_rtt'
\param fd file descriptor of a socket through which the packets will received
\param id value of the Identifier field in the ICMP header to be matched
\param seq value of the Sequence field in the ICMP header to be matched
\param addr_buff buffer where the addresses of received packets will be saved
\param avg_rtt if 3 packets were received the average round trip size will be
saved there
\return Number of matching packets received (max 3). */
int receive(int fd, uint16_t id, uint16_t seq, struct in_addr* addr_buff,
            int* avg_rtt);

/* Specify the parameters of consecutive calls to send_packets().
\param fd file descriptor of a socket through which the packets will be sent
\param id value of the Identifier field in the ICMP header
\param dest IP address to which the packets will be sent */
void init_sender(int fd, uint16_t id, struct in_addr dest);

/* Send an echo request to the address specified in init_sender().
\param fd file descriptor of a socket through which the packets will be sent
\param seq value of the Sequence field in the ICMP header
\param ttl time to live in the IP header of sent packets */
void send_packets(uint16_t seq, uint8_t ttl);