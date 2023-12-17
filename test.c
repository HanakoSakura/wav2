#include <stdlib.h>

#include "wav2.h"

WAV wav;
int main(){
    // 加载WAV文件头，后面两个参数是数据缓冲区及其大小
    Load_WAV("1.wav",&wav,NULL,0);
    // 打印头信息
    printf("fmt : %hu,%u,%hu\n",wav.fmt_data.nc,wav.fmt_data.sr,wav.fmt_data.bps);
    printf("data size : %u\n",wav.data_chunk.size);

    // 加载数据
    void *buffer = malloc(wav.data_chunk.size);
    Read_Data(&wav.data_chunk,buffer,wav.data_chunk.size,wav.fp);

    // 保存数据
    Save_WAV("2.wav",NULL,buffer,wav.data_chunk.size,wav.fmt_data.sr);

    return 0;
}