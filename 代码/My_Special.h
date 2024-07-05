/* ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ 头文件 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#include "linux_kennel_list.h"

/* ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ 头文件 ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ */

/*------------------------------------------------------------------------------------------------------*/

/* ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ 宏函数 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */
#define LCD_DEV_PATH "/dev/fb0"         // 屏幕
#define DZK_FILE_PATH "./english.Dzk" // 字库
#define OPEN_FILE_FLAG O_RDWR // 可读可写
/*-------------------------*/

#define LCD_DEV_PATH "/dev/fb0"
#define PUT_PATH "/dev/input/event0"
#define UI_BMP "./UI/"       // UI路径
#define PHOTO_BMP "./PHOTO/" // 图片路径
#define BMP ".bmp"
#define BMP_PATH_LEN 300
#define BMP_NAME_LEN 50
#define MOD_ONE 1 // UI
#define MOD_TWO 2 // photo
#define LCD_W 800
#define LCD_H 480
#define INPUT_W 1024 // 触摸屏x轴
#define INPUT_H 600  // 触摸屏y轴
#define MUL_YT 1     // 不缩小
#define MUL_SX 4     // 缩小四倍

#define FULL_w 400
#define FULL_H 240

#define LOC_1_w 110
#define LOC_1_h 80
#define LOC_2_w 330
#define LOC_2_h 80
#define LOC_3_w 550
#define LOC_3_h 80

#define LOC_4_w 110
#define LOC_4_h 240
#define LOC_5_w 330
#define LOC_5_h 240
#define LOC_6_w 550
#define LOC_6_h 240

#define LOC_7_w 110
#define LOC_7_h 400
#define LOC_8_w 330
#define LOC_8_h 400
#define LOC_9_w 550
#define LOC_9_h 400

#define BUTTON_x 730
#define BUTTON_1 60
#define BUTTON_2 180
#define BUTTON_3 300
#define BUTTON_4 420

/* ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ 宏函数 ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ */

/*------------------------------------------------------------------------------------------------------*/

/* ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ 结构体 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

typedef struct OPEN_DEV // 设备结构体
{
    int lcd_fd;   // dev的文件描述符
    int *mmap_p;  // 映射空间的文件描述符
    int input_ts; // 触摸屏的文件描述符
} DEV, *DEV_P;

typedef struct ui // UI结构体，存放UI图片
{
    // 数据域
    char name[BMP_NAME_LEN]; // 存放UI图片名字
    char ui[BMP_PATH_LEN];   // 存放UI图片路径

    // 指针域
    struct list_head ui_list; ////UI的内核链表

} UI, *UI_LINK;

typedef struct photo
{
    // 数据域
    char name[BMP_NAME_LEN]; // 存放photo图片名字
    char data[BMP_PATH_LEN]; // 存放photo图片路径
    int nub;                 // 存放标志图片路径

    // 指针域

    struct list_head photo_list; // photo的内核链表

} PHOTO, *PHOTO_LINK;

typedef struct X_Y
{
    int x;
    int y;
} XY, *XY_P;

/*-----------------*/
struct inf
{
    int lcd_fd, dzk_fd; // 文件描述符
    int dzk_size;       // 文件大小
    int *lcd_star;
    int dzk_offset;
    int color;      // 颜色
    char *dzk_star; // 字库指针
} I;

/* ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ 结构体 ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ */

/*------------------------------------------------------------------------------------------------------*/

/* ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ 函数申明 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

DEV_P Project_Initialize();                                                             // 项目初始化函数
int Brushing(DEV_P DEV_pointer, char path[], int pos_w, int pos_h, int mul);            // 刷图函数
int Search_File(const char *obj_dir, char *new_file, int mask, void *head_node, int i); // 图片的路径
void *Create_New_Node(int mask);                                                        // 创建新节点
int Insert_Link_Node(void *new_node, int mask, void *head_node);                        // 链接节点
void *Click(double x1, double y1, double x2, double y2, DEV_P DEV_pointer, int mask);   // 点击函数
void MY_UI(DEV_P DEV_pointer, UI_LINK head_node);                                       // UI函数
void Function(DEV_P DEV_pointer, UI_LINK ui_head_node, PHOTO_LINK photo_head_node);     // 功能函数
void Choose(DEV_P DEV_pointer, PHOTO_LINK head_node, UI_LINK ui_head);                  // 选择
PHOTO_LINK Page(PHOTO_LINK head_node, int page);                                        // 页数
void Photo_Album(DEV_P DEV_pointer, PHOTO_LINK head_node, UI_LINK ui_head);             // 相册功能

/*---------------------*/
int Init();
int Free();
int Dis_Fontl(const char *fontl, int where_x, int where_y, int color);
int Draw_Point(int where_x, int where_y, int color); // 指定任意位置刷一个像素点
int Draw_Chinese(const char *fontls, int where_x, int where_y, int color);
int display(PHOTO_LINK head_node,DEV_P DEV_pointer,UI_LINK ui_head);



/* ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ 函数申明 ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ */

/*------------------------------------------------------------------------------------------------------*/
