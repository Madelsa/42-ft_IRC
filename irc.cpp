/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mabdelsa <mabdelsa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:32:55 by mabdelsa          #+#    #+#             */
/*   Updated: 2024/10/16 14:06:33 by mabdelsa         ###   ########.fr       */
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
    std::cout << "\nSignal (" << signum << ") received. Shutting down...\n";
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
    //AF_INET: Address family of the socket domain: IPV4
    //SOCK_STREAM: Used for TCP connections.
    //0: Automatically set the protocol as the SOCK_STRAM. TCP in our case.
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
    // - The `pollfd` structure is used by the `poll` system call to monitor multiple file descriptors for events.
    // - `fds` is an array of `pollfd` structures with a maximum size of `MAX_CLIENTS` to track the listening socket and connected clients.
    // - The array is first initialized to zero using `memset` to ensure all fields start in a known state.
    // - The first element, `fds[0]`, is set up to monitor the listening socket (`listenSocket`):
    //   - `fds[0].fd` is assigned the file descriptor of the listening socket.
    //   - `fds[0].events` is set to `POLLIN` to monitor for "readable" events, specifically new incoming connections.
    // - Additional client sockets will be added to this array later as they connect, allowing the server to monitor and handle multiple connections simultaneously.
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
    // Call the poll function to monitor multiple file descriptors (including the listening socket and client sockets) for events.
    // - `fds`: The array of `pollfd` structures set up earlier. It includes the listening socket and all currently connected clients.
    // - `clientCount`: The number of file descriptors currently being monitored. This starts with 1 (for the listening socket) and increases as clients connect.
    // - `-1`: The timeout value. A value of `-1` means `poll` will wait indefinitely for an event to occur (blocking behavior).
    // - If `poll` returns a value of `-1`, it indicates an error occurred, and the server logs an error message and exits the loop.
    int pollCount = poll(fds, clientCount, -1);
    if (pollCount == -1) {
        std::cerr << "Error: poll failed.\n";
        break;
    }

    // Step 10: Main server loop: monitors and processes events for each file descriptor in the fds array.
    // This loop iterates over all monitored file descriptors (fds[i]) and handles two main events:
    // 1. New client connections on the listening socket.
    // 2. Incoming data from connected clients.
    //
    // The loop works as follows:
    // - Iterate through each file descriptor in fds (from index 0 to clientCount - 1).
    // - If the current file descriptor is the listening socket (fds[i].fd == listenSocket) and
    //   a POLLIN event is detected, this indicates a new client is trying to connect.
    //   - The server calls accept() to accept the incoming connection and create a new client socket.
    //   - If accept() is successful, the new client socket is set to non-blocking mode using fcntl().
    //   - If the server has not reached its maximum client limit (MAX_CLIENTS), it creates a new
    //     Client object, adds it to the clients map using the clientSocket as the key, and then
    //     adds this client socket to the fds array for monitoring POLLIN events (incoming data).
    //   - A welcome message is immediately sent to the client to prompt for the password using
    //     the PASS command. If the maximum client limit is reached, the connection is closed.
    //
    // - If the current file descriptor is not the listening socket, but it has a POLLIN event,
    //   this indicates that there is incoming data from an existing client.
    //   - The server reads the data into a buffer using recv().
    //   - If recv() returns 0 or a negative value, it indicates the client has disconnected or
    //     an error occurred. In this case, the server:
    //     - Closes the client’s socket.
    //     - Removes the client from the clients map and updates the fds array.
    //     - Decreases clientCount and adjusts the loop index (i--) to ensure no fds entry is skipped.
    //   - If recv() returns a positive value (indicating data was received), the buffer is converted
    //     to a std::string (message), and trailing newline characters are removed.
    //     The server can then further process the client's message (e.g., handling commands).
  
for (int i = 0; i < clientCount; i++) {
    // Check if there is a new incoming connection on the listen socket. Client attempting to connect.
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
        
        // Handle error for recv
        if (bytesRecv == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No data available right now; it's okay to ignore this
                continue; // Continue to the next client
            } else {
                std::cerr << "Error receiving data from client: " << strerror(errno) << "\n";
                // Handle the disconnection logic as necessary
                close(fds[i].fd);
                clients.erase(fds[i].fd);
                fds[i] = fds[clientCount - 1]; // Move the last client into the current spot
                fds[clientCount - 1].fd = -1; // Reset the last position in fds to an invalid fd
                clientCount--;
                i--; // Decrement i to ensure we don't skip the next client
            }
        } else if (bytesRecv <= 0) {
            // Handle client disconnection
            std::cout << "Client " << clients[fds[i].fd].clientNumber << " disconnected.\n";
            close(fds[i].fd);
            clients.erase(fds[i].fd);
            // Remove the client socket from the fds array
            fds[i] = fds[clientCount - 1]; // Move the last client into the current spot
            fds[clientCount - 1].fd = -1; // Reset the last position in fds to an invalid fd
            clientCount--;
            i--; // Decrement i to ensure we don't skip the next client
        } else {
            std::string message(buffer, bytesRecv);
            message.erase(message.find_last_not_of("\r\n") + 1); // Trim trailing newlines

            int clientSocket = fds[i].fd;
            Client& currentClient = clients[clientSocket];

            if (message.substr(0, 5) == "PING ") {
                std::string pongResponse = "PONG " + message.substr(5) + "\r\n";
                send(fds[i].fd, pongResponse.c_str(), pongResponse.size(), 0);
                continue; // Skip further processing for this message
            }

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
                } 
            } else {
                // Check for NICK command
                if (message.substr(0, 5) == "NICK ") {
                    std::string nickname = message.substr(5);
                    nickname.erase(nickname.find_last_not_of("\r\n") + 1); // Trim trailing newlines

                    // Check nickname restrictions
                    bool validNickname = true;

                    // Check if the first character is a digit (if you want to disallow this)
                    if (!nickname.empty() && isdigit(nickname[0])) {
                        validNickname = false;
                    }

                    // Check for invalid characters
                    for (std::string::iterator it = nickname.begin(); it != nickname.end(); ++it) {
                        if (!isalnum(*it) && *it != '[' && *it != ']' && *it != '{' && *it != '}'
                            && *it != '|' && *it != '-' && *it != '_') { // Add any other valid characters you want to allow
                            validNickname = false;
                            break;
                        }
                    }

                    // If the nickname is not valid, send the ERR_ERRONEUSNICKNAME error
                    if (!validNickname) {
                        std::string errorMsg = ":server 432 * :Erroneous nickname.\r\n";
                        send(fds[i].fd, errorMsg.c_str(), errorMsg.size(), 0);
                    } else if (nickname.length() < 1 || nickname.length() > 9) {
                        std::string errorMsg = ":server 432 * :Nickname must be between 1 and 9 characters.\r\n";
                        send(fds[i].fd, errorMsg.c_str(), errorMsg.size(), 0);
                    } else if (nicknames.find(nickname) != nicknames.end()) {
                        std::string nickInUse = ":server 433 " + (currentClient.nickname.empty() ? "*" : currentClient.nickname) + " " + nickname + " :Nickname is already in use.\r\n";
                        send(fds[i].fd, nickInUse.c_str(), nickInUse.size(), 0);
                    } else {
                        // If the nickname is valid and unique, proceed with setting it
                        if (!currentClient.nickname.empty()) {
                            // If the client is changing their nickname, remove the old one from the set
                            nicknames.erase(currentClient.nickname);
                        }
                        currentClient.nickname = nickname;
                        nicknames.insert(nickname);
                        std::cout << "Client " << currentClient.clientNumber << " changed nickname to " << nickname << ".\n";

                        // Notify the client about the successful nickname change
                        std::string successMsg = "Nickname changed successfully to " + nickname + ".\r\n";
                        send(fds[i].fd, successMsg.c_str(), successMsg.size(), 0);
                        std::string promptUser = "Enter your username and user type (0 or 1) using the USER command.\r\n";
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
