/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mahmoud <mahmoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 13:29:32 by mahmoud           #+#    #+#             */
/*   Updated: 2024/10/20 20:48:15 by mahmoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"



// Client class to manage client attributes and actions
class Client
{
private:
    int socket; // Client socket descriptor
    int clientNumber; // Unique client identifier
    bool isRegistered; // Registration status
    bool hasSentPass; // Flag for password sent
    bool hasSentWelcomeMessage; // Flag for welcome message sent
    std::string nickname; // Client nickname
    std::string username; // Client username
    std::string realName; // Client real name
    std::vector<std::string> clientMessages; // Messages received from client
    std::vector<std::string> serverReplies; // Messages to send to client
    bool registerSteps[3]; // Registration steps

public:
    // Default constructor
    Client();
    ~Client();
    // Parameterized constructor
    Client(int socket, int clientNumber);

    void handleNickCommand(Client *client, const std::vector<std::string> &params, std::vector<std::string>& nicknames);
    void MOTD(Client *client);
    void handleUserCommand(Client *client, const std::vector<std::string> &params);
    void handleCapCommand(Client *client, const std::vector<std::string> &params);
    void handlePassCommand(Client *client, const std::vector<std::string> &params,
    const std::string &expectedPassword, std::map<int, Client> &clients);
    void disconnectClient(Client *client, std::map<int, Client> &clients);
    void printClientMessages(Client *client);
    void sendRepliesToClient(Client *client);

    

    // Getter methods
    int getSocket() const;
    int getClientNumber() const;
    bool getIsRegistered() const;
    bool getHasSentPass() const;
    bool getHasSentWelcomeMessage() const;
    const std::string &getNickname() const;
    const std::string &getUsername() const;
    const std::string &getRealName() const;
    std::vector<std::string>& getClientMessages();
    std::vector<std::string>& getServerReplies();
    bool getRegisterSteps(int index) const;


    // Setter methods
    void setSocket(int sock);
    void setClientNumber(int num);
    void setIsRegistered(bool reg);
    void setHasSentPass(bool sent);
    void setHasSentWelcomeMessage(bool sent);
    void clearClientMessages();
    void clearServerReplies();
    void setNickname(const std::string &nick);
    void setUsername(const std::string &user);
    void setRealName(const std::string &real);
    void setRegisterSteps(int index, bool reg); 


};

#endif // CLIENT_HPP
