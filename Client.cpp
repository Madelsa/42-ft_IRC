/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mahmoud <mahmoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 13:33:26 by mahmoud           #+#    #+#             */
/*   Updated: 2024/10/20 20:55:43 by mahmoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// Default constructor
Client::Client()
    : socket(-1), clientNumber(-1), isRegistered(false), hasSentPass(false), hasSentWelcomeMessage(false)
{
    clientMessages.clear();
    serverReplies.clear();
    nickname = "";
    username = "";
    realName = "";
    memset(registerSteps, 0, sizeof(registerSteps));
}

// Parameterized constructor
Client::Client(int socket, int clientNumber)
    : socket(socket), clientNumber(clientNumber), isRegistered(false), hasSentPass(false), hasSentWelcomeMessage(false)
{
    clientMessages.clear();
    serverReplies.clear();
    nickname = "";
    username = "";
    realName = "";
    memset(registerSteps, 0, sizeof(registerSteps));
}

Client::~Client(){}

void Client::MOTD(Client *client)
{
    std::ostringstream oss;
    std::string line;
    std::ifstream infile;

	if(!client->getIsRegistered())
		return;
    infile.open("./MOTD.txt", std::ios::in);
	
	client->getServerReplies().push_back(RPL_WELCOME(idFormat(client->getNickname(),client->getUsername()),client->getNickname()));
	client->getServerReplies().push_back(RPL_YOURHOST(client->getUsername(),"irssi", "1"));
	client->getServerReplies().push_back(RPL_CREATED(client->getUsername(), std::string("creation_date")));
	client->getServerReplies().push_back(RPL_MYINFO(client->getUsername(),"irssi", "1", "","",""));
	client->getServerReplies().push_back(RPL_ISUPPORT(client->getUsername(),"CHANMODES=ikolt"));
	client->getServerReplies().push_back(RPL_MOTDSTART(client->getUsername(),std::string("ircserver")));
    if (infile.is_open())
    {
        while (std::getline(infile,line))
        {
            client->getServerReplies().push_back(RPL_MOTD(std::string("ircserver"),line));
        }
        infile.close();
    }
    else
        client->getServerReplies().push_back(ERR_NOMOTD(std::string("ircserver")));
		
    client->getServerReplies().push_back( RPL_ENDOFMOTD(std::string("ircserver")));
    client->setHasSentWelcomeMessage(true);
    return ;
}

void Client::handleUserCommand(Client *client, const std::vector<std::string> &params) {
    if (params.size() < 3) {
        client->getServerReplies().push_back(ERR_NEEDMOREPARAMS(std::string("ircserver"), "USER"));
        return;
    }

    client->setUsername(params[0]);
    client->setRealName(params[2]);

    if (client->getHasSentPass()) {
        std::string successMessage = "USER " + client->getUsername() + " registered successfully!\r\n";
        client->getServerReplies().push_back(successMessage);
        client->setIsRegistered(true);
    }
}

void Client::handleNickCommand(Client *client, const std::vector<std::string> &params, std::vector<std::string>& nicknames) {
    if (params.size() < 1) {
        client->getServerReplies().push_back(ERR_NONICKNAMEGIVEN(std::string("ircserver")));
        return;
    }

    std::string nickName = params[0];


    if (!nickName.empty() && isdigit(nickName[0])) {
        client->getServerReplies().push_back(ERR_ERRONEUSNICKNAME(std::string("ircserver"), nickName));
        return;
    }

    if (nickName.length() > 9) {
        client->getServerReplies().push_back(ERR_ERRONEUSNICKNAME(std::string("ircserver"), nickName));
        return ;
    }
    

    for (std::string::iterator it = nickName.begin(); it != nickName.end(); ++it) {
        if (!isalnum(*it) && *it != '[' && *it != ']' && *it != '{' && *it != '}'
            && *it != '|' && *it != '-' && *it != '_') {
            client->getServerReplies().push_back(ERR_ERRONEUSNICKNAME(std::string("ircserver"), nickName));
            return;
        }
    }

    if (!nicknames.empty() && std::find(nicknames.begin(), nicknames.end(), nickName) != nicknames.end()) {
        client->getServerReplies().push_back(ERR_NICKNAMEINUSE(std::string("ircserv"), nickName));
        return;
    }

   if (!client->getNickname().empty()) {
        nicknames.erase(std::remove(nicknames.begin(), nicknames.end(), client->getNickname()), nicknames.end());
        client->getServerReplies().push_back(RPL_NICK(client->getNickname(), client->getUsername(), nickName));
        client->setNickname(nickName);
        nicknames.push_back(nickName);
        return;
    }
   
    client->setNickname(nickName);
    nicknames.push_back(nickName);
    client->getServerReplies().push_back(RPL_NICK(client->getNickname(), client->getUsername(), nickName));
    
}


void Client::handleCapCommand(Client *client, const std::vector<std::string> &params) {
    if (params.size() > 0 && params[0] == "LS") {
        client->setRegisterSteps(0, true);
        client->getServerReplies().push_back(":server CAP * LS :capabilities_here \r\n");
    } else if (client->getRegisterSteps(0)) {
        if (params.size() == 1 && params[0] == "REQ") {
            client->getServerReplies().push_back(":server CAP * REQ :requested_capabilities \r\n");
        } else if (params.size() == 1 && params[0] == "NAK") {
            client->getServerReplies().push_back(":server CAP * NAK :rejected_capabilities \r\n");
        } else if (params.size() == 1 && params[0] == "ACK") {
            client->getServerReplies().push_back(":server CAP * ACK :accepted_capabilities \r\n");
        }
    }
}

void Client::handlePassCommand(Client *client, const std::vector<std::string> &params, const std::string &expectedPassword, std::map<int, Client> &clients) {   
    (void)clients;
    if (params.size() != 1) {
        client->getServerReplies().push_back(ERR_NEEDMOREPARAMS(std::string("ircserver"), "PART"));
        return;
    }

    const std::string &passwordAttempt = params[0];
    if (passwordAttempt == expectedPassword) {
        std::string successMessage = "Password accepted\r\n";
        client->getServerReplies().push_back(successMessage);
        client->setHasSentPass(true);
    }
}


void Client::sendRepliesToClient(Client *client)
{
    // Check if the client socket is valid before attempting to send
    if (client->getSocket() == -1)
    {
        return; // Socket is closed, do not attempt to send
    }

    for (std::vector<std::string>::size_type j = 0; j < client->getServerReplies().size(); ++j) {
    ssize_t bytesSent = send(client->getSocket(), client->getServerReplies()[j].c_str(), client->getServerReplies()[j].length(), 0);
    std::cout << "server sent to client: " + client->getServerReplies()[j] + "\n";

    // Handle potential errors
    if (bytesSent < 0) {
        // Close the socket if sending fails
        close(client->getSocket());

        client->setSocket(-1); // Mark the socket as closed
        return;
    }
}
    // Clear replies after sending
    client->clearServerReplies();
}

void Client::printClientMessages(Client *client) {
    if (client->getSocket() == -1) {
        return; // Socket is closed, do not attempt to send
    }
    std::cout << "----------------------------------------------------------------\n";
    for (std::vector<std::string>::const_iterator it = client->getClientMessages().begin(); it != client->getClientMessages().end(); ++it)
    {
        std::cout << "Client " << client->getClientNumber() << " sent: " << *it << "\n";
    }
}

void Client::disconnectClient(Client *client, std::map<int, Client> &clients) {
    client->getServerReplies().push_back(ERR_PASSWDMISMATCH(std::string("ircserver")));
    client->getServerReplies().push_back(ERROR_MESSAGE(std::string("Password incorrect. Closing connection.")));


    std::cout << "Client " << client->getClientNumber() << " disconnected.\n";

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


// Getter methods implementation
int Client::getSocket() const { return socket; }
int Client::getClientNumber() const { return clientNumber; }
bool Client::getIsRegistered() const { return isRegistered; }
bool Client::getHasSentPass() const { return hasSentPass; }
bool Client::getHasSentWelcomeMessage() const { return hasSentWelcomeMessage; }
const std::string &Client::getNickname() const { return nickname; }
const std::string &Client::getUsername() const { return username; }
const std::string &Client::getRealName() const { return realName; }
std::vector<std::string>& Client::getClientMessages() { return clientMessages; }
std::vector<std::string>& Client::getServerReplies()  { return serverReplies; }
bool Client::getRegisterSteps(int index) const { return registerSteps[index]; }


// Setter methods implementation
void Client::setSocket(int sock) { socket = sock; }
void Client::setClientNumber(int num) { clientNumber = num; }
void Client::setIsRegistered(bool reg) { isRegistered = reg; }
void Client::setHasSentPass(bool sent) { hasSentPass = sent; }
void Client::setHasSentWelcomeMessage(bool sent) { hasSentWelcomeMessage = sent; }
void Client::clearClientMessages() { clientMessages.clear(); }
void Client::clearServerReplies() { serverReplies.clear(); }
void Client::setNickname(const std::string &nick) { nickname = nick; }
void Client::setUsername(const std::string &user) { username = user; }
void Client::setRealName(const std::string &real) { realName = real; }
void Client::setRegisterSteps(int index, bool reg) { registerSteps[index] = reg; }
