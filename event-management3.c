#define _CRT_SECURE_NO_WARNINGS  1 // ���ð�ȫ���� �滻����ȫ�����뺯��
#include <stdio.h>  //���������������
#include <stdlib.h>  // �����ڴ����
#include <string.h>  // �����ַ�������

#define MAX_STUDENTS 100 //  ����ȫ���������󳤶�
#define MAX_ACTIVITIES 100 // 
#define MAX_USERS 100 // 

// ѧ���ṹ��
typedef struct Student {  //������ Student �ṹ��
    int id;
    char name[50];
    char class_name[50]; 
    char department[50]; 
    int registered_activities[50]; 
    int num_registered_activities;
    struct Student* next; // ָ����һ��ѧ���ڵ��ָ��
} Student;

// ��ṹ��
typedef struct Activity {  //������ Activity �ṹ��
    int id; // ����
    char name[50]; // �����
    char description[100]; // �����
    int participants[MAX_STUDENTS]; // ����ѧ��ѧ��
    int num_participants; // ����ѧ������
    struct Activity* next; // ָ����һ����ڵ��ָ��
} Activity;

// �û��ṹ�壨���ڵ�¼��ע�ᣩ
typedef struct User {  //������ User �ṹ��
    char username[50]; // �û���
    char password[50]; // ����
    int is_admin; // �Ƿ��ǹ���Ա��1������Ա��0��ѧ����
    int student_id; // �����ѧ����������ѧ��ID
    struct User* next; // ָ����һ���û��ڵ��ָ��
} User;

// ջ�ṹ�壨���ڳ���������
typedef struct StackNode {  //������ StackNode �ṹ��
    void* data; // �洢�û�������
    int type; // 0: �û�, 1: �
    struct StackNode* next; // ָ����һ��ջ�ڵ��ָ��
} StackNode;

typedef struct Stack {  //������ Stack �ṹ��
    StackNode* top; // ջ��ָ��
} Stack;
//StackNode �ṹ���data ָ��������ܴ洢�û������ݣ��������ָ�� User �ṹ��� Activity �ṹ���ָ�룬
//type �������ִ洢�����ݵ������û���ֵΪ 0�����ǻ��ֵΪ 1����next ָ�뽫һ����ջ�ڵ㴮������
//ֻ��һ�� top ָ��ָ��ջ��Ԫ�أ�ͨ����ջ���Ĳ���ʵ�ֳ������ܣ�����ѹ���²�����¼��������������Ա㳷��

//ȫ�ֱ�����������������������ͷָ�� students��activities��users �ֱ�ָ��ѧ�������������û������ͷ�ڵ�
Student* students = NULL; // ѧ������ͷָ��
Activity* activities = NULL; // �����ͷָ��
User* users = NULL; // �û�����ͷָ��
Stack undo_stack; // ����ջ��������һ�� undo_stack ջ���ڳ�������

int num_students = 0; // ��ǰѧ������
int num_activities = 0; // ��ǰ�����
int num_users = 0; // ��ǰ�û�����

int current_user_index = -1; // ��ǰ��¼�û�������
//������ current_user_index ���ڼ�¼��ǰ��¼�û����û������е���������ʼֵΪ -1 ��ʾδ��¼

// ��������
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
//��Щ��������ʵ��ϵͳ�ĸ��ֹ��ܣ�����ʾ�˵������ѧ����ɾ��ѧ�����������


// ������
int main() {
    undo_stack.top = NULL; // ��ʼ������ջ��ջ��ָ��Ϊ NULL

    while (1) { //����һ������ѭ�������ϸ����û��ĵ�¼״̬��ʾ��ͬ�Ĳ˵�
        if (current_user_index == -1) {
            // δ��¼����ʾ��¼�˵�
            printf("1. ��¼\n");
            printf("2. ע��\n");
            printf("3. ������һ�β���\n");
            printf("��ѡ�������");
            char choice;
            scanf(" %c", &choice);
            if (choice == '1') {
                if (login()) {
                    if (users[current_user_index].is_admin) {
                        printf("����Ա��¼�ɹ���\n");
                    } else {
                        printf("ѧ����¼�ɹ���\n");
                    }
                } else {
                    printf("��¼ʧ�ܣ������ԣ�\n");
                }
            } else if (choice == '2') {
                register_user();
            } else if (choice == '3') {
                undo_last_operation();
            } else {
                printf("����������������룡\n");
            }
        } else {
            // �ѵ�¼�������û������ʾ�˵�
            if (users[current_user_index].is_admin) {
                display_admin_menu();
            } else {
                display_student_menu();
            }
        }
    }
    return 0;
}

// ��ʾ����Ա�˵�
void display_admin_menu() {
    while (1) {
        printf("********************************************\n");
        printf("*                 ����Ա�˵�               *\n");
        printf("* 1.����һλѧ����Ϣ  2.ɾ��һλͬѧ��Ϣ   *\n");
        printf("* 3.��ѧ�Ų�ѯѧ��    4.��������ѯѧ��     *\n");
        printf("* 5.�޸�ѧ����Ϣ      6.����������         *\n");
        printf("* 7.�����          8.��ʾ���л       *\n");
        printf("* 9.װ��ѧ����Ϣ      0.����ѧ����Ϣ       *\n");
        printf("* u.������һ�β���    q.�����ϼ��˵�       *\n");
        printf("********************************************\n");
        printf("��ѡ�������");

        char choice;//ͨ�� switch ��������Ӧ����
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
            current_user_index = -1; // �����ϼ��˵�
            return;
        default:
            printf("����������������룡\n");
            break;
        }
    }
}

// ��ʾѧ���˵�
void display_student_menu() {
    while (1) {
        printf("********************************************\n");
        printf("*                 ѧ���˵�                 *\n");
        printf("* 1.�޸ĸ�����Ϣ      2.�����           *\n");
        printf("* 3.�鿴�ѱ����    q.�����ϼ��˵�       *\n");
        printf("********************************************\n");
        printf("��ѡ�������");

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
            // �鿴�ѱ����
            {
                int student_id = users[current_user_index].student_id;
                Student* current = students;
                while (current != NULL) {
                    if (current->id == student_id) {
                        printf("�ѱ����Ļ��\n");
                        for (int j = 0; j < current->num_registered_activities; j++) {
                            int activity_id = current->registered_activities[j];
                            Activity* activity = activities;
                            while (activity != NULL) {
                                if (activity->id == activity_id) {
                                    printf("���ţ�%d, ����ƣ�%s, �������%s\n",
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
            current_user_index = -1; // �����ϼ��˵�
            return;
        default:
            printf("����������������룡\n");
            break;
        }
    }
}

// ���ѧ��
void add_student() {
    if (num_students >= MAX_STUDENTS) {
        printf("ѧ�������Ѵ����ޣ��޷���Ӹ���ѧ����\n");
        return;
    }
//��̬�����ڴ洴��һ���µ�ѧ���ڵ�
    Student* new_student = (Student*)malloc(sizeof(Student));
    printf("����ѧ����ѧ�ţ�-1 ������: ");
    scanf("%d", &new_student->id);

    if (new_student->id == -1) {
        free(new_student);
        return;
    }

    printf("����ѧ��������: ");
    scanf("%s", new_student->name);
    printf("����ѧ���İ༶: ");
    scanf("%s", new_student->class_name);
    printf("����ѧ����ѧԺ: ");
    scanf("%s", new_student->department);
    new_student->num_registered_activities = 0; // ��ʼ���ѱ����������Ȼ����½ڵ����ѧ������ͷ��
    new_student->next = students;//�����������������½ڵ�� next ָ��ԭ����ͷ���ٸ�������ͷָ��Ϊ�½ڵ㣬���ѧ�������� 1 ����ʾ��ӳɹ�
    students = new_student;//��һϵ�в�������������Ķ�̬�������ԣ���������ṹ������ѧ����Ϣ
    num_students++;  //����ѧ���ڵ���뵽ѧ�������ͷ��������ѧ����������ʾ��ӳɹ�
    printf("ѧ����Ϣ��ӳɹ�\n");
}

// ɾ��ѧ��
void delete_student() {
    int student_id;
    printf("����ѧ����ѧ��: ");
    scanf("%d", &student_id);//����Ҫɾ����ѧ����ѧ��

    Student* current = students;//����ѧ�������ҵ�Ҫɾ����ѧ���ڵ�
    Student* prev = NULL;//��ǰ�ڵ��ǰһ���ڵ�
    while (current != NULL) {
        if (current->id == student_id) {
            if (prev == NULL) {//�ҵ�ѧ��ƥ��Ľڵ����� prev �� NULL��˵��Ҫɾ����ͷ�ڵ�
                students = current->next;//ֱ�Ӹ�������ͷָ�� students = current->next
            } else {
                prev->next = current->next; //������ǰ�ڵ㣬���ͷŵ�ǰ�ڵ��ڴ�
            }
            free(current);
            num_students--;
            printf("ɾ��ѧ����Ϣ�ɹ�\n");
            return;
        }
        prev = current;
        current = current->next;
    }

    printf("ѧ�Ų�����\n");
}

// ��ѧ�Ų�ѯѧ��
void search_student_by_id() {
    int student_id;
    printf("����ѧ����ѧ��: ");
    scanf("%d", &student_id);

    Student* current = students;
    while (current != NULL) {
        if (current->id == student_id) {
            printf("ѧ��\t����\t�༶\tѧԺ\n");
            printf("%d\t%s\t%s\t%s\n",
                   current->id, current->name, current->class_name, current->department);
            return;
        }
        current = current->next;
    }

    printf("ѧ�Ų�����\n");
}

// ��������ѯѧ��
void search_student_by_name() {
    char student_name[50];
    printf("����ѧ��������: ");
    scanf("%s", student_name);

    Student* current = students;
    int found = 0;
    printf("ѧ��\t����\t�༶\tѧԺ\n");
    while (current != NULL) {
        if (strcmp(current->name, student_name) == 0) {
            printf("%d\t%s\t%s\t%s\n",
                   current->id, current->name, current->class_name, current->department);
            found = 1;
        }
        current = current->next;
    }

    if (!found) {
        printf("ѧ��������\n");
    }
}

// �޸�ѧ����Ϣ
void update_student() {
    int student_id;
    printf("����ѧ����ѧ��: ");
    scanf("%d", &student_id);

    Student* current = students;
    while (current != NULL) {
        if (current->id == student_id) {
            printf("����ѧ��������: ");
            scanf("%s", current->name);
            printf("����ѧ���İ༶: ");
            scanf("%s", current->class_name);
            printf("����ѧ����ѧԺ: ");
            scanf("%s", current->department);
            printf("ѧ����Ϣ���³ɹ�\n");
            return;
        }
        current = current->next;
    }

    printf("ѧ�Ų�����\n");
}

// ����������ѧ��
void sort_students_by_name() {
    // �������������Ϊ���ӣ��������ʹ�������������������¹�������
    // Ϊ�˼򻯴��룬������������Ѿ�����������
    printf("ѧ��\t����\t�༶\tѧԺ\n");
    Student* current = students;
    while (current != NULL) {
        printf("%d\t%s\t%s\t%s\n",
               current->id, current->name, current->class_name, current->department);
        current = current->next;
    }
}



//װ��ѧ����Ϣ
void load_students() {
    FILE* file = fopen("student_info.bin", "rb");
    if (file == NULL) {
        printf("�޷����ļ�\n");
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
    printf("ѧ����Ϣװ�سɹ�\n");
}
 
// ����ѧ����Ϣ
void save_students() {//��ѧ����Ϣ���浽�������ļ���
    FILE* file = fopen("student_info.bin", "wb");//�򿪶������ļ�
    if (file == NULL) {//����ļ���ʧ��
        printf("�޷����ļ�\n");//���������Ϣ������
        return;
    }
    fwrite(&num_students, sizeof(int), 1, file);//��ѧ������д���ļ�
    Student* current = students;//��ѧ����Ϣд���ļ�
    while (current != NULL) {//ѭ������ѧ������
        fwrite(current, sizeof(Student), 1, file);//        д��ѧ����Ϣ
        current = current->next;//        �ƶ�����һ��ѧ��
    }
    fclose(file);//�ر��ļ�
    printf("ѧ����Ϣ����ɹ�\n");
}

// ��ʾ����ѧ����Ϣ
void display_all_students() {
    Student* current = students;
    printf("ѧ��\t����\t�༶\tѧԺ\n");
    while (current != NULL) {
        printf("%d\t%s\t%s\t%s\n",
               current->id, current->name, current->class_name, current->department);
        current = current->next;
    }
}

// �����
void create_activity() {
    if (num_activities >= MAX_ACTIVITIES) {
        printf("������Ѵ����ޣ��޷�����������\n");
        return;
    }

    Activity* new_activity = (Activity*)malloc(sizeof(Activity));//���� malloc Ϊ�»�����ڴ�ռ�,���ڴ洢��������Ϣ
    printf("�������: ");
    scanf("%d", &new_activity->id);
    printf("��������: ");
    scanf("%s", new_activity->name);
    printf("��������: ");
    scanf("%s", new_activity->description);
    new_activity->num_participants = 0; // ��ʼ������ѧ������
    new_activity->next = activities;//������������»�ڵ���뵽������ͷ��
    activities = new_activity;//ͨ�� new_activity->next = activities ���½ڵ��ָ��ָ��ԭ���Ļ����ͷ�ڵ㣬Ȼ�� activities = new_activity ʹ�����ͷָ��ָ���½ڵ�
    num_activities++;

    // ���� push_stack ���������Ϣѹ�볷��ջ
    push_stack(&undo_stack, new_activity, 1);
    //����ڵ�ָ����������ͣ�1 ��ʾ������ݸ� push_stack �������ں����ڲ�����ջ�ڵ㲢���뵽ջ����

    printf("������ɹ�\n");
}

// ��ʾ���л
void display_all_activities() {
    Activity* current = activities;
    printf("����\t�����\t�����\n");
    while (current != NULL) {
        printf("%d\t\t%s\t\t%s\n",
               current->id, current->name, current->description);
        current = current->next;
    }
}
//��������߼���
//���룺�ڴ��еĻ����
//������ڿ���̨�����ͷ ������ ����� ��������������������ÿ����ľ�����Ϣ


// ѧ�������
void register_for_activity() {
    int student_id = users[current_user_index].student_id;//��ȡ��ǰ�û�ѧ��
    int activity_id;
    printf("�������: ");
    scanf("%d", &activity_id);

    // ���һ
    Activity* activity = activities;// �ӻ����ͷ��ʼ����
    while (activity != NULL) {// ѭ�����������
        if (activity->id == activity_id) {  // �ҵ��
            break;
        }
        activity = activity->next;
    }

    if (activity == NULL) {// �������
        printf("�������\n");
        return;
    }

    // ���ѧ���Ƿ��ѱ���
    Student* student = students;
    while (student != NULL) {
        if (student->id == student_id) {
            for (int j = 0; j < student->num_registered_activities; j++) {// ����ѧ���ѱ����Ļ
                if (student->registered_activities[j] == activity_id) {// �ж��Ƿ��ѱ���
                    printf("���ѱ����˻\n");
                    return;
                }
            }
            // �����
            student->registered_activities[student->num_registered_activities++] = activity_id;// �����
            activity->participants[activity->num_participants++] = student_id;// �����
            printf("�����ɹ�\n");
            return;
        }
        student = student->next;// ��������
    }

    printf("ѧ��������\n");
}

// ע���û�
void register_user() {
    if (num_users >= MAX_USERS) {
        printf("�û������Ѵ����ޣ��޷�ע������û���\n");
        return;
    }

    User* new_user = (User*)malloc(sizeof(User));//���� malloc Ϊ���û������ڴ�ռ�
    printf("�������û���: ");
    scanf("%s", new_user->username);
    printf("����������: ");
    scanf("%s", new_user->password);
    printf("��ѡ����ݣ�1: ����Ա, 0: ѧ����: ");
    scanf("%d", &new_user->is_admin);

    if (new_user->is_admin == 0) {
        printf("������ѧ��ѧ��: ");
        scanf("%d", &new_user->student_id);
        // ���ѧ���Ƿ����
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
            printf("ѧ��ѧ�Ų����ڣ��������ѧ����Ϣ��\n");
            free(new_user);
            return;
        }
    } else {
        new_user->student_id = -1; // ����Աû�й�����ѧ��ID
    }

    new_user->next = users;
    users = new_user;
    num_users++;

    // ���û���Ϣѹ�볷��ջ
    push_stack(&undo_stack, new_user, 0);

    printf("ע��ɹ�\n");
}

// ��¼����
int login() {
    char username[50], password[50];
    printf("�������û���: ");
    scanf("%s", username);
    printf("����������: ");
    scanf("%s", password);

    User* current = users;// ��������
    int index = 0;// ��¼��ǰ���������û�����
    while (current != NULL) {
        if (strcmp(current->username, username) == 0 && strcmp(current->password, password) == 0) {
            current_user_index = index;
            return 1;
        }
        current = current->next;
        index++;
    }

    printf("�û������������\n");
    return 0;
}

// ѧ���޸ĸ�����Ϣ
void modify_student_info() {
    int student_id = users[current_user_index].student_id;// ��ȡ��ǰ��¼�û���ѧ��
    Student* current = students;
    while (current != NULL) {
        if (current->id == student_id) {
            printf("����ѧ��������: ");
            scanf("%s", current->name);
            printf("����ѧ���İ༶: ");
            scanf("%s", current->class_name);
            printf("����ѧ����ѧԺ: ");
            scanf("%s", current->department);
            printf("ѧ����Ϣ���³ɹ�\n");
            return;
        }
        current = current->next;//��������
    }
    printf("ѧ��������\n");
}

// ������ѹ��ջ
//�����ݺ��������ʹ洢����ջ�ڵ���,������ջ�ڵ���뵽ջ��
void push_stack(Stack* stack, void* data, int type) {
    StackNode* new_node = (StackNode*)malloc(sizeof(StackNode));//���� malloc Ϊ��ջ�ڵ�����ڴ�ռ䡣
    new_node->data = data;//������ѹ��ջ
    new_node->type = type;
    new_node->next = stack->top;   //����ջ�ڵ���뵽ջ��
    stack->top = new_node;//����ջ�ڵ�����Ϊջ��
}

// ��ջ�е�������
void* pop_stack(Stack* stack) {
    if (stack->top == NULL) {  //���ջ�Ƿ�Ϊ��
        return NULL;
    }
    StackNode* node = stack->top;//��ջ������ջ�ڵ�
    void* data = node->data;//��ջ�ڵ�����ݷ���
    stack->top = node->next;//��ջ��ָ����һ���ڵ�
    free(node);//�ͷ�ջ�ڵ�
    return data;//����ջ�ڵ������
}

// ������һ�β���
void undo_last_operation() {//������һ�β���
    StackNode* node = undo_stack.top;//�ӳ���ջ��ȡ��ջ�ڵ�
    if (node == NULL) {//�������ջΪ�գ��������ʾ��Ϣ������
        printf("û�пɳ����Ĳ���\n");
        return;
    }

    if (node->type == 0) {
        // �����û�ע��
        User* user = (User*)node->data;//��ջ�ڵ���ȡ���û��ṹ��
        User* current = users;  // �����û�����
        User* prev = NULL;//�洢ǰһ���ڵ�
        while (current != NULL) {//�����û�����
            if (current == user) {
                if (prev == NULL){//�����ǰ�ڵ��������ͷ�ڵ㣬�����ͷָ��
                    users = current->next;//����ͷָ��
                } else {
                    prev->next = current->next;//��ǰһ���ڵ��nextָ��ָ��ǰ�ڵ����һ���ڵ�
                }
                free(current);
                num_users--;
                printf("�����û�ע��ɹ�\n");
                break;
            }
            prev = current;//��ǰһ���ڵ����Ϊ��ǰ�ڵ�
            current = current->next;//����ǰ�ڵ����Ϊ��һ���ڵ�
        }
    } else if (node->type == 1) {
        // ���������
        Activity* activity = (Activity*)node->data;//���ڵ�����ת��Ϊ��ṹ��
        Activity* current = activities; //�ӻ����ͷ��ʼ����
        Activity* prev = NULL;//��һ���ڵ�
        while (current != NULL) {//ѭ�����������
            if (current == activity) {//�����ǰ�ڵ���Ŀ���������ɾ������
                if (prev == NULL) {//�����ǰ�ڵ�������ͷ�ڵ㣬���������ͷ�ڵ�Ϊ��һ���ڵ�
                    activities = current->next;//��������ͷ�ڵ�Ϊ��һ���ڵ�
                } else {
                    prev->next = current->next;//����ǰ�ڵ��ǰһ���ڵ��nextָ��ָ��ǰ�ڵ����һ���ڵ�
                }
                free(current);//�ͷŵ�ǰ�ڵ���ڴ�
                num_activities--;
                printf("����������ɹ�\n");
                break;
            }
            prev = current;//����ǰ�ڵ㸳ֵ��ǰһ���ڵ�
            current = current->next;//����ǰ�ڵ㸳ֵ����һ���ڵ�
        }
    }

    // ����ջ��Ԫ��
    pop_stack(&undo_stack);
}
/*���ݽṹ������
��鳷��ջ�Ƿ�Ϊ�ա�
����ջ���ڵ���������ͣ��ֱ������û�ע��ͳ��������������
��Ϊ�����û�ע�ᣬ�����û�����ɾ����Ӧ���û��ڵ㡣
��Ϊ��������������������ɾ����Ӧ�Ļ�ڵ㡣
���� pop_stack ��������ջ��Ԫ��*/