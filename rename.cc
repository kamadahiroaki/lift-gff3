#include <cstdio>
#include <vector>
#include <string>
#include <set>
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

class Record{
public:
  string str;
  string seqid;
  int start;
  int end;
  string name;
  string score;
  string strand;
  string source;
  string type;
  string phase;
  string attributes;
  vector<string> attr;
  string id;
  string parent;

  string transform;
  string tname;
  int tstart;
  int tend;

  string seqid2;
  int start2;
  int end2;
  //  string name2;
  //  string score2;
  string strand2;
  // string source2;
  // string type2;
  // string phase2;
  // string attributes2;
  // vector<string> attr2;
  // string id2;
  // string parent2;

  Record(){}
  Record(string ss){
    str=ss;
    vector<string> s=split(ss,'\t');
    if(s.size()!=13){
      cerr << "format error: " << ss << endl;
      return;
    }
    seqid=s[0];
    start=atoi(s[1].c_str());
    end=atoi(s[2].c_str());
    name=s[3];
    score=s[4];
    strand=s[5];
    vector<string> n=split(name,':');
    source=n[0];
    type=n[1];
    phase=n[2];
    attributes=id=parent="";
    if(n.size()==4){
      attributes=n[3];
      attr=split(attributes,';');
      for(int i=0; i<attr.size(); i++){
	if(attr[i].size()>3 && attr[i].substr(0,3)=="ID="){
	  id=attr[i].substr(3);
	}
	if(attr[i].size()>7 && attr[i].substr(0,7)=="Parent="){
	  parent=attr[i].substr(7);
	}
      }
    }

    transform=s[6];
    tname="";
    if(transform!="->"){
      vector<string> t=split(transform,':');
      tname=t[0].substr(1);
      tstart=atoi(t[2].c_str());
      tend=atoi(t[3].c_str());
    }


    seqid2=s[7];
    start2=atoi(s[8].c_str());
    end2=atoi(s[9].c_str());
    //    name2=s[10];
    //    score2=s[11];
    strand2=s[12];
    // n=split(name2,':');
    // source2=n[0];
    // type2=n[1];
    // phase2=n[2];
    // attributes2=id2=parent2="";
    // if(n.size()==4){
    // attributes2=n[3];
    // attr2=split(attributes2,';');
    // for(int i=0; i<attr2.size(); i++){
    //   if(attr2[i].size()>3 && attr2[i].substr(0,3)=="ID="){
    // 	id2=attr2[i].substr(3);
    //   }
    //   if(attr2[i].size()>7 && attr2[i].substr(0,7)=="Parent="){
    // 	parent2=attr2[i].substr(7);
    //   }
    // }
    // }
  }
};


vector<Record> checkRecordSize(const vector<Record> &records)
{
  vector<Record> r;
  for(int i=0; i<records.size(); i++){
    int before=records[i].end-records[i].start;
    int after=records[i].end2-records[i].start2;
    if(records[i].transform!="->"){
      before=records[i].tend-records[i].tstart;
    }
    if(before!=after){
      cerr << "truncated region:\t" << records[i].str << "\n";
    }else{
      r.push_back(records[i]);
    }
  }
  return r;
}

void reviseIDParent(vector<Record> &records)
{
  map<string,vector<int> > id2children;
  for(int i=0; i<records.size(); i++){
    if(records[i].parent!=""){
      id2children[records[i].parent].push_back(i);
    }
  }
  
  vector<int> tempvector;
  vector<vector<int> > parent2children(records.size(),tempvector);
  for(int i=0; i<records.size(); i++){
    vector<int> v=id2children[records[i].id];
    int start=records[i].start;
    int end=records[i].end;
    if(records[i].transform!="->"){
      start=records[i].tstart;
      end=records[i].tend;
    }
    for(int j=0; j<v.size(); j++){
      int cstart=records[v[j]].start;
      int cend=records[v[j]].end;
      if(records[v[j]].transform!="->"){
	cstart=records[v[j]].tstart;
	cend=records[v[j]].tend;
      }
      if(records[i].seqid==records[v[j]].seqid && start<=cstart && cend<=end){
	parent2children[i].push_back(v[j]);
      }
    }
  }
  vector<int> revised;
  for(int i=0; i<records.size(); i++){
    if(records[i].transform!="->" && records[i].id!=""){
      string splitnum="_"+records[i].tname;
      records[i].id+=splitnum;
      for(int j=0; j<records[i].attr.size(); j++){
	if(records[i].attr[j].substr(0,3)=="ID="){
	  records[i].attr[j]="ID="+records[i].id;
	  revised.push_back(i);
	}
      }
      records[i].attributes=records[i].attr[0];
      for(int j=1; j<records[i].attr.size(); j++){
	records[i].attributes+=";"+records[i].attr[j];
      }
      records[i].name=records[i].source+":"+records[i].type+":"+records[i].phase+":"+records[i].attributes;
    }
  }
  while(1){
    set<int> childset;
    for(int i=0; i<revised.size(); i++){
      vector<int> children=parent2children[revised[i]];
      for(int j=0; j<children.size(); j++){
	records[children[j]].parent=records[revised[i]].id;
	records[children[j]].id+="_("+records[revised[i]].id+")";
	for(int k=0; k<records[children[j]].attr.size(); k++){
	  if(records[children[j]].attr[k].substr(0,7)=="Parent="){
	    records[children[j]].attr[k]="Parent="+records[children[j]].parent;
	  }
	}
	for(int k=0; k<records[children[j]].attr.size(); k++){
	  if(records[children[j]].attr[k].substr(0,3)=="ID="){
	    records[children[j]].attr[k]="ID="+records[children[j]].id;
	  }
	}
	records[children[j]].attributes=records[children[j]].attr[0];
	for(int k=1; k<records[children[j]].attr.size(); k++){
	  records[children[j]].attributes+=";"+records[children[j]].attr[k];
	}
	records[children[j]].name=records[children[j]].source+":"+records[children[j]].type+":"+records[children[j]].phase+":"+records[children[j]].attributes;
	childset.insert(children[j]);
      }
    }
    vector<int> v;
    for(auto iter=childset.begin(); iter!=childset.end(); iter++){
      v.push_back(*iter);
    }
    if(v.size()==0) break;
    revised=v;
  }

}

void revisePhase(vector<Record> &records)
{
  for(int i=0; i<records.size(); i++){
    if(records[i].phase=="0" || records[i].phase=="1" || records[i].phase=="2"){
      int p=atoi(records[i].phase.c_str());
      int p2=p;
      if(records[i].transform!="->"){
	int cs=records[i].start;
	int ce=records[i].end;
	int splitcs=records[i].tstart;
	int splitce=records[i].tend;
	int df;
	if(records[i].strand=="+"){
	  df=splitcs-cs;
	}else{
	  df=ce-splitce;
	}
	if(df%3==1){
	  p2=(p+2)%3;
	}
	if(df%3==2){
	  p2=(p+1)%3;
	}
      }
      records[i].phase=to_string(p2);

      records[i].name=records[i].source+":"+records[i].type+":"+records[i].phase+":"+records[i].attributes;
    }
  }
}

vector<Record> discardShortCds(const vector<Record> &records, const int threshold)
{
  vector<bool> valid(records.size(),true);
  map<string,vector<int> > cds;
  for(int i=0; i<records.size(); i++){
    if(records[i].type=="CDS" || records[i].type=="cds"){
      cds[records[i].parent].push_back(i);
    }
  }
  for(auto iter=cds.begin(); iter!=cds.end(); iter++){
    vector<int> v=iter->second;
    int total=0;
    for(int i=0; i<v.size(); i++){
      total+=records[v[i]].end2-records[v[i]].start2;
    }
    if(total<threshold){
      for(int i=0; i<v.size(); i++){
	valid[v[i]]=false;
	cerr << "Short CDS:\t" << records[v[i]].str << endl;
      }
    }
  }
  vector<Record> r;
  for(int i=0; i<records.size(); i++){
    if(valid[i]){
      r.push_back(records[i]);
    }
  }
  return r;
}

vector<Record> discardChildlessGene(const vector<Record> &records)
{
  vector<bool> valid(records.size(),false);
  map<string,string> parent;
  set<string> parenthood;
  for(int i=0; i<records.size(); i++){
    if(records[i].id!="" && records[i].parent!=""){
      parent[records[i].id]=records[i].parent;
    }
  }
  for(int i=0; i<records.size(); i++){
    if(records[i].type=="CDS" || records[i].type=="cds"){
      auto iter=parent.find(records[i].id);
      while(iter!=parent.end()){
	parenthood.insert(iter->second);
	iter=parent.find(iter->second);
      }
    }
  }
  for(int i=0; i<records.size(); i++){
    if(parenthood.find(records[i].id)!=parenthood.end()){
      valid[i]=true;
    }
    auto iter=parent.find(records[i].id);
    while(iter!=parent.end()){
      if(parenthood.find(iter->second)!=parenthood.end()){
	valid[i]=true;
	break;
      }
      iter=parent.find(iter->second);
    }
  }
  vector<Record> r;
  for(int i=0; i<records.size(); i++){
    if(parenthood.empty() || valid[i]){
      r.push_back(records[i]);
    }else{
      cerr << "No CDS:\t" << records[i].str << endl;
    }
  }
  return r;
}

void printBed(const vector<Record> &records)
{
  for(int i=0; i<records.size(); i++){
    Record r=records[i];
    cout << r.seqid2 << "\t" << r.start2 << "\t" << r.end2 << "\t" << r.name << "\t" << r.score << "\t" << r.strand2 << "\n";
  }
}

int main(int argc, char *argv[])
{
  vector<Record> records;
  string str;

  while(getline(cin,str)){
    if(str.size()==0 || str[0]=='#') continue;
    Record r(str);
    records.push_back(r);
  }

  records=checkRecordSize(records);

  reviseIDParent(records);

  revisePhase(records);

  records=discardShortCds(records,6);

  records=discardChildlessGene(records);

  printBed(records);

  return 0;
 
  return 0;
}

