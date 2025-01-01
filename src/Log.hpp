#ifndef LOG_HPP
#define LOG_HPP

#include<vector>
#include<string>
#include<iostream>
#include<iomanip>

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::exception;
using std::fixed;
using std::setprecision;

int Count = 0;

class Log {
  public:
  double income;
  double cost;
  int num;//表示是第几笔交易
  Log() {
    num = 0;
    income = 0.0;
    cost = 0.0;
  }
  Log(int _num, double _income, double _cost) {
    num = _num;
    income = _income;
    cost = _cost;
  }
  Log(int _num) {
    num = _num;
  }
  bool operator<(const Log &other) {
    return num < other.num;
  }
  bool operator==(const Log &other) {
    return num == other.num;
  }
  bool operator>(const Log &other) {
    return num > other.num;
  }
};

FileStorage<Log> StorageofLogs("Logs");
MemoryRiver<int, 1> CountRecord("CountRecord");//那个1存的是交易笔数

void logInit() {
  CountRecord.initialise();
}

int getCount() {//返回交易笔数
  int Count;
  CountRecord.get_info(Count, 1);
  return Count;
}

double getIncome(int cnt) {
  Log tmp(cnt);
  tmp = StorageofLogs.findToken(tmp);
  return tmp.income;
}

double getCost(int cnt) {
  Log tmp(cnt);
  tmp = StorageofLogs.findToken(tmp);
  return tmp.cost;
}

void add(double money) {//修改账目
  int Count = getCount();
  Count++;
  CountRecord.write_info(Count, 1);
  double _income = getIncome(Count - 1);
  double _cost = getCost(Count - 1);
  if(money > 0) _income += money;
  if(money < 0) _cost -= money;
  Log tmp(Count, _income, _cost);
  StorageofLogs.insertToken(tmp);
}

void showFinance(int cnt, bool flag) {//显示收入支出
  int Count = getCount();
  if(CurPrivilege != 7) {//需要权限为7
    throw exception();
  }
  if(!flag) {//没输Count输出总额
    cout << fixed << setprecision(2) << "+ " << getIncome(Count) << " - " << getCost(Count) << '\n';
    return;
  }
  if(cnt == 0) {//Count为0输出空行
    cout << "\n";
  }
  else if(cnt > Count || cnt < 0) {//Count不合规操作失败
    throw exception();
  }
  else cout << fixed << setprecision(2) << "+ " << getIncome(Count) - getIncome(Count - cnt) << 
  " - " << getCost(Count) - getCost(Count - cnt) << '\n';
}


#endif