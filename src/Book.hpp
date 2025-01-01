#ifndef BOOK_HPP
#define BOOK_HPP
#include<iostream>
#include<vector>
#include"Storage.hpp"
#include"User.hpp"
#include"Log.hpp"
#include"Strings.hpp"

using std::exception;
using std::vector;
using std::string;
using std::pair;
using std::cout;
using std::endl;
using std::fixed;
using std::setprecision;
using std::ostream;

extern int CurPrivilege;
extern vector<User>LoginStack;
extern int Count;

class Book {
  public:
    char ISBN[22]{};
    char name[62]{};
    char author[62]{};
    char keyword[62]{};
    long long quantity = 0;
    double price = 0.0;
    Book() = default;
    Book(char* _ISBN) {
      strcpy(ISBN, _ISBN);
    }
    bool operator<(const Book &other) {
      return strcmp(ISBN, other.ISBN) < 0;
    }
    bool operator==(const Book &other) {
      return strcmp(ISBN, other.ISBN) == 0;
    }
    bool operator>(const Book &other) {
      return strcmp(ISBN, other.ISBN) > 0;
    }
    void print() {
      cout << ISBN << '\t' << name << '\t' << author << '\t' << keyword << '\t' << fixed << setprecision(2) << price << '\t';
      cout << quantity << '\n';
    }
    friend ostream& operator<<(ostream &os, Book x) {
      os << x.ISBN << '\t' << x.name << '\t' << x.author << '\t' << x.keyword << '\t' << fixed << setprecision(2) << x.price << '\t';
      os << x.quantity << '\n';
      return os;
    }
};

FileStorage<Book> StorageofBooks;

void bookInit() {
  StorageofBooks.init();
}

void bookSelect(char* _ISBN) {//选择图书操作
  if(CurPrivilege < 3) {//要求权限大于等于3
    throw exception();
  }
  if(LoginStack.empty()) {//为空则操作失败
    throw exception();
  } 
  else {
    strcpy(LoginStack.back().currentISBN, _ISBN);
    Book tmp(_ISBN);
    Book check = StorageofBooks.findToken(tmp);
    if(strcmp(check.ISBN, _ISBN)) {
      StorageofBooks.insertToken(tmp);
    }
  }//修改栈顶选中的元素，并且检查一下这本书有没有，没有就插入一下
}

void buy(char* _ISBN, long long _quantity) {//购买图书操作
  if(CurPrivilege < 1) {//要求权限大于等于1
    throw exception();
  }
  if(_quantity <= 0) {//数量非正则操作失败
    throw exception();
  }
  Book tmp(_ISBN);
  tmp = StorageofBooks.findToken(tmp);
  if(strcmp(tmp.ISBN, _ISBN)) {//图书不存在则操作失败
    throw exception();
  }
  if(_quantity > tmp.quantity) {//超出库存则操作失败
    throw exception();
  }
  StorageofBooks.deleteToken(tmp);
  tmp.quantity -= _quantity;
  StorageofBooks.insertToken(tmp);
  add(_quantity * tmp.price);
  cout << fixed << setprecision(2) << _quantity * tmp.price << endl;//输出总金额
}

void import(long long _quantity, double _totalcost) {//采购图书操作
  if(CurPrivilege < 3) {//要求权限大于等于3
    throw exception();
  }
  if(_quantity < 0 || _totalcost < 0) {//非正数操作失败
    throw exception();
  }
  if(strlen(LoginStack.back().currentISBN) == 0) {//未选中图书操作失败
    throw exception();
  }
  char* ISBN = LoginStack.back().currentISBN;
  Book tmp(ISBN);
  tmp = StorageofBooks.findToken(tmp);
  StorageofBooks.deleteToken(tmp);
  tmp.quantity += _quantity;
  StorageofBooks.insertToken(tmp);
  add(-_totalcost);
}

void bookModify(vector<pair<string, string>> commands) {//修改图书操作
  if(CurPrivilege < 3) {//要求权限大于等于3
    throw exception();
  }
  bool vis[10]{};//0- ISBN, 1-name, 2-author, 3-keyword, 4-price
  if(strlen(LoginStack.back().currentISBN) == 0) {//未选中图书操作失败
    throw exception();
  }
  char* ISBN = LoginStack.back().currentISBN;
  Book tmp(ISBN);
  tmp = StorageofBooks.findToken(tmp);//找到要操作的图书
  Book target = tmp;

  bool ISBNflag = false; //记录有没有修改ISBN

  for(int i = 0; i < commands.size(); i++) {
    string key = commands[i].first;
    char* val = (char*)(commands[i].second.c_str());
    if(key == "ISBN") {
      ISBNflag = true;
      if(vis[0]) throw exception();//有重复参数失败
      vis[0] = true;
      Book ori(val);
      ori = StorageofBooks.findToken(ori);
      if(!strcmp(val, ori.ISBN)) {//检查一下ISBN重复了没有
        throw exception();
      }
      if(strcmp(target.ISBN, val)) {
        strcpy(target.ISBN, val);
      } else {//不允许将ISBN修改为原来的ISBN
        throw exception();
      }
    }
    else if(key == "name") {
      if(vis[1]) throw exception();
      vis[1] = true;
      strcpy(target.name, val);
    }
    else if(key == "author") {
      if(vis[2]) throw exception();
      vis[2] = true;
      strcpy(target.author, val);
    }
    else if(key == "keyword") {
      if(vis[3]) throw exception();
      processKeyword(commands[i].second);//检查关键词有没有重复段
      vis[3] = true;
      strcpy(target.keyword, val);
    }
    else if(key == "price") {
      if(vis[4]) throw exception();
      vis[4] = true;
      target.price = stringToDouble(commands[i].second);
    }
    else {
      throw exception();
    }
  }

  //cout << tmp;
  //cout << target;

  StorageofBooks.deleteToken(tmp);
  StorageofBooks.insertToken(target);//在文件里面修改

  if(ISBNflag) {//假如ISBN已经被改过，在栈里面重新修改
    for(int i = 0; i < LoginStack.size(); i++) {
      if(!strcmp(LoginStack[i].currentISBN, tmp.ISBN)) {
         strcpy(LoginStack[i].currentISBN, target.ISBN);
      }
    }
  }
}

void findAll() {//查找所有图书
  if(CurPrivilege == 0) {//要求权限大于等于1
    throw exception();
  }
  int tot, start;
  node now; 
  StorageofBooks.dict.get_info(tot, 1);
  StorageofBooks.dict.get_info(start, 2);
  StorageofBooks.dict.read(now, start);
  bool flag = false;//有没有输出
  while(true) {
    Book data[StorageofBooks.maxsize];
    StorageofBooks.list.read(data, now.pos);
    for(int i = 0; i < now.size; i++) {
      if(strlen(data[i].ISBN)) {
        flag = true;
        cout << data[i];
      }
    }
    if(now.next != -1) {
      StorageofBooks.dict.read(now, now.next);
    } else {
      break;
    }
  }
  if(!flag) {//没有目标图书输出空行
    cout << "\n";
  }
}

void findbyISBN(char* _ISBN) {//依照ISBN检索图书
  if(CurPrivilege == 0) {//要求权限大于等于1
    throw exception();
  }
  Book tmp(_ISBN);
  tmp = StorageofBooks.findToken(tmp);
  if(!strcmp(tmp.ISBN, _ISBN)) {//找到输出这本书
    cout << tmp;
  } else {//找不到输出空行
    cout << '\n';
  }
}

void findbyName(char* _name) {//依照名称检索图书
  if(CurPrivilege == 0) {//要求权限大于等于1
    throw exception();
  }
  int tot, start;
  node now; 
  StorageofBooks.dict.get_info(tot, 1);
  StorageofBooks.dict.get_info(start, 2);
  StorageofBooks.dict.read(now, start);
  bool flag = false;//有没有输出
  while(true) {
    Book data[StorageofBooks.maxsize];
    StorageofBooks.list.read(data, now.pos);
    for(int i = 0; i < now.size; i++) {
      if(!strcmp(data[i].name, _name)) {//找到了符合条件的图书将之输出
        flag = true;
        cout << data[i];
      }
    }
    if(now.next != -1) {
      StorageofBooks.dict.read(now, now.next);
    } else {
      break;
    }
  }
  if(!flag) {//没有目标图书输出空行
    cout << "\n";
  }
}

void findbyAuthor(char* _author) {//依照作者检索图书
  if(CurPrivilege == 0) {//要求权限大于等于1
    throw exception();
  }
  int tot, start;
  node now; 
  StorageofBooks.dict.get_info(tot, 1);
  StorageofBooks.dict.get_info(start, 2);
  StorageofBooks.dict.read(now, start);
  bool flag = false;//有没有输出
  while(true) {
    Book data[StorageofBooks.maxsize];
    StorageofBooks.list.read(data, now.pos);
    for(int i = 0; i < now.size; i++) {
      if(!strcmp(data[i].author, _author)) {//找到了符合条件的图书将之输出
        flag = true;
        cout << data[i];
      }
    }
    if(now.next != -1) {
      StorageofBooks.dict.read(now, now.next);
    } else {
      break;
    }
  }
  if(!flag) {//没有目标图书输出空行
    cout << "\n";
  }
}

bool containKeyword(vector<string> &key1, char* _keyword) {//检查有没有重合的关键词
  vector<string> key2 = processKeyword((string)_keyword);
  for(int i = 0; i < key1.size(); i++) {
    for(int j = 0; j < key2.size(); j++) {
      if(key1[i] == key2[j]) return true;//检查到有相同的关键词
    }
  }
  return false;
}

void findbyKeyword(char* _keyword) {//依照关键词检索图书
  if(CurPrivilege == 0) {//要求权限大于等于1
    throw exception();
  }
  int tot, start;
  node now; 
  StorageofBooks.dict.get_info(tot, 1);
  StorageofBooks.dict.get_info(start, 2);
  StorageofBooks.dict.read(now, start);
  bool flag = false;//有没有输出
  vector<string> keys = processKeyword(string(_keyword));
  if(keys.size() > 1) {//keyword出现多个关键词则操作失败
    throw exception();
  }
  while(true) {
    Book data[StorageofBooks.maxsize];
    StorageofBooks.list.read(data, now.pos);
    for(int i = 0; i < now.size; i++) {
      if(containKeyword(keys, data[i].keyword)) {//找到了符合条件的图书将之输出
        flag = true;
        cout << data[i];
      }
    }
    if(now.next != -1) {
      StorageofBooks.dict.read(now, now.next);
    } else {
      break;
    }
  }
  if(!flag) {//没有目标图书输出空行
    cout << "\n";
  }
}

#endif