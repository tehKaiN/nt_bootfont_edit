/*
2k pro sp4 pl
           |           |           |           |           |           |           |           |           |           |
4d 64 65 54 15 04 00 00 00 01 00 00 01 00 00 00 34 00 00 00 34 11 00 00 00 11 00 00 22 00 00 00 00 00 00 00 00 00 00 00
           |           |           |
00 00 00 00 10 01 02 08 10 45 13 00

xp pro sp2 pl

4d 64 65 54 15 04 00 00 00 01 00 00 01 00 00 00 34 00 00 00 34 13 00 00 00 13 00 00 24 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 10 01 02 08 10 45 13 00

xppsp3pl

4d 64 65 54 15 04 00 00 00 01 00 00 01 00 00 00 34 00 00 00 34 13 00 00 00 13 00 00 24 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 10 01 02 08 10 45 13 00

xp home sp2 pl
           |           |           |           |           |           |           |           |           |           |
4d 64 65 54 15 04 00 00 00 01 00 00 01 00 00 00 34 00 00 00 34 13 00 00 00 13 00 00 24 00 00 00 00 00 00 00 00 00 00 00
           |           |           |
00 00 00 00 10 01 02 08 10 45 13 00

xp pro sp3 ru

4d 64 65 54 19 04 00 00 00 01 00 00 01 00 00 00 34 00 00 00 34 13 00 00 00 13 00 00 24 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 10 01 02 08 10 45 13 00

xp sp3 chs simple
           |           |           |           |           |           |           |           |           |           |
4d 64 65 54 04 08 00 00 be 00 00 00 9f 22 00 00 34 00 00 00 4e 0e 00 00 1a 0e 00 00 5c de 04 00 a1 fe 00 00 00 00 00 00
           |           |           |
00 00 00 00 10 01 02 08 10 00 00 00

xp sp2 japan
  MdeT     |  locale   |  charNo   |  char2No  |  font1Addr|  font2Addr| font1Size | font2Size |           |           |
4d 64 65 54 11 04 00 00 be 00 00 00 f8 0d 00 00 34 00 00 00 4e 0e 00 00 1a 0e 00 00 e0 f6 01 00 81 84 87 9f e0 ea fa fc
           |           |           |
00 00 00 00 10 01 02 08 10 1c 00 01

*/

struct h
	{
	DWORD MdeT;
	DWORD localeID; //0x0415 == 1045 == POL
	DWORD charNo; // 256; chs: 0xBE == 190
	DWORD char2No; //1; chs: 0x229F == 8863
	DWORD fontData1Addr; //0x34 == 52
	DWORD fontData2Addr; //0x1334 == 4916; 2k: 0x1134 == 4404; chs: 0x0E4E == 3662
	DWORD fontData1Size; //0x1300 == 4864; 2k: 0x1100 == 4352; chs: 0x0E1A == 3610
	DWORD fontData2Size; //0x24 == 36; 2k: 0x22 == 34; chs: 0x04DE5C == 319068
	unsigned char unkRest[20];
	}

//chs: fontData1Size/charNo == 19 == sizeof(ch1)
//chs: fontData2Size/unk3 == 36 == sizeof(ch2)

//localeID: http://msdn.microsoft.com/en-us/library/dd318693%28v=VS.85%29.aspx

/*

0x10 == 16
0x11 == 17
0x13 == 19
0x22 == 34
0x24 == 36
0x34 == 52

*/

//from russian bootfont.bin

struct ch1
	{
	unsigned char ansiNo; //?
	unsigned char data[16]; //chs g
	WORD unicodeNo;
	}

//from chs, based on 0x3002

struct ch2
	{
	WORD gbkNo; //reversed endian?
	unsigned char data[32]; //16x16
	WORD unicodeNo;
	}
