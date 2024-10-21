/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mabdelsa <mabdelsa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 13:33:26 by mahmoud           #+#    #+#             */
/*   Updated: 2024/10/21 15:23:31 by mabdelsa         ###   ########.fr       */
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


void Client::sendRepliesToClient(Client *client)
{
    if (client->getSocket() == -1)
    {
        return; // Socket is closed, do not attempt to send
    }

    for (std::vector<std::string>::size_type j = 0; j < client->getServerReplies().size(); ++j) {
    ssize_t bytesSent = send(client->getSocket(), client->getServerReplies()[j].c_str(), client->getServerReplies()[j].length(), 0);
    std::cout << "server sent to client: " + client->getServerReplies()[j];;

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

void Client::disconnectClient(Client *client) {
    client->getServerReplies().push_back(ERR_PASSWDMISMATCH(std::string("ircserv")));
    client->getServerReplies().push_back(ERROR_MESSAGE(std::string("Password incorrect. Closing connection.")));


    std::cout << "Client " << client->getClientNumber() << " disconnected.\n";

    // Send replies before closing the socket
    sendRepliesToClient(client);

    client->clearClientMessages();
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
std::string &Client::getPartialBuffer() { return partialBuffer; }


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
void Client::setPartialBuffer(const std::string &buffer) { partialBuffer = buffer; }
void Client::addRequestedCapability(const std::string &capability) { requestedCapabilities.insert(capability); }

