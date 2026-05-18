#ifndef DATA_LOGGING_H
#define DATA_LOGGING_H
#include "main.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
// Flash memory configuration
#define LOG_BASE_ADDR       0x08060000
#define LOG_SECTOR_SIZE     (256 * 1024)
#define LOG_MAX_ADDR        0x08100000
// Log record structure (12 bytes per record, no padding)
#pragma pack(1)
typedef struct {
   uint32_t left_encoder_count;    // Raw encoder count from TIM3
   uint32_t right_encoder_count;   // Raw encoder count from TIM4
   uint32_t dummy;                 // Used to check if record is empty (0xFFFFFFFF = empty)
} LogRecord_t;
#pragma pack()
#define RECORD_SIZE     sizeof(LogRecord_t)
#define LOG_MAX_RECORDS (LOG_SECTOR_SIZE / RECORD_SIZE)
// Shared control variables
extern volatile uint32_t log_write_ptr;
extern volatile uint32_t total_samples;
extern volatile uint32_t log_time_ms;
extern volatile uint8_t  logging_active;
// Function declarations
/**
* @brief Initialize logging system (call once at boot)
* @retval None
*/
void logging_init(void);
/**
* @brief Begin a new logging session
* @retval None
*/
void logging_start(void);
/**
* @brief Stop the current logging session
* @retval None
*/
void logging_stop(void);
/**
* @brief Log one sample to flash (called every 10ms by timer interrupt)
* @retval 0 if success, -1 if flash is full
*/
int logging_record(void);
/**
* @brief Dump all logged data as CSV over UART
* @retval None
*/
void logging_dump_csv(void);
/**
* @brief Erase the logging flash sector (wipes all data)
* @retval 0 if success, -1 if failed
*/
int logging_erase(void);
/**
* @brief Write a 32-bit value to flash
* @retval 0 if success, -1 if flash is full
*/
int flash_write_word(uint32_t value);
/**
* @brief Scan flash at boot to find where logging ended
* @retval None
*/
void scan_flash_for_end(void);
#endif /* DATA_LOGGING_H */

