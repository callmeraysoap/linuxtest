#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>

// 必须与发送方相同的结构体定义
typedef struct {
    int sensor_id;
    float temperature;
    char timestamp[20];
} SensorData;

int main() {
    // 1. 打开消息队列（只读模式）
    mqd_t mq = mq_open("/sensor_queue", O_CREAT | O_RDONLY , 0666, NULL);
    if (mq == (mqd_t)-1) {
        printf("mq_open error");
        return 1;
    }

    // 2. 获取队列属性（用于确定消息大小）
    struct mq_attr attr;
    mq_getattr(mq, &attr);
    printf("Queue max message size: %ld\n", attr.mq_msgsize);

    // 3. 接收消息
    SensorData received_data;
    ssize_t bytes_read = mq_receive(mq, (char*)&received_data, attr.mq_msgsize, NULL);
    if (bytes_read == -1) {
        printf("mq_receive  error");
        mq_close(mq);
        return 1;
    }

    printf("Received: sensor_id=%d, temp=%.1f, time=%s\n",
           received_data.sensor_id, received_data.temperature, received_data.timestamp);

    // 4. 关闭并删除队列（避免资源泄漏）
    mq_close(mq);
    mq_unlink("/sensor_queue");
    return 0;
}