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
      vector<string> name=split(strs[3],':');
      cout << name[0] << "\t" << name[1] << "\t";
      cout << atoi(strs[1].c_str())+1 << "\t";
      cout << strs[2] << "\t";
      cout << strs[4] << "\t" << strs[5] << "\t";
      cout << name[2];
      if(name.size()>3) cout << "\t" << name[3];
      cout << "\n";
    }
  }
  return 0;
}
