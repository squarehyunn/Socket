/*
 * client.c
 *
 *  Created on: 2022. 3. 28.
 *      Author: atec
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//소켓사용시 필요한 헤더파일
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//여기까지
#include <pthread.h>

#define BUFFSIZE 100
#define NAMESIZE 20

char message[BUFFSIZE];

void * rcv(void *arg){
	//printf("rcv thread created\n");
	int sock = (int)arg;
//	char buff[5000];
	char buff[200];
	int len;
	while(1){
		memset(buff,0,sizeof(buff));
		len = read(sock,buff,sizeof(buff));

//		FILE * des = fopen("save/test.png","wb");
//		fwrite((void *)buff,1,len,des);

		if(len <=0){
			printf("소켓 끊어짐\n");
			pthread_exit(0);
			//break;s
		}
			//printf("buff = %s",buff);

		if(!strcmp(buff,"#")){
			printf("#입력됨 종료\n");
			exit(0);
			break;
		}

		printf("\n[server] : %s\n",buff);
		//fclose(des);
	}
	pthread_exit(0);
	return 0;
}

int main(int argc,char **argv){

	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;
	void * thread_result;

	char id[200];
	printf("argc : %d\n",argc);
	if(argc<2){
		printf("you have to enter ID");
		return 0;
	}
	strcpy(id,argv[1]);
	printf("id : %s\n",id);
	sock = socket(PF_INET, SOCK_STREAM, 0);//소켓 구멍 만들고
	if(sock==-1){
		printf("socket error\n");
	}else{
		printf("socket ok\n");
	}


	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;//어디 접속할 것인지 정하고
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//주소는 같은 pc인 로컬로
	serv_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr))==-1){
		printf("connect error");
	}else{
		printf("connection success\n");
	}

	pthread_create(&rcv_thread,NULL,rcv,(void *)sock);

	char chat[100];
	char msg[1000];

	printf("while before\n");
	while(1){//보내기
		printf("채팅 입력 : ");
		gets(chat);
		sprintf(msg,"[%s] : %s\n",id,chat);
		printf("(send) %s \n",msg);
		write(sock,msg,strlen(msg)+1);//위의 15개의 숫자(15개의 패킷)을 계속 보낸다.
		sleep(1); //너무 빠르니까 1초에 한번씩 보내기
	}

	close(sock);
	return 0;


}

