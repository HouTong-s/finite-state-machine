// 有限状态机.cpp 
//
/*
消除多余空格
三种状态：消除多余空格使其剩余一个;""双引号里面正常输出，不做改变;其余情况正常输出。
三种状态码，和对应的情况如下
0：正常输出
1：刚读取了空格
2：位于两个引号之间
例如：（这里的“空”表示空格的意思）
输入是：abc空空空"cde空空eff"空空空空asd
输出为：abc空"cde空空eff"空asd
*/

#include <iostream>
#include<string>
#include<map>
#include<vector>

using namespace std;

/*
* 状态转移函数
* n为当前状态码，event为当前的动作(或者说是事件)
* 如果有错误的event，则返回false；否则返回true
*/
bool transfer_state(int & n,char event,string & result)
{
    switch (n)
    {
    case 0:
        if (event == ' ')
        {
            n = 1;
            result.push_back(event);
        }
        else if (event == '\"')
        {
            n = 2;
            result.push_back(event);
        }
        else
        {
            n = 0;
            result.push_back(event);
        }
        break;
    case 1:
        if (event == ' ')
        {
            n = 1;
        }
        else if (event == '\"')
        {
            n = 2;
            result.push_back(event);
        }
        else
        {
            n = 0;
            result.push_back(event);
        }
        break;
    case 2:
        if (event == '\"')
        {
            n = 0;
            result.push_back(event);
        }
        else
        {
            result.push_back(event);
        }
        break;
    default:
        return false;
    }
    return true;
}

//状态枚举类
enum class Status
{
    //三个状态，用A、B、C来指代0、1、2
    A = 0,B,C
};
//下面用一个结构体来表示状态转移
struct fsm_item
{
    //status为当前状态码
    Status status;
    /*
    map的键表示event，值表示该event对应的后一个状态
    键为'#'时，表示默认事件(default event)的下一个状态；也可无该键，这取决于具体需求
    */
    map<char,Status> transfers;
    fsm_item(Status s,const map<char, Status>& trfs)
    {
        status = s;
        transfers = trfs;
    }
};
//状态转移表
const vector<fsm_item*> items = { new fsm_item(Status::A,map<char,Status>{ {' ',Status::B},{'\"',Status::C},{'#',Status::A} }),
                                  new fsm_item(Status::B,map<char,Status>{ {' ',Status::B},{'\"',Status::C},{'#',Status::A}}),
                                  new fsm_item(Status::C,map<char,Status>{ {'\"',Status::A},{'#',Status::C}}),};

/*
* 利用结构体的状态转移函数，便于扩展，但原理是按照上一个函数来的，不过看起来更工整3
* n为当前状态码，event为当前的动作(或者说是事件)
* 如果有错误的event，则返回false；否则返回true
*/
bool struct_transfer_state(Status& current_status, char event, string& result)
{
    //是否是错误的状态转移，这里不存在错误的状态转移
    bool isFalseTransfer = true;
    for (int i = 0; i < items.size(); i++)
    {
        if (items[i]->status == current_status)
        {
            //属于状态转移表中的状态，设为false
            isFalseTransfer = false;

            for (auto transfer : items[i]->transfers)
            {
                if (event == transfer.first)
                {              
                    //特异性需求，这里在如下情况下result后面要添加字符
                    if (current_status != Status::B || event != ' ')
                    {
                        result.push_back(event);
                    }
                    current_status = transfer.second;
                    return true;
                }              
            }
            //默认情况的状态转移         
            current_status = items[i]->transfers['#'];   
            //这里也是特异性需求
            result.push_back(event);
            return true;
        }
    }
    if (isFalseTransfer)
        return false;
    else
    {
        return true;
    }
}

//纯状态转移，只考虑状态的迁移，不涉及result的改变(可作为其他自动机的雏形)
bool pure_struct_transfer_state(Status& current_status, char event)
{
    //是否是错误的状态转移，这里不存在错误的状态转移
    bool isFalseTransfer = true;
    for (int i = 0; i < items.size(); i++)
    {
        if (items[i]->status == current_status)
        {
            //属于状态转移表中的状态，设为false
            isFalseTransfer = false;

            for (auto transfer : items[i]->transfers)
            {
                if (event == transfer.first)
                {
                    current_status = transfer.second;
                    return true;
                }
            }
            //这里有默认event转移，所有可以返回true,否则这里返回false
            current_status = items[i]->transfers['#'];      
            return true;
        }
    }
    if (isFalseTransfer)
        return false;
    else
    {
        return true;
    }
}
int main()
{
    string s = "abc   \"cde  eff\"    asd";
    //getline(cin, s);
    
    //初始状态为A
    Status status = Status::A;
    string result = "";
    for (int i = 0; i < s.size(); i++)
    {
        if (!struct_transfer_state(status, s[i],result))
        {
            cout << "error" << endl;
            exit(1);
        }  
        else
        //打印状态转移的序列
        {
            cout << (int)status << " ";
        }
    }
    cout << endl << result << endl;
}
//状态转移序列：0 0 0 1 1 1 2 2 2 2 2 2 2 2 2 0 1 1 1 1 0 0 0
