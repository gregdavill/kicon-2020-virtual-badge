#include <samd51.h>


//uint32_t frame_buffer[16];


extern const uint32_t never[];
extern const uint32_t gamma[];


void delay(int n)
{
    int i;

    for (;n >0; n--)
    {
            __asm("nop");
    }
}

void led_toggle() {
    PORT->Group[1].OUTTGL.reg = (1<<8);
    PORT->Group[0].OUTTGL.reg = (1<<17);
    delay(1);
}


void timer_start(int count){
    if(count == 0){
        count = 1;
    }
    /* Load timer */
    TC0->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_STOP;
    TC0->COUNT16.COUNT.reg = count;
    while(TC0->COUNT16.SYNCBUSY.reg & TCC_SYNCBUSY_COUNT);

    /*Start */ 
    EVSYS->SWEVT.reg = EVSYS_SWEVT_CHANNEL0; // Note channel is x-1 (0 is no channel)
   

    
    //TC0->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;



    while(!(TC0->COUNT16.INTFLAG.reg & TC_INTFLAG_OVF));
    TC0->COUNT16.INTFLAG.reg = TC_INTFLAG_OVF;
}

void timer_init(){
    MCLK->APBAMASK.bit.TC0_ = 1;
    GCLK->PCHCTRL[TC0_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK0
                                   | GCLK_PCHCTRL_CHEN;

    TC0->COUNT32.CTRLA.reg = TC_CTRLA_SWRST;
    while(TC0->COUNT16.SYNCBUSY.reg & TC_SYNCBUSY_SWRST);

    TC0->COUNT16.EVCTRL.reg = TC_EVCTRL_OVFEO
                            | TC_EVCTRL_TCEI
                            | TC_EVCTRL_EVACT_START;



    TC0->COUNT16.CTRLBSET.reg = TC_CTRLBSET_ONESHOT | TC_CTRLBSET_DIR;
    TC0->COUNT32.CTRLA.reg = TC_CTRLA_ENABLE;
    while(TC0->COUNT16.SYNCBUSY.reg & TC_SYNCBUSY_ENABLE);


    MCLK->APBBMASK.bit.EVSYS_ = 1;
    GCLK->PCHCTRL[EVSYS_GCLK_ID_0].reg = GCLK_PCHCTRL_GEN_GCLK0
                                   | GCLK_PCHCTRL_CHEN;
    GCLK->PCHCTRL[EVSYS_GCLK_ID_1].reg = GCLK_PCHCTRL_GEN_GCLK0
                                   | GCLK_PCHCTRL_CHEN;
    
    /* CLEAR BLANK on EVENT0 */
    EVSYS->Channel[0].CHANNEL.reg = EVSYS_CHANNEL_EVGEN(0) 
                                  | EVSYS_CHANNEL_PATH_ASYNCHRONOUS; // SW triggered
    EVSYS->USER[EVSYS_ID_USER_PORT_EV_0].reg = EVSYS_USER_CHANNEL(0+1); // Note channel is x-1 (0 is no channel)
    EVSYS->USER[EVSYS_ID_USER_TC0_EVU].reg = EVSYS_USER_CHANNEL(0+1); // Note channel is x-1 (0 is no channel)

    /* Set BLANK on EVENT 1*/
    EVSYS->Channel[1].CHANNEL.reg = EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_TC0_OVF)
                                  | EVSYS_CHANNEL_PATH_ASYNCHRONOUS;
    EVSYS->USER[EVSYS_ID_USER_PORT_EV_1].reg = EVSYS_USER_CHANNEL(1+1); // Note channel is x-1 (0 is no channel)
   
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
    SERCOM1->SPI.BAUD.reg = 0;
    SERCOM1->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
    while(SERCOM1->SPI.SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_ENABLE);
    

    PORT->Group[0].PMUX[17>>1].bit.PMUXO = 2;
    PORT->Group[0].PINCFG[17].bit.PMUXEN = 1;

    PORT->Group[0].PMUX[19>>1].bit.PMUXO = 2;
    PORT->Group[0].PINCFG[19].bit.PMUXEN = 1;
}


int frame = 0;
int main()
{
    PORT->Group[1].DIRSET.reg = (1 << 8);
    
    
    
    PORT->Group[0].DIRSET.reg = (1 << 16) | (1 << 18);
    
    PORT->Group[0].OUTCLR.reg = (1 << 16); // Ignore BLANK


    GCLK->GENCTRL[0].reg = GCLK_GENCTRL_DIV(0) 
                         | GCLK_GENCTRL_GENEN
                         | GCLK_GENCTRL_SRC_DFLL;


    /* PORT EVENT 0, CLEAR 16*/
    PORT->Group[0].EVCTRL.reg = PORT_EVCTRL_PORTEI0
                              | PORT_EVCTRL_EVACT0(PORT_EVCTRL_EVACT0_CLR_Val)
                              | PORT_EVCTRL_PID0(16)
                              | PORT_EVCTRL_PORTEI1
                              | PORT_EVCTRL_EVACT1(PORT_EVCTRL_EVACT0_SET_Val)
                              | PORT_EVCTRL_PID1(16);
                              


    spi_init();

    timer_init();

    int i = 0;

    int col = 0;
    

    int d = 0;
    int c = 0;

    while(1){
        
        for(int k = 0; k < 8; k++){
            bitplane(col, k);
        }
        if(++col >= 16){
            i++;   
            col = 0;


            if(i > 40){
                i = 0;
                frame++;

                if(frame > 499)
                    frame = 0;
            }
        }
    }

}


void bitplane(uint32_t col, uint32_t offset){
    

    uint32_t mask = ~(0x10000 << col);
    while(!SERCOM1->SPI.INTFLAG.bit.DRE){}
    SERCOM1->SPI.DATA.reg = mask;

    while(!SERCOM1->SPI.INTFLAG.bit.DRE){}
    SERCOM1->SPI.DATA.reg = never[col + 16*offset + frame*16*8];

    while(!SERCOM1->SPI.INTFLAG.bit.TXC){}

    PORT->Group[0].OUTCLR.reg = (1 << 18);
    PORT->Group[0].OUTSET.reg = (1 << 18);
    
    //PORT->Group[0].OUTCLR.reg = (1 << 16);
    //timer_start(5<<offset);
    timer_start(gamma[offset]/2);
    //PORT->Group[0].OUTSET.reg = (1 << 16);
}
