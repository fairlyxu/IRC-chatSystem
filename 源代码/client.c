/*
 ============================================================================
 Name        : client.c
 Author      : fairly
 Version     : 1.0
 Copyright   : copyright@fairly
 Description : 聊天程序客户端
 ============================================================================
 */

#ifndef __INEF_H__
#define __INET_H__
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<sys/time.h>
#include<unistd.h>
#include<ctype.h>
#include <errno.h>
#include <resolv.h>
#include <stdlib.h>
#include<time.h>
#define MAX_LINK 10
#define MAX_NAME  100
#define MAX_BUF 1024
#endif

/************关于本文档********************************************
 *filename: client.c
 *********************************************************************/
/*登录函数*/
int login(int sockfd, char * name) {
	char password[100];
	char buffer[100];
	int flag;
	char str[100];

	/**
	 * 假设帐号为Jim,密码为123456
	 * 发送给服务端的识别为  /Jim/123456
	 * */

	printf("\t  请输入您的密码 : ");
	scanf("%s", password);
	strcpy(buffer, "/");
	strcat(buffer, name);
	strcat(buffer, "/");
	strcat(buffer, password); //字符串拼接
	strcat(buffer, "/");
	flag = write(sockfd, buffer, strlen(buffer)); //发送给服务器
	if (flag <= 0) {
		printf("\t*******************************************\n");
		printf("\t**        连接超时，请重试.......   \t **\n");
		printf("\t*******************************************\n");
		fflush(stdout);
		return 0;
	}

	flag = recv(sockfd, str, 1024, 0);
	str[flag] = '\0';

	if (flag <= 0) {
		printf("\t*******************************************\n");
		printf("\t**        服务器忙碌，请重试.......   \t **\n");
		printf("\t*******************************************\n");
		fflush(stdout);
		return 0;
	} else if (strcmp("0", str) == 0) {
		printf("\t*******************************************\n");
		printf("\t**  登录失败，请检查用户名或者密码 \t **\n");
		printf("\t*******************************************\n");
		fflush(stdout);
		return 0;
	} else {

		printf("\n\t*******************************************\n");
		printf("\t**\t  ......登录成功......        \t **\n");
		printf("\t*******************************************\n");
		fflush(stdout);
		sleep(10);

		return 1;
	}
}
/*注册函数*/
int regist(int sockfd) {
	char name[100];
	char password[100];
	char password2[100];
	char buffer[100];

	int flag;
	char str[100];

	/**
	 * 假设注册帐号为Jim,密码为123456
	 * 发送给服务端的识别为  @Jim@123456
	 * */
	system("clear");
	printf("\t*******************************************\n");
	printf("\t**          ....注册新帐号....  \t **\n");
	printf("\t*******************************************\n");
	printf("\t**\t          请输入帐号           \t **\n\t--> ");
	scanf("%s", name);
	printf("\t**\t          请输入密码           \t **\n\t-->");
	scanf("%s", password);
	printf("\t**\t          请确认密码           \t **\n\t-->");
	scanf("%s", password2);
	while (strcmp(password, password2) != 0) {
		printf("\t**\t  两次输入密码不一致，请重试\t **\n");
		printf("\t**\t          请输入密码           \t **\n\t-->");
		scanf("%s", password);
		printf("\t**\t          请确认密码           \t **\n\t-->");
		scanf("%s", password2);
	}
	//拼接字符串，发送到服务器
	strcpy(buffer, "@");
	strcat(buffer, name);
	strcat(buffer, "@");
	strcat(buffer, password); //字符串拼接
	strcat(buffer, "@");
	flag = write(sockfd, buffer, strlen(buffer)); //发送给服务器

	if (flag <= 0) {
		printf("\t*******************************************\n");
		printf("\t**        连接超时，请重试.......   \t **\n");
		printf("\t*******************************************\n");
		fflush(stdout);
		return 0;
	}

	flag = recv(sockfd, str, 1024, 0);
	str[flag] = '\0';
	if (flag <= 0) {
		printf("\t*******************************************\n");
		printf("\t**        连接超时，请重试.......   \t **\n");
		printf("\t*******************************************\n");
		fflush(stdout);
		return 0;
	} else if (strcmp("0", str) == 0) {
		printf("\t*******************************************\n");
		printf("\t**\t  注册失败，请重试......\t **\n");
		printf("\t*******************************************\n");
		sleep(10);
		fflush(stdout);
		return 0;
	} else {

		printf("\t*******************************************\n");
		printf("\t**\t  ......注册成功......        \t **\n");
		printf("\t*******************************************\n");
		sleep(10);
		fflush(stdout);
		return 1;
	}
}
/*发送语句加上时间*/
char* getTime() {
	/*获得当前时间*/
	time_t now;
	struct tm* timenow;
	char timestr[255];

	time(&now);

	timenow = localtime(&now);
	strcpy(timestr, "\n");
strcat(timestr, asctime(timenow));
	
	return &timestr;

}
/*记录聊天文件中*/
int record(char message[MAX_BUF], char name[MAX_NAME]) {
	FILE *cfptr;//文件指针
	if ((cfptr = fopen(name, "a+")) == NULL) {
		printf("无法找到记录文件\n");
		fclose(cfptr);
		return 0;
	} else {
		fprintf(cfptr, "%s\n", message);
		fclose(cfptr);
		return 1;
	}
	fclose(cfptr);
	return 0;
	return 0;
}

/*查看聊天记录*/
int view(char username[MAX_NAME]) {
	char mess[MAX_BUF];
	FILE *cfptr;//文件指针
	if ((cfptr = fopen(username, "r")) == NULL) {
		printf("无法找到记录文件\n");
		fclose(cfptr);
		return 0;
	} else {
		while (!feof(cfptr)) {

			fscanf(cfptr, "%s", mess);

			if ((strcmp(username," ") != 0)) {
				printf("\t %s", mess);
			}
			printf("\n");
		}
	}

	fclose(cfptr);
	return 0;
}

/*私聊启动函数*/
int startP2P(int sockfd) {
	char temp[MAX_BUF];
	int flag;

	char str[MAX_BUF];
	bzero(temp, MAX_BUF);
	strcpy(temp, "#");

	/*
	 * 提出私聊请求的标志 ：    #2/XXXXXXXXX
	 * 发送私聊信息的标志 ：   ##2/XXXXXXXX
	 * */

	flag = write(sockfd, temp, strlen(temp)); //发送给服务器

	if (flag <= 0) {
		printf("\t*******************************************\n");
		printf("\t**        连接超时，请重试.......   \t **\n");
		printf("\t*******************************************\n");
		fflush(stdout);
		return 0;
	}

	bzero(temp, MAX_BUF);
	flag = read(sockfd, temp, MAX_BUF);
	str[flag] = '\0';
	fflush(stdout);
	if (flag <= 0) {
		printf("\t*******************************************\n");
		printf("\t**        连接超时，请重试.......   \t **\n");
		printf("\t*******************************************\n");
		fflush(stdout);
	} else if (strcmp(temp, "0") == 0) {
		system("clear");
		printf("\t*******************************************\n");
		printf("\t**   当前在线人数为0，无法进行私聊...\t **\n");
		printf("\t*******************************************\n");
		sleep(10);
		return 0;
	}
	system("clear");
	printf("\t**          私聊频道启动成功......     \t**\n");
	printf("\t** 当前在线好友为: %s\t**\n", temp);
	fflush(stdout);
	return 1;
}
/*主菜单函数*/
int mainMenu() {
	int choice;
	system("clear");
	printf("\t*******************************************\n");
	printf("\t**           Fairly聊天系统        \t **\n");
	printf("\t**\t          1.登录              \t **\n");
	printf("\t**\t          2.注册              \t **\n");
	printf("\t**\t          3.软件使用说明     \t **\n");
	printf("\t**\t          4.退出              \t **\n");
	printf("\t*******************************************\n");
	printf("\t请选择 ：-----> ");
	scanf("%d", &choice);

	return choice;
}
/*子菜单函数 ：选择聊天方式*/
int nextChoice() {
	int choice;
	system("clear");
	printf("\t*******************************************\n");
	printf("\t**          ....请选择聊天方式....  \t **\n");
	printf("\t**\t          1.群聊频道           \t **\n");
	printf("\t**\t          2.私聊频道           \t **\n");
	printf("\t**\t          3.聊天小助手         \t **\n");
	printf("\t**\t          4.查看历史记录       \t **\n");
	printf("\t**\t          5.退出              \t **\n");
	printf("\t*******************************************\n");

	printf("\t请选择 ：-----> ");
	scanf("%d", &choice);

	return choice;
}

void item() {
	system("clear");
	printf("\t     ------");
	printf("\n\t                                               ///    ");
	printf("\n\t                                             <'  )    \t");
	printf("\n\t                                               )(    \t");
	printf("\n\t    我是一只小小鸟，想要飞却怎么样也飞不高。  ( \\\\    ");
	printf("\n\t                       —————————————————————— ''// —");
	printf("\n\t                                                  \t");
	printf("\n\t****************************************************\n");

}

/*主函数入口*/
int main(int argc, char **argv) {
	int sockfd, len;
	struct sockaddr_in dest; //服务器地址，包括端口
	char str[MAX_BUF]; //发送的字符缓冲区
	fd_set sockset;
	struct timeval tv; //超时限制
	int recvlen;
	int myport = 9999;
	char name[MAX_NAME], temp[MAX_BUF];
	int loginFlag, mychoice = 0;
	char friend[MAX_BUF]; //存储私聊对象的socket描述符号

	/* 创建一个 socket 用于 tcp 通信 */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket");
		exit(errno);
	}

	/* 初始化服务器端（对方）的地址和端口信息 */
	bzero(&dest, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(myport);
	char date[] = "127.0.0.1";
	if (inet_aton(date, (struct in_addr *) &dest.sin_addr.s_addr) == 0) {
		perror(date);
		exit(errno);
	}

	/* 连接服务器 */
	if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0) {
		perror("Connect ");
		exit(errno);
	}

	/* 把集合清空 */
	FD_ZERO(&sockset);
	/* 把标准输入句柄0加入到集合中 */
	FD_SET(0, &sockset);
	/* 把当前连接句柄sockfd加入到集合中 */
	FD_SET(sockfd, &sockset);

	item();
	printf("\t**    欢迎使用Fairly聊天系统....祝您玩的开心....**\n");
	sleep(10);
	while (mychoice != 1) {
		mychoice = mainMenu();
		switch (mychoice) {
		case 1: {
			system("clear");
			printf("\t*******************************************\n");
			printf("\t**\t  ......用户登录......        \t **\n");
			printf("\t*******************************************\n");
			printf("\t  请输入您的用户名 :");

			scanf("%s", name);
			loginFlag = login(sockfd, name);
			while (loginFlag == 0) {
				printf("\t  请输入您的用户名 :");
				scanf("%s", name);
				loginFlag = login(sockfd, name);
			}
record("" ,name);
			break;
		}
		case 2: {
			loginFlag = regist(sockfd);
			//如果登录不成功，则一直堵塞在登录状态，除非退出
			while (loginFlag == 0) {
				loginFlag = regist(sockfd);
			}
			break;
		}

		case 3: {
			system("clear");
			printf("\t*******************************************\n");
			printf("\t**\t  ......软件使用说明......     \t **\n");
			printf("\t*******************************************\n");
			fflush(stdout);
			printf("\t选择登录，若没有帐号，则注册后再登录。\n");
			printf("\t本软件可以进行版面聊天（即群聊）和一对一私聊，\n        具体操作请查询聊天小助手。\n");
			printf("\t           copyright@fairlyXu ，使用请遵循GPL\n");
			sleep(10);
			break;
		}

		case 4: {
			item();
			printf("\t**\t         ......成功退出.....              **");
			printf("\n\t****************************************************\n");
			fflush(stdout);
			exit(0);
			break;
		}
		default:
			printf("\t*******************************************\n");
			printf("\t**\t ....选择错误，请重试....      \t **\n");
			printf("\t*******************************************\n");
			fflush(stdout);
			mychoice = 0;
			break;
		}

	}

	mychoice = 0;//初始化重新下一个选择

	while (1) {
		mychoice = nextChoice();
		switch (mychoice) {
		case 1: {
			system("clear");
			printf("\t***********欢迎进入“大家闲得慌”版面********\t\n");
			printf("\t准备就绪，可以开始聊天了……开始你的七嘴八舌之旅吧～\t\n");
			printf("\t*******************************************\n");
			fflush(stdout);
			while (1) {
				/* 把集合清空 */
				FD_ZERO(&sockset);
				/* 把标准输入句柄0加入到集合中 */
				FD_SET(0, &sockset);
				/* 把当前连接句柄sockfd加入到集合中 */
				FD_SET(sockfd, &sockset);

				/* 设置最大等待时间 */
				tv.tv_sec = 1;
				tv.tv_usec = 0;
				/* 开始等待 */
				recvlen = select(sockfd + 1, &sockset, NULL, NULL, &tv);
				if (recvlen == -1) {
					printf("接受消息出错！ %s\n", strerror(errno));
					break;
				} else if (recvlen == 0) {
					continue;
				} else {
					if (FD_ISSET(sockfd, &sockset)) {
						/* 连接的socket上有消息到来则接收对方发过来的消息并显示 */
						bzero(str, MAX_BUF);
						/* 接收对方发过来的消息，最多接收 MAXBUF 个字节 */
						len = read(sockfd, str, MAX_BUF);
						record(str, name);
						if (len < 0) {
							printf("接受消息出错！ %s\n", strerror(errno));
							break;
						}

						str[len] = '\0';
						printf("%s\n", str);
						fflush(stdout);
					}

					/* 监听有数据要发送 */
					if (FD_ISSET(0, &sockset)) {
						printf("\n我 说:");
						fflush(stdout);
						scanf("%s", str);
						//printf("\n");
						len = strlen(str);
						str[len] = '\0';
						if (str[0] == 'q') {
							sprintf(str, "%s 离开版面聊天室\n", name);
							write(sockfd, str, strlen(str));
							break;
						} else {
							bzero(temp, MAX_BUF);
							strcpy(temp, name);
							strcat(temp, " 说： ");
							strcat(temp, str);
							record(temp, name);
							strcat(temp, getTime());
							strcat(temp, "\n");

							len = write(sockfd, temp, strlen(temp));
							if (len != strlen(temp)) {
								printf("输入错误\n");
								break;
							}
						}
					}
				}
			}
		}
			break;
		case 2: {
			if (startP2P(sockfd) == 1) {
				printf("\t**    請選擇你要連接的朋友編號      \t**\n\t--> ");

				scanf("%s", friend);
				system("clear");
				printf("\t***********欢迎进入“悄悄话”私聊频道********\t\n");
				printf("\t准备就绪，可以开始聊天了……按下回车输入消息即可发信息给对方\n");
				printf("\t*******************************************\n");
				fflush(stdout);
				while (1) {
					/* 把集合清空 */
					FD_ZERO(&sockset);
					/* 把标准输入句柄0加入到集合中 */
					FD_SET(0, &sockset);
					/* 把当前连接句柄sockfd加入到集合中 */
					FD_SET(sockfd, &sockset);

					/* 设置最大等待时间 */
					tv.tv_sec = 1;
					tv.tv_usec = 0;

					/* 开始等待 */
					recvlen = select(sockfd + 1, &sockset, NULL, NULL, &tv);
					if (recvlen == -1) {
						printf("将退出，接受数据出错！ %s\n", strerror(errno));
						break;
					} else if (recvlen == 0) {
						/* printf
						 ("没有任何消息到来，用户也没有按键，继续等待……\n"); */
						continue;
					} else {
						if (FD_ISSET(sockfd, &sockset)) {
							/* 连接的socket上有消息到来则接收对方发过来的消息并显示 */
							bzero(str, MAX_BUF);
							/* 接收对方发过来的消息，最多接收 MAXBUF 个字节 */
							len = read(sockfd, str, MAX_BUF);
							if (len == 0)
								break;
							str[len] = '\0';
							printf("%s", str);
							fflush(stdout);
							record(str, name);
						}
						/* 监听有数据要发送 */

						if (FD_ISSET(0, &sockset)) {
							printf("\n我 说:");
							fflush(stdout);
							scanf("%s", str);
							printf("\n");
							len = strlen(str);
							str[len] = '\0';
							if (str[0] == 'q') {
								sprintf(str, "%s 离开一对一聊天室", name);
								write(sockfd, str, strlen(str));
								break;

							} else {
								bzero(temp, MAX_BUF);
								strcat(temp, "#");
								strcat(temp, "#");
								strcat(temp, friend);
								strcat(temp, "/");
								strcat(temp, name);
								strcat(temp, " 悄悄地对你说： ");
								strcat(temp, str);
								record(temp, name);
								strcat(temp, getTime());
								strcat(temp, "\n");

								strcat(temp, "(若要回复好友信息，请返回私聊频道发送消息....)\n");
								len = write(sockfd, temp, strlen(temp));
								if (len != strlen(temp)) {
									printf("输入错误");
									break;
								}
							}
						}
					}

				}
			}
		}
			break;
		case 3: {
			system("clear");
			printf("\t*******************************************\n");
			printf("\t**\t  ......聊天小助手......     \t **\n");
			printf("\t*******************************************\n");
			char
					helper[] =
							"\t1.群聊 。选择群聊选项，即可与大家进行聊天。\n\t2.私聊。选择私聊频道，根据当前在线朋友，选择对应的序号，即可发起聊天。\n\t3.若在群聊版面就收到某位朋友的私聊信息，若要回复，必须回到私聊版面回复。\n\t4.按q退出聊天室....";
			printf("%s\n ", helper);
			fflush(stdout);
			sleep(10);
		}
			break;

		case 4:
				view (name);
				
				sleep(10);
break ;

		case 5: {
			system("clear");
			item();
			printf("\t**\t         ......成功退出.....              **");
			printf("\n\t****************************************************\n");
			fflush(stdout);
			exit(0);
			break;
		}
		default:
			printf("\t*******************************************\n");
			printf("\t**\t ....选择错误，请重试....      \t **\n");
			printf("\t*******************************************\n");
			fflush(stdout);
			mychoice = 0;
			break;
		}

	}
	/* 关闭连接 */
	close(sockfd);
	return 0;
}


