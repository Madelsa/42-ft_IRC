/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mabdelsa <mabdelsa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:32:55 by mabdelsa          #+#    #+#             */
/*   Updated: 2024/10/14 16:06:39 by mabdelsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <map>
#include <cstdlib> // For std::atoi

// Maximum number of clients the server can handle simultaneously.
const int MAX_CLIENTS = 100;

int main(int argc, char* argv[]) {
    // Step 1: Validate the number of command-line arguments.
    //     The program expects two arguments: port and password.
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }

    // Step 2: Parse the port number from the command-line argument.
    int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Invalid port number. Please provide a valid port (1-65535).\n";
        return 1;
    }

    // Step 3: Store the password provided by the user.
    std::string serverPassword = argv[2];

    // Step 4: Create a socket to listen for incoming connections.
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        std::cerr << "Error: could not create socket.\n";
        return 1;
    }

    // Step 5: Define server address structure (IPv4 and port).
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any address
    serverAddr.sin_port = htons(port); // Convert port number to network byte order

    // Step 6: Bind the socket to the specified IP and port.
    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error: could not bind socket.\n";
        return 1;
    }

    // Step 7: Listen for incoming connections (up to 5 pending connections).
    listen(listenSocket, 5);
    std::cout << "Server listening on port " << port << "\n";

    // Step 8: Initialize the pollfd structure to monitor the listen socket and clients.
    struct pollfd fds[MAX_CLIENTS];
    memset(fds, 0, sizeof(fds));
    fds[0].fd = listenSocket; // Add the listen socket to the poll array
    fds[0].events = POLLIN; // Monitor for incoming connections

    // Initialize client count and a map to track client numbers.
    int clientCount = 1; // Start with only the listening socket
    std::map<int, int> clientIndex; // Maps file descriptors to client numbers
    std::map<int, bool> authenticatedClients; // Maps file descriptors to authentication status

    while (true) {
        // Step 9: Poll to monitor multiple file descriptors for events.
        int pollCount = poll(fds, clientCount, -1);
        if (pollCount == -1) {
            std::cerr << "Error: poll failed.\n";
            break;
        }

        // Step 10: Iterate through the file descriptors to handle events.
        for (int i = 0; i < clientCount; i++) {
            // Check if there is a new incoming connection on the listen socket.
            if (fds[i].fd == listenSocket && (fds[i].revents & POLLIN)) {
                sockaddr_in client;
                socklen_t clientSize = sizeof(client);
                int clientSocket = accept(listenSocket, (sockaddr*)&client, &clientSize);
                if (clientSocket == -1) {
                    std::cerr << "Error: failed to accept client.\n";
                } else {
                    // Set the client socket to non-blocking mode.
                    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
                    if (clientCount < MAX_CLIENTS) {
                        fds[clientCount].fd = clientSocket; // Add the client socket to the poll array
                        fds[clientCount].events = POLLIN; // Monitor for incoming messages
                        clientIndex[clientSocket] = clientCount; // Map the socket to a client number
                        authenticatedClients[clientSocket] = false; // Initially, the client is not authenticated
                        std::cout << "Client " << clientCount << " connected.\n";
                        clientCount++;
                    } else {
                        std::cerr << "Error: maximum clients reached.\n";
                        close(clientSocket);
                    }
                }
            } 
            // Handle incoming data from existing clients.
            else if (fds[i].revents & POLLIN) {
                char buffer[4096];
                memset(buffer, 0, sizeof(buffer));
                int bytesRecv = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytesRecv <= 0) {
                    // If no data is received or an error occurs, disconnect the client.
                    std::cout << "Client " << clientIndex[fds[i].fd] << " disconnected.\n";
                    close(fds[i].fd);
                    fds[i] = fds[clientCount - 1]; // Replace with the last client
                    clientIndex[fds[i].fd] = clientIndex[fds[clientCount - 1].fd]; // Update the map
                    authenticatedClients.erase(fds[clientCount - 1].fd); // Remove the client from the auth map
                    clientIndex.erase(fds[clientCount - 1].fd); // Remove the disconnected client
                    clientCount--;
                    i--; // Adjust the loop index
                } else {
                    // Convert the received data to a string for easier processing.
                    std::string message(buffer, bytesRecv);
                    
                    // Trim trailing newlines (if any)
                    message.erase(message.find_last_not_of("\r\n") + 1);

                    int clientNum = clientIndex[fds[i].fd];

                    // Step 11: Authenticate the client using the password.
                    if (!authenticatedClients[fds[i].fd]) {
                        // The client is not authenticated yet, so we expect a PASS command.
                        if (message.substr(0, 5) == "PASS ") {
                            std::string receivedPassword = message.substr(5);
                            // Trim any trailing newline characters.
                            receivedPassword.erase(receivedPassword.find_last_not_of("\r\n") + 1);

                            if (receivedPassword == serverPassword) {
                                authenticatedClients[fds[i].fd] = true;
                                std::string authSuccess = ":server 001 * :Password accepted\r\n";
                                send(fds[i].fd, authSuccess.c_str(), authSuccess.size(), 0);
                                std::cout << "Client " << clientNum << " authenticated.\n";
                            } else {
                                std::cerr << "Client " << clientNum << " provided an incorrect password.\n";
                                // Optionally send a message back to the client
                                std::string authFail = ":server 402 * :Password incorrect. Please try again.\r\n";
                                send(fds[i].fd, authFail.c_str(), authFail.size(), 0);
                                // Keep the client connected, waiting for the correct password.
                            }
                        } else {
                            // Instead of printing the received message, just inform the client they need to authenticate.
                            std::cerr << "Client " << clientNum << " sent a non-PASS command. Denied.\n";
                            std::string needPass = ":server 401 * :You must authenticate with a PASS command.\r\n";
                            send(fds[i].fd, needPass.c_str(), needPass.size(), 0);
                            // Deny processing of any other commands until authenticated.
                        }
                    } else {
                        // The client is authenticated; print their message.
                        std::cout << "Client " << clientNum << ": " << message << std::endl; // Print authenticated messages
                        
                        // Respond to other commands like PING.
                        if (message.substr(0, 4) == "PING") {
                            std::string pongReply = "PONG " + message.substr(5) + "\r\n";
                            send(fds[i].fd, pongReply.c_str(), pongReply.size(), 0);
                        }
                    }
                }
            }
        }
    }
    // Step 12: Clean up and close the listening socket before exiting.
    close(listenSocket);
    return 0;
}
