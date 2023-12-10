#include <algorithm>
#include <chrono>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

enum class op_type { regs, memory, segment, data, accum };

void mov(std::string a, std::string b);
void add(std::string a, std::string b);
void push(std::string a);
void pop(std::string a);
void mul(std::string a);
void imul(std::string a);  // не реализован
void cmp(std::string a, std::string b);
void hlt();
void adc(std::string a, std::string b);
void jmp(std::string a);
void loop(std::string a);
void call(std::string a);
void ret(std::string a);
void jg(std::string a);
void jz(std::string a);

std::vector<char> char_coms;

void WriteToMemFile() {
  std::ofstream com("mem.hex", std::ios::binary);
  for (char c : char_coms) {
    com.write((char *)&c, 1);
  }
  com.close();
  return;
}

void misha() {
  std::ifstream dota("com.txt");
  std::string com;
  std::string word;
  while (!dota.eof()) {
    std::vector<std::string> vec;
    std::getline(dota, com);
    std::replace(com.begin(), com.end(), ',', ' ');
    std::stringstream ss(com);
    while (ss >> word) {  // Extract word from the stream.
      vec.push_back(word);
    }
    if (vec.size() == 3) {  // размер команды
      if (vec[0] == "mov") {
        mov(vec[1], vec[2]);
      }
      if (vec[0] == "add") {
        add(vec[1], vec[2]);
      }
      if (vec[0] == "cmp") {
        cmp(vec[1], vec[2]);
      }
      if (vec[0] == "adc") {
        adc(vec[1], vec[2]);
      }
    } else if (vec.size() == 2) {
      if (vec[0] == "push") {
        push(vec[1]);
      }
      if (vec[0] == "pop") {
        pop(vec[1]);
      }
      if (vec[0] == "mul") {
        mul(vec[1]);
      }
      if (vec[0] == "jmp") {
        jmp(vec[1]);
      }
      if (vec[0] == "loop") {
        loop(vec[1]);
      }
      if (vec[0] == "call") {
        call(vec[1]);
      }
      if (vec[0] == "ret") {
        ret(vec[1]);
      }
      if (vec[0] == "jg") {
        jg(vec[1]);
      }
      if (vec[0] == "jz") {
        jz(vec[1]);
      }
    } else if (vec.size() == 1) {
      if (vec[0] == "hlt") {
        hlt();
      }
    }
  }
}

int main() {
  auto start = std::chrono::system_clock::now();
  misha();
  WriteToMemFile();
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::time_t end_time = std::chrono::system_clock::to_time_t(end);

  std::cout << "finished computation at " << std::ctime(&end_time)
            << "elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
  return 0;
}

void mov(std::string a, std::string b) {
  op_type op1, op2;
  if (a.find("ax") != std::string::npos) {
    op1 = op_type::accum;
  } else if (a.find("[") != std::string::npos) {
    op1 = op_type::memory;
  } else if (a.find("es") != std::string::npos ||
             a.find("ss") != std::string::npos ||
             a.find("cs") != std::string::npos ||
             a.find("ds") != std::string::npos) {
    op1 = op_type::segment;
  } else if (a.find("ax") != std::string::npos ||
             a.find("bx") != std::string::npos ||
             a.find("cx") != std::string::npos ||
             a.find("dx") != std::string::npos ||
             a.find("sp") != std::string::npos ||
             a.find("bp") != std::string::npos ||
             a.find("si") != std::string::npos ||
             a.find("di") != std::string::npos ||
             a.find("al") != std::string::npos ||
             a.find("bl") != std::string::npos ||
             a.find("cl") != std::string::npos ||
             a.find("dl") != std::string::npos ||
             a.find("ah") != std::string::npos ||
             a.find("bh") != std::string::npos ||
             a.find("ch") != std::string::npos ||
             a.find("dh") != std::string::npos) {
    op1 = op_type::regs;
  } else
    op1 = op_type::data;

  if (b.find("ax") != std::string::npos) {
    op2 = op_type::accum;
  } else if (b.find("[") != std::string::npos) {
    op2 = op_type::memory;
  } else if (b.find("es") != std::string::npos ||
             b.find("ss") != std::string::npos ||
             b.find("cs") != std::string::npos ||
             b.find("ds") != std::string::npos) {
    op2 = op_type::segment;
  } else if (b.find("ax") != std::string::npos ||
             b.find("bx") != std::string::npos ||
             b.find("cx") != std::string::npos ||
             b.find("dx") != std::string::npos ||
             b.find("sp") != std::string::npos ||
             b.find("bp") != std::string::npos ||
             b.find("si") != std::string::npos ||
             b.find("di") != std::string::npos ||
             b.find("al") != std::string::npos ||
             b.find("bl") != std::string::npos ||
             b.find("cl") != std::string::npos ||
             b.find("dl") != std::string::npos ||
             b.find("ah") != std::string::npos ||
             b.find("bh") != std::string::npos ||
             b.find("ch") != std::string::npos ||
             b.find("dh") != std::string::npos) {
    op2 = op_type::regs;
  } else
    op2 = op_type::data;

  if ((op1 == op_type::regs || op1 == op_type::accum) &&
      (op2 == op_type::regs || op2 == op_type::accum)) {
    char com1 = 0, com2 = 0;
    com1 = com1 | 0b10001000;
    com2 |= 0b11000000;
    if (!(a.find("l") != std::string::npos ||
          a.find("h") != std::string::npos)) {
      com1 |= 0b00000001;
    }
    if (a == "ax" || a == "al") com2 |= 0b00000000;
    if (a == "cx" || a == "cl") com2 |= 0b00000001;
    if (a == "dx" || a == "dl") com2 |= 0b00000010;
    if (a == "bx" || a == "bl") com2 |= 0b00000011;
    if (a == "sp" || a == "ah") com2 |= 0b00000100;
    if (a == "bp" || a == "ch") com2 |= 0b00000101;
    if (a == "si" || a == "dh") com2 |= 0b00000110;
    if (a == "di" || a == "bh") com2 |= 0b00000111;
    if (b == "ax" || b == "al") com2 |= 0b00000000;
    if (b == "cx" || b == "cl") com2 |= 0b00001000;
    if (b == "dx" || b == "dl") com2 |= 0b00010000;
    if (b == "bx" || b == "bl") com2 |= 0b00011000;
    if (b == "sp" || b == "ah") com2 |= 0b00100000;
    if (b == "bp" || b == "ch") com2 |= 0b00101000;
    if (b == "si" || b == "dh") com2 |= 0b00110000;
    if (b == "di" || b == "bh") com2 |= 0b00111000;
    char_coms.push_back(com1);
    char_coms.push_back(com2);
    return;
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////
  if (((op1 == op_type::regs || op1 == op_type::accum) &&
       op2 == op_type::memory) ||
      ((op2 == op_type::regs || op2 == op_type::accum) &&
       op1 == op_type::memory)) {
    char com1 = 0, com2 = 0, com3 = 0, com4 = 0;
    std::string word;
    std::vector<std::string> vec;
    int flag = 0;
    if (op1 == op_type::regs || op1 == op_type::accum) {
      if (b.find("[") != std::string::npos &&
          a.find("ax") == std::string::npos) {
        com1 |= 0b10001010;
        com2 |= 0b00000000;
        if (!(a.find("l") != std::string::npos ||
              a.find("h") != std::string::npos)) {
          com1 |= 0b00000001;
        }
        if (a == "ax" || a == "al") com2 |= 0b00000000;
        if (a == "cx" || a == "cl") com2 |= 0b00001000;
        if (a == "dx" || a == "dl") com2 |= 0b00010000;
        if (a == "bx" || a == "bl") com2 |= 0b00011000;
        if (a == "sp" || a == "ah") com2 |= 0b00100000;
        if (a == "bp" || a == "ch") com2 |= 0b00101000;
        if (a == "si" || a == "dh") com2 |= 0b00110000;
        if (a == "di" || a == "bh") com2 |= 0b00111000;

        if (b.find("[bx+si]") != std::string::npos)
          com2 |= 0b00000000;
        else if (b.find("[bx+di]") != std::string::npos)
          com2 |= 0b00000001;
        else if (b.find("[bp+si]") != std::string::npos)
          com2 |= 0b00000010;
        else if (b.find("[bp+di]") != std::string::npos)
          com2 |= 0b00000011;
        else if (b.find("[si]") != std::string::npos)
          com2 |= 0b00000100;
        else if (b.find("[di]") != std::string::npos)
          com2 |= 0b00000101;
        else if (b.find("[bx]") != std::string::npos)
          com2 |= 0b00000111;
        else {
          flag = 1;
          std::replace(b.begin(), b.end(), '+', ' ');
          std::replace(b.begin(), b.end(), '[', ' ');
          std::replace(b.begin(), b.end(), ']', ' ');
          std::stringstream ss(b);
          while (ss >> word) {
            vec.push_back(word);
          }
          if (vec.size() == 1) {
            com2 |= 0b00000110;
            if (vec[0].size() < 4) {
              vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
            }
            std::string firstPair = vec[0].substr(0, 2);
            std::string secondPair = vec[0].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          } else if ((vec.size() == 2 && vec[1].size() <= 2) ||
                     (vec.size() == 3 && vec[2].size() <= 2)) {
            if (vec[0] == "bx" && vec.size() == 2)
              com2 |= 0b01000111;
            else if (vec[0] == "bp" && vec.size() == 2)
              com2 |= 0b01000110;
            else if (vec[0] == "di" && vec.size() == 2)
              com2 |= 0b01000101;
            else if (vec[0] == "si" && vec.size() == 2)
              com2 |= 0b01000100;
            else if (vec[0] == "bx" && vec[1] == "si")
              com2 |= 0b01000000;
            else if (vec[0] == "bx" && vec[1] == "di")
              com2 |= 0b01000001;
            else if (vec[0] == "bp" && vec[1] == "si")
              com2 |= 0b01000010;
            else if (vec[0] == "bp" && vec[1] == "di")
              com2 |= 0b01000011;
            if (vec.size() == 2) {
              flag = 2;
              if (vec[1].size() < 2) {
                vec[1].insert(vec[1].begin(), 2 - vec[1].size(), '0');
              }
              std::string firstPair = vec[1].substr(0, 2);
              com3 = std::stoi(firstPair, nullptr, 16);

            } else {
              flag = 2;
              if (vec[2].size() < 2) {
                vec[2].insert(vec[2].begin(), 2 - vec[2].size(), '0');
              }
              std::string firstPair = vec[2].substr(0, 2);
              com3 = std::stoi(firstPair, nullptr, 16);
            }
          } else if ((vec.size() == 2 && vec[1].size() <= 4 &&
                      vec[1].size() > 2) ||
                     (vec.size() == 3 && vec[2].size() <= 4 &&
                      vec[2].size() > 2)) {
            if (vec[0] == "bx" && vec.size() == 2)
              com2 |= 0b10000111;
            else if (vec[0] == "bp" && vec.size() == 2)
              com2 |= 0b10000110;
            else if (vec[0] == "di" && vec.size() == 2)
              com2 |= 0b10000101;
            else if (vec[0] == "si" && vec.size() == 2)
              com2 |= 0b10000100;
            else if (vec[0] == "bx" && vec[1] == "si")
              com2 |= 0b10000000;
            else if (vec[0] == "bx" && vec[1] == "di")
              com2 |= 0b10000001;
            else if (vec[0] == "bp" && vec[1] == "si")
              com2 |= 0b10000010;
            else if (vec[0] == "bp" && vec[1] == "di")
              com2 |= 0b10000011;
            if (vec.size() == 2) {
              if (vec[1].size() < 4) {
                vec[1].insert(vec[1].begin(), 4 - vec[1].size(), '0');
              }
              std::string firstPair = vec[1].substr(0, 2);
              std::string secondPair = vec[1].substr(2, 2);
              com3 = std::stoi(secondPair, nullptr, 16);
              com4 = std::stoi(firstPair, nullptr, 16);
            } else {
              flag = 2;
              if (vec[2].size() < 4) {
                vec[2].insert(vec[2].begin(), 4 - vec[2].size(), '0');
              }
              std::string firstPair = vec[2].substr(0, 2);
              std::string secondPair = vec[2].substr(2, 2);
              com3 = std::stoi(secondPair, nullptr, 16);
              com4 = std::stoi(firstPair, nullptr, 16);
            }
          }
        }
        // com3 младшая половина
        // com4 старшая
        char_coms.push_back(com1);
        char_coms.push_back(com2);
        if (flag == 1) {
          char_coms.push_back(com3);
          char_coms.push_back(com4);
        } else if (flag == 2) {
          char_coms.push_back(com3);
        }
      } else {
        com1 |= 0b10100000;
        com2 |= 0b00000000;
        if (!(a.find("l") != std::string::npos ||
              a.find("h") != std::string::npos)) {
          com1 |= 0b00000001;
        }
        std::replace(b.begin(), b.end(), '[', ' ');
        std::replace(b.begin(), b.end(), ']', ' ');
        std::stringstream ss(b);
        while (ss >> word) {
          vec.push_back(word);
        }
        if (vec.size() == 1) {
          if (vec[0].size() < 4) {
            vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
          }
          std::string firstPair = vec[0].substr(0, 2);
          std::string secondPair = vec[0].substr(2, 2);
          com2 = std::stoi(secondPair, nullptr, 16);
          com3 = std::stoi(firstPair, nullptr, 16);
          char_coms.push_back(com1);
          char_coms.push_back(com2);
          char_coms.push_back(com3);
        }
      }
    } else {
      if (b.find("[") != std::string::npos &&
          a.find("ax") == std::string::npos) {
        com1 |= 0b10001000;
        com2 |= 0b00000000;
        if (!(b.find("l") != std::string::npos ||
              b.find("h") != std::string::npos)) {
          com1 |= 0b00000001;
        }
        if (b == "ax" || b == "al") com2 |= 0b00000000;
        if (b == "cx" || b == "cl") com2 |= 0b00001000;
        if (b == "dx" || b == "dl") com2 |= 0b00010000;
        if (b == "bx" || b == "bl") com2 |= 0b00011000;
        if (b == "sp" || b == "ah") com2 |= 0b00100000;
        if (b == "bp" || b == "ch") com2 |= 0b00101000;
        if (b == "si" || b == "dh") com2 |= 0b00110000;
        if (b == "di" || b == "bh") com2 |= 0b00111000;

        if (a.find("[bx+si]") != std::string::npos)
          com2 |= 0b00000000;
        else if (a.find("[bx+di]") != std::string::npos)
          com2 |= 0b00000001;
        else if (a.find("[bp+si]") != std::string::npos)
          com2 |= 0b00000010;
        else if (a.find("[bp+di]") != std::string::npos)
          com2 |= 0b00000011;
        else if (a.find("[si]") != std::string::npos)
          com2 |= 0b00000100;
        else if (a.find("[di]") != std::string::npos)
          com2 |= 0b00000101;
        else if (a.find("[bx]") != std::string::npos)
          com2 |= 0b00000111;
        else {
          flag = 1;
          std::replace(a.begin(), a.end(), '+', ' ');
          std::replace(a.begin(), a.end(), '[', ' ');
          std::replace(a.begin(), a.end(), ']', ' ');
          std::stringstream ss(a);
          while (ss >> word) {
            vec.push_back(word);
          }
          if (vec.size() == 1) {
            com2 |= 0b00000110;
            if (vec[0].size() < 4) {
              vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
            }
            std::string firstPair = vec[0].substr(0, 2);
            std::string secondPair = vec[0].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          } else if ((vec.size() == 2 && vec[1].size() <= 2) ||
                     (vec.size() == 3 && vec[2].size() <= 2)) {
            if (vec[0] == "bx" && vec.size() == 2)
              com2 |= 0b01000111;
            else if (vec[0] == "bp" && vec.size() == 2)
              com2 |= 0b01000110;
            else if (vec[0] == "di" && vec.size() == 2)
              com2 |= 0b01000101;
            else if (vec[0] == "si" && vec.size() == 2)
              com2 |= 0b01000100;
            else if (vec[0] == "bx" && vec[1] == "si")
              com2 |= 0b01000000;
            else if (vec[0] == "bx" && vec[1] == "di")
              com2 |= 0b01000001;
            else if (vec[0] == "bp" && vec[1] == "si")
              com2 |= 0b01000010;
            else if (vec[0] == "bp" && vec[1] == "di")
              com2 |= 0b01000011;
            if (vec.size() == 2) {
              flag = 2;
              if (vec[1].size() < 2) {
                vec[1].insert(vec[1].begin(), 2 - vec[1].size(), '0');
              }
              std::string firstPair = vec[1].substr(0, 2);
              com3 = std::stoi(firstPair, nullptr, 16);
            } else {
              flag = 2;
              if (vec[2].size() < 2) {
                vec[2].insert(vec[2].begin(), 2 - vec[2].size(), '0');
              }
              std::string firstPair = vec[2].substr(0, 2);
              com3 = std::stoi(firstPair, nullptr, 16);
            }
          } else if ((vec.size() == 2 && vec[1].size() <= 4 &&
                      vec[1].size() > 2) ||
                     (vec.size() == 3 && vec[2].size() <= 4 &&
                      vec[2].size() > 2)) {
            if (vec[0] == "bx" && vec.size() == 2)
              com2 |= 0b10000111;
            else if (vec[0] == "bp" && vec.size() == 2)
              com2 |= 0b10000110;
            else if (vec[0] == "di" && vec.size() == 2)
              com2 |= 0b10000101;
            else if (vec[0] == "si" && vec.size() == 2)
              com2 |= 0b10000100;
            else if (vec[0] == "bx" && vec[1] == "si")
              com2 |= 0b10000000;
            else if (vec[0] == "bx" && vec[1] == "di")
              com2 |= 0b10000001;
            else if (vec[0] == "bp" && vec[1] == "si")
              com2 |= 0b10000010;
            else if (vec[0] == "bp" && vec[1] == "di")
              com2 |= 0b10000011;
            if (vec.size() == 2) {
              if (vec[1].size() < 4) {
                vec[1].insert(vec[1].begin(), 4 - vec[1].size(), '0');
              }
              std::string firstPair = vec[1].substr(0, 2);
              std::string secondPair = vec[1].substr(2, 2);
              com3 = std::stoi(secondPair, nullptr, 16);
              com4 = std::stoi(firstPair, nullptr, 16);
            } else {
              if (vec[2].size() < 4) {
                vec[2].insert(vec[2].begin(), 4 - vec[2].size(), '0');
              }
              std::string firstPair = vec[2].substr(0, 2);
              std::string secondPair = vec[2].substr(2, 2);
              com3 = std::stoi(secondPair, nullptr, 16);
              com4 = std::stoi(firstPair, nullptr, 16);
            }
          }
        }
        char_coms.push_back(com1);
        char_coms.push_back(com2);
        if (flag == 1) {
          char_coms.push_back(com3);
          char_coms.push_back(com4);
        } else if (flag == 2) {
          char_coms.push_back(com3);
        }
      } else {
        com1 |= 0b10100011;
        com2 |= 0b00000000;
        if (!(b.find("l") != std::string::npos ||
              b.find("h") != std::string::npos)) {
          com1 |= 0b00000001;
        }
        std::replace(a.begin(), a.end(), '[', ' ');
        std::replace(a.begin(), a.end(), ']', ' ');
        std::stringstream ss(a);
        while (ss >> word) {
          vec.push_back(word);
        }
        if (vec.size() == 1) {
          if (vec[0].size() < 4) {
            vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
          }
          std::string firstPair = vec[0].substr(0, 2);
          std::string secondPair = vec[0].substr(2, 2);
          com2 = std::stoi(secondPair, nullptr, 16);
          com3 = std::stoi(firstPair, nullptr, 16);
          char_coms.push_back(com1);
          char_coms.push_back(com2);
          char_coms.push_back(com3);
        }
      }
    }
    return;
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  if (op1 == op_type::regs && op2 == op_type::data) {
    char com1 = 0, com2 = 0, com3 = 0;
    std::string word;
    std::vector<std::string> vec;
    bool flag = false;
    com1 |= 0b10110000;
    if (!(a.find("l") != std::string::npos ||
          a.find("h") != std::string::npos)) {
      com1 |= 0b00001000;
    }
    if (a == "ax" || a == "al") com1 |= 0b00000000;
    if (a == "cx" || a == "cl") com1 |= 0b00000001;
    if (a == "dx" || a == "dl") com1 |= 0b00000010;
    if (a == "bx" || a == "bl") com1 |= 0b00000011;
    if (a == "sp" || a == "ah") com1 |= 0b00000100;
    if (a == "bp" || a == "ch") com1 |= 0b00000101;
    if (a == "si" || a == "dh") com1 |= 0b00000110;
    if (a == "di" || a == "bh") com1 |= 0b00000111;
    std::stringstream ss(b);
    while (ss >> word) {
      vec.push_back(word);
    }
    if (vec.size() ==
        1) {  // спросить по поводу добавления условия если число <4
      if (vec[0].size() < 4) {
        vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
      }
      std::string firstPair = vec[0].substr(0, 2);
      std::string secondPair = vec[0].substr(2, 2);
      com2 = std::stoi(secondPair, nullptr, 16);
      com3 = std::stoi(firstPair, nullptr, 16);
    }
    char_coms.push_back(com1);
    char_coms.push_back(com2);
    char_coms.push_back(com3);
    return;
  }
  //////////////////////////////////////////////////////////////////////////////////
  if ((op1 == op_type::segment &&
       (op2 == op_type::regs || op2 == op_type::accum)) ||
      ((op1 == op_type::regs || op1 == op_type::accum) &&
       op2 == op_type::segment)) {
    char com1 = 0, com2 = 0;
    if (op1 == op_type::segment) {
      com1 |= 0b10001110;
      com2 |= 0b11000000;
      if (!(a.find("l") != std::string::npos ||
            a.find("h") != std::string::npos)) {
        com1 |= 0b00001000;
      }
      if (a == "es") com2 |= 0b00000000;
      if (a == "cs") com2 |= 0b00001000;
      if (a == "ss") com2 |= 0b00010000;
      if (a == "ds") com2 |= 0b00011000;
      if (b == "ax" || b == "al") com2 |= 0b00000000;
      if (b == "cx" || b == "cl") com2 |= 0b00000001;
      if (b == "dx" || b == "dl") com2 |= 0b00000010;
      if (b == "bx" || b == "bl") com2 |= 0b00000011;
      if (b == "sp" || b == "ah") com2 |= 0b00000100;
      if (b == "bp" || b == "ch") com2 |= 0b00000101;
      if (b == "si" || b == "dh") com2 |= 0b00000110;
      if (b == "di" || b == "bh") com2 |= 0b00000111;
      char_coms.push_back(com1);
      char_coms.push_back(com2);
    } else {
      com1 |= 0b10001100;
      com2 |= 0b11000000;
      if (!(a.find("l") != std::string::npos ||
            a.find("h") != std::string::npos)) {
        com1 |= 0b00001000;
      }
      if (b == "es") com2 |= 0b00000000;
      if (b == "cs") com2 |= 0b00001000;
      if (b == "ss") com2 |= 0b00010000;
      if (b == "ds") com2 |= 0b00011000;
      if (a == "ax" || a == "al") com2 |= 0b00000000;
      if (a == "cx" || a == "cl") com2 |= 0b00000001;
      if (a == "dx" || a == "dl") com2 |= 0b00000010;
      if (a == "bx" || a == "bl") com2 |= 0b00000011;
      if (a == "sp" || a == "ah") com2 |= 0b00000100;
      if (a == "bp" || a == "ch") com2 |= 0b00000101;
      if (a == "si" || a == "dh") com2 |= 0b00000110;
      if (a == "di" || a == "bh") com2 |= 0b00000111;
      char_coms.push_back(com1);
      char_coms.push_back(com2);
    }
    return;
  }
  //////////////////////////////////////////////////////////////////////////////////////////
  if ((op1 == op_type::segment && op2 == op_type::memory) ||
      (op1 == op_type::memory && op2 == op_type::segment)) {
    char com1 = 0, com2 = 0, com3 = 0, com4 = 0;
    int flag = 0;
    std::string word;
    std::vector<std::string> vec;
    if (op1 == op_type::segment) {
      com1 |= 0b10001110;  //мб ошибка
      com2 |= 0b00000000;
      if (!(a.find("l") != std::string::npos ||
            a.find("h") != std::string::npos)) {
        com1 |= 0b00001000;
      }
      if (a == "es") com2 |= 0b00000000;
      if (a == "cs") com2 |= 0b00001000;
      if (a == "ss") com2 |= 0b00010000;
      if (a == "ds") com2 |= 0b00011000;

      if (b.find("[bx+si]") != std::string::npos)
        com2 |= 0b00000000;
      else if (b.find("[bx+di]") != std::string::npos)
        com2 |= 0b00000001;
      else if (b.find("[bp+si]") != std::string::npos)
        com2 |= 0b00000010;
      else if (b.find("[bp+di]") != std::string::npos)
        com2 |= 0b00000011;
      else if (b.find("[si]") != std::string::npos)
        com2 |= 0b00000100;
      else if (b.find("[di]") != std::string::npos)
        com2 |= 0b00000101;
      else if (b.find("[bx]") != std::string::npos)
        com2 |= 0b00000111;
      else {
        flag = 1;
        std::replace(b.begin(), b.end(), '+', ' ');
        std::replace(b.begin(), b.end(), '[', ' ');
        std::replace(b.begin(), b.end(), ']', ' ');
        std::stringstream ss(b);
        while (ss >> word) {
          vec.push_back(word);
        }
        if (vec.size() == 1) {
          com2 |= 0b00000110;
          if (vec[0].size() < 4) {
            vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
          }
          std::string firstPair = vec[0].substr(0, 2);
          std::string secondPair = vec[0].substr(2, 2);
          com3 = std::stoi(secondPair, nullptr, 16);
          com4 = std::stoi(firstPair, nullptr, 16);
        } else if ((vec.size() == 2 && vec[1].size() <= 2) ||
                   (vec.size() == 3 && vec[2].size() <= 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b01000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b01000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b01000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b01000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b01000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b01000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b01000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b01000011;
          if (vec.size() == 2) {
            flag = 2;
            if (vec[1].size() < 2) {
              vec[1].insert(vec[1].begin(), 2 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);

          } else {
            flag = 2;
            if (vec[2].size() < 2) {
              vec[2].insert(vec[2].begin(), 2 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);
          }
        } else if ((vec.size() == 2 && vec[1].size() <= 4 &&
                    vec[1].size() > 2) ||
                   (vec.size() == 3 && vec[2].size() <= 4 &&
                    vec[2].size() > 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b10000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b10000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b10000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b10000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b10000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b10000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b10000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b10000011;
          if (vec.size() == 2) {
            if (vec[1].size() < 4) {
              vec[1].insert(vec[1].begin(), 4 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            std::string secondPair = vec[1].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          } else {
            if (vec[2].size() < 4) {
              vec[2].insert(vec[2].begin(), 4 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            std::string secondPair = vec[2].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          }
        }
      }
      // com3 младшая половина
      // com4 старшая
      char_coms.push_back(com1);
      char_coms.push_back(com2);
      if (flag == 1) {
        char_coms.push_back(com3);
        char_coms.push_back(com4);
      } else if (flag == 2) {
        char_coms.push_back(com3);
      }
    } else {
      com1 |= 0b10001100;  //мб ошибка
      com2 |= 0b00000000;
      if (!(b.find("l") != std::string::npos ||
            b.find("h") != std::string::npos)) {
        com1 |= 0b00001000;
      }
      if (b == "ax" || b == "al") com2 |= 0b00000000;
      if (b == "cx" || b == "cl") com2 |= 0b00001000;
      if (b == "dx" || b == "dl") com2 |= 0b00010000;
      if (b == "bx" || b == "bl") com2 |= 0b00011000;
      if (b == "sp" || b == "ah") com2 |= 0b00100000;
      if (b == "bp" || b == "ch") com2 |= 0b00101000;
      if (b == "si" || b == "dh") com2 |= 0b00110000;
      if (b == "di" || b == "bh") com2 |= 0b00111000;

      if (a.find("[bx+si]") != std::string::npos)
        com2 |= 0b00000000;
      else if (a.find("[bx+di]") != std::string::npos)
        com2 |= 0b00000001;
      else if (a.find("[bp+si]") != std::string::npos)
        com2 |= 0b00000010;
      else if (a.find("[bp+di]") != std::string::npos)
        com2 |= 0b00000011;
      else if (a.find("[si]") != std::string::npos)
        com2 |= 0b00000100;
      else if (a.find("[di]") != std::string::npos)
        com2 |= 0b00000101;
      else if (a.find("[bx]") != std::string::npos)
        com2 |= 0b00000111;
      else {
        flag = 1;
        std::replace(a.begin(), a.end(), '+', ' ');
        std::replace(a.begin(), a.end(), '[', ' ');
        std::replace(a.begin(), a.end(), ']', ' ');
        std::stringstream ss(a);
        while (ss >> word) {
          vec.push_back(word);
        }
        if (vec.size() == 1) {
          com2 |= 0b00000110;
          if (vec[0].size() < 4) {
            vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
          }
          std::string firstPair = vec[0].substr(0, 2);
          std::string secondPair = vec[0].substr(2, 2);
          com3 = std::stoi(secondPair, nullptr, 16);
          com4 = std::stoi(firstPair, nullptr, 16);
        } else if ((vec.size() == 2 && vec[1].size() <= 2) ||
                   (vec.size() == 3 && vec[2].size() <= 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b01000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b01000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b01000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b01000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b01000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b01000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b01000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b01000011;
          if (vec.size() == 2) {
            flag = 2;
            if (vec[1].size() < 2) {
              vec[1].insert(vec[1].begin(), 2 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);
          } else {
            flag = 2;
            if (vec[2].size() < 2) {
              vec[2].insert(vec[2].begin(), 2 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);
          }
        } else if ((vec.size() == 2 && vec[1].size() <= 4 &&
                    vec[1].size() > 2) ||
                   (vec.size() == 3 && vec[2].size() <= 4 &&
                    vec[2].size() > 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b10000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b10000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b10000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b10000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b10000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b10000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b10000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b10000011;
          if (vec.size() == 2) {
            if (vec[1].size() < 4) {
              vec[1].insert(vec[1].begin(), 4 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            std::string secondPair = vec[1].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          } else {
            if (vec[2].size() < 4) {
              vec[2].insert(vec[2].begin(), 4 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            std::string secondPair = vec[2].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          }
        }
      }
      char_coms.push_back(com1);
      char_coms.push_back(com2);
      if (flag == 1) {
        char_coms.push_back(com3);
        char_coms.push_back(com4);
      } else if (flag == 2) {
        char_coms.push_back(com3);
      }
    }
    return;
  }
}
//?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
void add(std::string a, std::string b) {
  op_type op1, op2;
  if (a.find("ax") != std::string::npos) {
    op1 = op_type::accum;
  } else if (a.find("[") != std::string::npos) {
    op1 = op_type::memory;
  } else if (a.find("es") != std::string::npos ||
             a.find("ss") != std::string::npos ||
             a.find("cs") != std::string::npos ||
             a.find("ds") != std::string::npos) {
    op1 = op_type::segment;
  } else if (a.find("ax") != std::string::npos ||
             a.find("bx") != std::string::npos ||
             a.find("cx") != std::string::npos ||
             a.find("dx") != std::string::npos ||
             a.find("sp") != std::string::npos ||
             a.find("bp") != std::string::npos ||
             a.find("si") != std::string::npos ||
             a.find("di") != std::string::npos ||
             a.find("al") != std::string::npos ||
             a.find("bl") != std::string::npos ||
             a.find("cl") != std::string::npos ||
             a.find("dl") != std::string::npos ||
             a.find("ah") != std::string::npos ||
             a.find("bh") != std::string::npos ||
             a.find("ch") != std::string::npos ||
             a.find("dh") != std::string::npos) {
    op1 = op_type::regs;
  } else
    op1 = op_type::data;

  if (b.find("ax") != std::string::npos) {
    op2 = op_type::accum;
  } else if (b.find("[") != std::string::npos) {
    op2 = op_type::memory;
  } else if (b.find("es") != std::string::npos ||
             b.find("ss") != std::string::npos ||
             b.find("cs") != std::string::npos ||
             b.find("ds") != std::string::npos) {
    op2 = op_type::segment;
  } else if (b.find("ax") != std::string::npos ||
             b.find("bx") != std::string::npos ||
             b.find("cx") != std::string::npos ||
             b.find("dx") != std::string::npos ||
             b.find("sp") != std::string::npos ||
             b.find("bp") != std::string::npos ||
             b.find("si") != std::string::npos ||
             b.find("di") != std::string::npos ||
             b.find("al") != std::string::npos ||
             b.find("bl") != std::string::npos ||
             b.find("cl") != std::string::npos ||
             b.find("dl") != std::string::npos ||
             b.find("ah") != std::string::npos ||
             b.find("bh") != std::string::npos ||
             b.find("ch") != std::string::npos ||
             b.find("dh") != std::string::npos) {
    op2 = op_type::regs;
  } else
    op2 = op_type::data;

  if ((op1 == op_type::regs || op1 == op_type::accum) &&
      (op2 == op_type::regs || op2 == op_type::accum)) {
    char com1 = 0, com2 = 0;
    com1 = com1 | 0b00000000;
    com2 |= 0b11000000;
    if (!(a.find("l") != std::string::npos ||
          a.find("h") != std::string::npos)) {
      com1 |= 0b00000001;
    }
    if (a == "ax" || a == "al") com2 |= 0b00000000;
    if (a == "cx" || a == "cl") com2 |= 0b00000001;
    if (a == "dx" || a == "dl") com2 |= 0b00000010;
    if (a == "bx" || a == "bl") com2 |= 0b00000011;
    if (a == "sp" || a == "ah") com2 |= 0b00000100;
    if (a == "bp" || a == "ch") com2 |= 0b00000101;
    if (a == "si" || a == "dh") com2 |= 0b00000110;
    if (a == "di" || a == "bh") com2 |= 0b00000111;
    if (b == "ax" || b == "al") com2 |= 0b00000000;
    if (b == "cx" || b == "cl") com2 |= 0b00001000;
    if (b == "dx" || b == "dl") com2 |= 0b00010000;
    if (b == "bx" || b == "bl") com2 |= 0b00011000;
    if (b == "sp" || b == "ah") com2 |= 0b00100000;
    if (b == "bp" || b == "ch") com2 |= 0b00101000;
    if (b == "si" || b == "dh") com2 |= 0b00110000;
    if (b == "di" || b == "bh") com2 |= 0b00111000;
    char_coms.push_back(com1);
    char_coms.push_back(com2);
    return;
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////
  if (((op1 == op_type::regs || op1 == op_type::accum) &&
       op2 == op_type::memory) ||
      ((op2 == op_type::regs || op2 == op_type::accum) &&
       op1 == op_type::memory)) {
    char com1 = 0, com2 = 0, com3 = 0, com4 = 0;
    std::string word;
    std::vector<std::string> vec;
    int flag = 0;
    if (op1 == op_type::regs || op1 == op_type::accum) {
      com1 |= 0b00000011;
      com2 |= 0b00000000;
      if (!(a.find("l") != std::string::npos ||
            a.find("h") != std::string::npos)) {
        com1 |= 0b00000001;
      }
      if (a == "ax" || a == "al") com2 |= 0b00000000;
      if (a == "cx" || a == "cl") com2 |= 0b00001000;
      if (a == "dx" || a == "dl") com2 |= 0b00010000;
      if (a == "bx" || a == "bl") com2 |= 0b00011000;
      if (a == "sp" || a == "ah") com2 |= 0b00100000;
      if (a == "bp" || a == "ch") com2 |= 0b00101000;
      if (a == "si" || a == "dh") com2 |= 0b00110000;
      if (a == "di" || a == "bh") com2 |= 0b00111000;

      if (b.find("[bx+si]") != std::string::npos)
        com2 |= 0b00000000;
      else if (b.find("[bx+di]") != std::string::npos)
        com2 |= 0b00000001;
      else if (b.find("[bp+si]") != std::string::npos)
        com2 |= 0b00000010;
      else if (b.find("[bp+di]") != std::string::npos)
        com2 |= 0b00000011;
      else if (b.find("[si]") != std::string::npos)
        com2 |= 0b00000100;
      else if (b.find("[di]") != std::string::npos)
        com2 |= 0b00000101;
      else if (b.find("[bx]") != std::string::npos)
        com2 |= 0b00000111;
      else {
        flag = 1;
        std::replace(b.begin(), b.end(), '+', ' ');
        std::replace(b.begin(), b.end(), '[', ' ');
        std::replace(b.begin(), b.end(), ']', ' ');
        std::stringstream ss(b);
        while (ss >> word) {
          vec.push_back(word);
        }
        if (vec.size() == 1) {
          com2 |= 0b00000110;
          if (vec[0].size() < 4) {
            vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
          }
          std::string firstPair = vec[0].substr(0, 2);
          std::string secondPair = vec[0].substr(2, 2);
          com3 = std::stoi(secondPair, nullptr, 16);
          com4 = std::stoi(firstPair, nullptr, 16);
        } else if ((vec.size() == 2 && vec[1].size() <= 2) ||
                   (vec.size() == 3 && vec[2].size() <= 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b01000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b01000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b01000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b01000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b01000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b01000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b01000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b01000011;
          if (vec.size() == 2) {
            flag = 2;
            if (vec[1].size() < 2) {
              vec[1].insert(vec[1].begin(), 2 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);

          } else {
            flag = 2;
            if (vec[2].size() < 2) {
              vec[2].insert(vec[2].begin(), 2 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);
          }
        } else if ((vec.size() == 2 && vec[1].size() <= 4 &&
                    vec[1].size() > 2) ||
                   (vec.size() == 3 && vec[2].size() <= 4 &&
                    vec[2].size() > 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b10000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b10000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b10000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b10000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b10000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b10000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b10000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b10000011;
          if (vec.size() == 2) {
            if (vec[1].size() < 4) {
              vec[1].insert(vec[1].begin(), 4 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            std::string secondPair = vec[1].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          } else {
            flag = 2;
            if (vec[2].size() < 4) {
              vec[2].insert(vec[2].begin(), 4 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            std::string secondPair = vec[2].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          }
        }
      }
      // com3 младшая половина
      // com4 старшая
      char_coms.push_back(com1);
      char_coms.push_back(com2);
      if (flag == 1) {
        char_coms.push_back(com3);
        char_coms.push_back(com4);
      } else if (flag == 2) {
        char_coms.push_back(com3);
      }
    } else {
      com1 |= 0b00000000;
      com2 |= 0b00000000;
      if (!(b.find("l") != std::string::npos ||
            b.find("h") != std::string::npos)) {
        com1 |= 0b00000001;
      }
      if (b == "ax" || b == "al") com2 |= 0b00000000;
      if (b == "cx" || b == "cl") com2 |= 0b00001000;
      if (b == "dx" || b == "dl") com2 |= 0b00010000;
      if (b == "bx" || b == "bl") com2 |= 0b00011000;
      if (b == "sp" || b == "ah") com2 |= 0b00100000;
      if (b == "bp" || b == "ch") com2 |= 0b00101000;
      if (b == "si" || b == "dh") com2 |= 0b00110000;
      if (b == "di" || b == "bh") com2 |= 0b00111000;

      if (a.find("[bx+si]") != std::string::npos)
        com2 |= 0b00000000;
      else if (a.find("[bx+di]") != std::string::npos)
        com2 |= 0b00000001;
      else if (a.find("[bp+si]") != std::string::npos)
        com2 |= 0b00000010;
      else if (a.find("[bp+di]") != std::string::npos)
        com2 |= 0b00000011;
      else if (a.find("[si]") != std::string::npos)
        com2 |= 0b00000100;
      else if (a.find("[di]") != std::string::npos)
        com2 |= 0b00000101;
      else if (a.find("[bx]") != std::string::npos)
        com2 |= 0b00000111;
      else {
        flag = 1;
        std::replace(a.begin(), a.end(), '+', ' ');
        std::replace(a.begin(), a.end(), '[', ' ');
        std::replace(a.begin(), a.end(), ']', ' ');
        std::stringstream ss(a);
        while (ss >> word) {
          vec.push_back(word);
        }
        if (vec.size() == 1) {
          com2 |= 0b00000110;
          if (vec[0].size() < 4) {
            vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
          }
          std::string firstPair = vec[0].substr(0, 2);
          std::string secondPair = vec[0].substr(2, 2);
          com3 = std::stoi(secondPair, nullptr, 16);
          com4 = std::stoi(firstPair, nullptr, 16);
        } else if ((vec.size() == 2 && vec[1].size() <= 2) ||
                   (vec.size() == 3 && vec[2].size() <= 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b01000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b01000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b01000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b01000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b01000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b01000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b01000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b01000011;
          if (vec.size() == 2) {
            flag = 2;
            if (vec[1].size() < 2) {
              vec[1].insert(vec[1].begin(), 2 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);
          } else {
            flag = 2;
            if (vec[2].size() < 2) {
              vec[2].insert(vec[2].begin(), 2 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);
          }
        } else if ((vec.size() == 2 && vec[1].size() <= 4 &&
                    vec[1].size() > 2) ||
                   (vec.size() == 3 && vec[2].size() <= 4 &&
                    vec[2].size() > 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b10000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b10000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b10000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b10000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b10000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b10000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b10000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b10000011;
          if (vec.size() == 2) {
            if (vec[1].size() < 4) {
              vec[1].insert(vec[1].begin(), 4 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            std::string secondPair = vec[1].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          } else {
            if (vec[2].size() < 4) {
              vec[2].insert(vec[2].begin(), 4 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            std::string secondPair = vec[2].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          }
        }
      }
      char_coms.push_back(com1);
      char_coms.push_back(com2);
      if (flag == 1) {
        char_coms.push_back(com3);
        char_coms.push_back(com4);
      } else if (flag == 2) {
        char_coms.push_back(com3);
      }
    }
    return;
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  if (op1 == op_type::regs && op2 == op_type::data) {
    char com1 = 0, com2 = 0, com3 = 0, com4 = 0;
    std::string word;
    std::vector<std::string> vec;
    bool flag = false;
    com1 |= 0b10000000;

    if (a == "ax" || a == "al") com2 |= 0b11000000;
    if (a == "cx" || a == "cl") com2 |= 0b11000001;
    if (a == "dx" || a == "dl") com2 |= 0b11000010;
    if (a == "bx" || a == "bl") com2 |= 0b11000011;
    if (a == "sp" || a == "ah") com2 |= 0b11000100;
    if (a == "bp" || a == "ch") com2 |= 0b11000101;
    if (a == "si" || a == "dh") com2 |= 0b11000110;
    if (a == "di" || a == "bh") com2 |= 0b11000111;
    std::stringstream ss(b);
    while (ss >> word) {
      vec.push_back(word);
    }
    if (vec.size() ==
        1) {  // спросить по поводу добавления условия если число <4
      if (vec[0].size() <= 4 && vec[0].size() > 2) {
        com1 |= 0b00000001;
        vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
        std::string firstPair = vec[0].substr(0, 2);
        std::string secondPair = vec[0].substr(2, 2);
        com3 = std::stoi(secondPair, nullptr, 16);
        com4 = std::stoi(firstPair, nullptr, 16);
        char_coms.push_back(com1);
        char_coms.push_back(com2);
        char_coms.push_back(com3);
        char_coms.push_back(com4);
      } else if (vec[0].size() <= 2) {
        if ((a.find("l") != std::string::npos ||
             a.find("h") != std::string::npos)) {
          com1 |= 0b00000000;
        } else {
          com1 |= 0b00000011;
        }
        vec[0].insert(vec[0].begin(), 2 - vec[0].size(), '0');
        std::string firstPair = vec[0].substr(0, 2);
        com3 = std::stoi(firstPair, nullptr, 16);
        char_coms.push_back(com1);
        char_coms.push_back(com2);
        char_coms.push_back(com3);
      }
    }
    return;
  }
  if (op1 == op_type::accum && op2 == op_type::data) {
    char com1 = 0, com2 = 0, com3 = 0;
    std::string word;
    std::vector<std::string> vec;
    bool flag = false;
    com1 |= 0b00000101;
    std::stringstream ss(b);
    while (ss >> word) {
      vec.push_back(word);
    }
    if (vec.size() ==
        1) {  // спросить по поводу добавления условия если число <4
      vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
      std::string firstPair = vec[0].substr(0, 2);
      std::string secondPair = vec[0].substr(2, 2);
      com2 = std::stoi(secondPair, nullptr, 16);
      com3 = std::stoi(firstPair, nullptr, 16);
    }
    char_coms.push_back(com1);
    char_coms.push_back(com2);
    char_coms.push_back(com3);
    return;
  }
}
//?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
void push(std::string a) {
  op_type op1;
  if (a.find("ax") != std::string::npos) {
    op1 = op_type::accum;
  } else if (a.find("[") != std::string::npos) {
    op1 = op_type::memory;
  } else if (a.find("es") != std::string::npos ||
             a.find("ss") != std::string::npos ||
             a.find("cs") != std::string::npos ||
             a.find("ds") != std::string::npos) {
    op1 = op_type::segment;
  } else if (a.find("ax") != std::string::npos ||
             a.find("bx") != std::string::npos ||
             a.find("cx") != std::string::npos ||
             a.find("dx") != std::string::npos ||
             a.find("sp") != std::string::npos ||
             a.find("bp") != std::string::npos ||
             a.find("si") != std::string::npos ||
             a.find("di") != std::string::npos ||
             a.find("al") != std::string::npos ||
             a.find("bl") != std::string::npos ||
             a.find("cl") != std::string::npos ||
             a.find("dl") != std::string::npos ||
             a.find("ah") != std::string::npos ||
             a.find("bh") != std::string::npos ||
             a.find("ch") != std::string::npos ||
             a.find("dh") != std::string::npos) {
    op1 = op_type::regs;
  } else
    op1 = op_type::data;

  if (op1 == op_type::regs || op1 == op_type::accum) {
    char com1 = 0;
    com1 |= 0b01010000;

    if (a == "ax") com1 |= 0b00000000;
    if (a == "cx") com1 |= 0b00000001;
    if (a == "dx") com1 |= 0b00000010;
    if (a == "bx") com1 |= 0b00000011;
    if (a == "sp") com1 |= 0b00000100;
    if (a == "bp") com1 |= 0b00000101;
    if (a == "si") com1 |= 0b00000110;
    if (a == "di") com1 |= 0b00000111;
    char_coms.push_back(com1);
    return;
  }

  if (op1 == op_type::memory) {
    char com1 = 0, com2 = 0, com3 = 0, com4 = 0;
    std::string word;
    std::vector<std::string> vec;
    int flag = 0;
    com1 |= 0b11111111;
    if (a.find("[bx+si]") != std::string::npos)
      com2 |= 0b00110000;
    else if (a.find("[bx+di]") != std::string::npos)
      com2 |= 0b00110001;
    else if (a.find("[bp+si]") != std::string::npos)
      com2 |= 0b00110010;
    else if (a.find("[bp+di]") != std::string::npos)
      com2 |= 0b00110011;
    else if (a.find("[si]") != std::string::npos)
      com2 |= 0b00110100;
    else if (a.find("[di]") != std::string::npos)
      com2 |= 0b00110101;
    else if (a.find("[bx]") != std::string::npos)
      com2 |= 0b00110111;
    else {
      flag = 1;
      std::replace(a.begin(), a.end(), '+', ' ');
      std::replace(a.begin(), a.end(), '[', ' ');
      std::replace(a.begin(), a.end(), ']', ' ');
      std::stringstream ss(a);
      while (ss >> word) {
        vec.push_back(word);
      }
      if (vec.size() == 1) {
        com2 |= 0b00110110;
        if (vec[0].size() < 4) {
          vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
        }
        std::string firstPair = vec[0].substr(0, 2);
        std::string secondPair = vec[0].substr(2, 2);
        com3 = std::stoi(secondPair, nullptr, 16);
        com4 = std::stoi(firstPair, nullptr, 16);
      } else if ((vec.size() == 2 && vec[1].size() <= 2) ||
                 (vec.size() == 3 && vec[2].size() <= 2)) {
        if (vec[0] == "bx" && vec.size() == 2)
          com2 |= 0b01110111;
        else if (vec[0] == "bp" && vec.size() == 2)
          com2 |= 0b01110110;
        else if (vec[0] == "di" && vec.size() == 2)
          com2 |= 0b01110101;
        else if (vec[0] == "si" && vec.size() == 2)
          com2 |= 0b01110100;
        else if (vec[0] == "bx" && vec[1] == "si")
          com2 |= 0b01110000;
        else if (vec[0] == "bx" && vec[1] == "di")
          com2 |= 0b01110001;
        else if (vec[0] == "bp" && vec[1] == "si")
          com2 |= 0b01110010;
        else if (vec[0] == "bp" && vec[1] == "di")
          com2 |= 0b01110011;
        if (vec.size() == 2) {
          flag = 2;
          if (vec[1].size() < 2) {
            vec[1].insert(vec[1].begin(), 2 - vec[1].size(), '0');
          }
          std::string firstPair = vec[1].substr(0, 2);
          com3 = std::stoi(firstPair, nullptr, 16);
        } else {
          flag = 2;
          if (vec[2].size() < 2) {
            vec[2].insert(vec[2].begin(), 2 - vec[2].size(), '0');
          }
          std::string firstPair = vec[2].substr(0, 2);
          com3 = std::stoi(firstPair, nullptr, 16);
        }
      } else if ((vec.size() == 2 && vec[1].size() <= 4 && vec[1].size() > 2) ||
                 (vec.size() == 3 && vec[2].size() <= 4 && vec[2].size() > 2)) {
        if (vec[0] == "bx" && vec.size() == 2)
          com2 |= 0b10110111;
        else if (vec[0] == "bp" && vec.size() == 2)
          com2 |= 0b10110110;
        else if (vec[0] == "di" && vec.size() == 2)
          com2 |= 0b10110101;
        else if (vec[0] == "si" && vec.size() == 2)
          com2 |= 0b10110100;
        else if (vec[0] == "bx" && vec[1] == "si")
          com2 |= 0b10110000;
        else if (vec[0] == "bx" && vec[1] == "di")
          com2 |= 0b10110001;
        else if (vec[0] == "bp" && vec[1] == "si")
          com2 |= 0b10110010;
        else if (vec[0] == "bp" && vec[1] == "di")
          com2 |= 0b10110011;
        if (vec.size() == 2) {
          if (vec[1].size() < 4) {
            vec[1].insert(vec[1].begin(), 4 - vec[1].size(), '0');
          }
          std::string firstPair = vec[1].substr(0, 2);
          std::string secondPair = vec[1].substr(2, 2);
          com3 = std::stoi(secondPair, nullptr, 16);
          com4 = std::stoi(firstPair, nullptr, 16);
        } else {
          if (vec[2].size() < 4) {
            vec[2].insert(vec[2].begin(), 4 - vec[2].size(), '0');
          }
          std::string firstPair = vec[2].substr(0, 2);
          std::string secondPair = vec[2].substr(2, 2);
          com3 = std::stoi(secondPair, nullptr, 16);
          com4 = std::stoi(firstPair, nullptr, 16);
        }
      }
    }
    char_coms.push_back(com1);
    char_coms.push_back(com2);
    if (flag == 1) {
      char_coms.push_back(com3);
      char_coms.push_back(com4);
    } else if (flag == 2) {
      char_coms.push_back(com3);
    }
    return;
  }
  if (op1 == op_type::segment) {
    char com1 = 0;
    com1 |= 0b00000110;

    if (a == "es") com1 |= 0b00000000;
    if (a == "cs") com1 |= 0b00001000;
    if (a == "ss") com1 |= 0b00010000;
    if (a == "ds") com1 |= 0b00011000;
    char_coms.push_back(com1);
    return;
  }
}
//??????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
void pop(std::string a) {
  op_type op1;
  if (a.find("ax") != std::string::npos) {
    op1 = op_type::accum;
  } else if (a.find("[") != std::string::npos) {
    op1 = op_type::memory;
  } else if (a.find("es") != std::string::npos ||
             a.find("ss") != std::string::npos ||
             a.find("cs") != std::string::npos ||
             a.find("ds") != std::string::npos) {
    op1 = op_type::segment;
  } else if (a.find("ax") != std::string::npos ||
             a.find("bx") != std::string::npos ||
             a.find("cx") != std::string::npos ||
             a.find("dx") != std::string::npos ||
             a.find("sp") != std::string::npos ||
             a.find("bp") != std::string::npos ||
             a.find("si") != std::string::npos ||
             a.find("di") != std::string::npos ||
             a.find("al") != std::string::npos ||
             a.find("bl") != std::string::npos ||
             a.find("cl") != std::string::npos ||
             a.find("dl") != std::string::npos ||
             a.find("ah") != std::string::npos ||
             a.find("bh") != std::string::npos ||
             a.find("ch") != std::string::npos ||
             a.find("dh") != std::string::npos) {
    op1 = op_type::regs;
  } else
    op1 = op_type::data;

  if (op1 == op_type::regs || op1 == op_type::accum) {
    char com1 = 0;
    com1 |= 0b01011000;

    if (a == "ax") com1 |= 0b00000000;
    if (a == "cx") com1 |= 0b00000001;
    if (a == "dx") com1 |= 0b00000010;
    if (a == "bx") com1 |= 0b00000011;
    if (a == "sp") com1 |= 0b00000100;
    if (a == "bp") com1 |= 0b00000101;
    if (a == "si") com1 |= 0b00000110;
    if (a == "di") com1 |= 0b00000111;
    char_coms.push_back(com1);
    return;
  }

  if (op1 == op_type::memory) {
    char com1 = 0, com2 = 0, com3 = 0, com4 = 0;
    std::string word;
    std::vector<std::string> vec;
    int flag = 0;
    com1 |= 0b10001111;
    if (a.find("[bx+si]") != std::string::npos)
      com2 |= 0b00000000;
    else if (a.find("[bx+di]") != std::string::npos)
      com2 |= 0b00000001;
    else if (a.find("[bp+si]") != std::string::npos)
      com2 |= 0b00000010;
    else if (a.find("[bp+di]") != std::string::npos)
      com2 |= 0b00000011;
    else if (a.find("[si]") != std::string::npos)
      com2 |= 0b00000100;
    else if (a.find("[di]") != std::string::npos)
      com2 |= 0b00000101;
    else if (a.find("[bx]") != std::string::npos)
      com2 |= 0b00000111;
    else {
      flag = 1;
      std::replace(a.begin(), a.end(), '+', ' ');
      std::replace(a.begin(), a.end(), '[', ' ');
      std::replace(a.begin(), a.end(), ']', ' ');
      std::stringstream ss(a);
      while (ss >> word) {
        vec.push_back(word);
      }
      if (vec.size() == 1) {
        com2 |= 0b00000110;
        if (vec[0].size() < 4) {
          vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
        }
        std::string firstPair = vec[0].substr(0, 2);
        std::string secondPair = vec[0].substr(2, 2);
        com3 = std::stoi(secondPair, nullptr, 16);
        com4 = std::stoi(firstPair, nullptr, 16);
      } else if ((vec.size() == 2 && vec[1].size() <= 2) ||
                 (vec.size() == 3 && vec[2].size() <= 2)) {
        if (vec[0] == "bx" && vec.size() == 2)
          com2 |= 0b01000111;
        else if (vec[0] == "bp" && vec.size() == 2)
          com2 |= 0b01000110;
        else if (vec[0] == "di" && vec.size() == 2)
          com2 |= 0b01000101;
        else if (vec[0] == "si" && vec.size() == 2)
          com2 |= 0b01000100;
        else if (vec[0] == "bx" && vec[1] == "si")
          com2 |= 0b01000000;
        else if (vec[0] == "bx" && vec[1] == "di")
          com2 |= 0b01000001;
        else if (vec[0] == "bp" && vec[1] == "si")
          com2 |= 0b01000010;
        else if (vec[0] == "bp" && vec[1] == "di")
          com2 |= 0b01000011;
        if (vec.size() == 2) {
          flag = 2;
          if (vec[1].size() < 2) {
            vec[1].insert(vec[1].begin(), 2 - vec[1].size(), '0');
          }
          std::string firstPair = vec[1].substr(0, 2);
          com3 = std::stoi(firstPair, nullptr, 16);
        } else {
          flag = 2;
          if (vec[2].size() < 2) {
            vec[2].insert(vec[2].begin(), 2 - vec[2].size(), '0');
          }
          std::string firstPair = vec[2].substr(0, 2);
          com3 = std::stoi(firstPair, nullptr, 16);
        }
      } else if ((vec.size() == 2 && vec[1].size() <= 4 && vec[1].size() > 2) ||
                 (vec.size() == 3 && vec[2].size() <= 4 && vec[2].size() > 2)) {
        if (vec[0] == "bx" && vec.size() == 2)
          com2 |= 0b10000111;
        else if (vec[0] == "bp" && vec.size() == 2)
          com2 |= 0b10000110;
        else if (vec[0] == "di" && vec.size() == 2)
          com2 |= 0b10000101;
        else if (vec[0] == "si" && vec.size() == 2)
          com2 |= 0b10000100;
        else if (vec[0] == "bx" && vec[1] == "si")
          com2 |= 0b10000000;
        else if (vec[0] == "bx" && vec[1] == "di")
          com2 |= 0b10000001;
        else if (vec[0] == "bp" && vec[1] == "si")
          com2 |= 0b10000010;
        else if (vec[0] == "bp" && vec[1] == "di")
          com2 |= 0b10000011;
        if (vec.size() == 2) {
          if (vec[1].size() < 4) {
            vec[1].insert(vec[1].begin(), 4 - vec[1].size(), '0');
          }
          std::string firstPair = vec[1].substr(0, 2);
          std::string secondPair = vec[1].substr(2, 2);
          com3 = std::stoi(secondPair, nullptr, 16);
          com4 = std::stoi(firstPair, nullptr, 16);
        } else {
          if (vec[2].size() < 4) {
            vec[2].insert(vec[2].begin(), 4 - vec[2].size(), '0');
          }
          std::string firstPair = vec[2].substr(0, 2);
          std::string secondPair = vec[2].substr(2, 2);
          com3 = std::stoi(secondPair, nullptr, 16);
          com4 = std::stoi(firstPair, nullptr, 16);
        }
      }
    }
    char_coms.push_back(com1);
    char_coms.push_back(com2);
    if (flag == 1) {
      char_coms.push_back(com3);
      char_coms.push_back(com4);
    } else if (flag == 2) {
      char_coms.push_back(com3);
    }
    return;
  }
  if (op1 == op_type::segment) {
    char com1 = 0;
    com1 |= 0b00000111;

    if (a == "es") com1 |= 0b00000000;
    if (a == "cs") com1 |= 0b00001000;
    if (a == "ss") com1 |= 0b00010000;
    if (a == "ds") com1 |= 0b00011000;
    char_coms.push_back(com1);
    return;
  }
}
//???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
void mul(std::string a) {
  if (a.find("ax") != std::string::npos || a.find("bx") != std::string::npos ||
      a.find("cx") != std::string::npos || a.find("dx") != std::string::npos ||
      a.find("sp") != std::string::npos || a.find("bp") != std::string::npos ||
      a.find("si") != std::string::npos || a.find("di") != std::string::npos ||
      a.find("al") != std::string::npos || a.find("bl") != std::string::npos ||
      a.find("cl") != std::string::npos || a.find("dl") != std::string::npos ||
      a.find("ah") != std::string::npos || a.find("bh") != std::string::npos ||
      a.find("ch") != std::string::npos || a.find("dh") != std::string::npos) {
    char com1 = 0, com2 = 0;
    com1 |= 0b11110110;
    com2 |= 0b11100000;
    if (!(a.find("l") != std::string::npos ||
          a.find("h") != std::string::npos)) {
      com1 |= 0b00000001;
    }
    if (a == "ax" || a == "al") com2 |= 0b00000000;
    if (a == "cx" || a == "cl") com2 |= 0b00000001;
    if (a == "dx" || a == "dl") com2 |= 0b00000010;
    if (a == "bx" || a == "bl") com2 |= 0b00000011;
    if (a == "sp" || a == "ah") com2 |= 0b00000100;
    if (a == "bp" || a == "ch") com2 |= 0b00000101;
    if (a == "si" || a == "dh") com2 |= 0b00000110;
    if (a == "di" || a == "bh") com2 |= 0b00000111;
    char_coms.push_back(com1);
    char_coms.push_back(com2);
    return;
  }
}
//????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
void cmp(std::string a, std::string b) {
  op_type op1, op2;
  if (a.find("ax") != std::string::npos) {
    op1 = op_type::accum;
  } else if (a.find("[") != std::string::npos) {
    op1 = op_type::memory;
  } else if (a.find("es") != std::string::npos ||
             a.find("ss") != std::string::npos ||
             a.find("cs") != std::string::npos ||
             a.find("ds") != std::string::npos) {
    op1 = op_type::segment;
  } else if (a.find("ax") != std::string::npos ||
             a.find("bx") != std::string::npos ||
             a.find("cx") != std::string::npos ||
             a.find("dx") != std::string::npos ||
             a.find("sp") != std::string::npos ||
             a.find("bp") != std::string::npos ||
             a.find("si") != std::string::npos ||
             a.find("di") != std::string::npos ||
             a.find("al") != std::string::npos ||
             a.find("bl") != std::string::npos ||
             a.find("cl") != std::string::npos ||
             a.find("dl") != std::string::npos ||
             a.find("ah") != std::string::npos ||
             a.find("bh") != std::string::npos ||
             a.find("ch") != std::string::npos ||
             a.find("dh") != std::string::npos) {
    op1 = op_type::regs;
  } else
    op1 = op_type::data;

  if (b.find("ax") != std::string::npos) {
    op2 = op_type::accum;
  } else if (b.find("[") != std::string::npos) {
    op2 = op_type::memory;
  } else if (b.find("es") != std::string::npos ||
             b.find("ss") != std::string::npos ||
             b.find("cs") != std::string::npos ||
             b.find("ds") != std::string::npos) {
    op2 = op_type::segment;
  } else if (b.find("ax") != std::string::npos ||
             b.find("bx") != std::string::npos ||
             b.find("cx") != std::string::npos ||
             b.find("dx") != std::string::npos ||
             b.find("sp") != std::string::npos ||
             b.find("bp") != std::string::npos ||
             b.find("si") != std::string::npos ||
             b.find("di") != std::string::npos ||
             b.find("al") != std::string::npos ||
             b.find("bl") != std::string::npos ||
             b.find("cl") != std::string::npos ||
             b.find("dl") != std::string::npos ||
             b.find("ah") != std::string::npos ||
             b.find("bh") != std::string::npos ||
             b.find("ch") != std::string::npos ||
             b.find("dh") != std::string::npos) {
    op2 = op_type::regs;
  } else
    op2 = op_type::data;

  if ((op1 == op_type::regs || op1 == op_type::accum) &&
      (op2 == op_type::regs || op2 == op_type::accum)) {
    char com1 = 0, com2 = 0;
    com1 = com1 | 0b00111000;
    com2 |= 0b11000000;
    if (!(a.find("l") != std::string::npos ||
          a.find("h") != std::string::npos)) {
      com1 |= 0b00000001;
    }
    if (a == "ax" || a == "al") com2 |= 0b00000000;
    if (a == "cx" || a == "cl") com2 |= 0b00000001;
    if (a == "dx" || a == "dl") com2 |= 0b00000010;
    if (a == "bx" || a == "bl") com2 |= 0b00000011;
    if (a == "sp" || a == "ah") com2 |= 0b00000100;
    if (a == "bp" || a == "ch") com2 |= 0b00000101;
    if (a == "si" || a == "dh") com2 |= 0b00000110;
    if (a == "di" || a == "bh") com2 |= 0b00000111;
    if (b == "ax" || b == "al") com2 |= 0b00000000;
    if (b == "cx" || b == "cl") com2 |= 0b00001000;
    if (b == "dx" || b == "dl") com2 |= 0b00010000;
    if (b == "bx" || b == "bl") com2 |= 0b00011000;
    if (b == "sp" || b == "ah") com2 |= 0b00100000;
    if (b == "bp" || b == "ch") com2 |= 0b00101000;
    if (b == "si" || b == "dh") com2 |= 0b00110000;
    if (b == "di" || b == "bh") com2 |= 0b00111000;
    char_coms.push_back(com1);
    char_coms.push_back(com2);
    return;
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////
  if (((op1 == op_type::regs || op1 == op_type::accum) &&
       op2 == op_type::memory) ||
      ((op2 == op_type::regs || op2 == op_type::accum) &&
       op1 == op_type::memory)) {
    char com1 = 0, com2 = 0, com3 = 0, com4 = 0;
    std::string word;
    std::vector<std::string> vec;
    int flag = 0;
    if (op1 == op_type::regs || op1 == op_type::accum) {
      com1 |= 0b00111011;
      com2 |= 0b00000000;
      if (!(a.find("l") != std::string::npos ||
            a.find("h") != std::string::npos)) {
        com1 |= 0b00000001;
      }
      if (a == "ax" || a == "al") com2 |= 0b00000000;
      if (a == "cx" || a == "cl") com2 |= 0b00001000;
      if (a == "dx" || a == "dl") com2 |= 0b00010000;
      if (a == "bx" || a == "bl") com2 |= 0b00011000;
      if (a == "sp" || a == "ah") com2 |= 0b00100000;
      if (a == "bp" || a == "ch") com2 |= 0b00101000;
      if (a == "si" || a == "dh") com2 |= 0b00110000;
      if (a == "di" || a == "bh") com2 |= 0b00111000;

      if (b.find("[bx+si]") != std::string::npos)
        com2 |= 0b00000000;
      else if (b.find("[bx+di]") != std::string::npos)
        com2 |= 0b00000001;
      else if (b.find("[bp+si]") != std::string::npos)
        com2 |= 0b00000010;
      else if (b.find("[bp+di]") != std::string::npos)
        com2 |= 0b00000011;
      else if (b.find("[si]") != std::string::npos)
        com2 |= 0b00000100;
      else if (b.find("[di]") != std::string::npos)
        com2 |= 0b00000101;
      else if (b.find("[bx]") != std::string::npos)
        com2 |= 0b00000111;
      else {
        flag = 1;
        std::replace(b.begin(), b.end(), '+', ' ');
        std::replace(b.begin(), b.end(), '[', ' ');
        std::replace(b.begin(), b.end(), ']', ' ');
        std::stringstream ss(b);
        while (ss >> word) {
          vec.push_back(word);
        }
        if (vec.size() == 1) {
          com2 |= 0b00000110;
          if (vec[0].size() < 4) {
            vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
          }
          std::string firstPair = vec[0].substr(0, 2);
          std::string secondPair = vec[0].substr(2, 2);
          com3 = std::stoi(secondPair, nullptr, 16);
          com4 = std::stoi(firstPair, nullptr, 16);
        } else if ((vec.size() == 2 && vec[1].size() <= 2) ||
                   (vec.size() == 3 && vec[2].size() <= 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b01000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b01000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b01000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b01000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b01000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b01000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b01000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b01000011;
          if (vec.size() == 2) {
            flag = 2;
            if (vec[1].size() < 2) {
              vec[1].insert(vec[1].begin(), 2 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);

          } else {
            flag = 2;
            if (vec[2].size() < 2) {
              vec[2].insert(vec[2].begin(), 2 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);
          }
        } else if ((vec.size() == 2 && vec[1].size() <= 4 &&
                    vec[1].size() > 2) ||
                   (vec.size() == 3 && vec[2].size() <= 4 &&
                    vec[2].size() > 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b10000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b10000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b10000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b10000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b10000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b10000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b10000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b10000011;
          if (vec.size() == 2) {
            if (vec[1].size() < 4) {
              vec[1].insert(vec[1].begin(), 4 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            std::string secondPair = vec[1].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          } else {
            flag = 2;
            if (vec[2].size() < 4) {
              vec[2].insert(vec[2].begin(), 4 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            std::string secondPair = vec[2].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          }
        }
      }
      // com3 младшая половина
      // com4 старшая
      char_coms.push_back(com1);
      char_coms.push_back(com2);
      if (flag == 1) {
        char_coms.push_back(com3);
        char_coms.push_back(com4);
      } else if (flag == 2) {
        char_coms.push_back(com3);
      }
    } else {
      com1 |= 0b00111000;
      com2 |= 0b00000000;
      if (!(b.find("l") != std::string::npos ||
            b.find("h") != std::string::npos)) {
        com1 |= 0b00000001;
      }
      if (b == "ax" || b == "al") com2 |= 0b00000000;
      if (b == "cx" || b == "cl") com2 |= 0b00001000;
      if (b == "dx" || b == "dl") com2 |= 0b00010000;
      if (b == "bx" || b == "bl") com2 |= 0b00011000;
      if (b == "sp" || b == "ah") com2 |= 0b00100000;
      if (b == "bp" || b == "ch") com2 |= 0b00101000;
      if (b == "si" || b == "dh") com2 |= 0b00110000;
      if (b == "di" || b == "bh") com2 |= 0b00111000;

      if (a.find("[bx+si]") != std::string::npos)
        com2 |= 0b00000000;
      else if (a.find("[bx+di]") != std::string::npos)
        com2 |= 0b00000001;
      else if (a.find("[bp+si]") != std::string::npos)
        com2 |= 0b00000010;
      else if (a.find("[bp+di]") != std::string::npos)
        com2 |= 0b00000011;
      else if (a.find("[si]") != std::string::npos)
        com2 |= 0b00000100;
      else if (a.find("[di]") != std::string::npos)
        com2 |= 0b00000101;
      else if (a.find("[bx]") != std::string::npos)
        com2 |= 0b00000111;
      else {
        flag = 1;
        std::replace(a.begin(), a.end(), '+', ' ');
        std::replace(a.begin(), a.end(), '[', ' ');
        std::replace(a.begin(), a.end(), ']', ' ');
        std::stringstream ss(a);
        while (ss >> word) {
          vec.push_back(word);
        }
        if (vec.size() == 1) {
          com2 |= 0b00000110;
          if (vec[0].size() < 4) {
            vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
          }
          std::string firstPair = vec[0].substr(0, 2);
          std::string secondPair = vec[0].substr(2, 2);
          com3 = std::stoi(secondPair, nullptr, 16);
          com4 = std::stoi(firstPair, nullptr, 16);
        } else if ((vec.size() == 2 && vec[1].size() <= 2) ||
                   (vec.size() == 3 && vec[2].size() <= 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b01000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b01000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b01000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b01000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b01000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b01000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b01000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b01000011;
          if (vec.size() == 2) {
            flag = 2;
            if (vec[1].size() < 2) {
              vec[1].insert(vec[1].begin(), 2 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);
          } else {
            flag = 2;
            if (vec[2].size() < 2) {
              vec[2].insert(vec[2].begin(), 2 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);
          }
        } else if ((vec.size() == 2 && vec[1].size() <= 4 &&
                    vec[1].size() > 2) ||
                   (vec.size() == 3 && vec[2].size() <= 4 &&
                    vec[2].size() > 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b10000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b10000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b10000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b10000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b10000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b10000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b10000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b10000011;
          if (vec.size() == 2) {
            if (vec[1].size() < 4) {
              vec[1].insert(vec[1].begin(), 4 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            std::string secondPair = vec[1].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          } else {
            if (vec[2].size() < 4) {
              vec[2].insert(vec[2].begin(), 4 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            std::string secondPair = vec[2].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          }
        }
      }
      char_coms.push_back(com1);
      char_coms.push_back(com2);
      if (flag == 1) {
        char_coms.push_back(com3);
        char_coms.push_back(com4);
      } else if (flag == 2) {
        char_coms.push_back(com3);
      }
    }
    return;
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  if (op1 == op_type::regs && op2 == op_type::data) {
    char com1 = 0, com2 = 0, com3 = 0, com4 = 0;
    std::string word;
    std::vector<std::string> vec;
    bool flag = false;
    com1 |= 0b10000000;
    com2 |= 0b11111000;
    if (a == "ax" || a == "al") com2 |= 0b00000000;
    if (a == "cx" || a == "cl") com2 |= 0b00000001;
    if (a == "dx" || a == "dl") com2 |= 0b00000010;
    if (a == "bx" || a == "bl") com2 |= 0b00000011;
    if (a == "sp" || a == "ah") com2 |= 0b00000100;
    if (a == "bp" || a == "ch") com2 |= 0b00000101;
    if (a == "si" || a == "dh") com2 |= 0b00000110;
    if (a == "di" || a == "bh") com2 |= 0b00000111;
    std::stringstream ss(b);
    while (ss >> word) {
      vec.push_back(word);
    }
    if (vec.size() ==
        1) {  // спросить по поводу добавления условия если число <4
      if (vec[0].size() <= 4 && vec[0].size() > 2) {
        com1 |= 0b00000001;
        vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
        std::string firstPair = vec[0].substr(0, 2);
        std::string secondPair = vec[0].substr(2, 2);
        com3 = std::stoi(secondPair, nullptr, 16);
        com4 = std::stoi(firstPair, nullptr, 16);
        char_coms.push_back(com1);
        char_coms.push_back(com2);
        char_coms.push_back(com3);
        char_coms.push_back(com4);
      } else if (vec[0].size() <= 2) {
        if ((a.find("l") != std::string::npos ||
             a.find("h") != std::string::npos)) {
          com1 |= 0b00000000;
        } else {
          com1 |= 0b00000011;
        }
        vec[0].insert(vec[0].begin(), 2 - vec[0].size(), '0');
        std::string firstPair = vec[0].substr(0, 2);
        com3 = std::stoi(firstPair, nullptr, 16);
        char_coms.push_back(com1);
        char_coms.push_back(com2);
        char_coms.push_back(com3);
      }
    }
    return;
  }
  if (op1 == op_type::accum && op2 == op_type::data) {
    char com1 = 0, com2 = 0, com3 = 0;
    std::string word;
    std::vector<std::string> vec;
    bool flag = false;
    com1 |= 0b00111101;
    std::stringstream ss(b);
    while (ss >> word) {
      vec.push_back(word);
    }
    if (vec.size() ==
        1) {  // спросить по поводу добавления условия если число <4
      vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
      std::string firstPair = vec[0].substr(0, 2);
      std::string secondPair = vec[0].substr(2, 2);
      com2 = std::stoi(secondPair, nullptr, 16);
      com3 = std::stoi(firstPair, nullptr, 16);
    }
    char_coms.push_back(com1);
    char_coms.push_back(com2);
    char_coms.push_back(com3);
    return;
  }
}
void hlt() {
  char com1 = 0;
  com1 |= 0b11110100;
  char_coms.push_back(com1);
  return;
}
//????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
void adc(std::string a, std::string b) {
  op_type op1, op2;
  if (a.find("ax") != std::string::npos) {
    op1 = op_type::accum;
  } else if (a.find("[") != std::string::npos) {
    op1 = op_type::memory;
  } else if (a.find("es") != std::string::npos ||
             a.find("ss") != std::string::npos ||
             a.find("cs") != std::string::npos ||
             a.find("ds") != std::string::npos) {
    op1 = op_type::segment;
  } else if (a.find("ax") != std::string::npos ||
             a.find("bx") != std::string::npos ||
             a.find("cx") != std::string::npos ||
             a.find("dx") != std::string::npos ||
             a.find("sp") != std::string::npos ||
             a.find("bp") != std::string::npos ||
             a.find("si") != std::string::npos ||
             a.find("di") != std::string::npos ||
             a.find("al") != std::string::npos ||
             a.find("bl") != std::string::npos ||
             a.find("cl") != std::string::npos ||
             a.find("dl") != std::string::npos ||
             a.find("ah") != std::string::npos ||
             a.find("bh") != std::string::npos ||
             a.find("ch") != std::string::npos ||
             a.find("dh") != std::string::npos) {
    op1 = op_type::regs;
  } else
    op1 = op_type::data;

  if (b.find("ax") != std::string::npos) {
    op2 = op_type::accum;
  } else if (b.find("[") != std::string::npos) {
    op2 = op_type::memory;
  } else if (b.find("es") != std::string::npos ||
             b.find("ss") != std::string::npos ||
             b.find("cs") != std::string::npos ||
             b.find("ds") != std::string::npos) {
    op2 = op_type::segment;
  } else if (b.find("ax") != std::string::npos ||
             b.find("bx") != std::string::npos ||
             b.find("cx") != std::string::npos ||
             b.find("dx") != std::string::npos ||
             b.find("sp") != std::string::npos ||
             b.find("bp") != std::string::npos ||
             b.find("si") != std::string::npos ||
             b.find("di") != std::string::npos ||
             b.find("al") != std::string::npos ||
             b.find("bl") != std::string::npos ||
             b.find("cl") != std::string::npos ||
             b.find("dl") != std::string::npos ||
             b.find("ah") != std::string::npos ||
             b.find("bh") != std::string::npos ||
             b.find("ch") != std::string::npos ||
             b.find("dh") != std::string::npos) {
    op2 = op_type::regs;
  } else
    op2 = op_type::data;

  if ((op1 == op_type::regs || op1 == op_type::accum) &&
      (op2 == op_type::regs || op2 == op_type::accum)) {
    char com1 = 0, com2 = 0;
    com1 = com1 | 0b00010000;
    com2 |= 0b11000000;
    if (!(a.find("l") != std::string::npos ||
          a.find("h") != std::string::npos)) {
      com1 |= 0b00000001;
    }
    if (a == "ax" || a == "al") com2 |= 0b00000000;
    if (a == "cx" || a == "cl") com2 |= 0b00000001;
    if (a == "dx" || a == "dl") com2 |= 0b00000010;
    if (a == "bx" || a == "bl") com2 |= 0b00000011;
    if (a == "sp" || a == "ah") com2 |= 0b00000100;
    if (a == "bp" || a == "ch") com2 |= 0b00000101;
    if (a == "si" || a == "dh") com2 |= 0b00000110;
    if (a == "di" || a == "bh") com2 |= 0b00000111;
    if (b == "ax" || b == "al") com2 |= 0b00000000;
    if (b == "cx" || b == "cl") com2 |= 0b00001000;
    if (b == "dx" || b == "dl") com2 |= 0b00010000;
    if (b == "bx" || b == "bl") com2 |= 0b00011000;
    if (b == "sp" || b == "ah") com2 |= 0b00100000;
    if (b == "bp" || b == "ch") com2 |= 0b00101000;
    if (b == "si" || b == "dh") com2 |= 0b00110000;
    if (b == "di" || b == "bh") com2 |= 0b00111000;
    char_coms.push_back(com1);
    char_coms.push_back(com2);
    return;
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////
  if (((op1 == op_type::regs || op1 == op_type::accum) &&
       op2 == op_type::memory) ||
      ((op2 == op_type::regs || op2 == op_type::accum) &&
       op1 == op_type::memory)) {
    char com1 = 0, com2 = 0, com3 = 0, com4 = 0;
    std::string word;
    std::vector<std::string> vec;
    int flag = 0;
    if (op1 == op_type::regs || op1 == op_type::accum) {
      com1 |= 0b00010011;
      com2 |= 0b00000000;
      if (!(a.find("l") != std::string::npos ||
            a.find("h") != std::string::npos)) {
        com1 |= 0b00000001;
      }
      if (a == "ax" || a == "al") com2 |= 0b00000000;
      if (a == "cx" || a == "cl") com2 |= 0b00001000;
      if (a == "dx" || a == "dl") com2 |= 0b00010000;
      if (a == "bx" || a == "bl") com2 |= 0b00011000;
      if (a == "sp" || a == "ah") com2 |= 0b00100000;
      if (a == "bp" || a == "ch") com2 |= 0b00101000;
      if (a == "si" || a == "dh") com2 |= 0b00110000;
      if (a == "di" || a == "bh") com2 |= 0b00111000;

      if (b.find("[bx+si]") != std::string::npos)
        com2 |= 0b00000000;
      else if (b.find("[bx+di]") != std::string::npos)
        com2 |= 0b00000001;
      else if (b.find("[bp+si]") != std::string::npos)
        com2 |= 0b00000010;
      else if (b.find("[bp+di]") != std::string::npos)
        com2 |= 0b00000011;
      else if (b.find("[si]") != std::string::npos)
        com2 |= 0b00000100;
      else if (b.find("[di]") != std::string::npos)
        com2 |= 0b00000101;
      else if (b.find("[bx]") != std::string::npos)
        com2 |= 0b00000111;
      else {
        flag = 1;
        std::replace(b.begin(), b.end(), '+', ' ');
        std::replace(b.begin(), b.end(), '[', ' ');
        std::replace(b.begin(), b.end(), ']', ' ');
        std::stringstream ss(b);
        while (ss >> word) {
          vec.push_back(word);
        }
        if (vec.size() == 1) {
          com2 |= 0b00000110;
          if (vec[0].size() < 4) {
            vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
          }
          std::string firstPair = vec[0].substr(0, 2);
          std::string secondPair = vec[0].substr(2, 2);
          com3 = std::stoi(secondPair, nullptr, 16);
          com4 = std::stoi(firstPair, nullptr, 16);
        } else if ((vec.size() == 2 && vec[1].size() <= 2) ||
                   (vec.size() == 3 && vec[2].size() <= 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b01000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b01000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b01000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b01000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b01000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b01000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b01000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b01000011;
          if (vec.size() == 2) {
            flag = 2;
            if (vec[1].size() < 2) {
              vec[1].insert(vec[1].begin(), 2 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);

          } else {
            flag = 2;
            if (vec[2].size() < 2) {
              vec[2].insert(vec[2].begin(), 2 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);
          }
        } else if ((vec.size() == 2 && vec[1].size() <= 4 &&
                    vec[1].size() > 2) ||
                   (vec.size() == 3 && vec[2].size() <= 4 &&
                    vec[2].size() > 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b10000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b10000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b10000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b10000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b10000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b10000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b10000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b10000011;
          if (vec.size() == 2) {
            if (vec[1].size() < 4) {
              vec[1].insert(vec[1].begin(), 4 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            std::string secondPair = vec[1].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          } else {
            flag = 2;
            if (vec[2].size() < 4) {
              vec[2].insert(vec[2].begin(), 4 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            std::string secondPair = vec[2].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          }
        }
      }
      // com3 младшая половина
      // com4 старшая
      char_coms.push_back(com1);
      char_coms.push_back(com2);
      if (flag == 1) {
        char_coms.push_back(com3);
        char_coms.push_back(com4);
      } else if (flag == 2) {
        char_coms.push_back(com3);
      }
    } else {
      com1 |= 0b00010000;
      com2 |= 0b00000000;
      if (!(b.find("l") != std::string::npos ||
            b.find("h") != std::string::npos)) {
        com1 |= 0b00000001;
      }
      if (b == "ax" || b == "al") com2 |= 0b00000000;
      if (b == "cx" || b == "cl") com2 |= 0b00001000;
      if (b == "dx" || b == "dl") com2 |= 0b00010000;
      if (b == "bx" || b == "bl") com2 |= 0b00011000;
      if (b == "sp" || b == "ah") com2 |= 0b00100000;
      if (b == "bp" || b == "ch") com2 |= 0b00101000;
      if (b == "si" || b == "dh") com2 |= 0b00110000;
      if (b == "di" || b == "bh") com2 |= 0b00111000;

      if (a.find("[bx+si]") != std::string::npos)
        com2 |= 0b00000000;
      else if (a.find("[bx+di]") != std::string::npos)
        com2 |= 0b00000001;
      else if (a.find("[bp+si]") != std::string::npos)
        com2 |= 0b00000010;
      else if (a.find("[bp+di]") != std::string::npos)
        com2 |= 0b00000011;
      else if (a.find("[si]") != std::string::npos)
        com2 |= 0b00000100;
      else if (a.find("[di]") != std::string::npos)
        com2 |= 0b00000101;
      else if (a.find("[bx]") != std::string::npos)
        com2 |= 0b00000111;
      else {
        flag = 1;
        std::replace(a.begin(), a.end(), '+', ' ');
        std::replace(a.begin(), a.end(), '[', ' ');
        std::replace(a.begin(), a.end(), ']', ' ');
        std::stringstream ss(a);
        while (ss >> word) {
          vec.push_back(word);
        }
        if (vec.size() == 1) {
          com2 |= 0b00000110;
          if (vec[0].size() < 4) {
            vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
          }
          std::string firstPair = vec[0].substr(0, 2);
          std::string secondPair = vec[0].substr(2, 2);
          com3 = std::stoi(secondPair, nullptr, 16);
          com4 = std::stoi(firstPair, nullptr, 16);
        } else if ((vec.size() == 2 && vec[1].size() <= 2) ||
                   (vec.size() == 3 && vec[2].size() <= 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b01000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b01000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b01000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b01000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b01000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b01000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b01000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b01000011;
          if (vec.size() == 2) {
            flag = 2;
            if (vec[1].size() < 2) {
              vec[1].insert(vec[1].begin(), 2 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);
          } else {
            flag = 2;
            if (vec[2].size() < 2) {
              vec[2].insert(vec[2].begin(), 2 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            com3 = std::stoi(firstPair, nullptr, 16);
          }
        } else if ((vec.size() == 2 && vec[1].size() <= 4 &&
                    vec[1].size() > 2) ||
                   (vec.size() == 3 && vec[2].size() <= 4 &&
                    vec[2].size() > 2)) {
          if (vec[0] == "bx" && vec.size() == 2)
            com2 |= 0b10000111;
          else if (vec[0] == "bp" && vec.size() == 2)
            com2 |= 0b10000110;
          else if (vec[0] == "di" && vec.size() == 2)
            com2 |= 0b10000101;
          else if (vec[0] == "si" && vec.size() == 2)
            com2 |= 0b10000100;
          else if (vec[0] == "bx" && vec[1] == "si")
            com2 |= 0b10000000;
          else if (vec[0] == "bx" && vec[1] == "di")
            com2 |= 0b10000001;
          else if (vec[0] == "bp" && vec[1] == "si")
            com2 |= 0b10000010;
          else if (vec[0] == "bp" && vec[1] == "di")
            com2 |= 0b10000011;
          if (vec.size() == 2) {
            if (vec[1].size() < 4) {
              vec[1].insert(vec[1].begin(), 4 - vec[1].size(), '0');
            }
            std::string firstPair = vec[1].substr(0, 2);
            std::string secondPair = vec[1].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          } else {
            if (vec[2].size() < 4) {
              vec[2].insert(vec[2].begin(), 4 - vec[2].size(), '0');
            }
            std::string firstPair = vec[2].substr(0, 2);
            std::string secondPair = vec[2].substr(2, 2);
            com3 = std::stoi(secondPair, nullptr, 16);
            com4 = std::stoi(firstPair, nullptr, 16);
          }
        }
      }
      char_coms.push_back(com1);
      char_coms.push_back(com2);
      if (flag == 1) {
        char_coms.push_back(com3);
        char_coms.push_back(com4);
      } else if (flag == 2) {
        char_coms.push_back(com3);
      }
    }
    return;
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  if (op1 == op_type::regs && op2 == op_type::data) {
    char com1 = 0, com2 = 0, com3 = 0, com4 = 0;
    std::string word;
    std::vector<std::string> vec;
    bool flag = false;
    com1 |= 0b10000000;
    com2 |= 0b11010000;
    if (a == "ax" || a == "al") com2 |= 0b00000000;
    if (a == "cx" || a == "cl") com2 |= 0b00000001;
    if (a == "dx" || a == "dl") com2 |= 0b00000010;
    if (a == "bx" || a == "bl") com2 |= 0b00000011;
    if (a == "sp" || a == "ah") com2 |= 0b00000100;
    if (a == "bp" || a == "ch") com2 |= 0b00000101;
    if (a == "si" || a == "dh") com2 |= 0b00000110;
    if (a == "di" || a == "bh") com2 |= 0b00000111;
    std::stringstream ss(b);
    while (ss >> word) {
      vec.push_back(word);
    }
    if (vec.size() ==
        1) {  // спросить по поводу добавления условия если число <4
      if (vec[0].size() <= 4 && vec[0].size() > 2) {
        com1 |= 0b00000001;
        vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
        std::string firstPair = vec[0].substr(0, 2);
        std::string secondPair = vec[0].substr(2, 2);
        com3 = std::stoi(secondPair, nullptr, 16);
        com4 = std::stoi(firstPair, nullptr, 16);
        char_coms.push_back(com1);
        char_coms.push_back(com2);
        char_coms.push_back(com3);
        char_coms.push_back(com4);
      } else if (vec[0].size() <= 2) {
        if ((a.find("l") != std::string::npos ||
             a.find("h") != std::string::npos)) {
          com1 |= 0b00000000;
        } else {
          com1 |= 0b00000011;
        }
        vec[0].insert(vec[0].begin(), 2 - vec[0].size(), '0');
        std::string firstPair = vec[0].substr(0, 2);
        com3 = std::stoi(firstPair, nullptr, 16);
        char_coms.push_back(com1);
        char_coms.push_back(com2);
        char_coms.push_back(com3);
      }
    }
    return;
  }
  if (op1 == op_type::accum && op2 == op_type::data) {
    char com1 = 0, com2 = 0, com3 = 0;
    std::string word;
    std::vector<std::string> vec;
    bool flag = false;
    com1 |= 0b00010101;
    std::stringstream ss(b);
    while (ss >> word) {
      vec.push_back(word);
    }
    if (vec.size() ==
        1) {  // спросить по поводу добавления условия если число <4
      vec[0].insert(vec[0].begin(), 4 - vec[0].size(), '0');
      std::string firstPair = vec[0].substr(0, 2);
      std::string secondPair = vec[0].substr(2, 2);
      com2 = std::stoi(secondPair, nullptr, 16);
      com3 = std::stoi(firstPair, nullptr, 16);
    }
    char_coms.push_back(com1);
    char_coms.push_back(com2);
    char_coms.push_back(com3);
    return;
  }
}
void jmp(std::string a) {
  unsigned short int b;
  std::stringstream s;
  s << std::hex << a;
  s >> b;
  char com1 = 0b11101011;
  char com2 = (unsigned char)(b - char_coms.size() - 2);
  char_coms.push_back(com1);
  char_coms.push_back(com2);
}
void loop(std::string a) {
  unsigned short int b;
  std::stringstream s;
  s << std::hex << a;
  s >> b;
  char com1 = 0b11100010;
  char com2 = (unsigned char)(b - char_coms.size() - 2);
  char_coms.push_back(com1);
  char_coms.push_back(com2);
}
void call(std::string a) {
  unsigned short int b;
  std::stringstream s;
  s << std::hex << a;
  s >> b;
  char com1 = 0b11101000;
  short int c = (short int)(b - char_coms.size() - 3);
  char com2 = c & 0x00ff;
  char com3 = (c >> 8) & 0x00ff;
  char_coms.push_back(com1);
  char_coms.push_back(com2);
  char_coms.push_back(com3);
}
void ret(std::string a) {
  unsigned short int b;
  std::stringstream s;
  s << std::hex << a;
  s >> b;
  char com1 = 0b11000011;
  char com2 = (unsigned char)(b - char_coms.size() - 2);
  char_coms.push_back(com1);
  char_coms.push_back(com2);
}
void jg(std::string a) {
  unsigned short int b;
  std::stringstream s;
  s << std::hex << a;
  s >> b;
  char com1 = 0b01111111;
  char com2 = (unsigned char)(b - char_coms.size() - 2);
  char_coms.push_back(com1);
  char_coms.push_back(com2);
}
void jz(std::string a) {
  unsigned short int b;
  std::stringstream s;
  s << std::hex << a;
  s >> b;
  char com1 = 0b01110100;
  char com2 = (unsigned char)(b - char_coms.size() - 2);
  char_coms.push_back(com1);
  char_coms.push_back(com2);
}