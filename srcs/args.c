#include "ft_traceroute.h"

/*
	getaddr and fill addr
*/
int			resolve_hostname(char *addr, char *hostname)
{
	int ret;
	struct addrinfo *res;
	struct addrinfo hints;
	char			*buffer;

	ft_memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	ret = getaddrinfo(hostname, NULL, &hints, &res);
	if (!ret)
	{
		buffer = inet_ntoa(((struct sockaddr_in *)res->ai_addr)->sin_addr);
		freeaddrinfo(res);
		if (!buffer)
			return (1);
		ft_strcpy(addr, buffer);
	}
	return (ret);
}

int			handle_options(int argc, char *argv[], int *i, t_traceroute *traceroute)
{
	int		j;
	int		k;
	char	options[NB_OPTIONS] = {'h', 'f', 'm'};

	(void)argc;
	k = 0;
	j = 1;
	while (argv[*i][j])
	{
		k = 0;
		while (k < NB_OPTIONS)
		{
			if (argv[*i][j] == options[k])
				break ;
			k++;
		}
		if (k == NB_OPTIONS)
			break ;
		else
		{
			if (options[k] == 'h')
			{
				show_help();
				traceroute->options.h = 1;
				return (0);
			}
			else if (options[k] == 'm')
			{
				char *str;

				if (ft_strlen(&argv[*i][j + 1]))
					str = &argv[*i][j + 1];
				else if (argc  - 1 < *i + 1)
					return (args_error(traceroute->prg_name, ERR_REQ_ARG, "m", 0, 0));
				else
				{
					*i += 1;
					str = argv[*i];
				}
				traceroute->ttl_val = ft_atoi(str);
				if (traceroute->ttl_val <= 0 || traceroute->ttl_val > 255)
					return (args_error(traceroute->prg_name, ERR_OOR_ARG, str, 1, 255));
				if (!is_num(str))
					return (args_error(traceroute->prg_name, ERR_INV_ARG, str, 0, 0));
				traceroute->options.m = 1;
				return (0);
			}
			else if (options[k] == 'f')
			{
				char *str;

				if (ft_strlen(&argv[*i][j + 1]))
					str = &argv[*i][j + 1];
				else if (argc  - 1 < *i + 1)
					return (args_error(traceroute->prg_name, ERR_REQ_ARG, "m", 0, 0));
				else
				{
					*i += 1;
					str = argv[*i];
				}
				traceroute->first_ttl = ft_atoi(str);
				if (traceroute->first_ttl <= 0 || traceroute->first_ttl > 255)
					return (args_error(traceroute->prg_name, ERR_OOR_ARG, str, 1, 255));
				if (!is_num(str))
					return (args_error(traceroute->prg_name, ERR_INV_ARG, str, 0, 0));
				traceroute->options.f = 1;
				return (0);
			}
		}
		j++;
	}
	if (k == NB_OPTIONS)
	{
		char option[2] = {0, 0};
		option[0] = argv[*i][j];
		args_error(traceroute->prg_name, ERR_INV_OPT, option, 0, 0);
		return (2);
	}
	return (0);
}

int			check_args(int argc, char *argv[], t_traceroute *traceroute)
{
	int		ret;

	ft_memset(traceroute, 0, sizeof(t_traceroute));
	traceroute->prg_name = argv[0];
	traceroute->hostname = NULL;
	if (argc < 2)
	{
		show_help();
		traceroute->options.h = 1;
		return (0);
	}
	ft_memset(&traceroute->options, 0, sizeof(t_options));
	for (int i = 1; i < argc; i++)
	{
		if (*argv[i] != '-' || (ft_strlen(argv[i]) == 1))
		{
			if (traceroute->hostname)
				return (args_error(traceroute->prg_name, ERR_NB_DEST, NULL, 0, 0));
			traceroute->hostname = argv[i];
			ret = resolve_hostname(traceroute->address, traceroute->hostname);
			if (ret)
				return (getaddrinfo_error(traceroute->prg_name, ret, traceroute->hostname));
			else if (!traceroute->address)
			{
				dprintf(STDERR_FILENO, "%s: inet_ntoa: Error\n", traceroute->prg_name);
				return (2);
			}
		}
		if (*argv[i] == '-')
		{
			ret = handle_options(argc, argv, &i, traceroute);
			if (ret)
				return (ret);
		}
	}
	return (0);
}
