#include <samd51.h>


void delay(int n)
{
    int i;

    for (;n >0; n--)
    {
        for (i=0;i<50;i++)
            __asm("nop");
    }
}

void led_toggle() {
    PORT->Group[1].OUTTGL.reg = (1<<8);
    PORT->Group[0].OUTTGL.reg = (1<<17);
    delay(1);
}


void spi_init(){
    // LED BLANK = PA16
    // LED LATCH = PA18
    // LED SCLK = PA17
    // LED DAT = PA19    

    MCLK->APBAMASK.reg |= MCLK_APBAMASK_SERCOM1;
    GCLK->PCHCTRL[SERCOM1_GCLK_ID_CORE].reg = GCLK_PCHCTRL_GEN_GCLK0
                                            | GCLK_PCHCTRL_CHEN;


    SERCOM1->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_SWRST;
    while(SERCOM1->SPI.SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_SWRST);

    SERCOM1->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_DOPO(2)
                           | SERCOM_SPI_CTRLA_MODE(3)
                           | SERCOM_SPI_CTRLA_DORD;
    SERCOM1->SPI.CTRLC.reg = SERCOM_SPI_CTRLC_DATA32B;
    SERCOM1->SPI.BAUD.reg = 1;
    SERCOM1->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
    while(SERCOM1->SPI.SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_ENABLE);
    

    PORT->Group[0].PMUX[17>>1].bit.PMUXO = 2;
    PORT->Group[0].PINCFG[17].bit.PMUXEN = 1;

    PORT->Group[0].PMUX[19>>1].bit.PMUXO = 2;
    PORT->Group[0].PINCFG[19].bit.PMUXEN = 1;
}

int main()
{
    PORT->Group[1].DIRSET.reg = (1 << 8);
    
    
    
    PORT->Group[0].DIRSET.reg = (1 << 16) | (1 << 18);
    
    PORT->Group[0].OUTCLR.reg = (1 << 16); // Ignore BLANK
    spi_init();

    int i = 0;

    int col = 0;
    

    while(1){
        

        PORT->Group[0].OUTSET.reg = (1 << 16);
        
        uint32_t mask = ~(0x10000 << col);
        while(!SERCOM1->SPI.INTFLAG.bit.DRE){}
        SERCOM1->SPI.DATA.reg = mask;

        while(!SERCOM1->SPI.INTFLAG.bit.DRE){}
        SERCOM1->SPI.DATA.reg = 0xFFFFFFFF;

        while(!SERCOM1->SPI.INTFLAG.bit.TXC){}

        PORT->Group[0].OUTCLR.reg = (1 << 18);
        PORT->Group[0].OUTSET.reg = (1 << 18);
        
        PORT->Group[0].OUTCLR.reg = (1 << 16);

        col++;

        if(col >= 16){
            i++;

            
            col = 0;
        }

        led_toggle();
    }

}
