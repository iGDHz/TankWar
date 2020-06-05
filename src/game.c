/***************************************************************************************
 *	FileName					:	game.c
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


tank xdata own;//�û�����
tank xdata ai[5];//AI
uchar Walls_life[64];//ǽ������ֵ���������ж��Ƿ�ɴݻ���ǰ�У�
uchar *map;
uchar Wall_Num;
uint point;
uint t,t1,t2;
uint butt;

void Send (unsigned int dat) //��������
{
        SBUF=dat;
        while(TI==0);
        TI=0;
}

unsigned char Receive(void)	//��������
{
        unsigned char dat;
        if(RI) 
		{
        	RI=0;
        	dat=SBUF;
        	return dat;
		}
		return 6;
}

void message(uint leveltype,uint time)
{
		SelectScreen(1);
		Show816(0,32,num+16*12);
		Show816(0,32+8*1,num+16*13);
		Show816(0,32+8*2,num+16*leveltype);//"�ؿ�leveltype"
		Show816(2,32,num+16*(time/60));
		Show816(2,32+8*1,num+16*10);
		Show816(2,32+8*2,num+16*(time%60/10));
		Show816(2,32+8*3,num+16*(time%10));//"2:30"
		Show816(4,32,num+16*14);
		Show816(4,32+8*1,num+16*11);
		Show816(4,32+8*2,num+16*own.life);//����ֵ
}

Status WallCheck(uchar Wall_Num,uchar raw,uchar colum)//��ײ��⣬����ǽ���Ƿ���ERROR,δ����ǽ�ڷ���SUCCESS
{
	uint i;
	for(i = 0; i < Wall_Num; i++)
	{
		if(Walls_life[i] && raw == *(map+Wall_Num+i) && colum == *(map+i))
		{
			 return ERROR;
		}
	}
}

Status CheckWin()
{
	uint i;
	for(i = 0; i < 5; i++)
	{
		if(ai[i].life) return ERROR;
	}
	return SUCCESS;
}

Status CheckDefeat()
{
	uint i;
	if(own.bullet_num)
	{
		for(i = 0; i < own.bullet_num; i++)
		{
			if(own.raw == 7 && own.colum == 4*12)
			{
				return SUCCESS;
			}
		}
	}
	for(i = 0; i < 5; i++)
	{
		if(ai[i].bullet_num)
		{
			if(ai[i].bullet[0].raw == 7 && ai[i].bullet[0].colum == 4*12) return SUCCESS;
		}
	}
	return ERROR;
}
void Collision(uchar Wall_Num,uchar raw,uchar colum,uint type)//type Ϊ̹������ 0��own 1:ai
{
	uint i,j;
	for(i = 0; i < Wall_Num; i++)
	{
		if(Walls_life[i] == 5) continue;//��ǽ�޷��ݻ�
		if(Walls_life[i] && raw == *(map+Wall_Num+i) && colum == *(map+i))
		{
			if((--Walls_life[i]) == 0)
			{
				SelectScreen(colum/64);
				SetLine(raw);
				SetColum(colum%64);
				for(j = 0; j <4; j++)
				{
					Write_data(0x00);
				}
			 }
		}
	}	
	if(!type)//����̹�˹���
	{
		for(i = 0; i < 5; i++)
		{
			if(!ai[i].life) continue;//̹��������
			if(raw == ai[i].raw && colum == ai[i].colum)
			{
				 ai[i].life--;
				 Send(50);
			}
		}
	}
	else //�з�̹�˹���
	{
		if(raw == own.raw && colum == own.colum) 
		{
			own.life--;
			DelTank(&own);
			CreateTank(3,own.life,7,4*10,0,&own);//���� ����ֵ ��7�� ��40��
		}
	}
}

void CreateBullet(Tank tank_now)
{
	if(tank_now->bullet_num == 5) return ;
	(tank_now->bullet[tank_now->bullet_num]).direction = tank_now->direction;//����ȷ��
	(tank_now->bullet[tank_now->bullet_num]).raw = tank_now->raw;//��λ��ȷ��
	(tank_now->bullet[tank_now->bullet_num]).colum = tank_now->colum;//��λ��ȷ��
	tank_now->bullet_num ++;//��������
}

void CreateTank(uint direction,uint life,uchar raw,uchar colum,uint TankBullet,Tank new_tank) //���� ����ֵ �� �� �ӵ���Ŀ  ̹�˽ṹ��
{
	uint i,j,length;
	new_tank->direction = direction;
	new_tank->life = life;//����ֵ
	new_tank->raw = raw;//��(1`7)
	new_tank->colum = colum;//��(0~96)
	new_tank->bullet_num = TankBullet;
	if(direction < 3)
	{
		length = 6;
		i = (direction == 1)?8:14;//8.����,14.����
		//for(j = 0; j < 2; j++) new_tank->expos[j] = tanksite[i++];
		//for(j = 0; j < 4; j++) new_tank->expos[j] = tanksite[i++];
	}
	else
	{
		length = 4;
		i = (direction == 3)?0:4;//0.���ϣ�4.����
		//for(j = 0; j < 4; j++) new_tank->pos[j] = tanksite[i++];
	}
	for(j = 0; j < length; j++) new_tank->pos[j] = tanksite[i++];
}

void DelTank(Tank tank_now)//̹��ɾ����ˢ��λ�ã�
{	
	uint i;
	SelectScreen(tank_now->colum/64);//ѡ��̹��������Ļ
	SetLine(tank_now->raw);
	//length = tank_now->direction>2?6:4;
	if(tank_now->direction > 2) //�����ƶ�
	{
		SetColum(tank_now->colum);
		for(i = 0; i < 4; i++)
		{
			if(!(tank_now->colum/64) && i + tank_now->colum == 64)//����
			{
				SelectScreen(1);
				SetLine(tank_now->raw);//������Ӧ��
				SetColum(0);//������Ӧ��
			}
			if((!tank_now->colum && !i)) Write_data(0xff);//���ұ߿򲹳�
			else if(tank_now->raw == 7) Write_data(0x80);//�±߿򲹳�
			else if(!tank_now->raw) Write_data(0x01);//�ϱ߿򲹳�
			else Write_data(0x00);
		}
	}
	else
	{
		if(tank_now->colum-2 <= 0 && tank_now->direction == 1) SetColum(0);
		else tank_now->direction == 3?SetColum(tank_now->colum-2):SetColum(tank_now->colum);
		for(i = 0; i < 6; i++)
		{
			if(!(tank_now->colum/64) && i + tank_now->colum == 64)//����
			{
				SelectScreen(1);
				SetLine(tank_now->raw);//������Ӧ��
				SetColum(0);//������Ӧ��
			}
			//if(tank_now->colum + i - 2 <= 0 || tank_now->colum + i >= 96) continue;
			if(tank_now->colum + i >= 96) continue;
			if(!tank_now->colum && !i) Write_data(0xff);//���ұ߿򲹳�
			else if(tank_now->raw == 7) Write_data(0x80);//�±߿򲹳�
			else if(!tank_now->raw) Write_data(0x01);//�ϱ߿򲹳�
			else Write_data(0x00);
		}
	}	
}
void ShowTank(Tank tank_now)
{
	int i,j;
	if(!tank_now->life) return ;
	if(tank_now->bullet_num)//�ӵ���ʾ
	{
		for(i = 0; i < tank_now->bullet_num; i++)
		{
			SelectScreen((tank_now->bullet[i]).colum/64);
			SetLine((tank_now->bullet[i]).raw);
			SetColum((tank_now->bullet[i]).colum%64);
			for(j = 0; j < 4; j++)
			{
				Write_data(bullet[j]);
				delay(5);
			}
			delay(5);
		}
	}
	SelectScreen(tank_now->colum/64);//ѡ��̹��������Ļ
	SetLine(tank_now->raw);
	//length = tank_now->direction>2?6:4;
	if(tank_now->direction > 2) //�����ƶ�
	{
		SetColum(tank_now->colum);
		for(i = 0; i < 4; i++)
		{
			if(!(tank_now->colum/64) && i + tank_now->colum == 64)//����
			{
				SelectScreen(1);
				SetLine(tank_now->raw);//������Ӧ��
				SetColum(0);//������Ӧ��
			}
			if(!tank_now->colum && !i) Write_data(0xff);//���ұ߿򲹳�
			else if(tank_now->raw == 7) Write_data(tank_now->pos[i]|0x80);//�±߿򲹳�
			else if(!tank_now->raw) Write_data(tank_now->pos[i]|0x01);//�ϱ߿򲹳�
			//else if((!tank_now->colum && !i)|| (tank_now->colum == 92 && i == 3)) Write_data(0xff);//���ұ߿򲹳�
			else Write_data(tank_now->pos[i]);
			delay(5);
		}
	}
	else
	{
		if(tank_now->colum-2 <= 0 && tank_now->direction == 1) SetColum(1);
		else tank_now->direction == 3?SetColum(tank_now->colum-2):SetColum(tank_now->colum);
		for(i = 0; i < 6; i++)
		{
			if(!(tank_now->colum/64) && i + tank_now->colum == 64)//����
			{
				SelectScreen(1);
				SetLine(tank_now->raw);//������Ӧ��
				SetColum(0);//������Ӧ��
			}
			if((tank_now->colum + i - 2 <= 0 && tank_now == 3)|| tank_now->colum + i >= 96) continue;
			if(!tank_now->colum && !i) Write_data(0xff);//���ұ߿򲹳�
			else if(tank_now->raw == 7) Write_data(tank_now->pos[i] | 0x80);//�±߿򲹳�
			else if(!tank_now->raw) Write_data(tank_now->pos[i] | 0x01);//�ϱ߿򲹳�
			else Write_data(tank_now->pos[i]);
		}
	}
}

void move(uint direction,Tank tank_now)//1.�� 2.�� 3.�� 4.��
{
	DelTank(tank_now);
	if(tank_now->direction != direction)
	{
		CreateTank(direction,tank_now->life,tank_now->raw,tank_now->colum,tank_now->bullet_num,tank_now);
	}
	else 
	{
		switch(tank_now->direction)
		{
			case 1:if(tank_now->colum > 0 && WallCheck(Wall_Num,tank_now->raw,tank_now->colum-4)) tank_now->colum -= 4;break;//����
			case 2:if(tank_now->colum < 92 && WallCheck(Wall_Num,tank_now->raw,tank_now->colum+4)) tank_now->colum += 4;break;//����
			case 3:if(tank_now->raw > 0 && WallCheck(Wall_Num,tank_now->raw-1,tank_now->colum)) tank_now->raw -= 1;break;//����
			case 4:if(tank_now->raw < 7 && WallCheck(Wall_Num,tank_now->raw+1,tank_now->colum)) tank_now->raw += 1;break;//����
		}
	}
}

void DelBullet(Bullet biu)
{
	uint i;
	SelectScreen(biu.colum/64);
	SetLine(biu.raw);
	SetColum(biu.colum%64);
	for(i = 0; i < 4 ; i++)
	{
		if(!biu.colum && !i) Write_data(0xff);//���ұ߿򲹳�
		else if(biu.raw == 7) Write_data(0x80);//�±߿򲹳�
		else if(!biu.raw) Write_data(0x01);//�ϱ߿򲹳�
		else Write_data(0x00);
	}
}

Status TankLife(uchar raw,uchar colum,uint type)//�ӵ�ײ��̹��
{
	uint i;
	if(!type)
	{
		for(i = 0; i < 5; i++)
		{
			if(!ai[i].life) continue;//̹��������
			if(raw == ai[i].raw && colum == ai[i].colum) return ERROR;
		}
		return SUCCESS;
	}
	else
	{
		if(raw == own.raw && colum == own.colum) return ERROR;
		return SUCCESS;
	}
}
void MoveBullet(Tank tank_now,uint type)
{
	uint i,j;
	for(i = 0; i < tank_now->bullet_num; i++)
	{
		DelBullet(tank_now->bullet[i]);
		switch((tank_now->bullet[i]).direction)
		{
			case 1:{
			if((tank_now->bullet[i]).colum > 0 && WallCheck(Wall_Num,(tank_now->bullet[i]).raw,(tank_now->bullet[i]).colum-4) && TankLife((tank_now->bullet[i]).raw,(tank_now->bullet[i]).colum-4,type)) (tank_now->bullet[i]).colum -= 4;
			else 
			{
				Collision(Wall_Num,(tank_now->bullet[i]).raw,(tank_now->bullet[i]).colum-4,type);
				tank_now->bullet_num--;
				for(j = i; j < tank_now->bullet_num; j++)//�ӵ��ṹ���̲���
				{
					tank_now->bullet[j].raw = tank_now->bullet[j+1].raw ;
					tank_now->bullet[j].colum = tank_now->bullet[j+1].colum ;
					tank_now->bullet[j].direction = tank_now->bullet[j+1].direction ;
				}
			}
			break;}//����
			case 2:{
			if((tank_now->bullet[i]).colum < 92 && WallCheck(Wall_Num,(tank_now->bullet[i]).raw,(tank_now->bullet[i]).colum+4) && TankLife((tank_now->bullet[i]).raw,(tank_now->bullet[i]).colum+4,type)) (tank_now->bullet[i]).colum += 4;
			else 
			{
				Collision(Wall_Num,(tank_now->bullet[i]).raw,(tank_now->bullet[i]).colum+4,type);
		 		tank_now->bullet_num--;
				for(j = i; j < tank_now->bullet_num; j++)//�ӵ��ṹ���̲���
				{
					tank_now->bullet[j].raw = tank_now->bullet[j+1].raw ;
					tank_now->bullet[j].colum = tank_now->bullet[j+1].colum ;
					tank_now->bullet[j].direction = tank_now->bullet[j+1].direction ;
				}
			}
			break;}//����
			case 3:{
			if((tank_now->bullet[i]).raw > 0 && WallCheck(Wall_Num,(tank_now->bullet[i]).raw-1,(tank_now->bullet[i]).colum) && TankLife((tank_now->bullet[i]).raw-1,(tank_now->bullet[i]).colum,type)) (tank_now->bullet[i]).raw -= 1;
			else 
			{
				Collision(Wall_Num,(tank_now->bullet[i]).raw-1,(tank_now->bullet[i]).colum,type);
				tank_now->bullet_num--;
				for(j = i; j < tank_now->bullet_num; j++)//�ӵ��ṹ���̲���
				{
					tank_now->bullet[j].raw = tank_now->bullet[j+1].raw ;
					tank_now->bullet[j].colum = tank_now->bullet[j+1].colum ;
					tank_now->bullet[j].direction = tank_now->bullet[j+1].direction ;
				}
			}
			break;}//����
			case 4:{
			if((tank_now->bullet[i]).raw < 7 && WallCheck(Wall_Num,(tank_now->bullet[i]).raw+1,(tank_now->bullet[i]).colum) && TankLife((tank_now->bullet[i]).raw+1,(tank_now->bullet[i]).colum,type)) (tank_now->bullet[i]).raw += 1;
			else {
				Collision(Wall_Num,(tank_now->bullet[i]).raw+1,(tank_now->bullet[i]).colum,type);
				tank_now->bullet_num--;
				for(j = i; j < tank_now->bullet_num; j++)//�ӵ��ṹ���̲���
					{
						tank_now->bullet[j].raw = tank_now->bullet[j+1].raw ;
						tank_now->bullet[j].colum = tank_now->bullet[j+1].colum ;
						tank_now->bullet[j].direction = tank_now->bullet[j+1].direction ;
					}
				}
			break;}//����
		}
	}
}


void GameButton(Tank tank_now)
{
	switch(butt)
	{
		case 0:CreateBullet(tank_now);break;
		case 1:move(1,tank_now);break;//LEFT
		case 2:move(2,tank_now);break;//RIGHT
		case 3:move(3,tank_now);break;//UP
		case 4:move(4,tank_now);break;//DOWN
		case 5:{
		while(Receive() != 5);
		break;}//��ͣ
		default:break;
	}
	butt = 6;
}

void ShowWall(uint Wall_Num)//��ͼ��ʾ
{
	uint i,j;
	for(i = 0; i < Wall_Num; i++)
	{
		SelectScreen(map[i]/64);
		SetLine(map[i+Wall_Num]);
		SetColum(map[i]);
		for(j = 0; j < 4; j++)
		{
				Write_data(0xff);
		}
	}
}

void InitMap(uint leveltype)
{
	uint i;
	for(i = 0; i < Wall_Num; i++)
	{
		Walls_life[i] = *(map + i + Wall_Num*2);
	}
	switch(leveltype)
	{
		case 1:{
			CreateTank(4,1,0,0,0,&ai[0]);//���� ����ֵ1 ��0�� �ӵ�0 ��0��
			CreateTank(1,1,0,4*6,0,&ai[1]);//���� ����ֵ1 ��0�� �ӵ�0 ��24��
			CreateTank(2,1,0,4*18,0,&ai[2]);//���� ����ֵ1 ��0�� �ӵ�0 ��72��
			CreateTank(4,1,0,4*23,0,&ai[3]);//���� ����ֵ1 ��0�� �ӵ�0 ��92��
			CreateTank(4,1,2,4*5,0,&ai[4]);//���� ����ֵ1 ��0��  �ӵ�0 ��20��
			break;
		}
		case 2:{
			CreateTank(4,1,0,0,0,&ai[0]);//���� ����ֵ1 ��0�� �ӵ�0 ��0��
			CreateTank(4,1,3,4*7,0,&ai[1]);//���� ����ֵ1 ��3�� �ӵ�0 ��28��
			CreateTank(4,1,3,4*16,0,&ai[2]);//���� ����ֵ1 ��3�� �ӵ�0 ��64��
			CreateTank(4,1,0,4*23,0,&ai[3]);//���� ����ֵ1 ��0�� �ӵ�0 ��92��
			CreateTank(1,1,0,4*12,0,&ai[4]);//���� ����ֵ1 ��0��  �ӵ�0 ��12��
			break;		
		}
		case 3:{
			CreateTank(4,1,0,0,0,&ai[0]);//���� ����ֵ1 ��0�� �ӵ�0 ��0��
			CreateTank(4,1,3,4*5,0,&ai[1]);//���� ����ֵ1 ��3�� �ӵ�0 ��20��
			CreateTank(4,1,3,4*19,0,&ai[2]);//���� ����ֵ1 ��3�� �ӵ�0 ��76��
			CreateTank(4,1,0,4*23,0,&ai[3]);//���� ����ֵ1 ��0�� �ӵ�0 ��92��
			CreateTank(1,1,0,4*12,0,&ai[4]);//���� ����ֵ1 ��0��  �ӵ�0 ��12��
			break;				
		}
		case 4:{
			CreateTank(2,1,0,0,0,&ai[0]);//���� ����ֵ1 ��0�� �ӵ�0 ��0��
			CreateTank(4,1,1,4*5,0,&ai[1]);//���� ����ֵ1 ��1�� �ӵ�0 ��20��
			CreateTank(4,1,1,4*19,0,&ai[2]);//���� ����ֵ1 ��1�� �ӵ�0 ��76��
			CreateTank(1,1,0,4*23,0,&ai[3]);//���� ����ֵ1 ��0�� �ӵ�0 ��92��
			CreateTank(4,1,0,4*12,0,&ai[4]);//���� ����ֵ1 ��0��  �ӵ�0 ��12��
			break;			
		}
		case 5:{
			CreateTank(4,2,0,0,0,&ai[0]);//���� ����ֵ2 ��0�� �ӵ�0 ��0��
			CreateTank(2,2,0,4*5,0,&ai[1]);//���� ����ֵ2 ��3�� �ӵ�0 ��20��
			CreateTank(4,2,3,4*19,0,&ai[2]);//���� ����ֵ2 ��3�� �ӵ�0 ��76��
			CreateTank(4,2,0,4*8,0,&ai[3]);//���� ����ֵ2 ��0�� �ӵ�0 ��92��
			CreateTank(1,2,0,4*12,0,&ai[4]);//���� ����ֵ2 ��0��  �ӵ�0 ��12��
			break;			
		}
	}
}

void game()
{	
	uint i,leveltype,flag,Ai_direction,time;//leveltype.�ؿ�  flag.��Ե��� point.���� Wall_Num.ǽ������
	butt = 0;
	point = 0;
	switch(arrow[0]+arrow[1])
	{
		case 0:leveltype = 1;map = map1xy;Wall_Num = 45;InitMap(leveltype);ShowWall(45);break;//��һ��
		case 1:leveltype = 4;map = map4xy;Wall_Num = 49;InitMap(leveltype);ShowWall(49);break;//���Ĺ�
		case 2:leveltype = 2;map = map2xy;Wall_Num = 45;InitMap(leveltype);ShowWall(45);break;//�ڶ���
		case 3:leveltype = 5;map = map5xy;Wall_Num = 45;InitMap(leveltype);ShowWall(45);break;//�����
		case 4:leveltype = 3;map = map3xy;Wall_Num = 45;InitMap(leveltype);ShowWall(45);break;//������
	}
	CreateTank(3,5,7,4*10,0,&own);//���� ����ֵ5 ��7�� ��40��
	SelectScreen(0);
	SetLine(7);
	SetColum(4*12);
	for(i = 0; i <4; i++) Write_data(wall[i+4]);//����
	time = 150;
	message(leveltype,time);
	SelectScreen(1);
	for(i = 0; i < 8; i++)
	{
		SetLine(i);
		SetColum(32);
		Write_data(0xff);
	}//���Ʒֽ���
	for(i = 0; i < 4; i++) ShowTank(&ai[i]);
	//TMOD = 0x21;//���ù�����ʽ
	TH0 = 15536/256;//��ȡ�߰�λ
	TL0 = 15536%256;//��ȡ�Ͱ�λ��һ���ж�Ϊ0.05s
	EA = 1;//�����ж�
	ET0 = 1;//����ʱ��0
	TR0 = 1;//������ʱ��0
	while(own.life)
	{	
		if(CheckWin())
		{
			Send(leveltype*time);
			leveltype += 1 ;
			SelectScreen(0);
			Show816(3,40,num+16*16);
			Show816(3,48,num+16*17);
			Show816(3,56,num+16*18);
			delay(1000);
			ClearScreen(2);
			Border();
			switch(leveltype)
			{
				case 2:map = map2xy;Wall_Num = 45;InitMap(leveltype);ShowWall(45);break;//�ڶ���
				case 3:map = map3xy;Wall_Num = 45;InitMap(leveltype);ShowWall(45);break;//������
				case 4:map = map4xy;Wall_Num = 49;InitMap(leveltype);ShowWall(49);break;//���Ĺ�
				case 5:map = map5xy;Wall_Num = 45;InitMap(leveltype);ShowWall(45);break;//�����
				default:{
					LevelChoice();
					return ;
				}
			}
			CreateTank(3,5,7,4*10,0,&own);//���� ����ֵ5 ��7�� ��40��
			SelectScreen(0);
			SetLine(7);
			SetColum(4*12);
			for(i = 0; i <4; i++) Write_data(wall[i+4]);
			time = 150;
			message(leveltype,time);
			SelectScreen(1);
			for(i = 0; i < 8; i++)
			{
				SetLine(i);
				SetColum(32);
				Write_data(0xff);
			}//���Ʒֽ���
			for(i = 0; i < 4; i++) ShowTank(&ai[i]);
		}
		SelectScreen(1);
		Show816(4,32,num+16*14);
		Show816(4,32+8*1,num+16*11);
		Show816(4,32+8*2,num+16*own.life);//����ֵ
		ShowTank(&own);
		for(i = 0; i < 5; i++) ShowTank(&ai[i]);
		if(t1>=10)
		{
			MoveBullet(&own,0);
			for(i = 0; i < 5; i++)
			{
				if(ai[i].life) MoveBullet((ai+i),1);
			}
			t1 = 0;
		}
		if(t >= 20) 
		{
				time--;
				for(i = 0; i < 5; i++) 
				{
					flag = 1;
					Ai_direction = ai[i].direction;
					while(1)//�ж��Ƿ�����ǽ��
					{
						switch(Ai_direction)
						{
							case 1:if(ai[i].colum && WallCheck(Wall_Num,ai[i].raw,ai[i].colum-4)) flag = 0;break;//��
							case 2:if(ai[i].colum != 92 && WallCheck(Wall_Num,ai[i].raw,ai[i].colum+4)) flag = 0;break;//��
							case 3:if(ai[i].raw && WallCheck(Wall_Num,ai[i].raw-1,ai[i].colum)) flag = 0;break;//��
							case 4:if(ai[i].raw != 7 && WallCheck(Wall_Num,ai[i].raw+1,ai[i].colum)) flag = 0;break;//��
						}
						if(!flag) break;//ֱ��Ϊ��ײ����Ե�Լ�ǽ��
						Ai_direction = rand()%4 + 1;
					}
					if(ai[i].bullet_num!=1) 
					{
						CreateBullet(ai+i);
						MoveBullet((ai+i),1);
					}
					move(Ai_direction,&ai[i]);
				}
				message(leveltype,time);	
				t = 0;
		}
		if(CheckDefeat() || !time ) break;
		ShowTank(&own);
		GameButton(&own);
	}
	SelectScreen(0);
	Show816(3,32,num+19*16);//D
	Show816(3,40,num+20*16);//E
	Show816(3,48,num+21*16);//F
	Show816(3,56,num+22*16);//E
	SelectScreen(1);
	Show816(3,0,num+23*16);//A
	Show816(3,8,num+24*16);//T
	delay(1000);
	ClearScreen(2);
	arrow[0] = arrow[1] = 0;
	LevelChoice();
}

void T0_time() interrupt 1
{
	uint temp;
	TH0 = 15536/256;//��ȡ�߰�λ
	TL0 = 15536%256;//��ȡ�Ͱ�λ��һ���ж�Ϊ0.05s
	t += 1;
	t1 += 1;
	if(!(t%2))
	{
		temp = Receive();
		if(temp!=6) butt = temp;
	}
}

