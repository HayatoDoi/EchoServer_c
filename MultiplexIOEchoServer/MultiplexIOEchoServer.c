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
#include <fcntl.h>

int main(){
	int listen_sock;//接続待ちソケット fd

	struct sockaddr_in listen_sock_addr = {0};//リスニングソケットのアドレス情報

	//Ipv4 TCPソケットの作成
	listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//接続待ちソケットをノンブロックモードにする
	fcntl(listen_sock, F_SETFL, O_NONBLOCK); 

	//サーバソケットアドレスの用意
	listen_sock_addr.sin_family = AF_INET;
	listen_sock_addr.sin_port = htons(9000);
	listen_sock_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

	//リスニングソケットにアドレス情報を付与
	bind(listen_sock, (struct sockaddr*) &listen_sock_addr, sizeof(listen_sock_addr));
	
	//接続待ちソケットを起動する
	listen(listen_sock, SOMAXCONN);

	// 読み取り検査対象のファイルディスクリプタ集合
	fd_set rfds;
	FD_ZERO(&rfds);
	// リスニングソケットをエントリ
	FD_SET(listen_sock, &rfds);

	//クライアント情報
	struct sockaddr_in client_addr;//アドレス情報
	socklen_t client_addr_len;
	char client_ip[256];//IPアドレス
	char client_port[256];//port番号

	//無限ループでクライアント接続処理
	while(true){
		usleep(500 * 1000);

		//loopを回し検査対象の fdを検索
		for(int fd = 0; fd < FD_SETSIZE; fd++) { 
			// 検査対象のFDでなければスキップ
			if(!FD_ISSET(fd, &rfds)) {
				continue;
			}
			// リスニングソケットの検査
			if(fd == listen_sock){
				client_addr_len = sizeof(client_addr);

				//接続済みソケットを作成する
				int connect_sock = accept(listen_sock, (struct sockaddr*) &client_addr, (socklen_t*) &client_addr_len);

				if(connect_sock < 0) {
					continue;
				}

				//接続済みソケットをノンブロックモードに
				fcntl(connect_sock, F_SETFL, O_NONBLOCK); 
				// 検査対象に追加
				FD_SET(connect_sock, &rfds);

				//接続クライアント情報の取り出し
				getnameinfo((struct sockaddr*) &client_addr, sizeof(client_addr), 
					client_ip, 256, client_port, 256,
					NI_NUMERICHOST | NI_NUMERICSERV );
				printf("Connection start %s:%s\n",client_ip,client_port);

				continue;
			}
			//クライアントからの文字列を受け取り
			char srv_buf[1024];
			int srv_buf_len;
			srv_buf_len = recv(fd, srv_buf, sizeof(srv_buf), MSG_NOSIGNAL);

			//受信データがあればエコーバック
			if(srv_buf_len > 0) {
				send(fd, srv_buf, srv_buf_len, MSG_NOSIGNAL);
				printf("%s", srv_buf);
			}

			//ソケットが切断された
			if(srv_buf_len == 0) {
				close(fd);
				//検査対象から外す      
				FD_CLR(fd, &rfds);

				//接続クライアント情報の取り出し
				socklen_t client_addr_len = sizeof(client_addr);
				getpeername(fd, (struct sockaddr *)&client_addr, &client_addr_len);
				getnameinfo((struct sockaddr*) &client_addr, sizeof(client_addr), 
					client_ip, 256, client_port, 256,
					NI_NUMERICHOST | NI_NUMERICSERV );
				printf("Connection end %s:%s\n",client_ip,client_port);
			}
		}
	}

	//接続待ちソケットを閉じる
	close(listen_sock);
	return 0;
}

