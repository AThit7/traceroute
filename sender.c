// Paweł Borek 324938

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "traceroute.h"

u_int16_t compute_icmp_checksum(const void* buff, int length) {
  u_int32_t sum;
  const u_int16_t* ptr = buff;
  assert(length % 2 == 0);
  for (sum = 0; length > 0; length -= 2) sum += *ptr++;
  sum = (sum >> 16) + (sum & 0xffff);
  return (u_int16_t)(~(sum + (sum >> 16)));
}

static struct icmp header;
static int sockfd;
static struct sockaddr_in recipient;

/* Prepare data for their use in send_packets(); fill in the structs' fields
 * that won't change during runtime. */
void init_sender(int fd, uint16_t id, struct in_addr dest) {
  header.icmp_type = ICMP_ECHO;
  header.icmp_code = 0;
  header.icmp_hun.ih_idseq.icd_id = htons(id);
  sockfd = fd;
  bzero(&recipient, sizeof(recipient));
  recipient.sin_family = AF_INET;
  recipient.sin_addr = dest;
}

/* Set TTL to 'ttl' in socket options. Send 3 ICMP_ECHO packets with Sequences
 * 'seq', 'seq'+1 and 'seq'+2. */
void send_packets(uint16_t seq, uint8_t ttl) {
  if (0 > setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(uint8_t))) {
    perror("setsockopt()");
    exit(EXIT_FAILURE);
  }

  for (uint16_t i = 0; i < 3; i++) {
    header.icmp_hun.ih_idseq.icd_seq = htons(seq + i);
    header.icmp_cksum = 0;
    header.icmp_cksum =
        compute_icmp_checksum((u_int16_t*)&header, sizeof(header));

    if (0 > sendto(sockfd, &header, sizeof(header), 0,
                   (struct sockaddr*)&recipient, sizeof(recipient))) {
      perror("sendto()");
      exit(EXIT_FAILURE);
    }
  }
}