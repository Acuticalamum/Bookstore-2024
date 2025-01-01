#ifndef STRINGS_HPP
#define STRINGS_HPP
#include<vector>
#include<string>
#include<iostream>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::exception;
using std::stod;
using std::pair;

vector<string> splitString(string &s) {
  vector<string> ret{};
  s = s + " ";
  int last = -1;
  for(int i = 0; i < s.size(); i++) {
    if(s[i] == ' ') {
      string t = s.substr(last + 1 , i - last - 1);
      last = i;
      if(t == "") continue;
      if(t == " ") continue;
      ret.push_back(t);
    }
  }
  return ret;
}

long long stringToInt(string &s) {
  for(int i = 0; i < s.size(); i++) {
    if(s[i] < '0' || s[i] > '9') {
      throw exception();
    }
  }
  bool flag = false;
  long long ret = 0;
  if(s[0] == '-') {
    s = s.substr(1, s.size() - 1);
    flag = true;
  }
  for(int i = 0; i < s.size(); i++) {
    ret = ret * 10 + s[i] - '0';
  }
  return flag ? -ret : ret;
}

double stringToDouble(string &s) {
  try {
    return stod(s);
  } catch(const std::invalid_argument& ex) {
    throw exception();
  }
}

pair<string, string> processToken(string &s) {//处理-""=""型的指令
  if(s[0] != '-') {//非法指令
    throw exception();
  }
  int pos = s.find('=');
  if(pos == string::npos) {//没有等号，非法指令
    throw exception();
  }
  string key = s.substr(1, pos - 1);
  string val;
  if(key == "ISBN") {
    val = s.substr(pos + 1);
    if(val.size() > 20) {
      throw exception();
    }
  }
  else if(key == "name") {
    val = s.substr(pos + 1);
    if(val[0] == '\"' && val[val.size() - 1] == '\"') {
      val = val.substr(1, val.size() - 2);
      if(val.size() > 60) {
        throw exception();
      }
    } else {
      throw exception();
    }
  }
  else if(key == "author") {
    val = s.substr(pos + 1);
    if(val[0] == '\"' && val[val.size() - 1] == '\"') {
      val = val.substr(1, val.size() - 2);
      if(val.size() > 60) {
        throw exception();
      }
    } else {
      throw exception();
    }
  }
  else if(key == "keyword") {
    val = s.substr(pos + 1);
    if(val[0] == '\"' && val[val.size() - 1] == '\"') {
      val = val.substr(1, val.size() - 2);
      if(val.size() > 60) {
        throw exception();
      }
    } else {
      throw exception();
    }
  }
  else if(key == "price") {
    val = s.substr(pos + 1);
  }
  else {//提示词不对
    throw exception();
  }
  if(val == "") {//附加参数为空则操作失败
    throw exception();
  }
  return make_pair(key, val);
}

vector<string> processKeyword(string s) {//将keyword分成一个个关键词
  vector<string> ret{};
  s = s + "|";
  int last = -1;
  for(int i = 0; i < s.size(); i++) {
    if(s[i] == '|') {
      string t = s.substr(last + 1 , i - last - 1);
      last = i;
      for(int i = 0; i < ret.size(); i++) {
        if(ret[i] == t) {//Keyword包含重复信息段则操作失败
          throw exception();
        }
      }
      ret.push_back(t);
    }
  }
  return ret;
}

void invalid(string &str) {//输出不合法提示信息
  //cout << str << endl;
  cout << "Invalid\n";
  //cout << ++cnt << endl;
}
#endif