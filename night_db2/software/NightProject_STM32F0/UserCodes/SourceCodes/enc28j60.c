/********************************************************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher 
 * Copyright: GPL V2
 * http://www.gnu.org/licenses/gpl.html
 *
 * Based on the enc28j60.c file from the AVRlib library by Pascal Stang.
 * For AVRlib See http://www.procyonengineering.com/
 * Used with explicit permission of Pascal Stang.
 *
 * Title: Microchip ENC28J60 Ethernet Interface Driver
 * Chip type           : ATMEGA88 with ENC28J60
 *******************************************************************************/

#include "appconfig.h"

#define 	ENC28J60_CSL()		SPI_CS_LOW()				/* ???? */
#define 	ENC28J60_CSH()		SPI_CS_HIGH()				/* ???? */

#define MIN(a,b) (a) < (b) ? (a) : (b)
static unsigned char Enc28j60Bank;
static unsigned int NextPacketPtr;


unsigned char enc28j60ReadOp(unsigned char op, unsigned char address)
{
    unsigned char dat = 0;
    
    ENC28J60_CSL();
    
    dat = op | (address & ADDR_MASK);
    SPI_SendByte(dat);
    dat = SPI_SendByte(0xFF);
    // do dummy read if needed (for mac and mii, see datasheet page 29)
    if(address & 0x80)
    {
        dat = SPI_SendByte(0xFF);
    }
    // release CS
    ENC28J60_CSH();
    return dat;
}

void enc28j60WriteOp(unsigned char op, unsigned char address, unsigned char data)
{
    unsigned char dat = 0;
    
    ENC28J60_CSL();
    // issue write command
    dat = op | (address & ADDR_MASK);
    SPI_SendByte(dat);
    // write data
    dat = data;
    SPI_SendByte(dat);
    ENC28J60_CSH();
}

void enc28j60ReadBuffer(unsigned int len, unsigned char* data)
{
    ENC28J60_CSL();
    // issue read command
    SPI_SendByte(ENC28J60_READ_BUF_MEM);
    while(len)
    {
        len--;
        // read data
        *data = (unsigned char)SPI_SendByte(0);
        data++;
    }
    *data='\0';
    ENC28J60_CSH();
}

void enc28j60WriteBuffer(unsigned int len, unsigned char* data)
{
    ENC28J60_CSL();
    // issue write command
    SPI_SendByte(ENC28J60_WRITE_BUF_MEM);
    
    while(len)
    {
        len--;
        SPI_SendByte(*data);
        data++;
    }
    ENC28J60_CSH();
}

void enc28j60SetBank(unsigned char address)
{
    // set the bank (if needed)
    if((address & BANK_MASK) != Enc28j60Bank)
    {
        // set the bank
        enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
        enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
        Enc28j60Bank = (address & BANK_MASK);
    }
}

unsigned char enc28j60Read(unsigned char address)
{
    // set the bank
    enc28j60SetBank(address);
    // do the read
    return enc28j60ReadOp(ENC28J60_READ_CTRL_REG, address);
}

void enc28j60Write(unsigned char address, unsigned char data)
{
    // set the bank
    enc28j60SetBank(address);
    // do the write
    enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
//	data = 0;
//	enc28j60WriteOp(ENC28J60_READ_CTRL_REG, address, data);
	
}

void enc28j60PhyWrite(unsigned char address, unsigned int data)
{
    // set the PHY register address
    enc28j60Write(MIREGADR, address);
    // write the PHY data
    enc28j60Write(MIWRL, data);
    enc28j60Write(MIWRH, data>>8);
    // wait until the PHY write completes
    while(enc28j60Read(MISTAT) & MISTAT_BUSY)
    {
        //Del_10us(1);
        //_nop_();
    }
}

void enc28j60clkout(unsigned char clk)
{
    //setup clkout: 2 is 12.5MHz:
    enc28j60Write(ECOCON, clk & 0x7);
}

void enc28j60Init(unsigned char* macaddr)
{   
    //uint16_t i;
    ENC28J60_CSH();	      
	
		// perform hardware reset
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);
		Sys_Delay_MS(10);
		GPIO_SetBits(GPIOB, GPIO_Pin_1);
		Sys_Delay_MS(20);

    // perform system reset
    enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
   
    // check CLKRDY bit to see if reset is complete
    // The CLKRDY does not work. See Rev. B4 Silicon Errata point. Just wait.
    //while(!(enc28j60Read(ESTAT) & ESTAT_CLKRDY));
    // do bank 0 stuff
    // initialize receive buffer
    // 16-bit transfers, must write low byte first
    // set receive buffer start address	  
	  Sys_Delay_MS(3);	  //gjk errata 2
	  
    NextPacketPtr = RXSTART_INIT;
    // Rx start    
    enc28j60Write(ERXSTL, RXSTART_INIT&0xFF);	 
    enc28j60Write(ERXSTH, RXSTART_INIT>>8);
    // set receive pointer address     
    enc28j60Write(ERXRDPTL, RXSTART_INIT&0xFF);
    enc28j60Write(ERXRDPTH, RXSTART_INIT>>8);
    // RX end
    enc28j60Write(ERXNDL, RXSTOP_INIT&0xFF);
    enc28j60Write(ERXNDH, RXSTOP_INIT>>8);
    // TX start	  1500
    enc28j60Write(ETXSTL, TXSTART_INIT&0xFF);
    enc28j60Write(ETXSTH, TXSTART_INIT>>8);
    // TX end
    enc28j60Write(ETXNDL, TXSTOP_INIT&0xFF);
    enc28j60Write(ETXNDH, TXSTOP_INIT>>8);
    // do bank 1 stuff, packet filter:
    // For broadcast packets we allow only ARP packtets
    // All other packets should be unicast only for our mac (MAADR)
    //
    // The pattern to match on is therefore
    // Type     ETH.DST
    // ARP      BROADCAST
    // 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
    // in binary these poitions are:11 0000 0011 1111
    // This is hex 303F->EPMM0=0x3f,EPMM1=0x30
    
   // enc28j60Write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
	enc28j60Write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_BCEN);  //?????DHCP
    enc28j60Write(EPMM0, 0x3f);
    enc28j60Write(EPMM1, 0x30);
    enc28j60Write(EPMCSL, 0xf9);
    enc28j60Write(EPMCSH, 0xf7);    
    enc28j60Write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
    // bring MAC out of reset 
    enc28j60Write(MACON2, 0x00);	  
    
    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX);
    // set inter-frame gap (non-back-to-back)

    enc28j60Write(MAIPGL, 0x12);
    enc28j60Write(MAIPGH, 0x0C);
    // set inter-frame gap (back-to-back)

    enc28j60Write(MABBIPG, 0x15);	 
    // Set the maximum packet size which the controller will accept
    // Do not send packets longer than MAX_FRAMELEN:
  
    enc28j60Write(MAMXFLL, MAX_FRAMELEN&0xFF);	
    enc28j60Write(MAMXFLH, MAX_FRAMELEN>>8);
    // do bank 3 stuff
    // write MAC address
    // NOTE: MAC address in ENC28J60 is byte-backward
    enc28j60Write(MAADR5, macaddr[0]);	
    enc28j60Write(MAADR4, macaddr[1]);
    enc28j60Write(MAADR3, macaddr[2]);
    enc28j60Write(MAADR2, macaddr[3]);
    enc28j60Write(MAADR1, macaddr[4]);
    enc28j60Write(MAADR0, macaddr[5]);

    //??PHY????  LEDB????
    enc28j60PhyWrite(PHCON1, PHCON1_PDPXMD);    
     /* PHY LED */	
    enc28j60PhyWrite(PHLCON,0x476);

    // no loopback of transmitted frames
    enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);

    // switch to bank 0    
    enc28j60SetBank(ECON1);

    // enable interrutps
 //   enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);

    // enable packet reception
    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
		
}

// read the revision of the chip:
unsigned char enc28j60getrev(void)
{
    //?EREVID ?????????? EREVID ??????
    //????,????5 ????,??????????
    //????
    return(enc28j60Read(EREVID));
}

void enc28j60PacketSend(unsigned int len, unsigned char* packet)
{
	 //enc28j60WriteOp(ENC28J60_BIT_FIELD_SET,ECON1, ECON1_TXRST);		   //by gjk
     //enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR,ECON1, ECON1_TXRST);		   //by gjk
     //enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR,EIR, EIR_TXERIF | EIR_TXIF); //by gjk
	 //enc28j60WriteOp(ENC28J60_BIT_FIELD_SET,ECON1, ECON1_TXRTS);		   //by gjk




    // Set the write pointer to start of transmit buffer area
	enc28j60Write(EWRPTL, (TXSTART_INIT)&0xFF);
	enc28j60Write(EWRPTH, (TXSTART_INIT)>>8);
	// Set the TXND pointer to correspond to the packet size given
	enc28j60Write(ETXNDL, (TXSTART_INIT+len)&0xFF);
	enc28j60Write(ETXNDH, (TXSTART_INIT+len)>>8);
	// write per-packet control byte (0x00 means use macon3 settings)
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
	// copy the packet into the transmit buffer
	enc28j60WriteBuffer(len, packet);
	// send the contents of the transmit buffer onto the network
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);

    if(enc28j60getrev() == 0x05u || enc28j60getrev() == 0x06u)
	{
		uint16_t AttemptCounter = 0x0000;
		while((enc28j60Read(EIR) & (EIR_TXERIF | EIR_TXIF)) && (++AttemptCounter < 1000));
		if((enc28j60Read(EIR) & EIR_TXERIF) || (AttemptCounter >= 1000))
		{
			WORD_VAL ReadPtrSave;
			WORD_VAL TXEnd;
			TXSTATUS TXStatus;
			uint8_t i;

			// Cancel the previous transmission if it has become stuck set
			//BFCReg(ECON1, ECON1_TXRTS);
            enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);

			// Save the current read pointer (controlled by application)
			ReadPtrSave.v[0] = enc28j60Read(ERDPTL);
			ReadPtrSave.v[1] = enc28j60Read(ERDPTH);

			// Get the location of the transmit status vector
			TXEnd.v[0] = enc28j60Read(ETXNDL);
			TXEnd.v[1] = enc28j60Read(ETXNDH);
			TXEnd.Val++;
			
			// Read the transmit status vector
			enc28j60Write(ERDPTL, TXEnd.v[0]);
			enc28j60Write(ERDPTH, TXEnd.v[1]);

			enc28j60ReadBuffer(sizeof(TXStatus) ,(uint8_t*)&TXStatus);

			// Implement retransmission if a late collision occured (this can 
			// happen on B5 when certain link pulses arrive at the same time 
			// as the transmission)
			for(i = 0; i < 16u; i++)
			{
				if((enc28j60Read(EIR) & EIR_TXERIF) && TXStatus.bits.LateCollision)
				{
					// Reset the TX logic
                    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
                    enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
                    enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXERIF | EIR_TXIF);

					// Transmit the packet again
					enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
					while(!(enc28j60Read(EIR) & (EIR_TXERIF | EIR_TXIF)));

					// Cancel the previous transmission if it has become stuck set
					enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);

					// Read transmit status vector
					enc28j60Write(ERDPTL, TXEnd.v[0]);
					enc28j60Write(ERDPTH, TXEnd.v[1]);
                    enc28j60ReadBuffer(sizeof(TXStatus) ,(uint8_t*)&TXStatus);
				}
				else
				{
					break;
				}
			}

			// Restore the current read pointer
			enc28j60Write(ERDPTL, ReadPtrSave.v[0]);
			enc28j60Write(ERDPTH, ReadPtrSave.v[1]);
		}
	}
    // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
	if( (enc28j60Read(EIR) & EIR_TXERIF) )
    {
        //enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
    }
}
// Gets a packet from the network receive buffer, if one is available.
// The packet will by headed by an ethernet header.
//      maxlen  The maximum acceptable length of a retrieved packet.
//      packet  Pointer where packet data should be stored.
// Returns: Packet length in bytes if a packet was retrieved, zero otherwise.
unsigned int enc28j60PacketReceive(unsigned int maxlen, unsigned char* packet)
{
 unsigned int rxstat;
    unsigned int len;
    
    // check if a packet has been received and buffered
    //if( !(enc28j60Read(EIR) & EIR_PKTIF) ){
    // The above does not work. See Rev. B4 Silicon Errata point 6.
    if( enc28j60Read(EPKTCNT) ==0 )  //???????????
    {
        return(0);
    }
    
    // Set the read pointer to the start of the received packet		 ??????
    enc28j60Write(ERDPTL, (NextPacketPtr));
    enc28j60Write(ERDPTH, (NextPacketPtr)>>8);
    
    // read the next packet pointer
    NextPacketPtr  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
    NextPacketPtr |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
    
    // read the packet length (see datasheet page 43)
    len  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
    len |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
    
    len-=4; //remove the CRC count
    // read the receive status (see datasheet page 43)
    rxstat  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
    rxstat |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
    // limit retrieve length
    if (len>maxlen)
    {
        len=maxlen;		//by gjk
    }
    
    // check CRC and symbol errors (see datasheet page 44, table 7-3):
    // The ERXFCON.CRCEN is set by default. Normally we should not
    // need to check this.
    if ((rxstat & 0x80)==0)
    {
        // invalid
        len=0;
    }
    else
    {
        // copy the packet from the receive buffer
        enc28j60ReadBuffer(len, packet);
    }
    // Move the RX read pointer to the start of the next received packet
    // This frees the memory we just read out
    enc28j60Write(ERXRDPTL, (NextPacketPtr));
    enc28j60Write(ERXRDPTH, (NextPacketPtr)>>8);
    
    // decrement the packet counter indicate we are done with this packet
    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
    return(len);
}
//*******************************************************************************************
//
// Function : icmp_send_request
// Description : Send ARP request packet to destination.
//
//*******************************************************************************************

unsigned int enc28j60_read_phyreg(unsigned char address)	 //by gjk 2013-02-20
{
	unsigned int mydat;
	
	// set the PHY register address
	enc28j60Write(MIREGADR, address);
	enc28j60Write(MICMD, MICMD_MIIRD);
	
	// Loop to wait until the PHY register has been read through the MII
	// This requires 10.24us
	while( (enc28j60Read(MISTAT) & MISTAT_BUSY) );
	
	// Stop reading
	//enc28j60Write(MICMD, MICMD_MIIRD);
	enc28j60Write(MICMD,0x00);	  /*by gjk 09/03/09  ??0x00*/
	// Obtain results and return
	mydat = enc28j60Read ( MIRDH );
	mydat<<=8;	 /*jerkoh090120*/
	mydat |= enc28j60Read ( MIRDL );
	

	return mydat;
}


//*******************************************************************************************
//
// Function : enc28j60_mac_is_linked
// Description : return MAC link status.
//
//*******************************************************************************************

unsigned char  enc28j60_mac_is_linked(void)		//by gjk 2013-02-20
{
	if ( (enc28j60_read_phyreg(PHSTAT1) & PHSTAT1_LLSTAT ) )
	   {    
		return 0;	    /*ok*/
	   }
	else
	   {	   
		return 1;	    /*error*/
	   }
}


void enc28j60RegDump(void)
{   			
	printf("RevID: 0x%x\n", enc28j60Read(EREVID));

	printf( ("Cntrl: ECON1 ECON2 ESTAT  EIR  EIE\n"));
	printf( ("         "));
	printf("%02x",enc28j60Read(ECON1));
	printf( ("    "));
	printf("%02x",enc28j60Read(ECON2));
	printf( ("    "));
	printf("%02x",enc28j60Read(ESTAT));
	printf( ("    "));
	printf("%02x",enc28j60Read(EIR));
	printf( ("   "));
	printf("%02x",enc28j60Read(EIE));
	printf( ("\n"));

	printf( ("MAC  : MACON1  MACON2  MACON3  MACON4  MAC-Address\n"));
	printf( ("        0x"));
	printf("%02x",enc28j60Read(MACON1));
	printf( ("    0x"));
	printf("%02x",enc28j60Read(MACON2));
	printf( ("    0x"));
	printf("%02x",enc28j60Read(MACON3));
	printf( ("    0x"));
	printf("%02x",enc28j60Read(MACON4));
	printf( ("   "));
	printf("%02x",enc28j60Read(MAADR5));
	printf("%02x",enc28j60Read(MAADR4));
	printf("%02x",enc28j60Read(MAADR3));
	printf("%02x",enc28j60Read(MAADR2));
	printf("%02x",enc28j60Read(MAADR1));
	printf("%02x",enc28j60Read(MAADR0));
	printf( ("\n"));

	printf( ("Rx   : ERXST  ERXND  ERXWRPT ERXRDPT ERXFCON EPKTCNT MAMXFL\n"));
	printf( ("       0x"));
	printf("%02x",enc28j60Read(ERXSTH));
	printf("%02x",enc28j60Read(ERXSTL));
	printf( (" 0x"));
	printf("%02x",enc28j60Read(ERXNDH));
	printf("%02x",enc28j60Read(ERXNDL));
	printf( ("  0x"));
	printf("%02x",enc28j60Read(ERXWRPTH));
	printf("%02x",enc28j60Read(ERXWRPTL));
	printf( ("  0x"));
	printf("%02x",enc28j60Read(ERXRDPTH));
	printf("%02x",enc28j60Read(ERXRDPTL));
	printf( ("   0x"));
	printf("%02x",enc28j60Read(ERXFCON));
	printf( ("    0x"));
	printf("%02x",enc28j60Read(EPKTCNT));
	printf( ("  0x"));
	printf("%02x",enc28j60Read(MAMXFLH));
	printf("%02x",enc28j60Read(MAMXFLL));
	printf( ("\n"));

	printf( ("Tx   : ETXST  ETXND  MACLCON1 MACLCON2 MAPHSUP\n"));
	printf( ("       0x"));
	printf("%02x",enc28j60Read(ETXSTH));
	printf("%02x",enc28j60Read(ETXSTL));
	printf( (" 0x"));
	printf("%02x",enc28j60Read(ETXNDH));
	printf("%02x",enc28j60Read(ETXNDL));
	printf( ("   0x"));
	printf("%02x",enc28j60Read(MACLCON1));
	printf( ("     0x"));
	printf("%02x",enc28j60Read(MACLCON2));
	printf( ("     0x"));
	printf("%02x",enc28j60Read(MAPHSUP));
	printf( ("\n"));
	printf( ("PHY   : PHCON1  PHCON2  PHSTAT1 PHSTAT2\n"));
	printf( ("       0x"));
	printf("%02x",enc28j60Read(PHCON1));//ist 16 bit breit nicht 8 !
	printf( ("     0x"));
    printf("%02x",enc28j60Read(PHCON2));//ist 16 bit breit nicht 8 !
    printf( ("     0x"));
    printf("%02x",enc28j60Read(PHSTAT1));//ist 16 bit breit nicht 8 !
	printf( ("     0x"));
    printf("%02x",enc28j60Read(PHSTAT2));//ist 16 bit breit nicht 8 !
    printf( ("\n"));

}

