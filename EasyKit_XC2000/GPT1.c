//****************************************************************************
// @Module        General Purpose Timer Unit (GPT1)
// @Filename      GPT1.c
// @Project       EasyKit_XC2000.dav
//----------------------------------------------------------------------------
// @Controller    Infineon XC2238N-40F80
//
// @Compiler      TASKING VX-toolset
//
// @Codegenerator 2.1
//
// @Description   This file contains functions that use the GPT1 module.
//
//----------------------------------------------------------------------------
// @Date          6/25/2018 15:56:06
//
//****************************************************************************

// USER CODE BEGIN (GPT1_General,1)

// USER CODE END



//****************************************************************************
// @Project Includes
//****************************************************************************

#include "MAIN.h"

// USER CODE BEGIN (GPT1_General,2)

// USER CODE END


//****************************************************************************
// @Macros
//****************************************************************************

// USER CODE BEGIN (GPT1_General,3)

// USER CODE END


//****************************************************************************
// @Defines
//****************************************************************************

// USER CODE BEGIN (GPT1_General,4)

// USER CODE END


//****************************************************************************
// @Typedefs
//****************************************************************************

// USER CODE BEGIN (GPT1_General,5)

// USER CODE END


//****************************************************************************
// @Imported Global Variables
//****************************************************************************

// USER CODE BEGIN (GPT1_General,6)

// USER CODE END


//****************************************************************************
// @Global Variables
//****************************************************************************

// USER CODE BEGIN (GPT1_General,7)

// USER CODE END


//****************************************************************************
// @External Prototypes
//****************************************************************************

// USER CODE BEGIN (GPT1_General,8)

// USER CODE END


//****************************************************************************
// @Prototypes Of Local Functions
//****************************************************************************

// USER CODE BEGIN (GPT1_General,9)

// USER CODE END


//****************************************************************************
// @Function      void GPT1_vInit(void) 
//
//----------------------------------------------------------------------------
// @Description   This is the initialization function of the GPT1 function 
//                library. It is assumed that the SFRs used by this library 
//                are in reset state. 
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

void GPT1_vInit(void)
{

  volatile unsigned int uwTemp;

  // USER CODE BEGIN (Init,2)

  // USER CODE END
  ///  -----------------------------------------------------------------------
  ///  Configuration of Timer Block Prescaler 1:
  ///  -----------------------------------------------------------------------
  GPT12E_KSCCFG  =  0x0003;      // Module Enable

  uwTemp         =  GPT12E_KSCCFG;  // dummy read to avoid pipeline effects

  ///  -----------------------------------------------------------------------
  ///  Configuration of Timer Block Prescaler 1:
  ///  -----------------------------------------------------------------------
  ///  - prescaler for timer block 1 is 16


  ///  -----------------------------------------------------------------------
  ///  Configuration of the GPT1 Core Timer 3:
  ///  -----------------------------------------------------------------------
  ///  - timer 3 works in timer mode
  ///  - external up/down control is disabled
  ///  - prescaler factor is 32
  ///  - up/down control bit is reset
  ///  - alternate output function T3OUT () is disabled
  ///  - timer 3 output toggle latch (T3OTL) is set to 0

  GPT12E_T3CON   =  0x1801;      // load timer 3 control register
  GPT12E_T3      =  0x0000;      // load timer 3 register

  ///  -----------------------------------------------------------------------
  ///  Configuration of the GPT1 Auxiliary Timer 2:
  ///  -----------------------------------------------------------------------
  ///  - timer 2 works in timer mode
  ///  - external up/down control is disabled
  ///  - prescaler factor is 16
  ///  - up/down control bit is reset
  ///  - timer 2 run bit is reset

  GPT12E_T2CON   =  0x0000;      // load timer 2 control register
  GPT12E_T2      =  0x0000;      // load timer 2 register

  ///  -----------------------------------------------------------------------
  ///  Configuration of the GPT1 Auxiliary Timer 4:
  ///  -----------------------------------------------------------------------
  ///  - timer 4 works in timer mode
  ///  - external up/down control is disabled
  ///  - prescaler factor is 16
  ///  - up/down control bit is reset
  ///  - timer 4 run bit is reset

  GPT12E_T4CON   =  0x0000;      // load timer 4 control register
  GPT12E_T4      =  0x0000;      // load timer 4 register

  ///  -----------------------------------------------------------------------
  ///  Configuration of the used GPT1 Port Pins:
  ///  -----------------------------------------------------------------------



  ///  -----------------------------------------------------------------------
  ///  Configuration of the used GPT1 Interrupts:
  ///  -----------------------------------------------------------------------

  // USER CODE BEGIN (GPT1_Function,3)

  // USER CODE END

  GPT12E_T3CON_T3R  =  1;        // set timer 3 run bit

} //  End of function GPT1_viTmr4




// USER CODE BEGIN (GPT1_General,10)

// USER CODE END

