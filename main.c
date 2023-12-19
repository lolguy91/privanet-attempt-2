#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

#define PT_BEAM 0x01
#define PT_IDENT 0x02
#define PT_RESP_IDENT 0x03
#define PT_TEXT_MSG 0x04
#define BEAM_MAGICNUM 0xFFFFFFFFFFFFFFFF
#define BASE_PORT 64420
#define MAX_SESSIONS 256

typedef struct {
    unsigned char type;
    char data[255];
} Packet;

typedef struct {
    unsigned long long magnum;
    unsigned long long uuid;
    char hostname[255];
} IdentificationPacket;

typedef struct {
    unsigned long long uuid;
    char hostname[255];
} SessionIdentity;

typedef struct {
    int socketDescriptor;
    SessionIdentity sessionIdentities[MAX_SESSIONS];
    unsigned char selfSessionID;
    char selfHostname[255];
    unsigned long long myUuid;
} ThreadData;

unsigned long long generateUUID() {
    return ((unsigned long long)rand() << 32) | rand();
}

void sendBeamPacket(int socket, unsigned char sessionID) {
    printf("Sending Beam Packet #%u.\n", sessionID);
    Packet beamPacket;
    beamPacket.type = PT_BEAM;
    *(unsigned long long*)beamPacket.data = BEAM_MAGICNUM;

    struct sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(BASE_PORT + sessionID);
    clientAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sendto(socket, &beamPacket, sizeof(Packet), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
}

void sendIdentificationPacket(int socket, struct sockaddr_in* senderAddr, unsigned long long uuid, const char* hostname) {
    printf("Sending Identification Packet.\n");
    Packet identPacket;
    identPacket.type = PT_IDENT;

    IdentificationPacket data;
    data.magnum = BEAM_MAGICNUM;
    data.uuid = uuid;
    strncpy(data.hostname, hostname, sizeof(data.hostname));

    memcpy(identPacket.data, &data, sizeof(identPacket.data));

    sendto(socket, &identPacket, sizeof(Packet), 0, (struct sockaddr*)senderAddr, sizeof(struct sockaddr_in));
}

void sendResponseIdentificationPacket(int socket, struct sockaddr_in* senderAddr, unsigned long long uuid, const char* hostname) {
    printf("Sending Response Identification Packet.\n");
    Packet respIdentPacket;
    respIdentPacket.type = PT_RESP_IDENT;

    IdentificationPacket data;
    data.magnum = BEAM_MAGICNUM;
    data.uuid = uuid;
    strncpy(data.hostname, hostname, sizeof(data.hostname));

    memcpy(respIdentPacket.data, &data, sizeof(respIdentPacket.data));

    sendto(socket, &respIdentPacket, sizeof(Packet), 0, (struct sockaddr*)senderAddr, sizeof(struct sockaddr_in));
}

void sendTextMessage(int socket, struct sockaddr_in* destAddr, const char* message) {
    printf("Sending Text Message.\n");
    Packet textMsgPacket;
    textMsgPacket.type = PT_TEXT_MSG;
    strncpy(textMsgPacket.data, message, sizeof(textMsgPacket.data));

    sendto(socket, &textMsgPacket, sizeof(Packet), 0, (struct sockaddr*)destAddr, sizeof(struct sockaddr_in));
}

void printIdentificationPacket(const IdentificationPacket* identData) {
    printf("Received Identity Packet from UUID: 0x%16llx, Hostname: %s\n", identData->uuid, identData->hostname);
}

void printSessionIdentities(const SessionIdentity* sessionIdentities, unsigned char selfSessionID) {
    printf("Session Identities:\n");
    for (unsigned char sessionID = 0; sessionID < MAX_SESSIONS; ++sessionID) {
        if (sessionID == selfSessionID) {
            printf("[%u] Self - %s\n", sessionID, sessionIdentities[sessionID].hostname);
        } else if (sessionIdentities[sessionID].uuid != 0) {
            printf("[%u] UUID: 0x%16llx, Hostname: %s\n", sessionID,
                   sessionIdentities[sessionID].uuid, sessionIdentities[sessionID].hostname);
        }else{
            break;
        }

    }
}


void* listeningThread(void* data) {
    ThreadData* threadData = (ThreadData*)data;

    while (1) {
        Packet receivedPacket;
        struct sockaddr_in senderAddr;
        socklen_t senderAddrLen = sizeof(senderAddr);

        ssize_t bytesRead = recvfrom(threadData->socketDescriptor, &receivedPacket, sizeof(Packet), 0,
                                     (struct sockaddr*)&senderAddr, &senderAddrLen);

        if (bytesRead < 0) {
            perror("Recvfrom error");
            break;
        }

        if (receivedPacket.type == PT_BEAM && receivedPacket.data[0] == BEAM_MAGICNUM) {
            printf("Received Beam Packet.\n");
            // Respond with identification packet
            sendIdentificationPacket(threadData->socketDescriptor, &senderAddr, threadData->myUuid, threadData->selfHostname);
        } else if (receivedPacket.type == PT_RESP_IDENT) {
            // Handle response identification packet
            IdentificationPacket* identData = (IdentificationPacket*)receivedPacket.data;
            if (identData->magnum == BEAM_MAGICNUM) {
                printf("Received Response-Identity Packet from UUID: 0x%16llx, Hostname: %s\n",
                       identData->uuid, identData->hostname);

                // Add the sender's identity to the array
                unsigned char senderSessionID = ntohs(senderAddr.sin_port - htons(BASE_PORT));
                SessionIdentity* senderIdentity = &threadData->sessionIdentities[senderSessionID];
                senderIdentity->uuid = identData->uuid;
                strncpy(senderIdentity->hostname, identData->hostname, sizeof(senderIdentity->hostname));
            } else {
                printf("Received Malformed Response Identity Packet\n");
            }
        } else if (receivedPacket.type == PT_TEXT_MSG) {
            // Handle text message
            printf("\nReceived Text Message: %s\n", receivedPacket.data);
        } else {
            printf("Received unknown packet.\n");
        }
    }

    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    ThreadData threadData;
    pthread_t listenerThread;
    memset(&threadData,0,sizeof(ThreadData));

    threadData.myUuid = generateUUID();

    if (gethostname(threadData.selfHostname,sizeof(threadData.selfHostname)) != 0) {
        perror("gethostname");
    }

    threadData.socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (threadData.socketDescriptor < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Exploration phase
    for (threadData.selfSessionID = 0; threadData.selfSessionID < MAX_SESSIONS; ++threadData.selfSessionID) {
        sendBeamPacket(threadData.socketDescriptor, threadData.selfSessionID);

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(threadData.socketDescriptor, &fds);

        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int ready = select(threadData.socketDescriptor + 1, &fds, NULL, NULL, &timeout);

        if (ready == -1) {
            perror("Select error");
            exit(EXIT_FAILURE);
        } else if (ready > 0) {

            if (threadData.selfSessionID == MAX_SESSIONS - 1) {
                break;
            }
            // Packet received within timeout
            Packet receivedPacket;
            struct sockaddr_in senderAddr;
            socklen_t senderAddrLen = sizeof(senderAddr);

            ssize_t bytesRead = recvfrom(threadData.socketDescriptor, &receivedPacket, sizeof(Packet), 0,
                                         (struct sockaddr*)&senderAddr, &senderAddrLen);

            if (bytesRead < 0) {
                perror("Recvfrom error");
                break;
            }

            if (receivedPacket.type == PT_IDENT && receivedPacket.data[0] == BEAM_MAGICNUM) {
                printf("Received valid Identification Packet during exploration.\n");
                // Respond with response identification packet
                sendResponseIdentificationPacket(threadData.socketDescriptor, &senderAddr, threadData.myUuid,
                                                 threadData.sessionIdentities[threadData.selfSessionID].hostname);
                IdentificationPacket* identData = (IdentificationPacket*)receivedPacket.data;

                // Add the sender's identity to the array
                unsigned char senderSessionID = ntohs(senderAddr.sin_port - htons(BASE_PORT));
                SessionIdentity* senderIdentity = &threadData.sessionIdentities[senderSessionID];
                senderIdentity->uuid = identData->uuid;
                strncpy(senderIdentity->hostname, identData->hostname, sizeof(senderIdentity->hostname));

                continue;
            } else {
                printf("Received unknown packet during exploration.\n");
            }
        } else {
            // Timeout reached, take this ID and move on
            break;
        }
    }
    if (threadData.selfSessionID == MAX_SESSIONS - 1) {
        printf("Too many instances running on this computer.\n");
        close(threadData.socketDescriptor);
        exit(EXIT_SUCCESS);
    }

    printf("Using session ID: %u\n", threadData.selfSessionID);

    // Add the yourself to the array
    SessionIdentity* senderIdentity = &threadData.sessionIdentities[threadData.selfSessionID];
    senderIdentity->uuid = threadData.myUuid;
    strncpy(senderIdentity->hostname, threadData.sessionIdentities[threadData.selfSessionID].hostname,
            sizeof(senderIdentity->hostname));

    // Create a new socket with the assigned session ID
    close(threadData.socketDescriptor);
    threadData.socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(BASE_PORT + threadData.selfSessionID);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(threadData.socketDescriptor, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind error");
        close(threadData.socketDescriptor);
        exit(EXIT_FAILURE);
    }

    // Create a separate thread for listening
    if (pthread_create(&listenerThread, NULL, listeningThread, (void*)&threadData) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // TUI prompt for sending a text message
    while (1) {
        // Print the updated list of session identities
        printSessionIdentities(threadData.sessionIdentities, threadData.selfSessionID);
        printf("Enter Session ID to send a message (or 'q' to quit): ");
        char input[10];
        fgets(input, sizeof(input), stdin);
        if (input[0] == 'q' || input[0] == 'Q') {
            break;
        }

        unsigned char destSessionID = atoi(input);
        if (destSessionID < MAX_SESSIONS && threadData.sessionIdentities[destSessionID].uuid != 0) {
            char message[255];
            printf("Enter message: ");
            fgets(message, sizeof(message), stdin);
            
            struct sockaddr_in destAddr;
            destAddr.sin_family = AF_INET;
            destAddr.sin_port = htons(BASE_PORT + destSessionID);
            destAddr.sin_addr.s_addr = htonl(INADDR_ANY);

            // Send the text message
            sendTextMessage(threadData.socketDescriptor, &destAddr, message);
        } else {
            printf("Invalid Session ID or Session ID not available.\n");
        }
    }

    // Wait for the listener thread to finish
    pthread_cancel(listenerThread);

    close(threadData.socketDescriptor);
    return 0;
}
