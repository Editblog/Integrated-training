/**************************************************************************************************
  Filename:       GenericApp.c
  Revised:        $Date: 2009-03-18 15:56:27 -0700 (Wed, 18 Mar 2009) $
  Revision:       $Revision: 19453 $

  Description:    Generic Application (no Profile).


  Copyright 2004-2009 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, 
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com. 
**************************************************************************************************/

/*********************************************************************
  This application isn't intended to do anything useful, it is
  intended to be a simple example of an application's structure.

  This application sends "Hello World" to another "Generic"
  application every 15 seconds.  The application will also
  receive "Hello World" packets.

  The "Hello World" messages are sent/received as MSG type message.

  This applications doesn't have a profile, so it handles everything
  directly - itself.

  Key control:
    SW1:
    SW2:  initiates end device binding
    SW3:
    SW4:  initiates a match description request
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "GenericApp.h"
#include "DebugTrace.h"

#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"

#include "led_key.h"
#include "string.h"
#include "dht11.h"
#include "stdio.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// This list should be filled with Application specific Cluster IDs.
const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] =
{
  GENERICAPP_CLUSTERID
};

const SimpleDescriptionFormat_t GenericApp_SimpleDesc =
{
  GENERICAPP_ENDPOINT,              //  int Endpoint;
  GENERICAPP_PROFID,                //  uint16 AppProfId[2];
  GENERICAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  GENERICAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  GENERICAPP_FLAGS,                 //  int   AppFlags:4;
  GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)GenericApp_ClusterList,  //  byte *pAppInClusterList;
  GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)GenericApp_ClusterList   //  byte *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in GenericApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t GenericApp_epDesc;//ϵͳ�Ծ�����õĶ˵��������ṹ�����

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
byte GenericApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // GenericApp_Init() is called.
devStates_t GenericApp_NwkState;


byte GenericApp_TransID;  // This is the unique message ID (counter)

afAddrType_t GenericApp_DstAddr;//ϵͳ�Ѷ���������ַ�ṹ�����

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void GenericApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
void GenericApp_HandleKeys( byte shift, byte keys );
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void GenericApp_SendTheMessage( void );

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      GenericApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */

//���ڽ������ݺ�ᴥ���������
void SerialApp_CallBack(uint8 port, uint8 event)   //�������ݻᴥ��
{
    unsigned char buf[32]={0};
    HalUARTRead(HAL_UART_PORT_0,buf,32);
    AF_DataRequest( &GenericApp_DstAddr, &GenericApp_epDesc,//�����ַ���˵�������
                       GENERICAPP_CLUSTERID,
                       (byte)osal_strlen( buf ) + 1,
                       (byte *)buf,
                       &GenericApp_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS );
//    if(strcmp((char*)buf,"temperature1") == 0)
//    {
        
//    }
//    else if(strcmp((char*)buf,"humidity1") == 0)
//    {
//        
//    }

}

//���ڳ�ʼ������
static void InitUart(void)
{
	halUARTCfg_t   uartConfig;
	uartConfig.configured           = TRUE;  
	uartConfig.baudRate             = HAL_UART_BR_9600; //���ò�����
	uartConfig.flowControl          = FALSE;
	uartConfig.flowControlThreshold = 64; 
	uartConfig.rx.maxBufSize        = 128;  
	uartConfig.tx.maxBufSize        = 128;  
	uartConfig.idleTimeout          = 6;    
	uartConfig.intEnable            = TRUE;                
	uartConfig.callBackFunc         = SerialApp_CallBack;  //���ûص����� 
	HalUARTOpen (HAL_UART_PORT_0, &uartConfig);   
}


void GenericApp_Init( byte task_id )
{
  GenericApp_TaskID = task_id;
  GenericApp_NwkState = DEV_INIT;
  GenericApp_TransID = 0;

  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

  InitUart();//���ô��ڵĳ�ʼ������
  GenericApp_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;//����ͨ��ģʽ
  GenericApp_DstAddr.endPoint = GENERICAPP_ENDPOINT;//���ö˵�
  GenericApp_DstAddr.addr.shortAddr = 0;    //Э�����ĵ�ַ����0

  // Fill out the endpoint description.
  GenericApp_epDesc.endPoint = GENERICAPP_ENDPOINT;
  GenericApp_epDesc.task_id = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &GenericApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( GenericApp_TaskID );

  // Update the display
#if defined ( LCD_SUPPORTED )
    HalLcdWriteString( "GenericApp", HAL_LCD_LINE_1 );
#endif
    
  ZDO_RegisterForZDOMsg( GenericApp_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( GenericApp_TaskID, Match_Desc_rsp );
}

/*********************************************************************
 * @fn      GenericApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  afDataConfirm_t *afDataConfirm;

  // Data Confirmation message fields
  byte sentEP;
  ZStatus_t sentStatus;
  byte sentTransID;       // This should match the value sent
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG:
          GenericApp_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;
          
        case KEY_CHANGE:
          GenericApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        case AF_DATA_CONFIRM_CMD:
          // This message is received as a confirmation of a data packet sent.
          // The status is of ZStatus_t type [defined in ZComDef.h]
          // The message fields are defined in AF.h
          afDataConfirm = (afDataConfirm_t *)MSGpkt;
          sentEP = afDataConfirm->endpoint;
          sentStatus = afDataConfirm->hdr.status;
          sentTransID = afDataConfirm->transID;
          (void)sentEP;
          (void)sentTransID;

          // Action taken when confirmation is received.
          if ( sentStatus != ZSuccess )
          {
            // The data wasn't delivered -- Do something
          }
          break;

        case AF_INCOMING_MSG_CMD:
          GenericApp_MessageMSGCB( MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
          GenericApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if(GenericApp_NwkState == DEV_ZB_COORD)//����״̬���Э����
          {
            led_ctrl(LED0,LED_ON);
            led_ctrl(LED1,LED_ON);
//              osal_set_event(GenericApp_TaskID,GENERICAPP_MY_EVT1);//��������¼�
//              osal_start_timerEx(GenericApp_TaskID,GENERICAPP_MY_EVT2,1000);
          }
          else if(GenericApp_NwkState == DEV_ROUTER)//����״̬���·����
          {
            led_ctrl(LED0,LED_OFF);
            led_ctrl(LED1,LED_ON);
          }
          else if(GenericApp_NwkState == DEV_END_DEVICE)//����״̬����ն�
          {
            led_ctrl(LED0,LED_ON);
            led_ctrl(LED1,LED_OFF);
            unsigned char str[100];
            Start_DHT11();
            sprintf(str,"temperature:%d,humidity:%d\r\n",temperature,humidity);
            HalUARTWrite(HAL_UART_PORT_0,str,strlen((char *)str));//���͵��ն˵Ĵ���
            osal_start_timerEx(GenericApp_TaskID,GENERICAPP_MY_EVT1,2000);
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in GenericApp_Init()).
  if ( events & GENERICAPP_SEND_MSG_EVT )
  {
    // Send "the" message
    GenericApp_SendTheMessage();

    // Setup to send message again
    osal_start_timerEx( GenericApp_TaskID,
                        GENERICAPP_SEND_MSG_EVT,
                      GENERICAPP_SEND_MSG_TIMEOUT );

    // return unprocessed events
    return (events ^ GENERICAPP_SEND_MSG_EVT);
  }
  
  if ( events & GENERICAPP_MY_EVT1 )
  {//�¼�Ҫ������
    //P1_0 = !P1_0;
    //P1_1 = !P1_1;
    unsigned char str[100];
    
    if(GenericApp_NwkState == DEV_END_DEVICE)//�������״̬���ն�,�ն˷�����
    {
      if(Start_DHT11() == 1)
        {
          static char beep_flag=0,huoer_flag=0,R_flag=0,G_flag=0,B_flag=0,warn_flag=0,relay_flag=0;
          //�ж�һ������״̬
          if(P1_4 == 1)
          {
            warn_flag = 0;
          }
          else
          {
            warn_flag = 1;
          }
          //�����趨�¶ȷ���������
          if(temperature > 28 && warn_flag)
          {
              P0_6 = 0;
              beep_flag=1;
          }
          else
          {
              P0_6 = 1;
              beep_flag=0;
          }
          //�����趨ʪ�ȣ��̵�����
          if(humidity < 50 && warn_flag)
          {
              P0_7 = 0;
              relay_flag = 1;
          }
          else
          {
              P0_7 = 1;
              relay_flag = 0;
          }
          //�жϻ���Ԫ��״̬
          if(P0_5 == 0)
          {
            huoer_flag=1;
          }
          else
          {
            huoer_flag=0;
          }
          //�ж�R��״̬
          if(P1_2 == 0)
          {
            R_flag = 0;
          }
          else
          {
            R_flag = 1;
          }
          //�ж�G��״̬
          if(P1_3 == 0)
          {
            G_flag = 0;
          }
          else
          {
            G_flag = 1;
          }
          //�ж�B��״̬
          if(P1_7 == 0)
          {
            B_flag = 0;
          }
          else
          {
            B_flag = 1;
          }
          
          sprintf(str,"{\"temperature\":%d,\"humidity\":%d,\"beep\":%d,\"relay\":%d,\"reverse\":%d,\"led1\":%d,\"led2\":%d,\"led3\":%d,\"warn\":%d}",temperature,humidity,beep_flag,relay_flag,huoer_flag,R_flag,G_flag,B_flag,warn_flag);
          HalUARTWrite(HAL_UART_PORT_0,str,strlen((char *)str));//���͵��ն˵Ĵ���
        }
        else
        {
            HalUARTWrite(HAL_UART_PORT_0,"error\r\n",strlen("error\r\n"));
        }
      //���͸�Э����
      AF_DataRequest( &GenericApp_DstAddr, &GenericApp_epDesc,//�����ַ���˵�������
                       GENERICAPP_CLUSTERID,
                       (byte)osal_strlen( str ) + 1,
                       (byte *)str,
                       &GenericApp_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS );
    //HalUARTWrite(HAL_UART_PORT_0,str,strlen((char *)str));   
      osal_start_timerEx(GenericApp_TaskID,GENERICAPP_MY_EVT1,2000);
    }
    
    return (events ^ GENERICAPP_MY_EVT1);
  } 
  
  if ( events & GENERICAPP_MY_EVT2 )
  {//�¼�Ҫ������
    
    P0_7 = 0;//�̵�����
    return (events ^ GENERICAPP_MY_EVT2);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */

/*********************************************************************
 * @fn      GenericApp_ProcessZDOMsgs()
 *
 * @brief   Process response messages
 *
 * @param   none
 *
 * @return  none
 */
void GenericApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
{
  switch ( inMsg->clusterID )
  {
    case End_Device_Bind_rsp:
      if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess )
      {
        // Light LED
        HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
      }
#if defined(BLINK_LEDS)
      else
      {
        // Flash LED to show failure
        HalLedSet ( HAL_LED_4, HAL_LED_MODE_FLASH );
      }
#endif
      break;

    case Match_Desc_rsp:
      {
        ZDO_ActiveEndpointRsp_t *pRsp = ZDO_ParseEPListRsp( inMsg );
        if ( pRsp )
        {
          if ( pRsp->status == ZSuccess && pRsp->cnt )
          {
            GenericApp_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            GenericApp_DstAddr.addr.shortAddr = pRsp->nwkAddr;
            // Take the first endpoint, Can be changed to search through endpoints
            GenericApp_DstAddr.endPoint = pRsp->epList[0];

            // Light LED
            HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
          }
          osal_mem_free( pRsp );
        }
      }
      break;
  }
}

/*********************************************************************
 * @fn      GenericApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_4
 *                 HAL_KEY_SW_3
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void GenericApp_HandleKeys( byte shift, byte keys )
{
  zAddrType_t dstAddr;
  
  // Shift is used to make each button/switch dual purpose.
  if ( shift )
  {
    if ( keys & HAL_KEY_SW_1 )
    {
    }
    if ( keys & HAL_KEY_SW_2 )
    {
    }
    if ( keys & HAL_KEY_SW_3 )
    {
    }
    if ( keys & HAL_KEY_SW_4 )
    {
    }
  }
  else
  {
    if ( keys & HAL_KEY_SW_1 )
    {
    }

    if ( keys & HAL_KEY_SW_2 )
    {
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );

      // Initiate an End Device Bind Request for the mandatory endpoint
      dstAddr.addrMode = Addr16Bit;
      dstAddr.addr.shortAddr = 0x0000; // Coordinator
      ZDP_EndDeviceBindReq( &dstAddr, NLME_GetShortAddr(), 
                            GenericApp_epDesc.endPoint,
                            GENERICAPP_PROFID,
                            GENERICAPP_MAX_CLUSTERS, (cId_t *)GenericApp_ClusterList,
                            GENERICAPP_MAX_CLUSTERS, (cId_t *)GenericApp_ClusterList,
                            FALSE );
    }

    if ( keys & HAL_KEY_SW_3 )
    {
    }

    if ( keys & HAL_KEY_SW_4 )
    {
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );
      // Initiate a Match Description Request (Service Discovery)
      dstAddr.addrMode = AddrBroadcast;
      dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;
      ZDP_MatchDescReq( &dstAddr, NWK_BROADCAST_SHORTADDR,
                        GENERICAPP_PROFID,
                        GENERICAPP_MAX_CLUSTERS, (cId_t *)GenericApp_ClusterList,
                        GENERICAPP_MAX_CLUSTERS, (cId_t *)GenericApp_ClusterList,
                        FALSE );
    }
  }
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      GenericApp_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
//���������յ���Ϣ������
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )//������Ϣ�ص�����
{
  switch ( pkt->clusterId )
  {
    case GENERICAPP_CLUSTERID:
        //pkt->cmd.Data  ���ǽ��յ������ݵ��׵�ַ
        if(GenericApp_NwkState == DEV_ZB_COORD)//����״̬��Э����
        {
            HalUARTWrite(HAL_UART_PORT_0,pkt->cmd.Data,strlen((char *)(pkt->cmd.Data)));
        }
        else if(GenericApp_NwkState == DEV_END_DEVICE)  //����״̬���ն�
        {
            HalUARTWrite(HAL_UART_PORT_0,pkt->cmd.Data,strlen((char *)(pkt->cmd.Data)));
        }
        
        
        break;
  }
}

/*********************************************************************
 * @fn      GenericApp_SendTheMessage
 *
 * @brief   Send "the" message.
 *
 * @param   none
 *
 * @return  none
 */
void GenericApp_SendTheMessage( void )
{
  char theMessageData[] = "Hello World";

  if ( AF_DataRequest( &GenericApp_DstAddr, &GenericApp_epDesc,
                       GENERICAPP_CLUSTERID,
                       (byte)osal_strlen( theMessageData ) + 1,
                       (byte *)theMessageData,
                       &GenericApp_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
    // Successfully requested to be sent.
  }
  else
  {
    // Error occurred in request to send.
  }
}

/*********************************************************************
*********************************************************************/

#pragma optimize=none       //�����б����Ż�
#pragma vector=P0INT_VECTOR //P1�������ж�,vector�ж�ʸ����,������δ�������������ط�
__interrupt void exint_s1(void)//P1��
{
      if(P0IFG & 0X02)//0000 0100
      {//ȷ����P1_2�������ⲿ�ж�
          osal_start_timerEx(GenericApp_TaskID,GENERICAPP_MY_EVT2,100);
      }
      P0IFG = 0;//���ж�״̬��־λ��0
      P0IF = 0; //���жϱ�־λ��0
}


#pragma optimize=none       //�����б����Ż�
#pragma vector=P2INT_VECTOR //P1�������ж�,vector�ж�ʸ����,������δ�������������ط�
__interrupt void exint_s2(void)//P1��
{
      if(P2IFG & 0X01)//0000 0100
      {//ȷ����P1_2�������ⲿ�ж�
          osal_start_timerEx(GenericApp_TaskID,GENERICAPP_MY_EVT2,10);
      }
      P2IFG = 0;//���ж�״̬��־λ��0
      P2IF = 0; //���жϱ�־λ��0
}
