#include <cstdio>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
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

class Record{
public:
  string str;
  string seqid;
  string source;
  string type;
  int start;
  int end;
  string score;
  string strand;
  string phase;
  string attributes;
  string id;
  string parent;
  Record(){}
  Record(string ss){
    str=ss;
    vector<string> s=split(ss,'\t');
    if(s.size()<8){
      cerr << "format error: " << ss << endl;
      return;
    }
    seqid=s[0];
    source=s[1];
    type=s[2];
    start=atoi(s[3].c_str());
    end=atoi(s[4].c_str());
    score=s[5];
    strand=s[6];
    phase=s[7];
    attributes=id=parent="";
    if(s.size()==9){
      attributes=s[8];
      vector<string> attr=split(s[8],';');
      for(int i=0; i<attr.size(); i++){
	if(attr[i].size()>3 && attr[i].substr(0,3)=="ID="){
	  id=attr[i].substr(3);
	}
	if(attr[i].size()>6 && attr[i].substr(0,7)=="Parent="){
	  parent=attr[i].substr(7);
	}
      }
    }
  }
};

bool cmp(const Record &a, const Record &b)
{
  if(a.seqid!=b.seqid){
    return a.seqid<b.seqid;
  }else{
    if(a.parent==""){
      if(a.start==b.start){
	return a.end<b.end;
      }else{
	return a.start<b.start;
      }
    }else if(a.type==b.type){
      if(a.strand=="+"){
	if(a.start==b.start){
	  return a.end<b.end;
	}else{
	  return a.start<b.start;
	}
      }else{
	if(a.start==b.start){
	  return a.end>b.end;
	}else{
	  return a.start>b.start;
	}
      }
    }else if(a.type=="five_prime_UTR"){
      return true;
    }else if(b.type=="five_prime_UTR"){
      return false;
    }else if(a.type=="three_prime_UTR"){
      return false;
    }else if(b.type=="three_prime_UTR"){
      return true;
    }else if((a.type=="EXON" || a.type=="exon") && (b.type=="CDS" || b.type=="cds") && (a.start<=b.start && b.end<=a.end)){
      return true;
    }else if((a.type=="CDS" || a.type=="cds") && (b.type=="EXON" || b.type=="exon") && (b.start<=a.start && a.end<=b.end)){
      return false;
    }else{
      if(a.start==b.start && a.end==b.end){
	if(a.type=="CDS" || a.type=="cds"){
	  return false;
	}else{
	  return true;
	}
      }else{
	if(a.strand=="+"){
	  if(a.start!=b.start){
	    return a.start<b.start;
	  }else{
	    return a.end<b.end;
	  }
	}else{
	  if(a.start!=b.start){
	    return b.start<a.start;
	  }else{
	    return b.end<a.end;
	  }
	}
      }
    }
  }
}

void printvec(const vector<Record> &records, vector<Record> &subrecords, map<string,vector<int> > &children)
{
  sort(subrecords.begin(),subrecords.end(),cmp);
  for(int i=0; i<subrecords.size(); i++){
    cout << subrecords[i].str << "\n";
    vector<int> child=children[subrecords[i].id];
    vector<Record> v;
    for(int j=0; j<child.size(); j++){
      v.push_back(records[child[j]]);
    }
    printvec(records,v,children);
  }
}

int main(int argc, char *argv[])
{
  vector<Record> records;
  string str;
  while(getline(cin,str)){
    Record r(str);
    records.push_back(r);
  }


  map<string,vector<int> > children;
  vector<Record> gene;
  for(int r=0; r<records.size(); r++){
    if(records[r].parent==""){
      gene.push_back(records[r]);
    }else{
      children[records[r].parent].push_back(r);
    }
  }

  printvec(records,gene,children);

  return 0;
}

