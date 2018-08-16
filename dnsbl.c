//TODO get your head around some of the memory management, and get on top of efficient use of variables/evaluations etc. 

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>


u_int32_t main(int argc, char **argv){

	if( argc < 2){
		printf("Please provide a hostname or IP to check against the blacklist\n");
		exit(-1);
	}

	#define MAX_HOSTNAMESIZE 150

	char *result = (char*)malloc(16);
	char *hostquery = (char*)malloc(MAX_HOSTNAMESIZE);
	char *sblquery = (char*)malloc(MAX_HOSTNAMESIZE);
	char oct1[4], oct2[4], oct3[4], oct4[4];
	//THIS WORKS
	char *sblserver = (char*)malloc(MAX_HOSTNAMESIZE);

	//THIS DOES NOT
	//char sblserver[MAX_HOSTNAMESIZE];

	//because of this...
	sblserver = ".bdbmx.com";

	socklen_t len = sizeof(struct sockaddr_storage);

	struct addrinfo hints;
	struct addrinfo *res;

	
	//check for sillybugger length input, then make ourselves a nice DNSBL query
	int s = (int)strlen(argv[1]);
	if (s < MAX_HOSTNAMESIZE){

		char sbl_host[MAX_HOSTNAMESIZE], sbl_service[6];

		//Get the IP of the hostname we are checking against the blacklist
		hints.ai_family = AF_INET;
		hints.ai_protocol = 0;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = 0;

		//build the query to send the dnsbl server
		//TODO improve this method, it looks slow!
		hostquery = argv[1];

		s = getaddrinfo(hostquery, NULL, &hints, &res);
		if (s != 0){
			printf("\"%s\" When trying to look up the given host.\n",gai_strerror(s));
			exit(-1);
		}

   		struct sockaddr_in *rp = (struct sockaddr_in*)res->ai_addr;
		free(res);

		char sbl_query_ip[MAX_HOSTNAMESIZE];

		inet_ntop(AF_INET, &(rp->sin_addr), sbl_query_ip, len);
	
		printf("IP we are checking on the blacklist: %s\n",sbl_query_ip);

		//s = getnameinfo(res->ai_addr, len, sbl_host, MAX_HOSTNAMESIZE, NULL, 0, 0);

		sscanf(sbl_query_ip,"%[0-9].%[0-9].%[0-9].%[0-9]",oct1,oct2,oct3,oct4);
		strncat(sblquery,oct4,3);
		strncat(sblquery,".",1);
		strncat(sblquery,oct3,3);
		strncat(sblquery,".",1);
		strncat(sblquery,oct2,3);
		strncat(sblquery,".",1);
		strncat(sblquery,oct1,3);
		
		strncat(sblquery,sblserver,MAX_HOSTNAMESIZE);
		free(sblserver);

		printf("Query we are sending to DNSBL server: %s\n",sblquery);

		//send the dnsbl format query that we just built
		s = getaddrinfo(sblquery, NULL, &hints, &res);
		free(sblquery);

		if (s != 0){
			printf("%s\n",gai_strerror(s));
		}
		else{
			rp = (struct sockaddr_in*)res->ai_addr;
			inet_ntop(AF_INET, &(rp->sin_addr), result, len);
			printf("%s\n",result);		
		}
	}
	else {
		printf("TOO BIG!\n");
		exit(-1);
	};

}
