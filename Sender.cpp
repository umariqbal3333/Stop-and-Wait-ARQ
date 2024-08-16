// Sender side implementation
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <cmath>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
using namespace std;

#define MAXLINE 1024
	
// Driver code
int main() {
    	int sockfd, NoOfPacketsToBeSent;
	float RetransmissionTimer;
	char buffer[MAXLINE], Packet[MAXLINE] = "Packet:";
	struct sockaddr_in ReceiverPort, SenderPort;
	
	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&ReceiverPort, 0, sizeof(ReceiverPort));
    	memset(&SenderPort, 0, sizeof(SenderPort));
		
	// Filling sender and receiver information
	ReceiverPort.sin_family = SenderPort.sin_family = AF_INET;
	ReceiverPort.sin_addr.s_addr = SenderPort.sin_addr.s_addr = INADDR_ANY;
	cin >> ReceiverPort.sin_port >> SenderPort.sin_port >> RetransmissionTimer >> NoOfPacketsToBeSent;

	// Bind the socket with the sender address
	if (bind(sockfd, (const struct sockaddr *)&SenderPort, sizeof(SenderPort)) < 0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	
	int n, i = 1;
	socklen_t len;
	bool AckReceived;			// True only if Acknowledgement with i+1 as sequence number is received

	ofstream out("sender.txt");
	
	while(i <= NoOfPacketsToBeSent){	
		// Making Packet array by adding sequence number to "Packet:"
		int j = 7;
		string num = to_string(i);
		for(; j < 8 + (int)log10(i); j++)
			Packet[j] = num[j-7];
		Packet[j] = '\0';

		// Sending packet to receiver
		sendto(sockfd, Packet, 8 + (int)log10(i), MSG_CONFIRM, (const struct sockaddr *) &ReceiverPort, sizeof(ReceiverPort));
		cout << "\n" << Packet << "\n";		// Printing in terminal
		out << "\n" << Packet << endl;		// Writing to sender.txt

		AckReceived = false;			// Whether received Acknowledgement within Retransmission Timer or not
		time_t start, end;
		time (&start);
		time (&end);

		// Loop till Retransmission Timer
		while(difftime(end, start) <= RetransmissionTimer){
			// Receiving Acknowledgement from receiver
			n = recvfrom(sockfd, buffer, MAXLINE, MSG_DONTWAIT, (struct sockaddr *) &ReceiverPort, &len);

			// n > 16 implies that some Acknowledgement has been received
			if(n > 16){
				// Extracting sequence number from Acknowledgement received
				string NumInAck;
				for(j = 16; j < n; j++)
					NumInAck.push_back(buffer[j]);
				int ReceivedAckNum = stoi(NumInAck);

				if(ReceivedAckNum == i+1){
					AckReceived = true;
					i++;			// Send packet with next sequence number in the next iteration
					break;
				}
			}
			time (&end);
		}

		// Retransmission Timer expired if the required Acknowledgement not received within Retransmission Timer
		if(!AckReceived){
			cout << "Retransmission Timer expired.\n";
			out << "Retransmission Timer expired." << endl;
		}
	}
	
	close(sockfd);

	out.close();

	return 0;
}

// Reference: https://www.geeksforgeeks.org/udp-server-client-implementation-c/
