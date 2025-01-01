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
vector<double> income, cost;

void logInit() {//初始化
  income.clear();
  cost.clear();
  income.push_back(0.0);  
  cost.push_back(0.0);
}

int getCount() {//返回交易笔数
  return Count;
}

void showCount() {//输出交易笔数
  cout << Count << endl;
}

void add(double money) {//修改账目
  Count++;
  if(money > 0) {
    income.push_back(income.back() + money);  
    cost.push_back(cost.back());
  }
  if(money == 0) {
    income.push_back(income.back());  
    cost.push_back(cost.back());
  }
  if(money < 0) {
    income.push_back(income.back());
    cost.push_back(cost.back() - money);
  }
}

void showFinance(int cnt, bool flag) {//显示收入支出
  if(!flag) {//没输Count输出总额
    cout << fixed << setprecision(2) << "+ " << income[Count] << " - " << cost[Count] << '\n';
    return;
  }
  if(cnt == 0) {//Count为0输出空行
    cout << "\n";
  }
  else if(cnt > Count || cnt < 0) {//Count不合规操作失败
    throw exception();
  }
  else cout << fixed << setprecision(2) << "+ " << income[Count] - income[Count - cnt] << " - " <<cost[Count] - cost[Count - cnt] << '\n';
}


#endif