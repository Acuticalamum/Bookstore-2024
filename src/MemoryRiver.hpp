#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

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
    
    template<class Ttoken>
    void readToken(Ttoken &t, const int index) {
      file.open(file_name);
      file.seekg(index);
      file.read(reinterpret_cast<char *>(&t), sizeof(Ttoken));
      file.close();
    }

};


#endif //BPT_MEMORYRIVER_HPP