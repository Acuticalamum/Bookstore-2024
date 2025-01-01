#ifndef USER_HPP
#define USER_HPP
#include<cstring>
#include<vector>
#include<iostream>
#include "Storage.hpp"

using std::string;
using std::vector;
using std::exception;
using std::cout;
using std::endl;

class User {
  public:
    char id[35]{};
    char name[35]{};
    char password[35]{};
    int privilege = 0;
    char currentISBN[22]{};

    User() = default;
    User(char* _id, char* _name, char* _password, int _privilege) : privilege(_privilege) {
      strcpy(id, _id);
      strcpy(name, _name);
      strcpy(password, _password);
      memset(currentISBN, '\0', sizeof(currentISBN));
    }
    User(char* _id) {
      strcpy(id, _id);
    }
    bool operator<(const User &other) {
      return strcmp(id, other.id) < 0;
    }
    bool operator==(const User &other) {
      return strcmp(id, other.id) == 0;
    }
    bool operator>(const User &other) {
      return strcmp(id, other.id) > 0;
    }
};

FileStorage <User> StorageofUser("User");
vector<User> LoginStack;
int CurPrivilege = 0;

void userInit() {
  StorageofUser.init();
  User root("root", "root", "sjtu", 7);
  StorageofUser.insertToken(root);
}//创建超级管理员

void su(char* _id, char* _password) {
  User tmp(_id);
  tmp = StorageofUser.findToken(tmp);
  if(strcmp(tmp.id, _id)) {
    throw exception(); 
  }//该用户不存在则操作失败
  if(strlen(_password) && strcmp(_password, tmp.password)) {
    throw exception();
  }//密码错误则操作失败
  if(CurPrivilege > tmp.privilege) {
    LoginStack.push_back(tmp);
    CurPrivilege = tmp.privilege;
    return;
  }//高于该等级直接登录
  if(strlen(_password)) {
    LoginStack.push_back(tmp);
    CurPrivilege = tmp.privilege;
    return;
  } else {
    throw exception();
  }//常规账户登录
}//登录操作

void logout() {
  if(CurPrivilege == 0) {
    throw exception();
  }//权限等级要求 >= 1
  if(LoginStack.empty()) {
    throw exception();
  }//登录栈为空则操作失败
  LoginStack.pop_back();
  if(LoginStack.empty()) {
    CurPrivilege = 0;
  } else {
    CurPrivilege = LoginStack.back().privilege;
  }//修改当前的权限
}//注销操作

void userRegister(char* _id, char* _password, char* _name) {
  User tmp(_id);
  tmp = StorageofUser.findToken(tmp);
  if(!strcmp(tmp.id, _id)) {
    throw exception();
  }//有重复id操作失败
  tmp = User(_id, _name, _password, 1);
  StorageofUser.insertToken(tmp);//插入成功
}//注册操作

void passwd(char* _id, char* CurrentPassword, char* NewPassword) {
  if(CurPrivilege == 0) {
    throw exception();
  }//权限等级要求 >= 1
  User tmp(_id);
  tmp = StorageofUser.findToken(tmp);
  if(strcmp(tmp.id, _id)) {
    throw exception(); 
  }//该用户不存在则操作失败
  if(strlen(CurrentPassword) && strcmp(CurrentPassword, tmp.password)) {
    throw exception();
  }//密码错误则操作失败
  if(CurPrivilege == 7) {
    StorageofUser.deleteToken(tmp);
    strcpy(tmp.password, NewPassword);
    StorageofUser.insertToken(tmp);
    return;
  }//权限为7直接修改
  if(strlen(CurrentPassword)) {
    StorageofUser.deleteToken(tmp);
    strcpy(tmp.password, NewPassword);
    StorageofUser.insertToken(tmp);
  } else {
    throw exception();
  }//常规修改
}//修改密码

void useradd(char* _id, char* _password, int _privilege, char* _name) {
  if(CurPrivilege < 3) {
    throw exception();
  }//权限要求等级 >= 3
  if(_privilege >= CurPrivilege) {
    throw exception();
  }//如果待创建帐户的权限等级大于等于当前帐户权限等级则操作失败
  if(_privilege != 0 && _privilege != 1 && _privilege != 3 && _privilege != 7) {
    throw exception();
  }
  User tmp(_id);
  tmp = StorageofUser.findToken(tmp);
  if(!strcmp(tmp.id, _id)) {
    throw exception();
  }//有重复id操作失败
  tmp = User(_id, _name, _password, _privilege);
  StorageofUser.insertToken(tmp);//插入成功
}//添加用户

void userDelete(char* _id) {
  if(CurPrivilege != 7) {
    throw exception();
  }//权限要求等级 >= 7
  User tmp(_id);
  tmp = StorageofUser.findToken(tmp);
  if(strcmp(tmp.id, _id)) {
    throw exception();
  }//id不存在则操作失败
  for(auto it = LoginStack.begin(); it != LoginStack.end(); it++) {
    if(tmp == *it) {
      throw exception();
    }
  }//用户已登录则操作失败
  StorageofUser.deleteToken(tmp);
}//删除用户

#endif