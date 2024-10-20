/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mahmoud <mahmoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 13:45:13 by mahmoud           #+#    #+#             */
/*   Updated: 2024/10/20 21:25:52 by mahmoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"
#include "Utils.hpp"

Server::Server(int port, const std::string& password) 
    : port(port), serverPassword(password), running(1) {
    // setupSocket();
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
            close(fds[i].fd);
            clients.erase(fds[i].fd);
            fds[i] = fds[clientCount - 1];
            fds[clientCount - 1].fd = -1;
            clientCount--;
            i--;
        }
    } else if (bytesRecv <= 0) {
        std::cout << "Client " << clients[fds[i].fd].getClientNumber() << " disconnected.\n";
        close(fds[i].fd);
        clients.erase(fds[i].fd);
        fds[i] = fds[clientCount - 1];
        fds[clientCount - 1].fd = -1;
        clientCount--;
        i--;
    } else {
        std::string message(buffer, bytesRecv);
        message.erase(message.find_last_not_of("\r\n") + 1);
        Client &currentClient = clients[fds[i].fd];

        // Trim and split the message
        message = ft_trim(message);
        std::vector<std::string> splitMessage = ft_split(message, '\n');
        for (std::vector<std::string>::size_type j = 0; j < splitMessage.size(); ++j) {
            std::string line = splitMessage[j];
            // Trim each line
            std::string trimmedLine = ft_trim(line);
            if (trimmedLine.empty()) continue;

            currentClient.getClientMessages().push_back(trimmedLine);
            // Handle command processing
            std::vector<std::string> tokens = ft_split(trimmedLine, ' ');
            if (tokens.empty()) continue;

            std::string command = tokens[0];
            std::vector<std::string> params(tokens.begin() + 1, tokens.end());

            // Handle commands
            if (command == "CAP") {
                currentClient.handleCapCommand(&currentClient, params);
            } else if (currentClient.getRegisterSteps(0) && command == "PASS") {
                currentClient.handlePassCommand(&currentClient, params, serverPassword, clients);
                currentClient.setRegisterSteps(1, true);
            } else if (!currentClient.getRegisterSteps(1) && command == "USER") {
                currentClient.disconnectClient(&currentClient, clients);
            } else if (currentClient.getRegisterSteps(1) && (command == "USER" || command == "NICK")) {
                if (command == "NICK") {
                    currentClient.handleNickCommand(&currentClient, params, nicknames);
                } else if (command == "USER") {
                    currentClient.handleUserCommand(&currentClient, params);
                }
                if (!currentClient.getHasSentWelcomeMessage()) {
                    currentClient.MOTD(&currentClient);
                }
            } else if (command == "PING") {
                currentClient.getServerReplies().push_back(RPL_PONG(idFormat(currentClient.getNickname(), currentClient.getUsername()), params[0]));
            }
            currentClient.sendRepliesToClient(&currentClient);
            currentClient.clearServerReplies(); // Clear replies after sending
        }
    }
}

void Server::cleanupClients(int clientCount, struct pollfd fds[]) {
    for (int i = 1; i < clientCount; i++) {
        close(fds[i].fd);
    }
    close(listenSocket);
}

