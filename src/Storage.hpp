#ifndef STORAGE_HPP
#define STORAGE_HPP
#include "MemoryRiver.hpp"
#include<iostream>

using std::string;
using std::cout;
using std::endl;

class node {
  public:
    int pos;
    int size;
    int next;
    int nodepos;
    node(int _pos, int _size, int _next, int _nodepos) : pos(_pos), size(_size), next(_next), nodepos(_nodepos) {}
    node() = default;
};

template<class T>
class FileStorage {
  public:  
  string fileStorageName;
  const static int maxsize = 1400;
  MemoryRiver<node, 3> dict;
  MemoryRiver<T[maxsize], 3> list; 

  bool init() {
    bool flag = dict.initialise(fileStorageName + "dict");
    list.initialise(fileStorageName + "list");
    if(!flag) {//如果没有创建文件
      node x(100, 1, -1, 110); //100表示第一个块代表的key的起始地址， -1表示end
      dict.write_info(1, 1);
      dict.write_info(110, 2); //110表示块起始地址
      dict.update(x, 110);
      T k[maxsize];
      list.update(k, 100);
    }
    return flag;
  }

  FileStorage() = default;
  FileStorage(string _fileStorageName) : fileStorageName(_fileStorageName) {
   init();
  }

  T getFirstToken(node &x) {
    T ret;
    list.readToken(ret, x.pos);
    return ret;
  }

  node findNode(T &x) {
    int tot, start;
    node now, tmp; 
    dict.get_info(tot, 1);
    dict.get_info(start, 2);
    dict.read(now, start);
    T first = getFirstToken(now);
    if(now.next == -1) return now;
    while(first < x || first == x) {
      tmp = now;
      if(now.next == -1) break;
      dict.read(now, now.next);
      if(!now.size) continue;
      first = getFirstToken(now);
    }
    return tmp;
  }
  
  void split(node &x) {
    int pos = x.pos;
    T data[maxsize];
    list.read(data, pos);

    int tot;
    dict.get_info(tot, 1);
    for(int i = 0; i < maxsize / 2; i++) {
      data[i] = data[i + maxsize / 2];
    }

    dict.write_info(tot + 1, 1);
  
    node other;
    other.size = maxsize / 2;
    other.next = x.next;
    other.nodepos = dict.write(other);
    other.pos = list.write(data);
    dict.update(other, other.nodepos);

    x.size = maxsize / 2;
    x.next = other.nodepos;
    dict.update(x, x.nodepos);
  }

  void insertToken(T &x) {
    node block = findNode(x);
    int pos = block.pos;
    int size = block.size;
    T data[maxsize];
    list.read(data, pos);

    int j = -1;//从哪里开始改
    if(data[size - 1] < x) {
      j = size - 1;
    } else {
      for(int i = 0; i < size - 1; i++) {
        if(data[i] == x) return;
        if(data[i] < x && data[i + 1] > x) {
          j = i;
          break;
        }
      }
    }

    for(int i = size - 1; i > j; i--) {
      data[i + 1] = data[i];
    }
    data[j + 1] = x;
    block.size = size + 1; //修改成功

    list.update(data, pos);
    dict.update(block, block.nodepos);
    //在文件中修改

    if(block.size == maxsize) {
      split(block);
    }//裂成两个新块
  }
  
  void deleteToken(T &x) {
    node block = findNode(x);
    int pos = block.pos;
    int size = block.size;
    T data[maxsize];
    list.read(data, pos);

    int j = -1;//删掉下标为几的key
    for(int i = 0; i < size; i++) {
      if(data[i] == x) {
        j = i;
        break;
      }
    }
  
    if(j == -1) return; //找不到就直接退出

    for(int i = j; i < size - 1; i++) {
      data[i] = data[i + 1];
    }
    block.size = size - 1; //修改成功

    list.update(data, pos);
    dict.update(block, block.nodepos); //在文件中修改
  }
  
  T findToken(T &x) {
    node block = findNode(x);
    int pos = block.pos;
    int size = block.size;
    T data[maxsize];
    list.read(data, pos);
    for(int i = 0; i < size; i++) {
      if(data[i] == x) return data[i];
      //cout << data[i].id << endl;
    }
    return T();
  }

};

#endif