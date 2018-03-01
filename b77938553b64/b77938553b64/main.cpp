#include "sx127x_lora.h"

SPI spi(D11, D12, D13); // mosi, miso, sclk
//           dio0, dio1, nss, spi, rst
SX127x radio(  D2,   D3, D10, spi, A0); // sx1276 arduino shield

SX127x_lora lora(radio);
DigitalInOut rfsw(A4);    // for SX1276 arduino shield

void rfsw_callback()
{
    if (radio.RegOpMode.bits.Mode == RF_OPMODE_TRANSMITTER) {
        rfsw = 1;
    } else {
        rfsw = 0;
    }
}
/**********************************************************************/

int main()
{
    uint8_t seq = 0;
    
    printf("\r\nreset-tx ");
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
                
    /* constant payload length of one byte */
    lora.RegPayloadLength = 1;
    radio.write_reg(REG_LR_PAYLOADLENGTH, lora.RegPayloadLength);
    
    for (;;) {       
        radio.tx_buf[0] = seq;  /* set payload */
        lora.start_tx(lora.RegPayloadLength);   /* begin transmission */
        
        printf("sent\r\n");
        while (lora.service() != SERVICE_TX_DONE)   /* wait for transmission to complete */
            ;
        printf("got-tx-done\r\n");
        
        radio.set_opmode(RF_OPMODE_STANDBY);
        wait(0.5);  /* throttle sending rate */
        seq++;  /* change payload */
    }
}

