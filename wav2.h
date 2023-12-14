#ifndef __AKUMA_WAV__
#define __AKUMA_WAV__

#include <stdio.h>
#include <string.h>
//#include <stdlib.h>

/*
  @author Karyna Sakura(Dark Devil)
  @account HanakoSakura
  
  A library for read and write wav files.
  For Sayo I,
  Our Virtual Singer.
*/

typedef unsigned int uint;

typedef struct _CHUNK_ {
    uint offset; // 块的起始位置
    char id[4]; // 块ID
    uint size; // 块大小
    void *data; // 数据
} CHUNK, *rCHUNK;

void Init_Chunk(rCHUNK ch, uint offset, const char* id, uint size, void* data) {
    if(!ch || !id) return;
    ch->data = data;
    memcpy(ch->id, id, 4);
    ch->offset = offset;
    ch->size = size;
}

int Load_Chunk(rCHUNK now,rCHUNK next,FILE *fp){
    if(!now || !next || !fp) return 1;
    // Seek `now`
    if(fseek(fp, now->offset, SEEK_SET)) return 1;
    // Get the offset for `next`
    next->offset = now->offset + 8 + now->size;
    // Jump to `next`
    if(fseek(fp, next->offset, SEEK_SET)) return 1;
    // Read `next` info.
    int c = 0;
    c |= fread(next->id,4,1,fp);
    c |= fread(&next->size,4,1,fp);
    return !c;
}

typedef struct _FMT_ {
    unsigned short af; // Audio Format
    unsigned short nc; // Num Channels
    unsigned int sr; // Sample Rate
    unsigned int br; // Byte Rate
    unsigned short ab; // Align Block
    unsigned short bps; // Bit Per Sample
} FMT,*rFMT;

void Init_FMT(rFMT fmt,unsigned short nc,unsigned int sr,unsigned int bps){
    if(!fmt) return;
    fmt->af = 1;
    fmt->nc = nc;
    fmt->sr = sr;
    fmt->bps = bps;
    fmt->ab = bps * nc / 8;
    fmt->br = fmt->ab * fmt->sr;
}

void Read_FMT(rCHUNK fmt_ck,rFMT fmt_data,FILE *fp){
    if(!fmt_ck || !fmt_data || !fp) return;
    fseek(fp,fmt_ck->offset+8,SEEK_SET);
    fread(fmt_data,sizeof(FMT),1,fp);
}

void Read_Data(rCHUNK fmt_ck,void* data,unsigned int data_size,FILE *fp){
    if(!fmt_ck || !data || !fp) return;
    fseek(fp,fmt_ck->offset+8,SEEK_SET);
    fread(data,1,data_size,fp);
}

typedef struct _WAV_ {
    FILE *fp;
    char riff_id[4]; // 'RIFF'
    uint size; // File Size - 8
    char wave_format[4]; // 'WAVE'
    CHUNK fmt_chunk;
    FMT fmt_data;
    CHUNK data_chunk;
    void* data;
} WAV,*rWAV;

void Load_WAV(const char* path,rWAV wav_t,void* data,unsigned int data_size){
    if(!path) return;
    rWAV wav;
    WAV twav;
    CHUNK start,tmp;
    if(!wav_t) wav = &twav;
    else wav = wav_t;
    // Open FIle
    FILE *fp = fopen(path,"rb");
    if(!fp) return;
    // Init Start Part
    wav->fp = fp;
    fread(wav->riff_id,4,1,fp);
    fread(&wav->size,4,1,fp);
    fread(wav->wave_format,4,1,fp);

    Init_Chunk(&start,0,"RIFF",4,NULL);
    // Loop to read
    Load_Chunk(&start,&tmp,fp);
    do{
        if(!memcmp(tmp.id,"fmt ",4)){
            Init_Chunk(&wav->fmt_chunk,tmp.offset,tmp.id,tmp.size,NULL);
        }else
        if(!memcmp(tmp.id,"data",4)){
            Init_Chunk(&wav->data_chunk,tmp.offset,tmp.id,tmp.size,NULL);
        }
    }while(!Load_Chunk(&tmp,&tmp,fp));
    // Read FMT;
    Read_FMT(&wav->fmt_chunk,&wav->fmt_data,fp);
    // Read Data;
    Read_Data(&wav->data_chunk,data,data_size,fp);
}

void Save_WAV(char* path,rWAV wav_t,void* data,unsigned int data_size,unsigned int sr){
    if(!path||!data) return;
    rWAV wav;
    WAV twav;
    if(!wav_t){
        wav = &twav;
        Init_FMT(&wav->fmt_data,1,sr,16);
        wav->data = data;
        wav->data_chunk.size = data_size;
    }
    else wav = wav_t;

    // Open FIle
    FILE *fp = fopen(path,"wb");
    if(!fp) return;
    
    unsigned int fsize = data_size + 
        // Data Chunk
        8 +
        // FMT Chunk
        16 + 8 +
        // WAV Format
        4;
    
    fwrite("RIFF",4,1,fp);
    fwrite(&fsize,4,1,fp);
    fwrite("WAVEfmt ",4,2,fp);
    fsize = 16;
    fwrite(&fsize,4,1,fp);
    fwrite(&wav->fmt_data,16,1,fp);
    fwrite("data",4,1,fp);
    fwrite(&wav->data_chunk.size,4,1,fp);
    fwrite(data,1,data_size,fp);
}

#endif