#ifndef _GLIBCXX_FSTREAM
#include<fstream>
#endif

#ifndef _GLIBCXX_SSTREAM
#include<sstream>
#endif

#ifndef CSTDIO_H
#define CSTDIO_H
#include <cstdio>
#endif

#ifndef CMATH_H
#define CMATH_H
#include<cmath>
#define TWOPI 6.28318530718
#endif

#ifndef EFFECTS_H
#include "effects.h"
#endif
using namespace std;

#ifndef COMPILER_H
#define COMPILER_H

class effecttype{
    public:
    string spec;//effect specifier (2 letters)
    vector<float> params,polyvars1,polyvars2;//effect parameters; polyvars1==h, polyvars2==w
    bool polyfitexists;//represents whether or not params contains polynomial coefficients
    
    effecttype(string s,vector<float> p){
        spec=s;
        params=p;
    }
    effecttype(string strline){
        stringstream line(strline);//convert strline to stringstream object
        char tempspec[3];
        string tempstr;
        line.get(tempspec,2);
        spec.assign(tempspec,2);
        if(getline(line,tempstr))params=stov(tempstr);//convert next group of characters into a vector of params
    }
    effecttype(const char *sp){
        spec.push_back(sp[0]);
        spec.push_back(sp[1]);
    }
    effecttype(const char *sp,vector<float> p){
        spec.push_back(sp[0]);
        spec.push_back(sp[1]);
        params=p;
    }
    effecttype(){}
    
    short set(string strline){
        stringstream line(strline);//convert strline to stringstream object
        char tempspec[3];
        string tempstr;
        line.get(tempspec,3);
        spec.assign(tempspec,2);
        if(getline(line,tempstr))params=stov(tempstr);//convert next group of characters into a vector of params
        
        if(spec=="ns")return 1;//return 1 if effect is a noteslide effect
        else if(spec=="lp"||spec=="hp"||spec=="bp"||spec=="bs")return 2;//return 2 if effect is a filter
        else return 0;
    }
    
    audiotype apply(audiotype audio){//returns pointer to modified audio data
        if(spec=="ec")return echo(audio,params);
        //else if(spec=="di")return distort(audio,start,end);
        //else if(spec=="rv")return reverb(audio,start,end);
        else if(spec=="fi")return fadein(audio,params);
        else if(spec=="fo")return fadeout(audio,params);
        //else if(spec=="lf")return lfo(audio,start,end);
        else if(spec=="rp")return repeat(audio,params);
        //else if(spec=="dt")return detune(audio,start,end);
        //else if(spec=="no")return 
        else return audio;
    }
    audiotype apply(audiotype audio,int start,int end){//returns pointer to modified audio data
        if(start>0)audio.data.erase(audio.data.begin(),audio.data.begin()+start);//erase audio data before start
        if(end<audio.size()-1)audio.data.erase(audio.data.begin()+end,audio.data.end());//erase audio data after end
        
        if(spec=="ec")return echo(audio,params);
        //else if(spec=="di")return distort(audio,start,end);
        //else if(spec=="rv")return reverb(audio,start,end);
        //else if(spec=="fi")return fadein(audio,start,end);
        //else if(spec=="fo")return fadeout(audio,start,end);
        //else if(spec=="lf")return lfo(audio,start,end);
        else if(spec=="rp")return repeat(audio,params);
        //else if(spec=="dt")return detune(audio,start,end);
        //else if(spec=="no")return 
        else return audio;
    }
    
    void filttopoly(float tstart,float tend){//convert vector of h and w values to vector polynomial coefficients for h and w
        short i;
        for(i=0;i<params.size();i++){
            if(i%2==0)polyvars1.push_back(params[i]);
            else polyvars2.push_back(params[i]);
        }
        vector<float> t=linspace(tstart,tend,polyvars1.size());//space h.size() time points from tstart to tend
        polyvars1=polyfit(t,polyvars1);//replace h values with polynomial coefficients
        polyvars2=polyfit(t,polyvars2);//replace w values with polynomial coefficients
        
        polyfitexists++;//set polyfitexists flag to indicate that params contains polyfit coefficients
    }
};

class effectlisttype{
    public:
    vector<effecttype> note,seq,slide,notefilt,seqfilt;//lists of note effects and sequence effects
        
    bool add(string strline){
        stringstream line(strline);//convert strline to stringstream object
        string subline;
        bool effectfound=false;
        effecttype tempeffect;
        
        while(getline(line,subline,' ')){//parse effect line by spaces
            if(subline=="seq")break;//if "seq" is encountered, save upcoming effects to seqeffects rather than noteeffects
            switch(tempeffect.set(subline)){
                case 0: note.push_back(tempeffect);break;//add tempeffect to effect vector
                case 1: slide.push_back(tempeffect);break;//add tempeffect to noteslide vector
                case 2: notefilt.push_back(tempeffect);break;//add tempeffect to note effect vector
            }
            effectfound=true;
            subline.clear();
        }
        while(getline(line,subline,' ')){//parse effect line by spaces
            switch(tempeffect.set(subline)){
                case 0: seq.push_back(tempeffect);break;//add tempeffect to effect vector
                case 1: slide.push_back(tempeffect);break;//add tempeffect to noteslide vector
                case 2: seqfilt.push_back(tempeffect);break;//add tempeffect to note effect vector
            }
            effectfound++;
            subline.clear();
        }
        return effectfound;
    }
    void addnote(const char *spec,vector<float> params){
        effecttype tempnote(spec,params);
        note.push_back(tempnote);
    }
    void addseq(const char *spec,vector<float> params){
        effecttype tempseq(spec,params);
        seq.push_back(tempseq);
    }
};

class insttype{
    public:
    short index,subinst;
    char type;
    vector<float> A;
    effectlisttype effects;
    bool savedata;
    
    void loadinst(string instinfo){
        int i;
        int subinst=0,commapos=instinfo.find(',');
        string Astr;
        
        if(type=='i'){
            string fname=instinfo,subinststr=instinfo;
            
            if(commapos!=-1){
                fname.erase(fname.begin()+commapos,fname.end());
                subinststr.erase(subinststr.begin(),subinststr.begin()+commapos);
                subinst=stof(subinststr);
            }
            fname.insert(0,"inst/");
            fname.insert(fname.size(),".txt");
            
            ifstream file;
            file.open(fname.c_str());
            if(!file.is_open()){
                cout<<fname<<"\" failed to open!\n";
                return;
            }
            for(i=0;i<subinst;i++)file.ignore(100000,'\n'); //skip to the specified sub-instrument
            getline(file,Astr,'\n');
            file.close();
            A=stov(Astr);
        }
        else if(type=='f'){}
    }
    insttype(){}
    insttype(string strline){
        bool inlineinst=true;
        type=strline[0];
        stringstream line(strline);//convert strline to stringstream object
        string tempstr;
        line.ignore(256,' ');
        if(getline(line,tempstr,' '))index=stof(tempstr);//append index to end of index vector
        tempstr.clear();
        
        getline(line,tempstr,' ');
        int i;
        for(i=0;i<tempstr.size();i++)if(isalpha(tempstr[i])){inlineinst=false; break;} //discriminate between inst data and inst file name
        if(inlineinst)A=stov(tempstr);//convert next group of characters into a vector of inst params
        else loadinst(tempstr);
        tempstr.clear();
        
        if(getline(line,tempstr))effects.add(tempstr);//convert next group of characters into an effects list
    }
    bool set(string strline){
        type=strline[0];
        stringstream line(strline);//convert strline to stringstream object
        string tempstr;
        line.ignore(256,' ');
        if(getline(line,tempstr,' '))index=stof(tempstr);//append index to end of index vector
        else return false;
        tempstr.clear();
        if(getline(line,tempstr,' '))A=stov(tempstr);//convert next group of characters into a vector of inst params
        else return false;
        if(getline(line,tempstr))effects.add(tempstr);//convert next group of characters into an effects list
        return true;
    }
};

class instlisttype{
    public:
    vector<insttype> inst;
    
    void add(string strline){
        insttype tempinst(strline);
        inst.push_back(tempinst);
    }
    void add(insttype tempinst){
        inst.push_back(tempinst);
    }
    insttype findinst(int index){
        int i;
        for(i=0;i<inst.size();i++)if(index==inst[i].index)return inst[i];
        return inst[0];
    }
    int findindex(int index){
        int i;
        for(i=0;i<inst.size();i++)if(index==inst[i].index)return i;
        return -1; //if no index is found, return -1
    }
    int freeindex(){
        int index=1;
        bool isfree;
        do{
            isfree=true;
            int i;
            for(i=0;i<inst.size();i++)
                if(index==inst[i].index){
                    isfree=false;
                    index++;
                    break;
                }
        }while(!isfree);
        return index;
    }
};

struct paramtype{
    float bpm,basef;
    int maxinstsize;//maximum number of elements of an instrument that will be played
};

float filtfunc(vector<effecttype> filt,float t,float h){
    float Afactor=1;
    int f;
    for(f=0;f<filt.size();f++){
        if(filt[f].spec=="hp")Afactor=Afactor*max(min((1+h-polyeval(filt[f].polyvars1,t))/polyeval(filt[f].polyvars2,t),1),0);
        else if(filt[f].spec=="lp")Afactor=Afactor*max(min((1-(h-polyeval(filt[f].polyvars1,t)))/polyeval(filt[f].polyvars2,t),1),0);
        else if(filt[f].spec=="bp")Afactor=Afactor*exp(-2.77*pow(h-polyeval(filt[f].polyvars1,t),2)/pow(polyeval(filt[f].polyvars2,t),2));
        else if(filt[f].spec=="bs")Afactor=Afactor*(1-exp(-2.77*pow(h-polyeval(filt[f].polyvars1,t),2)/pow(polyeval(filt[f].polyvars2,t),2)));
    }
    //if(h==10 && (int)t%200==0)cout<<Afactor<<" ";
    return Afactor;
}

vector<float> fourierseries(vector<float> farray,int startsample,int samplelength,float starttime,vector<float> A,vector<effecttype> allfl){
    audiotype audio(startsample+samplelength,0);
    int i;
    long s;
    if(A.size()%2==1)A.push_back(0); //ensure that both sin and cos coefficients are included for all frequencies
    
    for(i=0;i<A.size();i+=2){
        for(s=startsample;s<startsample+samplelength;s++){
            audio.add(filtfunc(allfl,s,i)*(A[i]*sin(((i+1)*farray[s]*TWOPI*s/44100.0)+starttime)+A[i+1]*cos(((i+1)*farray[s]*TWOPI*s/44100.0)+starttime)),s);
        }
    }
    return audio.data;    
}

class comtype{
    public:
    short insti;
    float volume;
    vector<float> bstart,blength;
    vector<vector<float> > note;
    effectlisttype effects;
    
    bool mute,solo;
    
    comtype(){}
    comtype(string str){
        mute=(str[0]=='m');
        solo=(str[0]=='s');
        if(mute||solo)str=str.substr(1);//crop out first element of str
        
        stringstream line(str);//convert strline to stringstream object
        string tempstr;
        if(getline(line,tempstr,' '))insti=stof(tempstr);
        tempstr.clear();
        if(getline(line,tempstr,' '))volume=stof(tempstr);
        tempstr.clear();
        if(getline(line,tempstr,' '))bstart=stov(tempstr);
        tempstr.clear();
        if(getline(line,tempstr,' '))blength=stov(tempstr);
        tempstr.clear();
        if(getline(line,tempstr,' '))note=stovv(tempstr);
        tempstr.clear();
        if(getline(line,tempstr)){
            effects.add(tempstr);//convert next group of characters into a noteeffects element
            tempstr.clear();
        }
        /*//command parameters
        cout<<"\tinst index="<<inst;
        cout<<"\tvolume="<<volume;
        cout<<"\nbstart=";
        int i;
        for(i=0;i<bstart.size();i++){
            cout<<bstart[i]<<",";
        }
        cout<<"\nblength=";
        for(i=0;i<blength.size();i++){
            cout<<blength[i]<<",";
        }
        int j;
        for(i=0;i<note.size();i++){
            for(j=0;j<note[i].size();j++)
                cout<<note[i][j]<<",";
            cout<<";";}*/
    }
    
    bool set(string str){
        mute=(str[0]=='m');
        solo=(str[0]=='s');
        if(mute||solo)str=str.substr(1);//crop out first element of str
        
        stringstream line(str);//convert strline to stringstream object
        string tempstr;
        if(getline(line,tempstr,' '))insti=stof(tempstr);
        else return false;
        tempstr.clear();
        if(getline(line,tempstr,' '))volume=stof(tempstr);
        else return false;
        tempstr.clear();
        if(getline(line,tempstr,' '))bstart=stov(tempstr);
        else return false;
        tempstr.clear();
        if(getline(line,tempstr,' '))blength=stov(tempstr);
        else return false;
        tempstr.clear();
        if(getline(line,tempstr,' '))note=stovv(tempstr);
        else return false;
        tempstr.clear();
        effectlisttype noteeffects,seqeffects;
        while(getline(line,tempstr,' ')){
            if(tempstr=="seq")break;//if "seq" is encountered, save upcoming effects to seqeffects rather than noteeffects
            if(!noteeffects.add(tempstr))return 0;//convert next group of characters into a noteeffects element
            tempstr.clear();
        }
        while(getline(line,tempstr,' ')){
            if(!seqeffects.add(tempstr))return 0;//convert next group of characters into a seqeffects element
            tempstr.clear();
        }
        return true;
    }
    
    string tostr(){ //return string representing sound command
        string comstr;
        stringstream s;
        if(bstart.empty())return comstr;
        
        int i,j;
        comstr+=ftoa(insti);
        comstr+=' ';
        comstr+=ftoa(volume);
        comstr+=" [";
        comstr+=ftoa(bstart[0]);
        for(i=1;i<bstart.size();i++){
            comstr+=',';
            comstr+=ftoa(bstart[i]);
        }
        comstr+="] [";
        comstr+=ftoa(blength[0]);
        for(i=1;i<blength.size();i++){
            comstr+=',';
            comstr+=ftoa(blength[i]);
        }
        comstr+="] [";
        for(i=0;i<note.size();i++){
            comstr+=ftoa(note[i][0]);
            for(j=1;j<note[i].size();j++){
                comstr+=',';
                comstr+=ftoa(note[i][j]);
            }
            if(i+1<note.size())comstr+=';';
        }
        comstr+="] ";
        for(i=0;i<effects.note.size();i++){
            comstr+=effects.note[i].spec;
            comstr+=ftoa(effects.note[i].params[0]);
            for(j=1;j<effects.note[i].params.size();j++){
                comstr+=',';
                comstr+=ftoa(effects.note[i].params[j]);
            }
            comstr+=' ';
        }
        for(i=0;i<effects.notefilt.size();i++){
            comstr+=effects.notefilt[i].spec;
            comstr+=ftoa(effects.notefilt[i].params[0]);
            for(j=1;j<effects.notefilt[i].params.size();j++){
                comstr+=',';
                comstr+=ftoa(effects.notefilt[i].params[j]);
            }
            comstr+=' ';
        }
        for(i=0;i<effects.seq.size();i++){
            comstr+=effects.seq[i].spec;
            comstr+=ftoa(effects.seq[i].params[0]);
            for(j=1;j<effects.seq[i].params.size();j++){
                comstr+=',';
                comstr+=ftoa(effects.seq[i].params[j]);
            }
            comstr+=' ';
        }
        for(i=0;i<effects.seqfilt.size();i++){
            comstr+=effects.seqfilt[i].spec;
            comstr+=ftoa(effects.seqfilt[i].params[0]);
            for(j=1;j<effects.seqfilt[i].params.size();j++){
                comstr+=',';
                comstr+=ftoa(effects.seqfilt[i].params[j]);
            }
            comstr+=' ';
        }
        for(i=0;i<effects.slide.size();i++){
            comstr+=effects.slide[i].spec;
            comstr+=ftoa(effects.slide[i].params[0]);
            for(j=1;j<effects.slide[i].params.size();j++){
                comstr+=',';
                comstr+=ftoa(effects.slide[i].params[j]);
            }
            comstr+=' ';
        }
        return comstr;
    }
    
    float bsize(){
        float bsize=0;
        short i;
        for(i=0;i<bstart.size();i++){
            if(bstart[i]+blength[i]-bstart[0]>bsize)bsize=bstart[i]+blength[i]-bstart[0];
        }
        return bsize;
    }
    
    void transpose(float amount){
        int i,j;
        for(i=0;i<note.size();i++)
            for(j=0;j<note[i].size();j++)
                note[i][j]+=amount;
    }
    
    void genaudio(audiotype *audio,insttype inst,paramtype params){
        //Make sure bstart, blength, and note are all the same length (in time)
        if(blength.size()<bstart.size()){
            short offset=blength.size();
            short i;
            for(i=offset;i<bstart.size();i++)blength.push_back(blength[i-offset]);//repeat blength until it is as long as bstart
        }
        short n;
        if(note.size()<bstart.size()){
            short offset=note.size();
            short i;
            for(i=offset;i<bstart.size();i++)note.push_back(note[i-offset]);//repeat blength until it is as long as bstart
        }
        for(n=0;n<blength.size()-1;n++)
            if(blength[n]==0)blength[n]=bstart[n+1]-bstart[n];//if blength(i)==0, set blength(i) such that note i is played until next note is reached
        if(blength.back()==0)blength.back()=pow(2,ceil(log(sum(blength,1,blength.size()-1)))/log(2))-sum(blength,1,blength.size()-1);//blength(end) is set such that the length of the sequence is the lowest possible power of 2. (this is a very ambiguous problem.  Perhaps the last note should not even be played.)
        int i;
        audiotype seqaudio,noteaudio;
        for(i=0;i<bstart.size();i++){
            if(!inst.savedata || !noteaudio.size()){
                //Define Ns: length of ith note in samples
                long Ns,Ps;
                Ns=floor(audio->spb*blength[i]);//length of note in samples
                if(i<bstart.size()-1)Ps=floor(audio->spb*(bstart[i+1]-bstart[i]));//number of samples between current note and next note
                else Ps=Ns;//for last note in sequence, number of samples between current note and next note is assumbed to be Ns
                
                //Collect all note filter effects in a single variable
                short n;
                vector<effecttype> allfl=effects.notefilt;//all filters compiled into a vector
                for(n=0;n<effects.seqfilt.size();n++){
                    allfl.push_back(effects.seqfilt[n]);
                    allfl.back().filttopoly(bstart[0],bstart.back());//convert filter params to polynomial coefficients for specified params
                    allfl.back().params.clear();
                    allfl.back().params.push_back(polyeval(allfl.back().polyvars1,bstart[i]));
                    allfl.back().params.push_back(polyeval(allfl.back().polyvars2,bstart[i]));
                    allfl.back().params.push_back(polyeval(allfl.back().polyvars1,(bstart[i]+bstart[i+1])/2));
                    allfl.back().params.push_back(polyeval(allfl.back().polyvars2,(bstart[i]+bstart[i+1])/2));
                    allfl.back().params.push_back(polyeval(allfl.back().polyvars1,bstart[i+1]));
                    allfl.back().params.push_back(polyeval(allfl.back().polyvars2,bstart[i+1]));
                }
                for(n=0;n<allfl.size();n++)allfl[n].filttopoly(0,Ns);
                if(effects.slide.size()>0){
                    long flength=Ps;//sample size of f vector
                    Ns=Ps;//number of samples from bstart[i] to bstart[i+1]
                }
                else long flength=Ns;
                for(n=0;n<note[i].size();n++){
                    long s;
                    vector<float> farray;//array representing frequency value at each sample
                    if(effects.slide.size()>0){
                        farray.resize(Ns,params.basef*pow(2,note[i][n]/12.0));//initialize farray with Ps samples containing note[i][n] frequency
                        if(effects.slide.size()){
                            //apply noteslide
                        }
                    }
                    else farray.resize(Ns,params.basef*pow(2,note[i][n]/12.0));//initialize farray with Ns samples containing note[n][i] frequency
                    
                    noteaudio.data.clear();
                    noteaudio.data.assign(Ns,0);
                    noteaudio.spb=audio->spb;
                    switch(inst.type){
                        case 'i':noteaudio.data=fourierseries(farray,0,floor(noteaudio.spb*blength[i]),10,inst.A,allfl);//construct audio from fourier series
                            break;
                        case 'w':
                            int s;
                            for(s=0;s<floor(noteaudio.spb*blength[i]) && s<inst.A.size();s++)noteaudio.add(inst.A[s],s);//construct audio from normalized wave data
                            break;
                        case 'f':
                            float slope=(inst.A.size()-1)/(inst.A.back()-inst.A[0]);//factor used for conversion from frequency to harmonic
                            short f;
                            for(f=0;f<allfl.size();f++){
                                short p;
                                for(p=0;p<allfl[f].params.size();p+=2){
                                    allfl[f].params[p]=((allfl[f].params[p]-inst.A[0])*slope)+1;//convert h from frequency to harmonic
                                    allfl[f].params[p+1]=(allfl[f].params[p]*slope)+1;//convert w from frequency to harmonic
                                }
                            }
                            float randomphase;
                            float randomf;
                            for(f=0;f<inst.A.size();f++){
                                randomphase=TWOPI*rand1();
                                randomf=10*rand1();
                                for(s=0;s<Ns;s++)noteaudio.data[s]+=filtfunc(allfl,s,f)*sin((TWOPI*(inst.A[f]+randomf)*s/44100.0)+randomphase);
                            }
                            break;
                    }
                    noteaudio.data=anticlick(noteaudio.data);//apply anticlick
                    for(s=0;s<inst.effects.note.size();s++)noteaudio=inst.effects.note[s].apply(noteaudio);//apply each inst note effect to noteaudio
                    for(s=0;s<effects.note.size();s++)noteaudio=effects.note[s].apply(noteaudio);//apply each com note effect to noteaudio
                    seqaudio.add(noteaudio,floor(audio->spb*(bstart[i]-bstart[0])));//add noteaudio to seqaudio
                }
            }
            else seqaudio.add(noteaudio,floor(audio->spb*(bstart[i]-bstart[0])));//add noteaudio to seqaudio
        }
        seqaudio.spb=audio->spb;
        for(i=0;i<effects.seq.size();i++)seqaudio=effects.seq[i].apply(seqaudio);//apply each sequence effect to seqaudio
        seqaudio.data=norm(seqaudio.data);
        seqaudio.scale(volume);//scale seqaudio by volume factor
        audio->add(seqaudio,floor(audio->spb*bstart[0]));
    }
};

class comlisttype{
    public:
    vector<comtype> com;
    bool solodetected;
    
    void add(string strline){
        comtype tempcom(strline);
        com.push_back(tempcom);
        if(tempcom.solo||solodetected)solodetected=true;//set solodetected to true if solo.back is true or solodetected is already true
    }
    
    comlisttype(){
        solodetected=false;
    }
    
    float bsize(){
        float bsize=0,combsize;
        short c;
        for(c=0;c<com.size();c++){
            combsize=com[c].bsize()+com[c].bstart[0];
            if(combsize>bsize)bsize=combsize;
        }
        return bsize;
    }
    
    void clear(){
        com.clear();
        solodetected=false;
    }
};

class songtype{
    public:
    paramtype params;
    instlisttype inst;
    comlisttype com;
    audiotype audio;

    void init(const char *filename){
        ifstream songfile;
        songfile.open(filename);
        if(!songfile){cerr<<"Error opening song file!\n";}//make sure file was opened
        
        string strline;//each line will be extracted from songfile and assigned to strline
    
        bool firstline=true;
        stringstream line;
        while(getline(songfile,strline)){
            if(strline[0]==0 || strline[0]==47)continue;//blank line or comment ('/') found
            else if(firstline){//then extract bpm and basef parameters
                firstline=false;
                line.str(strline);//convert strline to stringstream object
                string tempstr;
                if(getline(line,tempstr,' ')){
                    params.bpm=stof(tempstr);//append index to end of inst vector
                    audio.spb=60*44100/params.bpm;//samples per beat specified for audio
                }
                else cerr<<"Error parsing bpm!\n";//default in case of error
                tempstr.clear();
                if(getline(line,tempstr,' '))params.basef=stof(tempstr);//append value to end of volume vector
                else cerr<<"Error parsing basef!\n";//default in case of error
                tempstr.clear();
                if(getline(line,tempstr,' '))params.maxinstsize=(int)stof(tempstr);//append value to end of volume vector
                else params.maxinstsize=100000;//default in case not specified
                tempstr.clear();
            }
            else if(strline=="start")com.clear();//delete all previously collected commands if "start" is encountered
            else if(strline=="end")break;//stop collecting commands if "end" is encountered
            else if(strline[0]=='i'||strline[0]=='f'||strline[0]=='w')inst.add(strline);
            else com.add(strline);//add formatted strline to com
        }
    }
    
    songtype(char *filename){
        init(filename);
    }
    songtype(){}
    
    void exportcoms(){
        ofstream ofile("Generated commands.txt",ios::trunc);
        ofile<<params.bpm<<' '<<params.basef<<'\n';
        int i,j;
        for(i=0;i<inst.inst.size();i++){
            ofile<<inst.inst[i].type<<' '<<inst.inst[i].index<<' '<<inst.inst[i].A[0];
            for(j=1;j<inst.inst[i].A.size();j++)ofile<<','<<inst.inst[i].A[j];
            ofile<<'\n';
        }
        ofile<<'\n';
        for(i=0;i<com.com.size();i++)ofile<<com.com[i].tostr()<<'\n';
        ofile.close();
    }
    void exportcoms(const char* filename){
        ofstream ofile(filename,ios::trunc);
        ofile<<params.bpm<<' '<<params.basef<<'\n';
        
        int i,j;
        for(i=0;i<inst.inst.size();i++){
            ofile<<inst.inst[i].type<<' '<<inst.inst[i].index<<' '<<inst.inst[i].A[0];
            for(j=1;j<inst.inst[i].A.size();j++)ofile<<','<<inst.inst[i].A[j];
            ofile<<'\n';
        }
        ofile<<'\n';
        for(i=0;i<com.com.size();i++){
            ofile<<com.com[i].tostr()<<'\n';
        }
        ofile.close();
    }
    void exportwav(const char* filename){
        ofstream ofile(filename,ios::trunc);
        long i;
        if(audio.data.size()>0)ofile<<audio.data[0];
        for(i=1;i<audio.data.size();i++){
            ofile<<" "<<audio.data[i];
        }
        ofile.close();
    }
    
    void genaudio(){
        exportcoms();
        long i;
        cout<<"\n"<<com.com.size()<<" commands total. Processing command: ";
        for(i=0;i<com.com.size();i++){
            cout<<i<<" ";
            if(!com.com[i].mute && (!com.solodetected || com.com[i].solo))
                com.com[i].genaudio(&audio,inst.findinst(com.com[i].insti),params);//generate audio data, from com[i] parameters, in memory pointed to by audio
        }
        audio.data=norm(audio.data);
        i=0;
        while(audio.data[i]==0)i++;
        audio.data.erase(audio.data.begin(),audio.data.begin()+i);
        exportwav("Generated raw audio.txt");
    }
    
    int play(){
        audio.play();
    }
};
#endif
