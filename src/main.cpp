#include<iostream>
#include<string>
#include"User.hpp"
#include"Strings.hpp"
#include"Book.hpp"
#include"Log.hpp"

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::exception;
using std::pair;

int main() {
  //freopen("../test1.in", "r", stdin);
  userInit();//登录超级管理员root
  logInit();//初始化账目日志
  bookInit();
  string str; //每次输入的指令
  while(getline(cin, str)) {
    //cout << str << endl;
    vector<string> tokens = splitString(str);
    string cmd = tokens[0];
    if(cmd == "quit" || cmd == "exit") {//退出程序
      exit(0);
    }
    else if(cmd == "su") {//登录账号 User部分
      try {
        if(tokens.size() == 3) {//有密码
          if(tokens[1].size() > 30 || tokens[2].size() > 30) {//字符串长度不符
            throw exception();
          }
          char *_id = (char*)(tokens[1].c_str());
          char *_password = (char*)(tokens[2].c_str());
          su(_id, _password);
        }
        else if(tokens.size() == 2) {//没密码
          if(tokens[1].size() > 30) {//字符串长度不符
            throw exception();
          }
          char *_id = (char*)(tokens[1].c_str());
          char *_password{};
          su(_id, _password);
        }
        else {//参数个数不符
          throw exception();
        }
      } catch(exception &ex) {
        invalid(str);
      }
    }
    else if(cmd == "logout") {//登出账号 User部分
      try {
        logout();
      } catch(exception &ex) {
        invalid(str);
      }
    }
    else if(cmd == "register") {//注册账号 User部分
      try {
        if(tokens.size() == 4) {
          if(tokens[1].size() > 30 || tokens[2].size() > 30 || tokens[3].size() > 30) {
            throw exception();
          }
          char* _id = (char*)(tokens[1].c_str());
          char* _password = (char*)(tokens[2].c_str());
          char* _name = (char*)(tokens[3].c_str());
          userRegister(_id, _password, _name);
        }
        else {
          throw exception();
        }
      } catch(exception &ex) {
        invalid(str);
      }
    }
    else if(cmd == "passwd") {//修改密码 User部分
      try {
        if(tokens.size() == 4) {//有密码
          if(tokens[1].size() > 30 || tokens[2].size() > 30 || tokens[3].size() > 30) {
            throw exception();
          }
          char* _id = (char*)(tokens[1].c_str());
          char* CurrentPassword = (char*)(tokens[2].c_str());
          char* NewPassword = (char*)(tokens[3].c_str());
          passwd(_id, CurrentPassword, NewPassword);
        }
        else if(tokens.size() == 3) {//没密码
          if(tokens[1].size() > 30 || tokens[2].size() > 30) {
            throw exception();
          }
          char* _id = (char*)(tokens[1].c_str());
          char* NewPassword = (char*)(tokens[2].c_str());
          passwd(_id, "", NewPassword);
        }
        else {//参数个数不符
          throw exception();
        }
      } catch(exception &ex) {
        invalid(str);
      }
    }
    else if(cmd == "useradd") {//创建账户 User部分
      try {
        if(tokens.size() == 5) {
          if(tokens[1].size() > 30 || tokens[2].size() > 30 || tokens[3].size() != 1 || tokens[4].size() > 30) {
            throw exception();
          }
          char* _id = (char*)(tokens[1].c_str());
          char* _password = (char*)(tokens[2].c_str());
          int _privilege = (int)(tokens[3][0] - '0');
          char* _name = (char*)(tokens[4].c_str());
          useradd(_id, _password, _privilege, _name);
        }
        else {
          throw exception();
        }
      } catch(exception &ex) {
        invalid(str);
      }
    }
    else if(cmd == "delete") {//删除账户 User部分
      try {
        if(tokens.size() == 2) {
          if(tokens[1].size() > 30) {
            throw exception();
          }
          char* _id = (char*)(tokens[1].c_str());
          userDelete(_id);
        }
        else {
          throw exception();
        }
      } catch(exception &ex) {
        invalid(str);
      }
    }
    else if(cmd == "show" && ((tokens.size() > 1 && tokens[1] != "finance") || tokens.size() == 1)) {//检索图书 Book部分
      try {
        if(tokens.size() == 1) {//检索全部图书
          findAll();
        }
        else if(tokens.size() == 2) {//检索某些图书
          auto pii = processToken(tokens[1]);
          if(pii.first == "ISBN") {
            findbyISBN((char*)(pii.second.c_str()));
          }
          else if(pii.first == "name") {
            findbyName((char*)(pii.second.c_str()));
          }
          else if(pii.first == "author") {
            findbyAuthor((char*)(pii.second.c_str()));
          }
          else if(pii.first == "keyword") {
            findbyKeyword((char*)(pii.second.c_str()));
          }
          else {
            throw exception();
          }
        }
        else {//参数个数不符
          throw exception();
        }
      } catch(exception &ex) {
        invalid(str);
      }
    }
    else if(cmd == "buy") {//购买图书 Book部分
      try {
        if(tokens.size() == 3) {
          if(tokens[1].size() > 20) {
            throw exception();
          }
          char* _ISBN = (char*)(tokens[1].c_str());
          long long _quantity = stringToInt(tokens[2]);
          buy(_ISBN, _quantity);
        }
        else {
          throw exception();
        }
      } catch(exception &ex) {
        invalid(str);
      }
    }
    else if(cmd == "select") {//选择图书 Book部分
      try {
        if(tokens.size() == 2) {
          if(tokens[1].size() > 20) {//ISBN大小超出
            throw exception();
          }
          else {
            char* _ISBN = (char*)(tokens[1].c_str());
            bookSelect(_ISBN);
          }
        }
        else {//参数个数不符
          throw exception();
        }
      } catch(exception &ex) {
        invalid(str);
      }
    }
    else if(cmd == "modify") {//修改图书 Book部分
      try {
        vector<pair<string, string>> commands;
        for(int i = 1; i < tokens.size(); i++) {
          auto pii = processToken(tokens[i]);
          commands.push_back(pii);
          //cout << pii.first << ' ' << pii.second << endl;
        }
        bookModify(commands);
      } catch(exception &ex) {
        invalid(str);
      }
    }
    else if(cmd == "import") {//采购图书 Book部分
      try {
        if(tokens.size() == 3) {
          long long _quantity = stringToInt(tokens[1]);
          double _totalcost = stringToDouble(tokens[2]);
          import(_quantity, _totalcost);
        }
        else {
          throw exception();
        }
      } catch(exception &ex) {
        invalid(str);
      }
    }
    else if(cmd == "Count") {//交易笔数 Log部分
      try {
        if(tokens.size() != 1) {//参数个数不符
          throw exception();
        }
        else {
          showCount();
        }
      } catch(exception &ex) {
        invalid(str);
      }
    }
    else if(cmd == "show" && tokens.size() > 1 && tokens[1] == "finance") {//收入支出 Log部分
      try {
        if(tokens.size() == 2) {//没输Count
          showFinance(getCount(), false);
        }
        else if(tokens.size() == 3) {//输了Count
          int cnt = stringToInt(tokens[2]);
          showFinance(cnt, true);
        }
        else {
          throw exception();
        }
      } catch(exception &ex) {
        invalid(str);
      }
    }
    else {//其余情况均不合法
      invalid(str);
    }
  }
  return 0;
}