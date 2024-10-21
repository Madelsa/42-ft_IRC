/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mabdelsa <mabdelsa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 13:45:13 by mahmoud           #+#    #+#             */
/*   Updated: 2024/10/21 16:14:46 by mabdelsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"
#include "Client.hpp"
#include "Utils.hpp"

Server::Server(int port, const std::string& password) 
    : port(port), serverPassword(password), running(1) {
}


// Retrieves the current date and time formatted as "DD-MM-YYYY HH:MM:SS".
// 
// This function uses the standard C++ time library to get the current time 
// from the system clock. It then converts this time into local time format 
// and formats it into a string representation. The function returns the 
// formatted date and time as a std::string, which can be used for logging 
// or displaying the server's current time.
std::string Server::getCurrentDateTime()
{
    // Get the current time
    std::time_t rawTime;
    std::time(&rawTime);

    // Convert to local time
    struct tm *localTime = std::localtime(&rawTime);

    // Buffer to hold the formatted date and time
    char buffer[80];

    // Format: DD-MM-YYYY HH:MM:SS
    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", localTime);

    return std::string(buffer);
}


volatile sig_atomic_t running = 1;

void signalHandler(int signum)
{
    std::cout << "\nSignal (" << signum << ") received. Shutting down...\n";
    
    running = 0;
}

void Server::setupSocket() {
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        throw std::runtime_error("Error: could not create socket.");
    }

    // Set socket options
    int opt = 1;
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        close(listenSocket);
        throw std::runtime_error("Error: could not bind socket.");
    }

    listen(listenSocket, 5);
    std::cout << "Server listening on port " << port << "\n";
}

// Main loop for the server, managing incoming client connections and processing their input.
// 
// This function initializes the server, sets up the listening socket, and enters 
// a loop where it waits for incoming client connections and messages. It utilizes 
// the poll function to efficiently handle multiple client sockets. When a new 
// client connects, it calls acceptClient to add the client to the list. For each 
// client that sends data, it processes the input using handleClientInput. The loop 
// continues until a shutdown signal is received. After exiting the loop, it 
// cleans up by closing all client connections and freeing any resources.
void Server::run() {
    serverStartTime = getCurrentDateTime();
    setupSocket();
    std::map<int, Client> clients;
    struct pollfd fds[MAX_CLIENTS];
    memset(fds, 0, sizeof(fds));
    int clientCount = 1;
    fds[0].fd = listenSocket;
    fds[0].events = POLLIN;
    
    // Set up signal handling
    std::signal(SIGINT, signalHandler);

    while (running) {
        int pollCount = poll(fds, clientCount, -1);
        if (pollCount == -1) {
            break;
        }

        for (int i = 0; i < clientCount; i++) {
            if (fds[i].fd == listenSocket && (fds[i].revents & POLLIN)) {
                acceptClient(clientCount, clients, fds);
            } else if (fds[i].revents & POLLIN) {
                handleClientInput(i, clientCount, clients, fds, serverStartTime);
            }
        }
    }

    cleanupClients(clientCount, fds);
}


// Accepts new client connections, updating the clients map and file descriptor array.
// This function is called when a new client connection is detected on the listening 
// socket. It uses the accept system call to create a new socket for the connected 
// client. If successful, the function sets the socket to non-blocking mode and 
// adds the new client to the clients map. It also updates the file descriptor array 
// to include the new client, allowing for future polling of this client for 
// incoming data. If the maximum number of clients is reached, the function will 
// close the new socket and print an error message.
void Server::acceptClient(int &clientCount, std::map<int, Client> &clients, struct pollfd fds[]) {
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    int clientSocket = accept(listenSocket, (sockaddr *)&client, &clientSize);
    if (clientSocket == -1) {
        std::cerr << "Error: failed to accept client.\n";
        return;
    } else {
        fcntl(clientSocket, F_SETFL, O_NONBLOCK);
        if (clientCount < MAX_CLIENTS) {
            clients[clientSocket] = Client(clientSocket, clientCount);
            fds[clientCount].fd = clientSocket;
            fds[clientCount].events = POLLIN;
            std::cout << "Client " << clientCount << " connected.\n";
            clientCount++;
        } else {
            std::cerr << "Error: maximum clients reached.\n";
            close(clientSocket);
        }
    }
}

// Removes a client from the server, closing their socket and updating the client list and file descriptor array.
// 
// This function is responsible for cleaning up resources associated with a disconnected client. 
// It first closes the client's socket using the close system call to free up the associated resources. 
// Then, it removes the client from the 'clients' map, which holds all connected clients. 
// After that, it updates the file descriptor array 'fds' to ensure it does not leave empty slots. 
// The last client in the array is moved to the current index to maintain a contiguous list of active file descriptors. 
// Finally, the function decrements the total client count, ensuring that the next call to poll or accept uses the correct number of active clients.
// 
// Note: The variable 'i' is also decremented to account for the fact that the current index is no longer valid after the removal.
void Server::removeClientFD(int i, int &clientCount, std::map<int, Client> &clients, struct pollfd fds[])
{
    close(fds[i].fd);
    clients.erase(fds[i].fd);
    fds[i] = fds[clientCount - 1];
    fds[clientCount - 1].fd = -1;
    clientCount--;
    i--;
}


// Processes incoming data from connected clients, handling complete messages and dispatching commands.
// 
// This function is called when a client socket is ready to read. It reads the incoming 
// data into a buffer and checks for any errors or disconnections. If data is received, 
// it appends the data to the client's partial buffer. The function looks for complete 
// messages that end with a newline character. For each complete message, it trims 
// whitespace and splits the message into tokens. Based on the first token (the command), 
// it calls the appropriate handling function for that command (e.g., CAP, NICK, USER). 
// If a client disconnects or an error occurs during receiving data, the function handles 
// cleanup for that client.
void Server::handleClientInput(int i, int &clientCount, std::map<int, Client> &clients, struct pollfd fds[], std::string serverStartTime) {
    std::vector<std::string> nicknames;    
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    int bytesRecv = recv(fds[i].fd, buffer, sizeof(buffer), 0);

    if (bytesRecv == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return; // No data available right now; it's okay to ignore this
        } else {
            std::cerr << "Error receiving data from client: " << strerror(errno) << "\n";
            removeClientFD(i, clientCount, clients, fds);
        }
    } else if (bytesRecv <= 0) {
        std::cout << "Client " << clients[fds[i].fd].getClientNumber() << " disconnected.\n";
        removeClientFD(i, clientCount, clients, fds);
    } else {
    
        Client &currentClient = clients[fds[i].fd];
        currentClient.getPartialBuffer().insert(currentClient.getPartialBuffer().end(), buffer, buffer + bytesRecv);

        // Check if a complete message (ending with '\n') is in the buffer
        size_t newlinePos = currentClient.getPartialBuffer().find('\n');
        while (newlinePos != std::string::npos) {
            // Extract the complete message from the buffer
           std::string completeMessage = currentClient.getPartialBuffer().substr(0, newlinePos);

            // Remove the processed message from partialBuffer
            currentClient.setPartialBuffer(currentClient.getPartialBuffer().erase(0, newlinePos + 1));
            // Trim and handle the complete message
            completeMessage.erase(completeMessage.find_last_not_of("\r\n") + 1);
            completeMessage = ft_trim(completeMessage);

            // If the message is empty, just skip it and continue to the next message
            if (completeMessage.empty()) {
                newlinePos = currentClient.getPartialBuffer().find('\n');
                continue;
            }

            currentClient.getClientMessages().push_back(completeMessage);

            std::vector<std::string> tokens = ft_split(completeMessage, ' ');
            if (tokens.empty()) continue;

            std::string command = tokens[0];
            std::vector<std::string> params(tokens.begin() + 1, tokens.end());

            // Debugging output
            currentClient.printClientMessages(&currentClient);
            std::cout << "Command: " << command << "\n";
            for (std::vector<std::string>::size_type j = 0; j < params.size(); ++j)
            {
                std::string param = params[j];
                std::cout << "Parameter[" << j << "]: " << param << "\n";
            }
            currentClient.clearClientMessages();

            // Handle commands based on your existing logic
            if (command == "CAP") {
                currentClient.handleCapCommand(&currentClient, params);
            } 
            else if (currentClient.getRegisterSteps(0) && command == "PASS") {
                if (!currentClient.getHasSentWelcomeMessage())
                {
                    currentClient.handlePassCommand(&currentClient, params, serverPassword);
                    currentClient.setRegisterSteps(1, true);
                }
                else 
                    currentClient.getServerReplies().push_back(ERR_ALREADYREGISTERED(std::string("ircserv")));
            } 
            else if (!currentClient.getRegisterSteps(1) && command == "USER") {
                currentClient.disconnectClient(&currentClient);
                removeClientFD(i, clientCount, clients, fds);
                return;
            } 
            else if (currentClient.getRegisterSteps(1) && (command == "USER" || command == "NICK")) {
                if (command == "NICK") {
                    currentClient.handleNickCommand(&currentClient, params, nicknames);
                } 
                else if (command == "USER") {
                    if (!currentClient.getHasSentWelcomeMessage())
                    {
                        currentClient.handleUserCommand(&currentClient, params);
                        if (!currentClient.getHasSentPass())
                        {
                            currentClient.disconnectClient(&currentClient);
                            removeClientFD(i, clientCount, clients, fds);
                            return;
                        }
                    }
                    else
                        currentClient.getServerReplies().push_back(ERR_ALREADYREGISTERED(std::string("ircserv")));
                }
                if (!currentClient.getHasSentWelcomeMessage()) {
                    currentClient.MOTD(&currentClient, serverStartTime);
                }
            } 
            else if (command == "PING") {
                currentClient.getServerReplies().push_back(RPL_PONG(idFormat(currentClient.getNickname(), currentClient.getUsername()), params[0]));
            }
            currentClient.sendRepliesToClient(&currentClient);
            currentClient.clearServerReplies();

            // Find the next newline character in the updated buffer
            newlinePos = currentClient.getPartialBuffer().find('\n');
        }
    }
}

void Server::cleanupClients(int clientCount, struct pollfd fds[]) {
    for (int i = 1; i < clientCount; i++) {
        close(fds[i].fd);
    }
    close(listenSocket);
}
