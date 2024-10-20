/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mahmoud <mahmoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 21:06:48 by mahmoud           #+#    #+#             */
/*   Updated: 2024/10/20 21:08:17 by mahmoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "Server.hpp"

// Function to trim whitespace from the start and end of a string
std::string ft_trim(const std::string &str);

// Function to split a string by a given delimiter
std::vector<std::string> ft_split(std::string str, char delimiter);

// Function to get the current date and time in the format: DD-MM-YYYY HH:MM:SS
std::string getCurrentDateTime();

#endif // UTILS_HPP
