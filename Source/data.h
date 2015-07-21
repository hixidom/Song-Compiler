#ifndef _GLIBCXX_FSTREAM
#include<fstream>
#endif

#ifndef _GLIBCXX_VECTOR
#include<vector>
#endif

#ifndef _GLIBCXX_CSTRING
#include<cstring>
#endif

#include<iostream>

#ifndef DATA_H
#define DATA_H

#define SEC_NAME_SIZE 5 //4 chars + null termination
using namespace std;

class datasectiontype{
    public:
    char name[SEC_NAME_SIZE];
    vector<vector<int> > group;
    
    void load(ifstream *ifile){
        vector<int> temp;
        
        int i;
        for(i=0;i<4;i++){
            if(ifile->peek()==222)break;
            else name[i]=ifile->get();
        }
        name[4]='\0';
        ifile->seekg(1,ifile->cur);
        while(!ifile->eof()){
            while(ifile->peek()!=230 && !ifile->eof()){
                temp.push_back(ifile->get()-100);
            }
            group.push_back(temp);
            temp.clear();
            ifile->seekg(1,ifile->cur);
            if(ifile->peek()==220){
                ifile->seekg(1,ifile->cur);
                break;
            }
        }
    }
    datasectiontype(){}
    datasectiontype(ifstream *ifile){
        load(ifile);
    }
    datasectiontype(const char *newname){
        int i;
        for(i=0;i<SEC_NAME_SIZE;i++)name[i]=newname[i];
        name[SEC_NAME_SIZE-1]='\0';
    }
};
class datatype{//data is organized in a main dataset composed of sections which are composed of groups which each contain [datarank] bytes.
    char* datafilename;
    
    public:
    vector<datasectiontype> section;
    
    void load(char *filename){
        cout<<"loading database...";
        datafilename=filename;
        ifstream ifile(filename,ifstream::binary);
        ifile.seekg(0,ifile.beg);
        if(ifile.peek()==-1){
            cout<<"empty\n";
            return;
        }
        
        ifile.ignore(100000,220);
        while(!ifile.eof()){
            datasectiontype s(&ifile);
            section.push_back(s);
            cout<<"\nsection: "<<s.name<<"\tgroups: "<<s.group.size();
        }
        
        ifile.close();
    }
    
    void save(){
        ofstream ofile(datafilename,ofstream::binary);
        int s,g,i;
        for(s=0;s<section.size();s++){
            ofile.put(220);
            ofile.write(section[s].name,SEC_NAME_SIZE-1);
            ofile.put(222);
            for(g=0;g<section[s].group.size();g++){
                for(i=0;i<section[s].group[g].size();i++){
                    ofile.put(section[s].group[g][i]+100);
                }
                ofile.put(230);
            }
        }
        
        ofile.close();
    }
        
    datatype(char *filename){
        load(filename);
    }
    
    int getsection(const char *sectionname){
        if(section.size()==0)return -1;
        int i;
        char sname[5];
        for(i=0;i<4;i++){
            if(i>=strlen(sectionname))sname[i]=' ';
            else sname[i]=sectionname[i];
        }
        sname[4]='\0';
        
        for(i=0;i<section.size();i++)
            if(!strcmp(section[i].name,sectionname))return i;
        return -1;
    }
    
    int setsection(const char *sectionname){
        int i=getsection(sectionname);
        if(i>-1)return i;
        else{
            datasectiontype s(sectionname);
            section.push_back(s);
        }
        return section.size()-1;
    }
    
    int find(const char *sectionname,vector<int> v){
        if(section.size()==0)return -1;
        int i=getsection(sectionname);
        if(i<0)return -1; //section not found
        else{
            int j;
            for(j=0;j<section[i].group.size();j++){
                if(v==section[i].group[j])return j;
            }
        }
        return -2; //group not found within specified section
    }
    
    int get(const char *sectionname,int g,int index){//returns (1000+n) if nth arg is invalid
        int i=getsection(sectionname);
        
        if(i>-1){
            if(g>=section[i].group.size()||g<0)return 1002;
            else if(index>=section[i].group[g].size()||index<0)return 1003;
            else return section[i].group[g][index];
        }
        else return 1001;
    }
    vector<float> get(const char *sectionname,int g){//returns empty vector upon error
        vector<float> empty;
        int i=getsection(sectionname);
        
        if(i>-1){
            if(g>=section[i].group.size()||g<0)return empty;
            else{
                vector<float> groupf(section[i].group[g].begin(),section[i].group[g].end());
                return groupf;
            }
        }
        else return empty;
    }
    int groupsize(const char *sectionname,int g){//returns empty vector upon error
        int i=getsection(sectionname);
        
        if(i>-1){
            if(g>=section[i].group.size()||g<0)return 0;
            else return section[i].group[g].size();
        }
        else return 0;
    }
    int sectionsize(const char *sectionname){//returns empty vector upon error
        int i=getsection(sectionname);
        
        if(i>-1)return section[i].group.size();
        else return 0;
    }
    
    void set(const char *sectionname,int g,int index,int value){//'value' is constrained to be between -100 and +100
        if(value>100)value=100;
        else if(value<-100)value=-100;
        
        int i=setsection(sectionname);        
        
        if(g>=section[i].group.size()||g==-1){
            vector<int> tempgroup;
            tempgroup.push_back(value);
            section[i].group.push_back(tempgroup);
        }
        else if(index>=section[i].group[g].size()||index==-1)section[i].group[g].push_back(value);
        else section[i].group[g][index]=value;
    }
    
    int set(const char *sectionname,int g,vector<int> newgroup){//'value' is constrained to be between -100 and +100
        int i;
        for(i=0;i<newgroup.size();i++){
            if(newgroup[i]>100)newgroup[i]=100;
            else if(newgroup[i]<-100)newgroup[i]=-100;
        }
        
        i=setsection(sectionname);
        
        if(g>=section[i].group.size()||g==-1){
            section[i].group.push_back(newgroup);
            return section[i].group.size()-1;
        }
        else if(g<-1)return 1002;
    }
    
    int set(const char *sectionname,int g,vector<float> newgroup){//'value' is constrained to be between -100 and +100
        int i;
        vector<int> converted(newgroup.begin(),newgroup.end());
        for(i=0;i<converted.size();i++){
            if(converted[i]>100)converted[i]=100;
            else if(newgroup[i]<-100)newgroup[i]=-100;
        }
        
        i=setsection(sectionname);
        
        if(g>=section[i].group.size()||g==-1){
            section[i].group.push_back(converted);
            return section[i].group.size()-1;
        }
        else if(g<-1)return 1002;
    }
    
    ~datatype(){
        save();
    }          
};
#endif
