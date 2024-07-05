#include "My_Special.h"

// 创建新节点
void *Create_New_Node(int mask)
{
    if (mask == MOD_ONE)
    {
        UI_LINK new_node = (UI_LINK)malloc(sizeof(UI));
        if (new_node == (UI_LINK)NULL)
        {
            perror("new_node fail:\n");
            return (UI_LINK)-1;
        }

        // 清空申请的结构体堆空间
        memset(new_node, 0, sizeof(UI));

        // 游离节点
        INIT_LIST_HEAD(&new_node->ui_list);

        return (void *)new_node;
    }

    if (mask == MOD_TWO)
    {
        PHOTO_LINK new_node = (PHOTO_LINK)malloc(sizeof(PHOTO));
        if (new_node == (PHOTO_LINK)NULL)
        {
            perror("new_node fail:\n");
            return (PHOTO_LINK)-1;
        }

        // 清空申请的结构体堆空间
        memset(new_node, 0, sizeof(PHOTO));

        // 游离节点
        INIT_LIST_HEAD(&new_node->photo_list);

        return (void *)new_node;
    }
}

// 链接节点
int Insert_Link_Node(void *new_node, int mask, void *head_node)
{
    if (new_node == NULL)
    {
        perror("传过来的节点有问题：\n");
        return -1;
    }

    if (mask == MOD_ONE)
    {
        list_add_tail(&((UI_LINK)new_node)->ui_list, &((UI_LINK)head_node)->ui_list);
    }
    if (mask == MOD_TWO)
    {
        list_add_tail(&((PHOTO_LINK)new_node)->photo_list, &((PHOTO_LINK)head_node)->photo_list);
    }
    return 0;
}

// 获取大节点函数
void *Get_Node(void *head, char data_name[BMP_NAME_LEN], int mask)
{

    if (mask == MOD_ONE)
    {
        UI_LINK pos = (UI_LINK)NULL;
        UI_LINK head_node = (UI_LINK)head;
        list_for_each_entry(pos, &head_node->ui_list, ui_list)
        {
            if (strcmp(data_name, pos->name) == 0)
            {
                printf("hit data:%s\n", pos->name);
                return (void *)pos;
            }
        }
    }
    if (mask == MOD_TWO)
    {
        PHOTO_LINK pos = (PHOTO_LINK)NULL;
        PHOTO_LINK head_node = (PHOTO_LINK)head;
        list_for_each_entry(pos, &head_node->photo_list, photo_list)
        {
            if (strcmp(data_name, pos->name) == 0)
            {
                printf("hit data:%s\n", pos->name);
                return (void *)pos;
            }
        }
    }
    return (void *)-1;
}

// 点击函数
void *Click(double x1, double y1, double x2, double y2, DEV_P DEV_pointer, int mask)
{
    // 触摸屏像素点位置转触摸点位置
    int init_x = (x1 / LCD_W) * INPUT_W;
    int init_y = (y1 / LCD_H) * INPUT_H;
    int end_x = (x2 / LCD_W) * INPUT_W;
    int end_y = (y2 / LCD_H) * INPUT_H;

    struct input_event touch;
    int touch_x = -1, touch_y = -1;
    while (1)
    {
        if (read(DEV_pointer->input_ts, &touch, sizeof(touch)) == -1)
        {
            perror("read input_ts fail");
            return (void *)-1;
        }

        if (touch.type == EV_ABS && touch.code == ABS_X)
            touch_x = touch.value;
        if (touch.type == EV_ABS && touch.code == ABS_Y)
            touch_y = touch.value;
        if (touch.type == EV_KEY && touch.code == BTN_TOUCH && touch.value == 0)
        {
            printf("%d<%d<%d-------%d<%d<%d\n", init_x, touch_x, end_x, init_y, touch_y, end_y);
            if (mask == MOD_ONE)
            {
                if (init_x < touch_x && touch_x < end_x && init_y < touch_y && touch_y < end_y)
                {
                    return (void *)true;
                }
            }

            if (mask == MOD_TWO)
            {
                XY_P input_xy = (XY_P)malloc(sizeof(XY));
                input_xy->x = touch_x * ((double)LCD_W / INPUT_W);
                input_xy->y = touch_y * ((double)LCD_H / INPUT_H);

                return (void *)input_xy;
            }
        }
    }

    return (void *)false;
}

// 滑动
bool slide(double x1, double y1, double x2, double y2, DEV_P DEV_pointer, int mask, UI_LINK head_node)
{
    // 触摸屏像素点位置转触摸点位置
    int init_x = (x1 / LCD_W) * INPUT_W;
    int init_y = (y1 / LCD_H) * INPUT_H;
    int end_x = (x2 / LCD_W) * INPUT_W;
    int end_y = (y2 / LCD_H) * INPUT_H;

    struct input_event touch;
    int touch_x = -1, touch_y = -1;
    int backup_x = 1024;
    int backup = 0;

    UI_LINK kungo = (UI_LINK)Get_Node(head_node, "4.bmp", MOD_ONE);
    UI_LINK cat = (UI_LINK)Get_Node(head_node, "cat.bmp", MOD_ONE);

    while (1)
    {
        if (read(DEV_pointer->input_ts, &touch, sizeof(touch)) == -1)
        {
            perror("read input_ts fail");
            return (void *)-1;
        }

        if (touch.type == EV_ABS && touch.code == ABS_X)
            touch_x = touch.value;
        if (touch.type == EV_ABS && touch.code == ABS_Y)
            touch_y = touch.value;
        if (backup_x == 1024)
        {
            backup_x = touch_x;
        }

        backup = touch_x * 800 / 1024;

        if (mask == MOD_TWO && 150 < backup && backup < 650)
        {
            // Brushing(DEV_pointer, kungo->ui, FULL_w, FULL_H, MUL_YT);
            if (backup % 2 == 0)
            {
                Brushing(DEV_pointer, cat->ui, backup, 400, MUL_YT);
            }
        }

        if (touch.type == EV_KEY && touch.code == BTN_TOUCH && touch.value == 0)
        {
            printf("%d<%d<%d-------%d<%d<%d\n", init_x, touch_x, end_x, init_y, touch_y, end_y);
            if (mask == MOD_ONE)
            {
                if (init_x < touch_x && touch_x < end_x && init_y < touch_y && touch_y < end_y)
                {
                    return true;
                }
            }

            if (mask == MOD_TWO)
            {
                if (600 * 1024 / 800 < touch_x && touch_x < 800 * 1024 / 800)
                {
                    return true;
                }
            }
        }
    }

    return (void *)false;
}

// 找保存文件路径
int Search_File(const char *obj_dir, char *new_file, int mask, void *head_node, int i)
{
    DIR *dp = opendir(obj_dir);
    if (dp == (DIR *)NULL)
    {
        perror("opendir:");
        return -1;
    }

    char complete_path[BMP_PATH_LEN + 1] = "\0";

    while (1)
    {
        memset(complete_path, 0, BMP_PATH_LEN);
        struct dirent *eq = readdir(dp);
        if (eq == (struct dirent *)NULL)
        {
            break;
        }

        if (eq->d_name[0] == '.')
        {
            continue;
        }

        if (obj_dir[strlen(obj_dir) - 1] == '/')
        {
            sprintf(complete_path, "%s%s", obj_dir, eq->d_name);
        }
        else
        {
            sprintf(complete_path, "%s/%s", obj_dir, eq->d_name);
        }

        if (eq->d_type == DT_DIR) // 是目录
        {
            Search_File(complete_path, BMP, mask, head_node, i);
        }

        if (eq->d_type == DT_REG) // 是文件
        {

            if (strcmp(&eq->d_name[strlen(eq->d_name) - 4], BMP) == 0)
            {

                if (mask == MOD_ONE)
                {
                    UI_LINK new_node = (UI_LINK)Create_New_Node(MOD_ONE);
                    strcpy(new_node->ui, complete_path); // 保存UI的图片的路径
                    strcpy(new_node->name, eq->d_name);
                    Insert_Link_Node((void *)new_node, MOD_ONE, head_node);
                }
                if (mask == MOD_TWO)
                {
                    PHOTO_LINK new_node = (PHOTO_LINK)Create_New_Node(MOD_ONE);
                    strcpy(new_node->data, complete_path); // 保存photo的图片的路径
                    strcpy(new_node->name, eq->d_name);
                    new_node->nub = i;
                    i = i + 1;
                    Insert_Link_Node((void *)new_node, MOD_TWO, head_node);
                }
            }
        }
    }

    return 0;
}

// 项目初始化函数
DEV_P Project_Initialize(UI_LINK head_node)
{
    DEV_P DEV_pointer = (DEV_P)malloc(sizeof(DEV));
    if (DEV_pointer == (DEV_P)NULL)
    {
        perror("DEV_pointer malloc fail:\n");
        return (DEV_P)-1;
    }

    int lcd_fd = open(LCD_DEV_PATH, O_RDWR); // 打开设备
    int input_ts = open(PUT_PATH, O_RDONLY);
    if (lcd_fd == -1 || input_ts == -1)
    {
        perror("open dev fail:\n");
        return (DEV_P)-1;
    }

    int *mamp_p = (int *)mmap(NULL, LCD_W * LCD_H * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if (mamp_p == (int *)MAP_FAILED)
    {
        perror("mmap fail:\n");
        return (DEV_P)-1;
    }

    DEV_pointer->lcd_fd = lcd_fd;
    DEV_pointer->input_ts = input_ts;
    DEV_pointer->mmap_p = mamp_p;

    return DEV_pointer;
}

// 刷图函数
int Brushing(DEV_P DEV_pointer, char *bmp_path, int pos_w, int pos_h, int mul)
{

    int bmp_fd = open(bmp_path, O_RDONLY); // 打开传入的图片
    if (bmp_fd == -1)
    {
        printf("%s\n", bmp_path);
        perror("open fail:\n");
        return -1;
    }

    int w, h; // 图片的宽和高
    lseek(bmp_fd, 18, SEEK_SET);
    read(bmp_fd, &w, sizeof(int));
    read(bmp_fd, &h, sizeof(int));

    int i = (4 - (w / mul * 3) % 4) % 4; // 不能被4除的图片会补多少

    int *new_mmap_p = (DEV_pointer->mmap_p) + pos_w - ((w / mul) / 2) + LCD_W * (pos_h - ((h / mul) / 2));

    char rgb[w * h * 3 + i * h];
    lseek(bmp_fd, 54, SEEK_SET);
    if (read(bmp_fd, rgb, w * h * 3 + i * h) == -1)
    {
        perror("read fail:\n");
        return -1;
    }

    for (int y = 0, n = 0; y < h / mul; y++)
    {
        for (int x = 0; x < w / mul; x++, n += 3)
        {
            *(new_mmap_p + x + ((h / mul) - 1 - y) * LCD_W) = rgb[n] << 0 | rgb[n + 1] << 8 | rgb[n + 2] << 16;

            n += (mul - 1) * 3;
        }
        n += i / mul;
        n += (mul - 1) * w * 3;
    }

    return 0;
}

// UI函数
void MY_UI(DEV_P DEV_pointer, UI_LINK head_node)
{
    Brushing(DEV_pointer, ((UI_LINK)Get_Node(head_node, "1.bmp", MOD_ONE))->ui, FULL_w, FULL_H, MUL_YT);

    if ((bool)Click(572, 410, 722, 450, DEV_pointer, MOD_ONE) == true)
    {
        Brushing(DEV_pointer, ((UI_LINK)Get_Node(head_node, "2.bmp", MOD_ONE))->ui, FULL_w, FULL_H, MUL_YT);
        sleep(1);
        Brushing(DEV_pointer, ((UI_LINK)Get_Node(head_node, "3.bmp", MOD_ONE))->ui, FULL_w, FULL_H, MUL_YT);
        sleep(1);
        Brushing(DEV_pointer, ((UI_LINK)Get_Node(head_node, "4.bmp", MOD_ONE))->ui, FULL_w, FULL_H, MUL_YT);
        // 滑动
        while (slide(20, 320, 780, 480, DEV_pointer, MOD_TWO, head_node) != true)
            break;
        Brushing(DEV_pointer, ((UI_LINK)Get_Node(head_node, "5.bmp", MOD_ONE))->ui, FULL_w, FULL_H, MUL_YT);
        sleep(3);
        Brushing(DEV_pointer, ((UI_LINK)Get_Node(head_node, "6.bmp", MOD_ONE))->ui, FULL_w, FULL_H, MUL_YT);
        
    }

    // Brushing(DEV_pointer, ((UI_LINK)Get_Node(head_node, "xz.bmp", MOD_ONE))->ui, 730, 240, MUL_YT);

    return;
}

// 选择函数
void Choose(DEV_P DEV_pointer, PHOTO_LINK head_node, UI_LINK ui_head)
{
    int choose_mask = 0;
    while (1)
    {
        Brushing(DEV_pointer, ((UI_LINK)Get_Node(ui_head, "xz.bmp", MOD_ONE))->ui, 730, 240, MUL_YT);
        choose_mask = 0;
        XY_P xy = (XY_P)Click(0, 0, 800, 480, DEV_pointer, MOD_TWO);
        if (661 < xy->x && xy->x < 800 && 0 < xy->y && xy->y < 120)
        {
            choose_mask = 1;
        }
        else if (661 < (xy->x) && (xy->x) < 800 && 121 < (xy->y) && (xy->y) < 240)
        {
            choose_mask = 2;
        }
        else if (661 < (xy->x) && (xy->x) < 800 && 241 < (xy->y) && (xy->y) < 360)
        {
            choose_mask = 3;
        }
        else if (661 < (xy->x) && (xy->x) < 800 && 361 < (xy->y) && (xy->y) < 480)
        {
            choose_mask = 4;
        }
        else
        {
            continue;
        }

        free(xy);

        switch (choose_mask)
        {
        case 1:
            Photo_Album(DEV_pointer, head_node, ui_head);
            break;
        case 2:
            printf("wei kai fa\n");
            break;
        case 3:
            printf("wei kai fa\n");
            break;
        case 4:
            choose_mask = 0;
            break;
        default:
            break;
        }
        if (choose_mask == 0)
        {
            Brushing(DEV_pointer, ((UI_LINK)Get_Node(ui_head, "gj.bmp", MOD_ONE))->ui, FULL_w, FULL_H, MUL_YT);
            sleep(1);
            Brushing(DEV_pointer, ((UI_LINK)Get_Node(ui_head, "over.bmp", MOD_ONE))->ui, FULL_w, FULL_H, MUL_YT);
            usleep(100000);
            Brushing(DEV_pointer, ((UI_LINK)Get_Node(ui_head, "black.bmp", MOD_ONE))->ui, FULL_w, FULL_H, MUL_YT);
            break;
        }
    }

    return;
}

// 页数
PHOTO_LINK Page(PHOTO_LINK head_node, int page)
{
    int page_nub = 0;
    for (PHOTO_LINK temp = list_entry(head_node->photo_list.next, PHOTO, photo_list); temp != head_node; temp = list_entry(temp->photo_list.next, PHOTO, photo_list))
    {
        (temp->nub) / 9 == page ? (page_nub = (temp->nub) / 9) : (page_nub = (temp->nub) / 9 + 1);
        if ((temp->nub) % 9 == 1 && page_nub == page)
        {
            return temp;
        }
    }

    return (PHOTO_LINK)0;
}

// 双击退出
bool Double_click(DEV_P DEV_pointer)
{
    bool Once = (bool)Click(0, 0, 800, 480, DEV_pointer, MOD_ONE);
    bool Two = (bool)Click(0, 0, 800, 480, DEV_pointer, MOD_ONE);

    if (Once == true && Two == true)
    {
        return true;
    }
    return false;
}
/*----------------------------------------------------------------------------------------------------------------------------------------*/
int Init()
{
    // 打开LCD 汉字库
    I.lcd_fd = open(LCD_DEV_PATH, OPEN_FILE_FLAG);
    I.dzk_fd = open(DZK_FILE_PATH, OPEN_FILE_FLAG);
    if (I.lcd_fd == -1 || I.dzk_fd == -1)
    {
        perror("open file ");
        return -1;
    }

    I.dzk_size = lseek(I.dzk_fd, 0, SEEK_END); // 求出汉字库的大小，才能根据他的大小进行申请映射空间
    lseek(I.dzk_fd, 0, SEEK_SET);              // 重置偏移量
    // 映射lcd和dzk
    I.lcd_star = mmap(NULL, LCD_W * LCD_H * 4, PROT_READ | PROT_WRITE, MAP_SHARED, I.lcd_fd, 0);
    I.dzk_star = mmap(NULL, I.dzk_size, PROT_READ | PROT_WRITE, MAP_SHARED, I.dzk_fd, 0);
    if (I.lcd_star == MAP_FAILED || I.dzk_star == MAP_FAILED)
    {
        perror("mmap lcd ... mmap dzk ");
        return -1;
    }

    return 0;
}

int Free()
{
    close(I.lcd_fd);
    close(I.dzk_fd);
    munmap(I.lcd_star, LCD_W * LCD_H * 4);
    munmap(I.dzk_star, I.dzk_size);

    return 0;
}

int Dis_Fontl(const char *fontl, int where_x, int where_y, int color) // fontl存放对应要显示的汉字
{
    // 先判断指定的坐标是否可用
    if (where_x + 16 > 799) // 判断x
    {
        // 回到行首
        where_x = 0;
        if (where_y + 16 > 479)
        {
            printf("指定的坐标有误，无法显示！\n");
            return -1;
        }
        else
        {
            where_y += 16;
        }
    }

    // 先让lcd的映射指针跳到对应要显示的左上角的坐标位置
    // int * new_lcd_p = I.lcd_star+(800*where_y)+where_x;

    // I.dzk_offset = (94 * (fontl[0] - 1 - 0xa0) + (fontl[1] - 1 - 0xa0)) * 32;

    I.dzk_offset = *fontl * 32;                  //(int)(*fontl);
    char *new_dzk_p = I.dzk_star + I.dzk_offset; // 定义一个新的指针变量存放dzk字库的映射指针
    // 求出要显示的汉字在字库中的偏移量
    // 根据偏移量把映射指针跳到要显示的汉字对应的位置
    int x, y, z;
    char type; // 定义一个字符变量存放每次判断的字符汉字库数据

    for (y = 0; y < 16; y++)
    {
        for (x = 0; x < 2; x++)
        {
            type = *(new_dzk_p + 2 * y + x); // 获取字库中一个字节的汉字模
            for (z = 0; z < 8; z++)
            {

                if (type & (0x80 >> z))
                {
                    //*(new_lcd_p+800*y+(8*x+z)) = 0x0000ff00;
                    Draw_Point(where_x + (8 * x + z), (where_y + y), color);
                }
                else
                {
                    Draw_Point(where_x + (8 * x + z), (where_y + y), 0x00FFECF5);
                }
            }
        }
    }
    return 0;
}

int Draw_Point(int where_x, int where_y, int color)
{
    if (where_x > 799 || where_y > 479)
    {
        printf("指定的坐标有误，无法显示！\n");
        return -1;
    }

    *(I.lcd_star + 800 * where_y + where_x) = color;

    return 0;
}

int Draw_Chinese(const char *fontls, int where_x, int where_y, int color)
{
    // 循环截取每一个汉字
    const char *new_f = fontls;
    int n;
    for (n = 0; n < (strlen(fontls) / 1); n++)
    {

        Dis_Fontl(new_f, where_x, where_y, color);
        new_f += 1;
        where_x += 11;
    }

    return 0;
}

int display(PHOTO_LINK head_node, DEV_P DEV_pointer, UI_LINK ui_head)
{
    Init();

    //  Dis_Fontl("啊",400,240);//这里的啊要和字库的中文编码格式要一致（GBK）
    Draw_Chinese("ID", 30, 15, 0x00FF44FF);
    Draw_Chinese("Name", 80, 15, 0x00FF60AF);
    Draw_Chinese("Picture Path", 380, 15, 0x007D7DFF);

    int ID_y = 20, name_y = 20, path_y = 20;
    int i = 1;

    for (PHOTO_LINK temp_node = list_entry(head_node->photo_list.next, PHOTO, photo_list); temp_node != head_node; temp_node = list_entry(temp_node->photo_list.next, PHOTO, photo_list))
    {
        if (i == 22)
        {
            while (1)
            {
                ID_y = 20;
                name_y = 20;
                path_y = 20;

                if ((bool)Click(0, 0, 800, 450, DEV_pointer, MOD_ONE) == true)
                {
                    Brushing(DEV_pointer, ((UI_LINK)Get_Node(ui_head, "02.bmp", MOD_ONE))->ui, FULL_w, FULL_H, MUL_YT);
                    Draw_Chinese("ID", 30, 15, 0x00FF44FF);
                    Draw_Chinese("Name", 80, 15, 0x00FF60AF);
                    Draw_Chinese("Picture Path", 380, 15, 0x007D7DFF);
                    break;
                }
            }
        }

        ID_y += 20;
        name_y += 20;
        path_y += 20;
        char nub[10];
        snprintf(nub, sizeof(nub), "%d", temp_node->nub);

        Draw_Chinese(nub, 30, ID_y, 0x00FF44FF);
        Draw_Chinese(temp_node->name, 80, name_y, 0x00FF60AF);
        Draw_Chinese(temp_node->data, 380, path_y, 0x007D7DFF);

        i++;
    }

    Free();

    return 0;
}
/*-----------------------------------------------------------------------------------------------------------------*/

// 相册功能
void Photo_Album(DEV_P DEV_pointer, PHOTO_LINK head_node, UI_LINK ui_head)
{
    int i = 1;
    int page = 1;
    // PHOTO_LINK backup_node = Page(head_node, page);
    PHOTO_LINK temp_node = (PHOTO_LINK)NULL;
    PHOTO_LINK temp_1 = (PHOTO_LINK)NULL;
    PHOTO_LINK temp_2 = (PHOTO_LINK)NULL;
    PHOTO_LINK temp_3 = (PHOTO_LINK)NULL;
    PHOTO_LINK temp_4 = (PHOTO_LINK)NULL;
    PHOTO_LINK temp_5 = (PHOTO_LINK)NULL;
    PHOTO_LINK temp_6 = (PHOTO_LINK)NULL;
    PHOTO_LINK temp_7 = (PHOTO_LINK)NULL;
    PHOTO_LINK temp_8 = (PHOTO_LINK)NULL;
    PHOTO_LINK temp_9 = (PHOTO_LINK)NULL;
    while (1)
    {
        PHOTO_LINK backup_node = Page(head_node, page);
        Brushing(DEV_pointer, ((UI_LINK)Get_Node(ui_head, "xcui.bmp", MOD_ONE))->ui, FULL_w, FULL_H, MUL_YT);
        if (page == 0)
        {
            break;
        }

        // PHOTO_LINK backup_node = Page(head_node, page);
        // PHOTO_LINK temp_node = (PHOTO_LINK)NULL;
        // PHOTO_LINK temp_1 = (PHOTO_LINK)NULL;
        // PHOTO_LINK temp_2 = (PHOTO_LINK)NULL;
        // PHOTO_LINK temp_3 = (PHOTO_LINK)NULL;
        // PHOTO_LINK temp_4 = (PHOTO_LINK)NULL;
        // PHOTO_LINK temp_5 = (PHOTO_LINK)NULL;
        // PHOTO_LINK temp_6 = (PHOTO_LINK)NULL;
        // PHOTO_LINK temp_7 = (PHOTO_LINK)NULL;
        // PHOTO_LINK temp_8 = (PHOTO_LINK)NULL;
        // PHOTO_LINK temp_9 = (PHOTO_LINK)NULL;
        for (temp_node = backup_node; temp_node != head_node && i <= 9; temp_node = list_entry(temp_node->photo_list.next, PHOTO, photo_list))
        {
            if ((temp_node->nub) % 9 == 1 && i == 1)
            {
                temp_1 = temp_node;
                Brushing(DEV_pointer, temp_node->data, LOC_1_w, LOC_1_h, MUL_SX);
                printf("%d\n", temp_node->nub);
            }
            else if ((temp_node->nub) % 9 == 2 && i == 2)
            {
                temp_2 = temp_node;
                Brushing(DEV_pointer, temp_node->data, LOC_2_w, LOC_2_h, MUL_SX);
            }
            else if ((temp_node->nub) % 9 == 3 && i == 3)
            {
                temp_3 = temp_node;
                Brushing(DEV_pointer, temp_node->data, LOC_3_w, LOC_3_h, MUL_SX);
            }
            else if ((temp_node->nub) % 9 == 4 && i == 4)
            {
                temp_4 = temp_node;
                Brushing(DEV_pointer, temp_node->data, LOC_4_w, LOC_4_h, MUL_SX);
            }
            else if ((temp_node->nub) % 9 == 5 && i == 5)
            {
                temp_5 = temp_node;
                Brushing(DEV_pointer, temp_node->data, LOC_5_w, LOC_5_h, MUL_SX);
            }
            else if ((temp_node->nub) % 9 == 6 && i == 6)
            {
                temp_6 = temp_node;
                Brushing(DEV_pointer, temp_node->data, LOC_6_w, LOC_6_h, MUL_SX);
            }
            else if ((temp_node->nub) % 9 == 7 && i == 7)
            {
                temp_7 = temp_node;
                Brushing(DEV_pointer, temp_node->data, LOC_7_w, LOC_7_h, MUL_SX);
            }
            else if ((temp_node->nub) % 9 == 8 && i == 8)
            {
                temp_8 = temp_node;
                Brushing(DEV_pointer, temp_node->data, LOC_8_w, LOC_8_h, MUL_SX);
            }
            else if ((temp_node->nub) % 9 == 0 && i == 9)
            {
                temp_9 = temp_node;
                Brushing(DEV_pointer, temp_node->data, LOC_9_w, LOC_9_h, MUL_SX);
            }

            i += 1;
        }

        if (i == 10 || temp_node == head_node) // list_entry(head_node->photo_list.prev, PHOTO, photo_list)
        {
            while (1)
            {
                XY_P xy = (XY_P)Click(0, 0, 800, 480, DEV_pointer, MOD_TWO);
                Brushing(DEV_pointer, ((UI_LINK)Get_Node(ui_head, "bj.bmp", MOD_ONE))->ui, FULL_w, FULL_H, MUL_YT);
                if (661 < (xy->x) && (xy->x) < 800 && 0 < (xy->y) && (xy->y) < 120) // 列表
                {
                    i = 1;
                    Brushing(DEV_pointer, ((UI_LINK)Get_Node(ui_head, "02.bmp", MOD_ONE))->ui, FULL_w, FULL_H, MUL_YT);
                    printf("列表\n");
                    display(head_node, DEV_pointer, ui_head);
                    if (Double_click(DEV_pointer) == true)
                    {
                        free(xy);
                        break;;
                    }
                }
                else if (661 < (xy->x) && (xy->x) < 800 && 121 < (xy->y) && (xy->y) < 240) // 上一页
                {
                    i = 1;
                    if (page != 1)
                    {
                        page -= 1;
                    }
                    free(xy);
                    break;
                }
                else if (661 < (xy->x) && (xy->x) < 800 && 241 < (xy->y) && (xy->y) < 360) // 下一页
                {
                    i = 1;
                    if (page != ((list_entry(head_node->photo_list.prev, PHOTO, photo_list)->nub) / 9) + 1)
                    {
                        page += 1;
                    }
                    else
                    {
                        page = 1;
                    }
                    free(xy);
                    break;
                }
                else if (661 < (xy->x) && (xy->x) < 800 && 361 < (xy->y) && (xy->y) < 480) // 退出
                {
                    page = 0;
                    free(xy);
                    break;
                }
                else if (0 < (xy->x) && (xy->x) < 220 && 0 < (xy->y) && (xy->y) < 160 && temp_1 != (PHOTO_LINK)NULL) // 打开第1张图
                {
                    Brushing(DEV_pointer, temp_1->data, FULL_w, FULL_H, MUL_YT);
                    if (Double_click(DEV_pointer) == true)
                    {
                        i = 1;
                        free(xy);
                        break;
                    }
                }
                else if (221 < (xy->x) && (xy->x) < 440 && 0 < (xy->y) && (xy->y) < 160 && temp_2 != (PHOTO_LINK)NULL) // 打开第2张图
                {
                    Brushing(DEV_pointer, temp_2->data, FULL_w, FULL_H, MUL_YT);
                    if (Double_click(DEV_pointer) == true)
                    {
                        i = 1;
                        free(xy);
                        break;
                    }
                }
                else if (441 < (xy->x) && (xy->x) < 660 && 0 < (xy->y) && (xy->y) < 160 && temp_3 != (PHOTO_LINK)NULL) // 打开第3张图
                {
                    Brushing(DEV_pointer, temp_3->data, FULL_w, FULL_H, MUL_YT);
                    if (Double_click(DEV_pointer) == true)
                    {
                        i = 1;
                        free(xy);
                        break;
                    }
                }
                else if (0 < (xy->x) && (xy->x) < 220 && 161 < (xy->y) && (xy->y) < 320 && temp_4 != (PHOTO_LINK)NULL) // 打开第4张图
                {
                    Brushing(DEV_pointer, temp_4->data, FULL_w, FULL_H, MUL_YT);
                    if (Double_click(DEV_pointer) == true)
                    {
                        i = 1;
                        free(xy);
                        break;
                    }
                }
                else if (221 < (xy->x) && (xy->x) < 440 && 161 < (xy->y) && (xy->y) < 320 && temp_5 != (PHOTO_LINK)NULL) // 打开第5张图
                {
                    Brushing(DEV_pointer, temp_5->data, FULL_w, FULL_H, MUL_YT);
                    if (Double_click(DEV_pointer) == true)
                    {
                        i = 1;
                        free(xy);
                        break;
                    }
                }
                else if (441 < (xy->x) && (xy->x) < 660 && 161 < (xy->y) && (xy->y) < 320 && temp_6 != (PHOTO_LINK)NULL) // 打开第6张图
                {
                    Brushing(DEV_pointer, temp_6->data, FULL_w, FULL_H, MUL_YT);
                    if (Double_click(DEV_pointer) == true)
                    {
                        i = 1;
                        free(xy);
                        break;
                    }
                }
                else if (0 < (xy->x) && (xy->x) < 220 && 321 < (xy->y) && (xy->y) < 480 && temp_7 != (PHOTO_LINK)NULL) // 打开第7张图
                {
                    Brushing(DEV_pointer, temp_7->data, FULL_w, FULL_H, MUL_YT);
                    if (Double_click(DEV_pointer) == true)
                    {
                        i = 1;
                        free(xy);
                        break;
                    }
                }
                else if (221 < (xy->x) && (xy->x) < 440 && 321 < (xy->y) && (xy->y) < 480 && temp_8 != (PHOTO_LINK)NULL) // 打开第8张图
                {
                    Brushing(DEV_pointer, temp_8->data, FULL_w, FULL_H, MUL_YT);
                    if (Double_click(DEV_pointer) == true)
                    {
                        i = 1;
                        free(xy);
                        break;
                    }
                }
                else if (441 < (xy->x) && (xy->x) < 660 && 321 < (xy->y) && (xy->y) < 480 && temp_9 != (PHOTO_LINK)NULL) // 打开第9张图
                {
                    Brushing(DEV_pointer, temp_9->data, FULL_w, FULL_H, MUL_YT);
                    if (Double_click(DEV_pointer) == true)
                    {
                        i = 1;
                        free(xy);
                        break;
                    }
                }
                else
                {
                    free(xy);
                    continue;
                }

                //free(xy);
            }
        }
    }

    return;
}

// 功能函数
void Function(DEV_P DEV_pointer, UI_LINK ui_head_node, PHOTO_LINK photo_head_node)
{
    MY_UI(DEV_pointer, ui_head_node);
    Choose(DEV_pointer, photo_head_node, ui_head_node);

    return;
}

// 释放
void FREE(DEV_P DEV_pointer, UI_LINK ui_head_node, PHOTO_LINK photo_head_node)
{
    close(DEV_pointer->input_ts);
    close(DEV_pointer->lcd_fd);
    munmap(DEV_pointer->mmap_p, LCD_W * LCD_H * 4);

    free(DEV_pointer);

    while (!list_empty(&ui_head_node->ui_list))
    {

        UI_LINK temp_node = list_entry(ui_head_node->ui_list.next, UI, ui_list);

        list_del(&(temp_node->ui_list));

        free(temp_node);
    }

    while (!list_empty(&photo_head_node->photo_list))
    {

        PHOTO_LINK temp = list_entry(photo_head_node->photo_list.next, PHOTO, photo_list);
        list_del(&(temp->photo_list));

        free(temp);
    }
}

// 主函数
int main()
{
    // UI和photo的头
    UI_LINK ui_head_node = (UI_LINK)Create_New_Node(MOD_ONE);
    PHOTO_LINK photo_head_node = (PHOTO_LINK)Create_New_Node(MOD_TWO);
    if (ui_head_node == (UI_LINK)NULL || photo_head_node == (PHOTO_LINK)NULL)
    {
        perror("头节点创建失败：\n");
        return -1;
    }

    Search_File(UI_BMP, BMP, MOD_ONE, (void *)ui_head_node, 1);
    Search_File(PHOTO_BMP, BMP, MOD_TWO, (void *)photo_head_node, 1);

    DEV_P DEV_pointer = Project_Initialize(ui_head_node);
    if (DEV_pointer == (DEV_P)-1)
    {
        perror("return fail:\n");
        return 0;
    }

    Function(DEV_pointer, ui_head_node, photo_head_node);

    FREE(DEV_pointer, ui_head_node, photo_head_node);
    free(ui_head_node);
    free(photo_head_node);

    return 0;
}

//      　     ▃▆█▇▄▖
// 　 　    ▟◤▖　　　◥█▎
//       ◢◤　 ▐　　　 　▐▉
// 　 ▗◤　　　▂　▗▖　▕█▎
// 　◤　▗▅▖◥▄　▀◣　　█▊
//  ▐　▕▎ ◥▖◣◤　　◢██
// █◣　◥▅█▀　　　　▐██◤
// ▐█▙▂　　     　◢██◤
//   ◥██◣　　　◢▄◤
//  　　▀██▅▇▀
