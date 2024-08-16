# Implementation of Stop-and-Wait ARQ on top of UDP sockets

## Course Assignment - CS224 - Computer Networks
The problem statement can be found [here](https://github.com/ayushj05/Stop_and_Wait_ARQ/blob/main/Homework-3.pdf).

## Explanation of the code
### Sender:
* In the implementation of the sender side, first the socket is created using the socket() function.
* Then the information of both sender and receiver is given from the terminal so as to bind the sender port with the socket and also for the sender to know that to which port it has send the packets.
* Now that we know the sender port, the sender port is binded with the socket.
* Now using the sendto() function, the sender will send its packet to the receiver, staring with sequence number '1'.
* Now a Retransmission Timer is started using a while loop, in which the time elapsed from just before the start of while loop till the current iteration is measured.
* Now as per the Stop-and-Wait algorithm:
  * If the retransmission timer expires and there is no acknowledgment from the receiver then the sender will retransmit the same packet.
  * If the sender receives an acknowledgment before the retransmission timer expires and the acknowledgement contains x+1 as its sequence number then the sender transmits the packet with sequence number x+1.
  * If there is an acknowledgment from the receiver containing a sequence number other than x+1 then the sender has to ignore the acknowledgement.

## Receiver:
* In the implementation of the receiver side, first the socket is created using the socket() function.
* Then the receiver information is given from the terminal so as to bind the receiver port with the socket.
* Now that we know the receiver port, the receiver port is binded with the socket.
* Now using the recvfrom() function, the receiver will wait (because of MSG_WAITALL) until it receives any data.
* Upon receiving packet, the sender port info is saved into SenderPort and the packet received is saved in buffer.
* Now the Stop-and-Wait algorithm is ran, as per which the receiver should follow following steps:
  * Receive the packet and check if the packet with correct sequence number is received (the receiver expects the very first packet to have sequence number 1).
  * If the sequence no. is incorrect, an acknowledgment is sent with the expected packet sequence number (which is x+1, where x is the sequence no. of the last packet received from the sender).
  * If the sequence no. is correct, the following steps are done:
    * A uniformly distributed random variable is generated between 0 and 1. If this number is less than the PacketDropProbability then no acknowledgement is generated. The receiver will wait for the sender to retransmit the packet with the same Sequence number.
    * Otherwise, an acknowledgement will be sent to the sender which contains the sequence no. of the packet that the receiver is expecting next.

## Command for Compilation (as per Ubuntu `18.04`)
```bash
g++ Receiver.cpp -o Receiver
g++ Sender.cpp -o Sender
```

## Command to run the programs (as per Ubuntu `18.04`)
* Simulate delay using the following command:
```bash
sudo tc qdisc add dev lo root netem delay DelayInMiliseconds
```
* Open two terminals and run the following commands seperately in each of the terminals
```bash
./Receiver
```
```bash
./Sender
```
   Pass the inputs in the following format respectively:
```bash
ReceiverPort SenderPort PacketDropProbability
```
```bash
SenderPort ReceiverPort RetransmissionTimer NoOfPacketsToBeSent
```

(Note: You need to run the Receiver side first and then the Sender side.)

The sender.txt and receiver.txt files uploaded contain the output for following commands:
```bash
sudo tc qdisc add dev lo root netem delay 1000
```
```bash
./Receiver
```
```bash
./Sender
```
Inputs:
```bash
8080 8081 0.4
```
```bash
8081 8080 3 10
```
