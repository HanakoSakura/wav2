#ifndef __SAKURA_WAV__
#define __SAKURA_WAV__ 1

/*
  * The code for this file follows WTFPL.
  * (You can use it to do anything.)
  * Coded by Karyna Sakura(Dark Devil)
  * 
  * 我们的虚拟歌姬的核心库之一 · One of the core libraries of our virtual singer.
*/

#include <stdio.h>
#include <string.h>

// --------------------------------
// WAV 读写部分

typedef struct _CHUNK_{
    long start; // 当前块的起始位置
    char id[4]; // 块的ID
    unsigned int size; // 块的数据大小
    void* buffer; // 块的数据(输出用)
} CHUNK,*rCHUNK;

typedef struct _FMT_{
    unsigned short AudioFormat;
    unsigned short NumChannels;
    unsigned int SampleRate;
    unsigned int ByteRate;
    unsigned short BlockAlign;
    unsigned short BitPerSample;
    unsigned short cbSize;
    unsigned short VBPS;
    unsigned int ChannelMask;
    unsigned char GUID[16];
} FMT,*rFMT;

int ReadFMT(rCHUNK ck,rFMT fmt,FILE* fp){
    if(!ck || !fmt) return -1;
    fseek(fp,ck->start+8,SEEK_SET);
    fread(&fmt->AudioFormat,2,1,fp);
    fread(&fmt->NumChannels,2,1,fp);
    fread(&fmt->SampleRate,4,1,fp);
    fread(&fmt->ByteRate,4,1,fp);
    fread(&fmt->BlockAlign,2,1,fp);
    fread(&fmt->BitPerSample,2,1,fp);
    if(ck->size > 16){
        fread(&fmt->cbSize,2,1,fp);
        fread(&fmt->VBPS,2,1,fp);
        fread(&fmt->ChannelMask,4,1,fp);
        fread(fmt->GUID,16,1,fp);
    }
    fmt->BlockAlign = fmt->BitPerSample*fmt->NumChannels/8;
    fmt->ByteRate = fmt->BlockAlign*fmt->SampleRate;
    return 0;
}

int InitChunk(rCHUNK ck,long start,const char* id,unsigned int size,void* data){
    if (!ck || !id) return -1;
    ck->start = start;
    memcpy(ck->id,id,4);
    ck->size = size;
    ck->buffer = data;
    return 0;
}

int InitFMT(rFMT fmt,unsigned short AF,unsigned short NC,unsigned int SR,unsigned short BPS){
    if(!fmt) return -1;
    fmt->AudioFormat = AF;
    fmt->NumChannels = NC;
    fmt->SampleRate = SR;
    fmt->BitPerSample = BPS;

    fmt->BlockAlign = BPS * NC / 8;
    fmt->ByteRate = fmt->BlockAlign * fmt->SampleRate;
    return 0;
}

int InitFMTChunk(rCHUNK ck,rFMT fmt,unsigned int fmt_size){
    if(!ck || !fmt) return -1;
    InitChunk(ck,12,"fmt ",fmt_size,fmt);
    return 0;
}

int LoadChunk(rCHUNK now,rCHUNK next,FILE* fp){
    if(!now||!next||!fp)return -1;
    // 定位到当前块
    if(fseek(fp,now->start,SEEK_SET)) return 1;
    // 获取下一个块的起始位置
    next->start = now->start + 8 + now->size;
    // 跳转到下一块
    if(fseek(fp,next->start,SEEK_SET)) return 1;
    // 读取块信息
    if(!fread((next->id),4,1,fp)) return 2;
    if(!fread(&(next->size),4,1,fp)) return 2;
    return 0;
}

int LoadWAV(FILE* fp,rCHUNK fmt,rCHUNK data){
    CHUNK start,tmp;
    InitChunk(&start,0,"RIFF",4,NULL);
    // 循环读取
    LoadChunk(&start,&tmp,fp);
    do{
        if(!memcmp(tmp.id,"fmt ",4)){
            InitChunk(fmt,tmp.start,tmp.id,tmp.size,NULL);
        }else
        if(!memcmp(tmp.id,"data",4)){
            InitChunk(data,tmp.start,tmp.id,tmp.size,NULL);
        }
    }while(!LoadChunk(&tmp,&tmp,fp));
    return 0;
}

int SaveWAV(FILE* fp,rCHUNK fmt,rCHUNK data){
    fwrite("RIFF",4,1,fp);
    fwrite("\0\0\0\0",4,1,fp);
    fwrite("WAVE",4,1,fp);

    fwrite(fmt->id,4,1,fp);
    fwrite(&fmt->size,4,1,fp);
    fwrite(fmt->buffer,fmt->size,1,fp);

    fwrite(data->id,4,1,fp);
    fwrite(&data->size,4,1,fp);
    fwrite(data->buffer,data->size,1,fp);

    unsigned int riff_size = ftell(fp) - 8;
    fseek(fp,4,SEEK_SET);
    fwrite(&riff_size,4,1,fp);
    return 0;
}

int SeekData(FILE* fp,rCHUNK data){
    return fseek(fp,data->start+8,SEEK_SET);
}

// Hack:它可能不工作,临时替代方案
int Read_WAV_Data_RAW(FILE* fp,rCHUNK data,rFMT fmt,int start_block,unsigned int end_block,void* buffer){
    if(!fp||!data||!buffer||!fmt) return -1;
    SeekData(fp,data);
    if(start_block>end_block||start_block>data->size/fmt->BlockAlign) return 1;
    fseek(fp,start_block*fmt->BlockAlign,SEEK_CUR);
    fread(buffer,fmt->BlockAlign,end_block-start_block,fp);
    return 0;
}

#endif