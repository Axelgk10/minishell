/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap_bonus.c                               :+:      :+:    :+:      */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 17:45:00 by gguardam          #+#    #+#             */
/*   Updated: 2025/05/20 17:45:00 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
* @brief Iterates through a list, applies the function
*	'f' to the content of each node
*	and creates a new list with the results.
*	If allocation fails, it clears everything.
*
* @param lst The original list to iterate through.
* @param f The function to apply to each node's content.
* @param del The function used to delete the content in case of failure.
* @return A new list resulting from the successive applications of 'f'
*	or NULL if an error occurs.
*/

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*new_lst;
	t_list	*new_node;

	if (!lst || !f || !del)
		return (NULL);
	new_lst = NULL;
	while (lst)
	{
		new_node = ft_lstnew(f(lst->content));
		if (!new_node)
		{
			ft_lstclear(&new_lst, del);
			return (NULL);
		}
		ft_lstadd_back(&new_lst, new_node);
		lst = lst->next;
	}
	return (new_lst);
}
