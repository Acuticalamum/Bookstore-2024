#include<bits/stdc++.h>
using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::cin;
using std::cout;

constexpr int maxsize = 1500;

class key {
  private:
    int val;
  public:
    char index[70]{};
    bool operator<(const key &x) {
      if(!strcmp(index, x.index)) {
        return val < x.val;
      }
      return strcmp(index, x.index) < 0;
    }
    bool operator==(const key &x) {
      return ((!strcmp(index, x.index)) && val == x.val);
    }
    bool operator>(const key &x) {
      if(!strcmp(index, x.index)) {
        return val > x.val;
      }
      return strcmp(index, x.index) > 0;
    }
    friend std::ostream& operator << (std::ostream& os, key &x) {
      os << x.index << ' ' << x.val << std::endl;
      return os;
    }//debug
    key() {
      val = 0;
      memset(index, '\0', sizeof(index));
    }
    key input() {
      cin >> index;
      cin >> val;
      return *this;
    }
    key(std::string str) {
      strcpy(index, str.c_str());
      val = -1;
    }//初始化find操作需要用到的key
    void printvalue() {
      printf("%d ", val);
    }
 };

template<class T, int info_len = 2>
class MemoryRiver {
  private:
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);
  public:
    MemoryRiver() = default;

    MemoryRiver(const string& file_name) : file_name(file_name) {}

    bool initialise(string FN = "") {
      if (FN != "") file_name = FN;
      ifstream file_check(file_name);
      if(!file_check.good()) {
        file.open(file_name, std::ios::out);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
          file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
        return true;
      } else {
        file_check.close();
        return false;
      }
    }

    void get_info(int &tmp, int n) {
      if (n > info_len) return;
      file.open(file_name);
      file.seekg((n - 1) * sizeof(int));
      file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
      file.close();
    }

    void write_info(int tmp, int n) {
      if (n > info_len) return;
      file.open(file_name);
      file.seekp((n - 1) * sizeof(int));
      file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
      file.close();
    }

    void update(T &t, const int index) {
      file.open(file_name);
      file.seekp(index);
      file.write(reinterpret_cast<char *>(&t), sizeofT);
      file.close();
    }
    
    int write(T &t) {
        file.open(file_name);
        file.seekp(0, std::ios::end);
        int index = file.tellp();
        file.write(reinterpret_cast<char *>(&t), sizeofT);
        file.close();
        return index;
    }

    void read(T &t, const int index) {
      file.open(file_name);
      file.seekg(index);
      file.read(reinterpret_cast<char *>(&t), sizeofT);
      file.close();
    }

    void readkey(key &t, const int index) {
      file.open(file_name);
      file.seekg(index);
      file.read(reinterpret_cast<char *>(&t), sizeof(key));
      file.close();
    }
};

class node {
  private:
    int pos;
    int size;
    int next;
    int nodepos;
  public:
    int getpos() {
      return pos;
    }
    int getsize() {
      return size;
    }
    int getnext() {
      return next;
    }
    int getnodepos() {
      return nodepos;
    }
    void setsize(int _size) {
      size = _size;
    }
    void setnext(int _next) {
      next = _next;
    }
    void setnodepos(int _nodepos) {
      nodepos = _nodepos;
    }
    void setpos(int _pos) {
      pos = _pos;
    }
    node(int _pos, int _size, int _next, int _nodepos) : pos(_pos), size(_size), next(_next), nodepos(_nodepos) {}
    node() = default;
};

MemoryRiver<key[maxsize], 3> book("book");
MemoryRiver<node, 3> dict("dict"); //第一个存储node的总个数，第二个存储第一个node的地址

void init() {
  book.initialise("book");
  bool flag = dict.initialise("dict");
  if(flag) {
    //文建刚刚创建，初始化第一个节点
    node x(100, 1, -1, 110); //100表示第一个块代表的key的起始地址， -1表示end
    dict.write_info(1, 1);
    dict.write_info(110, 2); //110表示块起始地址
    dict.update(x, 110);
    key k[maxsize];
    book.update(k, 100);
  }
}

key getFirstKey(node &x) {
  key ret;
  book.readkey(ret, x.getpos());
  return ret;
}

node findNode(key &x) {
  int tot, start;
  node now, tmp; 
  dict.get_info(tot, 1);
  dict.get_info(start, 2);
  dict.read(now, start);
  key first = getFirstKey(now);
  //cout << first << std::endl;
  //cout << now.getsize() << std::endl;
  while(first < x || first == x) {
    tmp = now;
    if(now.getnext() == -1) break;
    dict.read(now, now.getnext());
    if(!now.getsize()) continue;
    first = getFirstKey(now);
  }
  return tmp;
}

void split(node &x) {
  int pos = x.getpos();
  key data[maxsize];
  book.read(data, pos);

  int tot;
  dict.get_info(tot, 1);
  for(int i = 0; i < maxsize / 2; i++) {
    data[i] = data[i + maxsize / 2];
  }

  dict.write_info(tot + 1, 1);
  /*node other(tot * (sizeof(key[maxsize])) + 100, 300, x.getnext(), 110 + tot * (sizeof(node)));
  // 第一个代表other新的索引位， 第二个代表other是300个元素的块，第三个代表other的后继，第四个代表other在dict的位置
  x.setsize(300);
  x.setnext(other.getnodepos());
  //将x的信息修改为other的前驱
  book.update(data, other.getpos());//将data中的信息重新写到other的位置里面
  dict.update(x, pos);
  dict.update(other, other.getpos());*/
  node other;
  other.setsize(maxsize / 2);
  other.setnext(x.getnext());
  other.setnodepos(dict.write(other));
  other.setpos(book.write(data));
  dict.update(other, other.getnodepos());

  x.setsize(maxsize / 2);
  x.setnext(other.getnodepos());
  dict.update(x, x.getnodepos());
}

void insertKey(key &x) {

  node block = findNode(x);
  int pos = block.getpos();
  int size = block.getsize();
  key data[maxsize];
  book.read(data, pos);

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
  block.setsize(size + 1); //修改成功

  book.update(data, pos);
  dict.update(block, block.getnodepos());
  //在文件中修改

  if(block.getsize() == maxsize) {
    split(block);
  }//裂成两个新块
}

void deleteKey(key &x) {

  node block = findNode(x);
  int pos = block.getpos();
  int size = block.getsize();
  key data[maxsize];
  book.read(data, pos);

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
  block.setsize(size - 1); //修改成功

  book.update(data, pos);
  dict.update(block, block.getnodepos()); //在文件中修改

}

void find(std::string &str) {

  key x(str);
  char s[70];
  strcpy(s, str.c_str());
  node block = findNode(x);
  int pos = block.getpos();
  int size = block.getsize();
  key data[maxsize];
  book.read(data, pos);
  
  bool flag = false;
  bool flag_ = false;

  while(true) {
    for(int i = 0; i < size; i++) {
      if(strcmp(s, data[i].index)) {
        if(flag) {
          flag_ = true;
          break;
        }
        else continue;
      }
      data[i].printvalue();
      flag = true;
    }
    if(flag_) break;
    if(block.getnext() == -1) break;
    dict.read(block, block.getnext());
    book.read(data, block.getpos());
    size = block.getsize();
  }
  
  if(!flag) cout << "null";

  cout << std::endl;

}

int n;

int main() {
  init();
  cin >> n;
  std::string operation;
  for(int i = 1; i <= n; i++) {
    cin >> operation;
    if(operation == "find") {
      std::string str;
      cin >> str;
      find(str);
    }
    if(operation == "insert") {
      key x;
      x.input();
      insertKey(x);
    }
    if(operation == "delete") {
      key x;
      x.input();
      deleteKey(x);
    }
  }
  return 0;
}
