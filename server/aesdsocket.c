#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 1024

const char *DATA_FILE = "/var/tmp/aesdsocketdata";

int socket_fd = 0;
int accepted_socket_fd = 0;
struct addrinfo *servinfo = NULL;
FILE *output_file = NULL;
FILE *client_data = NULL;

void signal_handler(int signal)
{
	if (signal == SIGINT || signal == SIGTERM)
	{
		syslog(LOG_INFO, "Caught signal, exiting");
	}
	if (client_data)
	{
		fclose(client_data);
	}
	if (accepted_socket_fd)
	{
		close(accepted_socket_fd);
	}
	if (output_file)
	{
		fclose(output_file);
	}
	if (socket_fd)
	{
		close(socket_fd);
	}
	if (servinfo)
	{
		freeaddrinfo(servinfo);
	}
	// closelog();
	remove(DATA_FILE);
	exit(0);
}


int main(int argc, char *argv[])
{
	// openlog(NULL, 0, LOG_USER);
	syslog(LOG_INFO, "Starting!");
	struct sigaction new_action;
	memset(&new_action,0,sizeof(struct sigaction));
	new_action.sa_handler=signal_handler;
	sigaction(SIGINT, &new_action, NULL);
	sigaction(SIGTERM, &new_action, NULL);

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	// hints.ai_family = AF_UNSPEC;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo("localhost", "9000", &hints, &servinfo);

	socket_fd = socket(PF_INET, SOCK_STREAM, 0);

	if(socket_fd == -1)
	{
		syslog(LOG_INFO, "Socket file descriptor error: %s", strerror(errno));
		return -1;
	}

	syslog(LOG_INFO, "Socket file descriptor success!");

	const int option = 1;

	int setsockopt_status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	if(setsockopt_status != 0)
	{
		syslog(LOG_INFO, "Socket options error: %s", strerror(errno));
		return -1;
	}

	syslog(LOG_INFO, "Socket options success!");

	int bind_status = bind(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen);
	
	if(bind_status != 0)
	{
		syslog(LOG_INFO, "Binding error: %s", strerror(errno));
		return -1;
	}

	syslog(LOG_INFO, "Binding success!");

	if(argc == 2 && strcmp(argv[1], "-d") == 0) 
	{
		// Daemon mode required.
		syslog(LOG_INFO, "Deamon mode requested!");

		pid_t child_pid = fork();

		if (child_pid == -1)
		{
			// We could not create the child.
			syslog(LOG_INFO, "Child creation failed: %s", strerror(errno));
			return -1;
		}

		if (child_pid != 0)
		{
			// We are the parent process.
			syslog(LOG_INFO, "Parent process is returning!");
			return 0;
		}

		// For the child process, let execution continue.
		syslog(LOG_INFO, "Child process continues!");
	}

	int listen_backlog = 128;

	int listen_status = listen(socket_fd, listen_backlog);

	if(listen_status != 0) 
	{
		syslog(LOG_INFO, "Listen error: %s", strerror(errno));
		return -1;
	}

	struct sockaddr input_addr;
	socklen_t input_addrlen = sizeof(input_addr);

	while(1) 
	{
		accepted_socket_fd = accept(socket_fd, (struct sockaddr *)&input_addr, &input_addrlen);

		if(accepted_socket_fd == -1) 
		{
			syslog(LOG_INFO, "Accept error: %s", strerror(errno));
			// Go back to accepting connections.
 			continue;
		}

		struct sockaddr_in *client_addr = (struct sockaddr_in *)&input_addr;
 		
		syslog(LOG_INFO, "Accepted connection from %s", inet_ntoa(client_addr->sin_addr));

		client_data = fdopen(accepted_socket_fd, "rb");

		if(client_data != NULL)
		{
			char *input_line = NULL;
			size_t n_bytes_size = 0;

			ssize_t num_char_read = getline(&input_line, &n_bytes_size, client_data);

			syslog(LOG_INFO, "Received: %s", input_line);

			// If characters were read, write them to the file.
			// Otherwise, let the while loop go back to accepting connections.
			if (num_char_read != -1)
			{
				output_file = fopen(DATA_FILE, "a+");

				fputs(input_line, output_file);
				fflush(output_file);
				free(input_line);
                   
				char buffer[BUFFER_SIZE];
				size_t size = 0;

				fseek(output_file, 0, SEEK_SET);
				while ((size = fread(buffer, sizeof(char), BUFFER_SIZE, output_file)) > 0)
				send(accepted_socket_fd, buffer, size, 0);
				syslog(LOG_INFO, "Sent: %s\n", buffer);
				fclose(output_file);
				output_file = NULL;
					
				fclose(client_data);
				client_data = NULL;
				close(accepted_socket_fd);
				accepted_socket_fd = 0;
				syslog(LOG_INFO, "Closed connection from %s\n", inet_ntoa(client_addr->sin_addr));	
			}
			else
			{
				free(input_line);
			}
		}
	}
	// closelog();
	syslog(LOG_INFO, "Exiting");
	return 0;
}