#include "xparameters.h"
#include "xil_types.h"
#include "xil_cache.h"
#include "xuartps.h"    // PS UART
#include "xscutimer.h"  // PS Timer
#include "xdmaps.h"     // PS DMA
#include "xinterrupt_wrap.h"
#include "xil_printf.h"

#define BRAM_MEMORY XPAR_AXI_BRAM_CTRL_0_BASEADDR
#define LENGTH 8192 // Длина буфера в словах (u32)

// Базовый адрес DDR для PS7
#define DDR_MEMORY (XPAR_PS7_DDR_0_BASEADDRESS + 0x00020000)

#define TIMER_LOAD_VALUE 0xFFFFFFFF

volatile static int Done = 0;   /* Флаг завершения DMA */
volatile static int Error = 0;  /* Флаг ошибки DMA */

XUartPs 	Uart_PS;
XScuTimer 	Timer;
XDmaPs 		Dma;
XScuGic 	Gic;

void DmaDoneHandler(unsigned int Channel, XDmaPs_Cmd *DmaCmd, void *CallbackRef)
{
    Done = 1;
}

void DmaFaultHandler(unsigned int Channel, XDmaPs_Cmd *DmaCmd, void *CallbackRef)
{
    Error = 1;
}


u8 menu(void)
{
    u8 byte;
    print("Enter 1 for BRAM to BRAM transfer\r\n");
    print("Enter 2 for BRAM to DDR3 transfer\r\n");
    print("Enter 3 for DDR3 to DDR3 transfer\r\n");
    print("Enter 4 to exit\r\n");
    while (!XUartPs_IsReceiveData(STDIN_BASEADDRESS));
    byte = XUartPs_ReadReg(STDIN_BASEADDRESS, XUARTPS_FIFO_OFFSET);
    return(byte);
}

u8 byte_sel(void)
{
    u8 byte;
    print("Enter number of words you want to transfer:\r\n");
    print("1=256; 2=512; 3=1024; 4=2048; 5=4096; 6=8192;\r\n");
    while (!XUartPs_IsReceiveData(STDIN_BASEADDRESS));
    byte = XUartPs_ReadReg(STDIN_BASEADDRESS, XUARTPS_FIFO_OFFSET);
    return(byte);
}

int main (void) {

    u8 select;
    int i;
    int num;
    u8 num_in;
    int dma_improvement;

    u32 * source, * destination;
    int software_cycles, interrupt_cycles;
    int test_done = 0;

    int Status;
    XUartPs_Config      *UartConfigPtr;
    XScuTimer_Config    *TimerConfigPtr;
    XDmaPs_Config       *DmaCfgPtr;
    XScuGic_Config      *GicConfigPtr;

    volatile u32 CntValue1;

    XDmaPs_Cmd DmaCmd = {
        .ChanCtrl = {
            .SrcBurstSize = 4,
            .SrcBurstLen = 4,
            .SrcInc = 1,
            .DstBurstSize = 4,
            .DstBurstLen = 4,
            .DstInc = 1,
        },
    };
    unsigned int Channel = 0;

    // 1. Инициализация UART
    UartConfigPtr = XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);
    if (NULL == UartConfigPtr) {
        return XST_FAILURE;
    }

    Status = XUartPs_CfgInitialize(&Uart_PS, UartConfigPtr, UartConfigPtr->BaseAddress);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // 2. Инициализация SCU Timer
    TimerConfigPtr = XScuTimer_LookupConfig(XPAR_XSCUTIMER_0_BASEADDR);
    if (NULL == TimerConfigPtr) {
        print("XScuTimer_LookupConfig failed\r\n");
        return XST_FAILURE;
    }

    Status = XScuTimer_CfgInitialize(&Timer, TimerConfigPtr, TimerConfigPtr->BaseAddr);
    if (Status != XST_SUCCESS) {
        print("XScuTimer_CfgInitialize failed\r\n");
        return XST_FAILURE;
    }

    // 3. Инициализация GIC
    GicConfigPtr = XScuGic_LookupConfig(XPAR_XSCUGIC_0_BASEADDR);
    if (NULL == GicConfigPtr) {
        print("XScuGic_LookupConfig failed\r\n");
        return XST_FAILURE;
    }

    Status = XScuGic_CfgInitialize(&Gic, GicConfigPtr, GicConfigPtr->CpuBaseAddress);
    if (Status != XST_SUCCESS) {
        print("XScuGic_CfgInitialize failed\r\n");
        return XST_FAILURE;
    }

    XScuTimer_LoadTimer(&Timer, TIMER_LOAD_VALUE);
    XScuTimer_Start(&Timer);

    print("-- Simple DMA Design Example (Vitis 2026.1) --\r\n");

    CntValue1 = XScuTimer_GetCounterValue(&Timer);
    xil_printf("Above message printing took %d clock cycles\r\n", TIMER_LOAD_VALUE - CntValue1);

    // 4. Инициализация DMA Controller
    DmaCfgPtr = XDmaPs_LookupConfig(XPAR_XDMAPS_0_BASEADDR);
    if (!DmaCfgPtr) {
        print("Lookup DMAC failed\r\n");
        return XST_FAILURE;
    }

    Status = XDmaPs_CfgInitialize(&Dma, DmaCfgPtr, DmaCfgPtr->BaseAddress);
    if (Status != XST_SUCCESS) {
        print("XDmaPs_CfgInitialize failed\r\n");
        return XST_FAILURE;
    }

	Status = XSetupInterruptSystem(&Dma, &XDmaPs_FaultISR,
				       Dma.Config.IntrId[0],
				       Dma.Config.IntrParent,
				       XINTERRUPT_DEFAULT_PRIORITY);

	Status = XSetupInterruptSystem(&Dma, &XDmaPs_DoneISR_0,
				       Dma.Config.IntrId[1],
				       Dma.Config.IntrParent,
				       XINTERRUPT_DEFAULT_PRIORITY);
                       
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    test_done = 0;

    while(test_done == 0)
    {
        num_in = byte_sel();

        switch(num_in)
        {
            case '1' : num = 256; break;
            case '2' : num = 512; break;
            case '3' : num = 1024; break;
            case '4' : num = 2048; break;
            case '5' : num = 4096; break;
            case '6' : num = 8192; break;
            default  : num = 256; break;
        }
        xil_printf("Sending %d words\r\n", num);

        select = menu();

        switch(select)
        {
            case '1' :
                source = (u32 *)BRAM_MEMORY;
                destination = (u32 *)(BRAM_MEMORY + (LENGTH * sizeof(u32)));
                print("BRAM to BRAM transfer\r\n");
                break;
            case '2' :
                source = (u32 *)BRAM_MEMORY;
                destination = (u32 *)DDR_MEMORY;
                print("BRAM to DDR3 transfer\r\n");
                break;
            case '3' :
                source = (u32 *)DDR_MEMORY;
                destination = (u32 *)(DDR_MEMORY + (LENGTH * sizeof(u32)));
                print("DDR3 to DDR3 transfer\r\n");
                break;
            case '4' :
                test_done = 1;
                break;
            default :
                source = (u32 *)DDR_MEMORY;
                destination = (u32 *)(DDR_MEMORY + (LENGTH * sizeof(u32)));
                print("DDR3 to DDR3 transfer\r\n");
                break;
        }

        if(test_done)
            break;

        // Заполнение буфера источника
        for (i = 0; i < num; i++)
            *(source + i) = num - i;

        // Сброс буфера приемника
        for (i = 0; i < num; i++)
            *(destination + i) = 0;

        // Сброс кэша: выталкиваем данные из процессора в RAM
        Xil_DCacheFlushRange((UINTPTR)source, num * sizeof(u32));
        Xil_DCacheFlushRange((UINTPTR)destination, num * sizeof(u32));

        // --- Программная передача ---
        XScuTimer_RestartTimer(&Timer);

        for (i = 0; i < num; i++)
            *(destination + i) = *(source + i);

        CntValue1 = XScuTimer_GetCounterValue(&Timer);
        software_cycles = TIMER_LOAD_VALUE - CntValue1;
        xil_printf("Moving data through processor took %d clock cycles\r\n", software_cycles);

        // Очищаем область назначения перед тестом DMA
        for (i = 0; i < num; i++)
            *(destination + i) = 0;

        Xil_DCacheFlushRange((UINTPTR)destination, num * sizeof(u32));

        // --- Передача через DMA ---
        DmaCmd.BD.SrcAddr = (UINTPTR)source;
        DmaCmd.BD.DstAddr = (UINTPTR)destination;
        DmaCmd.BD.Length = num * sizeof(u32);


        Done = 0;
        Error = 0;

        XDmaPs_SetDoneHandler(&Dma, Channel, DmaDoneHandler, NULL);
        XDmaPs_SetFaultHandler(&Dma, DmaFaultHandler, NULL);

        Status = XDmaPs_Start(&Dma, Channel, &DmaCmd, 0);
        if (Status != XST_SUCCESS) {
            xil_printf("XDmaPs_Start failed\r\n");
            return XST_FAILURE;
        }

        XScuTimer_RestartTimer(&Timer);

        while ((Done == 0) && (Error == 0));

        CntValue1 = XScuTimer_GetCounterValue(&Timer);
        interrupt_cycles = TIMER_LOAD_VALUE - CntValue1;

        if (Error) {
            print("Error occurred during DMA transfer\r\n");
        } else {
            xil_printf("Moving data through DMA took %d clock cycles\r\n", interrupt_cycles);
        }

        // Инвалидация кэша: принуждаем CPU читать свежие данные напрямую из RAM
        Xil_DCacheInvalidateRange((UINTPTR)destination, num * sizeof(u32));

        // Проверка корректности передачи
        for (i = 0; i < num; i++) {
            if (destination[i] != source[i]) {
                xil_printf("Data match failed at index = %d, src = %d, dst = %d\r\n", i, source[i], destination[i]);
                print("-- Exiting main() --\r\n");
                return XST_FAILURE;
            }
        }

        print("Transfered data verified successfully\r\n");
        if (interrupt_cycles > 0) {
            dma_improvement = software_cycles / interrupt_cycles;
            xil_printf("Improvement using DMA = %d x\r\n", dma_improvement);
        }
        xil_printf("-------------------------------------------------------------------\r\n\r\n");
    }

    print("-- Exiting main() --\r\n");
    return 0;
}