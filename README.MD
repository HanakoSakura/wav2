# WAV2 つすのねね WAV Library

Useless.

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