/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mahmoud <mahmoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 13:45:13 by mahmoud           #+#    #+#             */
/*   Updated: 2024/10/21 04:12:30 by mahmoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"
#include "Utils.hpp"

Server::Server(int port, const std::string& password) 
    : port(port), serverPassword(password), running(1) {
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

void Server::run() {
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
                handleClientInput(i, clientCount, clients, fds);
            }
        }
    }

    cleanupClients(clientCount, fds);
}

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

void Server::removeClientFD(int i, int &clientCount, std::map<int, Client> &clients, struct pollfd fds[])
{
    close(fds[i].fd);
    clients.erase(fds[i].fd);
    fds[i] = fds[clientCount - 1];
    fds[clientCount - 1].fd = -1;
    clientCount--;
    i--;
}

void Server::handleClientInput(int i, int &clientCount, std::map<int, Client> &clients, struct pollfd fds[]) {
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
                    currentClient.handlePassCommand(&currentClient, params, serverPassword, clients);
                    currentClient.setRegisterSteps(1, true);
                }
                else 
                    currentClient.getServerReplies().push_back(ERR_ALREADYREGISTERED(std::string("ircserver")));
            } 
            else if (!currentClient.getRegisterSteps(1) && command == "USER") {
                currentClient.disconnectClient(&currentClient);
                removeClientFD(i, clientCount, clients, fds);
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
                        }
                    }
                    else
                        currentClient.getServerReplies().push_back(ERR_ALREADYREGISTERED(std::string("ircserver")));
                }
                if (!currentClient.getHasSentWelcomeMessage()) {
                    currentClient.MOTD(&currentClient);
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

