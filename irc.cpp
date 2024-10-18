/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mahmoud <mahmoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 14:26:54 by mahmoud           #+#    #+#             */
/*   Updated: 2024/10/18 23:02:29 by mahmoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <iostream>
// #include <cstring>
// #include <poll.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <netinet/in.h>
// #include <map>
// #include <vector>
// #include <sstream>
// #include <csignal>
// #include <cstdlib>

// const int MAX_CLIENTS = 100;

// class Client {
// public:
//     int socket;
//     int clientNumber;
//     bool isRegistered;
//     bool hasSentPass;
//     std::vector<std::string> messages;
//     std::vector<std::string> serverReplies;
//     bool conRegi[1]; // To handle CAP command registration state

//     // Default constructor
//     Client() 
//         : socket(-1), clientNumber(-1), isRegistered(false), hasSentPass(false) {
//         messages.clear();
//         serverReplies.clear();
//         conRegi[0] = false;
//     }

//     // Parameterized constructor
//     Client(int socket, int clientNumber) 
//         : socket(socket), clientNumber(clientNumber), isRegistered(false), hasSentPass(false) {
//         messages.clear();
//         serverReplies.clear();
//         conRegi[0] = false;
//     }
// };

// volatile sig_atomic_t running = 1;

// void signalHandler(int signum) {
//     std::cout << "\nSignal (" << signum << ") received. Shutting down gracefully...\n";
//     running = 0;
// }

// // Function to trim whitespace
// std::string ft_trim(const std::string &str) {
//     std::size_t start = str.find_first_not_of(" \n\r\t");
//     std::size_t end = str.find_last_not_of(" \n\r\t");
//     if (start == std::string::npos || end == std::string::npos) {
//         return "";
//     }
//     return str.substr(start, end - start + 1);
// }

// // Function to split strings by a delimiter
// std::vector<std::string> ft_split(std::string str, char delimiter) {
//     std::vector<std::string> result;
//     std::string word;
    
//     for (std::size_t i = 0; i < str.length(); ++i) {
//         if (str[i] != delimiter) {
//             word += str[i];
//         } else {
//             if (!word.empty()) {
//                 result.push_back(word);
//                 word.clear();
//             }
//         }
//     }
//     if (!word.empty()) {
//         result.push_back(word);
//     }
//     return result;
// }

// void sendRepliesToClient(Client *client) 
// {
//     // Check if the client socket is valid before attempting to send
//     for (std::vector<std::string>::size_type j = 0; j < client->serverReplies.size(); ++j) {
//         ssize_t bytesSent = send(client->socket, client->serverReplies[j].c_str(), client->serverReplies[j].length(), 0);
        
//         // Handle potential errors
//         if (bytesSent < 0) {
//             perror("Failed to send message to client");
//             // Close the socket if sending fails
//             close(client->socket);

//             client->socket = -1; // Mark the socket as closed
//             return;
//         }
//     }
// }

// void printClientMessages(Client *client) {
        
//         if (client->socket == -1) {
//             return; // Socket is closed, do not attempt to send
//         }
        // std::cout << "----------------------------------------------------------------\n";
        // for (std::vector<std::string>::const_iterator it = client->messages.begin(); it != client->messages.end(); ++it) {
        //     std::cout << "Client " << client->clientNumber << " sent: " << *it << "\n";
        // }
// }



// void disconnectClient(Client *client, std::map<int, Client>& clients) {

//     std::string errorMessage = "ERR_PASSWDMISMATCH (464) :Password incorrect\r\n";
//     client->serverReplies.push_back(errorMessage); // Store the error message

//     std::string closeMessage = "ERROR :Password incorrect. Closing connection.\r\n";
//     client->serverReplies.push_back(closeMessage); // Store the close message

//     // Send replies before closing the socket
//     sendRepliesToClient(client);

//     client->messages.clear();

//     // Close the client's socket
//     close(client->socket);

//     // Set the socket to -1 to mark it as closed
//     client->socket = -1;

//     // Erase the client from the map
//     clients.erase(client->socket);
// }



// void handlePassCommand(Client *client, const std::vector<std::string> &params, const std::string &expectedPassword, std::map<int, Client>& clients) {
//     if (params.size() != 1) {
//         // Not enough parameters error
//         std::string errorMessage = "ERR_NEEDMOREPARAMS (461) :Not enough parameters\r\n";
//         client->serverReplies.push_back(errorMessage); // Store the error message
//         return;
//     }

//     const std::string &passwordAttempt = params[0];
//     if (passwordAttempt == expectedPassword) {
//         // Password accepted
//         std::string successMessage = "Password accepted\r\n";
//         client->serverReplies.push_back(successMessage); // Store the error message
//         // Further registration logic can be placed here
//     } else 
//     {
//         disconnectClient(client, clients);
//     }
// }



// void handleCapCommand(Client *client, const std::vector<std::string> &params) {
//     if (params.size() > 0 && params[0] == "LS") {
//         client->conRegi[0] = true;
//         client->serverReplies.push_back(":server CAP * LS :capabilities_here \r\n");
//     } else if (client->conRegi[0]) {
//         if (params.size() == 1 && params[0] == "REQ") {
//             client->serverReplies.push_back(":server CAP * REQ :requested_capabilities \r\n");
//         } else if (params.size() == 1 && params[0] == "NAK") {
//             client->serverReplies.push_back(":server CAP * NAK :rejected_capabilities \r\n");
//         } else if (params.size() == 1 && params[0] == "ACK") {
//             client->serverReplies.push_back(":server CAP * ACK :accepted_capabilities \r\n");
//         } else if (params.size() == 1 && params[0] == "END") {
//             client->isRegistered = true;
//         }
//     }
// }

// int main(int argc, char* argv[]) {
//     if (argc != 3) {
//         std::cerr << "Usage: ./ircserv <port> <password>\n";
//         return 1;
//     }

//     int port = std::atoi(argv[1]);
//     if (port <= 0 || port > 65535) {
//         std::cerr << "Error: Invalid port number. Please provide a valid port (1-65535).\n";
//         return 1;
//     }

//     std::string serverPassword = argv[2];

//     int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (listenSocket < 0) {
//         std::cerr << "Error: could not create socket.\n";
//         return 1;
//     }

//     sockaddr_in serverAddr;
//     memset(&serverAddr, 0, sizeof(serverAddr));
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_addr.s_addr = INADDR_ANY;
//     serverAddr.sin_port = htons(port);

//     if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
//         std::cerr << "Error: could not bind socket.\n";
//         return 1;
//     }

//     listen(listenSocket, 5);
//     std::cout << "Server listening on port " << port << "\n";

//     struct pollfd fds[MAX_CLIENTS];
//     memset(fds, 0, sizeof(fds));
//     fds[0].fd = listenSocket;
//     fds[0].events = POLLIN;

//     int clientCount = 1;
//     std::map<int, Client> clients;

//     std::signal(SIGINT, signalHandler);

//     while (running) {
//         int pollCount = poll(fds, clientCount, -1);
//         if (pollCount == -1) {
//             std::cerr << "Error: poll failed.\n";
//             break;
//         }

//         for (int i = 0; i < clientCount; i++) {
//             if (fds[i].fd == listenSocket && (fds[i].revents & POLLIN)) {
//                 sockaddr_in client;
//                 socklen_t clientSize = sizeof(client);
//                 int clientSocket = accept(listenSocket, (sockaddr*)&client, &clientSize);
//                 if (clientSocket == -1) {
//                     std::cerr << "Error: failed to accept client.\n";
//                 } else {
//                     fcntl(clientSocket, F_SETFL, O_NONBLOCK);
//                     if (clientCount < MAX_CLIENTS) {
//                         clients[clientSocket] = Client(clientSocket, clientCount);
//                         fds[clientCount].fd = clientSocket;
//                         fds[clientCount].events = POLLIN;
//                         std::cout << "Client " << clientCount << " connected.\n";
//                         clientCount++;
//                     } else {
//                         std::cerr << "Error: maximum clients reached.\n";
//                         close(clientSocket);
//                     }
//                 }
//             } else if (fds[i].revents & POLLIN) {
//                 char buffer[4096];
//                 memset(buffer, 0, sizeof(buffer));
//                 int bytesRecv = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                

//                 // Handle error for recv
//                 if (bytesRecv == -1) {
//                     if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                         // No data available right now; it's okay to ignore this
//                         continue; // Continue to the next client
//                     } else {
//                         std::cerr << "Error receiving data from client: " << strerror(errno) << "\n";
//                         // Handle the disconnection logic as necessary
//                         close(fds[i].fd);
//                         clients.erase(fds[i].fd);
//                         fds[i] = fds[clientCount - 1]; // Move the last client into the current spot
//                         fds[clientCount - 1].fd = -1; // Reset the last position in fds to an invalid fd
//                         clientCount--;
//                         i--; // Decrement i to ensure we don't skip the next client
//                     }
//                 } else if (bytesRecv <= 0) {
//                     // Handle client disconnection
//                     std::cout << "Client " << clients[fds[i].fd].clientNumber << " disconnected.\n";
//                     close(fds[i].fd);
//                     clients.erase(fds[i].fd);
//                     // Remove the client socket from the fds array
//                     fds[i] = fds[clientCount - 1]; // Move the last client into the current spot
//                     fds[clientCount - 1].fd = -1; // Reset the last position in fds to an invalid fd
//                     clientCount--;
//                     i--; // Decrement i to ensure we don't skip the next client
//                 } else {
//                     std::string message(buffer, bytesRecv);
//                     message.erase(message.find_last_not_of("\r\n") + 1);
//                     Client& currentClient = clients[fds[i].fd];

//                     // Trim and split the message
//                     message = ft_trim(message);
//                     std::vector<std::string> splitMessage = ft_split(message, '\n');
//                     for (std::vector<std::string>::size_type j = 0; j < splitMessage.size(); ++j) {
                        
//                         std::string line = splitMessage[j];
//                         // Trim each line
//                         std::string trimmedLine = ft_trim(line);
//                         if (trimmedLine.empty()) continue; // Skip empty lines

//                         currentClient.messages.push_back(trimmedLine);
//                         // std::cout << "Client " << currentClient.clientNumber << " sent: " << trimmedLine << "\n";

//                         // Split command and parameters
//                         std::vector<std::string> tokens = ft_split(trimmedLine, ' ');

//                         if (tokens.empty()) continue;
                        
//                         std::string command = tokens[0];
//                         std::vector<std::string> params(tokens.begin() + 1, tokens.end());

//                         // Debugging output
//                         printClientMessages(&currentClient);
//                         std::cout << "Command: " << command << "\n";
//                         for (std::vector<std::string>::size_type j = 0; j < params.size(); ++j) {
//                             std::string param = params[j];
//                             std::cout << "Parameter[" << j << "]: " << param << "\n";
//                         }
//                         currentClient.messages.clear();


//                         // Handle commands
//                         if (command == "CAP") {
//                             handleCapCommand(&currentClient, params);
//                         } else if (command == "PASS") {
//                             currentClient.hasSentPass = true;
//                             handlePassCommand(&currentClient, params, serverPassword, clients);
                                
//                         } else if (command == "PING") {
//                             // Ensure that we have one argument for PING
//                             if (params.size() != 1) {
//                                 std::string errorMessage = "ERR_NEEDMOREPARAMS (461) :Not enough parameters\r\n";
//                                 currentClient.serverReplies.push_back(errorMessage); // Store the error message
//                             } else {
//                                 // Respond with PONG using the server name and the same argument
//                                 std::string pongMessage = "PONG server_name " + params[0] + "\r\n";
//                                 currentClient.serverReplies.push_back(pongMessage);
//                             }
//                         }

//                         // if (currentClient.isRegistered) {
//                         //     // Optionally, you might want to disconnect the client immediately after
//                         //     if (!currentClient.hasSentPass)
//                         //     {
//                         //        disconnectClient(&currentClient);
//                         //     }
//                         // }
                        

//                         // Send replies to the client
//                         // printClientMessages(&currentClient);
//                         sendRepliesToClient(&currentClient);
//                         currentClient.serverReplies.clear(); // Clear replies after sending
//                     }
//                 }
//             }
//         }
//     }

//     for (int i = 1; i < clientCount; i++) 
//     {
//         close(fds[i].fd);
//     }
//     close(listenSocket);
//     std::cout << "Server shutting down...\n";
//     return 0;
// }



#include <iostream>
#include <cstring>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <map>
#include <vector>
#include <sstream>
#include <csignal>
#include <cstdlib>

const int MAX_CLIENTS = 100;

class Client {
private:
    int socket;
    int clientNumber;
    bool isRegistered;
    bool hasSentPass;
    std::string nickname;  // Added for storing nickname
    std::string username;  // Added for storing username
    std::string realName;  // Added for storing real name


public:

     // Default constructor
    Client() 
        : socket(-1), clientNumber(-1), isRegistered(false), hasSentPass(false) {
        clientMessages.clear();
        serverReplies.clear();
        conRegi[0] = false;
        nickname = "";  // Initialize nickname
        username = "";  // Initialize username
        realName = "";  // Initialize real name
    }


    // Parameterized constructor
    Client(int socket, int clientNumber) 
        : socket(socket), clientNumber(clientNumber), isRegistered(false), hasSentPass(false) {
        clientMessages.clear();
        serverReplies.clear();
        conRegi[0] = false;
        nickname = "";  // Initialize nickname
        username = "";  // Initialize username
        realName = "";  // Initialize real name
    }
    std::vector<std::string> clientMessages;
    std::vector<std::string> serverReplies;
    bool conRegi[1]; // To handle CAP command registration state

    // Getter methods
    int getSocket() const { return socket; }
    int getClientNumber() const { return clientNumber; }
    bool isClientRegistered() const { return isRegistered; }
    bool hasClientSentPass() const { return hasSentPass; }
    const std::string& getNickname() const { return nickname; }
    const std::string& getUsername() const { return username; }
    const std::string& getRealName() const { return realName; }

    // Setter methods
    void setSocket(int sock) { socket = sock; }
    void setClientNumber(int num) { clientNumber = num; }
    void setIsRegistered(bool reg) { isRegistered = reg; }
    void setHasSentPass(bool sent) { hasSentPass = sent; }
    void clearClientMessages() { clientMessages.clear(); }
    void clearServerReplies() { serverReplies.clear(); }
    void setNickname(const std::string& nick) { nickname = nick; }
    void setUsername(const std::string& user) { username = user; }
    void setRealName(const std::string& real) { realName = real; }
    void setCapRegi(bool reg) { conRegi[0] = reg; }
    bool isCapRegi() const { return conRegi[0]; }
};


volatile sig_atomic_t running = 1;

void signalHandler(int signum) {
    std::cout << "\nSignal (" << signum << ") received. Shutting down gracefully...\n";
    running = 0;
}

// Function to trim whitespace
std::string ft_trim(const std::string &str) {
    std::size_t start = str.find_first_not_of(" \n\r\t");
    std::size_t end = str.find_last_not_of(" \n\r\t");
    if (start == std::string::npos || end == std::string::npos) {
        return "";
    }
    return str.substr(start, end - start + 1);
}

// Function to split strings by a delimiter
std::vector<std::string> ft_split(std::string str, char delimiter) {
    std::vector<std::string> result;
    std::string word;

    for (std::size_t i = 0; i < str.length(); ++i) {
        if (str[i] != delimiter) {
            word += str[i];
        } else {
            if (!word.empty()) {
                result.push_back(word);
                word.clear();
            }
        }
    }
    if (!word.empty()) {
        result.push_back(word);
    }
    return result;
}

void sendRepliesToClient(Client *client) {
    // Check if the client socket is valid before attempting to send
    for (std::vector<std::string>::size_type j = 0; j < client->serverReplies.size(); ++j) {
        ssize_t bytesSent = send(client->getSocket(), client->serverReplies[j].c_str(), client->serverReplies[j].length(), 0);

        // Handle potential errors
        if (bytesSent < 0) {
            perror("Failed to send message to client");
            // Close the socket if sending fails
            close(client->getSocket());
            client->setSocket(-1); // Mark the socket as closed
            return;
        }
    }
    client->clearServerReplies(); // Clear replies after sending
}

void printClientMessages(Client *client) {
    if (client->getSocket() == -1) {
        return; // Socket is closed, do not attempt to send
    }
    std::cout << "----------------------------------------------------------------\n";
    for (std::vector<std::string>::iterator it = client->clientMessages.begin(); it != client->clientMessages.end(); ++it) {
        std::cout << "Client " << client->getClientNumber() << " sent: " << *it << "\n";
    }
}

void disconnectClient(Client *client, std::map<int, Client>& clients) {
    std::string errorMessage = "ERR_PASSWDMISMATCH (464) :Password incorrect\r\n";
    client->serverReplies.push_back(errorMessage); // Store the error message

    std::string closeMessage = "ERROR :Password incorrect. Closing connection.\r\n";
    client->serverReplies.push_back(closeMessage); // Store the close message

    // Send replies before closing the socket
    sendRepliesToClient(client);

    client->clearClientMessages();

    // Close the client's socket
    close(client->getSocket());

    // Set the socket to -1 to mark it as closed
    client->setSocket(-1);

    // Erase the client from the map
    clients.erase(client->getSocket());
}

void handlePassCommand(Client *client, const std::vector<std::string> &params, const std::string &expectedPassword, std::map<int, Client>& clients) {
    if (params.size() != 1) {
        // Not enough parameters error
        std::string errorMessage = "ERR_NEEDMOREPARAMS (461) :Not enough parameters\r\n";
        client->serverReplies.push_back(errorMessage); // Store the error message
        return;
    }

    const std::string &passwordAttempt = params[0];
    if (passwordAttempt == expectedPassword) {
        // Password accepted
        std::string successMessage = "Password accepted\r\n";
        client->serverReplies.push_back(successMessage); // Store the success message
        client->setHasSentPass(true); // Mark that password has been sent
    } else {
        disconnectClient(client, clients);
    }
}

void handleCapCommand(Client *client, const std::vector<std::string> &params) {
    if (params.size() > 0 && params[0] == "LS") {
        client->conRegi[0] = true;
        client->serverReplies.push_back(":server CAP * LS :capabilities_here \r\n");
    } else if (client->conRegi[0]) {
        if (params.size() == 1 && params[0] == "REQ") {
            client->serverReplies.push_back(":server CAP * REQ :requested_capabilities \r\n");
        } else if (params.size() == 1 && params[0] == "NAK") {
            client->serverReplies.push_back(":server CAP * NAK :rejected_capabilities \r\n");
        } else if (params.size() == 1 && params[0] == "ACK") {
            client->serverReplies.push_back(":server CAP * ACK :accepted_capabilities \r\n");
        } else if (params.size() == 1 && params[0] == "END") {
            client->setIsRegistered(true);
        }
    }
}

void handleNickCommand(Client *client, const std::vector<std::string> &params, std::map<int, Client>& clients) {
    if (params.size() != 1) {
        // Not enough parameters error
        std::string errorMessage = "ERR_NEEDMOREPARAMS (461) :Not enough parameters\r\n";
        client->serverReplies.push_back(errorMessage); // Store the error message
        return;
    }

    const std::string &nicknameAttempt = params[0];

    // Check nickname length
    if (nicknameAttempt.length() > 9) {
        std::string truncatedNick = nicknameAttempt.substr(0, 9);
        std::string errorMessage = "Nickname must be between 1 and 9 characters. The nickname has been truncated to: " + truncatedNick + "\r\n";
        client->serverReplies.push_back(errorMessage); // Inform client about truncation
        client->setNickname(truncatedNick); // Truncate nickname
    } else {
        client->setNickname(nicknameAttempt); // Set the nickname
    }

    // Check for uniqueness
    std::map<int, Client>::iterator it;
    for (it = clients.begin(); it != clients.end(); ++it) {
        if (it->second.getNickname() == client->getNickname() && it->first != client->getSocket()) {
            std::string errorMessage = "ERR_NICKNAMEINUSE (433) :" + client->getNickname() + " :Nickname is already in use\r\n";
            client->serverReplies.push_back(errorMessage); // Store the error message
            return;
        }
    }

    // Nickname accepted
    client->serverReplies.push_back("NICK " + client->getNickname() + "\r\n");
    client->setIsRegistered(true); // Mark client as registered after setting nickname
}

void handleUserCommand(Client *client, const std::vector<std::string> &params) {
    if (params.size() < 3) {
        // Not enough parameters error
        std::string errorMessage = "ERR_NEEDMOREPARAMS (461) :Not enough parameters\r\n";
        client->serverReplies.push_back(errorMessage); // Store the error message
        return;
    }

    client->setUsername(params[0]); // First parameter is the username
    client->setRealName(params[2]); // Third parameter is the real name

    // User command accepted
    std::string successMessage = "USER " + client->getUsername() + " :User registered\r\n";
    client->serverReplies.push_back(successMessage); // Store the success message
    client->setIsRegistered(true); // Mark client as registered after setting username
}



int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>\n";
        return 1;
    }

    int port = std::stoi(argv[1]);
    std::string expectedPassword = argv[2];

    // Signal handling
    signal(SIGINT, signalHandler);

    // Server setup
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Failed to create socket");
        return 1;
    }

    // Set socket options
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Binding failed");
        close(serverSocket);
        return 1;
    }

    listen(serverSocket, 5);
    std::cout << "Server listening on port " << port << "\n";

    std::map<int, Client> clients;
    struct pollfd fds[MAX_CLIENTS];
    int clientCount = 0;

    while (running) {
        for (int i = 0; i < clientCount; ++i) {
            fds[i].fd = clients.find(i)->second.getSocket();
            fds[i].events = POLLIN;
        }
        fds[clientCount].fd = serverSocket;
        fds[clientCount].events = POLLIN;

        int activity = poll(fds, clientCount + 1, -1);
        if (activity < 0) {
            perror("Poll error");
            continue;
        }

        for (int i = 0; i < clientCount + 1; ++i) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == serverSocket) {
                    // New connection
                    int newSocket = accept(serverSocket, nullptr, nullptr);
                    if (newSocket < 0) {
                        perror("Failed to accept connection");
                        continue;
                    }
                    clients[newSocket] = Client(newSocket, newSocket); // Use new client number
                    clientCount++;
                    std::cout << "New client connected: " << newSocket << "\n";
                } else {
                    // Handle existing client
                    char buffer[1024] = {0};
                    ssize_t bytesRead = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                    if (bytesRead <= 0) {
                        // Client disconnected
                        std::cout << "Client disconnected: " << fds[i].fd << "\n";
                        close(fds[i].fd);
                        clients.erase(fds[i].fd);
                        clientCount--;
                    } else {
                        buffer[bytesRead] = '\0'; // Null-terminate the buffer
                        std::string input = buffer;
                        input = ft_trim(input);
                        std::vector<std::string> params = ft_split(input, ' ');

                        if (!params.empty()) {
                            if (params[0] == "PASS") {
                                handlePassCommand(&clients[fds[i].fd], params, expectedPassword, clients);
                            } else if (params[0] == "CAP") {
                                handleCapCommand(&clients[fds[i].fd], params);
                            } else if (params[0] == "NICK") {
                                handleNickCommand(&clients[fds[i].fd], params, clients);
                            } else if (params[0] == "USER") {
                                handleUserCommand(&clients[fds[i].fd], params);
                            } else if (params[0] == "PING") {
                            // Ensure that we have one argument for PING
                            if (params.size() != 1) {
                                std::string errorMessage = "ERR_NEEDMOREPARAMS (461) :Not enough parameters\r\n";
                                clients[fds[i].fd].serverReplies.push_back(errorMessage); // Store the error message
                            } else {
                                // Respond with PONG using the server name and the same argument
                                std::string pongMessage = "PONG server_name " + params[0] + "\r\n";
                                clients[fds[i].fd].serverReplies.push_back(pongMessage);
                            }
                        }
                        }
                        // Print messages received from the client
                        clients[fds[i].fd].clientMessages.push_back(input);
                        printClientMessages(&clients[fds[i].fd]);

                        // Send replies to the client
                        sendRepliesToClient(&clients[fds[i].fd]);
                    }
                }
            }
        }
    }

    // Cleanup
    for (int i = 1; i < clientCount; i++) 
    {
        close(fds[i].fd);
    }
    close(serverSocket);
    std::cout << "Server shutting down...\n";

    return 0;
}
