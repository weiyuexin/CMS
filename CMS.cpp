#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<Windows.h>
#include<WinSock.h>
#include<mysql.h>
#include<string.h>
#include<conio.h>

using namespace std;
#pragma comment(lib,"libmysql.lib")
#pragma comment(lib,"wsock32.lib")
MYSQL* mysql = new MYSQL; //mysql连接  
MYSQL_FIELD* fd;    //字段列数组  
char field[32][32];    //存字段名二维数组  
MYSQL_RES* res; //这个结构代表返回行的一个查询结果集  
MYSQL_ROW column; //一个行数据的类型安全(type-safe)的表示，表示数据行的列  
char query[150]; //查询语句  
//定义用户的用户名
string username = "";
// 定义是否是管理员的标志,默认不是
bool isAdmin = false;
//定义商品类型
enum GoodsType//商品类别
{
    Food = 1,  //食品
    Cosmetic,  //化妆品
    Commodity,  //日用品
    Drink,  //饮料
    Stationery  //文具
};

//商品基本类型
struct Goods//商品基本信息
{
    string code;//商品编号
    string name;//商品名称
    string brand;//生产厂家
    double price;//商品价格
    int num;//商品数量
    GoodsType type;//商品类别
    string date;//入库时间
};


//连接数据库的函数
bool ConnectDatabase();
bool QueryDatabase1();
//初始化菜单，用来让用户选择登录
void InitMenu();
//顾客的主菜单,主要是展示商品，购买商品等功能
void CustomerMenu();
//仓库管理员的主菜单
void AdministratorMenu();
//选择登录角色
void ChooseLoginCharacter(string &,bool &);
//顾客登录页面
string CustormerLogin(void);
//管理员的登录页面
string AdministratorLogin(void);
//管理员选择操作的主函数
void AdministratorOperation();
//管理员添加商品的函数
void AddGoodsInfo();
//管理员修改商品信息的函数
void EditGoodsInfo();
//管理员删除商品的函数
void DeleteGoodsInfo();
//管理员查询商品的主函数
void SelectGoodsMain();



//主函数
int main()
{
    //连接数据库
    ConnectDatabase();
    //设置背景颜色
    system("color f8");
    //进入系统初始化菜单
    InitMenu();
    //选择登录角色，进行登录操作
    ChooseLoginCharacter(username,isAdmin);
    if (username != "") {
        if (isAdmin == true) {  //管理员菜单
            system("cls");//清屏
            AdministratorMenu();
            AdministratorOperation();
        }
        else {  //顾客菜单
            system("cls");//清屏
            CustomerMenu();
        }
    }
    else {
        cout << "登录失败，请重新登录" << endl;
        username = "";
        isAdmin = false;
        ChooseLoginCharacter(username,isAdmin);
    }


    //QueryDatabase1();



    system("pause");
    return 0;
}

//连接数据库函数
bool ConnectDatabase()
{
    //初始化mysql  
    mysql_init(mysql);
    //返回false则连接失败，返回true则连接成功  
    if (!(mysql_real_connect(mysql, "localhost", "root", "root", "cms", 0, NULL, 0))) //中间分别是主机，用户名，密码，数据库名，端口号（可以写默认0或者3306等），可以先写成参数再传进去  
    {
        printf("Error connecting to database:%s\n", mysql_error(mysql));
        return false;
    }
    else
    {
        //数据库连接成功时不做任何提示
        return true;
    }
    return true;
}

bool QueryDatabase1()
{
    sprintf_s(query, "select * from users"); //执行查询语句，这里是查询所有，user是表名，不用加引号，用strcpy也可以  
    mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
    //返回0 查询成功，返回1查询失败  
    if (mysql_query(mysql, query))    //执行SQL语句
    {
        printf("Query failed (%s)\n", mysql_error(mysql));
        return false;
    }
    else
    {
        printf("query success\n");
    }
    //获取结果集  
    if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
    {
        printf("Couldn't get result from %s\n", mysql_error(mysql));
        return false;
    }

    //打印数据行数  
    printf("number of dataline returned: %d\n", mysql_affected_rows(mysql));

    //获取字段的信息  
    char* str_field[32];  //定义一个字符串数组存储字段信息  
    for (int i = 0; i < 4; i++)  //在已知字段数量的情况下获取字段名  
    {
        str_field[i] = mysql_fetch_field(res)->name;
    }
    for (int i = 0; i < 4; i++)  //打印字段  
        printf("%10s\t", str_field[i]);
    printf("\n");
    //打印获取的数据  
    while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行  
    {
        printf("%10s\t%10s\t%10s\t%10s\n", column[0], column[1], column[2], column[3]);  //column是列数组  
    }
    return true;
}
//初始化页面，选择登录角色
void InitMenu() {
    printf("***************【商品管理系统】**************\n");
    printf("*                                           *\n");
    printf("*               1.管理员登录                *\n");
    printf("*               2.顾客  登录                *\n");
    printf("*               0.退出  系统                *\n");
    printf("*                                           *\n");
    printf("*********************************************\n");
}
//顾客主页面
void CustomerMenu() {
    printf("☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆\n");
    printf("☆              欢迎使用商品管理系统(顾客端)                ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【查看商品列表】···(a)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【购 买  商 品】···(b)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【修 改  密 码】···(c)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【退 出  系 统】···(q)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆\n");
}

//管理员主页面
void AdministratorMenu() {
    printf("☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆\n");
    printf("☆              欢迎使用商品管理系统(管理员端)              ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【商 品  进 货】···(a)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【商 品  编 辑】···(b)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【商 品  删 除】···(b)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【商 品  查 询】···(c)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【营 收  统 计】···(d)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【订 单  管 理】···(e)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【修 改  密 码】···(f)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【退 出  系 统】···(q)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆\n");
}

//选择登录角色函数
void ChooseLoginCharacter(string &username,bool &isAdmin) {
    //定义一个输入的参数，判断是顾客登录还是管理员登录，或者是退出
    int WhoLogin;
    printf("请输入想要进行的操作，输入0退出系统:");
    while (cin >> WhoLogin)
    {
        //判断登录方式
        switch (WhoLogin) {
        case 0:
            //退出系统
            printf("您已成功退出系统!");
            system("pause");
            exit(0);
        case 2:
            username = CustormerLogin();
            break;
        case 1:
            username = AdministratorLogin();
            isAdmin = TRUE;
            break;
        default:
            printf("输入不合法，请重新输入:");
            break;
        }
        if (WhoLogin != NULL)
            break;
    }
}


//顾客登录页面
string CustormerLogin(void) {
    string username;
    string password;
    printf("请输入用户名:");
    cin >> username;
    printf("请输入密码:");
    char ch;
    //使用getch()获取输入的密码，并替换输出为*,做到保密的效果
    ch = _getch();
    while (ch != '\n' && ch != '\r')
    {
        password += ch;
        cout << "*";
        ch = _getch();
    }
    //执行查询语句，这里是查询有没有对应的用户，users是表名，不用加引号，用strcpy也可以
    //sprintf_s(query, "select * from users where username="+ "hhh" );
    username = username + "'";
    password = password + "'";
    sprintf_s(query, "%s%s%s%s", "select * from users where username='", username.c_str()," and password='",password.c_str());
    //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码 
    mysql_query(mysql, "set names gbk"); 
    //cout << query<<endl;
    //执行SQL语句
    mysql_query(mysql, query);
    //获取结果集  
    if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
    {
        printf("Couldn't get result from %s\n", mysql_error(mysql));
        return "";
    }
    while (column = mysql_fetch_row(res))
    {
        //printf("%10s\t\n", column[1]);
        return column[1];
    }
    return "";
}
//管理员登录页面
string AdministratorLogin(void) {
    string username;
    string password;
    printf("请输入用户名:");
    cin >> username;
    printf("请输入密码:");
    char ch;
    //使用getch()获取输入的密码，并替换输出为*,做到保密的效果
    ch = _getch();
    while (ch != '\n' && ch != '\r')
    {
        password += ch;
        cout << "*";
        ch = _getch();
    }
    //执行查询语句，这里是查询有没有对应的用户，users是表名，不用加引号，用strcpy也可以
    //sprintf_s(query, "select * from users where username="+ "hhh" );
    username = username + "'";
    password = password + "'";
    sprintf_s(query, "%s%s%s%s", "select * from users where username='", username.c_str(), " and password='", password.c_str());
    //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码 
    mysql_query(mysql, "set names gbk");
    //cout << query<<endl;
    //执行SQL语句
    mysql_query(mysql, query);
    //获取结果集  
    if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
    {
        printf("Couldn't get result from %s\n", mysql_error(mysql));
        return "";
    }
    while (column = mysql_fetch_row(res))
    {
        //printf("%10s\t\n", column[1]);
        return column[1];
    }
    return "";
}

//管理员选择操作的主函数
void AdministratorOperation() {
    printf("请输入您要进行的操作的编号:");
    //定义选择操作的标识
    char WitchOperationForAdmin;
    cin >> WitchOperationForAdmin;
    //根据用户输入，选择不同的操作
    switch (WitchOperationForAdmin)
    {
    case 'a':
        AddGoodsInfo();
        break;
    case 'b':
        EditGoodsInfo();
        break;
    case 'c':
        DeleteGoodsInfo();
        break;
    case 'q':
        exit(0);
        system("pause");
        break;
    default:
        break;
    }
}

//管理员添加商品的函数
void AddGoodsInfo() {
    //定义类型为Goods的商品
    Goods good;
    //定义是否继续添加的标志，y标识继续添加，n表示不继续添加
    char continueAdd = 'y';
    do {
        //是否已经输入类型的标志
        bool flag = false;
        cout<< "请选择商品类别:" <<endl;
        cout<< "1.食品 2.化妆品 3.日用品 4.饮料 5.文具" << endl;
        cout<< "请输入相应编号:";
        do {

        } while(flag == false);
    } while (continueAdd == 'y');

    cout << "☆☆☆☆☆☆☆☆☆☆☆☆商品信息的添加☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    

}

//管理员修改商品的函数
void EditGoodsInfo() {

}

//管理员删除商品的函数
void DeleteGoodsInfo() {

}

//管理员查询商品的主函数
void SelectGoodsMain() {

}
