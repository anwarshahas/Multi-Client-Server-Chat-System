#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/wait.h>	//not
#include<signal.h>	//not
#include<pthread.h>

#define BACKLOG 10
struct user_data
{
	char name[100];
	int socket;
	char status[10];
}

	user[1000];
	pthread_t thread;
	pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

	char buffer[256],id[100],str[100],pswd[100];
	FILE* fd;
	int no_of_users=0,n;
	//struct sockaddr_in serv_addr,cli_addr;
void* get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family==AF_INET)
	{
	return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void error(char* msg)
{
	perror(msg);
	exit(1);
}

void* server_func(void* sock)
{
	int newsockfd=*((int*)sock),j=0,m,i,current;
	char to[100],from[100],name[256],choice[10];
	bzero(choice,256);
	n=recv(newsockfd,choice,10,0);
checking:
	m=1;
	bzero(id,100);
	bzero(from,100);
	bzero(to,100);
	bzero(str,100);
	bzero(name,256);
	n=recv(newsockfd,id,100,0);
password:
	bzero(pswd,100);
	n=recv(newsockfd,pswd,100,0);
	//printf("%s%s%s\n",choice,id,pswd);
	fd=fopen("login","a+");
	if(strncmp(choice,"Register",8)==0)
	{
		//printf("user want to reg:");
		while(fscanf(fd,"%s",str)!=EOF)
		{
			n=strcmp(id,str);
			if(n==0)
			{
				m=2;
				break;
			}
			fscanf(fd,"%s",str);
		}
		if(m==2)
		{
			send(newsockfd,"Already Exist\0",15,0);
			fclose(fd);
			goto checking;
		}
		else
		{
			fprintf(fd,"%s\t",id);
			fprintf(fd,"%s\n",pswd);
			bzero(user[no_of_users].name,100);
			strcpy(user[no_of_users].name,id);
			user[no_of_users].socket=newsockfd;
			strcpy(user[no_of_users].status,"on_line");
			current=no_of_users;
			no_of_users++;
			fclose(fd);
			send(newsockfd,"Account Registered\0",20,0);
		}
	}

	if(strncmp(choice,"Login",5)==0)
	{
		//printf("user want to login");
		while(fscanf(fd,"%s",str)!=EOF)
		{
			n=strcmp(id,str);
			fscanf(fd,"%s",str);
			if(n==0)
			{
				m=2;
				n=strcmp(pswd,str);
				if(n==0)
				{
					send(newsockfd,"USER LOGGED IN\0",20,0);
					
					for(i=0;i<no_of_users;i++)
					{
						n=strcmp(id,user[i].name);
						if(n==0)
						{
							user[i].socket=newsockfd;
							strcpy(user[i].status,"on_line");
							current=i;
							break;
						}
					}
					break;
				}

				else
				{
					send(newsockfd,"INCORRECT PASSWORD\0",20,0);
					fclose(fd);
					goto password;
				}
			}
		}

		if(m==1)
		{
			send(newsockfd,"NOT EXIST\0",11,0);
			fclose(fd);
			goto checking;
		}
	}
/*
char online[20];
bzero(online,20);
int flag=0;
	for(i=0;i<no_of_users;i++)
	{
	if((strcmp(user[i].status,"on_line")==0)&&(strcmp(user[i].name,id)!=0))
		{
			flag=1;
			strcpy(online,user[i].name);
			strcat(online,"\0");			
			send(newsockfd,online,20,0);
		}

	}
	if(flag==0)
	{
		send(newsockfd,"NO USERS AVAILABLE\0",20,0);
	}
	send(newsockfd,"OVER\0",20,0);
*/

char online[256];
bzero(online,256);
strcpy(online,"\0");
int flag=0;
	for(i=0;i<no_of_users;i++)
	{
	if((strcmp(user[i].status,"on_line")==0)&&(strcmp(user[i].name,id)!=0))
		{
			flag=1;
			//strcpy(online,user[i].name);
			strcat(online,user[i].name);
			strcat(online,"\n");			
			//send(newsockfd,online,20,0);
		}

	}

	strcat(online,"\0");	

	if(flag==0)
	{
		send(newsockfd,"NO USERS AVAILABLE\0",20,0);
	}
	else
	{
		send(newsockfd,online,256,0);
	}






	pthread_mutex_unlock(&mutex);
	bzero(buffer,256);
	int newsockfd2;


printf("%s <===>LOGGED IN\n",id);
	while(1)
	{
rev:
		n=recv(newsockfd,buffer,255,0);
		if(n<0)
  		error("ERROR! Reading From Socket");
   
  		if(strncmp(buffer,"quit",4)==0)
  		{
  			close(newsockfd);
  			strcpy(user[current].status,"off_line");
			user[current].socket=-1;
			printf("%s <===>LOGGED OUT\n",user[current].name);
  			pthread_exit(NULL);
  		} 
   
		i=3;
 
		strcpy(name,buffer);
		while(name[i]!=':')
		{
			if(name[i]=='\0')
			{
			send(newsockfd," MESSAGE FORMAT NOT CORRECT\0",29,0);
			goto rev;
			}
			to[i-3]=name[i];
			i++;
		}
		to[i-3]='\0';
		j=0;
		bzero(buffer,256);
		while(name[i]!='|')
		{
			buffer[j]=name[i];
			i++;
			j++;
		}
		buffer[j]='\0';
		j=0;
		for(i+=1;name[i]!='\0';i++)
		{
			from[j]=name[i];
			j++;
		}
		from[j]='\0';
 
		
		int flag2=0;
		for(j=0;j<no_of_users;j++)
		{
			if(((strncmp(to,user[j].name,strlen(to)))==0)&&(strcmp(user[j].status,"on_line")==0))
			{
				newsockfd2=user[j].socket;
				break;
			}
			if(((strncmp(to,user[j].name,strlen(to)))==0)&&(strcmp(user[j].status,"off_line")==0))
			{
				send(newsockfd,"SENDING FAILED : USER LOGGED OUT\0",34,0);
				flag2=1;
				break;
			}
		}

		char temp[256];
		strcpy(temp,from);
		strcat(temp,buffer);
		bzero(buffer,256);
		strcpy(buffer,"From ");
		strcat(buffer,temp);
		printf("Message::To %s %s\n",to,buffer);
		n=send(newsockfd2,buffer,sizeof buffer,0);
		if((n<0)&&(flag2==0))
		{
			send(newsockfd,"From Server: SENDING FAILED : USER LOGGED OUT\0",47,0);
		}
		else if(n>=0)
		{
			n=send(newsockfd,"From Server: Message send\0",27,0);
			if (n<0)
  				error("ERROR! Writing To Socket");
		}
	}
	close(newsockfd);
	pthread_exit(NULL);
}



int main(int argc,char** argv)
{
	struct addrinfo hints,*serv_info,*p;
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	int yes=1;
	char s[INET6_ADDRSTRLEN];

	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	
		  
	FILE* fd2;
	fd2=fopen("sever","a+");
	fd=fopen("login","w+");

	fprintf(fd2,"server started\n");
	fclose(fd2);
	fclose(fd);
	int sockfd,newsockfd[1000],i,portno,clilen,no=0;

	if (argc<2) 
	{
		fprintf (stderr,"ERROR! Provide A Port!\n");
  		exit(1);
  	}
	//portno = atoi(argv[1]);
	n=getaddrinfo(NULL,argv[1],&hints,&serv_info);
	if(n!=0)
	{
	fprintf(stderr,"getaddrinfo %s\n",gai_strerror(n));
	return 1;
	}

	for(p=serv_info;p!=NULL;p=p->ai_next)
	{
		if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1)
		{
			perror("server:socket\n");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
		{
            		perror("setsockopt");
            		exit(1);
        	}	
		
		if(bind(sockfd,p->ai_addr,p->ai_addrlen)==-1)
		{
			close(sockfd);
			perror("server:bind");
			continue;
		}	
		break;
	}

	if(p==NULL)
	{
		fprintf(stderr,"server:failed to bind\n");
		return 2;
	}
	freeaddrinfo(serv_info);

	if(listen(sockfd,BACKLOG)==-1)
	{
	perror("listen");
	exit(1);
	}

/*
  	sockfd=socket(AF_INET,SOCK_STREAM,0);
  	if(sockfd<0)
	{
		error("ERROR Cannot open socket!");
	}
	
/*
	bzero((char*)&serv_addr,sizeof (serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=INADDR_ANY;
	serv_addr.sin_port=htons(portno);
*/
/*	if(bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
	error("ERROR on binding!");
	listen(sockfd,5);
	clilen=sizeof(cli_addr);
*/
	printf("Waitting for Connection.........\n");
	while(1)
	{

		sin_size=sizeof their_addr;
		newsockfd[no]=accept(sockfd,(struct sockaddr *)&their_addr,&sin_size);

		if(newsockfd[no]==-1)
		{
			perror("accept");
			continue;
		}


		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s,sizeof s);


		/*newsockfd[no]=accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
		if(newsockfd[no]<0)
		error("ERROR on accepting");
		*/
		printf("Connected client:%d\n",no+1);
		printf("server:got connection from %s\n",s);
		pthread_mutex_lock(&mutex);
		pthread_create(&thread,NULL,server_func,(void*)&newsockfd[no]);
		no++;
	}
	close(sockfd);
	close(newsockfd[no]);
	return 0;
}



