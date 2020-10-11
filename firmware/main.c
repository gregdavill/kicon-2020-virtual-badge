#include <samd51.h>


void delay(int n)
{
    int i;

    for (;n >0; n--)
    {
        for (i=0;i<1000;i++)
            __asm("nop");
    }
}

int main()
{
    REG_PORT_DIRSET1 = (1 << 8);

    while(1)
    {
        REG_PORT_OUTSET1 = (1<<8);
        delay(200);
        REG_PORT_OUTCLR1 = (1<<8);
        delay(100);
    }
}
