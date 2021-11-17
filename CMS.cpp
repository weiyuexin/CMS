/*
*  基于C++、MySQL8开发的商品管理系统
*  author:韦月鑫、骆伟培、李娜、田悦琳
*/
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<Windows.h>
#include<WinSock.h>
#include<mysql.h>
#include<string.h>
#include<conio.h>
#include<ctime>
#include<stdio.h>
#include<iomanip>
#include "menu.h"

using namespace std;
#pragma comment(lib,"libmysql.lib")
#pragma comment(lib,"wsock32.lib")
MYSQL* mysql = new MYSQL; //mysql连接  
MYSQL_FIELD* fd;    //字段列数组  
char field[32][32];    //存字段名二维数组  
MYSQL_RES* res; //这个结构代表返回行的一个查询结果集  
MYSQL_ROW column; //一个行数据的类型安全(type-safe)的表示，表示数据行的列  
char query[150]; //查询语句  
//定义用户的用户名和密码
string username = "";
string password = "";
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
    int code;//商品编号
    string name;//商品名称
    string brand;//生产厂家
    double pur_price;//商品售价
    double price;//商品售价
    int num;//商品库存
    int salenum;//销量
    string type;//商品类别
    string date;//入库时间
};

//商品订单基本类型
struct OrderGoods//商品基本信息
{
    string code;//订单编号
    string username; //下单人
    string goodname; //商品名称
    int goodid;//商品编号
    int num; //该商品的购买数量
    double price;//售价
    double sum_mony;//单品总价
    int salesum;//商品销量，更新销量时要用到
    string date;//入库时间
    double sum_price;//订单总价
};


//连接数据库的函数
bool ConnectDatabase();
//顾客的主菜单,主要是展示商品，购买商品等功能
void CustomerMenu();
//仓库管理员的主菜单
void AdministratorMenu();
//选择登录角色
void ChooseLoginCharacter(string &,bool &);
//顾客登录页面
string CustormerLogin(void);
//顾客选择操作的主函数
void CustormerOperation();
//顾客浏览商品函数
void BrowseGoods();
//顾客购买商品函数
void BuyGoods();

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
//管理员选择订单操作方式函数
void SelectOrderOperation();
//管理员查询订单函数
void SelectOrder();
//管理员删除订单函数
void DeleteOrder();
//选择查询方式的函数
void SelectMethod();
//查询商品通过id
void SelectGoodsById();
//查询商品通过商品名称
void SelectGoodsByName();
//查询商品通过生产厂商
void SelectGoodsByBrand();
//查询商品通过商品类型
void SelectGoodsByType();
//查询商品通过价格排序
void SelectGoodsByPriceSort();
//查询商品通过销量排序
void SelectGoodsBySaleSort();
//营收统计菜单
void RevenueMenu();
//选择营收统计方式
void SelectRevenue();
//单个商品营收统计
void RevenueById();
//所有商品营收统计
void RevenueAll();
//管理员订单管理函数
void OrderManagementMenu();
//修改用户的密码函数
void ChangePassword();


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
    CustormerOperation();
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
    printf("☆               【商 品  删 除】···(c)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【商 品  查 询】···(d)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【营 收  统 计】···(e)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【订 单  管 理】···(f)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【修 改  密 码】···(g)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆               【退 出  系 统】···(q)                  ☆\n");
    printf("☆                                                          ☆\n");
    printf("☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆\n");
    AdministratorOperation();
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
    //string username;
    //string password
    printf("请输入顾客用户名:");
    cin >> username;
    printf("请输入顾客密码:");
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
    username = username;
    password = password;
    sprintf_s(query, "%s%s%s%s%s", "select * from users where username='", username.c_str(),"' and password='",password.c_str(),"'");
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
    //string username;
    printf("请输入管理员的用户名:");
    cin >> username;
    printf("请输入管理员密码:");
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
    username = username;
    password = password;
    sprintf_s(query, "%s%s%s%s%s", "select * from users where username='", username.c_str(), "' and password='", password.c_str() , "'");
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
        isAdmin = true;
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
        system("cls");
        AddGoodsInfo();//添加商品
        break;
    case 'b':
        system("cls");
        EditGoodsInfo();//修改商品
        break;
    case 'c':
        system("cls");
        DeleteGoodsInfo();//删除商品
        break;
    case 'd':
        system("cls");
        SelectGoodsMain();//查询商品
        break;
    case 'e':
        system("cls");
        RevenueMenu();//订单管理
        break;
    case 'f':
        system("cls");
        OrderManagementMenu();//订单管理
        break;
    case 'g':
        system("cls");
        ChangePassword();//修改密码
        break;
    case 'q':    //退出系统
        exit(0);
        system("pause");
        break;
    default:
        break;
    }
}

//顾客选择操作的主函数
void CustormerOperation() {
    printf("请输入您要进行的操作的编号:");
    //定义选择操作的标识
    char WitchOperationForAdmin;
    cin >> WitchOperationForAdmin;
    //根据用户输入，选择不同的操作
    switch (WitchOperationForAdmin)
    {
    case 'a':
        system("cls");
        BrowseGoods();
        break;
    case 'b':
        system("cls");
        BuyGoods();
        break;
    case 'c':
        system("cls");
        ChangePassword();
        break;
    case 'q':
        exit(0);
        system("pause");
        break;
    default:
        cout << "您的选择错误!按任意键继续!!!" << endl;
        system("pause");
        getchar();
        system("cls");
        CustomerMenu();
        break;
    }
}

//管理员添加商品的函数
void AddGoodsInfo() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆商品信息的添加☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    //定义类型为Goods的商品
    Goods good;
    //输入的类型id
    int typeId;
    //定义是否继续添加的标志，y标识继续添加，n表示不继续添加
    char continueAdd = 'y';
    do {
        //是否已经输入类型的标志
        bool flag = false;
        cout<< "请选择商品类别:" <<endl;
        cout<< "1.食品 2.化妆品 3.日用品 4.饮料 5.文具" << endl;
        cout<< "请输入相应的类型编号:";
        do {
            cin >> typeId;
            //判断输入的类型编号是否存在
            if (typeId >= 1 && typeId <= 5) {
                flag = true;
            }
            else {
                cout << "您选择的类型编号不存在!" << endl;
                cout << "请选择正确的商品类型编号：" << endl;
            }
            if (typeId == 1)
                good.type = "食品";
            if (typeId == 2)
                good.type = "化妆品";
            if (typeId == 3)
                good.type = "日用品";
            if (typeId == 4)
                good.type = "饮料";
            if (typeId == 5)
                good.type = "文具";
            printf("您选择的商品类型是(%s)\n",good.type.c_str());

            cout << "请输入商品编号:" << endl;
            cin >> good.code;
            cout << "请输入商品名称:" << endl;
            cin >> good.name;
            cout << "请输入商品生产厂家:" << endl;
            cin >> good.brand;
            cout << "请输入商品进价:" << endl;
            cin >> good.pur_price;
            cout << "请输入商品售价:" << endl;
            cin >> good.price;
            cout << "请输入商品数量:" << endl;
            cin >> good.num;
            cout << "请输入商品入库时间(格式：year/mon/day):" << endl;
            cin >> good.date;

           /* cout << good.code << endl;
            cout << good.name.c_str() << endl;
            cout << good.brand.c_str() << endl;
            cout << good.pur_price << endl;
            cout << good.price << endl;
            cout << good.type.c_str() << endl;*/

            char insert[150];//数据库插入语句
            mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
            sprintf_s(insert, "%s%d%s%s%s%s%s%lf%s%lf%s%s%s%d%s%s%s", "insert into goods values(",good.code , ",'" , good.name.c_str() ,"','",good.brand.c_str(),"',",good.pur_price,",",good.price,",'",good.type.c_str(),"',",good.num,",0,'",good.date.c_str(),"')");
            //cout << insert << "\n"; 
            if (mysql_query(mysql, insert))    //执行SQL语句
            {
                cout << "商品添加失败！！！想继续添加吗(y/n):";
            }
            else {
                cout << "商品添加成功！！！想继续添加吗(y/n):";
            }
            


            cin >> continueAdd;
            while (continueAdd != 'y' && continueAdd != 'n')
            {
                cout << "指令错误！！！！！<请输入y/n>" << endl;
                //cout << "数据添加成功！！！想继续输入吗(y/n):";
                cin >> continueAdd;
            }
            

        } while(flag == false);
    } while (continueAdd == 'y');
    cout << endl;
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    AdministratorMenu();//管理员主页面
}

//管理员修改商品的函数
void EditGoodsInfo() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆商 品 信 息 的 修 改☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    int id;//要修改的商品的id
    char flag = 'n';//是否继续的标志
    Goods good;
    do {//执行查询操作
        cout << "请输入要修改的商品的编号:" << endl;
        cin >> id;
        char selectsql[150];//数据库操作语句
        mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
        sprintf_s(selectsql, "%s%d", "select * from goods where id=", id);
        if (mysql_query(mysql, selectsql))    //执行SQL语句
        {
            cout << "该商品不存在！！！想继续查询吗(y/n):";
        }
        else {
            //获取结果集  
            if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
            {
                printf("Couldn't get result from %s\n", mysql_error(mysql));
            }
            //是否有数据的标志
            bool hasData = false;
            //打印获取的数据
            while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行  
            {
                hasData = true;
                good.code = atoi(column[0]);//char转换成int
                good.name = column[1];
                good.brand = column[2];
                good.pur_price = strtod(column[3], NULL);
                good.price = strtod(column[4], NULL);
                good.type = column[5];
                good.num = atoi(column[6]);
                good.salenum = atoi(column[7]);
                good.date = column[8];

                cout << "要修改的商品当前的详细信息为：" << endl;
                cout << setiosflags(ios::left) << setw(10) << "编号" << setw(16) << "商品名称" << setw(15)
                    << "生产厂家" << setw(10) << "商品类别" << setw(10) << "进价" << setw(10) <<
                    "售价" << setw(10) << "库存" << setw(10) << "销量" << setw(10) << "入库时间" << endl;
                cout << setiosflags(ios::left) << setw(10) << good.code << setw(16) << good.name << setw(15)
                    << good.brand << setw(10) << good.type << setw(10) << good.pur_price << setw(10) <<
                    good.price << setw(10) << good.num << setw(10) << good.salenum << setw(10) << good.date << endl;

                int which_update;//定义一个变量，表示要修改的选项
                char continue_update = 'y';
                do {
                    cout << "请输入想要修改的内容对应的编号(1:商品名称   2:商品厂商    3:售价    4:进价    5:库存):" << endl;
                    cin >> which_update;
                    switch (which_update) {
                    case 0:
                        cout << "请输入修改后的编号:";
                        cin >> good.code;
                        break;
                    case 1:
                        cout << "请输入修改后的商品名称:";
                        cin >> good.name;
                        break;
                    case 2:
                        cout << "请输入修改后的生产厂商:";
                        cin >> good.brand;
                        break;
                    case 3:
                        cout << "请输入修改后的售价:";
                        cin >> good.price;
                        break;
                    case 4:
                        cout << "请输入修改后的进价:";
                        cin >> good.pur_price;
                        break;
                    case 5:
                        cout << "请输入修改后的库存:";
                        cin >> good.num;
                        break;
                    default:
                        cout << "输入错误";
                        break;
                    }
                    cout << "是否继续修改其他项(y/n)：";
                    cin >> continue_update;
                    if (continue_update != 'y' && continue_update != 'n') {
                        cout << "您输入的选择不正确，请输入选择(y/n):";
                    }
                } while (continue_update == 'y');

                //更新数据库前，询问是否确认修改
                char flag;
                cout << "您确认要保存以上修改吗?(y/n):";
                cin >> flag;
                if (flag == 'y') {//进行修改操作
                    char update[150];//数据库插入语句
                    mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
                    sprintf_s(update, "%s%s%s%s%s%lf%s%lf%s%d%s%d%s%d", "update goods set name='", good.name.c_str(), "',brand='", good.brand.c_str(), "',purprice=", good.pur_price, ",saleprice=", good.price, ",num=", good.num, ",salesnum=", good.salenum, " where id=",id);
                   // cout << update << "\n";
                    if (mysql_query(mysql, update))    //执行SQL语句
                    {
                        cout << "商品更新失败！！！想继续修改吗(y/n):";
                    }
                    else {
                        cout << "商品更新成功！！！想继续修改吗(y/n):";
                    }

                }
                else if (flag == 'n') {
                    cout << "您上述修改未保存!" << endl;
                }

                //cout << "想要继续修改其他商品吗(y/n):";
            }
            if (hasData == false) {
                cout << "该商品不存在！！！想继续修改其他吗(y/n):";
            }

        }
        cin >> flag;
        while (flag != 'y' && flag != 'n')
        {
            cout << "指令错误！！！！！<请输入y/n>" << endl;
            cin >> flag;
        }
    } while (flag == 'y');
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    AdministratorMenu();//选择查询方式
}

//管理员删除商品的函数
void DeleteGoodsInfo() {
    cout << endl;
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆商品信息的删除☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    int id;//要删除的商品的编号
    char flag = 'n';//是否继续的标志
    do {//执行删除操作
        cout << "请输入要删除的商品的编号:" << endl;
        cin >> id;
        char queren = 'n';
        printf("确认要删除编号为%d的商品吗(y/n):", id);
        cin >> queren;
        if (queren == 'y') {
            char deletesql[150];//数据库插入语句
            mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
            sprintf_s(deletesql, "%s%d", "delete from goods where id=", id);
            //cout << insert << "\n"; 
            if (mysql_query(mysql, deletesql))    //执行SQL语句
            {
                cout << "商品删除失败！！！想继续删除吗(y/n):";
            }
            else {
                cout << "商品删除成功！！！想继续删除吗(y/n):";
            }
        }
        
        cin >> flag;
        while (flag != 'y' && flag != 'n')
        {
            cout << "指令错误！！！！！<请输入y/n>" << endl;
            cin >> flag;
        }
    } while (flag == 'y');
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    AdministratorMenu();//管理员主页面
}

//管理员查询商品的主函数
void SelectGoodsMain() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆查询商品信息☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    cout << "☆                                                          ☆" << endl;
    cout << "☆                 【按照编号查询】···(a)                ☆" << endl;
    cout << "☆                 【按照名称查询】···(b)                ☆" << endl;
    cout << "☆                 【按照厂商查询】···(c)                ☆" << endl;
    cout << "☆                 【按照类型查询】···(d)                ☆" << endl;
    cout << "☆                 【按照售价排序】···(e)                ☆" << endl;
    cout << "☆                 【按照销量排序】···(f)                ☆" << endl;
    cout << "☆                 【退 出  系 统】···(q)                ☆" << endl;
    cout << "☆                                                          ☆" << endl;
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    SelectMethod();
}

//选择查询方式的函数
void SelectMethod() {
    cout << "请输入您想要进行的查询操作方式:" <<endl;
    char whitch_method;
    cin >> whitch_method;
    switch (whitch_method) {
    case 'q':
        system("cls");//清屏
        AdministratorMenu();
        break;
    case'a' :
        system("cls");//清屏
        SelectGoodsById();
        break;
    case'b':
        system("cls");//清屏
        SelectGoodsByName();
        break;
    case'c':
        system("cls");//清屏
        SelectGoodsByBrand();
        break;
    case'd':
        system("cls");//清屏
        SelectGoodsByType();
        break;
    case'e':
        system("cls");//清屏
        SelectGoodsByPriceSort();
        break;
    case'f':
        system("cls");//清屏
        SelectGoodsBySaleSort();
        break;
    default:
        break;
    }
}

//查询商品通过id
void SelectGoodsById() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆通 过 编 号 查 询 商 品☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    Goods good;//存放返回的单条数据
    int id;//要查询的商品的编号
    char flag = 'n';//是否继续的标志
    do {//执行查询操作
        cout << "请输入要查询的商品的编号:" ;
        cin >> id;
        char selectsql[150];//数据库插入语句
        mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
        sprintf_s(selectsql, "%s%d", "select * from goods where id=", id);
        if (mysql_query(mysql, selectsql))    //执行SQL语句
        {
            cout << "该商品不存在！！！想继续查询吗(y/n):";
        }
        else {
            //获取结果集  
            if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
            {
                printf("Couldn't get result from %s\n", mysql_error(mysql));
            }
            //是否有数据的标志
            bool hasData = false;
            //打印获取的数据
            while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行  
            {
                hasData = true;
                good.code = atoi(column[0]);//char转换成int
                good.name = column[1];
                good.brand = column[2];
                good.pur_price = strtod(column[3],NULL);
                good.price = strtod(column[4], NULL);
                good.type = column[5];
                good.num = atoi(column[6]);
                good.salenum = atoi(column[7]);
                good.date = column[8];
              
                cout << "商品详细信息为：" << endl;
                cout << setiosflags(ios::left) << setw(10) << "编号" << setw(16) << "商品名称" << setw(15) 
                    << "生产厂家" << setw(10) << "商品类别"<< setw(10)<<"进价" << setw(10) <<
                    "售价" << setw(10) << "库存" << setw(10) << "销量" << setw(10) << "入库时间" << endl;
                cout << setiosflags(ios::left) << setw(10) << good.code << setw(16) << good.name << setw(15)
                    << good.brand << setw(10) << good.type << setw(10) << good.pur_price << setw(10) <<
                    good.price << setw(10) << good.num << setw(10) << good.salenum << setw(10) << good.date << endl;
                cout << "想要继续查询吗(y/n):";
            }
            if (hasData == false) {
                cout << "该商品不存在！！！想继续查询吗(y/n):";
            }
            
        }
        cin >> flag;
        while (flag != 'y' && flag != 'n')
        {
            cout << "指令错误！！！！！<请输入y/n>" << endl;
            cin >> flag;
        }
    } while (flag == 'y');
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    SelectGoodsMain();//选择查询方式
}

//查询商品通过商品名称
void SelectGoodsByName() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆通 过 商 品 名 称 查 询 商 品☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    Goods good;//存放返回的单条数据
    int sum = 0;//查询到的数据的总量
    string name;//要查询的商品的编号
    char flag = 'n';//是否继续的标志
    do {//执行查询操作
        cout << "请输入要查询的商品的名称:";
        cin >> name;
        char selectsql[150];//数据库插入语句
        mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
        sprintf_s(selectsql, "%s%s%s", "select * from goods where name='", name.c_str(),"'");
        //cout << selectsql << endl;
        if (mysql_query(mysql, selectsql))    //执行SQL语句
        {
            cout << "该商品不存在！！！想继续查询吗(y/n):";
        }
        else {
            //获取结果集  
            if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
            {
                printf("Couldn't get result from %s\n", mysql_error(mysql));
            }
            //是否有数据的标志
            bool hasData = false;
            //打印获取的数据
            bool has_out = false;
            while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行  
            {
                hasData = true;
                if (has_out == false) {
                    has_out = true;
                    cout << "商品详细信息为：" << endl;
                    cout << setiosflags(ios::left) << setw(10) << "编号" << setw(16) << "商品名称" << setw(15)
                        << "生产厂家" << setw(10) << "商品类别" << setw(10) << "进价" << setw(10) <<
                        "售价" << setw(10) << "库存" << setw(10) << "销量" << setw(10) << "入库时间" << endl;
                }
                //sum = atoi(column[9]);//获取查询到的数据条数
                //cout << sum;

                good.code = atoi(column[0]);//char转换成int
                good.name = column[1];
                good.brand = column[2];
                good.pur_price = strtod(column[3], NULL);
                good.price = strtod(column[4], NULL);
                good.type = column[5];
                good.num = atoi(column[6]);
                good.salenum = atoi(column[7]);
                good.date = column[8];

              
                cout << setiosflags(ios::left) << setw(10) << good.code << setw(16) << good.name << setw(15)
                    << good.brand << setw(10) << good.type << setw(10) << good.pur_price << setw(10) <<
                    good.price << setw(10) << good.num << setw(10) << good.salenum << setw(10) << good.date << endl;

            }
            if (hasData == false) {
                cout << "该商品不存在！！！想继续查询吗(y/n):";
            }
            else {
                cout << "想要继续查询吗(y/n):";
            }

        }
        cin >> flag;
        while (flag != 'y' && flag != 'n')
        {
            cout << "指令错误！！！！！<请输入y/n>" << endl;
            cin >> flag;
        }
    } while (flag == 'y');
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    SelectGoodsMain();//选择查询方式
}

//查询商品通过生产厂商
void SelectGoodsByBrand() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆通 过 商 品 厂 商 查 询 商 品☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    Goods good;//存放返回的单条数据
    int sum = 0;//查询到的数据的总量
    string brand;//要查询的商品的生产厂商
    char flag = 'n';//是否继续的标志
    do {//执行查询操作
        cout << "请输入要查询的商品的生产厂商:";
        cin >> brand;
        char selectsql[150];//数据库插入语句
        mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
        sprintf_s(selectsql, "%s%s%s", "select * from goods where brand='", brand.c_str(), "'");
        //cout << selectsql << endl;
        if (mysql_query(mysql, selectsql))    //执行SQL语句
        {
            cout << "该厂商没有相关的商品！！！想继续查询吗(y/n):";
        }
        else {
            //获取结果集  
            if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
            {
                printf("Couldn't get result from %s\n", mysql_error(mysql));
            }
            //是否有数据的标志
            bool hasData = false;
            //打印获取的数据
            bool has_out = false;
            while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行  
            {
                hasData = true;
                if (has_out == false) {
                    has_out = true;
                    cout << "商品详细信息为：" << endl;
                    cout << setiosflags(ios::left) << setw(10) << "编号" << setw(16) << "商品名称" << setw(15)
                        << "生产厂家" << setw(10) << "商品类别" << setw(10) << "进价" << setw(10) <<
                        "售价" << setw(10) << "库存" << setw(10) << "销量" << setw(10) << "入库时间" << endl;
                }
                //sum = atoi(column[9]);//获取查询到的数据条数
                //cout << sum;

                good.code = atoi(column[0]);//char转换成int
                good.name = column[1];
                good.brand = column[2];
                good.pur_price = strtod(column[3], NULL);
                good.price = strtod(column[4], NULL);
                good.type = column[5];
                good.num = atoi(column[6]);
                good.salenum = atoi(column[7]);
                good.date = column[8];


                cout << setiosflags(ios::left) << setw(10) << good.code << setw(16) << good.name << setw(15)
                    << good.brand << setw(10) << good.type << setw(10) << good.pur_price << setw(10) <<
                    good.price << setw(10) << good.num << setw(10) << good.salenum << setw(10) << good.date << endl;

            }
            if (hasData == false) {
                cout << "该厂商没有相关的商品！！！想继续查询吗(y/n):";
            }
            else {
                cout << "想要继续查询吗(y/n):";
            }

        }
        cin >> flag;
        while (flag != 'y' && flag != 'n')
        {
            cout << "指令错误！！！！！<请输入y/n>" << endl;
            cin >> flag;
        }
    } while (flag == 'y');
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    SelectGoodsMain();//选择查询方式
}

//查询商品通过商品类型
void SelectGoodsByType() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆通 过 商 品 厂 商 查 询 商 品☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    Goods good;//存放返回的单条数据
    int sum = 0;//查询到的数据的总量
    string type;//要查询的商品的编号
    char flag = 'n';//是否继续的标志
    do {//执行查询操作
        cout << "请输入要查询的商品的类型:";
        cin >> type;
        char selectsql[150];//数据库插入语句
        mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
        sprintf_s(selectsql, "%s%s%s", "select * from goods where type='", type.c_str(), "'");
        //cout << selectsql << endl;
        if (mysql_query(mysql, selectsql))    //执行SQL语句
        {
            cout << "该类型没有相关的商品！！！想继续查询吗(y/n):";
        }
        else {
            //获取结果集  
            if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
            {
                printf("Couldn't get result from %s\n", mysql_error(mysql));
            }
            //是否有数据的标志
            bool hasData = false;
            //打印获取的数据
            bool has_out = false;
            while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行  
            {
                hasData = true;
                if (has_out == false) {
                    has_out = true;
                    cout << "商品详细信息为：" << endl;
                    cout << setiosflags(ios::left) << setw(10) << "编号" << setw(16) << "商品名称" << setw(15)
                        << "生产厂家" << setw(10) << "商品类别" << setw(10) << "进价" << setw(10) <<
                        "售价" << setw(10) << "库存" << setw(10) << "销量" << setw(10) << "入库时间" << endl;
                }
                //sum = atoi(column[9]);//获取查询到的数据条数
                //cout << sum;

                good.code = atoi(column[0]);//char转换成int
                good.name = column[1];
                good.brand = column[2];
                good.pur_price = strtod(column[3], NULL);
                good.price = strtod(column[4], NULL);
                good.type = column[5];
                good.num = atoi(column[6]);
                good.salenum = atoi(column[7]);
                good.date = column[8];


                cout << setiosflags(ios::left) << setw(10) << good.code << setw(16) << good.name << setw(15)
                    << good.brand << setw(10) << good.type << setw(10) << good.pur_price << setw(10) <<
                    good.price << setw(10) << good.num << setw(10) << good.salenum << setw(10) << good.date << endl;

            }
            if (hasData == false) {
                cout << "该类型没有相关的商品！！！想继续查询吗(y/n):";
            }
            else {
                cout << "想要继续查询吗(y/n):";
            }

        }
        cin >> flag;
        while (flag != 'y' && flag != 'n')
        {
            cout << "指令错误！！！！！<请输入y/n>" << endl;
            cin >> flag;
        }
    } while (flag == 'y');
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    SelectGoodsMain();//选择查询方式
}

//查询商品通过价格排序
void SelectGoodsByPriceSort() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆按 照 商 品 价 格 排 序☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    Goods good;//存放返回的单条数据
    int sum = 0;//查询到的数据的总量
    int type;//排序规则
    char flag = 'n';//是否继续的标志
    do {//执行查询操作
        cout << "请选择排序规则（1:升序     2:降序）:";
        cin >> type;
        char selectsql[150];//数据库插入语句
        mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
        if (type == 1) {//排序规则
            sprintf_s(selectsql, "select * from goods order by saleprice");
        }
        else if (type == 2) {
            sprintf_s(selectsql, "select * from goods order by saleprice desc");
        }
        //cout << selectsql << endl;
        if (mysql_query(mysql, selectsql))    //执行SQL语句
        {
            cout << "没有相关的商品！！！想继续查询吗(y/n):";
        }
        else {
            //获取结果集  
            if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
            {
                printf("Couldn't get result from %s\n", mysql_error(mysql));
            }
            //是否有数据的标志
            bool hasData = false;
            //打印获取的数据
            bool has_out = false;
            while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行  
            {
                hasData = true;
                if (has_out == false) {
                    has_out = true;
                    if (type == 1) {//排序规则
                        cout << "按照价格升序排序的结果如下：" << endl;
                    }
                    else if (type == 2) {
                        cout << "按照价格降序排序的结果如下：" << endl;
                    }
                    cout << setiosflags(ios::left) << setw(10) << "编号" << setw(16) << "商品名称" << setw(15)
                        << "生产厂家" << setw(10) << "商品类别" << setw(10) << "进价" << setw(10) <<
                        "售价" << setw(10) << "库存" << setw(10) << "销量" << setw(10) << "入库时间" << endl;
                }
                //sum = atoi(column[9]);//获取查询到的数据条数
                //cout << sum;

                good.code = atoi(column[0]);//char转换成int
                good.name = column[1];
                good.brand = column[2];
                good.pur_price = strtod(column[3], NULL);
                good.price = strtod(column[4], NULL);
                good.type = column[5];
                good.num = atoi(column[6]);
                good.salenum = atoi(column[7]);
                good.date = column[8];


                cout << setiosflags(ios::left) << setw(10) << good.code << setw(16) << good.name << setw(15)
                    << good.brand << setw(10) << good.type << setw(10) << good.pur_price << setw(10) <<
                    good.price << setw(10) << good.num << setw(10) << good.salenum << setw(10) << good.date << endl;

            }
            if (hasData == false) {
                cout << "没有相关的商品！！！想继续查询吗(y/n):";
            }
            else {
                cout << "想要继续查询吗(y/n):";
            }

        }
        cin >> flag;
        while (flag != 'y' && flag != 'n')
        {
            cout << "指令错误！！！！！<请输入y/n>" << endl;
            cin >> flag;
        }
    } while (flag == 'y');
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    SelectGoodsMain();//选择查询方式
}

//查询商品通过销量排序
void SelectGoodsBySaleSort() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆按 照 商 品 销 量 排 序☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    Goods good;//存放返回的单条数据
    int sum = 0;//查询到的数据的总量
    int type;//排序规则
    char flag = 'n';//是否继续的标志
    do {//执行查询操作
        cout << "请选择排序规则（1:升序     2:降序）:";
        cin >> type;
        char selectsql[150];//数据库插入语句
        mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
        if (type == 1) {//排序规则
            sprintf_s(selectsql, "select * from goods order by salesnum");
        }
        else if (type == 2) {
            sprintf_s(selectsql, "select * from goods order by salesnum desc");
        }
        //cout << selectsql << endl;
        if (mysql_query(mysql, selectsql))    //执行SQL语句
        {
            cout << "没有相关的商品！！！想继续查询吗(y/n):";
        }
        else {
            //获取结果集  
            if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
            {
                printf("Couldn't get result from %s\n", mysql_error(mysql));
            }
            //是否有数据的标志
            bool hasData = false;
            //打印获取的数据
            bool has_out = false;
            while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行  
            {
                hasData = true;
                if (has_out == false) {
                    has_out = true;
                    if (type == 1) {//排序规则
                        cout << "按照销量升序排序的结果如下：" << endl;
                    }
                    else if (type == 2) {
                        cout << "按照销量降序排序的结果如下：" << endl;
                    }
                    cout << setiosflags(ios::left) << setw(10) << "编号" << setw(16) << "商品名称" << setw(15)
                        << "生产厂家" << setw(10) << "商品类别" << setw(10) << "进价" << setw(10) <<
                        "售价" << setw(10) << "库存" << setw(10) << "销量" << setw(10) << "入库时间" << endl;
                }
                //sum = atoi(column[9]);//获取查询到的数据条数
                //cout << sum;

                good.code = atoi(column[0]);//char转换成int
                good.name = column[1];
                good.brand = column[2];
                good.pur_price = strtod(column[3], NULL);
                good.price = strtod(column[4], NULL);
                good.type = column[5];
                good.num = atoi(column[6]);
                good.salenum = atoi(column[7]);
                good.date = column[8];


                cout << setiosflags(ios::left) << setw(10) << good.code << setw(16) << good.name << setw(15)
                    << good.brand << setw(10) << good.type << setw(10) << good.pur_price << setw(10) <<
                    good.price << setw(10) << good.num << setw(10) << good.salenum << setw(10) << good.date << endl;

            }
            if (hasData == false) {
                cout << "没有相关的商品！！！想继续查询吗(y/n):";
            }
            else {
                cout << "想要继续查询吗(y/n):";
            }

        }
        cin >> flag;
        while (flag != 'y' && flag != 'n')
        {
            cout << "指令错误！！！！！<请输入y/n>" << endl;
            cin >> flag;
        }
    } while (flag == 'y');
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    SelectGoodsMain();//选择查询方式
}

//管理员订单管理主函数
void OrderManagementMenu() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆订 单 管 理☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    cout << "☆                                                                                  ☆" << endl;
    cout << "☆                              【查 询 订 单】···(a)                            ☆" << endl;
    cout << "☆                                                                                  ☆" << endl;
    cout << "☆                              【删 除 订 单】···(b)                            ☆" << endl;
    cout << "☆                                                                                  ☆" << endl;
    cout << "☆                              【返       回】···(q)                            ☆" << endl;
    cout << "☆                                                                                  ☆" << endl;
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    SelectOrderOperation();//选择操作
}

//管理员选择订单操作函数
void SelectOrderOperation() {
    char OrderOperation; //订单操作方式选择
    char flag = 'y';   //是否继续选择其他操作
    do {
        cout << "请选择您要进行的订单操作方式(输入q返回上一级):";
        cin >> OrderOperation;
        switch (OrderOperation) {
        case 'a':
            system("cls");
            SelectOrder();
            break;
        case 'b':
            system("cls");
            DeleteOrder();
            break;
        case 'q':
            system("cls");
            AdministratorMenu();
            break;
        default:
            cout << "选择错误，是否继续(y/n)：" << endl;
            cin >> flag;
            break;
        }
    } while (flag == 'y');
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    AdministratorMenu();//选择查询方式
}

//管理员查询订单函数
void SelectOrder() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆按 照 商 品 销 量 排 序☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    OrderGoods good;//存放返回的订单的单条数据
    int sum = 0;//查询到的数据的总量
    char selectsql[150];//数据库查询语句
    mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
    sprintf_s(selectsql, "select * from indent");
    if (mysql_query(mysql, selectsql))    //执行SQL语句
    {
        cout << "没有相关的商品！！！想继续查询吗(y/n):";
    }
    else {
        //获取结果集  
        if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
        {
            printf("Couldn't get result from %s\n", mysql_error(mysql));
        }
        //是否有数据的标志
        bool hasData = false;
        //打印获取的数据
        bool has_out = false;
        while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行  
        {
            hasData = true;
            if (has_out == false) {
                has_out = true;
                cout << "所以订单如下：" << endl;
                cout << setiosflags(ios::left) << setw(10) << "订单编号" << setw(10) << "下单人" << setw(15)
                    << "商品编号" << setw(10) << "商品名称" << setw(10) << "售价" << setw(10) <<
                    "数量" << setw(10) << "订单总价"  << endl;
            }
            good.code = column[0];
            good.username = column[1];
            good.goodid = atoi(column[2]);
            good.goodname = column[3];
            good.price = strtod(column[4], NULL);
            good.num = atoi(column[5]);
            good.sum_price = strtod(column[6],NULL);

            //cout << good.code;
            cout << setiosflags(ios::left) << setw(10) << good.code << setw(10) << good.username << setw(15)
                    << good.goodid << setw(10) << good.goodname << setw(10) << good.price << setw(10) <<
                    good.num << setw(10) << good.sum_price << endl;
        }
    }
    cout << endl;
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    OrderManagementMenu();//选择查询方式
}

//管理员删除订单函数
void DeleteOrder() {
    cout << endl;
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆ 删 除 订 单 ☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    string id;//要删除的商品的编号
    char flag = 'n';//是否继续的标志
    do {//执行删除操作
        cout << "请输入要删除的订单的编号:" << endl;
        cin >> id;
        char queren = 'n';
        printf("确认要删除编号为%s的订单吗(y/n):", id.c_str());
        cin >> queren;
        if (queren == 'y') {
            char deletesql[150];//数据库插入语句
            mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
            sprintf_s(deletesql, "%s%s", "delete from indent where id=", id.c_str());
            //cout << insert << "\n"; 
            if (mysql_query(mysql, deletesql))    //执行SQL语句
            {
                cout << "订单删除失败！！！想继续删除吗(y/n):";
            }
            else {
                cout << "订单删除成功！！！想继续删除吗(y/n):";
            }
        }

        cin >> flag;
        while (flag != 'y' && flag != 'n')
        {
            cout << "指令错误！！！！！<请输入y/n>" << endl;
            cin >> flag;
        }
    } while (flag == 'y');
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    AdministratorMenu();//管理员主页面
}

//修改用户的密码函数
void ChangePassword() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆修 改 密 码☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    string old_password;
    string new_password1;
    string new_password2;
    char flag = 'y';//是否重试
    do {
        cout << "请输入旧密码:";
        char ch;
        //使用getch()获取输入的密码，并替换输出为*,做到保密的效果
        ch = _getch();
        while (ch != '\n' && ch != '\r')
        {
            old_password += ch;
            cout << "*";
            ch = _getch();
        }
        cout << endl;
        if (old_password == password) {//判断输入的旧密码是否正确
            cout << "请输入新密码:";
            //使用getch()获取输入的密码，并替换输出为*,做到保密的效果
            ch = _getch();
            while (ch != '\n' && ch != '\r')
            {
                new_password1 += ch;
                cout << "*";
                ch = _getch();
            }
            cout << "\n请重新输入新密码:";
            //使用getch()获取输入的密码，并替换输出为*,做到保密的效果
            ch = _getch();
            while (ch != '\n' && ch != '\r')
            {
                new_password2 += ch;
                cout << "*";
                ch = _getch();
            }
            cout << endl;

            //判断两次输入的密码是否相同
            if (new_password1 == new_password2) {
                char update[150];//数据库插入语句
                mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
                sprintf_s(update, "%s%s%s%s%s", "update users set password='", new_password1.c_str(), "' where username='", username.c_str(), "'");
                // cout << update << "\n";
                if (mysql_query(mysql, update))    //执行SQL语句
                {
                    cout << "密码修改失败，是否重试(y/n):";
                    cin >> flag;
                }
                else {
                    cout << "密码修改成功，下次登录时请使用新密码!" << endl;
                    flag = 'n';
                }
            }
            else {
                cout << "两次输入的密码不相同,是否重试(y/n):";
                cin >> flag;
            }
        }
        else {
            cout << "原密码错误，是否重试(y/n):";
            cin >> flag;
        }
        old_password = "";
        new_password1 = "";
        new_password2 = "";
    } while (flag == 'y');

    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    if (isAdmin == true) {
        AdministratorMenu();//选择查询方式
    }
    else {
        CustomerMenu();
    }
}

//顾客浏览商品函数
void BrowseGoods() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆ 商 品 列 表 ☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    Goods good;
    char selectsql[150];//数据库插入语句
    mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
    sprintf_s(selectsql, "%s", "select * from goods");
    //cout << selectsql << endl;
    if (mysql_query(mysql, selectsql))    //执行SQL语句
    {
        cout << "商品列表为空！！！";
    }
    else {
        //获取结果集  
        if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
        {
            printf("Couldn't get result from %s\n", mysql_error(mysql));
        }
        //是否有数据的标志
        bool hasData = false;
        //打印获取的数据
        bool has_out = false;
        while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行  
        {
            hasData = true;
            if (has_out == false) {
                has_out = true;
                cout << "所以商品信息如下：\n" << endl;
                cout << setiosflags(ios::left) << setw(10) << "编号" << setw(16) << "商品名称" << setw(15)
                    << "生产厂家" << setw(10) << "商品类别"  << setw(10) <<
                    "售价" << setw(10) << "库存" << setw(10) << "销量" << setw(10) << "入库时间" << endl;
            }
            //sum = atoi(column[9]);//获取查询到的数据条数
            //cout << sum;

            good.code = atoi(column[0]);//char转换成int
            good.name = column[1];
            good.brand = column[2];
            good.pur_price = strtod(column[3], NULL);
            good.price = strtod(column[4], NULL);
            good.type = column[5];
            good.num = atoi(column[6]);
            good.salenum = atoi(column[7]);
            good.date = column[8];


            cout << setiosflags(ios::left) << setw(10) << good.code << setw(16) << good.name << setw(15)
                << good.brand << setw(10) << good.type << setw(10) <<
                good.price << setw(10) << good.num << setw(10) << good.salenum << setw(10) << good.date << endl;

        }
    }

    cout << "\n……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    CustomerMenu();//返回顾客主菜单
}

//顾客购买商品函数
void BuyGoods() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆ 购 买 商 品 ☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    OrderGoods good[10];//保存订单信息
    int sum=0;//购买商品的种类数
    double total_price = 0;//所购买的商品的总价
    char is_continue_buy = 'y';//是否继续下单的标识
    char flag = 'y';//是否继续添加商品的标识
    do {
        // 基于当前系统的当前日期/时间
        time_t now = time(0);
        do {
            sum++;
            cout << "请输入要购买的商品的编号:";
            cin >> good[sum].goodid;
            cout << endl;
            cout << "请输入要购买的数量:";
            cin >> good[sum].num;
            cout << endl;

            char selectsql[150];//数据库插入语句
            mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
            sprintf_s(selectsql, "%s%d", "select * from goods where id=", good[sum].goodid);
            if (mysql_query(mysql, selectsql))    //执行SQL语句
            {
                cout << "该商品不存在！！！想继续查询吗(y/n):";
            }
            else {
                //获取结果集  
                if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
                {
                    printf("Couldn't get result from %s\n", mysql_error(mysql));
                }
                //打印获取的数据
                while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行  
                {
                    good[sum].code = now;
                    good[sum].goodname = column[1];
                    good[sum].price = strtod(column[4], NULL);
                    good[sum].sum_mony = good[sum].price * good[sum].num;
                    good[sum].salesum = atoi(column[7])+ good[sum].num;//更新后的销量
                    total_price += good[sum].sum_mony;
                }
            }
            cout << "是否继续添加商品(y/n):";
            cin >> flag;
        } while (flag == 'y');
        cout << "当前订单的信息如下:" << endl;
        cout << setiosflags(ios::left) << setw(10) << "商品编号" << setw(10) << "商品名称" << setw(10) << "购买数量" << setw(10) << "合计(元)" << endl;
        for (int i = 1; i <= sum; i++) {
            cout << setiosflags(ios::left) << setw(10) << good[i].goodid << setw(10) << good[i].goodname << setw(10) << good[i].num << setw(10) << good[i].sum_mony << endl;
        }
        cout << setiosflags(ios::left) << setw(10) << " " << setw(10) << "" << setw(10) << "" << setw(10) << total_price << endl;

        char isok = 'y';
        cout << "确定要下单吗(y/n):";
        cin >> isok;
        if (isok == 'y') {
            //保存到数据库
            for (int i = 1; i <= sum; i++) {
                char insert[150];//数据库插入语句
                mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
                sprintf_s(insert, "%s%d%s%s%s%d%s%s%s%lf%s%d%s%lf%s", "insert into indent values('", good[i].code.c_str(), "','", username.c_str(), "',", good[i].goodid, ",'", good[i].goodname.c_str(), "',", good[i].price, ",", good[i].num, ",", total_price, ")");
                //cout << insert << "\n"; 
                if (mysql_query(mysql, insert))    //执行SQL语句
                {
                }
                else {

                }
            }
            //更新销量
            for (int i = 1; i <= sum; i++) {
                char update[150];//数据库插入语句
                mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
                sprintf_s(update,"%s%d%s%d","update goods set salesnum=" , good[i].salesum," where id=",good[i].goodid);
                //cout << update << "\n";
                if (mysql_query(mysql, update))    //执行SQL语句
                {
                }
                else {
                    //cout << "成功" << endl;
                }
            }
            printf("下单成功，您已付款%lf元。还要继续下单吗(y/n):", total_price);
            cin >> is_continue_buy;
        }
        else {
            cout << "订单取消!还要继续下单吗(y/n):";
            cin >> is_continue_buy;
        }
        sum = 0;//下单商品数归零
    } while (is_continue_buy == 'y');
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    CustomerMenu();//管理员主页面
}


//营收统计菜单
void RevenueMenu() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆ 营 收 统 计 ☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    cout << "☆                                                                                  ☆" << endl;
    cout << "☆                              【单个商品营收】···(a)                            ☆" << endl;
    cout << "☆                                                                                  ☆" << endl;
    cout << "☆                              【 总  营  收 】···(b)                            ☆" << endl;
    cout << "☆                                                                                  ☆" << endl;
    cout << "☆                              【 返      回 】···(q)                            ☆" << endl;
    cout << "☆                                                                                  ☆" << endl;
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    SelectRevenue();//选择操作
}

//选择营收统计方式
void SelectRevenue() {
    char OrderOperation; //订单操作方式选择
    char flag = 'y';   //是否继续选择其他操作
    do {
        cout << "请选择您要进行的营收统计方式(输入q返回上一级目录):";
        cin >> OrderOperation;
        switch (OrderOperation) {
        case 'a':
            system("cls");
            RevenueById();
            break;
        case 'b':
            system("cls");
            RevenueAll();
            break;
        case 'q':
            system("cls");
            AdministratorMenu();
            break;
        default:
            cout << "选择错误，是否继续(y/n)：" << endl;
            cin >> flag;
            break;
        }
    } while (flag == 'y');
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    AdministratorMenu();//选择查询方式
}
//营收统计
void RevenueById() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆ 单 个 商 品 营 收 统 计 ☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    Goods good;//存放返回的单条数据
    int id;//要查询的商品的编号
    char flag = 'n';//是否继续的标志
    do {//执行查询操作
        cout << "请输入要查询收益的的商品的编号:";
        cin >> id;
        char selectsql[150];//数据库插入语句
        mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
        sprintf_s(selectsql, "%s%d", "select * from goods where id=", id);
        if (mysql_query(mysql, selectsql))    //执行SQL语句
        {
            cout << "该商品不存在！！！想继续查询吗(y/n):";
        }
        else {
            //获取结果集  
            if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
            {
                printf("Couldn't get result from %s\n", mysql_error(mysql));
            }
            //是否有数据的标志
            bool hasData = false;
            //打印获取的数据
            while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行  
            {
                hasData = true;
                good.code = atoi(column[0]);//char转换成int
                good.name = column[1];
                good.brand = column[2];
                good.pur_price = strtod(column[3], NULL);
                good.price = strtod(column[4], NULL);
                good.type = column[5];
                good.num = atoi(column[6]);
                good.salenum = atoi(column[7]);
                good.date = column[8];

                cout << "商品详细信息为：" << endl;
                cout << setiosflags(ios::left) << setw(10) << "编号" << setw(16) << "商品名称" << setw(15)
                    << "生产厂家" << setw(10) << "商品类别" << setw(10) << "进价" << setw(10) <<
                    "售价" << setw(10) << "库存" << setw(10) << "销量" << setw(10) << "入库时间" << endl;
                cout << setiosflags(ios::left) << setw(10) << good.code << setw(16) << good.name << setw(15)
                    << good.brand << setw(10) << good.type << setw(10) << good.pur_price << setw(10) <<
                    good.price << setw(10) << good.num << setw(10) << good.salenum << setw(10) << good.date << endl;
                cout << "该商品共售出" << good.salenum << "件，总共盈利：" << good.salenum * (good.price-good.pur_price) <<"元" << endl;


                cout << "想要继续查询吗(y/n):";
            }
            if (hasData == false) {
                cout << "该商品不存在！！！想继续查询吗(y/n):";
            }

        }
        cin >> flag;
        while (flag != 'y' && flag != 'n')
        {
            cout << "指令错误！！！！！<请输入y/n>" << endl;
            cin >> flag;
        }
    } while (flag == 'y');
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    RevenueMenu();//选择查询方式
}

//所以商品的营收统计
void RevenueAll() {
    cout << "☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆ 总 营 收 ☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆" << endl;
    Goods good;//存放返回的订单的单条数据
    int sum = 0;//查询到的数据的总量
    double all_revenue = 0.0;//总收入
    char selectsql[150];//数据库查询语句
    mysql_query(mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码  
    sprintf_s(selectsql, "select * from goods");
    if (mysql_query(mysql, selectsql))    //执行SQL语句
    {
        cout << "没有相关的商品！！！想继续查询吗(y/n):";
    }
    else {
        //获取结果集  
        if (!(res = mysql_store_result(mysql)))   //获得sql语句结束后返回的结果集  
        {
            printf("Couldn't get result from %s\n", mysql_error(mysql));
        }
        //是否有数据的标志
        bool hasData = false;
        //打印获取的数据
        bool has_out = false;
        while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行  
        {
            hasData = true;
            if (has_out == false) {
                has_out = true;
                cout << "商品销售的详细信息为：" << endl;
                cout << setiosflags(ios::left) << setw(10) << "编号" << setw(16) << "商品名称" << setw(15)
                    << "生产厂家" << setw(10) << "商品类别" << setw(10) << "进价" << setw(10) <<
                    "售价" << setw(10) << "库存" << setw(10) << "销量" << setw(10) << "入库时间" << setw(200) << "总盈利(元)" << endl;
            }
            good.code = atoi(column[0]);//char转换成int
            good.name = column[1];
            good.brand = column[2];
            good.pur_price = strtod(column[3], NULL);
            good.price = strtod(column[4], NULL);
            good.type = column[5];
            good.num = atoi(column[6]);
            good.salenum = atoi(column[7]);
            good.date = column[8];

            cout << setiosflags(ios::left) << setw(10) << good.code << setw(16) << good.name << setw(15)
                << good.brand << setw(10) << good.type << setw(10) << good.pur_price << setw(10) <<
                good.price << setw(10) << good.num << setw(10) << good.salenum << setw(10) << good.date << "  " << setw(20) << good.salenum * (good.price - good.pur_price) << endl;
            all_revenue += good.salenum * (good.price - good.pur_price);
        }
        cout << "\n你的总营收为:" << all_revenue << "元" << endl;
    }
    cout << endl;
    cout << "……信息处理完毕……" << endl;
    cout << "……按任意键返回主菜单……" << endl;
    system("pause");
    getchar();
    system("cls");
    RevenueMenu();//选择查询方式
}
