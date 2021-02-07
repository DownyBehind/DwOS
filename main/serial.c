//------------------------------------------------------------------------------
// ȭ�ϸ� : serial.c
// ��  �� : ezBoot�� �ø��� ���� ��ƾ�̴�. 
// 
// �ۼ��� : ����â (��)���̴��Ƽ frog@falinux.com
// �ۼ��� : 2003�� 05�� 20��
// ��  �� : 2003-06-07	����� (��)���̴��Ƽ freefrug@falinux.com  -- Ǫ��
//          
// ���۱� : (��)���̴��Ƽ 
// ��  �� : �� ��ƾ�� ��κ��� blob���� �����Դ�. 
//------------------------------------------------------------------------------

//******************************************************************************
//
// ��� ����
//
//******************************************************************************
#include <pxa255.h>
//#include <config.h> //@dw.kim 21.02.07
#include <serial.h>
#include <time.h>
#include <stdio.h>
#include <gpio.h>

// ��� ȭ�� pxa255.h �� �ִ� __REG ��ũ�θ� �Ϲ� ������ ���� ��Ų��. 
#define __REG(x)	              (x) 

//******************************************************************************
//
// ���� ���� ����
//
//******************************************************************************
// config.c �� ����
//extern TConfig Cfg; //@dw.kim 21.02.07

static int SerialErrorFlag = 0;

#ifdef IN_GUMSTIX
static volatile Word *UART = (volatile Word *) FFUART; //@dw.kim 21.02.07
#else
static volatile Word *UART = (volatile Word *) STUART; // UART ���̽� ��巹�� 
#endif

#define UART_DATA          	((volatile Word) UART[0] )  // ����Ÿ  
#define UART_IER		((volatile Word) UART[1] )  // ���ͷ�Ʈ �㰡 
#define UART_FCR		((volatile Word) UART[2] )  // ���ͷ�Ʈ ���� 
#define UART_LCR		((volatile Word) UART[3] )  // ���� ��Ʈ�� 
#define UART_MCR		((volatile Word) UART[4] )  // �� ��Ʈ�� 
#define UART_LSR		((volatile Word) UART[5] )  // ���� ���� 
#define UART_MSR		((volatile Word) UART[6] )  // �� ���� 
#define UART_SPR		((volatile Word) UART[7] )  // 
#define UART_ISR		((volatile Word) UART[8] )  // ���ܼ� ��Ʈ ���� 
                                                
#define UART_DLL		((volatile Word) UART[0] )  // UART ���� ���� 
#define UART_DLH           	((volatile Word) UART[1] )  // UART ���� ���� 

//******************************************************************************
//
// �Լ� ����
//
//******************************************************************************
//------------------------------------------------------------------------------
// ���� : �ø��� ����̽��� �ʱ�ȭ �Ѵ�. 
// �Ű� : ���� 
// ��ȯ : ���� 
// ���� : ���� 
//------------------------------------------------------------------------------
// void SerialInit( eBauds baudrate ) //@dw.kim 21.02.07
// {
// 	Word Temp;
	
// 	volatile Word *ClockEnableRegister = (volatile Word *)CKEN; 
	
// 	// ��Ʈ�� �����Ѵ�. 
//     	switch( Cfg.SeriaNumber )
//     	{

//     	case 0:	UART = (volatile Word *) FFUART; 	// �������� ���̽� �����͸� �����Ѵ�.

//         	*ClockEnableRegister |= CKEN_FFUART;
// 		set_GPIO_mode( GPIO34_FFRXD_MD );
// 		set_GPIO_mode( GPIO39_FFTXD_MD );
//               	break;

//     	case 1:	UART = (volatile Word *) BTUART; 	// �������� ���̽� �����͸� �����Ѵ�.

//               	*ClockEnableRegister |= CKEN_BTUART;
// 		set_GPIO_mode( GPIO42_BTRXD_MD );
// 		set_GPIO_mode( GPIO43_BTTXD_MD );
//               	break;

//     	default:UART = (volatile Word *) STUART; 	// �������� ���̽� �����͸� �����Ѵ�.

//               	*ClockEnableRegister |= CKEN_STUART;
// 		set_GPIO_mode( GPIO46_STRXD_MD );
// 		set_GPIO_mode( GPIO47_STTXD_MD );
// 		break;
//     	}
    
// 	// ��� ���ͷ�Ʈ�� ���� ��Ų��. 
// 	UART_IER = 0;
	
// 	// FIFO �� �����Ѵ�. 
// 	UART_FCR = 7;					
// 	UART_FCR = 1;					
	
// 	// baud, 8BIT 1STOP NO PARITY �� �����Ѵ�.
// 	UART_LCR = (LCR_WLS1|LCR_WLS0|LCR_DLAB);
	
//     	UART_DLL = baudrate;
//     	UART_DLH = 0x00;
    
//     	UART_LCR = (LCR_WLS1|LCR_WLS0); 

//     	// UART�� ���� ��Ų��. 
//     	UART_IER = IER_UUE;
  
// }
//------------------------------------------------------------------------------
// ���� : �ø��� ����̽��� �� ���ڸ� ������. 
// �Ű� : ���� 
// ��ȯ : ���� 
// ���� : ���� 
//------------------------------------------------------------------------------
void SerialOutChar( const char c  )
{
	// ���� �����Ҷ����� ��ٸ���. 
	while(( UART_LSR & LSR_TDRQ ) == 0 ); 

	// �ø��� ����Ÿ�� ����Ѵ�. 
	UART_DATA = c;
}

//------------------------------------------------------------------------------
// ���� : �ø��� ����̽��� �� ���ڸ� ������. 
// �Ű� : ���� 
// ��ȯ : ���� 
// ���� : CR �� LR CR �� �����Ͽ� ����Ѵ�. 
//------------------------------------------------------------------------------
void SerialOutChar_CheckCR( const char c  )
{
	SerialOutChar( c );
	if(c == '\n') SerialOutChar('\r');
}
//------------------------------------------------------------------------------
// ���� : �ø��� ����̽��� ���ڿ��� ������. 
// �Ű� : ���� 
// ��ȯ : ���� 
// ���� : ���� 
//------------------------------------------------------------------------------
int SerialOutStr( char *str, int size )
{
	int	lp ;

	for (lp=0;lp<size;lp++) SerialOutChar(str[lp]) ;

	return lp;
}
//------------------------------------------------------------------------------
// ���� : �ø��� ����̽��� ���ڿ��� ������. 
// �Ű� : ���� 
// ��ȯ : ���� 
// ���� : ���� 
//------------------------------------------------------------------------------
int SerialOutStr_CheckCR( char *str, int size )
{
	int	lp ;

	for (lp=0;lp<size;lp++) SerialOutChar_CheckCR(str[lp]) ;

	return lp;
}
//------------------------------------------------------------------------------
// ���� : �ø��� ����̽����� ���ŵ� ����Ÿ�� �ִ��� Ȯ���Ѵ�. 
// �Ű� : ���� 
// ��ȯ : ���ŵ� ����Ÿ�� ������ 1 / ������ 0
// ���� : ���� 
//------------------------------------------------------------------------------
int SerialIsReadyChar( void )
{
	// ���ŵ� ����Ÿ�� �ִ°��� Ȯ���Ѵ�. 
	if( UART_LSR & LSR_DR ) return 1;
	return 0;
}

//------------------------------------------------------------------------------
// ���� : �ø��� ���¸� ���� �ش�. 
// �Ű� : ���� 
// ��ȯ : ���ŵ� ����Ÿ�� ������ 1 / ������ 0
// ���� : ���� 
//------------------------------------------------------------------------------
int SeriallGet_LSR( void )
{
	return UART_LSR;
}
//------------------------------------------------------------------------------
// ���� : �ø��� ����̽����� ���� ���¸� �о� �´�. 
// �Ű� : ���� 
// ��ȯ : ���ŵ� ���� 
// ���� : ���� 
//------------------------------------------------------------------------------
char SerialIsGetChar( void )
{
	// ������ ���� �´�. 
    	SerialErrorFlag = UART_LSR & (LSR_PE | LSR_FE | LSR_OE);
    
	// ���ŵ� ����Ÿ�� ���� �´�. 
	return (char) UART_DATA;
}
//------------------------------------------------------------------------------
// ���� : �ø��� ����̽��� ���� ���� �ö�׸� Ŭ���� �Ѵ�. 
// �Ű� : ���� 
// ��ȯ : ���ŵ� ���� 
// ���� : ���� 
//------------------------------------------------------------------------------
char SerialIsClearError( void )
{
	SerialErrorFlag = 0;
	return (char)SerialErrorFlag;
}
//------------------------------------------------------------------------------
// ���� : �ø��� ����̽��� ���� ���� �ö�� ���� ��´�. 
// �Ű� : ���� 
// ��ȯ : ���ŵ� ���� 
// ���� : ���� 
//------------------------------------------------------------------------------
int SerialIsGetError( void )
{
	return SerialErrorFlag;
}

