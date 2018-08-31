/**
 * @file flash_dev.c
 * @brief flash device interface
 */

#include "wholeconfig.h"
#include "flash_dev.h"
#include "./delay/delay.h"
/**
 * @addtogroup USER_DRIVER
 * @{
 */


/**
 * @defgroup FLASH_DEV
 * @{
 */

/**
 * @defgroup FLASH_PRIVATE_INTERFACE
 * @{
 */
static uint8_t f_spi_rw_byte(uint8_t dat)
{
    uint8_t i = 0;
    for(i = 0;i < 8;i++)
    {
        if(dat & 0x80)
        {
            FLASH_MO_SET();
        }
        else
        {
            FLASH_MO_RESET();
        }
        delay_xus(1);
        FLASH_CLK_SET();
        dat <<= 1;
        
        if(FLASH_READ_MI())
        {
            dat++;
        } 
        delay_xus(1);
        FLASH_CLK_RESET();
    }
    return dat;
}


static void flash_write_enable()    
{
    FLASH_CS_RESET();
    f_spi_rw_byte(W25X_WRITEENABLE);
    FLASH_CS_SET();
}


static void flash_wait_write_end()    
{
    uint8_t status = 0;
    uint16_t err_count = 0;
    do 
    {
        delay_xms(5);
        FLASH_CS_RESET();
        f_spi_rw_byte(W25X_READSTATUSREG);        
        status = f_spi_rw_byte(DUMMY_BYTE);
        FLASH_CS_SET();
        err_count++;
    }
    while((status & WIP_FLAG) == 1 && err_count < 400);
}


static void flash_erase_sector(uint32_t addr)    
{
    flash_write_enable();
    flash_wait_write_end();
    FLASH_CS_RESET();
    f_spi_rw_byte(W25X_SECTORERASE);
    f_spi_rw_byte((addr & 0xFF0000) >> 16);
    f_spi_rw_byte((addr & 0xFF00) >> 8);
    f_spi_rw_byte(addr & 0xFF);
    FLASH_CS_SET();
    flash_wait_write_end();
}
 

static void flash_write_page(uint8_t * pbuf, uint32_t addr, uint16_t len)
{
    flash_write_enable();
    FLASH_CS_RESET();
    f_spi_rw_byte(W25X_PAGEPROGRAM);
    f_spi_rw_byte((addr & 0xFF0000) >> 16);
    f_spi_rw_byte((addr & 0xFF00) >> 8);
    f_spi_rw_byte(addr & 0xFF);
    if (len > FLASH_PAGE_SIZE)
    {
        len = FLASH_PAGE_SIZE;
    }

    while (len--)
    {
        f_spi_rw_byte(*pbuf);
        pbuf++;
    }
    FLASH_CS_SET();
    flash_wait_write_end();
}

#define flash_lock()    //CRITICAL_REGION_ENTER()
#define flash_unlock()  //CRITICAL_REGION_EXIT()


/**
 * @}
 */


/**
 * @defgroup FLASH_PUBLIC_INTERFACE
 * @{
 */

static uint8_t sg_flash_breakdown = 0;
#define FLASH_SELFCHECK()   do{if(sg_flash_breakdown > 0){return;}}while(0)

uint8_t flash_breakdown(uint8_t n)
{
    if(n > 1)
    {
        return sg_flash_breakdown;
    }
    sg_flash_breakdown = n;
    return sg_flash_breakdown;
}

static uint8_t sg_flash_power_count = 0;

void flash_power_down()
{
    FLASH_SELFCHECK();
    if(sg_flash_power_count > 0)
    {
        sg_flash_power_count--;
        if(sg_flash_power_count == 0)
        {
            flash_lock();
            FLASH_CS_RESET();
            f_spi_rw_byte(W25X_POWERDOWN);
            FLASH_CS_SET();
            delay_xus(3);
            flash_unlock();
        }
    }
}


void flash_wakeup()    
{
    FLASH_SELFCHECK();
    if(sg_flash_power_count == 0)
    {
        flash_lock();
        FLASH_CS_RESET();
        if (FlashMID == FlashMID_Winbond) 
        {
            f_spi_rw_byte(W25X_RELEASEPOWERDOWN);
        }
        FLASH_CS_SET();
        delay_xus(3);
        flash_unlock();
    }
    sg_flash_power_count++;
}



void flash_write_buf(uint8_t * pbuf, uint32_t addr, uint16_t len)
{
    uint16_t first_page_number = FLASH_PAGE_SIZE - (uint16_t)(addr % FLASH_PAGE_SIZE);
    uint16_t pages, i = 0;
    uint16_t last_page_number;
    FLASH_SELFCHECK();
    flash_lock();
    if(len > first_page_number)
    {
        flash_write_page(pbuf, addr, first_page_number);
        addr += first_page_number;
        pbuf += first_page_number;
        pages = (len - first_page_number) / FLASH_PAGE_SIZE;
        last_page_number = (len - first_page_number) % FLASH_PAGE_SIZE;
        for( i = 0;i < pages;i++)
        {
            flash_write_page(pbuf, addr, FLASH_PAGE_SIZE);
            addr += FLASH_PAGE_SIZE;
            pbuf += FLASH_PAGE_SIZE;
        }
        if(last_page_number != 0)
        {
            flash_write_page(pbuf, addr, last_page_number);
        }
    }
    else
    {
        flash_write_page(pbuf, addr, len);
    } 
    flash_unlock();
}


void flash_read_buf(uint8_t * pbuf, uint32_t addr, uint16_t len)    
{
    FLASH_SELFCHECK();
    FLASH_CS_RESET();
    f_spi_rw_byte(W25X_READDATA);
    f_spi_rw_byte((addr & 0xFF0000) >> 16);
    f_spi_rw_byte((addr & 0xFF00) >> 8);
    f_spi_rw_byte(addr & 0xFF);
    while (len--) 
    {
        *pbuf = f_spi_rw_byte(DUMMY_BYTE);
        pbuf++;
    }
    FLASH_CS_SET();
}


/**
 * @brief flash erase. the unit is the size of sector
 * @param addr start address
 * @param sector_n the number of sectors that want to erase
 */
void flash_erase_xsector(uint32_t addr, uint32_t sector_n)
{
    uint32_t i = 0;
    FLASH_SELFCHECK();
    flash_lock();
    b_log("erase: %x, %d\n\r", addr, sector_n);
    for(i = 0;i < sector_n;i++)
    {
        flash_erase_sector(addr);
        addr += W25X_SECTOR_SIZE;
    }
    flash_unlock();
}


void flash_erase_chip()
{
    FLASH_SELFCHECK();
    flash_lock();
    flash_write_enable();
    flash_wait_write_end();
    FLASH_CS_RESET();
    f_spi_rw_byte(W25X_CHIPERASE);
    FLASH_CS_SET();
    flash_wait_write_end(); 
    flash_unlock();    
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */


