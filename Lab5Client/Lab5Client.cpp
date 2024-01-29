/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Nov 20, 2023

Description:

It's a console program that takes as a command line argument the port number on which the TCP Server will listen for connection requests. A separate thread shall be created to handle the data received from each remote client and the remote clients can continue to send and receive data on the connections until either the server or the client closes the connection. This part is the client of the program.

*/
#include <SFML/Network.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <atomic>

struct tcpMessage {
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

std::atomic<bool> running(true);

void receiveData(sf::TcpSocket& socket) {
    while (running) {
        sf::Packet receivePacket;
        if (socket.receive(receivePacket) != sf::Socket::Done) {
            break; // Error or disconnected
        }

        // Extract the message
        tcpMessage receivedMessage;
        receivePacket >> receivedMessage.nVersion >> receivedMessage.nType
                      >> receivedMessage.nMsgLen >> receivedMessage.chMsg;
        std::cout << "Received Msg Type: " << static_cast<int>(receivedMessage.nType)
                  << "; Msg: " << receivedMessage.chMsg << std::endl;

    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <IP_ADDRESS> <PORT>" << std::endl;
        return 1;
    }

    sf::IpAddress serverIp = argv[1];
    unsigned short serverPort = static_cast<unsigned short>(std::stoi(argv[2]));
    sf::TcpSocket socket;

    if (socket.connect(serverIp, serverPort) != sf::Socket::Done) {
        std::cerr << "Connection to server failed" << std::endl;
        return 1;
    }

    std::thread receiveThread(receiveData, std::ref(socket));
    receiveThread.detach();

    tcpMessage message;
    message.nVersion = 102;  // Default version

    while (true) {
        std::string input;
        std::cout << "Please enter command: ";
        std::getline(std::cin, input);

        if (input.empty()) {
            continue;
        }

        std::istringstream iss(input);
        std::string command;
        iss >> command;

        if (command == "q") {
            running = false;
            socket.disconnect();
            break; // Quit command
        } else if (command == "v") {
            int version;
            if (iss >> version) {
                message.nVersion = static_cast<unsigned char>(version);
            }
        } else if (command == "t") {
            int type;
            std::string msg;
            if (iss >> type) {
                std::getline(iss, msg); // Get the rest of the line as the message
                if (!msg.empty() && msg[0] == ' ') {
                    msg.erase(0, 1); // Remove leading space
                }
                message.nType = static_cast<unsigned char>(type);
                message.nMsgLen = static_cast<unsigned short>(msg.length());
                std::copy(msg.begin(), msg.end(), message.chMsg);

                sf::Packet packet;
                packet << message.nVersion << message.nType << message.nMsgLen << message.chMsg;
                socket.send(packet);
            }
        } else {
            std::cout << "Invalid command" << std::endl;
        }
    }

    if (receiveThread.joinable()) {
        receiveThread.join();
    }

    socket.disconnect();
    return 0;
}
