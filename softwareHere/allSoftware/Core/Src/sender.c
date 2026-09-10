/*
 * sender.c
 *
 *  Created on: Aug 12, 2026
 *      Author: inder
 */

#include "sender.h"
#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart3;

int sendf(const char *message, ...) {
	char buff[BUFF_SIZE];
	size_t size = sizeof(buff);

	va_list ap;
	va_start(ap, message);

	int length = vsnprintf(buff, size, message, ap);
	if (length < 0) {
		va_end(ap);
		return -1;
	}

	if ((size_t)length >= size)
		length = (int)size - 1;

	if (HAL_UART_Transmit(&huart3, (uint8_t *)buff, (uint16_t)length,
	                      HAL_MAX_DELAY) != HAL_OK) {
		va_end(ap);
		return -1;
	}

	va_end(ap);
	return length;
}


