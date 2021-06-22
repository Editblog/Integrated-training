#include "dht11.h"
#include "OnBoard.h"
#define DATA  P0_4

unsigned char temperature=0,humidity=0;
void DATA_IO_input_Cfg(void)//����Ϊ��������ģʽ
{
    P0SEL &= 0xEF;   // 1110 1111 ��P04����Ϊ��ͨIOģʽ
    P0DIR &= 0xEF;   // 1110 1111 ��P04����Ϊ����ģʽ
    P0INP &= 0xEF;   // 1110 1111 ��P04����Ϊ������ģʽ
    P2INP &= 0xDF;   // 1101 1111 ��P0������Ϊ����ģʽ
}

void DATA_IO_output_Cfg(void)//����Ϊ���ģʽ
{
    P0SEL &= 0xEF;   // 1110 1111 ��P04����Ϊ��ͨIOģʽ
    P0DIR |= 0x10;   // 0001 0000 ��P04����Ϊ���ģʽ
//    P0INP |= 0x10;   // 0001 0000 ��P04����Ϊ��̬ģʽ
}

unsigned char Read_Byte(void)
{
  unsigned char tmp=0,i;
  for(i=0;i<8;i++)
  {
    while(!(DATA ==0));//ÿ��bit���Ǵ�50us�ĵ͵�ƽ��ʼ
    while(!(DATA ==1));
    MicroWait(30);//��ʱ30us
    if(DATA == 1)
    {
      tmp |= 1<<(7-i);//�Ѹ�bit����Ϊ1
    }
  }
  return tmp;
}

/*
@Start_DHT11 : �ɼ�һ����ʪ������
@retval : ����ɹ�������ʪ�ȵ�ֵ���浽ȫ�ֱ�����������1
          ���ʧ�ܣ�����0
*/
int Start_DHT11(void)//��ʼ�ɼ�����
{
  unsigned char buf[5];
  DATA_IO_output_Cfg();//����Ϊ���ģʽ
  DATA = 0;//���Ϳ�ʼ�ź�
  MicroWait(20000);//��ʱ����18ms
  DATA = 1;//����20~40us
  MicroWait(40);
  
  DATA_IO_input_Cfg();//����Ϊ����ģʽ
  while(!(DATA ==0));//��DHT11����Ӧ�ź�
  
  while(!(DATA ==1));//��DHT11���������ߣ�׼����������
  buf[0] = Read_Byte();
  buf[1] = Read_Byte();
  buf[2] = Read_Byte();
  buf[3] = Read_Byte();
  buf[4] = Read_Byte();

  
  if((buf[0]+buf[1]+buf[2]+buf[3])%256 == buf[4])
  {
    humidity = buf[0];
    temperature = buf[2];
    return 1;
  }
  return 0;
}


