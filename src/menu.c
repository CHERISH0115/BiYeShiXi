#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define CAMERA_CAPTURE "./camera_capture"
#define YUV_TO_JPEG "./yuv_to_jpeg"
#define AI_TEST "./test"
#define LED_CONTROL "./led"
#define THUNDER "./thunder"
#define DEVICE "/dev/video0"
#define YUV_FILE "test.yuv"
#define JPG_FILE "test.jpg"

int weather_result = 0;
char ai_output[4096] = {0};

void clear_screen() {
    printf("\033[2J\033[H");
}

void print_header(const char *title) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  %-56s║\n", title);
    printf("╚══════════════════════════════════════════════════════════╝\n");
}

void print_box(const char *label, const char *content) {
    printf("┌──────────────────────────────────────────────────────────┐\n");
    printf("│  %-12s: %-41s│\n", label, content);
    printf("└──────────────────────────────────────────────────────────┘\n");
}

void print_success(const char *message) {
    printf("\n");
    printf("  ✅  %s\n", message);
    printf("\n");
}

void print_error(const char *message) {
    printf("\n");
    printf("  ❌  %s\n", message);
    printf("\n");
}

void print_info(const char *message) {
    printf("  ℹ️   %s\n", message);
}

void print_warning(const char *message) {
    printf("\n");
    printf("  ⚠️   %s\n", message);
    printf("\n");
}

void print_separator() {
    printf("────────────────────────────────────────────────────────────\n");
}

void print_loading(const char *message) {
    printf("\n  ⏳  %s", message);
    fflush(stdout);
}

void print_done() {
    printf(" 完成!\n\n");
}

void wait_and_return() {
    printf("\n");
    printf("  ┌────────────────────────────────────────────────────────┐\n");
    printf("  │  ⏱️  1秒后自动返回主菜单...                            │\n");
    printf("  └────────────────────────────────────────────────────────┘\n");
    sleep(1);
}

void show_welcome() {
    clear_screen();
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║                                                          ║\n");
    printf("║                                                          ║\n");
    printf("║           📷  智能摄像头控制系统  📷                     ║\n");
    printf("║                                                          ║\n");
    printf("║              版本 1.0                                    ║\n");
    printf("║                                                          ║\n");
    printf("║   本系统支持：                                           ║\n");
    printf("║     • 拍照采集图像                                       ║\n");
    printf("║     • 格式转换处理                                       ║\n");
    printf("║     • AI智能问答                                         ║\n");
    printf("║     • 天气警报控制                                       ║\n");
    printf("║                                                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    printf("\n  🚀  正在启动系统");
    for (int i = 0; i < 3; i++) {
        fflush(stdout);
        sleep(1);
        printf(".");
    }
    printf("\n\n  ✨  系统就绪！\n");
    sleep(1);
}

void show_menu() {
    clear_screen();
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║                                                          ║\n");
    printf("║           📷  智能摄像头控制系统  📷                     ║\n");
    printf("║                                                          ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║                                                          ║\n");
    printf("║   [1]  📸  拍摄照片 (YUV格式)                            ║\n");
    printf("║                                                          ║\n");
    printf("║   [2]  🔄  转换YUV为JPEG格式                             ║\n");
    printf("║                                                          ║\n");
    printf("║   [3]  🤖  AI助手问答                                    ║\n");
    printf("║                                                          ║\n");
    printf("║   [4]  🚨  LED天气警报控制                               ║\n");
    printf("║                                                          ║\n");
    printf("║   [0]  🚪  退出系统                                      ║\n");
    printf("║                                                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    printf("\n  👉  请选择操作 [0-4]: ");
}

void menu1_capture_photo() {
    clear_screen();
    print_header("📸  拍摄照片");
    print_loading("正在启动摄像头并拍摄照片");
    
    char command[256];
    snprintf(command, sizeof(command), "%s %s %s", CAMERA_CAPTURE, DEVICE, YUV_FILE);
    int result = system(command);
    
    print_done();
    print_separator();
    if (result == 0) {
        print_success("照片拍摄成功！");
        print_box("保存文件", YUV_FILE);
        print_info("您可以使用菜单2将照片转换为JPEG格式");
    } else {
        print_error("照片拍摄失败");
        print_warning("请检查摄像头是否正确连接 (/dev/video0)");
    }
    wait_and_return();
}

void menu2_convert_to_jpeg() {
    clear_screen();
    print_header("🔄  格式转换");
    print_loading("正在转换YUV为JPEG格式");
    
    char command[256];
    snprintf(command, sizeof(command), "%s %s %s", YUV_TO_JPEG, YUV_FILE, JPG_FILE);
    int result = system(command);
    
    print_done();
    print_separator();
    if (result == 0) {
        print_success("转换成功！");
        print_box("源文件", YUV_FILE);
        print_box("目标文件", JPG_FILE);
        print_info("您可以使用菜单3让AI分析这张照片");
    } else {
        print_error("转换失败");
        print_warning("请检查源文件 test.yuv 是否存在");
        print_info("提示：先使用菜单1拍摄照片");
    }
    wait_and_return();
}

void menu3_ai_assistant() {
    clear_screen();
    print_header("🤖  AI助手问答");
    
    char question[1024] = {0};
    char use_image[10] = {0};
    char command[2048] = {0};
    
    printf("\n  💭 请输入您的问题 (例如：今天天气如何？): ");
    fgets(question, sizeof(question), stdin);
    question[strcspn(question, "\n")] = 0;
    
    if (strlen(question) == 0) {
        strcpy(question, "今天天气如何");
        print_info("使用默认问题：今天天气如何");
    }
    
    printf("\n  🖼️  是否使用 %s 作为输入? (y/n): ", JPG_FILE);
    fgets(use_image, sizeof(use_image), stdin);
    
    memset(ai_output, 0, sizeof(ai_output));
    
    print_separator();
    print_loading("正在连接AI助手，请稍候");
    
    // 显示加载动画
    for (int i = 0; i < 3; i++) {
        fflush(stdout);
        usleep(300000);  // 300ms
        printf(".");
    }
    printf("\n");
    
    if (use_image[0] == 'y' || use_image[0] == 'Y') {
        snprintf(command, sizeof(command), "CAI_API_KEY=e25c2846-195e-40c8-b19c-84a388ad06e6 %s %s %s", AI_TEST, question, JPG_FILE);
    } else {
        snprintf(command, sizeof(command), "CAI_API_KEY=e25c2846-195e-40c8-b19c-84a388ad06e6 %s %s", AI_TEST, question);
    }
    
    FILE *pipe = popen(command, "r");
    if (!pipe) {
        print_error("AI助手调用失败");
        wait_and_return();
        return;
    }
    
    // 设置超时时间为30秒
    int timeout = 30;
    time_t start_time = time(NULL);
    
    // 实时输出AI响应
    char buffer[1024];
    printf("\n  🤖  AI助手回答:\n");
    printf("  ┌────────────────────────────────────────────────────────┐\n");
    
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        // 检查是否超时
        if (time(NULL) - start_time > timeout) {
            print_error("AI助手响应超时");
            pclose(pipe);
            wait_and_return();
            return;
        }
        
        // 实时输出AI响应
        char *line = strtok(buffer, "\n");
        while (line != NULL) {
            printf("  │ %-54s │\n", line);
            fflush(stdout);  // 立即刷新输出
            strcat(ai_output, line);
            strcat(ai_output, "\n");
            line = strtok(NULL, "\n");
        }
    }
    pclose(pipe);
    
    printf("  └────────────────────────────────────────────────────────┘\n");
    print_done();
    print_separator();
    
    // 保存AI输出到文件
    FILE *fp = fopen("ai_output.txt", "w");
    if (fp) {
        fprintf(fp, "%s", ai_output);
        fclose(fp);
        print_success("AI回答已保存到 ai_output.txt");
        print_info("您可以使用菜单4进行天气警报控制");
    } else {
        print_error("无法保存AI输出到文件");
    }
    wait_and_return();
}

void menu4_led_control() {
    clear_screen();
    print_header("🚨  天气警报控制");
    print_loading("正在分析天气数据");
    
    // 调用thunder程序读取文件并判断天气
    int result = system(THUNDER);
    if (result == -1) {
        print_error("无法执行天气分析程序");
        wait_and_return();
        return;
    }
    
    weather_result = WEXITSTATUS(result);
    
    print_done();
    print_separator();
    print_loading("正在控制LED警报灯");
    
    // 控制LED，传递thunder的结果
    char command[512];
    snprintf(command, sizeof(command), "sudo %s %d", LED_CONTROL, weather_result);
    system(command);
    
    print_done();
    if (weather_result == 1) {
        print_warning("检测到雷雨天气，警报灯已启动");
    } else {
        print_success("天气正常，警报灯保持关闭");
    }
    wait_and_return();
}

void show_goodbye() {
    clear_screen();
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║                                                          ║\n");
    printf("║                                                          ║\n");
    printf("║              感谢使用，再见！👋                          ║\n");
    printf("║                                                          ║\n");
    printf("║         期待您的再次使用                                 ║\n");
    printf("║                                                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

int main() {
    int choice;
    
    show_welcome();
    
    while (1) {
        show_menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            print_error("输入无效，请输入数字 0-4");
            sleep(1);
            continue;
        }
        while (getchar() != '\n');
        
        switch (choice) {
            case 1:
                menu1_capture_photo();
                break;
            case 2:
                menu2_convert_to_jpeg();
                break;
            case 3:
                menu3_ai_assistant();
                break;
            case 4:
                menu4_led_control();
                break;
            case 0:
                show_goodbye();
                return 0;
            default:
                print_error("无效选择，请输入 0-4 之间的数字");
                sleep(1);
                break;
        }
    }
    
    return 0;
}
