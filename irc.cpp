/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mahmoud <mahmoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 14:26:54 by mahmoud           #+#    #+#             */
/*   Updated: 2024/10/20 21:19:09 by mahmoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


//####################################################################


// # define user_id(nickname, username) (":" + nickname + "!" + username + "@localhost")


// // NICKNAME ERRORS:
// #define ERR_NONICKNAMEGIVEN(client) (":localhost 431 " + client + " ::No nickname given.\r\n")

// #define ERR_NICKNAMEINUSE(client, nickname) (":localhost 433 " + client + " " + nickname + " :Nickname is already in use.\r\n")

// #define ERR_ERRONEUSNICKNAME(client, nickname) (":localhost 432 " + client + " " + nickname + " :Erroneous nickname\r\n")

// // GENERAL ERRORS:
// # define ERR_NEEDMOREPARAMS(client, command) (":localhost 461 " + client + " " + command + " :Not enough parameters.\r\n")

// #define ERROR_MESSAGE(reason) ("ERROR :" + reason + "\r\n")

// // PASSWORD ERRORS:
// #define ERR_PASSWDMISMATCH(client) (":localhost 464 " + client + " :Password incorrect.\r\n")

// // USER ERRORS:
// #define ERR_ALREADYREGISTERED(client) (":localhost 462 " + client + " :You may not reregister.\r\n")

// // CONNECTION REGISTRATION SUCCESS MESSAGES TO SEND:
// # define RPL_WELCOME(user_id, nickname) (":localhost 001 " + nickname + " :Welcome to the Internet Relay Network " + user_id + "\r\n")
// # define RPL_YOURHOST(client, servername, version) (":localhost 002 " + client + " :Your host is " + servername + " (localhost), running version " + version + "\r\n")
// # define RPL_CREATED(client, datetime) (":localhost 003 " + client + " :This server was created " + datetime + "\r\n")
// # define RPL_MYINFO(client, servername, version, user_modes, chan_modes, chan_param_modes) (":localhost 004 " + client + " " + servername + " " + version + " " + user_modes + " " + chan_modes + " " + chan_param_modes + "\r\n")
// # define RPL_ISUPPORT(client, tokens) (":localhost 005 " + client + " " + tokens " :are supported by this server\r\n")

// #define RPL_NICK(oclient, uclient, client) (":" + oclient + "!" + uclient + "@localhost NICK " +  client + "\r\n")

// #define RPL_PONG(user_id, token) (user_id + " PONG " + token + "\r\n")

// #define RPL_MOTDSTART(client, servername) (":localhost 375 " + client + " :- " + servername + " Message of the day - \r\n")

// #define RPL_MOTD(client, motd_line) (":localhost 372 " + client + " :" + motd_line + "\r\n")

// #define ERR_NOMOTD(client) (":localhost 422 " + client + " :MOTD File is missing\r\n")

// #define RPL_ENDOFMOTD(client) (":localhost 376 " + client + " :End of /MOTD command.\r\n")





// #include <iostream>
// #include <cstring>
// #include <string>
// #include <poll.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <netinet/in.h>
// #include <map>
// #include <vector>
// #include <sstream>
// #include <csignal>
// #include <cstdlib>
// #include <ctime>
// #include <fstream>


// const int MAX_CLIENTS = 100;

// class Client
// {
// private:
//     int socket;
//     int clientNumber;
//     bool isRegistered;
//     bool hasSentPass;
//     bool hasSentWelcomeMessage;
//     std::string nickname; // Added for storing nickname
//     std::string username; // Added for storing username
//     std::string realName; // Added for storing real name

// public:
//     // Default constructor
//     Client()
//         : socket(-1), clientNumber(-1), isRegistered(false), hasSentPass(false), hasSentWelcomeMessage(false)
//     {
//         clientMessages.clear();
//         serverReplies.clear();
//         nickname = ""; // Initialize nickname
//         username = ""; // Initialize username
//         realName = ""; // Initialize real name
//         memset(RegisterSteps, 0, 3);
//     }

//     // Parameterized constructor
//     Client(int socket, int clientNumber)
//         : socket(socket), clientNumber(clientNumber), isRegistered(false), hasSentPass(false), hasSentWelcomeMessage(false)
//     {
//         clientMessages.clear();
//         serverReplies.clear();
//         nickname = ""; // Initialize nickname
//         username = ""; // Initialize username
//         realName = ""; // Initialize real name
//         memset(RegisterSteps, 0, 3);
//     }
//     std::vector<std::string> clientMessages;
//     std::vector<std::string> serverReplies;
//    	bool	RegisterSteps[3];


//     // Getter methods
//     int getSocket() const { return socket; }
//     int getClientNumber() const { return clientNumber; }
//     bool getIsRegistered() const { return isRegistered; }
//     bool getHasSentPass() const { return hasSentPass; }
//     bool getHasSentWelcomeMessage() const { return hasSentWelcomeMessage; } // Getter for welcome message flag
//     const std::string &getNickname() const { return nickname; }
//     const std::string &getUsername() const { return username; }
//     const std::string &getRealName() const { return realName; }

//     // Setter methods
//     void setSocket(int sock) { socket = sock; }
//     void setClientNumber(int num) { clientNumber = num; }
//     void setIsRegistered(bool reg) { isRegistered = reg; }
//     void setHasSentPass(bool sent) { hasSentPass = sent; }
//     void setHasSentWelcomeMessage(bool sent) { hasSentWelcomeMessage = sent; }
//     void clearClientMessages() { clientMessages.clear(); }
//     void clearServerReplies() { serverReplies.clear(); }
//     void setNickname(const std::string &nick) { nickname = nick; }
//     void setUsername(const std::string &user) { username = user; }
//     void setRealName(const std::string &real) { realName = real; }
//     void setCapRegi(bool reg) { RegisterSteps[0] = reg; }
//     bool isCapRegi() const { return RegisterSteps[0]; }
// };

// volatile sig_atomic_t running = 1;

// void signalHandler(int signum)
// {
//     std::cout << "\nSignal (" << signum << ") received. Shutting down...\n";
//     running = 0;
// }

// // Function to trim whitespace
// std::string ft_trim(const std::string &str)
// {
//     std::size_t start = str.find_first_not_of(" \n\r\t");
//     std::size_t end = str.find_last_not_of(" \n\r\t");
//     if (start == std::string::npos || end == std::string::npos)
//     {
//         return "";
//     }
//     return str.substr(start, end - start + 1);
// }

// // Function to split strings by a delimiter
// std::vector<std::string> ft_split(std::string str, char delimiter)
// {
//     std::vector<std::string> result;
//     std::string word;

//     for (std::size_t i = 0; i < str.length(); ++i)
//     {
//         if (str[i] != delimiter)
//         {
//             word += str[i];
//         }
//         else
//         {
//             if (!word.empty())
//             {
//                 result.push_back(word);
//                 word.clear();
//             }
//         }
//     }
//     if (!word.empty())
//     {
//         result.push_back(word);
//     }
//     return result;
// }

// void sendRepliesToClient(Client *client)
// {
//     // Check if the client socket is valid before attempting to send
//     if (client->getSocket() == -1)
//     {
//         return; // Socket is closed, do not attempt to send
//     }

//     for (std::vector<std::string>::size_type j = 0; j < client->serverReplies.size(); ++j)
//     {
//         ssize_t bytesSent = send(client->getSocket(), client->serverReplies[j].c_str(), client->serverReplies[j].length(), 0);
//         std::cout << "server sent to client: " + client->serverReplies[j] + "\n";

//         // Handle potential errors
//         if (bytesSent < 0)
//         {
//             // Close the socket if sending fails
//             close(client->getSocket());

//             client->setSocket(-1); // Mark the socket as closed
//             return;
//         }
//     }
//     // Clear replies after sending
//     client->clearServerReplies();
// }

// void printClientMessages(Client *client)
// {

//     if (client->getSocket() == -1)
//     {
//         return; // Socket is closed, do not attempt to send
//     }
//     std::cout << "----------------------------------------------------------------\n";
//     for (std::vector<std::string>::const_iterator it = client->clientMessages.begin(); it != client->clientMessages.end(); ++it)
//     {
//         std::cout << "Client " << client->getClientNumber() << " sent: " << *it << "\n";
//     }
// }

// void disconnectClient(Client *client, std::map<int, Client> &clients)
// {

//     client->serverReplies.push_back(ERR_PASSWDMISMATCH(std::string("ircserver")));

//     client->serverReplies.push_back(ERROR_MESSAGE(std::string("Password incorrect. Closing connection."))); // Store the close message

//     std::cout << "Client " << client->getClientNumber() << " disconnected.\n";

                    
    
//     // Send replies before closing the socket
//     sendRepliesToClient(client);

//     client->clearClientMessages();

//     // Close the client's socket
//     close(client->getSocket());

//     // Set the socket to -1 to mark it as closed
//     client->setSocket(-1);

//     // Erase the client from the map
//     clients.erase(client->getSocket());
// }

// void handlePassCommand(Client *client, const std::vector<std::string> &params, const std::string &expectedPassword, std::map<int, Client> &clients)
// {   
//     (void)clients;
//     if (params.size() != 1)
//     {
//         // Not enough parameters error
//         client->serverReplies.push_back(ERR_NEEDMOREPARAMS(std::string("ircserver"), "PART")); // Store the error message
//         return;
//     }

//     const std::string &passwordAttempt = params[0];
//     if (passwordAttempt == expectedPassword)
//     {
//         // Password accepted
//         std::string successMessage = "Password accepted\r\n";
//         client->serverReplies.push_back(successMessage); // Store the error message
//         client->setHasSentPass(true);

//         // Further registration logic can be placed here
//     }
//     // else
//     // {
//     //     disconnectClient(client, clients);
//     // }
// }

// void handleCapCommand(Client *client, const std::vector<std::string> &params)
// {
//     if (params.size() > 0 && params[0] == "LS")
//     {
//         client->RegisterSteps[0] = true;
//         client->serverReplies.push_back(":server CAP * LS :capabilities_here \r\n");
//     }
//     else if (client->RegisterSteps[0])
//     {
//         if (params.size() == 1 && params[0] == "REQ")
//         {
//             client->serverReplies.push_back(":server CAP * REQ :requested_capabilities \r\n");
//         }
//         else if (params.size() == 1 && params[0] == "NAK")
//         {
//             client->serverReplies.push_back(":server CAP * NAK :rejected_capabilities \r\n");
//         }
//         else if (params.size() == 1 && params[0] == "ACK")
//         {
//             client->serverReplies.push_back(":server CAP * ACK :accepted_capabilities \r\n");
//         }
//     }
// }



// void handleNickCommand(Client *client, const std::vector<std::string> &params, std::vector<std::string>& nicknames) {
//     // Check if the nickname parameter is provided
//     if (params.size() < 1) {
//             client->serverReplies.push_back(ERR_NONICKNAMEGIVEN(std::string("ircserver")));
//         return;
//     }

//     std::string newNick = params[0]; // Retrieve the new nickname

//     // Check for invalid characters in the new nickname
//    if (newNick.find_first_of("#@:&") != std::string::npos) {
//     client->serverReplies.push_back(ERR_ERRONEUSNICKNAME(std::string("ircserv"), newNick));
//     return;
// }


// if (!nicknames.empty() && std::find(nicknames.begin(), nicknames.end(), newNick) != nicknames.end()) {
//     client->serverReplies.push_back(ERR_NICKNAMEINUSE(std::string("ircserv"), newNick));
//     return; // Make sure to return here to avoid further processing
// }


//     // If the client already has a nickname, remove it from the nicknames list
//     if (!client->getNickname().empty()) {
//         nicknames.erase(std::remove(nicknames.begin(), nicknames.end(), client->getNickname()), nicknames.end());
//         // Inform the client of the nickname change
//         // Ensure that when sending the welcome message, it contains the right parameters
//         client->serverReplies.push_back(RPL_NICK(client->getNickname(), client->getUsername(), newNick));

//     }

//     // Set the new nickname for the client
//     client->setNickname(newNick);
//     // Add the new nickname to the nicknames list
//     nicknames.push_back(newNick);

//     // Optionally, notify other clients about the nickname change
//     // Notify other clients of the nickname change (not implemented in your provided code, but could be added here)
//     // Example: notifyClientsAboutNickChange(client, newNick);
// }


// void handleUserCommand(Client *client, const std::vector<std::string> &params)
// {
//     if (params.size() < 3)
//     {
//         // Not enough parameters error
//         client->serverReplies.push_back(ERR_NEEDMOREPARAMS(std::string("ircserver"), "USER")); // Store the error message
//         return;
//     }

//     client->setUsername(params[0]); // First parameter is the username
//     client->setRealName(params[2]); // Third parameter is the real name

//     // User command accepted
//     if (client->getHasSentPass())
//     {
//         std::string successMessage = "USER " + client->getUsername() + " registered successfully!\r\n";
//         client->serverReplies.push_back(successMessage); // Store the success message
//         client->setIsRegistered(true);                   // Mark client as registered after setting username   
//     }
// }


// std::string getCurrentDateTime()
// {
//     // Get the current time
//     std::time_t rawTime;
//     std::time(&rawTime);

//     // Convert to local time
//     struct tm *localTime = std::localtime(&rawTime);

//     // Buffer to hold the formatted date and time
//     char buffer[80];

//     // Format: YYYY-MM-DD HH:MM:SS
//     strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", localTime);

//     return std::string(buffer);
// }



// void    MOTD(Client *client)
// {
//     std::ostringstream oss;
//     std::string line;
//     std::ifstream infile;

// 	if(!client->getIsRegistered())
// 		return;
//     infile.open("./MOTD.txt", std::ios::in);
	
// 	client->serverReplies.push_back(RPL_WELCOME(user_id(client->getNickname(),client->getUsername()),client->getNickname()));
// 	client->serverReplies.push_back(RPL_YOURHOST(client->getUsername(),"irssi", "1"));
// 	client->serverReplies.push_back(RPL_CREATED(client->getUsername(), std::string("creation_date")));
// 	client->serverReplies.push_back(RPL_MYINFO(client->getUsername(),"irssi", "1", "","",""));
// 	client->serverReplies.push_back(RPL_ISUPPORT(client->getUsername(),"CHANMODES=ikolt"));
// 	client->serverReplies.push_back(RPL_MOTDSTART(client->getUsername(),std::string("ircserver")));
//     if (infile.is_open())
//     {
//         while (std::getline(infile,line))
//         {
//             client->serverReplies.push_back(RPL_MOTD(std::string("ircserver"),line));
//         }
//         infile.close();
//     }
//     else
//         client->serverReplies.push_back(ERR_NOMOTD(std::string("ircserver")));
		
//     client->serverReplies.push_back( RPL_ENDOFMOTD(std::string("ircserver")));
//     client->setHasSentWelcomeMessage(true);
//     return ;
// }




// int main(int argc, char *argv[])
// {
//     if (argc != 3)
//     {
//         std::cerr << "Usage: ./ircserv <port> <password>\n";
//         return 1;
//     }

//     int port = std::atoi(argv[1]);
//     if (port <= 0 || port > 65535)
//     {
//         std::cerr << "Error: Invalid port number. Please provide a valid port (1-65535).\n";
//         return 1;
//     }

//     std::string serverPassword = argv[2];

//     int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (listenSocket < 0)
//     {
//         std::cerr << "Error: could not create socket.\n";
//         return 1;
//     }

//     // Set socket options
//     int opt = 1;
//     setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

//     sockaddr_in serverAddr;
//     memset(&serverAddr, 0, sizeof(serverAddr));
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_addr.s_addr = INADDR_ANY;
//     serverAddr.sin_port = htons(port);

//     if (bind(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
//     {
//         std::cerr << "Error: could not bind socket.\n";
//         close(listenSocket);
//         return 1;
//     }

//     listen(listenSocket, 5);
//     std::cout << "Server listening on port " << port << "\n";


// 	std::vector<std::string> nicknames;	
//     std::map<int, Client> clients;
//     struct pollfd fds[MAX_CLIENTS];
//     memset(fds, 0, sizeof(fds));
//     int clientCount = 1;
//     fds[0].fd = listenSocket;
//     fds[0].events = POLLIN;

//     std::signal(SIGINT, signalHandler);

//     while (running)
//     {
//         int pollCount = poll(fds, clientCount, -1);
//         if (pollCount == -1)
//         {
//             break;
//         }

//         for (int i = 0; i < clientCount; i++)
//         {
//             if (fds[i].fd == listenSocket && (fds[i].revents & POLLIN))
//             {
//                 sockaddr_in client;
//                 socklen_t clientSize = sizeof(client);
//                 int clientSocket = accept(listenSocket, (sockaddr *)&client, &clientSize);
//                 if (clientSocket == -1)
//                 {
//                     std::cerr << "Error: failed to accept client.\n";
//                     continue;
//                 }
//                 else
//                 {
//                     fcntl(clientSocket, F_SETFL, O_NONBLOCK);
//                     if (clientCount < MAX_CLIENTS)
//                     {
//                         clients[clientSocket] = Client(clientSocket, clientCount);
//                         fds[clientCount].fd = clientSocket;
//                         fds[clientCount].events = POLLIN;
//                         std::cout << "Client " << clientCount << " connected.\n";
//                         clientCount++;
//                     }
//                     else
//                     {
//                         std::cerr << "Error: maximum clients reached.\n";
//                         close(clientSocket);
//                     }
//                 }
//             }
//             else if (fds[i].revents & POLLIN)
//             {
//                 char buffer[4096];
//                 memset(buffer, 0, sizeof(buffer));
//                 int bytesRecv = recv(fds[i].fd, buffer, sizeof(buffer), 0);

//                 // Handle error for recv
//                 if (bytesRecv == -1)
//                 {
//                     if (errno == EAGAIN || errno == EWOULDBLOCK)
//                     {
//                         // No data available right now; it's okay to ignore this
//                         continue; // Continue to the next client
//                     }
//                     else
//                     {
//                         std::cerr << "Error receiving data from client: " << strerror(errno) << "\n";
//                         // Handle the disconnection logic as necessary
//                         close(fds[i].fd);
//                         clients.erase(fds[i].fd);
//                         fds[i] = fds[clientCount - 1]; // Move the last client into the current spot
//                         fds[clientCount - 1].fd = -1;  // Reset the last position in fds to an invalid fd
//                         clientCount--;
//                         i--; // Decrement i to ensure we don't skip the next client
//                     }
//                 }
//                 else if (bytesRecv <= 0)
//                 {
//                     // Handle client disconnection
//                     std::cout << "Client " << clients[fds[i].fd].getClientNumber() << " disconnected.\n";
//                     close(fds[i].fd);
//                     clients.erase(fds[i].fd);
//                     // Remove the client socket from the fds array
//                     fds[i] = fds[clientCount - 1]; // Move the last client into the current spot
//                     fds[clientCount - 1].fd = -1;  // Reset the last position in fds to an invalid fd
//                     clientCount--;
//                     i--; // Decrement i to ensure we don't skip the next client
//                 }
//                 else
//                 {
//                     std::string message(buffer, bytesRecv);
//                     message.erase(message.find_last_not_of("\r\n") + 1);
//                     Client &currentClient = clients[fds[i].fd];

//                     // Trim and split the message
//                     message = ft_trim(message);
//                     std::vector<std::string> splitMessage = ft_split(message, '\n');
//                     for (std::vector<std::string>::size_type j = 0; j < splitMessage.size(); ++j)
//                     {

//                         std::string line = splitMessage[j];
//                         // Trim each line
//                         std::string trimmedLine = ft_trim(line);
//                         if (trimmedLine.empty())
//                             continue; // Skip empty lines

//                         currentClient.clientMessages.push_back(trimmedLine);
//                         // std::cout << "Client " << currentClient.clientNumber << " sent: " << trimmedLine << "\n";

//                         // Split command and parameters
//                         std::vector<std::string> tokens = ft_split(trimmedLine, ' ');

//                         if (tokens.empty())
//                             continue;

//                         std::string command = tokens[0];
//                         std::vector<std::string> params(tokens.begin() + 1, tokens.end());

//                         // Debugging output
//                         printClientMessages(&currentClient);
//                         std::cout << "Command: " << command << "\n";
//                         for (std::vector<std::string>::size_type j = 0; j < params.size(); ++j)
//                         {
//                             std::string param = params[j];
//                             std::cout << "Parameter[" << j << "]: " << param << "\n";
//                         }
//                         currentClient.clientMessages.clear();

//                         // Handle commands
//                         if (command == "CAP")
//                         {
//                             handleCapCommand(&currentClient, params);
//                         }
//                         else if (currentClient.RegisterSteps[0] == true && command == "PASS")
//                         {
//                             if (!currentClient.getHasSentWelcomeMessage())
//                             {
//                                 handlePassCommand(&currentClient, params, serverPassword, clients);
//                                 currentClient.RegisterSteps[1] = true;
//                             }
//                             else
//                                 currentClient.serverReplies.push_back(ERR_ALREADYREGISTERED(std::string("ircserver")));

//                         }
//                         else if (!currentClient.RegisterSteps[1] && command == "USER")
//                             disconnectClient(&currentClient, clients);
// 	                    else if (currentClient.RegisterSteps[1] == true && (command == "USER" || command == "NICK"))
//                         {
                           
//                                 if (command == "NICK")
//                                 {
//                                     handleNickCommand(&currentClient, params, nicknames);
//                                 }
//                                 else if (command == "USER")
//                                 {
//                                     if (!currentClient.getHasSentWelcomeMessage())
//                                     {
//                                         handleUserCommand(&currentClient, params);
//                                         if (!currentClient.getHasSentPass())
//                                             disconnectClient(&currentClient, clients);
//                                     }
//                                     else
//                                         currentClient.serverReplies.push_back(ERR_ALREADYREGISTERED(std::string("ircserver")));
//                                 }
//                                 if (currentClient.getUsername() != "" && currentClient.getNickname() != "") 
//                                 {
//                                     currentClient.RegisterSteps[2] = true;
//                                     currentClient.setIsRegistered(true);
//                                 }
//                         }
//                         else if (command == "PING")
//                         {
//                            	currentClient.serverReplies.push_back(RPL_PONG(user_id(currentClient.getNickname(),currentClient.getUsername()),params[0]));

//                         }
//                         if (currentClient.getIsRegistered() == true && currentClient.RegisterSteps[2] == true && !currentClient.getHasSentWelcomeMessage())
//                         {
//                             MOTD(&currentClient);
//                         }
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
//     return 0;
// }






//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!





// #include "Server.hpp"
// #include "Client.hpp"



// volatile sig_atomic_t running = 1;

// void signalHandler(int signum)
// {
//     std::cout << "\nSignal (" << signum << ") received. Shutting down...\n";
//     running = 0;
// }

// // Function to trim whitespace
// std::string ft_trim(const std::string &str)
// {
//     std::size_t start = str.find_first_not_of(" \n\r\t");
//     std::size_t end = str.find_last_not_of(" \n\r\t");
//     if (start == std::string::npos || end == std::string::npos)
//     {
//         return "";
//     }
//     return str.substr(start, end - start + 1);
// }

// // Function to split strings by a delimiter
// std::vector<std::string> ft_split(std::string str, char delimiter)
// {
//     std::vector<std::string> result;
//     std::string word;

//     for (std::size_t i = 0; i < str.length(); ++i)
//     {
//         if (str[i] != delimiter)
//         {
//             word += str[i];
//         }
//         else
//         {
//             if (!word.empty())
//             {
//                 result.push_back(word);
//                 word.clear();
//             }
//         }
//     }
//     if (!word.empty())
//     {
//         result.push_back(word);
//     }
//     return result;
// }



// void sendRepliesToClient(Client *client)
// {
//     // Check if the client socket is valid before attempting to send
//     if (client->getSocket() == -1)
//     {
//         return; // Socket is closed, do not attempt to send
//     }

//     for (std::vector<std::string>::size_type j = 0; j < client->getServerReplies().size(); ++j) {
//     ssize_t bytesSent = send(client->getSocket(), client->getServerReplies()[j].c_str(), client->getServerReplies()[j].length(), 0);
//     std::cout << "server sent to client: " + client->getServerReplies()[j] + "\n";

//     // Handle potential errors
//     if (bytesSent < 0) {
//         // Close the socket if sending fails
//         close(client->getSocket());

//         client->setSocket(-1); // Mark the socket as closed
//         return;
//     }
// }
//     // Clear replies after sending
//     client->clearServerReplies();
// }

// void printClientMessages(Client *client) {
//     if (client->getSocket() == -1) {
//         return; // Socket is closed, do not attempt to send
//     }
//     std::cout << "----------------------------------------------------------------\n";
//     for (std::vector<std::string>::const_iterator it = client->getClientMessages().begin(); it != client->getClientMessages().end(); ++it)
//     {
//         std::cout << "Client " << client->getClientNumber() << " sent: " << *it << "\n";
//     }
// }

// void disconnectClient(Client *client, std::map<int, Client> &clients) {
//     client->getServerReplies().push_back(ERR_PASSWDMISMATCH(std::string("ircserver")));
//     client->getServerReplies().push_back(ERROR_MESSAGE(std::string("Password incorrect. Closing connection.")));


//     std::cout << "Client " << client->getClientNumber() << " disconnected.\n";

//     // Send replies before closing the socket
//     sendRepliesToClient(client);

//     client->clearClientMessages();

//     // Close the client's socket
//     close(client->getSocket());

//     // Set the socket to -1 to mark it as closed
//     client->setSocket(-1);

//     // Erase the client from the map
//     clients.erase(client->getSocket());
// }

// void handlePassCommand(Client *client, const std::vector<std::string> &params, const std::string &expectedPassword, std::map<int, Client> &clients) {   
//     (void)clients;
//     if (params.size() != 1) {
//         client->getServerReplies().push_back(ERR_NEEDMOREPARAMS(std::string("ircserver"), "PART"));
//         return;
//     }

//     const std::string &passwordAttempt = params[0];
//     if (passwordAttempt == expectedPassword) {
//         std::string successMessage = "Password accepted\r\n";
//         client->getServerReplies().push_back(successMessage);
//         client->setHasSentPass(true);
//     }
// }

// void handleCapCommand(Client *client, const std::vector<std::string> &params) {
//     if (params.size() > 0 && params[0] == "LS") {
//         client->setRegisterSteps(0, true);
//         client->getServerReplies().push_back(":server CAP * LS :capabilities_here \r\n");
//     } else if (client->getRegisterSteps(0)) {
//         if (params.size() == 1 && params[0] == "REQ") {
//             client->getServerReplies().push_back(":server CAP * REQ :requested_capabilities \r\n");
//         } else if (params.size() == 1 && params[0] == "NAK") {
//             client->getServerReplies().push_back(":server CAP * NAK :rejected_capabilities \r\n");
//         } else if (params.size() == 1 && params[0] == "ACK") {
//             client->getServerReplies().push_back(":server CAP * ACK :accepted_capabilities \r\n");
//         }
//     }
// }

// void handleNickCommand(Client *client, const std::vector<std::string> &params, std::vector<std::string>& nicknames) {
//     if (params.size() < 1) {
//         client->getServerReplies().push_back(ERR_NONICKNAMEGIVEN(std::string("ircserver")));
//         return;
//     }

//     std::string nickName = params[0];


//     if (!nickName.empty() && isdigit(nickName[0])) {
//         client->getServerReplies().push_back(ERR_ERRONEUSNICKNAME(std::string("ircserver"), nickName));
//         return;
//     }

//     if (nickName.length() > 9) {
//         client->getServerReplies().push_back(ERR_ERRONEUSNICKNAME(std::string("ircserver"), nickName));
//         return ;
//     }
    

//     for (std::string::iterator it = nickName.begin(); it != nickName.end(); ++it) {
//         if (!isalnum(*it) && *it != '[' && *it != ']' && *it != '{' && *it != '}'
//             && *it != '|' && *it != '-' && *it != '_') {
//             client->getServerReplies().push_back(ERR_ERRONEUSNICKNAME(std::string("ircserver"), nickName));
//             return;
//         }
//     }

//     if (!nicknames.empty() && std::find(nicknames.begin(), nicknames.end(), nickName) != nicknames.end()) {
//         client->getServerReplies().push_back(ERR_NICKNAMEINUSE(std::string("ircserv"), nickName));
//         return;
//     }

//    if (!client->getNickname().empty()) {
//         nicknames.erase(std::remove(nicknames.begin(), nicknames.end(), client->getNickname()), nicknames.end());
//         client->getServerReplies().push_back(RPL_NICK(client->getNickname(), client->getUsername(), nickName));
//         client->setNickname(nickName);
//         nicknames.push_back(nickName);
//         return;
//     }
   
//     client->setNickname(nickName);
//     nicknames.push_back(nickName);
//     client->getServerReplies().push_back(RPL_NICK(client->getNickname(), client->getUsername(), nickName));
    
// }





// void handleUserCommand(Client *client, const std::vector<std::string> &params) {
//     if (params.size() < 3) {
//         client->getServerReplies().push_back(ERR_NEEDMOREPARAMS(std::string("ircserver"), "USER"));
//         return;
//     }

//     client->setUsername(params[0]);
//     client->setRealName(params[2]);

//     if (client->getHasSentPass()) {
//         std::string successMessage = "USER " + client->getUsername() + " registered successfully!\r\n";
//         client->getServerReplies().push_back(successMessage);
//         client->setIsRegistered(true);
//     }
// }



// std::string getCurrentDateTime()
// {
//     // Get the current time
//     std::time_t rawTime;
//     std::time(&rawTime);

//     // Convert to local time
//     struct tm *localTime = std::localtime(&rawTime);

//     // Buffer to hold the formatted date and time
//     char buffer[80];

//     // Format: YYYY-MM-DD HH:MM:SS
//     strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", localTime);

//     return std::string(buffer);
// }



// void    MOTD(Client *client)
// {
//     std::ostringstream oss;
//     std::string line;
//     std::ifstream infile;

// 	if(!client->getIsRegistered())
// 		return;
//     infile.open("./MOTD.txt", std::ios::in);
	
// 	client->getServerReplies().push_back(RPL_WELCOME(idFormat(client->getNickname(),client->getUsername()),client->getNickname()));
// 	client->getServerReplies().push_back(RPL_YOURHOST(client->getUsername(),"irssi", "1"));
// 	client->getServerReplies().push_back(RPL_CREATED(client->getUsername(), std::string("creation_date")));
// 	client->getServerReplies().push_back(RPL_MYINFO(client->getUsername(),"irssi", "1", "","",""));
// 	client->getServerReplies().push_back(RPL_ISUPPORT(client->getUsername(),"CHANMODES=ikolt"));
// 	client->getServerReplies().push_back(RPL_MOTDSTART(client->getUsername(),std::string("ircserver")));
//     if (infile.is_open())
//     {
//         while (std::getline(infile,line))
//         {
//             client->getServerReplies().push_back(RPL_MOTD(std::string("ircserver"),line));
//         }
//         infile.close();
//     }
//     else
//         client->getServerReplies().push_back(ERR_NOMOTD(std::string("ircserver")));
		
//     client->getServerReplies().push_back( RPL_ENDOFMOTD(std::string("ircserver")));
//     client->setHasSentWelcomeMessage(true);
//     return ;
// }




// int main(int argc, char *argv[])
// {
//     if (argc != 3)
//     {
//         std::cerr << "Usage: ./ircserv <port> <password>\n";
//         return 1;
//     }

//     int port = std::atoi(argv[1]);
//     if (port <= 0 || port > 65535)
//     {
//         std::cerr << "Error: Invalid port number. Please provide a valid port (1-65535).\n";
//         return 1;
//     }

//     std::string serverPassword = argv[2];

//     int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (listenSocket < 0)
//     {
//         std::cerr << "Error: could not create socket.\n";
//         return 1;
//     }

//     // Set socket options
//     int opt = 1;
//     setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

//     sockaddr_in serverAddr;
//     memset(&serverAddr, 0, sizeof(serverAddr));
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_addr.s_addr = INADDR_ANY;
//     serverAddr.sin_port = htons(port);

//     if (bind(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
//     {
//         std::cerr << "Error: could not bind socket.\n";
//         close(listenSocket);
//         return 1;
//     }

//     listen(listenSocket, 5);
//     std::cout << "Server listening on port " << port << "\n";


// 	std::vector<std::string> nicknames;	
//     std::map<int, Client> clients;
//     struct pollfd fds[MAX_CLIENTS];
//     memset(fds, 0, sizeof(fds));
//     int clientCount = 1;
//     fds[0].fd = listenSocket;
//     fds[0].events = POLLIN;

//     std::signal(SIGINT, signalHandler);

//     while (running)
//     {
//         int pollCount = poll(fds, clientCount, -1);
//         if (pollCount == -1)
//         {
//             break;
//         }

//         for (int i = 0; i < clientCount; i++)
//         {
//             if (fds[i].fd == listenSocket && (fds[i].revents & POLLIN))
//             {
//                 sockaddr_in client;
//                 socklen_t clientSize = sizeof(client);
//                 int clientSocket = accept(listenSocket, (sockaddr *)&client, &clientSize);
//                 if (clientSocket == -1)
//                 {
//                     std::cerr << "Error: failed to accept client.\n";
//                     continue;
//                 }
//                 else
//                 {
//                     fcntl(clientSocket, F_SETFL, O_NONBLOCK);
//                     if (clientCount < MAX_CLIENTS)
//                     {
//                         clients[clientSocket] = Client(clientSocket, clientCount);
//                         fds[clientCount].fd = clientSocket;
//                         fds[clientCount].events = POLLIN;
//                         std::cout << "Client " << clientCount << " connected.\n";
//                         clientCount++;
//                     }
//                     else
//                     {
//                         std::cerr << "Error: maximum clients reached.\n";
//                         close(clientSocket);
//                     }
//                 }
//             }
//             else if (fds[i].revents & POLLIN)
//             {
//                 char buffer[4096];
//                 memset(buffer, 0, sizeof(buffer));
//                 int bytesRecv = recv(fds[i].fd, buffer, sizeof(buffer), 0);

//                 // Handle error for recv
//                 if (bytesRecv == -1)
//                 {
//                     if (errno == EAGAIN || errno == EWOULDBLOCK)
//                     {
//                         // No data available right now; it's okay to ignore this
//                         continue; // Continue to the next client
//                     }
//                     else
//                     {
//                         std::cerr << "Error receiving data from client: " << strerror(errno) << "\n";
//                         // Handle the disconnection logic as necessary
//                         close(fds[i].fd);
//                         clients.erase(fds[i].fd);
//                         fds[i] = fds[clientCount - 1]; // Move the last client into the current spot
//                         fds[clientCount - 1].fd = -1;  // Reset the last position in fds to an invalid fd
//                         clientCount--;
//                         i--; // Decrement i to ensure we don't skip the next client
//                     }
//                 }
//                 else if (bytesRecv <= 0)
//                 {
//                     // Handle client disconnection
//                     std::cout << "Client " << clients[fds[i].fd].getClientNumber() << " disconnected.\n";
//                     close(fds[i].fd);
//                     clients.erase(fds[i].fd);
//                     // Remove the client socket from the fds array
//                     fds[i] = fds[clientCount - 1]; // Move the last client into the current spot
//                     fds[clientCount - 1].fd = -1;  // Reset the last position in fds to an invalid fd
//                     clientCount--;
//                     i--; // Decrement i to ensure we don't skip the next client
//                 }
//                 else
//                 {
//                     std::string message(buffer, bytesRecv);
//                     message.erase(message.find_last_not_of("\r\n") + 1);
//                     Client &currentClient = clients[fds[i].fd];

//                     // Trim and split the message
//                     message = ft_trim(message);
//                     std::vector<std::string> splitMessage = ft_split(message, '\n');
//                     for (std::vector<std::string>::size_type j = 0; j < splitMessage.size(); ++j)
//                     {

//                         std::string line = splitMessage[j];
//                         // Trim each line
//                         std::string trimmedLine = ft_trim(line);
//                         if (trimmedLine.empty())
//                             continue; // Skip empty lines

//                         currentClient.getClientMessages().push_back(trimmedLine);
//                         // std::cout << "Client " << currentClient.clientNumber << " sent: " << trimmedLine << "\n";

//                         // Split command and parameters
//                         std::vector<std::string> tokens = ft_split(trimmedLine, ' ');

//                         if (tokens.empty())
//                             continue;

//                         std::string command = tokens[0];
//                         std::vector<std::string> params(tokens.begin() + 1, tokens.end());

//                         // Debugging output
//                         printClientMessages(&currentClient);
//                         std::cout << "Command: " << command << "\n";
//                         for (std::vector<std::string>::size_type j = 0; j < params.size(); ++j)
//                         {
//                             std::string param = params[j];
//                             std::cout << "Parameter[" << j << "]: " << param << "\n";
//                         }
//                         currentClient.clearClientMessages();

//                         // Handle commands
//                         if (command == "CAP")
//                         {
//                             handleCapCommand(&currentClient, params);
//                         }
//                         else if (currentClient.getRegisterSteps(0) == true && command == "PASS")
//                         {
//                             if (!currentClient.getHasSentWelcomeMessage())
//                             {
//                                 handlePassCommand(&currentClient, params, serverPassword, clients);
//                                 currentClient.setRegisterSteps(1, true);
//                             }
//                             else
//                                 currentClient.getServerReplies().push_back(ERR_ALREADYREGISTERED(std::string("ircserver")));

//                         }
//                         else if (!currentClient.getRegisterSteps(1) && command == "USER")
//                             disconnectClient(&currentClient, clients);
// 	                    else if (currentClient.getRegisterSteps(1) == true && (command == "USER" || command == "NICK"))
//                         {
//                                 if (command == "NICK")
//                                 {
//                                     handleNickCommand(&currentClient, params, nicknames);
//                                 }
//                                 else if (command == "USER")
//                                 {
//                                     if (!currentClient.getHasSentWelcomeMessage())
//                                     {
//                                         handleUserCommand(&currentClient, params);
//                                         if (!currentClient.getHasSentPass())
//                                             disconnectClient(&currentClient, clients);
//                                     }
//                                     else
//                                         currentClient.getServerReplies().push_back(ERR_ALREADYREGISTERED(std::string("ircserver")));
//                                 }
//                                 if (currentClient.getUsername() != "" && currentClient.getNickname() != "") 
//                                 {
//                                      currentClient.setRegisterSteps(2, true);
//                                     currentClient.setIsRegistered(true);
//                                 }
//                         }
//                         else if (command == "PING")
//                         {
//                            	currentClient.getServerReplies().push_back(RPL_PONG(idFormat(currentClient.getNickname(),currentClient.getUsername()),params[0]));

//                         }
//                         if (currentClient.getIsRegistered() && currentClient.getRegisterSteps(2) && !currentClient.getHasSentWelcomeMessage())
//                         {
//                             MOTD(&currentClient);
//                         }
//                         sendRepliesToClient(&currentClient);
//                         currentClient.clearServerReplies(); // Clear replies after sending
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
//     return 0;
// }







// void sendRepliesToClient(Client *client)
// {
//     // Check if the client socket is valid before attempting to send
//     if (client->getSocket() == -1)
//     {
//         return; // Socket is closed, do not attempt to send
//     }

//     for (std::vector<std::string>::size_type j = 0; j < client->getServerReplies().size(); ++j) {
//     ssize_t bytesSent = send(client->getSocket(), client->getServerReplies()[j].c_str(), client->getServerReplies()[j].length(), 0);
//     std::cout << "server sent to client: " + client->getServerReplies()[j] + "\n";

//     // Handle potential errors
//     if (bytesSent < 0) {
//         // Close the socket if sending fails
//         close(client->getSocket());

//         client->setSocket(-1); // Mark the socket as closed
//         return;
//     }
// }
//     // Clear replies after sending
//     client->clearServerReplies();
// }

// void printClientMessages(Client *client) {
//     if (client->getSocket() == -1) {
//         return; // Socket is closed, do not attempt to send
//     }
//     std::cout << "----------------------------------------------------------------\n";
//     for (std::vector<std::string>::const_iterator it = client->getClientMessages().begin(); it != client->getClientMessages().end(); ++it)
//     {
//         std::cout << "Client " << client->getClientNumber() << " sent: " << *it << "\n";
//     }
// }

// void disconnectClient(Client *client, std::map<int, Client> &clients) {
//     client->getServerReplies().push_back(ERR_PASSWDMISMATCH(std::string("ircserver")));
//     client->getServerReplies().push_back(ERROR_MESSAGE(std::string("Password incorrect. Closing connection.")));


//     std::cout << "Client " << client->getClientNumber() << " disconnected.\n";

//     // Send replies before closing the socket
//     sendRepliesToClient(client);

//     client->clearClientMessages();

//     // Close the client's socket
//     close(client->getSocket());

//     // Set the socket to -1 to mark it as closed
//     client->setSocket(-1);

//     // Erase the client from the map
//     clients.erase(client->getSocket());
// }

// void handlePassCommand(Client *client, const std::vector<std::string> &params, const std::string &expectedPassword, std::map<int, Client> &clients) {   
//     (void)clients;
//     if (params.size() != 1) {
//         client->getServerReplies().push_back(ERR_NEEDMOREPARAMS(std::string("ircserver"), "PART"));
//         return;
//     }

//     const std::string &passwordAttempt = params[0];
//     if (passwordAttempt == expectedPassword) {
//         std::string successMessage = "Password accepted\r\n";
//         client->getServerReplies().push_back(successMessage);
//         client->setHasSentPass(true);
//     }
// }

// void handleCapCommand(Client *client, const std::vector<std::string> &params) {
//     if (params.size() > 0 && params[0] == "LS") {
//         client->setRegisterSteps(0, true);
//         client->getServerReplies().push_back(":server CAP * LS :capabilities_here \r\n");
//     } else if (client->getRegisterSteps(0)) {
//         if (params.size() == 1 && params[0] == "REQ") {
//             client->getServerReplies().push_back(":server CAP * REQ :requested_capabilities \r\n");
//         } else if (params.size() == 1 && params[0] == "NAK") {
//             client->getServerReplies().push_back(":server CAP * NAK :rejected_capabilities \r\n");
//         } else if (params.size() == 1 && params[0] == "ACK") {
//             client->getServerReplies().push_back(":server CAP * ACK :accepted_capabilities \r\n");
//         }
//     }
// }

// void handleNickCommand(Client *client, const std::vector<std::string> &params, std::vector<std::string>& nicknames) {
//     if (params.size() < 1) {
//         client->getServerReplies().push_back(ERR_NONICKNAMEGIVEN(std::string("ircserver")));
//         return;
//     }

//     std::string nickName = params[0];


//     if (!nickName.empty() && isdigit(nickName[0])) {
//         client->getServerReplies().push_back(ERR_ERRONEUSNICKNAME(std::string("ircserver"), nickName));
//         return;
//     }

//     if (nickName.length() > 9) {
//         client->getServerReplies().push_back(ERR_ERRONEUSNICKNAME(std::string("ircserver"), nickName));
//         return ;
//     }
    

//     for (std::string::iterator it = nickName.begin(); it != nickName.end(); ++it) {
//         if (!isalnum(*it) && *it != '[' && *it != ']' && *it != '{' && *it != '}'
//             && *it != '|' && *it != '-' && *it != '_') {
//             client->getServerReplies().push_back(ERR_ERRONEUSNICKNAME(std::string("ircserver"), nickName));
//             return;
//         }
//     }

//     if (!nicknames.empty() && std::find(nicknames.begin(), nicknames.end(), nickName) != nicknames.end()) {
//         client->getServerReplies().push_back(ERR_NICKNAMEINUSE(std::string("ircserv"), nickName));
//         return;
//     }

//    if (!client->getNickname().empty()) {
//         nicknames.erase(std::remove(nicknames.begin(), nicknames.end(), client->getNickname()), nicknames.end());
//         client->getServerReplies().push_back(RPL_NICK(client->getNickname(), client->getUsername(), nickName));
//         client->setNickname(nickName);
//         nicknames.push_back(nickName);
//         return;
//     }
   
//     client->setNickname(nickName);
//     nicknames.push_back(nickName);
//     client->getServerReplies().push_back(RPL_NICK(client->getNickname(), client->getUsername(), nickName));
    
// }





// void handleUserCommand(Client *client, const std::vector<std::string> &params) {
//     if (params.size() < 3) {
//         client->getServerReplies().push_back(ERR_NEEDMOREPARAMS(std::string("ircserver"), "USER"));
//         return;
//     }

//     client->setUsername(params[0]);
//     client->setRealName(params[2]);

//     if (client->getHasSentPass()) {
//         std::string successMessage = "USER " + client->getUsername() + " registered successfully!\r\n";
//         client->getServerReplies().push_back(successMessage);
//         client->setIsRegistered(true);
//     }
// }






// void    MOTD(Client *client)
// {
//     std::ostringstream oss;
//     std::string line;
//     std::ifstream infile;

// 	if(!client->getIsRegistered())
// 		return;
//     infile.open("./MOTD.txt", std::ios::in);
	
// 	client->getServerReplies().push_back(RPL_WELCOME(idFormat(client->getNickname(),client->getUsername()),client->getNickname()));
// 	client->getServerReplies().push_back(RPL_YOURHOST(client->getUsername(),"irssi", "1"));
// 	client->getServerReplies().push_back(RPL_CREATED(client->getUsername(), std::string("creation_date")));
// 	client->getServerReplies().push_back(RPL_MYINFO(client->getUsername(),"irssi", "1", "","",""));
// 	client->getServerReplies().push_back(RPL_ISUPPORT(client->getUsername(),"CHANMODES=ikolt"));
// 	client->getServerReplies().push_back(RPL_MOTDSTART(client->getUsername(),std::string("ircserver")));
//     if (infile.is_open())
//     {
//         while (std::getline(infile,line))
//         {
//             client->getServerReplies().push_back(RPL_MOTD(std::string("ircserver"),line));
//         }
//         infile.close();
//     }
//     else
//         client->getServerReplies().push_back(ERR_NOMOTD(std::string("ircserver")));
		
//     client->getServerReplies().push_back( RPL_ENDOFMOTD(std::string("ircserver")));
//     client->setHasSentWelcomeMessage(true);
//     return ;
// }



// #include "Server.hpp"
// #include "Client.hpp"
// #include "Utils.hpp"

// volatile sig_atomic_t running = 1;

// void signalHandler(int signum)
// {
//     std::cout << "\nSignal (" << signum << ") received. Shutting down...\n";
//     running = 0;
// }


#include "Server.hpp"

int main(int argc, char *argv[])
{
    // Check for correct number of command-line arguments
    if (argc != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }

    // Convert the port argument to an integer
    int port = std::atoi(argv[1]);
    // Validate the port number
    if (port <= 0 || port > 65535)
    {
        std::cerr << "Error: Invalid port number. Please provide a valid port (1-65535).\n";
        return 1;
    }

    // Get the server password from command-line arguments
    std::string serverPassword = argv[2];

    try
    {
        // Create an instance of the Server class
        Server server(port, serverPassword);
        // Start the server
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}


// int main(int argc, char *argv[])
// {
    
//     if (argc != 3)
//     {
//         std::cerr << "Usage: ./ircserv <port> <password>\n";
//         return 1;
//     }

//     int port = std::atoi(argv[1]);
//     if (port <= 0 || port > 65535)
//     {
//         std::cerr << "Error: Invalid port number. Please provide a valid port (1-65535).\n";
//         return 1;
//     }

//     std::string serverPassword = argv[2];
// }
    
//     // Server server(port, serverPassword);
//     // server.serverOperations();

//     int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (listenSocket < 0)
//     {
//         std::cerr << "Error: could not create socket.\n";
//         return 1;
//     }

//     // Set socket options
//     int opt = 1;
//     setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

//     // sockaddr_in serverAddr;
//     struct sockaddr_in serverAddr;
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_addr.s_addr = INADDR_ANY;
//     serverAddr.sin_port = htons(port);

//     if (bind(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
//     {
//         std::cerr << "Error: could not bind socket.\n";
//         close(listenSocket);
//         return 1;
//     }

//     listen(listenSocket, 5);
//     std::cout << "Server listening on port " << port << "\n";


// 	std::vector<std::string> nicknames;	
//     std::map<int, Client> clients;
//     struct pollfd fds[MAX_CLIENTS];
//     memset(fds, 0, sizeof(fds));
//     int clientCount = 1;
//     fds[0].fd = listenSocket;
//     fds[0].events = POLLIN;

//     std::signal(SIGINT, signalHandler);

//     while (running)
//     {
//         int pollCount = poll(fds, clientCount, -1);
//         if (pollCount == -1)
//         {
//             break;
//         }

//         for (int i = 0; i < clientCount; i++)
//         {
//             if (fds[i].fd == listenSocket && (fds[i].revents & POLLIN))
//             {
//                 sockaddr_in client;
//                 socklen_t clientSize = sizeof(client);
//                 int clientSocket = accept(listenSocket, (sockaddr *)&client, &clientSize);
//                 if (clientSocket == -1)
//                 {
//                     std::cerr << "Error: failed to accept client.\n";
//                     continue;
//                 }
//                 else
//                 {
//                     fcntl(clientSocket, F_SETFL, O_NONBLOCK);
//                     if (clientCount < MAX_CLIENTS)
//                     {
//                         clients[clientSocket] = Client(clientSocket, clientCount);
//                         fds[clientCount].fd = clientSocket;
//                         fds[clientCount].events = POLLIN;
//                         std::cout << "Client " << clientCount << " connected.\n";
//                         clientCount++;
//                     }
//                     else
//                     {
//                         std::cerr << "Error: maximum clients reached.\n";
//                         close(clientSocket);
//                     }
//                 }
//             }
//             else if (fds[i].revents & POLLIN)
//             {
//                 char buffer[4096];
//                 memset(buffer, 0, sizeof(buffer));
//                 int bytesRecv = recv(fds[i].fd, buffer, sizeof(buffer), 0);

//                 // Handle error for recv
//                 if (bytesRecv == -1)
//                 {
//                     if (errno == EAGAIN || errno == EWOULDBLOCK)
//                     {
//                         // No data available right now; it's okay to ignore this
//                         continue; // Continue to the next client
//                     }
//                     else
//                     {
//                         std::cerr << "Error receiving data from client: " << strerror(errno) << "\n";
//                         // Handle the disconnection logic as necessary
//                         close(fds[i].fd);
//                         clients.erase(fds[i].fd);
//                         fds[i] = fds[clientCount - 1]; // Move the last client into the current spot
//                         fds[clientCount - 1].fd = -1;  // Reset the last position in fds to an invalid fd
//                         clientCount--;
//                         i--; // Decrement i to ensure we don't skip the next client
//                     }
//                 }
//                 else if (bytesRecv <= 0)
//                 {
//                     // Handle client disconnection
//                     std::cout << "Client " << clients[fds[i].fd].getClientNumber() << " disconnected.\n";
//                     close(fds[i].fd);
//                     clients.erase(fds[i].fd);
//                     // Remove the client socket from the fds array
//                     fds[i] = fds[clientCount - 1]; // Move the last client into the current spot
//                     fds[clientCount - 1].fd = -1;  // Reset the last position in fds to an invalid fd
//                     clientCount--;
//                     i--; // Decrement i to ensure we don't skip the next client
//                 }
//                 else
//                 {
//                     std::string message(buffer, bytesRecv);
//                     message.erase(message.find_last_not_of("\r\n") + 1);
//                     Client &currentClient = clients[fds[i].fd];

//                     // Trim and split the message
//                     message = ft_trim(message);
//                     std::vector<std::string> splitMessage = ft_split(message, '\n');
//                     for (std::vector<std::string>::size_type j = 0; j < splitMessage.size(); ++j)
//                     {

//                         std::string line = splitMessage[j];
//                         // Trim each line
//                         std::string trimmedLine = ft_trim(line);
//                         if (trimmedLine.empty())
//                             continue; // Skip empty lines

//                         currentClient.getClientMessages().push_back(trimmedLine);
//                         // std::cout << "Client " << currentClient.clientNumber << " sent: " << trimmedLine << "\n";

//                         // Split command and parameters
//                         std::vector<std::string> tokens = ft_split(trimmedLine, ' ');

//                         if (tokens.empty())
//                             continue;

//                         std::string command = tokens[0];
//                         std::vector<std::string> params(tokens.begin() + 1, tokens.end());

//                         // Debugging output
//                         currentClient.printClientMessages(&currentClient);
//                         std::cout << "Command: " << command << "\n";
//                         for (std::vector<std::string>::size_type j = 0; j < params.size(); ++j)
//                         {
//                             std::string param = params[j];
//                             std::cout << "Parameter[" << j << "]: " << param << "\n";
//                         }
//                         currentClient.clearClientMessages();

//                         // Handle commands
//                         if (command == "CAP")
//                         {
//                             currentClient.handleCapCommand(&currentClient, params);
//                         }
//                         else if (currentClient.getRegisterSteps(0) == true && command == "PASS")
//                         {
//                             if (!currentClient.getHasSentWelcomeMessage())
//                             {
//                                 currentClient.handlePassCommand(&currentClient, params, serverPassword, clients);
//                                 currentClient.setRegisterSteps(1, true);
//                             }
//                             else
//                                 currentClient.getServerReplies().push_back(ERR_ALREADYREGISTERED(std::string("ircserver")));

//                         }
//                         else if (!currentClient.getRegisterSteps(1) && command == "USER")
//                             currentClient.disconnectClient(&currentClient, clients);
// 	                    else if (currentClient.getRegisterSteps(1) == true && (command == "USER" || command == "NICK"))
//                         {
//                                 if (command == "NICK")
//                                 {
//                                     currentClient.handleNickCommand(&currentClient, params, nicknames);
//                                 }
//                                 else if (command == "USER")
//                                 {
//                                     if (!currentClient.getHasSentWelcomeMessage())
//                                     {
//                                         currentClient.handleUserCommand(&currentClient, params);
//                                         if (!currentClient.getHasSentPass())
//                                             currentClient.disconnectClient(&currentClient, clients);
//                                     }
//                                     else
//                                         currentClient.getServerReplies().push_back(ERR_ALREADYREGISTERED(std::string("ircserver")));
//                                 }
//                                 if (currentClient.getUsername() != "" && currentClient.getNickname() != "") 
//                                 {
//                                      currentClient.setRegisterSteps(2, true);
//                                     currentClient.setIsRegistered(true);
//                                 }
//                         }
//                         else if (command == "PING")
//                         {
//                            	currentClient.getServerReplies().push_back(RPL_PONG(idFormat(currentClient.getNickname(),currentClient.getUsername()),params[0]));

//                         }
//                         if (currentClient.getIsRegistered() && currentClient.getRegisterSteps(2) && !currentClient.getHasSentWelcomeMessage())
//                         {
//                             currentClient.MOTD(&currentClient);
//                         }
//                         currentClient.sendRepliesToClient(&currentClient);
//                         currentClient.clearServerReplies(); // Clear replies after sending
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
//     return 0;
// }


