#include"Socket.h"
#include<unistd.h>
#include<errno.h>

Socket client,server;
void whenServerError(Socket *skt){
	printf("服务端错误 %d\n",skt->errorNumber);
}
void whenServerAccepted(Socket *skt){
	printf("服务端收到连接\n");
}

void whenClientError(Socket *skt){
	printf("客户端错误 %d\n",skt->errorNumber);
}
void whenClientConnected(Socket *skt){
	printf("客户端连接成功\n");
}

int main(int argc,char* argv[]){
	auto address="127.0.0.1";
	uint16 port=2048;
	//服务端
	server.whenSocketError=whenServerError;
	server.whenSocketAccepted=whenServerAccepted;
	server.listenPort(port);
	//客户端
	client.whenSocketError=whenClientError;
	client.whenSocketConnected=whenClientConnected;
	client.connect(address,port);
	client.connect(address,port);
	//等待完成
	usleep(1000000);
	printf("main结束\n");
	return 0;
}