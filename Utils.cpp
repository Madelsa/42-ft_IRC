/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mabdelsa <mabdelsa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 21:02:31 by mahmoud           #+#    #+#             */
/*   Updated: 2024/10/21 13:55:01 by mabdelsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"



// Function to trim whitespace
std::string ft_trim(const std::string &str)
{
    std::size_t start = str.find_first_not_of(" \n\r\t");
    std::size_t end = str.find_last_not_of(" \n\r\t");
    if (start == std::string::npos || end == std::string::npos)
    {
        return "";
    }
    return str.substr(start, end - start + 1);
}

// Function to split strings by a delimiter
std::vector<std::string> ft_split(std::string str, char delimiter)
{
    std::vector<std::string> result;
    std::string word;

    for (std::size_t i = 0; i < str.length(); ++i)
    {
        if (str[i] != delimiter)
        {
            word += str[i];
        }
        else
        {
            if (!word.empty())
            {
                result.push_back(word);
                word.clear();
            }
        }
    }
    if (!word.empty())
    {
        result.push_back(word);
    }
    return result;
}
