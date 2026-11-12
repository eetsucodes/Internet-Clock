#include "includes.h"

#define NTP_SERVER "pool.ntp.org" // googlen time servu käy hyvi kans
#define NTP_PORT 123

#define NTP_TIMESTAMP_DELTA 2208988800ULL


typedef struct {
    uint8_t li_vn_mode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    uint32_t rootDelay;
    uint32_t rootDispersion;
    uint32_t refId;
    uint32_t refTm_s;
    uint32_t refTm_f;
    uint32_t origTm_s;
    uint32_t origTm_f;
    uint32_t rxTm_s;
    uint32_t rxTm_f;
    uint32_t txTm_s;
    uint32_t txTm_f;
} ntp_packet; // NTP paketin perusmuoto

int main() {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    ntp_packet packet;

    memset(&packet, 0, sizeof(ntp_packet));
    packet.li_vn_mode = 0x1b; // LI = 0, VN = 3 IPv4, Mode = 3 Client

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("Error in socket");
        return 1;
    }

    server = gethostbyname(NTP_SERVER);
    if (server == NULL) {
        fprintf(stderr, "Unknown Host!\n");
        return 1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(NTP_PORT);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connectionlost/connectionerror");
        return 1;
    }

    send(sockfd, (char*)&packet, sizeof(ntp_packet), 0);
    n = recv(sockfd, (char*)&packet, sizeof(ntp_packet), 0);
    close(sockfd);

    if (n < 0) {
        perror("Error on getting a response");
        return 1;
    }

    // Ajan muutos. vähennetään NTP_timestamp, muuten reilu 30min edellä.
    uint32_t txTm = ntohl(packet.txTm_s);
    time_t timeActual = (time_t)(txTm - NTP_TIMESTAMP_DELTA);

    printf("Time: %s", ctime(&timeActual));
    return 0;
}