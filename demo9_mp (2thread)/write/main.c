#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// 定义消息结构体（注意：不能直接传输指针或动态分配的内存）
typedef struct {
    int sensor_id;
    float temperature;
    char timestamp[20];
} SensorData;

int main() {
    // 1. 打开或创建消息队列
    mqd_t mq = mq_open("/sensor_queue", O_CREAT | O_WRONLY, 0666, NULL);
    if (mq == (mqd_t)-1) {
        printf("mq_open error");
        return 1;
    }

    // 2. 准备结构化数据
    SensorData data = {
        .sensor_id = 1001,
        .temperature = 25.5,
        .timestamp = "12:00:00"
    };

    // 3. 发送消息
    if (mq_send(mq, (const char*)&data, sizeof(SensorData), 0) == -1) {
        printf("mq_send error");
        mq_close(mq);
        return 1;
    }

    printf("Sent: sensor_id=%d, temp=%.1f\n", data.sensor_id, data.temperature);

    // 4. 关闭队列（不删除，接收方可能还需读取）
    mq_close(mq);
    return 0;
}