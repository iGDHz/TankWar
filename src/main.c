/***************************************************************************************
 *	FileName					:	main.c
 *	CopyRight					:
 *	ModuleName					:	
 *
 *	CPU							:
 *	RTOS						:
 *
 *	Create Data					:	2020/5/15
 *	Author/Corportation			:	Hz
 *
 *	Abstract Description		:	
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	
 *
 ***************************************************************************************/

 /**************************************************************
*	Include File Section
**************************************************************/

#include "../inc/main.h"
#include "12864.c"
#include "game.c"

void LevelChoice()
{
	SelectScreen(0);
	Show1616(0,0,level+32*7);//箭头
	Show1616(0,16*1,level);
	Show1616(0,16*2,level+32*1);
	Show1616(0,16*3,level+32*6);//第一关
	Show1616(2,16*1,level);
	Show1616(2,16*2,level+32*2);
	Show1616(2,16*3,level+32*6);//第二关
	Show1616(2*2,0,level+32*8);
	Show1616(2*2,16*1,level);
	Show1616(2*2,16*2,level+32*3);
	Show1616(2*2,16*3,level+32*6);//第三关
	SelectScreen(1);
	Show1616(0,16*5,level);
	Show1616(0,16*6,level+32*4);
	Show1616(0,16*7,level+32*6);//第四关
	Show1616(2,16*5,level);
	Show1616(2,16*6,level+32*5);
	Show1616(2,16*7,level+32*6);//第五关	
}

void Border()//边框
{
	uint pos,i,flag;
	uchar raw;
	raw = 0xb8;
	flag = 0;//屏幕选择
	pos = 0;
	SelectScreen(0);
	Write_com(0x40);//跳转至左屏首列
	SelectScreen(1);
	Write_com(0x40);//跳转至右屏首列
	while(raw<=0xbf)
	{
		SelectScreen(flag);
		flag = !flag;
		Write_com(raw);//跳至对应行
		if(!flag) raw += 1;
		for(i = 0; i < 64; i++)
		{
			Write_data(table[pos++]);
		}	
	}
}

int main()
{	
	uchar temp;
	Write_com(0x3f);
	ClearScreen(2);
	LevelChoice();
	TMOD=0x21;//设置定时器1为工作方式2；
    SCON=0x40;
    PCON=0x00;
    TH1=0xfa;
    TL1=0xfa;
    TR1=1;
	REN=1;
	while(1)
	{
		temp = Receive();
		switch(temp)
		{
			case 0:Border();game();break;
			case 1:{
			if(arrow[1])//位于右屏跳转
			{
				SelectScreen(arrow[1]);
				Show1616(arrow[0],0,level+32*8);
				arrow[1] = 0;
				SelectScreen(arrow[1]);
				Show1616(arrow[0],0,level+32*7);
				while(P1!=0xff) ;
			}
			break;}
			case 2:{
			if(!arrow[1])//左屏跳转
			{
				if(arrow[0] == 4) break;
				SelectScreen(arrow[1]);
				Show1616(arrow[0],0,level+32*8);
				arrow[1] = 1;
				SelectScreen(arrow[1]);
				Show1616(arrow[0],0,level+32*7);	
				while(P1!=0xff);
			}
			break;}
			case 3:{
			if(arrow[0])//箭头位于首行不进行跳转
			{
				SelectScreen(arrow[1]);
				Show1616(arrow[0],0,level+32*8);
				arrow[0] -= 2;//
				Show1616(arrow[0],0,level+32*7);
				while(P1!=0xff);
			}
			break;}
			case 4:{
			if(arrow[0]!=4)//箭头位于最后一行不进行跳转
			{
				if(arrow[1] && arrow[0]==2) break;
				SelectScreen(arrow[1]);
				Show1616(arrow[0],0,level+32*8);
				arrow[0] += 2;//
				Show1616(arrow[0],0,level+32*7);
				while(P1!=0xff);
			}
			break;}
			case 5:Border();game();break;
			default:break;
		}
	}
	return 0;
}
