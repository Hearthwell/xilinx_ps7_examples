#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xstatus.h"
#include "xintc.h"
#include "xgpio.h"

static XIntc InterruptController;
static XGpio gpioHandle;

static void gpio_isr0(void *args){
	(void) args;
	print("GPIO INTERRUPT TRIGGERED\n\r");
	XGpio_InterruptClear(&gpioHandle, XGPIO_IR_MASK);
}

static void initialize_gpio0(){
	if (XGpio_Initialize(&gpioHandle, XPAR_GPIO_0_DEVICE_ID) == XST_DEVICE_NOT_FOUND){
		print("COULD NOT INITIALIZE AXI GPIO\n\r");
		return;
	}
	XGpio_SetDataDirection(&gpioHandle, 1, 0x1);
	XGpio_SetDataDirection(&gpioHandle, 2, 0x0);
	XGpio_InterruptGlobalEnable(&gpioHandle);
	XGpio_InterruptEnable(&gpioHandle, XGPIO_IR_MASK);
}

static int initialize_intc(){
	int status;
	status = XIntc_Initialize(&InterruptController, XPAR_INTC_0_DEVICE_ID);
	if (status != XST_SUCCESS) return XST_FAILURE;

	status = XIntc_Connect(&InterruptController, XPAR_INTC_0_GPIO_0_VEC_ID, gpio_isr0, NULL);
	if (status != XST_SUCCESS) return XST_FAILURE;

	status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if (status != XST_SUCCESS) return XST_FAILURE;

	XIntc_Enable(&InterruptController, XPAR_INTC_0_GPIO_0_VEC_ID);

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_InterruptHandler)XIntc_InterruptHandler, &InterruptController);
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

int main(void)
{
	int status;
	initialize_gpio0();
	status = initialize_intc();

	if(status != XST_SUCCESS) print("COULD NOT INITIALIZE INTERRUPTS\n\r");

	while(1){

	}

	return 0;
}
