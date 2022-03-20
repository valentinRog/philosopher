/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vrogiste <vrogiste@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/20 13:50:22 by vrogiste          #+#    #+#             */
/*   Updated: 2022/03/20 14:46:15 by vrogiste         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	wait_other_threads(t_philo *philo)
{
	while (true)
	{
		pthread_mutex_lock(&philo->param->mutex_ready);
		if (philo->param->threads_ready)
		{
			pthread_mutex_unlock(&philo->param->mutex_ready);
			break;
		}
		pthread_mutex_unlock(&philo->param->mutex_ready);
		usleep(50);
	}	
}

static void	eat(t_philo *philo, t_philo *next_philo)
{
	if (philo->index < next_philo->index)
		{
			pthread_mutex_lock(&philo->mutex_fork);
			pthread_mutex_lock(&next_philo->mutex_fork);
		}
		else
		{
			pthread_mutex_lock(&next_philo->mutex_fork);
			pthread_mutex_lock(&philo->mutex_fork);
		}
		monitor(philo, EAT);
		micro_sleep(philo->param->time_to_eat);
		pthread_mutex_unlock(&philo->mutex_fork);
		pthread_mutex_unlock(&next_philo->mutex_fork);
		pthread_mutex_lock(&philo->mutex_last_eat);
		philo->last_eat = get_time();
		pthread_mutex_unlock(&philo->mutex_last_eat);
		philo->n_eaten++;
}

bool	check_death(t_philo *philo)
{
	bool	death;

	pthread_mutex_lock(&philo->param->mutex_death);
	death = philo->param->death;
	pthread_mutex_unlock(&philo->param->mutex_death);
	return (death);
}

void	*philoop(void *arg)
{
	t_list	*node = (t_list *)arg;
	t_philo	*philo = (t_philo*)node->content;
	t_philo	*next_philo = (t_philo*)node->next->content;

	wait_other_threads(philo);
	pthread_mutex_lock(&philo->mutex_last_eat);
	philo->last_eat = get_time();
	pthread_mutex_unlock(&philo->mutex_last_eat);
	while (true)
	{
		if (check_death(philo))
			break ;
		eat(philo, next_philo);
		if (philo->n_eaten >= philo->param->number_of_eating)
			break ;
		monitor(philo, SLEEP);
		micro_sleep(philo->param->time_to_sleep);
		if (check_death(philo))
			break ;
		monitor(philo, THINK);
	}
	return NULL;
}