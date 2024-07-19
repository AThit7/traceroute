// Paweł Borek 324938

#include "traceroute.h"

#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Print the results of receive(). */
void print_hop_info(struct in_addr* buff, int count, int time) {
  /* This guarantees that if there are 2 equal addresses in the buffer they will
   * be next to each other, and we can skip them later. */
  if (buff[2].s_addr == buff[0].s_addr) {
    struct in_addr tmp = buff[1];
    buff[1] = buff[2];
    buff[2] = tmp;
  }

  for (int j = 0; j < count; j++) {
    char str[20];
    in_addr_t addr = buff[j].s_addr;
    if (!inet_ntop(AF_INET, &addr, str, 20)) {
      perror("inet_top()");
      exit(EXIT_FAILURE);
    }
    printf("%s ", str);
    /* Skip the next address(es) if they are equal to the current one. (We
     * only want to print unique addresses.) */
    while (j + 1 < count && addr == buff[j + 1].s_addr) j++;
  }

  if (count == 3)
    printf("%dms\n", time);
  else if (count > 0)
    printf("???\n");
  else if (count == 0)
    printf("*\n");
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: ./traceroute <ipv4 address>\n");
    return EXIT_FAILURE;
  }

  struct in_addr dest;
  if (inet_pton(AF_INET, argv[1], &dest) != 1) {
    fprintf(stderr, "Incorrect IP address\n");
    return EXIT_FAILURE;
  }

  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0) {
    perror("socket()");
    return EXIT_FAILURE;
  }

  uint16_t id = getpid();
  init_sender(sockfd, id, dest);

  for (uint16_t i = 1; i <= 30; i++) {
    struct in_addr receive_buff[3];
    int avg_rtt_time;

    send_packets(i * 3, i);
    int n = receive(sockfd, id, i * 3, receive_buff, &avg_rtt_time);

    printf("%hu. ", i);
    print_hop_info(receive_buff, n, avg_rtt_time);

    /* Check if we got a response from 'dest' address in this hop. */
    for (int j = 0; j < n; j++)
      if (receive_buff[j].s_addr == dest.s_addr) return EXIT_SUCCESS;
  }
  return EXIT_SUCCESS;
}