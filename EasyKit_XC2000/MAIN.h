//****************************************************************************
// @Module        Project Settings
// @Filename      MAIN.h
// @Project       EasyKit_XC2000.dav
//----------------------------------------------------------------------------
// @Controller    Infineon XC2238N-40F80
//
// @Compiler      TASKING VX-toolset
//
// @Codegenerator 2.1
//
// @Description   This file contains all function prototypes and macros for 
//                the MAIN module.
//
//----------------------------------------------------------------------------
// @Date          6/25/2018 15:56:05
//
//****************************************************************************

// USER CODE BEGIN (MAIN_Header,1)

// USER CODE END



#ifndef _MAIN_H_
#define _MAIN_H_

//****************************************************************************
// @Project Includes
//****************************************************************************

// USER CODE BEGIN (MAIN_Header,2)

// USER CODE END


//****************************************************************************
// @Macros
//****************************************************************************

// USER CODE BEGIN (MAIN_Header,3)

// USER CODE END


//****************************************************************************
// @Defines
//****************************************************************************
#define TASKING_VIPER

#define far  __far
#define _inline  inline
#define _nop  __nop
#define _interrupt  __interrupt


// USER CODE BEGIN (MAIN_Header,4)

// USER CODE END


//****************************************************************************
// @Typedefs
//****************************************************************************

// USER CODE BEGIN (MAIN_Header,5)

// USER CODE END


//****************************************************************************
// @Imported Global Variables
//****************************************************************************

// USER CODE BEGIN (MAIN_Header,6)

// USER CODE END


//****************************************************************************
// @Global Variables
//****************************************************************************

// USER CODE BEGIN (MAIN_Header,7)

// USER CODE END


//****************************************************************************
// @Prototypes Of Global Functions
//****************************************************************************


void MAIN_vUnlockProtecReg(void);

void MAIN_vLockProtecReg(void);

void MAIN_vChangeFreq(void);


// USER CODE BEGIN (MAIN_Header,8)

// USER CODE END


//****************************************************************************
// @Interrupt Vectors
//****************************************************************************

// USER CODE BEGIN (MAIN_Header,9)

// USER CODE END


//****************************************************************************
// @Project Includes
//****************************************************************************


#include  "XC22xxNREGS.h"
#include  "SCS.h"

#include  "IO.h"
#include  "GPT1.h"
#include  "GPT2.h"

// USER CODE BEGIN (MAIN_Header,10)
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
// USER CODE END


#endif  // ifndef _MAIN_H_
