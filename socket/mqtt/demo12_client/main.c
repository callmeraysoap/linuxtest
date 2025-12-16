#include <asm-generic/socket.h>
#include <bits/types/struct_timeval.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

int mqtt_connect_packet(unsigned char *buf, const char *client_id) {
    int i = 0;
    int Datalen;
    buf[i++] = 0x10; // CONNECT 报文类型
    // buf[i++] = 12 + strlen(client_id); // 剩余长度
    /* 可变剩余长度 */
    Datalen = 10 + 2 + strlen(client_id);
    do {
        unsigned char EncodedByte = Datalen % 128;
        Datalen = Datalen / 128;
        if (Datalen > 0)
            EncodedByte = EncodedByte | 128;
        buf[i++] = EncodedByte;
    }while(Datalen > 0);
    // 固定头部
    buf[i++] = 0x00; 
    buf[i++] = 0x04; // "MQTT"
    buf[i++] = 'M'; 
    buf[i++] = 'Q'; 
    buf[i++] = 'T'; 
    buf[i++] = 'T';
    buf[i++] = 0x04; // 协议版本 4 (MQTT 3.1.1)
    buf[i++] = 0x02; // Connect Flags: Clean Session
    buf[i++] = 0x00; 
    buf[i++] = 0x3C; // Keep Alive = 60 秒

    // Client ID
    buf[i++] = 0x00; 
    buf[i++] = strlen(client_id);
    memcpy(buf + i, client_id, strlen(client_id));
    i += strlen(client_id);

    return i;
}

int mqtt_publishdate(unsigned char *buf, char * topic, char * message, unsigned char qos) {
    int i = 0;
    int topic_len = strlen(topic);
    int message_len = strlen(message);
    int Datalen;
    /*
    // 0x30 固定 0x3x 
    // bit3 dup置表示重发数据 qos为0时需要置为0
    // bit2 bit1 质量定义 0-2
    // bit0 RETAIN 遗留信息
    */
    buf[i++] = 0x30; 

    /* 剩余长度 = 可变报头加上有效负载的长度 qos不为0时需要添加报文标识符*/
    if (qos)
        Datalen = 2 + topic_len + 2 + message_len;
    else
        Datalen = 2 + topic_len + message_len;
    do {
        unsigned char EncodedByte = Datalen % 128;
        Datalen = Datalen / 128;
        if (Datalen > 0)
            EncodedByte = EncodedByte | 128;
        buf[i++] = EncodedByte;
    }while(Datalen > 0);
    /*主题长度*/
    buf[i++] = (unsigned char)(topic_len >> 8); 
    buf[i++] = topic_len & 0xff;
    memcpy(buf + i, topic, topic_len);
    i += topic_len;
    //报文内容
    if (qos)
    {
        buf[i++] = (message_len >> 8); 
        buf[i++] = message_len & 0xff;
    }
    memcpy((buf + i), message, message_len);
    i += message_len;
    return i;
}

int mqtt_Subscribe_topic(unsigned char *buf, char * topic, unsigned char qos) {
    int i = 0;
    int topic_len = strlen(topic);
    int Datalen;
    /*
    协议规定
    */

    buf[i++] = 0x82; 
    /*剩余长度*/
    Datalen = 2 +  topic_len + 1;    
    do {
        unsigned char EncodedByte = Datalen % 128;
        Datalen = Datalen / 128;
        if (Datalen > 0)
            EncodedByte = EncodedByte | 128;
        buf[i++] = EncodedByte;
    }while(Datalen > 0);

    /*主题长度*/
    buf[i++] = (unsigned char)(topic_len >> 8); 
    buf[i++] = (unsigned char)topic_len;
    memcpy(buf + i, topic, topic_len);
    i += topic_len;

    buf[i++] = qos;

    return i;
}

int main() {
    int sock;
    struct sockaddr_in server;
    unsigned char buffer[1024];

    // 1. 创建 socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv)) {
        perror("setsockopt");
        return 1;
    }
    // 2. 设置服务器地址 (本地 Mosquitto 默认端口 1883)
    server.sin_family = AF_INET;
    server.sin_port = htons(1883);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 3. 连接服务器
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        exit(1);
    }

    // // 4. 发送 CONNECT 报文
    int len = mqtt_connect_packet(buffer, "testClient1");
    write(sock, buffer, len);

    ssize_t recv_size = recv(sock, buffer, sizeof(buffer), 0);
    if (recv_size < 0) {
        if(errno == EWOULDBLOCK || errno == EAGAIN) {
            printf("超时\n");
        } else {
            perror("接收错误\n");
        }
    } else if (recv_size == 0) {
        perror("连接被关闭\n");
    } else {
        printf("接收到数据\n");
    }

    if (recv_size > 0) {
        printf("Received CONNACK: ");
        for (int j = 0; j < recv_size; j++) {
            printf("%02X ", (unsigned char)buffer[j]);
        }
        printf("\n");     
    }
    
    while(1) {
        len = mqtt_publishdate(buffer, "a/b", "{\"json\":\"test\"}", 0);
        write(sock, buffer, len);
        printf("sleep 1 %s \n", "{\"json\":\"test\"}");
        sleep(2);
    }
    close(sock);
    return 0;
}