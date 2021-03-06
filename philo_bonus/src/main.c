/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vrogiste <vrogiste@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/11 16:45:39 by vrogiste          #+#    #+#             */
/*   Updated: 2022/03/27 09:48:23 by vrogiste         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	main(int argc, char **argv)
{
	t_param	param;
	t_list	*lst;

	lst = NULL;
	if (init_param(&param, argc, argv) || init_philo(&param, &lst))
	{
		close_param_sem(&param);
		unlink_param();
		return (1);
	}
	launch_process(lst);
	lst_clear(lst);
	close_param_sem(&param);
	unlink_param();
	return (0);
}
