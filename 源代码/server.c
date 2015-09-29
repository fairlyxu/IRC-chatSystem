/*
 ============================================================================
 Name        : server.c
 Author      : fairly
 Version     : 1.0
 Copyright   : copyright@fairly
 Description : 聊天程序服务器端
 ============================================================================
 */


#ifndef __INEF_H__
#define __INET_H__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<sys/time.h>
#include<unistd.h>
#include <errno.h>
#include<ctype.h>
#include<signal.h>
#include <sys/wait.h>
#define MAX_LINK 10   //设置最大连接数
#define MAX_BUF 1024  //缓冲区大小
#define MAX_NAME  100
#define __SELECT__

#endif

/*客户注册写入文件函数*/
int regist(char username[30], char password[20]) {

    FILE *cfptr;//文件指针
    if ((cfptr = fopen("client.txt", "a+")) == NULL) {
        printf("File client.txt could not be opened\n");
        fclose(cfptr);
        return 0;
    } else {
        fprintf(cfptr, "%s %s\n", username, password);
        fclose(cfptr);
        return 1;
    }
    fclose(cfptr);
    return 0;
}
/*客户名字密码检验，是否已经注册（与文件内数据比较）*/
int login(char username[30], char password[20]) {
    char user[30];
    char pass[20];
    FILE *cfptr;//文件指针
    if ((cfptr = fopen("client.txt", "r")) == NULL) {
        printf("File client.txt could not be opened\n");
        fclose(cfptr);
        return 0;
    } else {
        while (!feof(cfptr)) {

            fscanf(cfptr, "%s%s", user, pass);

            if ((strcmp(username, user) == 0) && (strcmp(password, pass) == 0)) {

                fclose(cfptr);
                return 1;
            }
        }
    }

    fclose(cfptr);
    return 0;
}

void enterRoom(char *line, char*name) {
    strcpy(line, "");
    sprintf(line, "%s 进入聊天室", name);
}

//返回当前连接数
int clinkNumber(int user_link[MAX_LINK]) {
    int i = 0;
    while (user_link[i] != 0 && (i < MAX_LINK))
        i++;
    if (i == MAX_LINK)
        return -1;
    return (i);

}

int main(int argc, char **argv) {
    char *notice;
    int sockfd, new_fd;
    //int socketNum[MAX_LINK];  //保存客户端连接数
    int user_link[MAX_LINK];

    int userfd[MAX_LINK]; //保存连接客户端的socket描述符号
    char clientName[MAX_LINK][MAX_NAME];

    char line[MAX_BUF];
    char temp[MAX_BUF];

    int userCount, i, j;
    unsigned int cli_len;
    struct sockaddr_in server_addr, client_addr;//网络地址结构体，包括端口号，IP地址
    int port = 9999; //服务器端口号

    int flag;
    char strfd[10]; //将int型的userfd 转化为 字符串

    int length; //标志符，用来判断当前读写是否成功
    fd_set sockset;
    int maxfd = -1; //用来标志当前连接的最大描述符

    char *nam;        //分割字符串得到姓名和密码，临时保存变量
    char *pass;
    char *tokenPtr;
    //char friend[2] ; //保存

    //指定端口号，创建socket
    /*	if (argv[1])
        myport = atoi(argv[1]);
        else
        myport = 9999;
     */
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    /*  if (argv[3])
        server_addr.sin_addr.s_addr = inet_addr(argv[3]);
        else
        server_addr.sin_addr.s_addr = INADDR_ANY;
     */
    //绑定端口号
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr))
            < 0) {
        perror("bind");
        exit(1);
    }
    printf("-------------监听客户的连接------------\n");
    fflush(stdout);
    //监听端口号
    if (listen(sockfd, MAX_LINK) < 0) {
        perror("listen");
        exit(1);
    }
    cli_len = sizeof(client_addr);
    for (i = 0; i < MAX_LINK; ++i) {
        user_link[i] = 0;
        clientName[i][0] = '\0';
    }

    userCount = 0;
    FD_ZERO(&sockset);
    FD_SET(sockfd,&sockset);

    if (maxfd < sockfd + 1) {
        maxfd = sockfd + 1;
    }
    printf("------------启动聊天系统-----------\n");
    fflush(stdout);
    while (1) {

        select(maxfd, &sockset, NULL, NULL, NULL);
        //如果该client已经连接过服务器
        if (FD_ISSET(sockfd,&sockset) && (userCount = clinkNumber(user_link))
                >= 0) {
            new_fd = accept(sockfd, (struct sockaddr*) &client_addr, &cli_len);

            if (new_fd < 0) {
                user_link[userCount] = 0;
                printf("%d连接失败\n", new_fd);
                fflush(stdout);
            } else {

                user_link[userCount] = 1; //标志端口存在
                userfd[userCount] = new_fd;
                FD_SET(new_fd ,&sockset); //加入端口集合
                if (maxfd < (new_fd + 1))
                    maxfd = new_fd + 1;
                printf("\n-------------用户进程描述符号为 %d 连接服务器------------", new_fd);
                fflush(stdout);
            }
        }
        //监听已注册的端口是否有数据发送
        for (i = 0; i < MAX_LINK; ++i) {
            if ((user_link[i] == 1) && (FD_ISSET(userfd[i],&sockset))) {
                length = read(userfd[i], line, MAX_BUF);
                if (length == 0) //client的 socket已经关闭
                {
                    /*注销已经关闭的端口*/
                    printf("\n-------------%s 已经注销-----------\n", clientName[i]);
                    fflush(stdout);
                    user_link[i] = 0;
                    clientName[i][0] = '\0';
                    FD_CLR(userfd[i],&sockset);
                } else if (length > 0) {
                    strcpy(temp, line);
                    line[length] = '\0';
                    temp[length] = '\0';
                    /*根据不同的字符串提示头进行操作*/

                    /*注册*/
                    if (line[0] == '@') {
                        nam = strtok(temp, "@");
                        pass = strtok(NULL, "@");
                        flag = regist(nam, pass);
                        if (flag == 1) {
                            printf("\n注册成功\n ");
                            fflush(stdout);
                            notice = "1";
                            write(userfd[i], notice, strlen(notice));
                        } else {
                            printf("\n注册失败\n ");
                            fflush(stdout);
                            notice = "0";
                            write(userfd[i], notice, strlen(notice));
                        }
                    }

                    /*登录*/
                    else if (line[0] == '/') {
                        nam = strtok(temp, "/");
                        pass = strtok(NULL, "/");

                        /*检测用户名和密码是否正确*/

                        flag = login(nam, pass);

                        if (flag == 1) {
                            printf("\n在文件夹中找到\n ");
                            fflush(stdout);
                            notice = "1";
                            write(userfd[i], notice, strlen(notice));
                        } else {
                            printf("\n用户名或者密码无法在文件中找到\n ");
                            fflush(stdout);
                            notice = "0";
                            write(userfd[i], notice, strlen(notice));
                        }
                        //第一次进入聊天室&& (clientName[i][0] =="\0")
                        if ((line[0] == '/')) {
                            strcpy(clientName[i], nam);
                            enterRoom(line, clientName[i]);
                        }
                        //播放XX进入聊天室，进行广播
                        if(flag==1){
                            printf("%s\n", line);
                            fflush(stdout);
                        }
                        for (j = 0; j < MAX_LINK && (flag == 1); ++j) {
                            if ((j != i) && (user_link[j] == 1)) {
                                write(userfd[j], line, strlen(line));
                            }
                        }
                    }/*私聊*/
                    else if (line[0] == '#') {
                        /*若以2个##开始的字串，表示开始进行私聊*/
                        if (line[1] == '#') {
                            tokenPtr = strtok(temp, "/");
                            tokenPtr = strtok(NULL, "/");
                            strcpy(temp, tokenPtr);
                            printf("%s\n", temp);
                            fflush(stdout);
                            strfd[0] = line[2];
                            strfd[1] = '\0';
                            write(atoi(strfd), temp, strlen(temp));
                        }
                        /*只有一个#开始的字符串，表示第一次连接私聊
                          判断当前活动用户数是否为0
                         */
                        else {
                            bzero(temp, MAX_BUF);
                            if (userCount < 1) {
                                write(userfd[i], "0", strlen("0"));
                            } else {
                                for (j = 0; j < MAX_LINK; ++j) {
                                    if ((j != i) && (user_link[j] == 1)) {
                                        sprintf(strfd, "%d", userfd[j]);
                                        strcat(temp, strfd);
                                        strcat(temp, "-->");
                                        strcat(temp, clientName[j]);
                                        strcat(temp, "\n");
                                    }
                                }
                                write(userfd[i], temp, strlen(temp));
                            }
                        }
                    }
                    /*版面聊天*/
                    else {
                        printf("%s\n",line);
                        fflush(stdout);
                        for (j = 0; j < MAX_LINK; ++j) {
                            if ((j != i) && (user_link[j] == 1)) {
                                write(userfd[j], line, strlen(line));
                            }
                        }
                    }
                }
            }
        }//結束数据传送的监听
        //重置sockset集合
        FD_ZERO(&sockset);
        FD_SET(sockfd,&sockset);
        for (i = 0; i < MAX_LINK; ++i) {
            if (user_link[i] == 1) {
                FD_SET(userfd[i],&sockset);
            }
        }
    }
    close(sockfd);
    return 0;
}


