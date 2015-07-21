#ifndef CSTDLIB_H
#define CSTDLIB_H
#include <cstdlib>
#endif

#ifndef _GLIBCXX_FSTREAM
#include <fstream>
#endif

#include <iostream>
#include <windows.h>
#include <cstring>

#ifndef CTIME_H
#define CTIME_H
#include <ctime>
#endif

#ifndef OAL_TOOLS_H
#define OAL_TOOLS_H

#include "OpenAL Soft/al.h"
#include "OpenAL Soft/alc.h"
#include "OpenAL Soft/alext.h"
//#include "OpenAL Soft/alut.h" //I couldn't figure out how to compile ALUT files
using namespace std;

int CheckForError(string msg){
    return 1;//suppresses the error messages that would follow otherwise
    
    int error=alGetError();
    string premsg;
    
    switch(error){
        case AL_NO_ERROR: return 0;
        case AL_INVALID_NAME: premsg="Invalid ID passed to OAL function"; break;
        case AL_INVALID_ENUM: premsg="Invalid enum value passed to OAL function"; break;
        case AL_INVALID_VALUE: premsg="Invalid value passed to OAL function"; break;
        case AL_INVALID_OPERATION: premsg="Invalid operation requested"; break;
        case AL_OUT_OF_MEMORY: premsg="Not enough memory for requested operation"; break;
    }
        
    //Display error message to console
    cout<<"\n"<<premsg<<": "<<msg<<"\n";
    return 1;
}
    
//Return type should reflected intended sample data type. e.g. for 8-bit, return char; for 16-bit, return short; for 32-bit, return float...
char formatsample(float sample,ALenum format)
{
    if(sample>1)sample=1;
    else if(sample<-1)sample=-1;
    switch(format){
        case AL_FORMAT_MONO8:
        case AL_FORMAT_STEREO8:
            return (char)(127.0*(sample+1.0));break;
        case AL_FORMAT_MONO16:
        case AL_FORMAT_STEREO16:
            return (short)(127.0*sample);break;
        case AL_FORMAT_MONO_FLOAT32:
        case AL_FORMAT_STEREO_FLOAT32:
            return sample; //This produces no output audio for some reason
        default:
            cout<<"unrecognized audio playback format\n";
            return 0;break;
        }
    return 0;
}    

int playbufferv(vector<float> data){
    int numbuffers=6,bitspersample=8;
    short channels=2;
    
    //Initialize OpenAL
    ALCdevice *device;                                                          //Create an OpenAL Device
    ALCcontext *context;                                                        //And an OpenAL Context
    device = alcOpenDevice(NULL);                                               //Open the device
    CheckForError("alcOpenDevice");
    
    context = alcCreateContext(device, NULL);                                   //Give the device a context
    CheckForError("alcCreateContext");
    alcMakeContextCurrent(context);                                             //Make the context the current
    CheckForError("alcMakeContextCurrent");

    ALuint source;                                                              //Is the name of source (where the sound comes from)
    ALuint buffer[numbuffers];                                                           //Stores the sound data
    ALuint frequency=44100;                                                     //The Sample Rate of the WAVE file
    ALenum format=0;
    
    if(channels==1){
        if(bitspersample==8)format=AL_FORMAT_MONO8;                             //The audio format (bits per sample, number of channels)
        else if(bitspersample==16)format=AL_FORMAT_MONO16;
        else if(bitspersample==32)format=AL_FORMAT_MONO_FLOAT32;
    }
    if(channels==2){
        if(bitspersample==8)format=AL_FORMAT_STEREO8;                           
        else if(bitspersample==16)format=AL_FORMAT_STEREO16;
        else if(bitspersample==32)format=AL_FORMAT_STEREO_FLOAT32;
    }
    
    alGenBuffers(numbuffers, buffer);                                                   //Generate one OpenAL Buffer and link to "buffer"
    alGenSources(1, &source);                                                   //Generate one OpenAL Source and link to "source"
    CheckForError("alGenSources");                                              //Error during buffer/source generation
    
    /*alBufferData(buffer, format, buf, dataSize, frequency);                     //Store the sound data in the OpenAL Buffer
    alGetError();
    if(alGetError() != AL_NO_ERROR){
        alDeleteSources(1, &source);                                            //Delete the OpenAL Source
        alDeleteBuffers(1, &buffer);                                            //Delete the OpenAL Buffer
        alcMakeContextCurrent(NULL);                                            //Make no context current
        alcDestroyContext(context);                                             //Destroy the OpenAL Context
        alcCloseDevice(device);                                                 //Close the OpenAL Device
        return CheckForError("Error loading ALBuffer");                          //Error during buffer loading
    }*/
  
    //Sound setting variables
    ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };                                    //Position of the source sound
    ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };                                    //Velocity of the source sound
    ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };                                  //Position of the listener
    ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };                                  //Velocity of the listener
    ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };                 //Orientation of the listener
                                                                                //First direction vector, then vector pointing up) 
    //Listener                                                                               
    alListenerf(AL_GAIN,.16f);                                                  //Set gain of the listener
    alListenerfv(AL_POSITION,    ListenerPos);                                  //Set position of the listener
    alListenerfv(AL_VELOCITY,    ListenerVel);                                  //Set velocity of the listener
    alListenerfv(AL_ORIENTATION, ListenerOri);                                  //Set orientation of the listener
    
    //Source
    alSourcef (source, AL_PITCH,    1.0f     );                                 //Set the pitch of the source
    alSourcef (source, AL_GAIN,     1.0f     );                                 //Set the gain of the source
    alSourcefv(source, AL_POSITION, SourcePos);                                 //Set the position of the source
    alSourcefv(source, AL_VELOCITY, SourceVel);                                 //Set the velocity of the source
    alSourcei (source, AL_LOOPING,  AL_FALSE );                                 //Set if source is looping sound
    
    //Split up, buffer, queue, and play the data
    long chunksize=4096,offset,chunknum;
    unsigned char* buf=new unsigned char[channels*chunksize];
    ALuint bufferptr;
    
    ALint freebuffer=0;
    for(offset=0,freebuffer=0;freebuffer<numbuffers;offset+=chunksize,freebuffer++){
        long i,j; //i is buffer position, j is data position
        for(i=0,j=offset;i<channels*chunksize;i+=channels,j++){
            if(j<data.size()){
                buf[i]=formatsample(data[j],format);
                if(channels==2)buf[i+1]=formatsample(data[j],format);
            }
            else{
                buf[i]=0;
                if(channels==2)buf[i+1]=0;
            }
        }
        
        alBufferData(buffer[freebuffer], format, buf, channels*chunksize, frequency);           //Store the sound data in the OpenAL Buffer
    }
    CheckForError("Error filling buffers");
    alSourceQueueBuffers(source, numbuffers, buffer);                                //Add buffers to source queue
    CheckForError("Error adding buffer to source");
    alSourcePlay(source);
    
    freebuffer=0;
    if(offset<data.size()-1){    
        for(;offset<data.size();offset+=chunksize){
            long i,j; //i is buffer position, j is data position
            for(i=0,j=offset;i<channels*chunksize;i+=channels,j++){
                if(j<data.size()){
                    buf[i]=formatsample(data[j],format);
                    if(channels==2)buf[i+1]=formatsample(data[j],format);
                }
                else{
                    buf[i]=0;
                    if(channels==2)buf[i+1]=0;
                }
            }
            
            while(!freebuffer){
                alGetSourcei(source,AL_BUFFERS_PROCESSED,&freebuffer);
                if(freebuffer)alSourceUnqueueBuffers(source,1,&bufferptr);
                CheckForError("Error unqueueing buffer");
            }
            alBufferData(bufferptr, format, buf, channels*chunksize, frequency);//Store the sound data in the OpenAL Buffer
            freebuffer--;
            alSourceQueueBuffers(source, 1, &bufferptr);                        //Add buffer to source queue
        }
    }
    
    freebuffer=0;
    while(freebuffer!=numbuffers)alGetSourcei(source,AL_BUFFERS_PROCESSED,&freebuffer);
    clock_t t;
    t=clock();
    while(((float)clock()-t)/CLOCKS_PER_SEC<((double)chunksize/frequency)+0){}  //wait for last chunk to finish playing
    alSourceStop(source);
    
    //Clean-up
    alDeleteSources(1, &source);                                                //Delete the OpenAL Source
    CheckForError("alDeleteSources");
    alDeleteBuffers(numbuffers, buffer);                                        //Delete the OpenAL Buffer
    CheckForError("alDeleteBuffers");
    alcMakeContextCurrent(NULL);                                                //Make no context current
    CheckForError("alcMakeContextCurrent");
    alcDestroyContext(context);                                                 //Destroy the OpenAL Context
    CheckForError("alcDestroyContext");
    alcCloseDevice(device);                                                     //Close the OpenAL Device
    CheckForError("alcCloseDevice");
    delete buf;
    
    return EXIT_SUCCESS;                                                        
}

class wavetype{
    public:
    DWORD size,dataSize,chunkSize,sampleRate,avgBytesPerSec;
    short formatType,channels,bytesPerSample,bitsPerSample;
    unsigned char* buf;        
    
    int load(const char* filename){
        //Loading of the WAVE file
        FILE *fp = NULL;                                                            //Create FILE pointer for the WAVE file
        fp=fopen("sample1.wav","rb");                                                 //Open the WAVE file
        if (!fp) return CheckForError("Failed to open wave file");                        //Could not open file
        
        //Variables to store info about the WAVE file (all of them is not needed for OpenAL)
        char type[4];
        
        //Check that the WAVE file is OK
        fread(type,sizeof(char),4,fp);                                              //Reads the first bytes in the file
        if(type[0]!='R' || type[1]!='I' || type[2]!='F' || type[3]!='F')            //Should be "RIFF"
        return CheckForError ("Not RIFF");                                            //Not RIFF
    
        fread(&size, sizeof(DWORD),1,fp);                                           //Continue to read the file
        fread(type, sizeof(char),4,fp);                                             //Continue to read the file
        if (type[0]!='W' || type[1]!='A' || type[2]!='V' || type[3]!='E')           //This part should be "WAVE"
        return CheckForError("not WAVE");                                            //Not WAVE
        
        fread(type,sizeof(char),4,fp);                                              //Continue to read the file
        if (type[0]!='f' || type[1]!='m' || type[2]!='t' || type[3]!=' ')           //This part should be "fmt "
        return CheckForError("not fmt ");                                            //Not fmt 
        
        //Now we know that the file is a acceptable WAVE file
        //Info about the WAVE data is now read and stored
        fread(&chunkSize,sizeof(DWORD),1,fp);
        fread(&formatType,sizeof(short),1,fp);
        fread(&channels,sizeof(short),1,fp);
        fread(&sampleRate,sizeof(DWORD),1,fp);
        fread(&avgBytesPerSec,sizeof(DWORD),1,fp);
        fread(&bytesPerSample,sizeof(short),1,fp);
        fread(&bitsPerSample,sizeof(short),1,fp);
        
        fread(type,sizeof(char),4,fp);
        if (type[0]!='d' || type[1]!='a' || type[2]!='t' || type[3]!='a')           //This part should be "data"
        return CheckForError("Missing DATA");                                        //not data
        
        fread(&dataSize,sizeof(DWORD),1,fp);                                        //The size of the sound data is read
        
        //Display the info about the WAVE file
        /*cout << "Chunk Size: " << chunkSize << "\n";
        cout << "Format Type: " << formatType << "\n";
        cout << "Channels: " << channels << "\n";
        cout << "Sample Rate: " << sampleRate << "\n";
        cout << "Average Bytes Per Second: " << avgBytesPerSec << "\n";
        cout << "Bytes Per Sample: " << bytesPerSample << "\n";
        cout << "Bits Per Sample: " << bitsPerSample << "\n";
        cout << "Data Size: " << dataSize << "\n";*/
            
        buf= new unsigned char[dataSize];                            //Allocate memory for the sound data
        cout << fread(buf,sizeof(BYTE),dataSize,fp) << " bytes loaded\n";           //Read the sound data and display the 
                                                                                //number of bytes loaded.
                                                                                //Should be the same as the Data Size if OK
        fclose(fp);                                                             //Close the WAVE file
    }
    
    wavetype(){};
    wavetype(string filename){
        load(filename.c_str());
    }
    ~wavetype(){
        delete[] buf;                                                               //Delete the sound data buffer
    };
};

#endif
