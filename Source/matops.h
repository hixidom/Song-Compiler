#ifndef _GLIBCXX_VECTOR
#include <vector>
#endif

#include <string>
#include <iostream>

#ifndef CSTDLIB_H
#define CSTDLIB_H
#include <cstdlib>
#endif
using namespace std;

#ifndef MATOPS_H
#define MATOPS_H   

int nextpow(float f,float b){
    int i;
    for(i=0;i<100;i++)
        if(f<=pow(b,i))break;
    return i;
}

void dispfv(vector<float> v,int num){
    if(num==0)num=v.size();
    int i;
    cout<<"\n";
    for(i=0;i<num&&i<v.size();i++)cout<<v[i]<<" ";
}

float stof(string str){
    char *astr=new char [str.size()+1];
    short i;
    for(i=0;i<str.size();i++)astr[i]=str[i];
    astr[str.size()]='\0';
    float f=atof(astr);
    delete astr;
    return f;
}

bool isnum(char c){
    if((c>=48&&c<=57)||c==46||c==45)return true;
    else return false;
}

bool isalpha(char c){
    if((c>=65&&c<=90)||(c>=97&&c<=122))return true;
    else return false;
}

float sum(vector<float> v,int firstelement,int lastelement){
    float sum=0;
    short i;
    for(i=firstelement-1;i<lastelement;i++)sum=sum+v[i];
    return sum;
}

float max(float a,float b){
    if(a>b)return a;
    else return b;
}

float min(float a,float b){
    if(a<b)return a;
    else return b;
}
float trim(float a,float low,float high){
    return min(high,max(low,a));
}

float maxv(vector<float> v){
    if(v.empty())return 0;
    float maxv=v[0];
    int i;
    for(i=1;i<v.size();i++)if(v[i]>maxv)maxv=v[i];
    return maxv;
}

float minv(vector<float> v){
    if(v.empty())return 0;
    float minv=v[0];
    int i;
    for(i=1;i<v.size();i++)if(v[i]<minv)minv=v[i];
    return minv;
}

vector<float> norm(vector<float> v){
    float maxval=max(maxv(v),-minv(v));//find max of absolute value of array.
    int i;
    for(i=0;i<v.size();i++)v[i]/=maxval;//divide each element of array by maxval
    return v;
}

vector<float> linspace(float first,float last,int total){
    vector<float> v;
    total--;
    int i;
    for(i=0;i<=total;i++)v.push_back((float)((last-first)*((float)i/total))+first);
    return v;
}

vector<float> stov(string str){
    vector<float> array;
    string numstr;
    int rangei=0;
    short i;
    for(i=0;i<str.size();i++)if(isnum(str[i]))break;//step through str to first number
    for(;i<str.size();i++){
        if(isnum(str[i]))numstr.push_back(str[i]);
        else{
            array.push_back(stof(numstr));
            if(rangei==2){//Convert a:b:c to array of numbers. e.g. 3:2:9 -> 3,5,7,9
                float n,start=array[array.size()-3],step=array[array.size()-2],end=array[array.size()-1];
                array.erase(array.end()-3,array.end());
                for(n=start;n<end;n+=step)array.push_back(n);
                rangei=0;
            }
            numstr.clear();
            if(str[i]==58){rangei++; continue;} //colon indicates an abbreviated list
            if(str[i]==44 || str[i]==32)continue; //comma or space seperates numbers
            if(str[i]==93 || str[i]==59){//']' or semicolon marks the end of number list
                i++;
                break;
            }
        }
    }
    if(numstr.size()){//assign last collected element to array
        array.push_back(stof(numstr));
        numstr.clear();
    }
    
    char opchar;//variable to contain an operator symbol
    float opnum;//number included in operator, e.g. in "+64", 64 is opnum
    while(i<str.size()){
        opchar=str[i];
        for(i++;i<str.size() && isnum(str[i]);i++)numstr.push_back(str[i]);
        opnum=stof(numstr);
        numstr.clear();
        
        short j;
        switch(opchar){
            case '+':for(j=0;j<array.size();j++)array[j]+=opnum;break;
            case '-':for(j=0;j<array.size();j++)array[j]-=opnum;break;
            case '*':for(j=0;j<array.size();j++)array[j]*=opnum;break;
            case '/':for(j=0;j<array.size();j++)array[j]/=opnum;break;
        }
    }
    return array;
}
    
vector<vector<float> > stovv(string str){
    vector<vector<float> > matrix;
    vector<float> array;
    string numstr;
    short i;
    for(i=0;i<str.size();i++)if(isnum(str[i]))break;//step through str to first number
    for(;i<str.size();i++){
        if(isnum(str[i]))numstr.push_back(str[i]);
        else{
            array.push_back(stof(numstr));
            numstr.clear();
            if(str[i]==44)continue;//if ',' is reached, append number to array and continue
            if(str[i]==59){matrix.push_back(array);array.clear();continue;}//if ';' is reached, append array to bottom of matrix and continue
            if(str[i]==93){
                i++;//if ']' is reached, step over it
                break;
            }
        }
    }
    if(!numstr.empty()){array.push_back(stof(numstr)); numstr.clear();} //append any leftover number to array
    if(!array.empty()){matrix.push_back(array); array.clear();} //append any leftover array to matrix
    
    char opchar;//variable to contain an operator symbol
    float opnum;//number included in operator, e.g. in "+64", 64 is opnum
    while(i<str.size()){
        opchar=str[i];
        for(i++;i<str.size() && isnum(str[i]);i++)numstr.push_back(str[i]);
        opnum=stof(numstr);
        numstr.clear();
        
        short j,k;
        for(j=0;j<matrix.size();j++)
            for(k=0;k<matrix[j].size();k++)
                switch(opchar){
                    case '+':matrix[j][k]+=opnum;break;
                    case '-':matrix[j][k]-=opnum;break;
                    case '*':matrix[j][k]*=opnum;break;
                    case '/':matrix[j][k]/=opnum;break;
                }
    }
    return matrix;
}

int factorial(int n){
    if(n<=0)return 1;
    int result=1;
    for(;n>1;n--)result*=n;
    return result;
}

int combo(int n,int k){
    return factorial(n)/(factorial(k)*factorial(n-k));
}

float prodcombosum(vector<float> x,short num){
    if(num==0)return 1;
    float sum=0;
    int i;
    for(i=0;i<x.size();i++){
        vector<float> subset=x; subset.erase(subset.begin()+i);//temp is a subset of x equal to (x omit xi)
        sum+=x[i]*prodcombosum(subset,num-1);
    }
    return sum/num; // "/num" because each combination will be summed num times
}

vector<float> polyfit(vector<float> x,vector<float> y){
    while(x.size()<y.size())y.pop_back();//shorten y by 1 element until x.size()==y.size()
    while(x.size()>y.size())x.pop_back();//shorten x by 1 element until x.size()==y.size()
    
    //Use divided differences method to calculate polynomial coefficients, a
    vector<float> gx=x,gy=y,c;
    c.push_back(gy[0]);//coefficients of long-form polynomial
    short n,i,j;
    for(n=0;n<x.size()-1;n++){
        for(i=0;i<gy.size()-1;i++)gy[i]=(gy[i+1]-gy[i])/(gx[i+1+n]-gx[i]);
        c.push_back(gy[0]);
        gy.pop_back();
    }
    
    //Now we have to expand the polynomial and collect terms of various degree.
    vector<float> a(c.size(),0);//polynomial coefficents
    for(n=0;n<a.size();n++) //for each element of a
        for(i=n;i<a.size();i++){ //for each c[i] contributing to a[n]
            vector<float> subset;
            for(j=0;j<i;j++)subset.push_back(-x[j]); //only a subset of x applies to each c[i]
            a[n]+=c[i]*prodcombosum(subset,i-n); //c[i] coefficient times sum of all product combinations involving i elements of x
        }    
    return a;
}

float polyeval(vector<float> params,float x){//params is polynomial coefficients for terms 1,x,x^2,...
    float result=0;
    short i;
    for(i=0;i<params.size();i++)result+=params[i]*pow(x,i);
    return result;
}

int issubsetva(vector<float> s1,float *s2,int s2size){//checks if vector s1 is a subset of array s2
    int i,j;
    for(i=0;i<s1.size();i++){
        bool valuefound=false;
        for(j=0;j<s2size;j++){
            if(s1[i]==s2[j]){
                valuefound=true;
                break;
            }
        }
        if(valuefound==false)return 0;
    }
    return 1;
}
int issubsetvv(vector<float> s1,vector<float> s2){//checks if vector s1 is a subset of vector s2
    int i,j;
    for(i=0;i<s1.size();j++){
        bool valuefound=false;
        for(j=0;j<s2.size();j++){
            if(s1[i]==s2[j]){
                valuefound=true;
                break;
            }
        }
        if(valuefound==false)return 0;
    }
    return 1;
}
string ftoa(float n){
    stringstream s;
    s<<n;
    return s.str();
}
vector<float> randv(int n){
    int i;
    vector<float> v;
    for(i=0;i<n;i++)v.push_back((rand()%10000)/10000.0);
    return v;
}
float rand1(){
    return (rand()%10000)/10000.0;
}
float normalrand1(float SD){
    float cutoff=0.0000001;
    float a=(rand1()+0.0000001)/1.0000001,b=rand1();
    return sqrt(-2*log(SD*a))*cos(TWOPI*b);
}

//Linear interpolation or extrapolation of 1D data.  Values of X must be increasing!
vector<float> interp1(vector<float> X, vector<float> Y, vector<float> x){
    int i,j;
    vector<float> y;
    
    for(i=0;i<x.size();i++){
        for(j=1;j<X.size()-1;j++)
            if(x[i]<=X[j])break;
        y.push_back((((Y[j]-Y[j-1])/(X[j]-X[j-1]))*(x[i]-X[j-1]))+Y[j-1]);
    }
    return y;
}

vector<float> addv(vector<float> a,vector<float> b){
    if(a.size()>b.size())a.swap(b);
    int i;
    for(i=0;i<a.size();i++)a[i]+=b[i];
    return a;
}

vector<float> multv(vector<float> a,vector<float> b){
    if(a.size()>b.size())a.swap(b);
    int i;
    for(i=0;i<a.size();i++)a[i]*=b[i];
    return a;
}

vector<float> unionv(vector<float> a,vector<float> b){ //assumes a and b are each ascending
    vector<float> c;
    int i;
    for(i=0;i<max(a.size(),b.size());i++){
        if(i<a.size()){
            if(i<b.size()){
                if(a[i]<b[i]){
                    c.push_back(a[i]);
                    c.push_back(b[i]);
                }
                else{
                    c.push_back(b[i]);
                    c.push_back(a[i]);
                }
            }
            else c.push_back(a[i]);
        }
        else c.push_back(b[i]);
    }
    return c;
}

vector<float> flipv(vector<float> a){
    vector<float> c;
    int i;
    for(i=a.size()-1;i>=0;i--)c.push_back(a[i]);
    return c;
}

vector<float> blurv(vector<float> v,int N){
    vector<float> vblurred=v;
    if(N<2)return v;
    
    int s1,s2;
    if(N%2){
        s1=floor((N-1)/2); s2=ceil((N-1)/2);
    }
    else{
        s1=N/2; s2=N/2;
    }
    
    int i,j;
    for(i=0;i<v.size();i++){
        for(j=-s1;j<=s2;j++){
            if(j==0)continue;
            if(i+j<0)vblurred[i]+=v[0];
            else if(i+j>=v.size())vblurred[i]+=v.back();
            else vblurred[i]+=v[i+j];
        }
        vblurred[i]=vblurred[i]/N;
    }
    return vblurred;
}

#endif
