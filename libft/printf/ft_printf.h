/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 12:51:12 by axgimene          #+#    #+#             */
/*   Updated: 2025/05/09 18:34:39 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include <unistd.h>
# include <stdlib.h>

int	ft_printf(const char *format, ...);
int	ft_print_char(int c);
int	ft_print_str(char *str);
int	ft_print_nbr(int num);
int	ft_print_ptr(void *ptr);
int	ft_print_unsigned(unsigned int num);
int	ft_print_hex_lower(unsigned int num);
int	ft_print_hex_upper(unsigned int num);
int	ft_print_percent(void);

#endif