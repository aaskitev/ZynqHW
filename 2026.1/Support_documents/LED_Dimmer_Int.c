
/***************************** Include Files *********************************/

#include "xparameters.h"
#include "xil_io.h"
#include "xstatus.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xinterrupt_wrap.h"  

/************************** Constant Definitions *****************************/

  #define PWM_BASE_ADDRESS             XPAR_PWM_W_INT_0_BASEADDR

/* ID прерывания */
  #define INTC_PWM_INTERRUPT_ID ( 61 | XIL_TRIG_LOW_TO_HIGH_EDGE)

/************************** Variable Definitions *****************************/

volatile u32 brightness;

/************************** Interrupt Handler ********************************/

void PWMIsr(void *InstancePtr)
{
    (void)InstancePtr; // Неиспользуемый аргумент

    /* Уведомление пользователя о превышении допустимого значения PWM */
    print("PWM Value exceeded, brightness reset to zero. Please enter new value: \r\n");

    /* Сброс значения яркости в безопасный ноль */
    brightness = 0;
    Xil_Out32(PWM_BASE_ADDRESS, brightness);
}

/****************************************************************************/
/**
* Функция настройки системы прерываний с использованием Vitis xinterrupt_wrap.h
*
* @return	XST_SUCCESS в случае успеха или XST_FAILURE при ошибке.
*****************************************************************************/
int SetupInterruptSystem(void)
{
    int status;

    /* 
     * Функция XSetupInterruptSystem автоматизирует:
     * 1. Поиск конфига и инициализацию GIC/Interrupt Controller
     * 2. Регистрацию обработчика прерывания (PWMIsr)
     * 3. Настройку приоритета по умолчанию
     * 4. Включение конкретной линии прерывания (Enable)
     * 5. Инициализацию и включение таблицы исключений CPU (Exception Enable)
     */
    status = XSetupInterruptSystem(NULL, 
                                   (Xil_ExceptionHandler)PWMIsr, 
                                   INTC_PWM_INTERRUPT_ID, 
                                   XPAR_XSCUGIC_0_BASEADDR, 
                                   XINTERRUPT_DEFAULT_PRIORITY);

    if (status != XST_SUCCESS) {
        xil_printf("XSetupInterruptSystem failed with status: %d\r\n", status);
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

/************************** Main Code Entry **********************************/
int main(void)
{
    int status = XST_SUCCESS;
    u32 value = 0;
    u32 period = 0;
    brightness = 0;

    /* Сброс контроллера LED Dimmer в безопасный 0 */
    Xil_Out32(PWM_BASE_ADDRESS, 0);

    /* Инициализация системы прерываний через новое API */
    status = SetupInterruptSystem();
    if (status != XST_SUCCESS)
    {
        xil_printf("Interrupt Setup Failed!\r\n");
        return XST_FAILURE;
    }

    /* Основной цикл программы */
    while (1) 
    {
        print("Select a Brightness between 0 and 9\n\r");
        
        /* Чтение символа из UART console */
        value = inbyte();
        
        /* Преобразование ASCII символа в целое число */
        period = value - '0';
        
        xil_printf("Brightness Level %d selected\n\r", period);
        
        brightness = period * 110000;
        
        /* Запись коэффициента заполнения PWM в регистр периферии */
        Xil_Out32(PWM_BASE_ADDRESS, brightness);
    }

    return status;
}