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
#include <sys/wait.h>

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
		struct sockaddr_in client_addr;//クライアントのアドレス情報
		socklen_t client_addr_len = sizeof(client_addr);

		//接続要求が来たら、接続済みソケットを作成する
		connect_sock = accept(listen_sock, (struct sockaddr*) &client_addr, (socklen_t*) &client_addr_len);
		if(connect_sock < 0){
			continue;
		}

		//子プロセス作成前にSIGCHLDを無効化にする設定
		struct sigaction sa;
		sa.sa_handler = SIG_IGN;
		sa.sa_flags = SA_NOCLDWAIT;
		if(sigaction(SIGCHLD, &sa, NULL) == -1) {
			exit(1);
		}

		//子プロセスの作成
		pid_t p_id = fork();

		/////親プロセスの処理
		////////////////////////////////////////////////////////////////////////////////////////////////
		if(p_id != 0){
			//すぐに接続済みソケットを閉じる
			close(connect_sock);
		}

		/////子プロセスの処理
		////////////////////////////////////////////////////////////////////////////////////////////////
		else{
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

			//子プロセスの終了
			exit(0);
		}
		////////////////////////////////////////////////////////////////////////////////////////////////
	}

	//接続待ちソケットを閉じる
	close(listen_sock);
}

