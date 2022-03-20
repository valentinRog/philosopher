/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vrogiste <vrogiste@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/20 13:48:06 by vrogiste          #+#    #+#             */
/*   Updated: 2022/03/20 14:54:47 by vrogiste         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

bool	fill_param(t_param *param, int argc, char **argv)
{
	bool	error;

	error = false;
	if (argc < 5 || argc > 6)
		return (true);
	param->number_of_philo = atoi_error(argv[1], &error);
	if (error)
		return (true);
	param->time_to_die = atoi_error(argv[2], &error);
	if (error)
		return (true);
	param->time_to_eat = atoi_error(argv[3], &error);
	if (error)
		return (true);
	param->time_to_sleep = atoi_error(argv[4], &error);
	if (error)
		return (true);
	if (argc == 6)
		param->number_of_eating = atoi_error(argv[5], &error);
	else
		param->number_of_eating = 1;
	param->death = false;
	param->threads_ready = false;
	return (error);
}

void	init_philo(t_param *param, t_list **alst)
{
	for (int i = 0; i < param->number_of_philo; i++)
	{
		t_philo	*philo = malloc(sizeof(t_philo));
		philo->index = i + 1;
		philo->param = param;
		philo->n_eaten = 0;
		lst_add_back(alst, lst_new(philo));
	}
}

void	init_mutex(t_list *lst)
{
	t_list	*node = lst;
	t_param	*param = ((t_philo *)node->content)->param;

	pthread_mutex_init(&param->mutex_ready, NULL);
	pthread_mutex_init(&param->mutex_death, NULL);
	pthread_mutex_init(&param->mutex_print, NULL);
	for (int i = 0; i < (int)lst_size(lst); i++)
	{
		pthread_mutex_init(&((t_philo *)node->content)->mutex_fork, NULL);
		pthread_mutex_init(&((t_philo *)node->content)->mutex_last_eat, NULL);
		node = node->next;
	}
}

static void	death_loop(t_list *lst)
{
	t_list	*node = lst;

	while (true)
	{
		t_philo	*philo = (t_philo *)lst->content;
		pthread_mutex_lock(&philo->mutex_last_eat);
		if (get_time() - philo->last_eat >= (uint16_t)philo->param->time_to_die)
		{
			pthread_mutex_unlock(&philo->mutex_last_eat);
			pthread_mutex_lock(&philo->param->mutex_death);
			philo->param->death = true;
			pthread_mutex_unlock(&philo->param->mutex_death);
			printf("%ld %d", get_time() - philo->param->time_zero, philo->index);
			printf(" died\n");
			break;
		}
		pthread_mutex_unlock(&philo->mutex_last_eat);
		node = node->next;
		usleep(2000);		
	}
}

void	init_threads(t_list *lst)
{
	t_list	*node = lst;
	t_param	*param = ((t_philo *)node->content)->param;
	pthread_mutex_lock(&param->mutex_ready);
	for (int i = 0; i < (int)lst_size(lst); i++)
	{
		pthread_create(&((t_philo *)node->content)->thread, NULL, philoop, node);
		node = node->next;
	}
	param->threads_ready = true;
	param->time_zero = get_time();
	pthread_mutex_unlock(&param->mutex_ready);
	sleep(1);
	death_loop(lst);
	node = lst;
	for (int i = 0; i < (int)lst_size(lst); i++)
	{
		pthread_join(((t_philo *)node->content)->thread, NULL);
		node = node->next;
	}
}