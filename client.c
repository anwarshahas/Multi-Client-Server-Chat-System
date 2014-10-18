#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<time.h>

time_t now,then;
float delay=0.0;
pthread_t thread;

void error (char *msg)
{
 	perror(msg);
 	exit(0);
}
 

void *get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family==AF_INET)
	{
	return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}




void* receive(void* d)
{
	int sockfd=*((int *)d);
	int a;
	char buf[256];
	char enter; 
	while(1)
	{
		bzero(buf,256);
		a=recv(sockfd,buf,sizeof buf,0);
		if(a<=0)
			error("Error reading from socket");
		else
		printf("\n\nYou have a message: %s\n ",buf);
		//printf("data %d\n",a);
	}
	close(sockfd);
	pthread_exit(NULL);
}








int main(int argc,char** argv)
{
	int sockfd,portno,n,newsockfd,userno;
	char id[100],pswd[100],choice[10];
	//struct sockaddr_in serv_addr;
	//struct hostent *server;
	struct addrinfo hints, *servinfo, *p;
    	int rv;
    	char s[INET6_ADDRSTRLEN];
	char buffer[256];
	FILE *fd;


	if(argc<3)
	{
		fprintf(stderr,"Usage %s Host name port\n",argv[0]);
		exit(0);
	}

	//portno=atoi(argv[2]);
	//sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);


    	memset(&hints, 0, sizeof hints);
    	hints.ai_family = AF_UNSPEC;
    	hints.ai_socktype = SOCK_STREAM;

    	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) 
	{
        	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        	return 1;
    	}

    
    	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
        	if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
		{
            		perror("client: socket");
            		continue;
        	}

        	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
        	    	close(sockfd);
        	    	perror("client: connect");
        	    	continue;
	        }
	
	        break;
	}

    	if (p == NULL) 
	{
	        fprintf(stderr, "client: failed to connect\n");
	        return 2;
	}

    	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
    	printf("client: connecting to %s\n", s);

    	freeaddrinfo(servinfo); // all done with this structure







	/*if(sockfd<0)
		error("ERORR Opening Socket");
	
	server=gethostbyname(argv[1]);
	if(server==NULL)
	{
		fprintf(stderr,"NO such host\n");
		exit(0);
	}


	bzero((char*)&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	bcopy((char*)server->h_addr,(char*)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port=htons(portno);
	newsockfd=connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr));

	if(newsockfd<0)
		error("ERROR conncting");

	else
	{
*/
home:
	printf("1-Register\n2-Login\nEnter you'r choice:");
	fgets(choice,10,stdin);
	system("clear");
	strtok(choice,"\n");
	if(choice[0]=='1')
	{
		n=send(sockfd,"Register",8,0);
			if(n>0)
		printf("You can Register now\n");
	}


	if(choice[0]=='2')
	{
		n=send(sockfd,"Login",5,0);
		if(n>0)
			printf("You can Login now\n");
	}

	if((choice[0]!='1')&&(choice[0]!='2'))
	{
		printf("Enter correct choice\n");
		goto home;
	}
reg:
	bzero(id,100);
username:
	printf("Enter you'r usrename:");
	fgets(id,100,stdin);
	strtok(id,"\n");
	if(strncmp("user-",id,5)!=0)
	{
		printf("USERNAME FORMATE IS NOT CORRECT=> FORMAT  user-<no>\n");
		goto username;
	}
	
	n=send(sockfd,id,strlen(id),0);
password:
	printf("Enter the Password:");
	fgets(pswd,100,stdin);
	strtok(pswd,"\n");
	n=send(sockfd,pswd,strlen(pswd),0);
	bzero(buffer,256);

char report[20];
bzero(report,20);
	system("clear");
	n=recv(sockfd,report,20,0);
	if(n>0)
		printf("%s\n",report);
	if(strcmp(report,"Already Exist")==0)
		goto reg;
	if(strcmp(report,"INCORRECT PASSWORD")==0)
		goto password;
	if(strcmp(report,"NOT EXIST")==0)
		goto reg;
//time(&then);
	printf("----Online Users----\n");
/*
	char online[20];
	int flag=0;
	while(1)
	{
/*
		if(delay>2)
		{
		break;
		}
		time(&now);
		delay = difftime(now,then);
*/
/*		bzero(online,20);
		recv(sockfd,online,20,0);
		if(strncmp(online,"OVER",4)==0)
		{
		flag=1;
		break;
		}
		printf("%s\n",online);
	}

*/
/*
	if(flag=0)
	{
		bzero(online,20);
		recv(sockfd,online,20,0);
		//if(strncmp(online,"OVER",4)==0)
	}
*/

char online[256];
bzero(online,256);
recv(sockfd,online,256,0);
printf("%s\n",online);



	pthread_create(&thread,NULL,receive,(void*)&sockfd);

	while(1)
	{
enter:
		printf("Type Here  :");
		bzero(buffer,256);
		fgets(buffer,255,stdin);
		strtok(buffer,"\n");
		if((strncmp("To user-",buffer,8)==0)||(strcmp("quit",buffer)==0))
		{
			sprintf(buffer,"%s|%s",buffer,id);
			n=send(sockfd, buffer, strlen(buffer), 0);
     
			if(n<0)
			error("ERROR Writing To Socket");
   			if(strncmp(buffer,"quit",4)==0)
   			break; 
		}
		else if(strncmp(buffer,"\n",1)==0)
			goto enter;
		else
		{
			printf("\nMESSAGE FORMAT NOT CORRECT\n");
			goto enter;

		}
 
	}
   	close(sockfd); 
//}    
   	return 0;
    
  }
