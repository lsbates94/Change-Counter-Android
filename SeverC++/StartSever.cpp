#include "StartSever.h"
int main(int argc, char *argv[])
{
	char* filePath = "Img.jpg";
	FILE *image = fopen(filePath, "wb");
	WSADATA wsa;
	SOCKET master, new_socket, client_socket[30], s;
	struct sockaddr_in server, address;
	int max_clients = 30, activity, addrlen, i, valread;
	char *message = "Coin count \r\n";
	//size of our receive buffer, this is string length.
	int MAXRECV = 1024;
	//set of socket descriptors
	fd_set readfds;
	//1 extra for null character, string termination
	char *buffer;
	buffer = (char*)malloc((MAXRECV + 1) * sizeof(char));

	for (i = 0; i < 30; i++)
	{
		client_socket[i] = 0;
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		exit(EXIT_FAILURE);
	}

	//Create a socket
	if ((master = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		exit(EXIT_FAILURE);
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(27015);

	//Bind
	if (bind(master, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		exit(EXIT_FAILURE);
	}

	//Listen to incoming connections
	listen(master, 3);

	addrlen = sizeof(struct sockaddr_in);

	while (TRUE)
	{
		//clear the socket fd set
		FD_ZERO(&readfds);

		//add master socket to fd set
		FD_SET(master, &readfds);

		//add child sockets to fd set
		for (i = 0; i < max_clients; i++)
		{
			s = client_socket[i];
			if (s > 0)
			{
				FD_SET(s, &readfds);
			}
		}

		//wait for an activity on any of the sockets, timeout is NULL , so wait indefinitely
		activity = select(0, &readfds, NULL, NULL, NULL);

		if (activity == SOCKET_ERROR)
		{
			exit(EXIT_FAILURE);
		}

		//If something happened on the master socket , then its an incoming connection
		if (FD_ISSET(master, &readfds))
		{
			if ((new_socket = accept(master, (struct sockaddr *)&address, (int *)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			//send new connection greeting message
			//if (send(new_socket, message, strlen(message), 0) != strlen(message))
			//{
				//perror("send failed");
			//}

			
			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				if (client_socket[i] == 0)
				{
					client_socket[i] = new_socket;
					break;
				}
			}
		}

		//else its some IO operation on some other socket :)
		for (i = 0; i < max_clients; i++)
		{
			s = client_socket[i];
			//if client presend in read sockets             
			if (FD_ISSET(s, &readfds))
			{
				//get details of the client
				getpeername(s, (struct sockaddr*)&address, (int*)&addrlen);

				valread = recv(s, buffer, MAXRECV, 0);

				if (valread == SOCKET_ERROR)
				{
					int error_code = WSAGetLastError();
					if (error_code == WSAECONNRESET)
					{
						//Close the socket 
						closesocket(s);
						client_socket[i] = 0;
					}
					else
					{
						
					}
				}
				if (valread <= 0)
				{
					//Close the socket
					closesocket(s);
					client_socket[i] = 0;
				}

				//Echo back the message that came in
				else
				{
					buffer[valread] = '\0';	
					int filesize = atoi(buffer);


					if (image == NULL) {
						return -1;
					}
					char buf[4096];
					while (filesize > 0){
						int n = recv(s, buf, sizeof buf, 0);
						fwrite(buf, n, 1, image);
						filesize -= n;
					}
					fclose(image);
					createMat(filePath);
					float total = testSVM();

					cout << total << endl;

					
					send(s, message, strlen(message), 0);
				}
			}
		}
	}
	closesocket(s);
	WSACleanup();

	return 0;
}