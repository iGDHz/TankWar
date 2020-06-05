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
	Show1616(0,0,level+32*7);//��ͷ
	Show1616(0,16*1,level);
	Show1616(0,16*2,level+32*1);
	Show1616(0,16*3,level+32*6);//��һ��
	Show1616(2,16*1,level);
	Show1616(2,16*2,level+32*2);
	Show1616(2,16*3,level+32*6);//�ڶ���
	Show1616(2*2,0,level+32*8);
	Show1616(2*2,16*1,level);
	Show1616(2*2,16*2,level+32*3);
	Show1616(2*2,16*3,level+32*6);//������
	SelectScreen(1);
	Show1616(0,16*5,level);
	Show1616(0,16*6,level+32*4);
	Show1616(0,16*7,level+32*6);//���Ĺ�
	Show1616(2,16*5,level);
	Show1616(2,16*6,level+32*5);
	Show1616(2,16*7,level+32*6);//�����	
}

void Border()//�߿�
{
	uint pos,i,flag;
	uchar raw;
	raw = 0xb8;
	flag = 0;//��Ļѡ��
	pos = 0;
	SelectScreen(0);
	Write_com(0x40);//��ת����������
	SelectScreen(1);
	Write_com(0x40);//��ת����������
	while(raw<=0xbf)
	{
		SelectScreen(flag);
		flag = !flag;
		Write_com(raw);//������Ӧ��
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
	TMOD=0x21;//���ö�ʱ��1Ϊ������ʽ2��
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
			if(arrow[1])//λ��������ת
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
			if(!arrow[1])//������ת
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
			if(arrow[0])//��ͷλ�����в�������ת
			{
				SelectScreen(arrow[1]);
				Show1616(arrow[0],0,level+32*8);
				arrow[0] -= 2;//
				Show1616(arrow[0],0,level+32*7);
				while(P1!=0xff);
			}
			break;}
			case 4:{
			if(arrow[0]!=4)//��ͷλ�����һ�в�������ת
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
