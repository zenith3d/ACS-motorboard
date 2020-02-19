
void initCANBus() {
	/** 
	 * Function that initializes the CANBus according to the wiring and the needed speed (500kb/s).
	 * It also enable the CAN module interruptions
	**/

	// Enable CAN transmission
	DDRC |= 0x80;			// Configure the STANDBY pin as an output
	PORTC &= 0x7F; 			// Activate the MCP2562 (STANDBY to 0)

	// CAN General CONtrol register (see p.158)
	CANGCON = (1<<SWRES);		// Reset the CAN module
	CANGCON = 0x02; 		// Set the CAN module to 'enable' mode

	// CAN General Interrupt Enable register (see p.161)
	CANGIE |= (1<<ENIT) | (1<<ENRX);// Enable all interrupts, receive & transmit interrupts

	// CAN Bit Timing registers (see p.172/163)
	CANBT1 = 0x06;			// Baud rate prescaler (500kb/s)
	CANBT2 = 0x04;			// Propagation time segment
	CANBT3 = 0x13;  		// Compensate phase edge errors & filter noise (using 3 points sampling)

	// CAN Highest Priority MOb register
	CANHPMOB = 0x00; 		// Set no priority
}

void initCANMOBasReceiver(uint8_t mobNumber, uint32_t ID, uint8_t rtr) {
	/**
	 * Initialize a CAN MOb from 0 to 5 as a receiver. A MOB can be seen as a "CAN socket".
	**/

	// CAN Page mob register (see p.166)
	CANPAGE = (mobNumber << 4) & 0xF0;		// Selection of the MOb number

	// CAN IDentifier Tag registers (see p.169)
	if (rtr) CANIDT4 = (1 << RTRTAG);		// Configure as reception remote (rtr=1)
	else CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = (uint8_t)((ID & 0x00F) << 5);	// Configure the remote request identifier
	CANIDT1 = (uint8_t)(ID >> 3);			// Configure the remote request identifier

	// CAN IDentifier Mask registers (see p.170)
	if (rtr) CANIDM4 = (1 << RTRMSK);
	else CANIDM4 = 0x00;				// Enable bit comparison with the RTR
	CANIDM3 = 0xFF; 				// Enable bit comparison with the IDentifier
	CANIDM2 = 0xFF;					// (Interruption only if the received CAN message is a remote request with the correct identifier)
	CANIDM1 = 0xFF;

	// CAN MOb Control & DLC register (see p.168)
	CANCDMOB = 0x80; 				// Config MOB as reception
	 
	// CAN Enable Interrupt MOb registers (see p.162)
	CANIE2 |= (1 << mobNumber);			// Enable interrupt over MOb n°mobNumber
}

void initCANMOBasIDBandReceiver (uint8_t mobNumber, uint32_t BeginingID, uint32_t AreaSize, uint8_t rtr) {
	/**
	 * Initialize a CAN MOB from 0 to 5 as ID band receiver. A MOB can be seen as a "CAN socket" on different successive ID.
	**/

	// CAN Page mob register (see p.166)
	CANPAGE = (mobNumber << 4) & 0xF0;					// Selection of the MOb number

	// CAN IDentifier Tag registers (see p.169)
	if (rtr) CANIDT4 = (1 << RTRTAG);					// Configure as reception remote (rtr=1)
	else CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = (uint8_t)((BeginingID & 0x000F) << 5); 	// Configure the remote request identifier
	CANIDT1 = (uint8_t)(BeginingID >> 3);				// Configure the remote request identifier

	// CAN IDentifier Mask registers (see p.170)
	AreaSize = 0x00000000 - AreaSize;
	if (rtr) CANIDM4 = 0x00;
	else CANIDM4 = (1 << RTRMSK);	// Enable bit comparison with the RTR
	CANIDM3 = 0xFF; 				// Enable bit comparison with the IDentifier
	CANIDM2 = (uint8_t)((AreaSize & 0x000F) << 5);
	CANIDM1 = (uint8_t)(AreaSize >> 3);

	// CAN MOb Control & DLC register (see p.168)
	CANCDMOB = 0x80; 				// Config MOB as reception
	 
	// CAN Enable Interrupt MOb registers (see p.162)
	CANIE2 |= (1 << mobNumber);		// Enable interrupt over MOb n°mobNumber
}

void disableCANMOB (uint8_t mobNumber) {
	/**
	 * Reset a CAN MOB from 0 to 5.
	**/
	// CAN Page mob register (see p.166)
	CANPAGE = (mobNumber << 4) & 0xF0;	// Selection of the MOb number

	// CAN MOb Control & DLC register (see p.168)
	CANCDMOB = 0x00; 					// Config MOB as disabled
	  
	// CAN Enable Interrupt MOb registers (see p.162)
	switch (mobNumber) {				// Disable the interruptions over the proper MOB
		case 0 : CANIE2 &= 0xFE; break;
		case 1 : CANIE2 &= 0xFD; break;
		case 2 : CANIE2 &= 0xFB; break;
		case 3 : CANIE2 &= 0xF7; break;
		case 4 : CANIE2 &= 0xEF; break;
		case 5 : CANIE2 &= 0xDF; break;
		default : break;
	}
  }
 
void sendData(uint8_t mobNumber, uint8_t ID, uint8_t dlc, uint8_t* buffer) {
	/**
	 * Sending data through a CAN MOB from 0 to 5. 
	 * The Data Length Code (nb of bytes) is included between 1 and 8
	**/

	// CAN Page mob register (see p.166)
	CANPAGE = (mobNumber << 4) & 0xF0;	// Selection of the MOb number

	// CAN IDentifier Tag registers (see p.169)
	CANIDT4 = 0x00;								// Config as data (rtr = 0)
	CANIDT3 = 0x00;
	CANIDT2 = (uint8_t)((ID & 0x00F) << 5);		// Configure the identifier
	CANIDT1 = (uint8_t)(ID >> 3);				// Configure the identifier

	for (uint8_t i=0; i<dlc; i++) {
		CANPAGE &= 0xF0;			// Set the FIFO CAN Data Buffer
		CANPAGE |= i;				// Set the FIFO CAN Data Buffer Index
		CANMSG = buffer[i];
	}
	CANCDMOB = 0x40 | dlc;			// Enable transmission and specify the Data Length Code
	
	while ( !(CANSTMOB & (1 << TXOK)));
  }
