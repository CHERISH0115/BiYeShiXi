#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define AI_OUTPUT_FILE "ai_output.txt"

// 转换为小写
void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// 检查是否识别到雷雨天气 - 增强版算法
int check_thunderstorm(const char *output) {
    // 创建可修改的副本
    char *content = strdup(output);
    if (!content) return 0;
    
    // 转换为小写进行不区分大小写的匹配
    to_lowercase(content);
    
    // 中文关键字（精确匹配）
    const char *cn_keywords[] = {
        "雷雨", "雷暴", "暴雨", "闪电", "雷电", "打雷",
        "雷阵雨", "雷雨天气", "雷暴天气", "暴雨天气",
        "强对流天气", "雷击"
    };
    
    // 英文关键字
    const char *en_keywords[] = {
        "thunderstorm", "thunder", "lightning", "storm", 
        "heavy rain", "thunderstorms", "storms"
    };
    
    int cn_count = sizeof(cn_keywords) / sizeof(cn_keywords[0]);
    int en_count = sizeof(en_keywords) / sizeof(en_keywords[0]);
    
    // 检查中文关键字
    for (int i = 0; i < cn_count; i++) {
        if (strstr(output, cn_keywords[i]) != NULL) {
            free(content);
            return 1;
        }
    }
    
    // 检查英文关键字（小写匹配）
    for (int i = 0; i < en_count; i++) {
        if (strstr(content, en_keywords[i]) != NULL) {
            free(content);
            return 1;
        }
    }
    
    // 模糊匹配：检查是否包含"雷"和"雨"在较近距离内
    char *lei = strstr(content, "雷");
    if (lei) {
        // 在"雷"后面100个字符内查找"雨"
        char *yu = strstr(lei, "雨");
        if (yu && (yu - lei) < 100) {
            free(content);
            return 1;
        }
    }
    
    // 模糊匹配：检查是否包含"thunder"和"rain"
    char *thunder = strstr(content, "thunder");
    if (thunder) {
        char *rain = strstr(content, "rain");
        if (rain) {
            free(content);
            return 1;
        }
    }
    
    free(content);
    return 0;
}

void print_boxed_result(int result, const char *content) {
    printf("\n");
    printf("╔════════════════════════════════╗\n");
    printf("║      天气识别结果              ║\n");
    printf("╠════════════════════════════════╣\n");
    if (result == 1) {
        printf("║  结果: 1                       ║\n");
        printf("║  状态: ⚠️  识别到雷雨天气      ║\n");
        printf("║  建议: 请注意安全，携带雨具    ║\n");
    } else {
        printf("║  结果: 0                       ║\n");
        printf("║  状态: ✅ 未识别到雷雨天气     ║\n");
        printf("║  建议: 天气良好，适宜出行      ║\n");
    }
    printf("╚════════════════════════════════╝\n");
    printf("\n");
    
    // 调试信息：显示检测到的内容摘要
    printf("  📄 检测内容摘要:\n");
    printf("  ");
    int len = strlen(content);
    int show_len = len > 100 ? 100 : len;
    for (int i = 0; i < show_len; i++) {
        putchar(content[i]);
    }
    if (len > 100) {
        printf("...");
    }
    printf("\n\n");
}

int main() {
    FILE *fp = fopen(AI_OUTPUT_FILE, "r");
    if (!fp) {
        fprintf(stderr, "╔════════════════════════════════╗\n");
        fprintf(stderr, "║  错误: 无法打开文件 %-10s ║\n", AI_OUTPUT_FILE);
        fprintf(stderr, "╚════════════════════════════════╝\n");
        return -1;
    }
    
    char content[4096] = {0};
    size_t read_size = fread(content, 1, sizeof(content) - 1, fp);
    fclose(fp);
    
    if (read_size == 0) {
        fprintf(stderr, "╔════════════════════════════════╗\n");
        fprintf(stderr, "║  错误: 文件为空                ║\n");
        fprintf(stderr, "╚════════════════════════════════╝\n");
        return -1;
    }
    
    int result = check_thunderstorm(content);
    
    print_boxed_result(result, content);
    
    return result;
}
