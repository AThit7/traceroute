// Paweł Borek 324938

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "traceroute.h"

/* Return the pointer to ICMP header if the packet is of ICMP_ECHOREPLY type
or ICMP_TIME_EXCEEDED type and contains ICMP_ECHO header inside its data
segment. In the latter scenario the pointer returned references ICMP_ECHO
header. Otherwise return NULL. */
static struct icmp* get_header(void* packet) {
  struct ip* ip_header = packet;
  struct icmp* icmp_header = packet + 4 * ip_header->ip_hl;

  if (icmp_header->icmp_type == ICMP_ECHOREPLY)
    return icmp_header;
  else if (icmp_header->icmp_type == ICMP_TIME_EXCEEDED &&
           icmp_header->icmp_code == 0) {
    ip_header = (void*)icmp_header + sizeof(icmp_header);
    icmp_header = (void*)ip_header + 4 * ip_header->ip_hl;
    if (icmp_header->icmp_type == ICMP_ECHO) {
      return icmp_header;
    }
  }
  return NULL;
}

/* Check if the ICMP header of either ICMP_ECHOREPLY or ICMP_ECHO type has
Identification field equal to 'id' and Sequence field in range ['seq', 'seq'+3).
*/
static int verify_header(struct icmp* icmp_header, uint16_t id, uint16_t seq) {
  uint16_t header_id = ntohs(icmp_header->icmp_hun.ih_idseq.icd_id);
  uint16_t header_seq = ntohs(icmp_header->icmp_hun.ih_idseq.icd_seq);
  return id == header_id && header_seq >= seq && header_seq < seq + 3;
}

/* Wait for up to 1s for 3 packets. Sleep while waiting for a packet using
select(). Upon receiving a packet check if it is of correct type and verify its
Sequence and Identification field. */
int receive(int sockfd, uint16_t id, uint16_t seq, struct in_addr* addr_buff,
            int* avg_rtt) {
  fd_set recv_fds;
  struct sockaddr_in sender;
  socklen_t sender_len = sizeof(sender);
  u_int8_t buffer[IP_MAXPACKET];
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  int elapsed = 0;  // sum of packets' round trip times in microseconds
  int nfound = 0;   // number of matching packets received so far

  while (tv.tv_sec || tv.tv_usec) {
    /* We need to reinitialize the set of descriptors each iteration because
     * select() might modify them */
    FD_ZERO(&recv_fds);
    FD_SET(sockfd, &recv_fds);
    int retval = select(sockfd + 1, &recv_fds, NULL, NULL, &tv);

    if (retval == -1) {
      perror("select()");
      exit(EXIT_FAILURE);
    }
    if (retval == 0) return nfound;

    if (0 > recvfrom(sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender,
                     &sender_len)) {
      perror("revfrom()");
      exit(EXIT_FAILURE);
    }

    struct icmp* icmp_header = get_header(buffer);
    /* Check if the received packet is one of which we are waiting for. */
    if (icmp_header && verify_header(icmp_header, id, seq)) {
      /* Save the IP address to the buffer. */
      addr_buff[nfound++].s_addr = sender.sin_addr.s_addr;
      elapsed += (1000000 - tv.tv_usec);

      if (nfound == 3) {
        *avg_rtt = elapsed / 3000;  // average rtt per packet in milliseconds
        return nfound;
      }
    }
  }
  return nfound;
}