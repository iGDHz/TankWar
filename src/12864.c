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
	E = 1;//��ʹ���ź�
	RS = 0;//ָ��ѡ��
	RW = 1;//������;
	P0 = 0xff;//����ʾ����
	while(!busy) ;//æµλ����
}

void Write_data(uchar dat)
{
	uint i;
	CheckState();
	E = 1;//��ʹ���ź�
	RS = 1;//����ѡ��
	RW = 0;//д����
	P0 = dat;//����д��
	E = 0;//����������
	for(i = 0; i < 20; i++) ;
}
void Write_com(uchar command)
{
	CheckState();
	E = 1;//��ʹ���ź�
	RS = 0;//ָ��ѡ��
	RW = 0;//дָ��
	P0 = command;
	E = 0;//����������
}

void SelectScreen(uchar screen)//0��ȫ����1��������2������
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
	line = line & 0x07;//��ȡǰ��λ
	line = line | 0xb8;//ǰ��λ����
	Write_com(line);//������Ӧҳ
}

void SetColum(uchar colum)
{
	colum = colum & 0x3f;//����ǰ6λ
	colum = colum | 0x40; //ǰ6λ����
	Write_com(colum);//������Ӧ��
}

/*void SetStartLine(uchar startline)
{
	startline = startline & 0x3f;//����ǰ6λ
	startline = startline | 0xc0;//ǰ6λ����
	Write_com(startline);//������ʾ��ʼ��
}*/

void Show816(uchar raw,uchar colum,uchar *address)//rawҳ,colum��
{
	uchar i;
	SetLine(raw);//������Ӧ��
	SetColum(colum);//������Ӧ��
	for(i = 0; i < 8; i++)
	{
		if((colum == 8*7 && i == 7) || (!i &&colum == 8*4)) Write_data(0xff);
		else raw?Write_data(address[i]):Write_data(address[i]|0x01);//��һ����д
	}
	
	SetLine(raw+1);//������һ��
	SetColum(colum);//��������
	for(; i < 16; i++)
	{
		if((colum == 8*7 && i == 15) || (i==8 &&colum == 8*4)) Write_data(0xff);
		else Write_data(address[i]);
	}
	
}

void Show1616(uchar raw,uchar colum,uchar *address)
{
	uchar i;
	SetLine(raw);//������Ӧ��
	SetColum(colum);//������Ӧ��
	for(i = 0; i < 16; i++)
	{
		Write_data(*(address++));
	}
	SetLine(raw+1);//������һ��
	SetColum(colum);//��������
	for(i = 0; i < 16; i++)
	{
		Write_data(*(address++));
	}
}
void Set(uchar button)
{
	button?(Write_com(0x3f)):(Write_com(0x3e));//������ʾ
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
		if(arrow[0])//��ͷλ�����в�������ת
		{
			SelectScreen(arrow[1]);
			Show1616(arrow[0],0,level+32*8);
			arrow[0] -= 2;//
			Show1616(arrow[0],0,level+32*7);
			while(P1!=0xff);
		}
		break;}//UP
		case 0x08:{
		if(arrow[0]!=4)//��ͷλ�����һ�в�������ת
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
		if(arrow[1])//λ��������ת
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
		break;}//RIGHT
		default:break;
	}
}

