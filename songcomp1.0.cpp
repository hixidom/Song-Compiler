//Song Compiler written by Andrew Barrette
//g++ -o simpleplot simpleplot.cpp -lopenal32  //use this format to compile

#include "compiler.h"
using namespace std;

int main(int argc, char **argv){
    songtype song;
    if(argc>1){
        int fnsize=strlen(argv[0]);
        char filename[fnsize+1];//name of file to be used in initializing song object
        int i;
        for(i=0;i<fnsize+1;i++){
            filename[i]=argv[1][i];
        }
        cout<<"\n'"<<filename<<"'\n";
        song.init(filename);//create song object initialized with data from song file specified by filename
    }
    else{
        char filename[]="ToCompile.txt\0";
        cout<<"\n'"<<filename<<"'\n";
        song.init(filename);//create song object initialized with data from song file specified by filename
    }
    cout<<"song upload complete.\n";
    song.genaudio();//generate audio in song.audio
    cout<<"song generation complete. audio size="<<song.audio.data.size()<<"\n";
    
    char p[2]="y";
    while(!strcmp(p,"y")){
        song.play();
        cout<<"play again? (y/n): ";
        cin>>p;
    }
    
    /*//sound test
    cout<<"song playback complete.\n";
    wavetype wave1("WAV/sample1.wav");
    wave1.play()*/
    return 1;
}
