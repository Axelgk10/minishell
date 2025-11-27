/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstsize_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 17:00:00 by gguardam          #+#    #+#             */
/*   Updated: 2025/05/20 17:00:00 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * ft_lstsize - Counts the number of nodes in a list
 * @lst: The beginning of the list
 *
 * Return: The length of the list
 */

#include "libft.h"

int	ft_lstsize(t_list *lst)
{
	int	count;

	count = 0;
	while (lst)
	{
		count++;
		lst = lst-> next;
	}
	return (count);
}
