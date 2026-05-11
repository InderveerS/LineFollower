#include "data_logging.h"


// global variables

volatile uint32_t log_write_ptr = LOG_BASE_ADDR;
volatile uint32_t total_samples = 0;
volatile uint8_t logging_active = 0;
volatile uint8_t time_ms = 0;


// functions

int flash_write_word(uint32_t value) {
	if (log_write_ptr >= LOG_MAX_ADDR) {
		return -1;
	}

	HAL_Flash_Unlock();

	HAL_StatusTypeDef status = HAL_FLASH_Program(
			FLASH_TYPEPROGRAM_WORD,
	        log_write_ptr,
	        value
	    );

	    HAL_FLASH_Lock();

	    if (status == HAL_OK) {
	    	log_write_ptr+=4;
	    	return 0;
	    } else {
	    	return -1;
	    }
}

void scan_flash_for_end () {
	 LogRecord_t *ptr = (LogRecord_t *)LOG_BASE_ADDR;

	    while ((uint32_t)ptr < LOG_MAX_ADDR)
	    {
	        if (ptr->dummy == 0xFFFFFFFF) // if record is empty
	        {
	            log_write_ptr = (uint32_t)ptr;
	            return;
	        }
	        ptr++;
	    }
	    log_write_ptr = LOG_MAX_ADDR;
}


void logging_init() {
	total_samples = 0;
	logging_active = 0;
	time_ms = 0;

	scan_flash_for_end (); //

	 char msg[100];
	    int len = snprintf(msg, sizeof(msg),
	        "Logging initialized. Write pointer: 0x%lx, Samples: %lu\r\n",
	        log_write_ptr, total_samples);

	  HAL_UART_Transmit(&huart3, (uint8_t *)msg, len, 100);
	// 1. the UART peripheral to use 2. the data so send
	 // 3. the size of that data 4. the max time to wait

}


void logging_start() {
	    if (logging_erase() != 0)
	    {
	        char msg[100];
	        int len = snprintf(msg, sizeof(msg), "Failed to start logging\r\n");
	        HAL_UART_Transmit(&huart3, (uint8_t *)msg, len, 100);
	        return;
	    }


	    log_time_ms = 0;
	    total_samples = 0;

	    logging_active = 1;


	    HAL_TIM_Base_Start_IT(&htim6);

	    char msg[100];
	    int len = snprintf(msg, sizeof(msg),
	        "Logging started. Will run for 30 seconds at 100 Hz\r\n");
	    HAL_UART_Transmit(&huart3, (uint8_t *)msg, len, 100);
}


void logging_stop(void)
{

    HAL_TIM_Base_Stop_IT(&htim6);


    logging_active = 0;

    char msg[150];
    int len = snprintf(msg, sizeof(msg),
        "Logging stopped. Total samples: %lu, Time: %lu ms\r\n"
        "Ready to dump data. Send 'D' command to download.\r\n",
        total_samples, log_time_ms);
    HAL_UART_Transmit(&huart3, (uint8_t *)msg, len, 100);
}


int logging_erase(void)
{
    FLASH_EraseInitTypeDef EraseInitStruct = {0};
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Sector = FLASH_SECTOR_7;
    EraseInitStruct.NbSectors = 1;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    uint32_t SectorError = 0;


    HAL_FLASH_Unlock();


    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);


    HAL_FLASH_Lock();


    if (status == HAL_OK)
    {

        log_write_ptr = LOG_BASE_ADDR;
        total_samples = 0;
        log_time_ms = 0;


        char msg[100];
        int len = snprintf(msg, sizeof(msg), "Flash erased successfully\r\n");
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, len, 100);

        return 0;
    }
    else
    {

        char msg[100];
        int len = snprintf(msg, sizeof(msg), "Flash erase failed\r\n");
        HAL_UART_Transmit(&huart3, (uint8_t *)msg, len, 100);

        return -1;
    }
}


int logging_record(void)
{
    if (!logging_active) {
        return -1;
    }

    uint32_t left_count = __HAL_TIM_GET_COUNTER(&htim3);
    uint32_t right_count = __HAL_TIM_GET_COUNTER(&htim4);


    LogRecord_t record;
    record.left_encoder_count = left_count;
    record.right_encoder_count = right_count;
    record.dummy = 0;  // not empty


    if (flash_write_word(record.left_encoder_count) != 0)
        return -1;

    if (flash_write_word(record.right_encoder_count) != 0)
        return -1;

    if (flash_write_word(record.dummy) != 0)
        return -1;


    total_samples++;

    return 0;
}


void logging_dump_csv(void) {

    const char header[] = "sample_num,left_count,right_count\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t *)header, strlen(header), 100);

    uint32_t num_records = total_samples;

    LogRecord_t *ptr = (LogRecord_t *)LOG_BASE_ADDR;

    for (uint32_t i = 0; i < num_records; i++, ptr++)
    {
        if (ptr->dummy == 0xFFFFFFFF)
        {
            break;
        }

        char line[80];
        int len = snprintf(line, sizeof(line),
            "%lu,%lu,%lu\r\n",
            i,
            ptr->left_encoder_count,
            ptr->right_encoder_count);

        HAL_UART_Transmit(&huart3, (uint8_t *)line, len, 100);
    }

    const char footer[] = "EOF\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t *)footer, strlen(footer), 100);

    char summary[100];
    int len = snprintf(summary, sizeof(summary),
        "Dump complete. Total records: %lu\r\n", num_records);
    HAL_UART_Transmit(&huart3, (uint8_t *)summary, len, 100);
}


