/**
  ******************************************************************************
  * @file	enc28j60.c
  * @author  A mix of different versions from Internet. Edited by Nightmare
  * @version V1.0.0
  * @date	2015.1.29
  * @brief   
  ******************************************************************************
  *
  ******************************************************************************
  */




#include "appconfig.h"

#define 	ENC28J60_CS_LOW()		SPI_CS_LOW()
#define 	ENC28J60_CS_HIGH()		SPI_CS_HIGH()

static uint8_t ENC28J60_Bank;
static uint32_t NextPacketPtr;


//Before this, SPI init need to be performed
void ENC28J60_Init(uint8_t* macaddr)
{   
	ENC28J60_CS_HIGH();

	// Perform system reset
	ENC28J60_OpWrite(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);

	// Work around E2: wait for at least 1ms, instead of checking CLKRDY	
	//while(!(ENC28J60_RegRead(ESTAT) & ESTAT_CLKRDY)); // Check CLKRDY bit to see if reset is complete	
	Sys_Delay_MS(5);
	
	// Do bank 0 stuff
	// initialize receive buffer
	// 16-bit transfers, must write low byte first
	// Set receive buffer start address	  	  
	NextPacketPtr = RXSTART_INIT;
	// Rx start	
	ENC28J60_RegWrite(ERXSTL, RXSTART_INIT&0xFF);	 
	ENC28J60_RegWrite(ERXSTH, RXSTART_INIT>>8);
	// Set receive pointer address	 
	ENC28J60_RegWrite(ERXRDPTL, RXSTART_INIT&0xFF);
	ENC28J60_RegWrite(ERXRDPTH, RXSTART_INIT>>8);
	// RX end
	ENC28J60_RegWrite(ERXNDL, RXSTOP_INIT&0xFF);
	ENC28J60_RegWrite(ERXNDH, RXSTOP_INIT>>8);
	// TX start	  1500
	ENC28J60_RegWrite(ETXSTL, TXSTART_INIT&0xFF);
	ENC28J60_RegWrite(ETXSTH, TXSTART_INIT>>8);
	// TX end
	ENC28J60_RegWrite(ETXNDL, TXSTOP_INIT&0xFF);
	ENC28J60_RegWrite(ETXNDH, TXSTOP_INIT>>8);
	// Do bank 1 stuff, packet filter:
	// For broadcast packets we allow only ARP packtets
	// All other packets should be unicast only for our mac (MAADR)
	//
	// The pattern to match on is therefore
	// Type	 ETH.DST
	// ARP	  BROADCAST
	// 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
	// in binary these poitions are:11 0000 0011 1111
	// This is hex 303F->EPMM0=0x3f,EPMM1=0x30
	
	// ENC28J60_RegWrite(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
	ENC28J60_RegWrite(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_BCEN);  //?????DHCP
	ENC28J60_RegWrite(EPMM0, 0x3f);
	ENC28J60_RegWrite(EPMM1, 0x30);
	ENC28J60_RegWrite(EPMCSL, 0xf9);
	ENC28J60_RegWrite(EPMCSH, 0xf7);	
	ENC28J60_RegWrite(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
	// bring MAC out of reset 
	ENC28J60_RegWrite(MACON2, 0x00);	  
	
	ENC28J60_OpWrite(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX);
	// Set inter-frame gap (non-back-to-back)

	ENC28J60_RegWrite(MAIPGL, 0x12);
	ENC28J60_RegWrite(MAIPGH, 0x0C);
	// Set inter-frame gap (back-to-back)

	ENC28J60_RegWrite(MABBIPG, 0x15);	 
	// Set the maximum packet size which the controller will accept
	// Do not send packets longer than MAX_FRAMELEN:
  
	ENC28J60_RegWrite(MAMXFLL, MAX_FRAMELEN&0xFF);	
	ENC28J60_RegWrite(MAMXFLH, MAX_FRAMELEN>>8);
	// Do bank 3 stuff
	// write MAC address
	// NOTE: MAC address in ENC28J60 is byte-backward
	ENC28J60_RegWrite(MAADR5, macaddr[0]);	
	ENC28J60_RegWrite(MAADR4, macaddr[1]);
	ENC28J60_RegWrite(MAADR3, macaddr[2]);
	ENC28J60_RegWrite(MAADR2, macaddr[3]);
	ENC28J60_RegWrite(MAADR1, macaddr[4]);
	ENC28J60_RegWrite(MAADR0, macaddr[5]);

	// PHY: Full-duplex, set LEDB
	ENC28J60_PhyWrite(PHCON1, PHCON1_PDPXMD);	
	// PHY LED	
	ENC28J60_PhyWrite(PHLCON,0x476);

	// Work around E9 & E10
	// Loop back mode not available, don't use it!
	// No loopback of transmitted frames
	ENC28J60_PhyWrite(PHCON2, PHCON2_HDLDIS);

	// switch to bank 0	
	ENC28J60_SetBank(ECON1);

	// enable interrutps
 //   ENC28J60_OpWrite(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);

	// enable packet reception
	ENC28J60_OpWrite(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
		
}



uint8_t ENC28J60_OpRead(uint8_t op, uint8_t address)
{
	uint8_t dat = 0;
	
	ENC28J60_CS_LOW();
	
	dat = op | (address & ADDR_MASK);
	SPI_SendByte(dat);
	dat = SPI_SendByte(0xFF);
	
	if(address & 0x80)
	{
		dat = SPI_SendByte(0xFF);
	}
	
	ENC28J60_CS_HIGH();
	return dat;
}

void ENC28J60_OpWrite(uint8_t op, uint8_t address, uint8_t data)
{
	uint8_t dat = 0;
	
	ENC28J60_CS_LOW();
	
	dat = op | (address & ADDR_MASK);
	SPI_SendByte(dat);
	
	dat = data;
	SPI_SendByte(dat);
	
	ENC28J60_CS_HIGH();
}

void ENC28J60_BufferRead(uint32_t len, uint8_t* data)
{
	ENC28J60_CS_LOW();
	// issue read command
	SPI_SendByte(ENC28J60_READ_BUF_MEM);
	while(len)
	{
		len--;
		// read data
		*data = (uint8_t)SPI_SendByte(0);
		data++;
	}
	*data='\0';
	ENC28J60_CS_HIGH();
}

void ENC28J60_BufferWrite(uint32_t len, uint8_t* data)
{
	ENC28J60_CS_LOW();
	// issue write command
	SPI_SendByte(ENC28J60_WRITE_BUF_MEM);
	
	while(len)
	{
		len--;
		SPI_SendByte(*data);
		data++;
	}
	ENC28J60_CS_HIGH();
}

void ENC28J60_SetBank(uint8_t address)
{
	// Set the bank (if needed)
	if((address & BANK_MASK) != ENC28J60_Bank)
	{
		// Set the bank
		ENC28J60_OpWrite(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
		ENC28J60_OpWrite(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
		ENC28J60_Bank = (address & BANK_MASK);
	}
}

uint8_t ENC28J60_RegRead(uint8_t address)
{
	// Set the bank
	ENC28J60_SetBank(address);
	// Do the read
	return ENC28J60_OpRead(ENC28J60_READ_CTRL_REG, address);
}

void ENC28J60_RegWrite(uint8_t address, uint8_t data)
{
	// Set the bank
	ENC28J60_SetBank(address);
	// Do the write
	ENC28J60_OpWrite(ENC28J60_WRITE_CTRL_REG, address, data);	
}

void ENC28J60_PhyWrite(uint8_t address, uint32_t data)
{
	// Set the PHY register address
	ENC28J60_RegWrite(MIREGADR, address);
	// write the PHY data
	ENC28J60_RegWrite(MIWRL, data);
	ENC28J60_RegWrite(MIWRH, data>>8);
	// wait until the PHY write completes
	while(ENC28J60_RegRead(MISTAT) & MISTAT_BUSY)
	{
		//Del_10us(1);
		//_nop_();
	}
}

uint32_t ENC28J60_PhyRead(uint8_t address)	 //by gjk 2013-02-20
{
	uint32_t mydat;
	
	// Set the PHY register address
	ENC28J60_RegWrite(MIREGADR, address);
	ENC28J60_RegWrite(MICMD, MICMD_MIIRD);
	
	// Loop to wait until the PHY register has been read through the MII
	// This requires 10.24us
	while( (ENC28J60_RegRead(MISTAT) & MISTAT_BUSY) );
	
	// Stop reading
	//ENC28J60_RegWrite(MICMD, MICMD_MIIRD);
	ENC28J60_RegWrite(MICMD,0x00);	  /*by gjk 09/03/09  ??0x00*/
	// Obtain results and return
	mydat = ENC28J60_RegRead ( MIRDH );
	mydat<<=8;	 /*jerkoh090120*/
	mydat |= ENC28J60_RegRead ( MIRDL );
	

	return mydat;
}


void ENC28J60_PacketSend(uint8_t* packet, uint32_t len)
{
	// Set the write pointer to start of transmit buffer area
	ENC28J60_RegWrite(EWRPTL, (TXSTART_INIT)&0xFF);
	ENC28J60_RegWrite(EWRPTH, (TXSTART_INIT)>>8);
	// Set the TXND pointer to correspond to the packet size given
	ENC28J60_RegWrite(ETXNDL, (TXSTART_INIT+len)&0xFF);
	ENC28J60_RegWrite(ETXNDH, (TXSTART_INIT+len)>>8);
	// write per-packet control byte (0x00 means use macon3 settings)
	ENC28J60_OpWrite(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
	// copy the packet into the transmit buffer
	ENC28J60_BufferWrite(len, packet);
	// send the contents of the transmit buffer onto the network
	ENC28J60_OpWrite(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);

	if(ENC28J60_GetVersion() == 0x05u || ENC28J60_GetVersion() == 0x06u)
	{
		uint16_t AttemptCounter = 0x0000;
		while((ENC28J60_RegRead(EIR) & (EIR_TXERIF | EIR_TXIF)) && (++AttemptCounter < 1000));
		if((ENC28J60_RegRead(EIR) & EIR_TXERIF) || (AttemptCounter >= 1000))
		{
			WORD_VAL ReadPtrSave;
			WORD_VAL TXEnd;
			TXSTATUS TXStatus;
			uint8_t i;

			// Cancel the previous transmission if it has become stuck set
			//BFCReg(ECON1, ECON1_TXRTS);
			ENC28J60_OpWrite(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);

			// Save the current read pointer (controlled by application)
			ReadPtrSave.v[0] = ENC28J60_RegRead(ERDPTL);
			ReadPtrSave.v[1] = ENC28J60_RegRead(ERDPTH);

			// Get the location of the transmit status vector
			TXEnd.v[0] = ENC28J60_RegRead(ETXNDL);
			TXEnd.v[1] = ENC28J60_RegRead(ETXNDH);
			TXEnd.Val++;
			
			// Read the transmit status vector
			ENC28J60_RegWrite(ERDPTL, TXEnd.v[0]);
			ENC28J60_RegWrite(ERDPTH, TXEnd.v[1]);

			ENC28J60_BufferRead(sizeof(TXStatus) ,(uint8_t*)&TXStatus);

			// Implement retransmission if a late collision occured (this can 
			// happen on B5 when certain link pulses arrive at the same time 
			// as the transmission)
			for(i = 0; i < 16u; i++)
			{
				if((ENC28J60_RegRead(EIR) & EIR_TXERIF) && TXStatus.bits.LateCollision)
				{
					// Reset the TX logic
					ENC28J60_OpWrite(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
					ENC28J60_OpWrite(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
					ENC28J60_OpWrite(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXERIF | EIR_TXIF);

					// Transmit the packet again
					ENC28J60_OpWrite(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
					while(!(ENC28J60_RegRead(EIR) & (EIR_TXERIF | EIR_TXIF)));

					// Cancel the previous transmission if it has become stuck set
					ENC28J60_OpWrite(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);

					// Read transmit status vector
					ENC28J60_RegWrite(ERDPTL, TXEnd.v[0]);
					ENC28J60_RegWrite(ERDPTH, TXEnd.v[1]);
					ENC28J60_BufferRead(sizeof(TXStatus) ,(uint8_t*)&TXStatus);
				}
				else
				{
					break;
				}
			}

			// Restore the current read pointer
			ENC28J60_RegWrite(ERDPTL, ReadPtrSave.v[0]);
			ENC28J60_RegWrite(ERDPTH, ReadPtrSave.v[1]);
		}
	}
	// Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
	if( (ENC28J60_RegRead(EIR) & EIR_TXERIF) )
	{
		//ENC28J60_OpWrite(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
		ENC28J60_OpWrite(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
	}
}


// Gets a packet from the network receive buffer, if one is available.
// The packet will by headed by an ethernet header.
// maxlen  The maximum acceptable length of a retrieved packet.
// packet  Pointer where packet data should be stored.
// Returns: Packet length in bytes if a packet was retrieved, zero otherwise.
uint32_t ENC28J60_PacketReceive(uint8_t* packet, uint32_t maxlen)
{
	uint32_t rxstat;
	uint32_t len;

	// Work around E6
	// check if a packet has been received and buffered
	// if( !(ENC28J60_RegRead(EIR) & EIR_PKTIF) ){
	// The above does not work. See Rev. B4 Silicon Errata point 6.
	if( ENC28J60_RegRead(EPKTCNT) ==0 )
	{
		return(0);
	}
	
	// Set the read pointer to the start of the received packet
	ENC28J60_RegWrite(ERDPTL, (NextPacketPtr));
	ENC28J60_RegWrite(ERDPTH, (NextPacketPtr)>>8);
	
	// read the next packet pointer
	NextPacketPtr  = ENC28J60_OpRead(ENC28J60_READ_BUF_MEM, 0);
	NextPacketPtr |= ENC28J60_OpRead(ENC28J60_READ_BUF_MEM, 0)<<8;
	
	// read the packet length (see datasheet page 43)
	len  = ENC28J60_OpRead(ENC28J60_READ_BUF_MEM, 0);
	len |= ENC28J60_OpRead(ENC28J60_READ_BUF_MEM, 0)<<8;
	
	len-=4; //remove the CRC count
	// read the receive status (see datasheet page 43)
	rxstat  = ENC28J60_OpRead(ENC28J60_READ_BUF_MEM, 0);
	rxstat |= ENC28J60_OpRead(ENC28J60_READ_BUF_MEM, 0)<<8;
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
		ENC28J60_BufferRead(len, packet);
	}
	// Move the RX read pointer to the start of the next received packet
	// This frees the memory we just read out
	ENC28J60_RegWrite(ERXRDPTL, (NextPacketPtr));
	ENC28J60_RegWrite(ERXRDPTH, (NextPacketPtr)>>8);
	
	// decrement the packet counter indicate we are done with this packet
	ENC28J60_OpWrite(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	return(len);
}


//*******************************************************************************************
//
// Extended Functions
//
//*******************************************************************************************

// read the revision of the chip:
uint8_t ENC28J60_GetVersion(void)
{
	return(ENC28J60_RegRead(EREVID));
}

//E4 No output on CLKOUT in Power Save mode
void ENC28J60_SetExtClock(uint8_t clk)
{
	//setup clkout: 2 is 12.5MHz:
	ENC28J60_RegWrite(ECOCON, clk & 0x7);
}


uint8_t ENC28J60_GetLinkStatus(void)
{
	if ((ENC28J60_PhyRead(PHSTAT1) & PHSTAT1_LLSTAT))
		return ENC_LINK_UP;
	else
		return ENC_LINK_DOWN;
}

#if 0
void ENC28J60_RegDump(void)
{   			
	printf("RevID: 0x%x\n", ENC28J60_RegRead(EREVID));

	printf( ("Cntrl: ECON1 ECON2 ESTAT  EIR  EIE\n"));
	printf( ("		 "));
	printf("%02x",ENC28J60_RegRead(ECON1));
	printf( ("	"));
	printf("%02x",ENC28J60_RegRead(ECON2));
	printf( ("	"));
	printf("%02x",ENC28J60_RegRead(ESTAT));
	printf( ("	"));
	printf("%02x",ENC28J60_RegRead(EIR));
	printf( ("   "));
	printf("%02x",ENC28J60_RegRead(EIE));
	printf( ("\n"));

	printf( ("MAC  : MACON1  MACON2  MACON3  MACON4  MAC-Address\n"));
	printf( ("		0x"));
	printf("%02x",ENC28J60_RegRead(MACON1));
	printf( ("	0x"));
	printf("%02x",ENC28J60_RegRead(MACON2));
	printf( ("	0x"));
	printf("%02x",ENC28J60_RegRead(MACON3));
	printf( ("	0x"));
	printf("%02x",ENC28J60_RegRead(MACON4));
	printf( ("   "));
	printf("%02x",ENC28J60_RegRead(MAADR5));
	printf("%02x",ENC28J60_RegRead(MAADR4));
	printf("%02x",ENC28J60_RegRead(MAADR3));
	printf("%02x",ENC28J60_RegRead(MAADR2));
	printf("%02x",ENC28J60_RegRead(MAADR1));
	printf("%02x",ENC28J60_RegRead(MAADR0));
	printf( ("\n"));

	printf( ("Rx   : ERXST  ERXND  ERXWRPT ERXRDPT ERXFCON EPKTCNT MAMXFL\n"));
	printf( ("	   0x"));
	printf("%02x",ENC28J60_RegRead(ERXSTH));
	printf("%02x",ENC28J60_RegRead(ERXSTL));
	printf( (" 0x"));
	printf("%02x",ENC28J60_RegRead(ERXNDH));
	printf("%02x",ENC28J60_RegRead(ERXNDL));
	printf( ("  0x"));
	printf("%02x",ENC28J60_RegRead(ERXWRPTH));
	printf("%02x",ENC28J60_RegRead(ERXWRPTL));
	printf( ("  0x"));
	printf("%02x",ENC28J60_RegRead(ERXRDPTH));
	printf("%02x",ENC28J60_RegRead(ERXRDPTL));
	printf( ("   0x"));
	printf("%02x",ENC28J60_RegRead(ERXFCON));
	printf( ("	0x"));
	printf("%02x",ENC28J60_RegRead(EPKTCNT));
	printf( ("  0x"));
	printf("%02x",ENC28J60_RegRead(MAMXFLH));
	printf("%02x",ENC28J60_RegRead(MAMXFLL));
	printf( ("\n"));

	printf( ("Tx   : ETXST  ETXND  MACLCON1 MACLCON2 MAPHSUP\n"));
	printf( ("	   0x"));
	printf("%02x",ENC28J60_RegRead(ETXSTH));
	printf("%02x",ENC28J60_RegRead(ETXSTL));
	printf( (" 0x"));
	printf("%02x",ENC28J60_RegRead(ETXNDH));
	printf("%02x",ENC28J60_RegRead(ETXNDL));
	printf( ("   0x"));
	printf("%02x",ENC28J60_RegRead(MACLCON1));
	printf( ("	 0x"));
	printf("%02x",ENC28J60_RegRead(MACLCON2));
	printf( ("	 0x"));
	printf("%02x",ENC28J60_RegRead(MAPHSUP));
	printf( ("\n"));
	printf( ("PHY   : PHCON1  PHCON2  PHSTAT1 PHSTAT2\n"));
	printf( ("	   0x"));
	printf("%02x",ENC28J60_RegRead(PHCON1));//ist 16 bit breit nicht 8 !
	printf( ("	 0x"));
	printf("%02x",ENC28J60_RegRead(PHCON2));//ist 16 bit breit nicht 8 !
	printf( ("	 0x"));
	printf("%02x",ENC28J60_RegRead(PHSTAT1));//ist 16 bit breit nicht 8 !
	printf( ("	 0x"));
	printf("%02x",ENC28J60_RegRead(PHSTAT2));//ist 16 bit breit nicht 8 !
	printf( ("\n"));

}
#endif
