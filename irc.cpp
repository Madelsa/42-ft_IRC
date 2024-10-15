/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mahmoud <mahmoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:32:55 by mabdelsa          #+#    #+#             */
/*   Updated: 2024/10/15 22:10:11 by mahmoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <map>
#include <set>
#include <cstdlib> // For std::atoi
#include <sstream>
#include <csignal> // For signal handling

// Maximum number of clients the server can handle simultaneously.
const int MAX_CLIENTS = 100;

class Client {
public:
    int socket;
    int clientNumber;
    bool authenticated;
    std::string nickname;
    std::string username;
    int userType; // 0 for normal user, 1 for operator

    // Default constructor
    Client() : socket(-1), clientNumber(-1), authenticated(false), nickname(""), username(""), userType(0) {}

    // Parameterized constructor
    Client(int socket, int clientNumber) 
        : socket(socket), clientNumber(clientNumber), authenticated(false), nickname(""), username(""), userType(0) {}
};

// Global variable to track the server's running state
volatile sig_atomic_t running = 1;

// Signal handler function
void signalHandler(int signum) {
    std::cout << "\nSignal (" << signum << ") received. Shutting down gracefully...\n";
    running = 0; // Update the running state
}

int main(int argc, char* argv[]) {
    // Step 1: Validate the number of command-line arguments.
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

    // Initialize client count and a map to track clients.
    int clientCount = 1; // Start with only the listening socket
    std::map<int, Client> clients; // Maps socket descriptors to Client objects
    std::set<std::string> nicknames; // To keep track of used nicknames

    // Set up signal handling
    std::signal(SIGINT, signalHandler);

while (running) {
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
                fcntl(clientSocket, F_SETFL, O_NONBLOCK);
                if (clientCount < MAX_CLIENTS) {
                    Client newClient(clientSocket, clientCount);
                    clients[clientSocket] = newClient;
                    fds[clientCount].fd = clientSocket;
                    fds[clientCount].events = POLLIN;
                    std::cout << "Client " << clientCount << " connected.\n";
                    // Send the initial message to the client to prompt for the password.
                    std::string promptMessage = ":server 001 * :Please enter the server password using the PASS command.\r\n";
                    send(clientSocket, promptMessage.c_str(), promptMessage.size(), 0);
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
                std::cout << "Client " << clients[fds[i].fd].clientNumber << " disconnected.\n";
                close(fds[i].fd);
                clients.erase(fds[i].fd);
                fds[i] = fds[clientCount - 1];
                clientCount--;
                i--;
            } else {
                std::string message(buffer, bytesRecv);
                message.erase(message.find_last_not_of("\r\n") + 1); // Trim trailing newlines

                int clientSocket = fds[i].fd;
                Client& currentClient = clients[clientSocket];

                // Step 11: Authenticate the client using the password.
                if (!currentClient.authenticated) {
                    // Expect a PASS command
                    if (message.substr(0, 5) == "PASS ") {
                        std::string receivedPassword = message.substr(5);
                        receivedPassword.erase(receivedPassword.find_last_not_of("\r\n") + 1);

                        if (receivedPassword == serverPassword) {
                            currentClient.authenticated = true;
                            std::string authSuccess = ":server 001 * :Password accepted\r\n";
                            send(fds[i].fd, authSuccess.c_str(), authSuccess.size(), 0);
                            std::cout << "Client " << currentClient.clientNumber << " authenticated.\n";
                            
                            // Prompt for NICK
                            std::string promptNick = ":server 001 * :Enter your nickname using the NICK command.\r\n";
                            send(fds[i].fd, promptNick.c_str(), promptNick.size(), 0);
                        } else {
                            std::cerr << "Client " << currentClient.clientNumber << " provided an incorrect password.\n";
                            std::string authFail = ":server 402 * :Password incorrect. Please try again.\r\n";
                            send(fds[i].fd, authFail.c_str(), authFail.size(), 0);
                        }
                    } else {
                        // If they send any command other than PASS, wait for the correct command
                        std::string needPass = ":server 401 * :You must authenticate with a PASS command.\r\n";
                        send(fds[i].fd, needPass.c_str(), needPass.size(), 0);
                    }
                } else {
                    // Now check for NICK and USER commands
                    if (currentClient.nickname.empty() && message.substr(0, 5) == "NICK ") {
                        std::string nickname = message.substr(5);
                        nickname.erase(nickname.find_last_not_of("\r\n") + 1); // Trim trailing newlines

                        // Check nickname restrictions
                        if (nickname.length() < 1 || nickname.length() > 9) {
                            if (nickname.length() > 9) {
                                nickname = nickname.substr(0, 9); // Truncate to 9 chars
                            }
                            std::string errorMsg = ":server 432 * :Nickname must be between 1 and 9 characters.\r\n";
                            send(fds[i].fd, errorMsg.c_str(), errorMsg.size(), 0);
                        } else if (nicknames.find(nickname) != nicknames.end()) {
                            std::string nickInUse = ":server 433 " + nickname + " :Nickname is already in use.\r\n";
                            send(fds[i].fd, nickInUse.c_str(), nickInUse.size(), 0);
                        } else {
                            currentClient.nickname = nickname;
                            nicknames.insert(nickname);
                            std::cout << "Client " << currentClient.clientNumber << " set nickname to " << nickname << ".\n";

                            // After setting nickname, prompt for username
                            std::string promptUser = ":server 001 * :Enter your username and user type (0 or 1) using the USER command.\r\n";
                            send(fds[i].fd, promptUser.c_str(), promptUser.size(), 0);
                        }
                    } 
                    else if (currentClient.username.empty() && message.substr(0, 5) == "USER ") {
                        std::istringstream iss(message.substr(5));
                        std::string user, userTypeStr;
                        iss >> user >> userTypeStr;

                        // Validate user type
                        int userType = std::atoi(userTypeStr.c_str());
                        if (userType < 0 || userType > 1) {
                            std::string errorMsg = ":server 461 * :Invalid user type. Must be 0 (normal) or 1 (operator).\r\n";
                            send(fds[i].fd, errorMsg.c_str(), errorMsg.size(), 0);
                            continue;
                        }

                        // Check for exactly two arguments and validate username is not empty
                        if (user.empty() || userTypeStr.empty()) {
                            std::string errorMsg = ":server 461 * :USER command requires a username and user type (0 or 1).\r\n";
                            send(fds[i].fd, errorMsg.c_str(), errorMsg.size(), 0);
                            continue;
                        }

                        currentClient.username = user;
                        currentClient.userType = userType; // Store userType
                        std::cout << "Client " << currentClient.clientNumber << " set username to " << currentClient.username << ".\n";

                        // After both NICK and USER have been set, welcome the client
                        if (!currentClient.nickname.empty()) {
                            std::string welcomeMsg = ":server 001 " + currentClient.nickname + " :Welcome to the server!\r\n";
                            send(fds[i].fd, welcomeMsg.c_str(), welcomeMsg.size(), 0);
                        }
                    } else {
                        // After registration is complete, notify about already registered
                        if (!currentClient.nickname.empty() && !currentClient.username.empty()) {
                            std::string alreadyRegistered = ":server 462 * :You have already registered.\r\n";
                            send(fds[i].fd, alreadyRegistered.c_str(), alreadyRegistered.size(), 0);
                        }
                    }
                }
            }
        }
    }
}



    // Cleanup and shutdown
    for (int i = 1; i < clientCount; i++) 
    {
        close(fds[i].fd);
    }
    close(listenSocket);
    std::cout << "Server shutting down...\n";
    return 0;
}
