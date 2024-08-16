// Receiver side implementation
#include <iostream>
#include <fstream>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <cstring>
#include <cmath>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
using namespace std;
	
#define MAXLINE 1024
	
// Driver code
int main() {
	int sockfd;
	float PacketDropProbability;
	char buffer[MAXLINE], Acknowledgement[MAXLINE] = "Acknowledgement:";
	struct sockaddr_in ReceiverPort, SenderPort;

	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
		
	memset(&ReceiverPort, 0, sizeof(ReceiverPort));
	memset(&SenderPort, 0, sizeof(SenderPort));
	
	// Filling receiver information
	ReceiverPort.sin_family = AF_INET; // IPv4
	ReceiverPort.sin_addr.s_addr = INADDR_ANY;
	cin >> SenderPort.sin_port >> ReceiverPort.sin_port >> PacketDropProbability;
	
	// Bind the socket with the reciever address
	if (bind(sockfd, (const struct sockaddr *)&ReceiverPort, sizeof(ReceiverPort)) < 0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	
	socklen_t len = sizeof(SenderPort); //len is value/result
	int n, i = 1;

	ofstream out("receiver.txt");
	
	while(true){
		// Receiving Packet from sender
		n = recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &SenderPort, &len);

		// Extracting sequence number from packet received
		string NumInPacket;
		for(int j = 7; j < n; j++)
			NumInPacket.push_back(buffer[j]);
		int PacketNum = stoi(NumInPacket);

		if(PacketNum != i){
			// Making Acknowledgement array by adding sequence number 'i' to "Acknowledgement:"
			int j = 16;
			string num = to_string(i);
			for(; j < 17 + (int)log10(i); j++)
				Acknowledgement[j] = num[j-16];
			Acknowledgement[j] = '\0';

			// Sending Acknowlegement to sender
			sendto(sockfd, Acknowledgement, 17 + (int)log10(i), MSG_CONFIRM, (const struct sockaddr *) &SenderPort, len);
            		cout << Acknowledgement << "\n";
			out << Acknowledgement << endl;
        	}
		else{
			double rand_var = ((double) rand())/RAND_MAX;	// Uniformly Distributed Random Variable between 0 and 1

			if(rand_var < PacketDropProbability)	// Drop packet if rand_var is less than PacketDropProbability{
                		cout << "Packet dropped.\n";
				out << "Packet dropped." << endl;
            		}
			else{
				// Making Acknowledgement array by adding sequence number 'i+1' to "Acknowledgement:"
				int j = 16;
				string num = to_string(i+1);
				for(; j < 17 + (int)log10(i+1); j++)
					Acknowledgement[j] = num[j-16];
				Acknowledgement[j] = '\0';
				
				// Sending Acknowlegement to sender
			    	sendto(sockfd, Acknowledgement, 17 + (int)log10(i+1), MSG_CONFIRM, (const struct sockaddr *) &SenderPort, len);
				cout << Acknowledgement << "\n";
				out << Acknowledgement << endl;
				i++;			// Move to next sequence number
			}	
		}
	}

	out.close();
	
	return 0;
}

// Reference: https://www.geeksforgeeks.org/udp-server-client-implementation-c/
