#include "sx127x_lora.h"

DigitalOut myled(LED1);

SPI spi(D11, D12, D13); // mosi, miso, sclk
//           dio0, dio1, nss, spi, rst
SX127x radio(  D2,   D3, D10, spi, A0); // sx1276 arduino shield

SX127x_lora lora(radio);
DigitalInOut rfsw(A4);    // for SX1276 arduino shield

void rfsw_callback()
{
    if (radio.RegOpMode.bits.Mode == RF_OPMODE_TRANSMITTER)
        rfsw = 1;
    else
        rfsw = 0;
}
/**********************************************************************/

int main()
{   
    printf("\r\nreset-rx\r\n");
    radio.rf_switch = rfsw_callback;
    
    radio.set_frf_MHz(915.0);
    lora.enable();
    lora.setBw_KHz(125);
    lora.setSf(7);
    
    /* RFO or PABOOST choice:
     * SX1276 shield: RFO if using 900MHz, or PA_BOOST if using 433MHz
     */
    rfsw.input();
    if (rfsw.read()) {
        printf("LAS\r\n");
        /* LAS HF=PA_BOOST  LF=RFO */
        if (radio.HF)
            radio.RegPaConfig.bits.PaSelect = 1;
        else
            radio.RegPaConfig.bits.PaSelect = 0;
    } else {
        /* MAS shield board, only RFO TX */
        radio.RegPaConfig.bits.PaSelect = 0;
        printf("MAS\r\n");
    }
    rfsw.output();
    radio.write_reg(REG_PACONFIG, radio.RegPaConfig.octet);
                
    lora.start_rx(RF_OPMODE_RECEIVER);
    
    for (;;) {     
        if (lora.service() == SERVICE_READ_FIFO) {
            int i;
            myled.write(!myled.read()); // toggle LED
            for (i = 0; i < lora.RegRxNbBytes; i++) {
                printf("%02x ", radio.rx_buf[i]);
            }
            printf("\r\n");
        }
    }
}
