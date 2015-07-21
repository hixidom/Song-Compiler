#include "matops.h"

#ifndef OAL_TOOLS_H
#include "OALtools.h"
#endif

#ifndef EFFECTS_H
#define EFFECTS_H

using namespace std;

class audiotype{
    public:
    vector<float> data;//audio data
    float spb;//Samples per beat. To be used for time conversions
    
    audiotype(int size,float val){
        data.resize(size,val);
    }
    audiotype(){}
    
    void add(float v,long index){
        if(data.size()<index+1)data.resize(index+1,0);//extend data such that data.size()==v.size()
        data[index]+=v;//add data and v elementwise
    }
    void add(vector<float> v){
        if(data.size()<v.size())data.resize(v.size(),0);//extend data such that data.size()==v.size()
        long i;
        for(i=0;i<v.size();i++)data[i]+=v[i];//add data and v elementwise
    }
    void add(vector<float> v,long start){
        if(data.size()<start+v.size())data.resize(start+v.size(),0);//extend data such that data.size()==start+v.size()
        long i;
        for(i=0;i<v.size();i++)data[i+start]+=v[i];//add data and v elementwise starting at start
    }
    void add(audiotype v){
        if(data.size()<v.size())data.resize(v.size(),0);//extend data such that data.size()==v.size()
        long i;
        for(i=0;i<v.size();i++)data[i]+=v.data[i];//add data and v elementwise
    }
    void add(audiotype v,long start){
        if(data.size()<start+v.size())data.resize(start+v.size(),0);//extend data such that data.size()==start+v.size()
        long i;
        for(i=0;i<v.size();i++)data[i+start]+=v.data[i];//add data and v elementwise starting at start
    }
    
    void append(vector<float> v){
        long i;
        for(i=0;i<v.size();i++)data.push_back(v[i]);//append v to data
    }
    int size(){
        return data.size();
    }
    void clear(){
        data.clear();
    }
    void scale(float factor){
        if(factor==1)return;//do nothing
        long i;
        for(i=0;i<data.size();i++)data[i]*=factor;
    }
    vector<float> scaled(float factor){
        if(factor==1)return data;//do nothing
        vector<float> temp=data;
        long i;
        for(i=0;i<temp.size();i++)temp[i]*=factor;
        return temp;
    }
    void offset(float amount){
        if(amount==0)return;//do nothing
        long i;
        for(i=0;i<data.size();i++)data[i]+=amount;
    }
    
    void play(){
        int i;
        //dispfv(data,100);
        playbufferv(data);
    }
};

vector<float> anticlick(vector<float> audio){//remove *click* from audio by tapering first 100 samples and last 100 samples
    float width=100;
    if(audio.size()>width){
        long s;
        for(s=0;s<width;s++){
            audio[s]*=s/width;
            audio[s+audio.size()-width]*=(width-1-s)/width;
        }
    }
    return audio;
}

audiotype echo(audiotype audio,vector<float> params){//parameters are (1)delay(beats),(2)decay,(3)#limit or thresh
    params.resize(3,.5);//ensure that params has correct number of elements
    float num=params[2];//num is number of instances of audio: num = 1 + number of echoes
    if(num<1)num=ceil(log(params[2])/log(params[1]));//if params[2] represents amp thresh., use decay factor to calculate value of num
    audiotype newaudio=audio;
       
    int i;
    for(i=1;i<num;i++)newaudio.add(audio.scaled(pow(params[1],i)),audio.spb*params[0]*i);
    return newaudio;
}
audiotype repeat(audiotype audio,vector<float> params){//parameters are (1)delay(beats),(2)# of repeats
    params.resize(2,1);//ensure that params has correct number of elements
    audiotype newaudio=audio;
    int i;
    for(i=1;i<params[1];i++){
        newaudio.add(audio,audio.spb*params[0]*i);
    }
    return newaudio;
}
audiotype fadein(audiotype audio,vector<float> params){//parameters are (1)length(beats)
    params.resize(1,0);//ensure that params has correct number of elements
    long i,maxi=audio.spb*params[0];
    for(i=0;i<maxi && i<=audio.size();i++)audio.data[i]*=pow((float)i/maxi,2);
    return audio;
}
audiotype fadeout(audiotype audio,vector<float> params){//parameters are (1)length(beats)
    params.resize(1,0);//ensure that params has correct number of elements
    long i,maxi=audio.spb*params[0];
    for(i=1;i<maxi && i<=audio.size();i++)audio.data[audio.size()-i]*=pow((float)i/maxi,2);
    return audio;
}
#endif
