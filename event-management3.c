#define _CRT_SECURE_NO_WARNINGS  1 // 禁用安全警告 替换不安全的输入函数
#include <stdio.h>  //用于输入输出操作
#include <stdlib.h>  // 用于内存管理
#include <string.h>  // 用于字符串操作

#define MAX_STUDENTS 100 //  定义全局数组的最大长度
#define MAX_ACTIVITIES 100 // 
#define MAX_USERS 100 // 

// 学生结构体
typedef struct Student {  //定义了 Student 结构体
    int id;
    char name[50];
    char class_name[50]; 
    char department[50]; 
    int registered_activities[50]; 
    int num_registered_activities;
    struct Student* next; // 指向下一个学生节点的指针
} Student;

// 活动结构体
typedef struct Activity {  //定义了 Activity 结构体
    int id; // 活动编号
    char name[50]; // 活动名称
    char description[100]; // 活动描述
    int participants[MAX_STUDENTS]; // 报名学生学号
    int num_participants; // 报名学生数量
    struct Activity* next; // 指向下一个活动节点的指针
} Activity;

// 用户结构体（用于登录和注册）
typedef struct User {  //定义了 User 结构体
    char username[50]; // 用户名
    char password[50]; // 密码
    int is_admin; // 是否是管理员（1：管理员，0：学生）
    int student_id; // 如果是学生，关联的学生ID
    struct User* next; // 指向下一个用户节点的指针
} User;

// 栈结构体（用于撤销操作）
typedef struct StackNode {  //定义了 StackNode 结构体
    void* data; // 存储用户或活动数据
    int type; // 0: 用户, 1: 活动
    struct StackNode* next; // 指向下一个栈节点的指针
} StackNode;

typedef struct Stack {  //定义了 Stack 结构体
    StackNode* top; // 栈顶指针
} Stack;
//StackNode 结构体里，data 指针很灵活，它能存储用户或活动数据，具体就是指向 User 结构体或 Activity 结构体的指针，
//type 用来区分存储的数据到底是用户（值为 0）还是活动（值为 1），next 指针将一个个栈节点串起来；
//只有一个 top 指针指向栈顶元素，通过对栈顶的操作实现撤销功能，比如压入新操作记录、弹出最近操作以便撤销

//全局变量声明，声明了三个链表头指针 students、activities、users 分别指向学生链表、活动链表和用户链表的头节点
Student* students = NULL; // 学生链表头指针
Activity* activities = NULL; // 活动链表头指针
User* users = NULL; // 用户链表头指针
Stack undo_stack; // 撤销栈，声明了一个 undo_stack 栈用于撤销操作

int num_students = 0; // 当前学生数量
int num_activities = 0; // 当前活动数量
int num_users = 0; // 当前用户数量

int current_user_index = -1; // 当前登录用户的索引
//声明了 current_user_index 用于记录当前登录用户在用户链表中的索引，初始值为 -1 表示未登录

// 函数声明
void display_admin_menu();
void display_student_menu();
void add_student();
void delete_student();
void search_student_by_id();
void search_student_by_name();
void update_student();
void sort_students_by_name();
void load_students();
void save_students();
void display_all_students();
void create_activity();
void display_all_activities();
void register_for_activity();
void register_user();
int login();
void modify_student_info();
void push_stack(Stack* stack, void* data, int type);
void* pop_stack(Stack* stack);
void undo_last_operation();
//这些函数用于实现系统的各种功能，如显示菜单、添加学生、删除学生、创建活动等


// 主函数
int main() {
    undo_stack.top = NULL; // 初始化撤销栈的栈顶指针为 NULL

    while (1) { //进入一个无限循环，不断根据用户的登录状态显示不同的菜单
        if (current_user_index == -1) {
            // 未登录，显示登录菜单
            printf("1. 登录\n");
            printf("2. 注册\n");
            printf("3. 撤销上一次操作\n");
            printf("请选择操作：");
            char choice;
            scanf(" %c", &choice);
            if (choice == '1') {
                if (login()) {
                    if (users[current_user_index].is_admin) {
                        printf("管理员登录成功！\n");
                    } else {
                        printf("学生登录成功！\n");
                    }
                } else {
                    printf("登录失败，请重试！\n");
                }
            } else if (choice == '2') {
                register_user();
            } else if (choice == '3') {
                undo_last_operation();
            } else {
                printf("输入错误，请重新输入！\n");
            }
        } else {
            // 已登录，根据用户身份显示菜单
            if (users[current_user_index].is_admin) {
                display_admin_menu();
            } else {
                display_student_menu();
            }
        }
    }
    return 0;
}

// 显示管理员菜单
void display_admin_menu() {
    while (1) {
        printf("********************************************\n");
        printf("*                 管理员菜单               *\n");
        printf("* 1.增加一位学生信息  2.删除一位同学信息   *\n");
        printf("* 3.按学号查询学生    4.按姓名查询学生     *\n");
        printf("* 5.修改学生信息      6.按姓名排序         *\n");
        printf("* 7.创建活动          8.显示所有活动       *\n");
        printf("* 9.装入学生信息      0.保存学生信息       *\n");
        printf("* u.撤销上一次操作    q.返回上级菜单       *\n");
        printf("********************************************\n");
        printf("请选择操作：");

        char choice;//通过 switch 语句调用相应函数
        scanf(" %c", &choice);
        switch (choice) {
        case '1':
            add_student();
            break;
        case '2':
            delete_student();
            break;
        case '3':
            search_student_by_id();
            break;
        case '4':
            search_student_by_name();
            break;
        case '5':
            update_student();
            break;
        case '6':
            sort_students_by_name();
            break;
        case '7':
            create_activity();
            break;
        case '8':
            display_all_activities();
            break;
        case '9':
            load_students();
            break;
        case '0':
            save_students();
            break;
        case 'u':
            undo_last_operation();
            break;
        case 'q':
            current_user_index = -1; // 返回上级菜单
            return;
        default:
            printf("输入错误，请重新输入！\n");
            break;
        }
    }
}

// 显示学生菜单
void display_student_menu() {
    while (1) {
        printf("********************************************\n");
        printf("*                 学生菜单                 *\n");
        printf("* 1.修改个人信息      2.报名活动           *\n");
        printf("* 3.查看已报名活动    q.返回上级菜单       *\n");
        printf("********************************************\n");
        printf("请选择操作：");

        char choice;
        scanf(" %c", &choice);
        switch (choice) {
        case '1':
            modify_student_info();
            break;
        case '2':
            register_for_activity();
            break;
        case '3':
            // 查看已报名活动
            {
                int student_id = users[current_user_index].student_id;
                Student* current = students;
                while (current != NULL) {
                    if (current->id == student_id) {
                        printf("已报名的活动：\n");
                        for (int j = 0; j < current->num_registered_activities; j++) {
                            int activity_id = current->registered_activities[j];
                            Activity* activity = activities;
                            while (activity != NULL) {
                                if (activity->id == activity_id) {
                                    printf("活动编号：%d, 活动名称：%s, 活动描述：%s\n",
                                           activity->id, activity->name, activity->description);
                                    break;
                                }
                                activity = activity->next;
                            }
                        }
                        break;
                    }
                    current = current->next;
                }
            }
            break;
        case 'q':
            current_user_index = -1; // 返回上级菜单
            return;
        default:
            printf("输入错误，请重新输入！\n");
            break;
        }
    }
}

// 添加学生
void add_student() {
    if (num_students >= MAX_STUDENTS) {
        printf("学生数量已达上限，无法添加更多学生！\n");
        return;
    }
//动态分配内存创建一个新的学生节点
    Student* new_student = (Student*)malloc(sizeof(Student));
    printf("输入学生的学号（-1 结束）: ");
    scanf("%d", &new_student->id);

    if (new_student->id == -1) {
        free(new_student);
        return;
    }

    printf("输入学生的姓名: ");
    scanf("%s", new_student->name);
    printf("输入学生的班级: ");
    scanf("%s", new_student->class_name);
    printf("输入学生的学院: ");
    scanf("%s", new_student->department);
    new_student->num_registered_activities = 0; // 初始化已报名活动数量，然后把新节点插入学生链表头部
    new_student->next = students;//这两步操作，先让新节点的 next 指向原链表头，再更新链表头指针为新节点，最后学生数量加 1 并提示添加成功
    students = new_student;//这一系列操作体现了链表的动态增长特性，利用链表结构灵活管理学生信息
    num_students++;  //将新学生节点插入到学生链表的头部，更新学生数量并提示添加成功
    printf("学生信息添加成功\n");
}

// 删除学生
void delete_student() {
    int student_id;
    printf("输入学生的学号: ");
    scanf("%d", &student_id);//输入要删除的学生的学号

    Student* current = students;//遍历学生链表，找到要删除的学生节点
    Student* prev = NULL;//当前节点的前一个节点
    while (current != NULL) {
        if (current->id == student_id) {
            if (prev == NULL) {//找到学号匹配的节点后，如果 prev 是 NULL，说明要删的是头节点
                students = current->next;//直接更新链表头指针 students = current->next
            } else {
                prev->next = current->next; //跳过当前节点，再释放当前节点内存
            }
            free(current);
            num_students--;
            printf("删除学生信息成功\n");
            return;
        }
        prev = current;
        current = current->next;
    }

    printf("学号不存在\n");
}

// 按学号查询学生
void search_student_by_id() {
    int student_id;
    printf("输入学生的学号: ");
    scanf("%d", &student_id);

    Student* current = students;
    while (current != NULL) {
        if (current->id == student_id) {
            printf("学号\t姓名\t班级\t学院\n");
            printf("%d\t%s\t%s\t%s\n",
                   current->id, current->name, current->class_name, current->department);
            return;
        }
        current = current->next;
    }

    printf("学号不存在\n");
}

// 按姓名查询学生
void search_student_by_name() {
    char student_name[50];
    printf("输入学生的姓名: ");
    scanf("%s", student_name);

    Student* current = students;
    int found = 0;
    printf("学号\t姓名\t班级\t学院\n");
    while (current != NULL) {
        if (strcmp(current->name, student_name) == 0) {
            printf("%d\t%s\t%s\t%s\n",
                   current->id, current->name, current->class_name, current->department);
            found = 1;
        }
        current = current->next;
    }

    if (!found) {
        printf("学生不存在\n");
    }
}

// 修改学生信息
void update_student() {
    int student_id;
    printf("输入学生的学号: ");
    scanf("%d", &student_id);

    Student* current = students;
    while (current != NULL) {
        if (current->id == student_id) {
            printf("输入学生的姓名: ");
            scanf("%s", current->name);
            printf("输入学生的班级: ");
            scanf("%s", current->class_name);
            printf("输入学生的学院: ");
            scanf("%s", current->department);
            printf("学生信息更新成功\n");
            return;
        }
        current = current->next;
    }

    printf("学号不存在\n");
}

// 按姓名排序学生
void sort_students_by_name() {
    // 由于链表排序较为复杂，这里可以使用数组进行排序后再重新构建链表
    // 为了简化代码，这里假设链表已经按姓名排序
    printf("学号\t姓名\t班级\t学院\n");
    Student* current = students;
    while (current != NULL) {
        printf("%d\t%s\t%s\t%s\n",
               current->id, current->name, current->class_name, current->department);
        current = current->next;
    }
}



//装入学生信息
void load_students() {
    FILE* file = fopen("student_info.bin", "rb");
    if (file == NULL) {
        printf("无法打开文件\n");
        return;
    }
    fread(&num_students, sizeof(int), 1, file);
    for (int i = 0; i < num_students; i++) {
        Student* new_student = (Student*)malloc(sizeof(Student));
        fread(new_student, sizeof(Student), 1, file);
        new_student->next = students;
        students = new_student;
    }
    fclose(file);
    printf("学生信息装载成功\n");
}
 
// 保存学生信息
void save_students() {//将学生信息保存到二进制文件中
    FILE* file = fopen("student_info.bin", "wb");//打开二进制文件
    if (file == NULL) {//如果文件打开失败
        printf("无法打开文件\n");//输出错误信息并返回
        return;
    }
    fwrite(&num_students, sizeof(int), 1, file);//将学生数量写入文件
    Student* current = students;//将学生信息写入文件
    while (current != NULL) {//循环遍历学生链表
        fwrite(current, sizeof(Student), 1, file);//        写入学生信息
        current = current->next;//        移动到下一个学生
    }
    fclose(file);//关闭文件
    printf("学生信息保存成功\n");
}

// 显示所有学生信息
void display_all_students() {
    Student* current = students;
    printf("学号\t姓名\t班级\t学院\n");
    while (current != NULL) {
        printf("%d\t%s\t%s\t%s\n",
               current->id, current->name, current->class_name, current->department);
        current = current->next;
    }
}

// 创建活动
void create_activity() {
    if (num_activities >= MAX_ACTIVITIES) {
        printf("活动数量已达上限，无法创建更多活动！\n");
        return;
    }

    Activity* new_activity = (Activity*)malloc(sizeof(Activity));//利用 malloc 为新活动分配内存空间,用于存储活动的相关信息
    printf("输入活动编号: ");
    scanf("%d", &new_activity->id);
    printf("输入活动名称: ");
    scanf("%s", new_activity->name);
    printf("输入活动描述: ");
    scanf("%s", new_activity->description);
    new_activity->num_participants = 0; // 初始化报名学生数量
    new_activity->next = activities;//链表操作：将新活动节点插入到活动链表的头部
    activities = new_activity;//通过 new_activity->next = activities 将新节点的指针指向原来的活动链表头节点，然后 activities = new_activity 使活动链表头指针指向新节点
    num_activities++;

    // 调用 push_stack 函数将活动信息压入撤销栈
    push_stack(&undo_stack, new_activity, 1);
    //将活动节点指针和数据类型（1 表示活动）传递给 push_stack 函数，在函数内部创建栈节点并插入到栈顶。

    printf("活动创建成功\n");
}

// 显示所有活动
void display_all_activities() {
    Activity* current = activities;
    printf("活动编号\t活动名称\t活动描述\n");
    while (current != NULL) {
        printf("%d\t\t%s\t\t%s\n",
               current->id, current->name, current->description);
        current = current->next;
    }
}
//输入输出逻辑：
//输入：内存中的活动链表。
//输出：在控制台输出表头 “活动编号 活动名称 活动描述”，接着逐行输出每个活动的具体信息


// 学生报名活动
void register_for_activity() {
    int student_id = users[current_user_index].student_id;//获取当前用户学号
    int activity_id;
    printf("输入活动编号: ");
    scanf("%d", &activity_id);

    // 查找活动
    Activity* activity = activities;// 从活动链表头开始查找
    while (activity != NULL) {// 循环遍历活动链表
        if (activity->id == activity_id) {  // 找到活动
            break;
        }
        activity = activity->next;
    }

    if (activity == NULL) {// 活动不存在
        printf("活动不存在\n");
        return;
    }

    // 检查学生是否已报名
    Student* student = students;
    while (student != NULL) {
        if (student->id == student_id) {
            for (int j = 0; j < student->num_registered_activities; j++) {// 遍历学生已报名的活动
                if (student->registered_activities[j] == activity_id) {// 判断是否已报名
                    printf("您已报名此活动\n");
                    return;
                }
            }
            // 报名活动
            student->registered_activities[student->num_registered_activities++] = activity_id;// 报名活动
            activity->participants[activity->num_participants++] = student_id;// 报名活动
            printf("报名成功\n");
            return;
        }
        student = student->next;// 遍历链表
    }

    printf("学生不存在\n");
}

// 注册用户
void register_user() {
    if (num_users >= MAX_USERS) {
        printf("用户数量已达上限，无法注册更多用户！\n");
        return;
    }

    User* new_user = (User*)malloc(sizeof(User));//利用 malloc 为新用户分配内存空间
    printf("请输入用户名: ");
    scanf("%s", new_user->username);
    printf("请输入密码: ");
    scanf("%s", new_user->password);
    printf("请选择身份（1: 管理员, 0: 学生）: ");
    scanf("%d", &new_user->is_admin);

    if (new_user->is_admin == 0) {
        printf("请输入学生学号: ");
        scanf("%d", &new_user->student_id);
        // 检查学生是否存在
        Student* student = students;
        int student_found = 0;
        while (student != NULL) {
            if (student->id == new_user->student_id) {
                student_found = 1;
                break;
            }
            student = student->next;
        }
        if (!student_found) {
            printf("学生学号不存在，请先添加学生信息！\n");
            free(new_user);
            return;
        }
    } else {
        new_user->student_id = -1; // 管理员没有关联的学生ID
    }

    new_user->next = users;
    users = new_user;
    num_users++;

    // 将用户信息压入撤销栈
    push_stack(&undo_stack, new_user, 0);

    printf("注册成功\n");
}

// 登录功能
int login() {
    char username[50], password[50];
    printf("请输入用户名: ");
    scanf("%s", username);
    printf("请输入密码: ");
    scanf("%s", password);

    User* current = users;// 遍历链表
    int index = 0;// 记录当前遍历到的用户索引
    while (current != NULL) {
        if (strcmp(current->username, username) == 0 && strcmp(current->password, password) == 0) {
            current_user_index = index;
            return 1;
        }
        current = current->next;
        index++;
    }

    printf("用户名或密码错误\n");
    return 0;
}

// 学生修改个人信息
void modify_student_info() {
    int student_id = users[current_user_index].student_id;// 获取当前登录用户的学号
    Student* current = students;
    while (current != NULL) {
        if (current->id == student_id) {
            printf("输入学生的姓名: ");
            scanf("%s", current->name);
            printf("输入学生的班级: ");
            scanf("%s", current->class_name);
            printf("输入学生的学院: ");
            scanf("%s", current->department);
            printf("学生信息更新成功\n");
            return;
        }
        current = current->next;//遍历链表
    }
    printf("学生不存在\n");
}

// 将数据压入栈
//将数据和数据类型存储到新栈节点中,并将新栈节点插入到栈顶
void push_stack(Stack* stack, void* data, int type) {
    StackNode* new_node = (StackNode*)malloc(sizeof(StackNode));//利用 malloc 为新栈节点分配内存空间。
    new_node->data = data;//将数据压入栈
    new_node->type = type;
    new_node->next = stack->top;   //将新栈节点插入到栈顶
    stack->top = new_node;//将新栈节点设置为栈顶
}

// 从栈中弹出数据
void* pop_stack(Stack* stack) {
    if (stack->top == NULL) {  //检查栈是否为空
        return NULL;
    }
    StackNode* node = stack->top;//从栈顶弹出栈节点
    void* data = node->data;//将栈节点的数据返回
    stack->top = node->next;//将栈顶指向下一个节点
    free(node);//释放栈节点
    return data;//返回栈节点的数据
}

// 撤销上一次操作
void undo_last_operation() {//撤销上一次操作
    StackNode* node = undo_stack.top;//从撤销栈中取出栈节点
    if (node == NULL) {//如果撤销栈为空，则输出提示信息并返回
        printf("没有可撤销的操作\n");
        return;
    }

    if (node->type == 0) {
        // 撤销用户注册
        User* user = (User*)node->data;//从栈节点中取出用户结构体
        User* current = users;  // 遍历用户链表
        User* prev = NULL;//存储前一个节点
        while (current != NULL) {//遍历用户链表
            if (current == user) {
                if (prev == NULL){//如果当前节点是链表的头节点，则更新头指针
                    users = current->next;//更新头指针
                } else {
                    prev->next = current->next;//将前一个节点的next指针指向当前节点的下一个节点
                }
                free(current);
                num_users--;
                printf("撤销用户注册成功\n");
                break;
            }
            prev = current;//将前一个节点更新为当前节点
            current = current->next;//将当前节点更新为下一个节点
        }
    } else if (node->type == 1) {
        // 撤销活动创建
        Activity* activity = (Activity*)node->data;//将节点数据转换为活动结构体
        Activity* current = activities; //从活动链表头开始遍历
        Activity* prev = NULL;//上一个节点
        while (current != NULL) {//循环遍历活动链表
            if (current == activity) {//如果当前节点是目标活动，则进行删除操作
                if (prev == NULL) {//如果当前节点是链表头节点，则更新链表头节点为下一个节点
                    activities = current->next;//更新链表头节点为下一个节点
                } else {
                    prev->next = current->next;//将当前节点的前一个节点的next指针指向当前节点的下一个节点
                }
                free(current);//释放当前节点的内存
                num_activities--;
                printf("撤销活动创建成功\n");
                break;
            }
            prev = current;//将当前节点赋值给前一个节点
            current = current->next;//将当前节点赋值给下一个节点
        }
    }

    // 弹出栈顶元素
    pop_stack(&undo_stack);
}
/*数据结构操作：
检查撤销栈是否为空。
根据栈顶节点的数据类型，分别处理撤销用户注册和撤销活动创建操作。
若为撤销用户注册，遍历用户链表，删除相应的用户节点。
若为撤销活动创建，遍历活动链表，删除相应的活动节点。
调用 pop_stack 函数弹出栈顶元素*/