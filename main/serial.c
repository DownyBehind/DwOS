//------------------------------------------------------------------------------
// 화일명 : serial.c
// 설  명 : ezBoot의 시리얼 제어 루틴이다. 
// 
// 작성자 : 유영창 (주)제이닷디엔티 frog@falinux.com
// 작성일 : 2003년 05월 20일
// 수  정 : 2003-06-07	오재경 (주)제이닷디엔티 freefrug@falinux.com  -- 푸지
//          
// 저작권 : (주)제이닷디엔티 
// 주  의 : 이 루틴의 대부분은 blob에서 가져왔다. 
//------------------------------------------------------------------------------

//******************************************************************************
//
// 헤더 정의
//
//******************************************************************************
#include <pxa255.h>
//#include <config.h> //@dw.kim 21.02.07
#include <serial.h>
#include <time.h>
#include <stdio.h>
#include <gpio.h>

// 헤더 화일 pxa255.h 에 있는 __REG 매크로를 일반 값으로 변경 시킨다. 
#define __REG(x)	              (x) 

//******************************************************************************
//
// 광역 변수 정의
//
//******************************************************************************
// config.c 에 정의
//extern TConfig Cfg; //@dw.kim 21.02.07

static int SerialErrorFlag = 0;

#ifdef IN_GUMSTIX
static volatile Word *UART = (volatile Word *) FFUART; //@dw.kim 21.02.07
#else
static volatile Word *UART = (volatile Word *) STUART; // UART 베이스 어드레스 
#endif

#define UART_DATA          	((volatile Word) UART[0] )  // 데이타  
#define UART_IER		((volatile Word) UART[1] )  // 인터럽트 허가 
#define UART_FCR		((volatile Word) UART[2] )  // 인터럽트 상태 
#define UART_LCR		((volatile Word) UART[3] )  // 라인 콘트롤 
#define UART_MCR		((volatile Word) UART[4] )  // 모뎀 콘트롤 
#define UART_LSR		((volatile Word) UART[5] )  // 라인 상태 
#define UART_MSR		((volatile Word) UART[6] )  // 모뎀 상태 
#define UART_SPR		((volatile Word) UART[7] )  // 
#define UART_ISR		((volatile Word) UART[8] )  // 적외선 포트 선택 
                                                
#define UART_DLL		((volatile Word) UART[0] )  // UART 라인 상태 
#define UART_DLH           	((volatile Word) UART[1] )  // UART 라인 상태 

//******************************************************************************
//
// 함수 정의
//
//******************************************************************************
//------------------------------------------------------------------------------
// 설명 : 시리얼 디바이스를 초기화 한다. 
// 매계 : 없음 
// 반환 : 없음 
// 주의 : 없음 
//------------------------------------------------------------------------------
// void SerialInit( eBauds baudrate ) //@dw.kim 21.02.07
// {
// 	Word Temp;
	
// 	volatile Word *ClockEnableRegister = (volatile Word *)CKEN; 
	
// 	// 포트를 선택한다. 
//     	switch( Cfg.SeriaNumber )
//     	{

//     	case 0:	UART = (volatile Word *) FFUART; 	// 레지스터 베이스 포인터를 변경한다.

//         	*ClockEnableRegister |= CKEN_FFUART;
// 		set_GPIO_mode( GPIO34_FFRXD_MD );
// 		set_GPIO_mode( GPIO39_FFTXD_MD );
//               	break;

//     	case 1:	UART = (volatile Word *) BTUART; 	// 레지스터 베이스 포인터를 변경한다.

//               	*ClockEnableRegister |= CKEN_BTUART;
// 		set_GPIO_mode( GPIO42_BTRXD_MD );
// 		set_GPIO_mode( GPIO43_BTTXD_MD );
//               	break;

//     	default:UART = (volatile Word *) STUART; 	// 레지스터 베이스 포인터를 변경한다.

//               	*ClockEnableRegister |= CKEN_STUART;
// 		set_GPIO_mode( GPIO46_STRXD_MD );
// 		set_GPIO_mode( GPIO47_STTXD_MD );
// 		break;
//     	}
    
// 	// 모든 인터럽트를 금지 시킨다. 
// 	UART_IER = 0;
	
// 	// FIFO 를 정리한다. 
// 	UART_FCR = 7;					
// 	UART_FCR = 1;					
	
// 	// baud, 8BIT 1STOP NO PARITY 로 설정한다.
// 	UART_LCR = (LCR_WLS1|LCR_WLS0|LCR_DLAB);
	
//     	UART_DLL = baudrate;
//     	UART_DLH = 0x00;
    
//     	UART_LCR = (LCR_WLS1|LCR_WLS0); 

//     	// UART를 동작 시킨다. 
//     	UART_IER = IER_UUE;
  
// }
//------------------------------------------------------------------------------
// 설명 : 시리얼 디바이스에 한 문자를 보낸다. 
// 매계 : 없음 
// 반환 : 없음 
// 주의 : 없음 
//------------------------------------------------------------------------------
void SerialOutChar( const char c  )
{
	// 전송 가능할때까지 기다린다. 
	while(( UART_LSR & LSR_TDRQ ) == 0 ); 

	// 시리얼에 데이타를 출력한다. 
	UART_DATA = c;
}

//------------------------------------------------------------------------------
// 설명 : 시리얼 디바이스에 한 문자를 보낸다. 
// 매계 : 없음 
// 반환 : 없음 
// 주의 : CR 의 LR CR 로 변경하여 출력한다. 
//------------------------------------------------------------------------------
void SerialOutChar_CheckCR( const char c  )
{
	SerialOutChar( c );
	if(c == '\n') SerialOutChar('\r');
}
//------------------------------------------------------------------------------
// 설명 : 시리얼 디바이스에 문자열을 보낸다. 
// 매계 : 없음 
// 반환 : 없음 
// 주의 : 없음 
//------------------------------------------------------------------------------
int SerialOutStr( char *str, int size )
{
	int	lp ;

	for (lp=0;lp<size;lp++) SerialOutChar(str[lp]) ;

	return lp;
}
//------------------------------------------------------------------------------
// 설명 : 시리얼 디바이스에 문자열을 보낸다. 
// 매계 : 없음 
// 반환 : 없음 
// 주의 : 없음 
//------------------------------------------------------------------------------
int SerialOutStr_CheckCR( char *str, int size )
{
	int	lp ;

	for (lp=0;lp<size;lp++) SerialOutChar_CheckCR(str[lp]) ;

	return lp;
}
//------------------------------------------------------------------------------
// 설명 : 시리얼 디바이스에서 수신된 데이타가 있는지 확인한다. 
// 매계 : 없음 
// 반환 : 수신된 데이타가 있으면 1 / 없으면 0
// 주의 : 없음 
//------------------------------------------------------------------------------
int SerialIsReadyChar( void )
{
	// 수신된 데이타가 있는가를 확인한다. 
	if( UART_LSR & LSR_DR ) return 1;
	return 0;
}

//------------------------------------------------------------------------------
// 설명 : 시리얼 상태를 돌려 준다. 
// 매계 : 없음 
// 반환 : 수신된 데이타가 있으면 1 / 없으면 0
// 주의 : 없음 
//------------------------------------------------------------------------------
int SeriallGet_LSR( void )
{
	return UART_LSR;
}
//------------------------------------------------------------------------------
// 설명 : 시리얼 디바이스에서 에러 상태를 읽어 온다. 
// 매계 : 없음 
// 반환 : 수신된 문자 
// 주의 : 없음 
//------------------------------------------------------------------------------
char SerialIsGetChar( void )
{
	// 에러를 가져 온다. 
    	SerialErrorFlag = UART_LSR & (LSR_PE | LSR_FE | LSR_OE);
    
	// 수신된 데이타를 가져 온다. 
	return (char) UART_DATA;
}
//------------------------------------------------------------------------------
// 설명 : 시리얼 디바이스의 수신 에러 플라그를 클리어 한다. 
// 매계 : 없음 
// 반환 : 수신된 문자 
// 주의 : 없음 
//------------------------------------------------------------------------------
char SerialIsClearError( void )
{
	SerialErrorFlag = 0;
	return (char)SerialErrorFlag;
}
//------------------------------------------------------------------------------
// 설명 : 시리얼 디바이스의 수신 에러 플라그 값을 얻는다. 
// 매계 : 없음 
// 반환 : 수신된 문자 
// 주의 : 없음 
//------------------------------------------------------------------------------
int SerialIsGetError( void )
{
	return SerialErrorFlag;
}


