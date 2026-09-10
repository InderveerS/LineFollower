/*
 * sender.h
 *
 *  Created on: Aug 12, 2026
 *      Author: inder
 */

#ifndef INC_SENDER_H_
#define INC_SENDER_H_

#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>

#define BUFF_SIZE 64

int sendf(const char *message, ...)
__attribute__((format(printf, 1, 2)));

#endif /* INC_SENDER_H_ */
