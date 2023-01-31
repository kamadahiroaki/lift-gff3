#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
using namespace std;

vector<string> split(string str,char del){
  int first=0;
  int last=0;
  vector<string> result;
  while(true){
    if(first>=str.length()) break;
    last=str.find_first_of(del,first);
    if(last==string::npos) last=str.length();
    result.push_back(str.substr(first,last-first));
    first=last+1;
  }
  return result;
}

int main(int argc, char *argv[])
{
  string str;

  while(getline(cin,str)){
    if(str==""){
      cout << "\n";
    }else if(str[0]=='#'){
      cout << str << "\n";
    }else{
      vector<string> strs=split(str,'\t');
      cout << strs[0] << "\t";
      cout << atoi(strs[3].c_str())-1 << "\t";
      cout << strs[4] << "\t";
      //      cout << strs[1] << ":" << strs[2] << ":" << strs[7] << ":" << strs[8] << "\t";
      cout << strs[1] << ":" << strs[2] << ":" << strs[7] << ":";
      if(strs.size()>8){
	cout << strs[8];
      }
      cout << "\t";
      cout << strs[5] << "\t" << strs[6] << "\n";
    }
  }
  return 0;
}
