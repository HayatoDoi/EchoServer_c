#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <netdb.h>
#include <stdlib.h>
#include <pthread.h>

void *th_sub(void *arg);

int main(){
	int listen_sock;//接続待ちソケット fd

	struct sockaddr_in listen_sock_addr = {0};//リスニングソケットのアドレス情報

	//Ipv4 TCPソケットの作成
	listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//サーバソケットアドレスの用意
	listen_sock_addr.sin_family = AF_INET;
	listen_sock_addr.sin_port = htons(9000);
	listen_sock_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

	//リスニングソケットにアドレス情報を付与
	bind(listen_sock, (struct sockaddr*) &listen_sock_addr, sizeof(listen_sock_addr));
	
	//接続待ちソケットを起動する
	listen(listen_sock, SOMAXCONN);

	//無限ループでクライアント接続処理
	while(true){
		int connect_sock;//接続済みソケット fd

		//接続要求が来たら、接続済みソケットを作成する
		/* connect_sock = accept(listen_sock, (struct sockaddr*) &client_addr, (socklen_t*) &client_addr_len); */
		connect_sock = accept(listen_sock, NULL, NULL);//クライアント情報は必要ないので NULL,NULL

		if(connect_sock < 0){
			continue;
		}

		//接続済みソケットのメモリを確保
		pthread_t th;
		int *th_arg;
		th_arg = malloc(sizeof(connect_sock));
		*th_arg = connect_sock;

		//スレッドの作成
		pthread_create(&th, NULL, th_sub, (void *)th_arg);

		//スレッドにデタッチ属性を付与
		pthread_detach(th);
	}

	//接続待ちソケットを閉じる
	close(listen_sock);
}

void *th_sub(void *arg){
	//引数から、接続済みソケットを取り出し
	int connect_sock = *(int *)arg;
	free(arg);
	
	//デタッチ
	/* pthread_detach(pthread_self()); */

	struct sockaddr_in client_addr;//クライアントのアドレス情報
	socklen_t client_addr_len = sizeof(client_addr);
	//accept的な
	getpeername(connect_sock, (struct sockaddr *)&client_addr, &client_addr_len);

	//接続クライアント情報の取り出し
	char client_ip[256];
	char client_port[256];
	getnameinfo((struct sockaddr*) &client_addr, sizeof(client_addr), 
		client_ip, 256, client_port, 256,
		NI_NUMERICHOST | NI_NUMERICSERV );
	printf("Connection start %s:%s\n",client_ip,client_port);

	//ひたすらecho!!
	char srv_buf[1024];
	int srv_buf_len;
	while((srv_buf_len = recv(connect_sock, srv_buf, sizeof(srv_buf), MSG_NOSIGNAL)) > 0){
		printf("%s",srv_buf);
		//文字列の送信
		send(connect_sock, srv_buf, srv_buf_len, MSG_NOSIGNAL);
	}

	//接続済みソケットを閉じる
	printf("Connection end %s:%s\n",client_ip,client_port);
	close(connect_sock);
	return NULL;
}
