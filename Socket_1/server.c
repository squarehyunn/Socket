/*
 * server1.c
 *
 *  Created on: 2022. 3. 23.
 *      Author: atec
 */


//소켓사용시 필요한 헤더파일
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//여기까지
#include <pthread.h>

#define CLNT_MAX 10
#define BUFFSIZE 200

int g_clnt_socks[CLNT_MAX];
int g_clnt_count = 0;
int g_led_fd = -1;
int g_serv_sock;

void *send_msg(void * arg){
	int clnt_sock = (int)arg;
	char chat[100];
	char msg[100];
	while(1){
		printf("채팅 입력 : ");
		gets(msg);
		printf("msg : %s",msg);
		if(!strcmp(msg,"end")){
			printf("end입력됨\n");
			close(clnt_sock);
		}
		//sprintf(chat,"[server] : %s",msg);
		write(clnt_sock,msg,strlen(msg)+1); //내가 읽은(연락하고있는) 클라이언트만 빼고, 다른 클라이언트에게 똑같이 다 전달
		sleep(1);
	}
	pthread_exit(0);
}

void *rcv_msg(void * arg){

		printf("g_serv_sock : %d",g_serv_sock);


		int clnt_sock = (int)arg;
		int str_len = 0;

		char msg[BUFFSIZE];
		int i;

		int timeout = 4;
		int ret = -1, read_len, read_idx = 0, time_out_cnt = timeout*100;
		char read_buff[BUFFSIZE], read_char = 0;


		struct timeval recv_timeout;
		fd_set read_fds, cpy_fds;

		memset(msg,0,sizeof(msg));

		FD_ZERO(&read_fds);
		FD_SET(g_serv_sock,&read_fds);

		recv_timeout.tv_sec = timeout;
		recv_timeout.tv_usec = timeout*1000;


		printf("before while \n");
		   while(1){
//			cpy_fds = read_fds;
//			ret = select(g_serv_sock+1, &cpy_fds, 0, 0, &timeout);
//			if(ret == -1){
//				printf("통신에러 발생.\n");
//				return 0;
//			}else if(ret > 0){
			    memset(msg, 0, sizeof(msg));
				str_len = read(clnt_sock, msg, sizeof(msg)); //여기서 읽고
					if(str_len<=0){
						printf("소켓 끊어짐\n");
						pthread_exit(0);
						//break;
					}

				printf("\n%s\n",msg);
//			}
//
		   }


			pthread_exit(0);
			return NULL;


}

void *check(void * arg){
		int str_len = 0;
		char msg[BUFFSIZE];

		int timeout = 4;
		int ret = -1, read_len, read_idx = 0, time_out_cnt = timeout*100;
		char read_buff[BUFFSIZE], read_char = 0;


		struct timeval recv_timeout;
		fd_set read_fds;

		memset(msg,0,sizeof(msg));

		FD_ZERO(&read_fds);
		FD_SET(g_serv_sock,&read_fds);

		recv_timeout.tv_sec = timeout;
		recv_timeout.tv_usec = 0;

	while(1){
		ret = select(g_serv_sock+1,&read_fds, 0,0,&timeout);
		if(ret <= 0){
			printf("통신에러 발생.\n");
			return 0;
		}else if(ret > 0){
			printf("이상 무\n");
		}

	}
}

int main(int args, char **argv){
	//int serv_sock;
	int clnt_sock;

	pthread_t t_thread[2];

	struct sockaddr_in clnt_addr;
	int clnt_addr_size;

	struct sockaddr_in serv_addr;

	g_serv_sock = socket(PF_INET, SOCK_STREAM,0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(g_serv_sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr))==-1){
		printf("bind error\n");
	};

	if(listen(g_serv_sock,5)==-1){
		printf("listen error\n");
	};

	char buff[200];
	int recv_len = 0;
	//while(1){
		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(g_serv_sock,(struct sockaddr *)&clnt_addr,&clnt_addr_size);

		pthread_create(&t_thread[0],NULL,rcv_msg,(void *)clnt_sock);
		pthread_create(&t_thread[1],NULL,send_msg,(void *)clnt_sock);
	//}

		pthread_join(t_thread[0],NULL);
		pthread_join(t_thread[1],NULL);
		close(clnt_sock);

}

