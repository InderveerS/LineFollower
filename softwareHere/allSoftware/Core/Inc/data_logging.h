#ifndef DATA_LOGGING_H
#define DATA_LOGGING_H

#include "main.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// Variables
#define LOG_BASE_ADDR           0x08060000      // base
#define LOG_SECTOR_SIZE         (256 * 1024)   // memory for logging
#define LOG_MAX_ADDR            0x08100000 // max


// logging structure (ensures that no extra bytes are being used)

#pragma pack(1)
typedef struct {
    uint32_t dummy;          // checking for empty
    uint32_t left_encoder_count;    // Raw encoder count from TIM3
    uint32_t right_encoder_count;   // Raw encoder count from TIM4
} LogRecord_t;  // 12 bytes per record
#pragma pack()

#define RECORD_SIZE sizeof(LogRecord_t)  // shortcut to get size (12)
#define LOG_MAX_RECORDS (LOG_SECTOR_SIZE / RECORD_SIZE)  /// shortcut to get max (21845)

// logging structure control variables
extern volatile uint32_t log_write_ptr;     // current write flash position (where to write next sample)
extern volatile uint32_t total_samples;     // # of total samples logged (so far)
extern volatile uint8_t logging_active;     // is logging currently runnung (1 yes 0 no)





// Function declarations

/**
 * @brief Initialize logging system (call once at boot)
 * @param None
 * @retval None
 */
void logging_init(void);

/**
 * @brief Begins logging system (starts a new logging cycle)
 * @param None
 * @retval None
 */

void logging_start(void);

/**
 * @brief Ends the current logging cycle
 * @param None
 * @retval None
 */

void logging_end(void);

/**
 * @brief Records one logging cycle
 * @param None
 * @retval 0 is success, -1 if flash is full
 */

int logging_record(void);

/**
 * @brief Puts all the logged data into a csv over UART
 * @param None
 * @retval None
 */

void logging_csv(void);

/**
 * @brief Errases the logging flash sector (prepares for a new one)
 * DELETES ALL THE DATA
 * @param None
 * @retval 0 if success, -1 if failed
 */

int logging_errase(void);

/**
 * @brief Write a 32-bit value to flash (internal function)
 * @param value: The data to write
 * @retval 0 if success, -1 if flash is full
 */
int flash_write_word(uint32_t value);

/**
 * @brief Scan flash at boot to find where logging ended
 * This restores log_write_ptr after a power cycle
 * @param None
 * @retval None
 */
void flash_scan_for_end(void);

#endif /* DATA_LOGGING_H */









