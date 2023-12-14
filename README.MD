# WAV2 - つすのねね WAV Library

Useless. · 一个小小的WAV读写库

## Usage

+ Read WAV
```c
FILE *fp = fopen("./1.wav","rb");

LoadWAV(fp,&fmt,&data);
ReadFMT(&fmt,&fm,fp);
```

+ Write WAV
```c
InitFMT(&fm,1,1,64000,16);
InitFMTChunk(&fmt,&fm,16);
InitChunk(&data,0,"data",sizeof(p),p);

SaveWAV(out,&fmt,&data);
```

## Usage (wav2.h)

+ Read WAV

`Load_WAV("1.wav",&wav,NULL,0);` 读取WAV头，将`NULL`和`0`替换为缓冲区和读取字节数

第二个参数可以为`NULL`，此时为仅读取WAV数据

+ Write WAV

`Save_WAV("2.wav",NULL,buffer,128000,64000);` 
第二个参数为有效fmt数据的WAV头，`NULL`则使用默认参数（单声道，16位采样深度）
