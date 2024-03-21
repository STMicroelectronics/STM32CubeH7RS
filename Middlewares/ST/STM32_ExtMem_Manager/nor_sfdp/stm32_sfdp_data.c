/**
  ******************************************************************************
  * @file    stm32_sfpd.c
  * @author  MCD Application Team
  * @brief   This file includes a driver for SFPD support
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32_extmem_conf.h"

#if EXTMEM_DRIVER_NOR_SFDP == 1
#include "stm32_sfdp_data.h"
#if EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL != 0 && defined(EXTMEM_MACRO_DEBUG)
#include <stdio.h>
#endif /* EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL != 0 && defined(EXTMEM_MACRO_DEBUG) */
#include <string.h>

/** @defgroup NOR_SFDP_DATA Data module 
  * @ingroup NOR_SFDP
  * @{
  */

/* Private defines ------------------------------------------------------------*/
/** @defgroup NOR_SFDP_DATA_Private_define Private defines
  * this group contains all the private define of the sfdp data module.
  * @{
  */

/** @brief true definition
 *
 * definition of the value true and false 
 */
#define EXTMEM_SFDP_TRUE  1u      /**< true value */

/** @brief false definition
 *
 * definition of the value true and false 
 */
#define EXTMEM_SFDP_FALSE 0u      /**< false value */

/**
 * @brief clock definition 200Mhz
 */
#define CLOCK_200MHZ   200000000u
/**
 * @brief clock definition 166Mhz
 */
#define CLOCK_166MHZ   166000000u
/**
 * @brief clock definition 133Mhz
 */
#define CLOCK_133MHZ   133000000u
/**
 * @brief clock definition 100Mhz
 */
#define CLOCK_100MHZ   100000000u

/**
 * @brief SFDP command
 */
#define SFDP_COMMAND               0x5AU

/**
 * @brief SFDP signature
 */
#define SFDP_SIGNATURE             0x50444653U

/**
 * @brief SFDP header size
 */
#define SFDP_HEADER_SIZE              8U

/**
 * @brief SFDP param header size
 */
#define SFDP_PARAM_HEADER_SIZE        8U

/**
 * @brief SFDP basic table default size
 */
#define SFPD_PARAMS_BASIC_TABLE_DEFAULTSIZE  16u

/**
 * @brief SFDP param table ID
 */
typedef enum {    
   SFPD_PARAMID_UNKNOWN                                     = 0x00000u,
   SFPD_PARAMID_VENDOR                                      = 0x00001u,
   SFPD_PARAMID_FUNCTION_VENDOR                             = 0x00002u,
   SFPD_PARAMID_FUNCTION_JEDEC                              = 0x00004u,
   SFPD_PARAMID_BASIC_SPIPROTOCOL                           = 0x00008u,
   SFPD_PARAMID_SECTORMAP                                   = 0x00010u,
   SFPD_PARAMID_RPMC                                        = 0x00020u,
   SFPD_PARAMID_4BYTE_ADDRESS_INSTRUCTION                   = 0x00040u,
   SFPD_PARAMID_XSPI_V1_0                                   = 0x00080u,
   SFPD_PARAMID_XSPI_V2_0                                   = 0x00100u,
   SFPD_PARAMID_STATUS_CONTROL_CONFIG_REGISTER_MAP          = 0x00200u,
   SFPD_PARAMID_STATUS_CONTROL_CONFIG_REGISTER_MAP_MULTICHIP= 0x00400u,
   SFPD_PARAMID_STATUS_CONTROL_CONFIG_XSPI_V2_0             = 0x00800u,
   SFPD_PARAMID_OCTAL_DDR                                   = 0x01000u,
   SFPD_PARAMID_MSPT                                        = 0x02000u,
   SFPD_PARAMID_X4QUAD_DS                                   = 0x04000u,
   SFPD_PARAMID_QUAD_DDR                                    = 0x08000u,
   SFPD_PARAMID_SECURE_PACKET_READ_WRITE                    = 0x10000u,
   SFPD_PARAMID_RESERVED                                    = 0x20000u
} SFDP_ParamID_TypeDef;

/**
 * @brief DEBUG macro string
 */
#if EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL > 3 && defined(EXTMEM_MACRO_DEBUG)
#define SFDP_DEBUG_STR(_STR_)     do{                              \
                                  EXTMEM_MACRO_DEBUG("\t\tSFDP::");\
                                  EXTMEM_MACRO_DEBUG(_STR_);       \
                                  EXTMEM_MACRO_DEBUG("\n");        \
                              }while(0);
#else
#define SFDP_DEBUG_STR(_STR_)
#endif /* */

/**
 * @brief DEBUG macro string + integer value
 */
#if EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL > 3 && defined(EXTMEM_MACRO_DEBUG)
#define SFDP_DEBUG_INT(_STR_,_INT_) do {                                          \
                                   char int_char[50];                             \
                                   EXTMEM_MACRO_DEBUG("\t\tSFDP::");              \
                                   (void)sprintf(int_char,"%s0x%x",_STR_, _INT_); \
                                   EXTMEM_MACRO_DEBUG((uint8_t *)int_char);       \
                                   EXTMEM_MACRO_DEBUG("\n");                      \
                              }while(0);
#else
#define SFDP_DEBUG_INT(_STR_,_INT_)
#endif

/**
 * @brief DEBUG macro for function call
 */
#define CHECK_FUNCTION_CALL(_FUNC_)  do{                         \
                                     retr = _FUNC_;              \
                                     if ( EXTMEM_SFDP_OK != retr)\
                                     {                           \
                                       goto error;               \
                                     }                           \
                                     }while(0);

/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/** @defgroup NOR_SFDP_DATA_Private_TypeDefs Private typedefs
  * @{
  */

/**
 * @brief SFDP header definition
 */
typedef struct {
  uint8_t ID_lsb;                 /*!< ID lsb */
  uint8_t Minor_revision;         /*!< minor revision */
  uint8_t Major_revision;         /*!< major revision */
  uint8_t Length;                 /*!< Length */
  uint8_t TableAddressPointer[3]; /*!< address of the table pointer */
  uint8_t ID_msb;                 /*!< ID msb */
} SFDP_ParameterHeaderTypeDef;

/**
 * @brief SFDP param definition
 */
typedef struct {
  SFDP_ParamID_TypeDef type;  /*!< list of the param table available */
  uint32_t address;           /*!< address of the table */
  uint8_t size;               /*!< size of the table */
} SFDP_ParameterTableTypeDef;

/**
 * @brief SFDP JEDEC Basic Params definition
 */
typedef struct {
  uint32_t size;
  union {
    uint8_t data_BYTE[23*4];  /*!< data in bytes format */
    uint32_t data_DWORD[23];  /*!< data in DWORD format */
    struct {
      struct {
      uint32_t BlockSectorEraseSizes:2;
      uint32_t WriteGranularity:1;
      uint32_t VolatileRegisterProtect:1;
      uint32_t WriteEnableInstructionVolatileRegister:1;
      uint32_t :3;
      uint32_t _4KEraseInstruction:8;
      uint32_t Support_1S1S2SFastRead:1;
      uint32_t AddressBytes:2;
      uint32_t Support_DTR:1;
      uint32_t Support_1S2S2SFastRead:1;
      uint32_t Support_1S4S4SFastRead:1;
      uint32_t Support_1S1S4SFastRead:1;
      uint32_t :8;
      } D1;
      struct {
        uint32_t FlashSize;
      } D2;

      /* Fast read */
      struct {
        uint32_t _1S4S4S_DummyClock:5;
        uint32_t _1S4S4S_ModeClock:3;
        uint32_t _1S4S4S_FastReadInstruction:8;
        uint32_t _1S1S4S_DummyClock:5;
        uint32_t _1S1S4S_ModeClock:3;
        uint32_t _1S1S4S_FastReadInstruction:8;
      } D3;
      struct {
        uint32_t _1S1S2S_DummyClock:5;
        uint32_t _1S1S2S_ModeClock:3;
        uint32_t _1S1S2S_FastReadInstruction:8;
        uint32_t _1S2S2S_DummyClock:5;
        uint32_t _1S2S2S_ModeClock:3;
        uint32_t _1S2S2S_FastReadInstruction:8;
      } D4;
      struct {
        uint32_t _2S2S2S_FastReadSupport:1;
      uint32_t :3;
      uint32_t _4S4S4S_FastReadSupport:1;
      uint32_t :26;
      } D5;
      struct {
      uint32_t :16;
      uint32_t _2S2S2S_DummyClock:5;
      uint32_t _2S2S2S_ModeClock:3;
      uint32_t _2S2S2S_FastReadInstruction:8;
      } D6;
      struct {
      uint32_t :16;
      uint32_t _4S4S4S_DummyClock:5;
      uint32_t _4S4S4S_ModeClock:3;
      uint32_t _4S4S4S_FastReadInstruction:8;
      } D7;

      /* Erase */
      struct {
        uint32_t EraseType1_Size:8;
        uint32_t EraseType1_Instruction:8;
        uint32_t EraseType2_Size:8;
        uint32_t EraseType2_Instruction:8;
      } D8;
      struct {
        uint32_t EraseType3_Size:8;
        uint32_t EraseType3_Instruction:8;
        uint32_t EraseType4_Size:8;
        uint32_t EraseType4_Instruction:8;
      } D9;

      /* Timing */
      struct {
        uint32_t MutliplierEraseTime:4;
        uint32_t EraseType1_TypicalTime_count:5;
        uint32_t EraseType1_TypicalTime_units:2;
        uint32_t EraseType2_TypicalTime_count:5;
        uint32_t EraseType2_TypicalTime_units:2;
        uint32_t EraseType3_TypicalTime_count:5;
        uint32_t EraseType3_TypicalTime_units:2;
        uint32_t EraseType4_TypicalTime_count:5;
        uint32_t EraseType4_TypicalTime_units:2;
      } D10;
      struct {
        uint32_t MutliplierProgamTime:4;
        uint32_t PageSize:4;
        uint32_t PageProgram_TypicalTime:6;
        uint32_t ByteProgram_TypicalTime:5;
        uint32_t AddByteProgram_TypicalTime:5;

        uint32_t ChipErase_TypicalTime_count:5;   /* 28:24 count */
        uint32_t ChipErase_TypicalTime_units:2;   /* 30:29 units (00b: 16 ms, 01b: 256 ms, 10b: 4 s, 11b: 64 s) */

      uint32_t :1;
      } D11;
      struct {
        uint32_t ProhibitedOpDuringProgramSuspend:4;
        uint32_t ProhibitedOpDuringEraseSuspend:4;
      uint32_t :1;
      uint32_t ProgramResumeToSuspendInterval:4;
      uint32_t SuspendInProgress_ProgramMaxLatency:7;
      uint32_t EraseResumeToSuspendInterval:4;
      uint32_t SuspendInProgress_EraseMaxLatency:7;
      uint32_t :1;
      } D12;
      struct {
        uint32_t ProgramResume_Intruction:8;
        uint32_t ProgramSuspend_Intruction:8;
        uint32_t Resume_Intruction:8;
        uint32_t Suspend_Intruction:8;
      } D13;
      struct {
      uint32_t :2;
      uint32_t StatusRegister:6;
      uint32_t ExitDeepPowerdownDelay:7;
      uint32_t ExitDeepPowerdown_Instruction:8;
      uint32_t EnterDeepPowerdown_Instruction:8;
      uint32_t DeepPowerdown_Support:1;
      } D14;
      struct {
        uint32_t _4S4S4S_DisableSequence:4;
        uint32_t _4S4S4S_EnableSequence:5;
        uint32_t _0S4S4S_Support:1;
        uint32_t _0S4S4S_ExitMethod:6;
        uint32_t _0S4S4S_EntryMethod:4;
        uint32_t QuadEnableRequierment:3;
        uint32_t HoldORReset_Disable:1;
      uint32_t :8;
      } D15;
      struct {
        uint32_t VolatileNonVolatileRegister_WriteEnable:7;
        uint32_t :1;
        uint32_t SoftResetRescueSequence_Support:6;
        uint32_t Exit4ByteAddressing:10;
        uint32_t Enter4ByteAddressing:8;
      } D16;
      /* Added one for octal management, this part depends the information size of the flash device */
      struct {
        uint32_t _1S8S8S_DummyClock:5;
        uint32_t _1S8S8S_ModeClock:3;
        uint32_t _1S8S8S_FastReadInstruction:8;
        uint32_t _1S1S8S_DummyClock:5;
        uint32_t _1S1S8S_ModeClock:3;
        uint32_t _1S1S8S_FastReadInstruction:8;
      } D17;
      struct {
      uint32_t :18;
      uint32_t VariableOutputDriverStrength:5;
      uint32_t JEDECSPIProtocolReset:1;
      uint32_t DataStrobeSTRMode:2;
      uint32_t DataSTrobeQPISTRMode:1;
      uint32_t DataSTrobeQPIDTRMode:1;
      uint32_t :1;
      uint32_t OctalDTRCommandExtension:2;
      uint32_t OctalByteOrder:1;
      } D18;
      struct {
        uint32_t _8s8s8s_DisableSequence:4;
        uint32_t _8s8s8s_EnableSequence:5;
        uint32_t _8S8S8S_Support:1;
        uint32_t _8S8S8S_ExitMethod:6;
        uint32_t _8S8S8S_EnterMethod:4;
        uint32_t OctalRequirement:3;
      uint32_t :9;
      } D19;
      struct {
        uint32_t _4S4S4S_MaximunSpeedWithoutStrobe:4; /* 1111b: not supported, 1110b: not characterized , 1010b: Reserved
                                                         1100b: 400 MHz*, 1011b: 333 MHz*, 1010b: 266 MHz*, 1001b: 250 MHz*
                                                         1000b: 200 MHz,  0111b: 166 MHz,  0110b: 133 MHz , 0101b: 100 MHz
                                                         0100b: 80 MHz,   0011b: 66 MHz ,  0010b: 50 MHz  , 0001b: 33 MHz */
        uint32_t _4S4S4S_MaximunSpeedWithStrobe:4;
        uint32_t _4S4D4D_MaximunSpeedWithoutStrobe:4;
        uint32_t _4S4D4D_MaximunSpeedWithStrobe:4;
        uint32_t _8S8S8S_MaximunSpeedWithoutStrobe:4;
        uint32_t _8S8S8S_MaximunSpeedWithStrobe:4;
        uint32_t _8D8D8D_MaximunSpeedWithoutStrobe:4;
        uint32_t _8D8D8D_MaximunSpeedWithStrobe:4;
      } D20;
      struct {
        uint32_t _1S1D1D_FastReadSupport:1;
        uint32_t _1S2D2D_FastReadSupport:1;
        uint32_t _1S4D4D_FastReadSupport:1;
        uint32_t _4S4D4D_FastReadSupport:1;
      uint32_t :28;
      } D21;
      struct {
        uint32_t _1S1D1D_DummyClock:5;
        uint32_t _1S1D1D_ModeClock:3;
        uint32_t _1S1D1D_FastReadInstruction:8;
        uint32_t _1S2D2D_DummyClock:5;
        uint32_t _1S2D2D_ModeClock:3;
        uint32_t _1S2D2D_FastReadInstruction:8;
      } D22;
      struct {
        uint32_t _1S4D4D_DummyClock:5;
        uint32_t _1S4D4D_ModeClock:3;
        uint32_t _1S4D4D_FastReadInstruction:8;
        uint32_t _4S4D4D_DummyClock:5;
        uint32_t _4S4D4D_ModeClock:3;
        uint32_t _4S4D4D_FastReadInstruction:8;
      } D23;
    }Param_DWORD;
  } Params;
}SFDP_JEDECBasic_Params;

/**
 * @brief SFDP JEDEC 4ByteAddress Params definition
 */
typedef union {
  uint8_t data_BYTE[2*4]; /*!< data in BYTE format */
  uint32_t data_DWORD[2]; /*!< data in DWORD format */
  struct {
    struct {
      uint32_t Support_1S1S1S_ReadCommand:1;       /*!< Instruction=13h */
      uint32_t Support_1S1S1S_FastReadCommand:1;   /*!< Instruction=0Ch */
      uint32_t Support_1S1S2S_FastReadCommand:1;   /*!< Instruction=3Ch */
      uint32_t Support_1S2S2S_FastReadCommand:1;   /*!< Instruction=BCh */
      uint32_t Support_1S1S4S_FastReadCommand:1;   /*!< Instruction=6Ch */
      uint32_t Support_1S4S4S_FastReadCommand:1;   /*!< Instruction=ECh */
      uint32_t Support_1S1S1S_PageProgramCommand:1;/*!< Instruction=12h */
      uint32_t Support_1S1S4S_PageProgramCommand:1;/*!< Instruction=34h */
      uint32_t Support_1S4S4S_PageProgramCommand:1;/*!< Instruction=3Eh */
      uint32_t Support_EraseCommandType1size:1;    /*!< support of erase type1 */
      uint32_t Support_EraseCommandType2size:1;    /*!< Instruction lookup in next Dword */
      uint32_t Support_EraseCommandType3size:1;    /*!< Instruction lookup in next Dword */
      uint32_t Support_EraseCommandType4size:1;    /*!< Instruction lookup in next Dword */
      uint32_t Support_1S1D1D_DTRReadCommand:1;    /*!< Instruction=0Eh */
      uint32_t Support_1S2D2D_DTRReadCommand:1;    /*!< Instruction=BEh */
      uint32_t Support_1S4D4D_DTRReadCommand:1;    /*!< Instruction=EEh */
      uint32_t Support_VolatileIndividualSectorLockReadCommand:1;  /* Instruction=E0h */
      uint32_t Support_VolatileIndividualSectorLockWriteCommand:1; /* Instruction=E1h */
      uint32_t Support_NVolatileIndividualSectorLockReadCommand:1; /* Instruction=E2h */
      uint32_t Support_NVolatileIndividualSectorLockWriteCommand:1;/* Instruction=E3h */
      uint32_t Support_1S1S8S_FastReadCommand:1;   /* Instruction=7Ch */
      uint32_t Support_1S8S8S_FastReadCommand:1;   /* Instruction=CCh */
      uint32_t Support_1S8D8D_DTRReadCommand:1;    /* Instruction=FDh */
      uint32_t Support_1S1S8S_PageProgramCommand:1;/* Instruction=84h */
      uint32_t Support_1S8S8S_PageProgramCommand:1;/* Instruction=8Eh */
      uint32_t :7;
    }D1;
    struct {
      uint32_t InstructionEraseType1:8;  /* common usage is:21h for   4 kbyte erase*/
      uint32_t InstructionEraseType2:8;  /* common usage is:5Ch for  32 kbyte erase*/
      uint32_t InstructionEraseType3:8;  /* common usage is:DCh for  64 kbyte erase*/
      uint32_t InstructionEraseType4:8;  /* common usage is:DCh for 256 kbyte erase*/
    }D2;
  }Param_DWORD;
} SFDP_JEDEC4ByteAddress_Params;


/**
 * @brief SFDP JEDEC XSPI10 Params definition
 */
typedef union {
  uint8_t  data_BYTE[6*4];     /*!< data in BYTE format */
  uint32_t data_DWORD[6];      /*!< data in DWORD format */
  struct {
    /* Command Codes used in 8D-8D-8D protocol mode */
    struct {
      uint32_t ReadFastWrapCommand:8; /* 0 means not supported */
      uint32_t ReadFastCommand:8;
      uint32_t :6;
      uint32_t NumberOfDataBytesForWriteRegisterCommand:1;
      uint32_t NumberOfAdditionnalModifierBytesForWriteRegisterCommand:1;
      uint32_t NumberOfAdditionnalModifierBytesForStatusConfigRegisterCommand:1;
      uint32_t InitialLaLatencyForReadNonVolatileRegisterCommand:1;
      uint32_t InitialLatencyForReadVolatileRegisterCommand:1;
      uint32_t NumberOfAdditionalModifierBytesForReadRegisterCommand:1;
      uint32_t InitialLatencyForReadStatusRegisterCommand:1;
      uint32_t NumberOfAdditionalModifierBytesForReadStatusRegisterCommand:1;
      uint32_t SFDPCommand_8D8D8DMode_DummyCycles:1;
      uint32_t SFDPCommand_8D8D8DMode_AddressBytes:1;
    } D1;
    /* Commands Codes used in 8D-8D-8D protocol mode */
    struct {
      uint32_t WriteNVolatileRegisterCommand:8;
      uint32_t WriteVolatileRegisterCommand:8;
      uint32_t ReadNVolatileRegisterCommand:8;
      uint32_t ReadVolatileRegisterCommand:8;
    } D2;
    /* Memory Commands in 8D-8D-8D protocol mode */
    struct {
      uint32_t :10;
      uint32_t EnterDefaultProtocolMode_support:1;
      uint32_t SoftResetAndEnterDefaultProtocolMode_support:1;
      uint32_t ResetEnable_support:1;
      uint32_t SoftReset_support:1;
      uint32_t ExitDeepPowerDown_support:1;
      uint32_t EnterDeepPowerDown_support:1;
      uint32_t WriteNVolatileRegister_support:1;
      uint32_t WriteVolatileRegister_support:1;
      uint32_t WriteRegister_support:1;
      uint32_t ClearFlagStatusReg_support:1;
      uint32_t WriteStatusConfigurationRegister_support:1;
      uint32_t ReadNVolatileRegister_support:1;
      uint32_t ReadVolatileRegister_support:1;
      uint32_t ReadRegister_support:1;
      uint32_t ReadFlagStatusRegister_support:1;
      uint32_t ReadConfigurationRegister_support:1;
      uint32_t EraseChip_support:1;
      uint32_t Erase_32Kbytes_support:1;
      uint32_t Erase_4Kbytes_support:1;
      uint32_t SetupReadWrap_support:1;
      uint32_t ReadFastWrap_support:1;
      uint32_t ReadSFDP_8D8D8D:1;
    } D3;  /* this naming to be aligned with the specification */
    /* Dummy cycles used for various frequencies */
    struct {
      uint32_t :2;
      uint32_t Operation200Mhz_ConfigPattern:5;
      uint32_t Operation200Mhz_DummyCycle:5;
      uint32_t :20;
    } D4;
    /* Dummy cycles used for various frequencies */
    struct {
      uint32_t :2;
      uint32_t Operation100Mhz_ConfigPattern:5;
      uint32_t Operation100Mhz_DummyCycle:5;
      uint32_t Operation133Mhz_ConfigPattern:5;
      uint32_t Operation133Mhz_DummyCycle:5;
      uint32_t Operation166Mhz_ConfigPattern:5;
      uint32_t Operation166Mhz_DummyCycle:5;
    } D5;
    /* Default Dummy cycles after POR */
    struct {
      uint32_t _8D8D8DDefaultPOR_DummyCycle:5;
      uint32_t _8S8S8SDefaultPOR_DummyCycle:5;
      uint32_t :22;
    } D6;

  } Param_DWORD;
} SFPD_JEDEC_XSPI10; /* contains the command codes used in 8D-8D-8D protocol mode */

/**
 * @brief SFDP JEDEC octal ddr params definition
 */
typedef union {
  uint8_t  data_BYTE[8*4]; /*!< data in BYTE format */
  uint32_t data_DWORD[8];  /*!< data in DWORD format */
  struct {
    /* 1st DWORD - First Command Sequence */
    struct {
      uint32_t Byte3CommandSequence:8;
      uint32_t Byte2CommandSequence:8;
      uint32_t Byte1CommandSequence:8;
      uint32_t LenghtCommand:8;
    } D1;
    /* 2nd DWORD - First Command Sequence (continued)*/
    struct {
      uint32_t Byte7CommandSequence:8;
      uint32_t Byte6CommandSequence:8;
      uint32_t Byte5CommandSequence:8;
      uint32_t Byte4CommandSequence:8;
    } D2;
    /* 1st DWORD - Second Command Sequence */
    struct {
      uint32_t Byte3CommandSequence:8;
      uint32_t Byte2CommandSequence:8;
      uint32_t Byte1CommandSequence:8;
      uint32_t LenghtCommand:8;
    } D3;
    /* 2nd DWORD - Second Command Sequence (continued)*/
    struct {
      uint32_t Byte7CommandSequence:8;
      uint32_t Byte6CommandSequence:8;
      uint32_t Byte5CommandSequence:8;
      uint32_t Byte4CommandSequence:8;
    } D4;
    /* 1st DWORD - third Command Sequence */
    struct {
      uint32_t Byte3CommandSequence:8;
      uint32_t Byte2CommandSequence:8;
      uint32_t Byte1CommandSequence:8;
      uint32_t LenghtCommand:8;
    } D5;
    /* 2nd DWORD - third Command Sequence (continued)*/
    struct {
      uint32_t Byte7CommandSequence:8;
      uint32_t Byte6CommandSequence:8;
      uint32_t Byte5CommandSequence:8;
      uint32_t Byte4CommandSequence:8;
    } D6;
    /* 1st DWORD - fourth Command Sequence */
    struct {
      uint32_t Byte3CommandSequence:8;
      uint32_t Byte2CommandSequence:8;
      uint32_t Byte1CommandSequence:8;
      uint32_t LenghtCommand:8;
    } D7;
    /* 2nd DWORD - fourth Command Sequence (continued)*/
    struct {
      uint32_t Byte7CommandSequence:8;
      uint32_t Byte6CommandSequence:8;
      uint32_t Byte5CommandSequence:8;
      uint32_t Byte4CommandSequence:8;
    } D8;
  } Param_DWORD;
} SFPD_JEDEC_OCTALDDR; /* contains the command codes used in 8D-8D-8D protocol mode */

/**
 * @brief SFDP JEDEC SCCR Params definition
 */
typedef union {
  uint8_t  data_b[28*4];     /*!< data in byte format */
  uint32_t data_DWORD[28];   /*!< data in DWORD format */
  struct {

    uint32_t AddressOffsetVolatileRegister;
    uint32_t AddressOffsetNonVolatileRegister;


    /* Generic Addressable Read/Write Status/Control register commands for volatile registers */
    struct {
      uint32_t NbDummyCyclesReadStatusControlRegisterCommandForVolatileRegisters_1S1S1S:4;
      uint32_t :2;
      uint32_t NbDummyCyclesReadRegisterCmdVolatileRegister_8D8D8D_mode:4;
      uint32_t NbDummyCyclesReadRegisterCmdVolatileRegister_8S8S8S_mode:4;
      uint32_t NbDummyCyclesReadRegisterCmdVolatileRegister_4S4D4D_mode:4;
      uint32_t NbDummyCyclesReadRegisterCmdVolatileRegister_4S4S4S_mode:4;
      uint32_t NbDummyCyclesReadRegisterCmdVolatileRegister_2S2S2S_mode:4;
      uint32_t NbDummyCyclesReadRegisterCmdVolatileRegister_1S1S1S_mode:2;

      uint32_t NbAddressBytesGenericAddressableReadWriteStatusControlRegisterCommandsVolatileRegisters:2;
      uint32_t GenericAddressableWriteRegisterCmdVolatileRegister_support:1;
      uint32_t GenericAddressableReadRegisterCmdVolatileRegister_support:1;
    } D3;
    /* Generic Addressable Read/Write Status/Control register commands for non-volatile registers */
    struct {
      uint32_t NbDummyCyclesReadStatusControlRegisterCommandForNVolatileRegisters_1S1S1S:4;
      uint32_t :2;
      uint32_t NbDummyCyclesReadRegisterCmdNonVolatileRegister_8D8D8D_mode:4;
      uint32_t NbDummyCyclesReadRegisterCmdNonVolatileRegister_8S8S8S_mode:4;
      uint32_t NbDummyCyclesReadRegisterCmdNonVolatileRegister_4S4D4D_mode:4;
      uint32_t NbDummyCyclesReadRegisterCmdNonVolatileRegister_4S4S4S_mode:4;
      uint32_t NbDummyCyclesReadRegisterCmdNonVolatileRegister_2S2S2S_mode:4;
      uint32_t NbDummyCyclesReadRegisterCmdNonVolatileRegister_1S1S1S_mode:2;

      uint32_t NbAddressBytesGenericAddressableReadWriteStatusControlRegisterCommandsNVolatileRegisters:2;
      uint32_t GenericAddressableWriteRegisterCmdNonVolatileRegister_support:1;
      uint32_t GenericAddressableReadRegisterCmdNonVolatileRegister_support:1;
    } D4;
    /* WIP (Required for xSPI) */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  WIPBitLocationRegister:3;
      uint32_t LocalAddressForWIP:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t WIPpolarity:1;
      uint32_t WIPBitAvailable:1;
    } D5;
    /* WEL */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  WELBitLocationRegister:3;
      uint32_t WELLocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t WELWriteAccessBit:1;
      uint32_t WELpolarity:1;
      uint32_t WELBitAvailable:1;
    } D6;
    /* Program Error */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t Sharing:1;
      uint32_t Polarity:1;
      uint32_t BitAvailable:1;
    } D7;
    /* Erase Error */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t Sharing:1;
      uint32_t Polarity:1;
      uint32_t BitAvailable:1;
    } D8;
    /* 9th DWORD * Variable Dummy Cycle Settings * Volatile Register */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationLSBPhysicalBitsRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t NumberBitsUsedToSetWaitStates:2;
      uint32_t BitAvailable:1;
    } D9;
    /* 10th DWORD * Variable Dummy Cycle Settings * Non Volatile Register */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationLSBPhysicalBitsRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t NumberBitsUsedToSetWaitStates:2;
      uint32_t BitAvailable:1;
    } D10;
    /* 11th DWORD * Variable Dummy Cycle Settings * bit patterns */
    struct {
      uint32_t :2;
      uint32_t BitPatternUsedToSet_22DummyCycles:5;
      uint32_t Support_22DummyCycles:1;
      uint32_t BitPatternUsedToSet_24DummyCycles:5;
      uint32_t Support_24DummyCycles:1;
      uint32_t BitPatternUsedToSet_26DummyCycles:5;
      uint32_t Support_26DummyCycles:1;
      uint32_t BitPatternUsedToSet_28DummyCycles:5;
      uint32_t Support_28DummyCycles:1;
      uint32_t BitPatternUsedToSet_30DummyCycles:5;
      uint32_t Support_30DummyCycles:1;
    } D11;
    /* 12th DWORD * Variable Dummy Cycle Settings * bit patterns */
    struct {
      uint32_t :2;
      uint32_t BitPatternUsedToSet_12DummyCycles:5;
      uint32_t Support_12DummyCycles:1;
      uint32_t BitPatternUsedToSet_14DummyCycles:5;
      uint32_t Support_14DummyCycles:1;
      uint32_t BitPatternUsedToSet_16DummyCycles:5;
      uint32_t Support_16DummyCycles:1;
      uint32_t BitPatternUsedToSet_18DummyCycles:5;
      uint32_t Support_18DummyCycles:1;
      uint32_t BitPatternUsedToSet_20DummyCycles:5;
      uint32_t Support_20DummyCycles:1;
    } D12;
    /* 13th DWORD * Variable Dummy Cycle Settings * bit patterns */
    struct {
      uint32_t :2;
      uint32_t BitPatternUsedToSet_2DummyCycles:5;
      uint32_t Support_2DummyCycles:1;
      uint32_t BitPatternUsedToSet_4DummyCycles:5;
      uint32_t Support_4DummyCycles:1;
      uint32_t BitPatternUsedToSet_6DummyCycles:5;
      uint32_t Support_6DummyCycles:1;
      uint32_t BitPatternUsedToSet_8DummyCycles:5;
      uint32_t Support_8DummyCycles:1;
      uint32_t BitPatternUsedToSet_10DummyCycles:5;
      uint32_t Support_10DummyCycles:1;
    } D13;
    /* 14th DWORD * QPI Mode Enable Volatile */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t BitPolarity:1;
      uint32_t BitAvailable:1;
    } D14;
    /* 15th DWORD * QPI Mode Enable - Non Volatile */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t BitPolarity:1;
      uint32_t BitAvailable:1;
    } D15;
    /* 16th DWORD * Octal Mode Enable Volatile */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t BitPolarity:1;
      uint32_t BitAvailable:1;
    } D16;
    /* 17th DWORD * Octal Mode Enable - Non Volatile */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t BitPolarity:1;
      uint32_t BitAvailable:1;
    } D17;
    /* 18th DWORD * STR or DTR mode select * Volatile */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t BitPolarity:1;
      uint32_t BitAvailable:1;
    } D18;
    /* 19th DWORD * STR or DTR mode select * Non Volatile */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t BitPolarity:1;
      uint32_t BitAvailable:1;
    } D19;
    /* 20th DWORD * STR Octal Mode Enable * Volatile */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t BitPolarity:1;
      uint32_t BitAvailable:1;
    } D20;
    /* 21th DWORD * STR Octal Mode Enable * Non Volatile */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t BitPolarity:1;
      uint32_t BitAvailable:1;
    } D21;
    /* 22th DWORD * DTR Octal Mode Enable * Volatile */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t BitPolarity:1;
      uint32_t BitAvailable:1;
    } D22;
    /* 23th DWORD * DTR Octal Mode Enable * Non Volatile */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t BitPolarity:1;
      uint32_t BitAvailable:1;
    } D23;
    /* 24th DWORD * DPD Status */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t BitPolarity:1;
      uint32_t BitAvailable:1;
    } D24;
    /* 25th DWORD * UDPD Status */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t BitPolarity:1;
      uint32_t BitAvailable:1;
    } D25;
    /* 26th DWORD * Output Driver Strength - Volatile */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t BitPolarity:1;
      uint32_t BitAvailable:1;
    } D26;
    /* 27th DWORD * Output Driver Strength - Non Volatile */
    struct {
      uint32_t CommandWriteAccess:8;
      uint32_t CommandReadAccess:8;
      uint32_t AdressRegisterOrModesSupported:8;  /*  If Bit 28 is 1: Address of register where bit is located
                                                      Local address AAAA-AAAA
                                                  If Bit 28 is 0: Modes supported and dummy cycles used for direct command
                                                 */
      uint8_t  BitLocationRegister:3;
      uint32_t LocalAddress:1;
      uint32_t BitAccessedByCommandsUsingAddress:1;
      uint32_t :1;
      uint32_t BitPolarity:1;
      uint32_t BitAvailable:1;
    } D27;
    /* 27th DWORD * Output Driver Strength - Non Volatile */
    struct {
      uint32_t :16;
      uint32_t BitPatternSupportDriverType4 :3;
      uint32_t BitPatternSupportDriverType3 :3;
      uint32_t BitPatternSupportDriverType2 :3;
      uint32_t BitPatternSupportDriverType1 :3;
      uint32_t BitPatternSupportDriverType0 :3;
    } D28;


  } Param_DWORD;
} SFPD_JEDEC_SCCR_Map; /* contains the command codes used in 8D-8D-8D protocol mode */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup NOR_SFDP_DATA_Private_Variables Private Variables
  * @{
  */
/**
 * @brief this variable contains all the table available on a memory
 */
static SFDP_ParameterTableTypeDef     sfdp_param_info[10];

/**
 * @brief this variable contains the JEDEC basic table info
 */
static SFDP_JEDECBasic_Params         JEDEC_Basic = {0};

/**
 * @brief this variable contains the JEDEC address 4bit table info
 */
static SFDP_JEDEC4ByteAddress_Params  JEDEC_Address4Bit = {0};

/**
 * @brief this variable contains the JEDEC XSPIV1.0 table info
 */
static SFPD_JEDEC_XSPI10              JEDEC_XSPI10;

/**
 * @brief this variable contains the JEDEC SCCR table info
 */
static SFPD_JEDEC_SCCR_Map            JEDEC_SCCR_Map;

/**
 * @brief this variable contains the JEDEC octal DDR table info
 */
static SFPD_JEDEC_OCTALDDR            JEDEC_OctalDdr;

/**
 * @brief this variable contains the JEDEC header info
 */
static SFPD_HeaderTypeDef             JEDEC_SFDP_Header;




/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup NOR_SFDP_DATA_Private_Functions Private Functions
  * @{
  */
SFDP_StatusTypeDef sfdp_read_header(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object, SFPD_HeaderTypeDef *sfdp_header);
SFDP_StatusTypeDef sfdp_get_paraminfo(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object, uint32_t sfdp_adress, SFDP_ParameterTableTypeDef *Param_info);
SFDP_StatusTypeDef sfpd_enter_octal_mode(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object);
uint32_t sfdp_getfrequencevalue(uint32_t BitField);
SFDP_StatusTypeDef sfpd_set_dummycycle(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object, uint32_t Value);
/**
  * @}
  */

/** @defgroup NOR_SFDP_DATA_Exported_Functions Exported Functions
  * @{
  */

SFDP_StatusTypeDef SFDP_GetHeader(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object)
{
  SFDP_StatusTypeDef retr;
  SFDP_DEBUG_STR(__func__);
  int8_t status;
  uint8_t index_config = 0u;
  SAL_XSPI_PhysicalLinkTypeDef table_config[] = { PHY_LINK_1S1S1S, PHY_LINK_2S2S2S, PHY_LINK_4S4S4S, PHY_LINK_4S4D4D, PHY_LINK_8D8D8D };

  /* loop to find the link config of the memory */
  do {
    /* variable status = 0 means exit the loop */
    status = 0;

    /* Set the command mode */
    SFDP_DEBUG_STR("try a command configuration");
    Object->sfpd_private.DriverInfo.SpiPhyLink = table_config[index_config];
    index_config = index_config + 1u;
    if (HAL_OK != SAL_XSPI_MemoryConfig(&Object->sfpd_private.SALObject, PARAM_PHY_LINK, &Object->sfpd_private.DriverInfo.SpiPhyLink))
    {
      retr = EXTMEM_SFDP_ERROR_NOTYETHANDLED;
    }
    else
    {
      /* get the sfdp header */
      retr = sfdp_read_header(Object, &JEDEC_SFDP_Header);
      if (retr != EXTMEM_SFDP_OK) 
      {
        status = 1;
      }
    }
  } while((status == 1) && (index_config < 5u));

  return retr;
}

SFDP_StatusTypeDef SFDP_CollectData(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object)
{
  SFDP_StatusTypeDef retr = EXTMEM_SFDP_OK;
  uint32_t sfdp_adress = SFDP_HEADER_SIZE;
  SFDP_DEBUG_STR(__func__);

  /* reset the table mask */
  Object->sfpd_private.Sfdp_table_mask = 0;

  /* reset the param info */
  (void)memset(sfdp_param_info, 0x0, sizeof(sfdp_param_info));

  /* get the table param info */
  for(uint8_t index = 0u; index <  (JEDEC_SFDP_Header.param_number + 1u); index++)
  {
    CHECK_FUNCTION_CALL(sfdp_get_paraminfo(Object, sfdp_adress, &sfdp_param_info[index]))
    Object->sfpd_private.Sfdp_table_mask |= (uint32_t)sfdp_param_info[index].type;
    sfdp_adress+= SFDP_PARAM_HEADER_SIZE;
  }

  /* Read each table param to extract the information to build the driver */
  for (uint8_t index = 0u;  sfdp_param_info[index].type != SFPD_PARAMID_UNKNOWN; index++)
  {
    uint8_t *ptr = NULL;
    uint32_t size = sfdp_param_info[index].size;
    switch(sfdp_param_info[index].type)
    {
    case SFPD_PARAMID_BASIC_SPIPROTOCOL:
      JEDEC_Basic.size = sfdp_param_info[index].size;
      ptr = JEDEC_Basic.Params.data_BYTE;
      break;
    case SFPD_PARAMID_4BYTE_ADDRESS_INSTRUCTION:
      ptr = JEDEC_Address4Bit.data_BYTE;
      break;
    case SFPD_PARAMID_STATUS_CONTROL_CONFIG_REGISTER_MAP:
      ptr = JEDEC_SCCR_Map.data_b;
      break;
    case SFPD_PARAMID_XSPI_V1_0:
      ptr = JEDEC_XSPI10.data_BYTE;
      break;
    case SFPD_PARAMID_OCTAL_DDR:
      ptr = JEDEC_OctalDdr.data_BYTE;
      break;
    default :
      SFDP_DEBUG_STR("the table is not yet handled by the SW");
      break;
    }
    if (ptr != NULL)
    {
      if (HAL_OK != SAL_XSPI_GetSFDP(&Object->sfpd_private.SALObject, sfdp_param_info[index].address, ptr, size * 4u))
      {
        retr = EXTMEM_SFDP_ERROR_DRIVER;
        goto error;
      }
    }

    if (SFPD_PARAMID_BASIC_SPIPROTOCOL == sfdp_param_info[index].type)
    {
      /* save data about the reset procedure */
      Object->sfpd_private.Reset_info = JEDEC_Basic.Params.Param_DWORD.D16.SoftResetRescueSequence_Support;
    }
  }

error:
  return retr;
}

SFDP_StatusTypeDef SFDP_MemoryReset(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object)
{
  typedef enum {
    RESET_NONE,
    RESET_Fh_4DATA_8CLOCK,
    RESET_Fh_4DATA_10CLOCK,
    RESET_Fh_4DATA_16CLOCK,
    RESET_INSTRUCTION_F0,
    RESET_INSTRUCTION_66_99,
    RESET_ERROR
  } RESET_METHOD;

  SFDP_StatusTypeDef retr = EXTMEM_SFDP_ERROR_NO_PARAMTABLE_BASIC;
  RESET_METHOD reset_methode = RESET_ERROR;
  uint32_t sfdp_adress = SFDP_HEADER_SIZE;
  uint8_t find = 0u;
  SFDP_DEBUG_STR(__func__);

  /* get the table param info */
  for(uint8_t index = 0u; index <  (JEDEC_SFDP_Header.param_number + 1u); index++)
  {
    retr = sfdp_get_paraminfo(Object, sfdp_adress, &sfdp_param_info[0]);
    if (EXTMEM_SFDP_OK == retr)
    {
      /* check if the table is basic table */        
      if (SFPD_PARAMID_BASIC_SPIPROTOCOL == sfdp_param_info[0].type)
      {
        /* read the JEDEC basic param */
        if (HAL_OK != SAL_XSPI_GetSFDP(&Object->sfpd_private.SALObject, 
                                       sfdp_param_info[0].address, 
                                       JEDEC_Basic.Params.data_BYTE, 
                                       ((uint32_t)sfdp_param_info[0].size) * 4u))
        {
          retr = EXTMEM_SFDP_ERROR_DRIVER;
        }
        else
        {
          retr = EXTMEM_SFDP_OK;
          find = 1u;
        }
      }
    }

    if ((EXTMEM_SFDP_OK != retr) || (1u == find))
    {
        /* stop the read, if there is an error or if the table has been found */
        break;
    }
    /* look for the next table */        
    sfdp_adress+= SFDP_PARAM_HEADER_SIZE;
  }
  
  /* if an error has been returned or if the table has not been found */
  if ((EXTMEM_SFDP_OK != retr) || (0u == find))
  {
    retr = EXTMEM_SFDP_ERROR_DRIVER;
    goto error;
  }

  /* determine how to proced memory reset */
  if( 0x0u == JEDEC_Basic.Params.Param_DWORD.D16.SoftResetRescueSequence_Support)
  {
    /* 00_0000b: no software reset instruction is supported */
    reset_methode = RESET_NONE;
  }
  else if (0x1u == (0x1u & JEDEC_Basic.Params.Param_DWORD.D16.SoftResetRescueSequence_Support))
  {
    /* xx_xxx1b: drive Fh on all 4 data wires for 8 clocks */
    reset_methode = RESET_Fh_4DATA_8CLOCK;
  }
  else if (0x2u == (0x2u & JEDEC_Basic.Params.Param_DWORD.D16.SoftResetRescueSequence_Support))
  {
    /* xx_xx1xb: drive Fh on all 4 data wires for 10 clocks if device is operating in 4-byte address mode */
    reset_methode = RESET_Fh_4DATA_10CLOCK;
  }
  else if (0x4u == (0x4u & JEDEC_Basic.Params.Param_DWORD.D16.SoftResetRescueSequence_Support))
  {
    /* xx_x1xxb: drive Fh on all 4 data wires for 16 clocks */
    reset_methode = RESET_Fh_4DATA_16CLOCK;
  }
  else if (0x8u == (0x8u & JEDEC_Basic.Params.Param_DWORD.D16.SoftResetRescueSequence_Support))
  {
    /* xx_1xxxb: issue instruction F0h */
    reset_methode = RESET_INSTRUCTION_F0;
  }
  else if (0x10u == (0x10u & JEDEC_Basic.Params.Param_DWORD.D16.SoftResetRescueSequence_Support))
  {
    /* x1_xxxxb: issue reset enable instruction 66h, then issue reset instruction 99h. The reset enable,
    reset sequence may be issued on 1, 2, or 4 wires depending on the device operating mode.
    */
    reset_methode = RESET_INSTRUCTION_66_99;
  }
  else
  {
    /* there is an error on the bit definition not conform with JEDEC */
  }

  if (0x20u == (0x20u & JEDEC_Basic.Params.Param_DWORD.D16.SoftResetRescueSequence_Support))
  {
    /* 1x_xxxxb: exit 0-4-4 mode is required prior to other reset sequences above if the device may be
      operating in this mode. See 6.4.18, 0-4-4 Mode Exit
    */
    /* JEDEC_Basic.Params.Param_DWORD.D16.Exit4ByteAddressing
    Exit 4-Byte Addressing
    xx_xxxx_xxx1b: issue instruction E9h to exit 4-Byte address mode (write enable instruction 06h is not required)
    xx_xxxx_xx1xb: issue write enable instruction 06h, then issue instruction E9h to exit 4-Byte address mode
    xx_xxxx_x1xxb: 8-bit volatile extended address register used to define A[31:A24] bits. Read with instruction C8h. Write instruction is C5h, data length is 1 byte. Return to lowest memory segment by setting A[31:24] to 00h and use 3-Byte addressing.
    xx_xxxx_1xxxb: 8-bit volatile bank register used to define A[30:A24] bits. MSB (bit[7]) is used to enable/disable 4-byte address mode. When MSB is cleared to 0, 3-byte address mode is active and A30:A24 are used to select the active 128 Mbit memory segment. Read with instruction 16h. Write instruction is 17h, data length is 1 byte.
    xx_xxx1_xxxxb: A 16-bit nonvolatile configuration register controls 3-Byte/4-Byte address mode. Read instruction is B5h. Bit[0] controls address mode [0=3-Byte; 1=4-Byte]. Write configuration register instruction is B1h, data length is 2 bytes.
    xx_xx1x_xxxxb: Hardware reset
    xx_x1xx_xxxxb: Software reset (see bits 13:8 in this DWORD)
    xx_1xxx_xxxxb: Power cycle
    x1_xxxx_xxxxb: Reserved
    */
    retr = EXTMEM_SFDP_ERROR_NOTYETHANDLED;
    goto error;
  }

  switch(reset_methode)
  {
  case RESET_NONE:
    break;
  case RESET_INSTRUCTION_66_99:
    /* perform the reset in 1, 2 and 4 lines */
    SFDP_DEBUG_STR("::reset 0x66 0x99");
    (void)SAL_XSPI_SendCommand(&Object->sfpd_private.SALObject, 0x66, NULL, 0);
    (void)SAL_XSPI_SendCommand(&Object->sfpd_private.SALObject, 0x99, NULL, 0);
    break;
  case RESET_INSTRUCTION_F0:
  case RESET_Fh_4DATA_8CLOCK:
  case RESET_Fh_4DATA_10CLOCK:
  case RESET_Fh_4DATA_16CLOCK:
    retr = EXTMEM_SFDP_ERROR_NOTYETHANDLED;
    break;
  case RESET_ERROR:
  default :
    retr = EXTMEM_SFDP_ERROR_PARAM;
    break;
  }
error :
  return retr;
}

SFDP_StatusTypeDef SFDP_BuildGenericDriver(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object)
{
  SFDP_StatusTypeDef retr = EXTMEM_SFDP_OK;
  static const uint16_t block_erase_unit[] = { 16u, 256u, 4000u, 64000u};
  static const uint32_t chip_erase_unit[]  = { 16u, 256u, 4000u, 64000u};
  SFDP_DEBUG_STR(__func__);
  uint8_t flag4bitAddress = 0u;
  uint8_t flagExtension = EXTMEM_SFDP_FALSE;
  uint32_t dummyCycles, dummyCyclesValue;
  uint8_t FlashSize;

  if ((Object->sfpd_private.Sfdp_table_mask & (uint32_t)SFPD_PARAMID_BASIC_SPIPROTOCOL) != (uint32_t)SFPD_PARAMID_BASIC_SPIPROTOCOL)
  {
    /* This table is mandatory to build the driver data */
    retr = EXTMEM_SFDP_ERROR_NO_PARAMTABLE_BASIC;
    goto error;
  }

  if(((Object->sfpd_private.Sfdp_table_mask & (uint32_t)SFPD_PARAMID_4BYTE_ADDRESS_INSTRUCTION) != (uint32_t)SFPD_PARAMID_4BYTE_ADDRESS_INSTRUCTION) ||
      (JEDEC_Basic.Params.Param_DWORD.D1.AddressBytes == 0x0u))
  {
    /* The driver manages only 4-Bit addressing */
    retr = EXTMEM_SFDP_ERROR_NO_4BITADDRESSING;
    goto error;
  }

  /* ---------------------------------------------------
   *  Flash sizing
   * ---------------------------------------------------
   */
  /* Calculation of the flash density in puissance of 2 */
  if ((JEDEC_Basic.Params.Param_DWORD.D2.FlashSize & 0x80000000u) == 0x0u)
  {
#if( __CORTEX_M == 0)
#error "the assembly instruction is not available"
#else
    Object->sfpd_private.FlashSize = 31u - (uint8_t)__CLZ((JEDEC_Basic.Params.Param_DWORD.D2.FlashSize + 1u));
#endif /* __CORTEX_M */
  }
  else
  {
    Object->sfpd_private.FlashSize = (uint8_t)(JEDEC_Basic.Params.Param_DWORD.D2.FlashSize & 0x7FFFFFFFu);
  }

  /* Conversion bit to byte */
  Object->sfpd_private.FlashSize = Object->sfpd_private.FlashSize - 3u; /* divide by eight the value */

  SFDP_DEBUG_INT("-> flash size: 2^", Object->FlashSize);
  FlashSize = Object->sfpd_private.FlashSize - 1u;
  (void) SAL_XSPI_MemoryConfig(&Object->sfpd_private.SALObject, PARAM_FLASHSIZE, &FlashSize);

  /* get the page size info */
  Object->sfpd_private.PageSize = ((uint32_t)1u <<  JEDEC_Basic.Params.Param_DWORD.D11.PageSize);

  /* ---------------------------------------------------
   *  Erase management
   * ---------------------------------------------------
   */
  /* Manage erase data */
  Object->sfpd_private.DriverInfo.EraseType1Size      = (uint8_t)JEDEC_Basic.Params.Param_DWORD.D8.EraseType1_Size;
  Object->sfpd_private.DriverInfo.EraseType1Command   = (uint8_t)JEDEC_Basic.Params.Param_DWORD.D8.EraseType1_Instruction;
  Object->sfpd_private.DriverInfo.EraseType2Size      = (uint8_t)JEDEC_Basic.Params.Param_DWORD.D8.EraseType2_Size;
  Object->sfpd_private.DriverInfo.EraseType2Command   = (uint8_t)JEDEC_Basic.Params.Param_DWORD.D8.EraseType2_Instruction;
  Object->sfpd_private.DriverInfo.EraseType3Size      = (uint8_t)JEDEC_Basic.Params.Param_DWORD.D9.EraseType3_Size;
  Object->sfpd_private.DriverInfo.EraseType3Command   = (uint8_t)JEDEC_Basic.Params.Param_DWORD.D9.EraseType3_Instruction;
  Object->sfpd_private.DriverInfo.EraseType4Size      = (uint8_t)JEDEC_Basic.Params.Param_DWORD.D9.EraseType4_Size;
  Object->sfpd_private.DriverInfo.EraseType4Command   = (uint8_t)JEDEC_Basic.Params.Param_DWORD.D9.EraseType4_Instruction;

  if (Object->sfpd_private.DriverInfo.EraseType1Command != 0x0u)
  {
    Object->sfpd_private.DriverInfo.EraseType1Timing   = (uint32_t)JEDEC_Basic.Params.Param_DWORD.D10.MutliplierEraseTime * (JEDEC_Basic.Params.Param_DWORD.D10.EraseType1_TypicalTime_count + 1u)* block_erase_unit[JEDEC_Basic.Params.Param_DWORD.D10.EraseType1_TypicalTime_units];
  }

  if (Object->sfpd_private.DriverInfo.EraseType2Command != 0x0u)
  {
    Object->sfpd_private.DriverInfo.EraseType2Timing   = (uint32_t)JEDEC_Basic.Params.Param_DWORD.D10.MutliplierEraseTime * (JEDEC_Basic.Params.Param_DWORD.D10.EraseType2_TypicalTime_count + 1u)* block_erase_unit[JEDEC_Basic.Params.Param_DWORD.D10.EraseType2_TypicalTime_units];
  }

  if (Object->sfpd_private.DriverInfo.EraseType3Command != 0x0u)
  {
    Object->sfpd_private.DriverInfo.EraseType3Timing   = (uint32_t)JEDEC_Basic.Params.Param_DWORD.D10.MutliplierEraseTime * (JEDEC_Basic.Params.Param_DWORD.D10.EraseType3_TypicalTime_count + 1u)* block_erase_unit[JEDEC_Basic.Params.Param_DWORD.D10.EraseType3_TypicalTime_units];
  }

  if (Object->sfpd_private.DriverInfo.EraseType4Command != 0x0u)
  {
    Object->sfpd_private.DriverInfo.EraseType4Timing   = (uint32_t)JEDEC_Basic.Params.Param_DWORD.D10.MutliplierEraseTime * (JEDEC_Basic.Params.Param_DWORD.D10.EraseType4_TypicalTime_count + 1u)* block_erase_unit[JEDEC_Basic.Params.Param_DWORD.D10.EraseType4_TypicalTime_units];
  }

  Object->sfpd_private.DriverInfo.EraseChipTiming   = JEDEC_Basic.Params.Param_DWORD.D10.MutliplierEraseTime * (JEDEC_Basic.Params.Param_DWORD.D11.ChipErase_TypicalTime_count + 1u)* chip_erase_unit[JEDEC_Basic.Params.Param_DWORD.D11.ChipErase_TypicalTime_units];

  /* ------------------------------------------------------
   *   WIP/WEL : write in progress/ write enable management
   * ------------------------------------------------------
   */
  /* This bit definition is maintained for legacy compatibility only. New system implementations
  should refer to 6.4.19 for a full definition of volatile and non-volatile behavior. */
  Object->sfpd_private.DriverInfo.ReadWELCommand = 0x05;
  Object->sfpd_private.DriverInfo.ReadWIPCommand = 0x05;
  if (JEDEC_Basic.Params.Param_DWORD.D1.WriteEnableInstructionVolatileRegister != 0u)
  {
    Object->sfpd_private.DriverInfo.WriteWELCommand = 0x50U;
  }
  else
  {
    Object->sfpd_private.DriverInfo.WriteWELCommand = 0x06U;
  }

  /* Volatile or Non-Volatile Register and Write Enable Instruction for Status Register 1
  The instruction 01h is typically used to write status register 1 which contains Block Protection (BP) and other bits. Status register 1 is written by the first data byte following the instruction 01h. The protection bits must be written to zero to enable writes/erases to the device.
  This field describes how to modify the writable bits in status register 1 in either a volatile or non-volatile manner. Bits 1:0 in status register 1 are de-facto standard write enable and busy status and are excluded from the definitions below.
  */
  /* xxx_xxx1b: Non-Volatile Status Register 1, powers-up to last written value, use instruction 06h to enable write */
  if ((JEDEC_Basic.Params.Param_DWORD.D16.VolatileNonVolatileRegister_WriteEnable & 0x1u) != 0u)
  {
    Object->sfpd_private.DriverInfo.WriteWELCommand = 0x06;
  }
  /* xxx_xx1xb: Volatile Status Register 1, status register powers-up with bits set to "1"s, use instruction 06h to enable write */
  else if ((JEDEC_Basic.Params.Param_DWORD.D16.VolatileNonVolatileRegister_WriteEnable & 0x02u) != 0u)
  {
    Object->sfpd_private.DriverInfo.WriteWELCommand = 0x06;
  }
  /* xxx_x1xxb: Volatile Status Register 1, status register powers-up with bits set to "1"s, use instruction 50h to enable write */
  else if ((JEDEC_Basic.Params.Param_DWORD.D16.VolatileNonVolatileRegister_WriteEnable & 0x04u) != 0u)
  {
    Object->sfpd_private.DriverInfo.WriteWELCommand = 0x50;
  }
  /* xxx_1xxxb: Non-Volatile/Volatile status register 1 powers-up to last written value in the non-volatile status register,
  use instruction 06h to enable write to non-volatile status register. Volatile status register may be activated after
  power-up to override the non-volatile status register, use instruction 50h to enable write and activate the volatile
  status register.*/
  else if ((JEDEC_Basic.Params.Param_DWORD.D16.VolatileNonVolatileRegister_WriteEnable & 0x0Bu) != 0u)
  {
    Object->sfpd_private.DriverInfo.WriteWELCommand = 0x50;
  }
  /* xx1_xxxxb: Status Register 1 contains a mix of volatile and non-volatile bits. The 06h instruction is used to
  enable writing of the register.*/
  else if ((JEDEC_Basic.Params.Param_DWORD.D16.VolatileNonVolatileRegister_WriteEnable & 0x10u) != 0u)
  {
    Object->sfpd_private.DriverInfo.WriteWELCommand = 0x06;
  }
  /* x1x_xxxxb: Reserved
  1xx_xxxxb: Reserved
  NOTE If the status register is read-only then this field will contain all zeros in bits 4:0.
  */
  else
  {
    retr = EXTMEM_SFDP_ERROR_JEDECBASIC_D16;
    goto error;
  }

  if(0u != (Object->sfpd_private.Sfdp_table_mask & (uint32_t)SFPD_PARAMID_STATUS_CONTROL_CONFIG_REGISTER_MAP))
  {
    /* WIP */
    if (0u != JEDEC_SCCR_Map.Param_DWORD.D5.WIPBitAvailable)
    {
      Object->sfpd_private.DriverInfo.ReadWIPCommand  = (uint8_t)JEDEC_SCCR_Map.Param_DWORD.D5.CommandReadAccess;
      Object->sfpd_private.DriverInfo.WIPPosition     = JEDEC_SCCR_Map.Param_DWORD.D5.WIPBitLocationRegister;
      Object->sfpd_private.DriverInfo.WIPBusyPolarity = (uint8_t)JEDEC_SCCR_Map.Param_DWORD.D5.WIPpolarity;

      if (0u != JEDEC_SCCR_Map.Param_DWORD.D5.BitAccessedByCommandsUsingAddress)
      {
        /* Address management */
        Object->sfpd_private.DriverInfo.WIPAddress  = (uint8_t)JEDEC_SCCR_Map.Param_DWORD.D5.LocalAddressForWIP;
        Object->sfpd_private.DriverInfo.WIPPosition = JEDEC_SCCR_Map.Param_DWORD.D5.WIPBitLocationRegister;
      }
    }

    /* WEL */
    if (0u != JEDEC_SCCR_Map.Param_DWORD.D6.WELBitAvailable)
    {
      Object->sfpd_private.DriverInfo.ReadWELCommand  = (uint8_t)JEDEC_SCCR_Map.Param_DWORD.D6.CommandReadAccess;
      Object->sfpd_private.DriverInfo.WELPosition     = JEDEC_SCCR_Map.Param_DWORD.D6.WELBitLocationRegister;
      Object->sfpd_private.DriverInfo.WELBusyPolarity = (uint8_t)JEDEC_SCCR_Map.Param_DWORD.D6.WELpolarity;

      if (0u != JEDEC_SCCR_Map.Param_DWORD.D5.BitAccessedByCommandsUsingAddress)
      {
        /* Address management */
        Object->sfpd_private.DriverInfo.WELAddress  = (uint8_t)JEDEC_SCCR_Map.Param_DWORD.D6.WELLocalAddress;
        Object->sfpd_private.DriverInfo.WELPosition = JEDEC_SCCR_Map.Param_DWORD.D6.WELBitLocationRegister;
      }
    }

  }
  else
  {
      /*
       *   WIP : Status register read management
       *         Basic D14 Status register Polling device Busy
       */
      if(0x01u == (JEDEC_Basic.Params.Param_DWORD.D14.StatusRegister &  0x01u))
      {
        /* xx_xxx1b: Use of legacy polling is supported by reading the Status Register with 05h instruction
        and checking WIP bit[0] (0=ready; 1=busy). */
        Object->sfpd_private.DriverInfo.ReadWIPCommand = 0x05;
        Object->sfpd_private.DriverInfo.WIPPosition = 0u;
        Object->sfpd_private.DriverInfo.WIPBusyPolarity = 0u;
      } else if (0x02u == (JEDEC_Basic.Params.Param_DWORD.D14.StatusRegister &  0x02u))
      {
        /* xx_xx1xb: Bit 7 of the Flag Status Register may be polled any time a Program, Erase, Suspend/Resume
        command is issued, or after a Reset command while the device is busy. The read instruction is 70h.
        Flag Status Register bit definitions: bit[7]: Program or erase controller status (0=busy; 1=ready)*/
        Object->sfpd_private.DriverInfo.ReadWELCommand = 0x70;
        Object->sfpd_private.DriverInfo.WELPosition = 7u;
        Object->sfpd_private.DriverInfo.WELBusyPolarity = 0u;
      } else
      {
          retr = EXTMEM_SFDP_ERROR_JEDECBASIC_D14;
          goto error;
      }
      /*
       *   WEL : Write enable latch command
       */
      Object->sfpd_private.DriverInfo.WIPPosition     = 0u;
      Object->sfpd_private.DriverInfo.WIPBusyPolarity = 0u;
  }

  /* Set default value for instruction */
  Object->sfpd_private.DriverInfo.ReadInstruction     = 0x02U;

  /* -------------------------------------------------------------------------------------------------------------------

     If an octal DDR table is present and the target is 8D8D8D,
     when switch in octal DDR mode
     -------------------------------------------------------------------------------------------------------------------
  */
  if (((uint32_t)SFPD_PARAMID_OCTAL_DDR == (Object->sfpd_private.Sfdp_table_mask & (uint32_t)SFPD_PARAMID_OCTAL_DDR)) 
      && (EXTMEM_LINK_CONFIG_8LINES == Object->sfpd_private.Config))
  {
    /* check if we are not already in octal mode */
    if (PHY_LINK_8D8D8D == Object->sfpd_private.DriverInfo.SpiPhyLink)
    {
      flag4bitAddress = 1u;
    }
    else
    {
      /* Execute the flash command sequence to switch in octal DDR */
      if (EXTMEM_SFDP_OK == sfpd_enter_octal_mode(Object))
      {
        /* switch the memory interface configuration according the Access protocol field */
        flag4bitAddress = 1u;
        Object->sfpd_private.DriverInfo.SpiPhyLink = PHY_LINK_8D8D8D;
        if (HAL_OK != SAL_XSPI_MemoryConfig(&Object->sfpd_private.SALObject, PARAM_PHY_LINK, &Object->sfpd_private.DriverInfo.SpiPhyLink))
        {
          retr = EXTMEM_SFDP_ERROR_DRIVER;
          goto error;
        }

        if (JEDEC_SFDP_Header.AccessProtocol == 0xFDu)
        {
          /* set 20 wait state */
          dummyCycles = 20;
          (void)SAL_XSPI_MemoryConfig(&Object->sfpd_private.SALObject, PARAM_DUMMY_CYCLES, (void*)&dummyCycles);
        }
        if (JEDEC_SFDP_Header.AccessProtocol == 0xFEu)
        {
          /* set 8 wait state */
          dummyCycles = 8;
          (void)SAL_XSPI_MemoryConfig(&Object->sfpd_private.SALObject, PARAM_DUMMY_CYCLES, (void*)&dummyCycles);
        }
      }
      else
      {
        /* an error occurs when trying to switch the mode                        */
        /* when continue the process and check if another mode could be targeted */
      }
    }

    if ((0u != (Object->sfpd_private.Sfdp_table_mask & (uint32_t)SFPD_PARAMID_BASIC_SPIPROTOCOL)) &&
        (JEDEC_Basic.size > 16u))
    {
      /* check octal information to determine */
      switch(JEDEC_Basic.Params.Param_DWORD.D18.OctalDTRCommandExtension)
      {
        /* The Command Extension is the same as the Command. (The Command / Command Extension has the same value for the whole clock period.)*/
      case 0b00:
        flagExtension = EXTMEM_SFDP_TRUE;
        break;
        /* Command Extension is the inverse of the Command. The Command Extension acts as a confirmation of the Command */
      case 0b01:
        flagExtension = EXTMEM_SFDP_FALSE;
        break;
        /* Command and Command Extension forms a 16 bit command word */
      case 0b11:
      default :
        retr = EXTMEM_SFDP_ERROR_NOTYETHANDLED;
        goto error;
        break;
      }
    }
    if(HAL_OK != SAL_XSPI_MemoryConfig(&Object->sfpd_private.SALObject, PARAM_COMMAND_EXTENSION, &flagExtension))
    {
      retr = EXTMEM_SFDP_ERROR_DRIVER;
      goto error;
    }
  }

  if ((Object->sfpd_private.Sfdp_table_mask & (uint32_t)SFPD_PARAMID_4BYTE_ADDRESS_INSTRUCTION) == (uint32_t)SFPD_PARAMID_4BYTE_ADDRESS_INSTRUCTION)
  {
    if (0u == flag4bitAddress)
    {
      /* xxxx_xxx1b: issue instruction B7h (preceding write enable not required) */
      if (0x01u == (JEDEC_Basic.Params.Param_DWORD.D16.Enter4ByteAddressing & 0x01u))
      {
        /* send command to enter 4bit@ mode */
        if (HAL_OK != SAL_XSPI_SendCommand(&Object->sfpd_private.SALObject,0xB7, NULL, 0))
        {
            retr = EXTMEM_SFDP_ERROR_DRIVER;
            goto error;
        }

        /* Set 4Bit addressing on PHY side */
        if (HAL_OK != SAL_XSPI_MemoryConfig(&Object->sfpd_private.SALObject, PARAM_ADDRESS_4BITS, NULL))
        {
            retr = EXTMEM_SFDP_ERROR_DRIVER;
            goto error;
        }

        /* flag4bitAddress = 1u; this settings is not needed because variable is no more used */
      }
      /* xxxx_xx1xb: issue write enable instruction 06h, then issue instruction B7h */
      else if (0x2u == (JEDEC_Basic.Params.Param_DWORD.D16.Enter4ByteAddressing & 0x2u))
      {
        /* send command to write enable */
        if (HAL_OK != SAL_XSPI_SendCommand(&Object->sfpd_private.SALObject,
                                           Object->sfpd_private.DriverInfo.WriteWELCommand, NULL, 0u))
        {
            retr = EXTMEM_SFDP_ERROR_DRIVER;
            goto error;
        }

        /* control the write enable */
        if (HAL_OK != SAL_XSPI_CheckStatusRegister(&Object->sfpd_private.SALObject,
                                            Object->sfpd_private.DriverInfo.ReadWELCommand,
                                            Object->sfpd_private.DriverInfo.WELAddress,
                                            ((Object->sfpd_private.DriverInfo.WELBusyPolarity == 0u) ? 1u: 0u) << Object->sfpd_private.DriverInfo.WELPosition,
                                            1u << Object->sfpd_private.DriverInfo.WELPosition,
                                            1000))
        {
            retr = EXTMEM_SFDP_ERROR_DRIVER;
            goto error;
        }

        /* send command to enter 4bit@ mode */
        if (HAL_OK != SAL_XSPI_SendCommand(&Object->sfpd_private.SALObject, 0xB7, NULL, 0u))
        {
          retr = EXTMEM_SFDP_ERROR_DRIVER;
          goto error;
        }
      }
      /* x1xx_xxxxb: Always operates in 4-Byte address mode */
      else if (0x40u == (JEDEC_Basic.Params.Param_DWORD.D16.Enter4ByteAddressing & 0x40u))
      {
        /* nothing to do */
        /* flag4bitAddress = 1u; this settings is not needed because variable is no more used */
      }
      /* xx1x_xxxxb: Supports dedicated 4-Byte address instruction set. Consult vendor data sheet for the instruction set definition.*/
      else if (0x20u == (JEDEC_Basic.Params.Param_DWORD.D16.Enter4ByteAddressing & 0x20u))
      {
        /* specific memory  */
        /* one macromix nothing to do the command are 4Byte specific so nothing to do */
        /* flag4bitAddress = 1u; this settings is not needed because variable is no more used */
      }
        /* xxxx_x1xxb: 8-bit volatile extended address register used to define A[31:24] bits. Read with instruction C8h. Write instruction is C5h with 1 byte of data. Select the active 128 Mbit memory segment by setting the appropriate A[31:24] bits and use 3-Byte addressing.
           xxxx_1xxxb: 8-bit volatile bank register used to define A[30:A24] bits. MSB (bit[7]) is used to enable/disable 4-byte address mode. When MSB is set to 1, 4-byte address mode is active and A[30:24] bits are don not care. Read with instruction 16h. Write instruction is 17h with 1 byte of data. When MSB is cleared to 0, select the active 128 Mbit segment by setting the appropriate A[30:24] bits and use 3-Byte addressing.
           xxx1_xxxxb: A 16-bit nonvolatile configuration register controls 3-Byte/4-Byte address mode. Read instruction is B5h. Bit[0] controls address mode [0=3-Byte;1=4-Byte]. Write configuration register instruction is B1h, data length is 2 bytes.
        */
      else {
        retr = EXTMEM_SFDP_ERROR_NOTYETHANDLED;
        goto error;

      }

      /* Set 4Bit addressing on PHY side */
      if (HAL_OK != SAL_XSPI_MemoryConfig(&Object->sfpd_private.SALObject, PARAM_ADDRESS_4BITS, NULL))
      {
        retr = EXTMEM_SFDP_ERROR_DRIVER;
        goto error;
      }
    }

    if ((EXTMEM_LINK_CONFIG_8LINES == Object->sfpd_private.Config) && (PHY_LINK_1S1S1S == Object->sfpd_private.DriverInfo.SpiPhyLink))
    {
      /* check if we can switch to if the config is still 1S8S8S */
      if ((0u != JEDEC_Address4Bit.Param_DWORD.D1.Support_1S8S8S_FastReadCommand)  && (0u != JEDEC_Address4Bit.Param_DWORD.D1.Support_1S8S8S_PageProgramCommand))
      {
        /* Patch micron write command 0x81 @0x0 0xE7 */
        Object->sfpd_private.DriverInfo.SpiPhyLink = PHY_LINK_1S8S8S;
        if (HAL_OK != SAL_XSPI_MemoryConfig(&Object->sfpd_private.SALObject, PARAM_PHY_LINK, &Object->sfpd_private.DriverInfo.SpiPhyLink))
        {
          retr = EXTMEM_SFDP_ERROR_DRIVER;
          goto error;
        }
        Object->sfpd_private.DriverInfo.ReadInstruction = 0xCC;
        Object->sfpd_private.DriverInfo.PageProgramInstruction = 0x8E;
      }
    }

    /*
      need to be study more; it seems that macromix used it to define the command maybe because only one mode is
      supported in their case
    */
    if ((Object->sfpd_private.DriverInfo.SpiPhyLink == PHY_LINK_8D8D8D) || 
        (Object->sfpd_private.DriverInfo.SpiPhyLink == PHY_LINK_1S1S1S))
    {
      if (0u != JEDEC_Address4Bit.Param_DWORD.D1.Support_1S1S1S_PageProgramCommand) {Object->sfpd_private.DriverInfo.PageProgramInstruction = 0x12u;}
      if (0u != JEDEC_Address4Bit.Param_DWORD.D1.Support_1S1S1S_ReadCommand)        {Object->sfpd_private.DriverInfo.ReadInstruction        = 0x13u;}
      if (0u != JEDEC_Address4Bit.Param_DWORD.D1.Support_1S1S1S_FastReadCommand)    {Object->sfpd_private.DriverInfo.ReadInstruction        = 0x0Cu;}
    }

    Object->sfpd_private.DriverInfo.EraseType1Command   = (uint8_t)JEDEC_Address4Bit.Param_DWORD.D2.InstructionEraseType1;
    Object->sfpd_private.DriverInfo.EraseType2Command   = (uint8_t)JEDEC_Address4Bit.Param_DWORD.D2.InstructionEraseType2;
    Object->sfpd_private.DriverInfo.EraseType3Command   = (uint8_t)JEDEC_Address4Bit.Param_DWORD.D2.InstructionEraseType3;
    Object->sfpd_private.DriverInfo.EraseType4Command   = (uint8_t)JEDEC_Address4Bit.Param_DWORD.D2.InstructionEraseType4;
  }

  if(((uint32_t)SFPD_PARAMID_XSPI_V1_0 == (Object->sfpd_private.Sfdp_table_mask & (uint32_t)SFPD_PARAMID_XSPI_V1_0))
      && (PHY_LINK_8D8D8D == Object->sfpd_private.DriverInfo.SpiPhyLink))
  {
    uint32_t ClockOut;
    uint32_t MaxFreqMhz;
    /* Read command */
    if (0u != JEDEC_XSPI10.Param_DWORD.D1.ReadFastCommand)
    {
      Object->sfpd_private.DriverInfo.ReadInstruction = (uint8_t)JEDEC_XSPI10.Param_DWORD.D1.ReadFastCommand;
    }

	  /* adapt the memory freq according its capabilities */
    MaxFreqMhz = sfdp_getfrequencevalue(JEDEC_Basic.Params.Param_DWORD.D20._8D8D8D_MaximunSpeedWithStrobe);
    if(HAL_OK != SAL_XSPI_SetClock(&Object->sfpd_private.SALObject, Object->sfpd_private.DriverInfo.ClockIn, MaxFreqMhz, &ClockOut))
    {
      retr = EXTMEM_SFDP_ERROR_SETCLOCK;
      goto error;
    }
    
    /* get the dummy cycle value according the real output clock */
    if (ClockOut >= CLOCK_200MHZ)
    {
      dummyCycles = JEDEC_XSPI10.Param_DWORD.D4.Operation200Mhz_DummyCycle;
      dummyCyclesValue = JEDEC_XSPI10.Param_DWORD.D4.Operation200Mhz_ConfigPattern;
    }
    else if ((ClockOut >= CLOCK_166MHZ) && (ClockOut <= CLOCK_200MHZ))
    {
      dummyCycles = JEDEC_XSPI10.Param_DWORD.D5.Operation166Mhz_DummyCycle;
      dummyCyclesValue = JEDEC_XSPI10.Param_DWORD.D5.Operation166Mhz_ConfigPattern;
    }
    else if ((ClockOut >= CLOCK_133MHZ) && (ClockOut < CLOCK_166MHZ))
    {
      dummyCycles = JEDEC_XSPI10.Param_DWORD.D5.Operation133Mhz_DummyCycle;
      dummyCyclesValue = JEDEC_XSPI10.Param_DWORD.D5.Operation133Mhz_ConfigPattern;
    }
    else /* if (ClockOut =< 100Mhz) */
    {
      dummyCycles = JEDEC_XSPI10.Param_DWORD.D5.Operation100Mhz_DummyCycle;
      dummyCyclesValue = JEDEC_XSPI10.Param_DWORD.D5.Operation100Mhz_ConfigPattern;
    }

    /* Set the dummy cycle corresponding to the selected clock */
    (void)SAL_XSPI_MemoryConfig(&Object->sfpd_private.SALObject, PARAM_DUMMY_CYCLES, (void*)&dummyCycles);
    /* Write the dummy cycle value in the configuration register using information coming from SCCR Map */
    if((0u != (Object->sfpd_private.Sfdp_table_mask & (uint32_t)SFPD_PARAMID_STATUS_CONTROL_CONFIG_REGISTER_MAP))
	  && (0u != JEDEC_SCCR_Map.Param_DWORD.D10.BitAvailable))
    {
       retr = sfpd_set_dummycycle(Object, dummyCyclesValue);
       if (retr != EXTMEM_SFDP_OK)
       {
          goto error;
       }
    }
    else
    {
      retr = EXTMEM_SFDP_ERROR_CONFIGDUMMY;
      goto error;
    }
  }

  switch(Object->sfpd_private.DriverInfo.SpiPhyLink)
  {
  case PHY_LINK_8D8D8D :{
   break;
  }
  case PHY_LINK_1S1S1S :
    break;

  default :
    break;
  }

error :
  return retr;
}

/**
  * @}
  */

/** @addtogroup NOR_SFDP_DATA_Functions
  * @{
  */

/**
 * @brief this function returns the frequence corresponding to a frequecy
 * @param BitField bit field value
 * @return frequency value
 */
uint32_t sfdp_getfrequencevalue(uint32_t BitField)
{
  const uint16_t freqMhz_table[] = { 0x0, 33, 50, 66, 80, 100, 133, 166, 200, 250, 266, 333, 400 };
  SFDP_DEBUG_STR(__func__);

  if (BitField < 0b1010u)
  {
    return (uint32_t)freqMhz_table[BitField]* 1000000u;
  }
  return 0; /* the max frequency is unknown */
}

/**
 * @brief this function read and check the SFDP header
 * @param Object memory Object
 * @param sfdp_header data of the SFDP header
 * @return @ref SFDP_StatusTypeDef
 */
SFDP_StatusTypeDef sfdp_read_header(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object, SFPD_HeaderTypeDef *sfdp_header)
{
  SFDP_StatusTypeDef retr = EXTMEM_SFDP_OK;
  SFDP_DEBUG_STR(__func__);

  (void)memset(sfdp_header, 0xAA, SFDP_HEADER_SIZE);

  /* send the SFDP command to read the header */
  if(HAL_OK != SAL_XSPI_GetSFDP(&Object->sfpd_private.SALObject, 0, (uint8_t*)sfdp_header, SFDP_HEADER_SIZE))
  {
    retr = EXTMEM_SFDP_ERROR_MEDUIM;
    goto error;
  }

  /* check version numbrer */
  SFDP_DEBUG_STR("DCH : do we need to control the JEDEC version ?")

  /* check the magic number */
  if (SFDP_SIGNATURE != sfdp_header->Signature)
  {
    retr = EXTMEM_SFDP_ERROR_SIGNATURE;
    SFDP_DEBUG_STR("signature of the header: KO");
    goto error;
  }
  else
  {
    SFDP_DEBUG_STR("signature of the header: OK");
    SFDP_DEBUG_INT("param_number=", sfdp_header->param_number);
    SFDP_DEBUG_INT("AccessProtocol=", sfdp_header->AccessProtocol);
  }

error:
  return retr;
}

/**
 * @brief this function read and check the SFDP header
 * @param Object memory Object
 * @param sfdp_adress address of the SFDP table
 * @param sfdp_param_info pointer on parameter info
 * @return @ref SFDP_StatusTypeDef
 */
SFDP_StatusTypeDef sfdp_get_paraminfo(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object, uint32_t sfdp_adress, SFDP_ParameterTableTypeDef *Param_info)
{
  SFDP_StatusTypeDef retr = EXTMEM_SFDP_OK;
  SFDP_ParameterHeaderTypeDef sfdp_param_header = {0};
  SFDP_DEBUG_STR(__func__);

  /* send the SFDP command to read the header */
  if(HAL_OK != SAL_XSPI_GetSFDP(&Object->sfpd_private.SALObject, sfdp_adress, (uint8_t*)&sfdp_param_header, SFDP_PARAM_HEADER_SIZE))
  {
    retr = EXTMEM_SFDP_ERROR_MEDUIM;
    goto error;
  }

  Param_info->type = SFPD_PARAMID_UNKNOWN;
  Param_info->size = sfdp_param_header.Length ;
  Param_info->address = (((uint32_t)sfdp_param_header.TableAddressPointer[2u] << 16u)
                        |((uint32_t)sfdp_param_header.TableAddressPointer[1u] << 8u)
                        |((uint32_t)sfdp_param_header.TableAddressPointer[0u]));

  if ((sfdp_param_header.ID_msb > 0x00u) && (sfdp_param_header.ID_msb < 0x80u))
  {
    if ((sfdp_param_header.ID_msb & 0x01u) == 0x01u)
    {
      Param_info->type = SFPD_PARAMID_VENDOR;
      SFDP_DEBUG_STR("-> info SFPD_PARAMID_VENDOR");
    }
    else
    {
      Param_info->type = SFPD_PARAMID_FUNCTION_VENDOR;
      SFDP_DEBUG_STR("-> info SFPD_PARAMID_FUNCTION_VENDOR");
    }
  }
  else
  {
    if((sfdp_param_header.ID_lsb & 0x01u) == 0x00u)
    {
      Param_info->type = SFPD_PARAMID_FUNCTION_JEDEC;
      SFDP_DEBUG_STR("-> info SFPD_PARAMID_FUNCTION_JEDEC");
    }

    if((0xFFu == sfdp_param_header.ID_msb))
    {
      switch(sfdp_param_header.ID_lsb)
      {
      case 0x00u :
        Param_info->type = SFPD_PARAMID_BASIC_SPIPROTOCOL;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_BASIC_SPIPROTOCOL");
        Param_info->size = ((sfdp_param_header.Length ) < SFPD_PARAMS_BASIC_TABLE_DEFAULTSIZE) ? SFPD_PARAMS_BASIC_TABLE_DEFAULTSIZE : sfdp_param_header.Length;
        break;
      case 0x81u:
        Param_info->type = SFPD_PARAMID_SECTORMAP;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_SECTORMAP");
        break;
      case 0x03u:
        Param_info->type = SFPD_PARAMID_RPMC;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_RPMC");
        break;
      case 0x84u:
        Param_info->type = SFPD_PARAMID_4BYTE_ADDRESS_INSTRUCTION;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_4BYTE_ADDRESS_INSTRUCTION");
        break;
      case 0x05u:
        Param_info->type = SFPD_PARAMID_XSPI_V1_0;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_XSPI_V1_0");
        break;
      case 0x06u:
        Param_info->type = SFPD_PARAMID_XSPI_V2_0;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_XSPI_V2_0");
        break;
      case 0x87u:
        Param_info->type = SFPD_PARAMID_STATUS_CONTROL_CONFIG_REGISTER_MAP;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_STATUS_CONTROL_CONFIG_REGISTER_MAP");
        break;
      case 0x88u:
        Param_info->type = SFPD_PARAMID_STATUS_CONTROL_CONFIG_REGISTER_MAP_MULTICHIP;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_STATUS_CONTROL_CONFIG_REGISTER_MAP_MULTICHIP");
        break;
      case 0x09u:
        Param_info->type = SFPD_PARAMID_STATUS_CONTROL_CONFIG_XSPI_V2_0;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_STATUS_CONTROL_CONFIG_XSPI_V2_0");
        break;
      case 0x0Au:
        Param_info->type = SFPD_PARAMID_OCTAL_DDR;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_OCTAL_DDR");
        break;
      case 0x8Bu:
        Param_info->type = SFPD_PARAMID_MSPT;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_MSPT");
        break;
      case 0x0Cu:
        Param_info->type = SFPD_PARAMID_X4QUAD_DS;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_X4QUAD_DS");
        break;
      case 0x8Du:
        Param_info->type = SFPD_PARAMID_QUAD_DDR;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_QUAD_DDR");
        break;
      case 0x8Eu:
        Param_info->type = SFPD_PARAMID_SECURE_PACKET_READ_WRITE;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_SECURE_PACKET_READ_WRITE");
        break;
      case 0x0Fu:
        Param_info->type = SFPD_PARAMID_RESERVED;
        SFDP_DEBUG_STR("-> info SFPD_PARAMID_RESERVED");
        break;
      default :
        break;
      }
    }

    if( Param_info->type == SFPD_PARAMID_UNKNOWN)
    {
      SFDP_DEBUG_STR("-> the table is not conform to JEDEC standard");
    }
  }
  
error:
  return retr;
}

/**
 * @brief this function executes the octal DDR table to enter octal DDR mode
 * @param Object memory Object
 * @return @ref SFDP_StatusTypeDef
 */
SFDP_StatusTypeDef sfpd_enter_octal_mode(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object)
{
  SFDP_StatusTypeDef retr = EXTMEM_SFDP_OK;
  uint8_t data[7];
  SFDP_DEBUG_STR(__func__);
  /* D1-D2 command */
  if (0u != JEDEC_OctalDdr.Param_DWORD.D1.LenghtCommand)
  {
     data[0] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D1.Byte1CommandSequence;
     data[1] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D1.Byte2CommandSequence;
     data[2] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D1.Byte3CommandSequence;
     data[3] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D2.Byte4CommandSequence;
     data[4] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D2.Byte5CommandSequence;
     data[5] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D2.Byte6CommandSequence;
     data[6] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D2.Byte7CommandSequence;

     if (HAL_OK != SAL_XSPI_SendCommand(&Object->sfpd_private.SALObject, data[0], &data[1],
                                        (uint16_t)(JEDEC_OctalDdr.Param_DWORD.D1.LenghtCommand - 1u)))
     {
       retr = EXTMEM_SFDP_ERROR_OCTALMODE;
       goto error;
     }
  }

  /* D3-D4 command */
  if (0u != JEDEC_OctalDdr.Param_DWORD.D3.LenghtCommand)
  {
     data[0] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D3.Byte1CommandSequence;
     data[1] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D3.Byte2CommandSequence;
     data[2] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D3.Byte3CommandSequence;
     data[3] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D4.Byte4CommandSequence;
     data[4] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D4.Byte5CommandSequence;
     data[5] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D4.Byte6CommandSequence;
     data[6] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D4.Byte7CommandSequence;

     if (HAL_OK != SAL_XSPI_SendCommand(&Object->sfpd_private.SALObject, data[0], &data[1],
                                        (uint16_t)(JEDEC_OctalDdr.Param_DWORD.D3.LenghtCommand - 1u)))
     {
       retr = EXTMEM_SFDP_ERROR_OCTALMODE;
       goto error;
     }
  }

  /* D5-D6 command */
  if (0u != JEDEC_OctalDdr.Param_DWORD.D5.LenghtCommand)
  {
     data[0] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D5.Byte1CommandSequence;
     data[1] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D5.Byte2CommandSequence;
     data[2] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D5.Byte3CommandSequence;
     data[3] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D6.Byte4CommandSequence;
     data[4] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D6.Byte5CommandSequence;
     data[5] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D6.Byte6CommandSequence;
     data[6] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D6.Byte7CommandSequence;

     if (HAL_OK != SAL_XSPI_SendCommand(&Object->sfpd_private.SALObject, data[0], &data[1],
                                        (uint16_t)(JEDEC_OctalDdr.Param_DWORD.D5.LenghtCommand - 1u)))
     {
       retr = EXTMEM_SFDP_ERROR_OCTALMODE;
       goto error;
     }
  }

  /* D7-D8 command */
  if (0u != JEDEC_OctalDdr.Param_DWORD.D7.LenghtCommand)
  {
     data[0] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D7.Byte1CommandSequence;
     data[1] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D7.Byte2CommandSequence;
     data[2] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D7.Byte3CommandSequence;
     data[3] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D8.Byte4CommandSequence;
     data[4] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D8.Byte5CommandSequence;
     data[5] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D8.Byte6CommandSequence;
     data[6] = (uint8_t)JEDEC_OctalDdr.Param_DWORD.D8.Byte7CommandSequence;

     if (HAL_OK != SAL_XSPI_SendCommand(&Object->sfpd_private.SALObject, data[0], &data[1],
                                        (uint16_t)(JEDEC_OctalDdr.Param_DWORD.D7.LenghtCommand - 1u)))
     {
       retr = EXTMEM_SFDP_ERROR_OCTALMODE;
       goto error;
     }
  }
error:
  return retr;
}

/**
 * @brief this function writes the config register to set dummy cycle
 * @param Object memory Object
 * @return @ref SFDP_StatusTypeDef
 */
SFDP_StatusTypeDef sfpd_set_dummycycle(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object, uint32_t DummyValue)
{
const uint8_t MaskWaitStateValue[4] = { 0x3u, 0x7u, 0xFu, 0x1Fu };
SFDP_StatusTypeDef retr = EXTMEM_SFDP_OK;
uint8_t localValue[2];
uint32_t Address;

  if ((JEDEC_SCCR_Map.Param_DWORD.D10.BitAccessedByCommandsUsingAddress != 1u) ||
     (JEDEC_SCCR_Map.Param_DWORD.D10.LocalAddress != 1u))
  {
    /* Not yet supported */
    retr = EXTMEM_SFDP_ERROR_DRIVER;
    goto error;
  }
  
  /* calcul the Address */
  Address = JEDEC_SCCR_Map.Param_DWORD.D10.AdressRegisterOrModesSupported * 0x100u;
  
  /* Read the configuration */
  if (HAL_OK != SAL_XSPI_Read(&Object->sfpd_private.SALObject, (uint8_t)JEDEC_SCCR_Map.Param_DWORD.D10.CommandReadAccess, Address, (uint8_t *)localValue, 2u))
  {
    retr = EXTMEM_SFDP_ERROR_DRIVER;
    goto error;
  }
      
  /* send command to write enable */
  if (HAL_OK != SAL_XSPI_SendCommand(&Object->sfpd_private.SALObject,
                                     Object->sfpd_private.DriverInfo.WriteWELCommand, NULL, 0u))
  {
    retr = EXTMEM_SFDP_ERROR_DRIVER;
    goto error;
  }

  /* control the write enable */
  if (HAL_OK != SAL_XSPI_CheckStatusRegister(&Object->sfpd_private.SALObject,
                                            Object->sfpd_private.DriverInfo.ReadWELCommand,
                                            Object->sfpd_private.DriverInfo.WELAddress,
                                            ((Object->sfpd_private.DriverInfo.WELBusyPolarity == 0u) ? 1u: 0u) << Object->sfpd_private.DriverInfo.WELPosition,
                                            1u << Object->sfpd_private.DriverInfo.WELPosition,
                                            1000))
  {
    retr = EXTMEM_SFDP_ERROR_DRIVER;
    goto error;
  }
      
  /* clear the value */
  localValue[0] = localValue[0] & ~(MaskWaitStateValue[JEDEC_SCCR_Map.Param_DWORD.D10.NumberBitsUsedToSetWaitStates] << JEDEC_SCCR_Map.Param_DWORD.D10.BitLocationLSBPhysicalBitsRegister);
                                    
  /* Apply the value with the mask */
  localValue[0] = localValue[0] | (uint8_t)(DummyValue << JEDEC_SCCR_Map.Param_DWORD.D10.BitLocationLSBPhysicalBitsRegister);
  localValue[1] = localValue[0];
      
  /* Write de configuration */
  if (HAL_OK != SAL_XSPI_Write(&Object->sfpd_private.SALObject, (uint8_t)JEDEC_SCCR_Map.Param_DWORD.D10.CommandWriteAccess, Address, (uint8_t *)localValue, 2u))
  {
    retr = EXTMEM_SFDP_ERROR_DRIVER;
    goto error;
  }

  /* Read the configuration, line can be removed it is only used for debug purpose */
  if (HAL_OK != SAL_XSPI_Read(&Object->sfpd_private.SALObject, (uint8_t)JEDEC_SCCR_Map.Param_DWORD.D10.CommandReadAccess, Address, (uint8_t *)localValue, 2u))
  {
    retr = EXTMEM_SFDP_ERROR_DRIVER;
    goto error;
  }	
  
error :
  return retr;  
}

/**
  * @}
  */

/**
  * @}
  */

 #endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */
