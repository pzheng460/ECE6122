/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Nov 20, 2023

Description:

It's a console program that takes as a command line argument the port number on which the TCP Server will listen for connection requests. A separate thread will be created to handle the data received from each remote client and the remote clients can continue to send and receive data on the connections until either the server or the client closes the connection. The TCP server maintains a list of all connected clients so that it can send out the appropriate messages. The TCP server is able to receive data from clients without blocking the main application thread. The program responds to user input while handling socket communications at the same time.

*/
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <algorithm>

struct tcpMessage {
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

std::vector<sf::TcpSocket*> clients;
std::mutex clients_mutex;
sf::SocketSelector selector;
tcpMessage lastMessage;
std::mutex message_mutex;
bool server_running = true;

void broadcastMessage(const tcpMessage& msg, sf::TcpSocket* sender) {
    sf::Packet packet;
    packet << msg.nVersion << msg.nType << msg.nMsgLen << msg.chMsg;

    for (sf::TcpSocket* client : clients) {
        // std::cout << "Sending to client: " << client->getRemoteAddress() << std::endl;
        if (client != sender) {
            client->send(packet);
        }
    }
}

void handleClients(sf::TcpListener& listener) {
    while (server_running)
    {
        if (selector.wait())
        {
            // Test the listener
            if (selector.isReady(listener))
            {
                // The listener is ready: there is a pending connection
                sf::TcpSocket* client = new sf::TcpSocket;
                if (listener.accept(*client) == sf::Socket::Done)
                {
                    std::lock_guard<std::mutex> lock(clients_mutex);
                    clients.push_back(client);
                    selector.add(*client);  // Add the new client to the selector so that we will be notified when he sends something
                }
                else
                {
                    // Error, we won't get a new connection, delete the socket
                    delete client;
                }
            }
            else
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                for (sf::TcpSocket* client : clients)
                {
                    if (selector.isReady(*client))
                    {
                        sf::Packet packet;
                        if (client->receive(packet) != sf::Socket::Done)
                        {
                            // Error, we won't get any data, the socket has probably been disconnected
                            selector.remove(*client);
                            clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
                            client->disconnect();
                            delete client;
                            break;
                        }

                        tcpMessage message;
                        packet >> message.nVersion >> message.nType >> message.nMsgLen >> message.chMsg;
                        if (message.nVersion != 102) continue; // Ignore if version is not 102

                        {
                            std::lock_guard<std::mutex> message_lock(message_mutex);
                            lastMessage = message; // Store the last message
                        }

                        if (message.nType == 77)
                        {
                            broadcastMessage(message, client); // Broadcast message
                        }
                        else if (message.nType == 201)
                        {
                            // Reverse the message and send back
                            std::reverse(message.chMsg, message.chMsg + message.nMsgLen);
                            sf::Packet responsePacket;
                            responsePacket << message.nVersion << message.nType << message.nMsgLen << message.chMsg;
                            client->send(responsePacket);
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <PORT>" << std::endl;
        return 1;
    }

    unsigned short port = static_cast<unsigned short>(std::stoi(argv[1]));
    sf::TcpListener listener;

    if (listener.listen(port) != sf::Socket::Done) {
        std::cerr << "Error: Could not listen on port " << port << std::endl;
        return 1;
    }

    selector.add(listener);

    std::thread clientHandlerThread(handleClients, std::ref(listener));
    clientHandlerThread.detach();

    std::string command;
    while (server_running) {
        std::cout << "Please enter command: ";
        std::cin >> command;

        if (command == "exit") {
            server_running = false;
        } else if (command == "clients") {
            std::lock_guard<std::mutex> lock(clients_mutex);
            std::cout << "Number of Clients: " << clients.size() << std::endl;
            for (sf::TcpSocket* client : clients) {
                sf::IpAddress ip = client->getRemoteAddress();
                unsigned short clientPort = client->getRemotePort();
                std::cout << "IP Address: " << ip << " | Port: " << clientPort << std::endl;
            }
        } else if (command == "msg") {
            std::lock_guard<std::mutex> lock(message_mutex);
            std::cout << "Last Message: " << lastMessage.chMsg << std::endl;
        } else {
            std::cout << "Unknown command." << std::endl;
        }
    }

    server_running = false;
    if (clientHandlerThread.joinable())
    {
        clientHandlerThread.join();
    }
    listener.close();

    for (sf::TcpSocket* client : clients) {
        client->disconnect();
        delete client;
    }
    clients.clear();

    return 0;
}
