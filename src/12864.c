/***************************************************************************************
 *	FileName					:	12864.c
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
*	Prototype Declare Section
**************************************************************/

uint arrow[2];

void delay(uint xms)
{
	uint i,j;
	for(i = xms; i > 0; i--)
	{
		for(j = 250; j > 0; j--);
	}	
}
void CheckState()
{
	E = 1;//打开使能信号
	RS = 0;//指令选择
	RW = 1;//读数据;
	P0 = 0xff;//读显示数据
	while(!busy) ;//忙碌位缓冲
}

void Write_data(uchar dat)
{
	uint i;
	CheckState();
	E = 1;//打开使能信号
	RS = 1;//数据选择
	RW = 0;//写数据
	P0 = dat;//数据写入
	E = 0;//产生高脉冲
	for(i = 0; i < 20; i++) ;
}
void Write_com(uchar command)
{
	CheckState();
	E = 1;//打开使能信号
	RS = 0;//指令选择
	RW = 0;//写指令
	P0 = command;
	E = 0;//高脉冲输入
}

void SelectScreen(uchar screen)//0：全屏，1：左屏，2：右屏
{
	switch(screen)
	{
		case 0:CS1=0;delay(2);CS2=1;delay(2);break;
		case 1:CS1=1;delay(2);CS2=0;delay(2);break;
		case 2:CS1=0;delay(2);CS2=0;delay(2);break;
	}
}

void ClearScreen(uchar screen)
{
	uchar i,j;
	SelectScreen(screen);
	for(i = 0; i < 8; i++)
	{
	SetLine(i);
	SetColum(0);
	for(j = 0; j < 64; j++)
	{
		Write_data(0);
	}
	}
}
void SetLine(uchar line)
{
	line = line & 0x07;//获取前三位
	line = line | 0xb8;//前三位补上
	Write_com(line);//跳至对应页
}

void SetColum(uchar colum)
{
	colum = colum & 0x3f;//保存前6位
	colum = colum | 0x40; //前6位补齐
	Write_com(colum);//跳至对应列
}

/*void SetStartLine(uchar startline)
{
	startline = startline & 0x3f;//保存前6位
	startline = startline | 0xc0;//前6位补齐
	Write_com(startline);//设置显示起始行
}*/

void Show816(uchar raw,uchar colum,uchar *address)//raw页,colum列
{
	uchar i;
	SetLine(raw);//跳至对应行
	SetColum(colum);//跳至对应列
	for(i = 0; i < 8; i++)
	{
		if((colum == 8*7 && i == 7) || (!i &&colum == 8*4)) Write_data(0xff);
		else raw?Write_data(address[i]):Write_data(address[i]|0x01);//第一行书写
	}
	
	SetLine(raw+1);//跳至下一行
	SetColum(colum);//跳回首列
	for(; i < 16; i++)
	{
		if((colum == 8*7 && i == 15) || (i==8 &&colum == 8*4)) Write_data(0xff);
		else Write_data(address[i]);
	}
	
}

void Show1616(uchar raw,uchar colum,uchar *address)
{
	uchar i;
	SetLine(raw);//跳至对应行
	SetColum(colum);//跳至对应列
	for(i = 0; i < 16; i++)
	{
		Write_data(*(address++));
	}
	SetLine(raw+1);//跳至下一行
	SetColum(colum);//跳回首列
	for(i = 0; i < 16; i++)
	{
		Write_data(*(address++));
	}
}
void Set(uchar button)
{
	button?(Write_com(0x3f)):(Write_com(0x3e));//开关显示
}


void ButtonDown()
{
	uchar temp;
	temp = P1;
	temp = ~temp;
	temp = temp & 0x3f;
	switch(temp)
	{
		case 0x01:
		case 0x02:GAME = 0;delay(100);GAME = 1;Border();game();break;
		case 0x04:{
		if(arrow[0])//箭头位于首行不进行跳转
		{
			SelectScreen(arrow[1]);
			Show1616(arrow[0],0,level+32*8);
			arrow[0] -= 2;//
			Show1616(arrow[0],0,level+32*7);
			while(P1!=0xff);
		}
		break;}//UP
		case 0x08:{
		if(arrow[0]!=4)//箭头位于最后一行不进行跳转
		{
			if(arrow[1] && arrow[0]==2) break;
			SelectScreen(arrow[1]);
			Show1616(arrow[0],0,level+32*8);
			arrow[0] += 2;//
			Show1616(arrow[0],0,level+32*7);
			while(P1!=0xff);
		}
		break;}//DOWN
		case 0x10:{
		if(arrow[1])//位于右屏跳转
		{
			SelectScreen(arrow[1]);
			Show1616(arrow[0],0,level+32*8);
			arrow[1] = 0;
			SelectScreen(arrow[1]);
			Show1616(arrow[0],0,level+32*7);
			while(P1!=0xff) ;
		}
		break;}//LEFT
		case 0x20:{
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
		break;}//RIGHT
		default:break;
	}
}

