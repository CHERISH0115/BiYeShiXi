#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LED_PATH "/sys/class/leds/led3/brightness"

// 控制LED
int control_led(int state) {
    printf("echo %d > %s\n", state, LED_PATH);
    printf("已执行 echo %d > %s 指令\n", state, LED_PATH);
    
    FILE *fp = fopen(LED_PATH, "w");
    if (!fp) {
        return -1;
    }
    
    fprintf(fp, "%d", state);
    fclose(fp);
    
    return 0;
}

// LED闪烁5次，间隔200ms
void blink_led() {
    for (int i = 0; i < 5; i++) {
        control_led(1);
        usleep(200000);  // 200ms
        control_led(0);
        usleep(200000);  // 200ms
    }
}

int main(int argc, char *argv[]) {
    int thunderstorm_detected = 0;
    
    // 检查命令行参数
    if (argc > 1) {
        thunderstorm_detected = atoi(argv[1]);
    }
    
    // 控制LED
    if (thunderstorm_detected) {
        printf("\n");
        printf("╔════════════════════════════════╗\n");
        printf("║     ⚠️  警报灯已启动           ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║  状态: 正在闪烁警报            ║\n");
        printf("║  次数: 5次                     ║\n");
        printf("║  间隔: 200ms                   ║\n");
        printf("╚════════════════════════════════╝\n");
        printf("\n");
        blink_led();
    } else {
        printf("\n");
        printf("╔════════════════════════════════╗\n");
        printf("║     ✅ 天气正常                ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║  状态: 未检测到雷雨天气        ║\n");
        printf("║  操作: 警报灯保持熄灭          ║\n");
        printf("╚════════════════════════════════╝\n");
        printf("\n");
        control_led(0);
    }
    
    return 0;
}
