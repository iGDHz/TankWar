#include<reg52.h>
#include<string.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int

sbit dula = P2^6;
sbit wela = P2^7;
sbit GAME = P1^6;
sbit SPK = P2^5;

uchar *music_code;
uint music_times;
uint point;
uchar code points[] = {
0x3f,0x06,0x5b,0x4f,0x66,
0x6d,0x7d,0x07,0x7f,0x6f
};							

unsigned char code Music_Begin[]={ 0xf8,0x8c, 0xf9,0x5b, 0xfa,0x15, 0xfa,0x67, 0xfb,0x04,
                                  0xfb,0xa6,  0xfc,0x0c, 0xfe,0x22};
unsigned char code Music_Over[]={ 0xf8,0x8c};

void delay(uint xms)
{
	uint i,j;
	for(i = xms; i > 0; i--)
	{
		for(j = 250; j > 0; j--)
		;
	}
}

void ShowPoint(uint point)
{
	int i,point_num[6];
	uchar wei;
	i = 5;
	while(i>=0)	
	{
		point_num[i--] = point%10;
		point /= 10;
	}
	wei = 0xdf;
	for(i = 5; i >= 0; i--)
	{
		dula = 1;
		P0 = points[point_num[i]];
		dula = 0;
		wela = 1;
		P0 = wei;
		wela = 0;
		wei = _crol_(wei,-1);
		P0 = 0x00;//消隐
		delay(1);
	}
}

void Send (unsigned char dat) //发送单片机信息
{
        SBUF=dat;
        while(TI==0);
        TI=0;
}

unsigned int Receive(void)	 //接收单片机信息
{
        unsigned char dat;
        while(RI==0);
        RI=0;
        dat=SBUF;
        return dat;
}

void Music()
{
	music_times = 0;
	TH0 =  music_code[music_times];
	TL0 =  music_code[music_times];
	TR0 = 1;
	EA = 1;
	ET0 = 1;
	while(music_times<strlen(music_code))
	{						  
		music_times += 2;
		delay(30);
	}
	TR0 = 0;
	EA = 0;
	ET0 =0;
}

void Button()
{
	uchar temp;
	temp = P1;
	temp = ~temp;
	temp = temp & 0x3f;
	switch(temp)
	{
		case 0x01:Send(0);music_code = Music_Over;Music();break;
		case 0x02:Send(5);music_code = Music_Begin;Music();break;
		case 0x04:Send(3);break;//UP
		case 0x08:Send(4);break;//DOWN
		case 0x10:Send(1);break;//LEFT
		case 0x20:Send(2);break;//RIGHT
		default:;break;
	}
	while(P1!=0xff) ;
}

void main()
{
   	TMOD=0x21;
    SCON=0x50;
    PCON=0x00;
    TH1=0xfa;
    TL1=0xfa;
    TR1=1;
	ES=1;//开串口中断
    REN=1;//允许接收
	point = 0; 		 
	while(1)
	{
		ShowPoint(point);
		Button();
		delay(5);
	}
}

void timer0() interrupt 1 
{

	TH0 =  music_code[music_times];
	TL0 =  music_code[music_times+1];
	SPK = !SPK;
}

void re() interrupt 4
{
        uint temp;
		if(RI = 1)
		{
      	  while(RI==0);
     	  RI=0;
     	  temp=SBUF;
    	  point += temp;
		}
}