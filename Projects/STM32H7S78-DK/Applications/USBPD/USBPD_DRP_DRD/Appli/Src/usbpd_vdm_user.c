/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_vdm_user.c
  * @author  MCD Application Team
  * @brief   USBPD provider demo file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbpd_core.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_vdm_user.h"
#include "usbpd_dpm_user.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/** @addtogroup STM32_USBPD_APPLICATION
  * @{
  */

/** @addtogroup STM32_USBPD_APPLICATION_VDM_USER
  * @{
  */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Private_define */
#define MAX_SVID_USER   1
/* USER CODE END Private_define */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN Private_typedef */
/*
 * Structure to SVID supported by the devices
 */
typedef struct
{
  uint32_t  NumSVIDs;
  uint16_t  SVIDs[MAX_SVID_USER];
}USBPD_SVIDUSerInfo_TypeDef;

/* USER CODE END Private_typedef */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Private_macro */

/* USER CODE END Private_macro */

/* Private function prototypes -----------------------------------------------*/
static USBPD_StatusTypeDef USBPD_VDM_DiscoverIdentity(uint8_t PortNum, USBPD_DiscoveryIdentity_TypeDef *pIdentity);
static USBPD_StatusTypeDef USBPD_VDM_DiscoverSVIDs(uint8_t PortNum, uint16_t **p_SVID_Info, uint8_t *nb);
static USBPD_StatusTypeDef USBPD_VDM_DiscoverModes(uint8_t PortNum, uint16_t SVID, uint32_t **p_ModeInfo, uint8_t *nbMode);
static USBPD_StatusTypeDef USBPD_VDM_ModeEnter(uint8_t PortNum, uint16_t SVID, uint32_t ModeIndex);
static USBPD_StatusTypeDef USBPD_VDM_ModeExit(uint8_t PortNum, uint16_t SVID, uint32_t ModeIndex);
static void USBPD_VDM_SendAttention(uint8_t PortNum, uint8_t *NbData, uint32_t *VDO);
static void USBPD_VDM_ReceiveAttention(uint8_t PortNum, uint8_t NbData, uint32_t VDO);
static USBPD_StatusTypeDef USBPD_VDM_ReceiveSpecific(uint8_t PortNum, USBPD_VDM_Command_Typedef VDMCommand, uint8_t *NbData, uint32_t *VDO);
static void USBPD_VDM_InformIdentity(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, USBPD_DiscoveryIdentity_TypeDef *pIdentity);
static void USBPD_VDM_InformSVID(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, USBPD_SVIDInfo_TypeDef *pListSVID);
static void USBPD_VDM_InformMode(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, USBPD_ModeInfo_TypeDef *pModesInfo);
static void USBPD_VDM_InformModeEnter(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, uint16_t SVID, uint32_t ModeIndex);
static void USBPD_VDM_InformModeExit(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, uint16_t SVID, uint32_t ModeIndex);
static void USBPD_VDM_InformSpecific(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_Command_Typedef VDMCommand, uint8_t *NbData, uint32_t *VDO);
static void USBPD_VDM_SendSpecific(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_Command_Typedef VDMCommand, uint8_t *NbData, uint32_t *VDO);
static void USBPD_VDM_SendUVDM(uint8_t PortNum, USBPD_UVDMHeader_TypeDef *pUVDM_Header, uint8_t *pNbData, uint32_t *pVDO);
static USBPD_StatusTypeDef USBPD_VDM_ReceiveUVDM(uint8_t PortNum, USBPD_UVDMHeader_TypeDef UVDM_Header, uint8_t *pNbData, uint32_t *pVDO);

void USBPD_PE_InitVDM_Callback(uint8_t PortNum, USBPD_VDM_Callbacks *VDMCallbacks);
/* USER CODE BEGIN Private_prototypes */

/* USER CODE END Private_prototypes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN Private_variables */
USBPD_VDM_SettingsTypeDef DPM_VDM_Settings[USBPD_PORT_COUNT] =
{
  {
    .VDM_XID_SOP                = USBPD_XID,    /*!< A decimal number assigned by USB-IF before certification */
    .VDM_USB_VID_SOP            = USBPD_VID,    /*!< A decimal number assigned by USB-IF before certification */
    .VDM_PID_SOP                = USBPD_PID,    /*!< A unique number assigned by the Vendor ID holder identifying the product. */
    .VDM_ModalOperation         = MODAL_OPERATION_SUPPORTED, /*!< Product support Modes based on @ref USBPD_ModalOp_TypeDef */
    .VDM_bcdDevice_SOP          = 0xAAAAu,        /*!< A unique number assigned by the Vendor ID holder containing identity information relevant to the release version of the product. */
    .VDM_USBHostSupport         = USB_NOTCAPABLE, /*!< Indicates whether the UUT is capable of enumerating USB Host */
    .VDM_USBDeviceSupport       = USB_NOTCAPABLE, /*!< Indicates whether the UUT is capable of enumerating USB Devices */
    .VDM_ProductTypeUFPorCP     = PRODUCT_TYPE_PSD, /*!< Product type UFP or CablePlug of the UUT based on @ref USBPD_ProductType_TypeDef */
    .VDM_ProductTypeDFP         = PRODUCT_TYPE_UNDEFINED, /*!< Product type DFP of the UUT based on @ref USBPD_ProductType_TypeDef */
  }
};


const USBPD_VDM_Callbacks vdmCallbacks =
{
  USBPD_VDM_DiscoverIdentity,
  USBPD_VDM_DiscoverSVIDs,
  USBPD_VDM_DiscoverModes,
  USBPD_VDM_ModeEnter,
  USBPD_VDM_ModeExit,
  USBPD_VDM_InformIdentity,
  USBPD_VDM_InformSVID,
  USBPD_VDM_InformMode,
  USBPD_VDM_InformModeEnter,
  USBPD_VDM_InformModeExit,
  USBPD_VDM_SendAttention,
  USBPD_VDM_ReceiveAttention,
  USBPD_VDM_SendSpecific,
  USBPD_VDM_ReceiveSpecific,
  USBPD_VDM_InformSpecific,
  USBPD_VDM_SendUVDM,
  USBPD_VDM_ReceiveUVDM,
};

extern USBPD_ParamsTypeDef DPM_Params[USBPD_PORT_COUNT];
uint8_t VDM_Mode_On[USBPD_PORT_COUNT];

USBPD_IDHeaderVDO_TypeDef IDHeaderVDO[USBPD_PORT_COUNT];

USBPD_DiscoveryIdentity_TypeDef sIdentity[USBPD_PORT_COUNT];

uint16_t Remote_CurrentSVID[USBPD_PORT_COUNT];
uint16_t Remote_SVID_Mode[USBPD_PORT_COUNT];

USBPD_SVIDUSerInfo_TypeDef SVIDInfo[USBPD_PORT_COUNT];
/* USER CODE END Private_variables */

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  VDM Discovery identity callback
  * @note   Function is called to get Discovery identity information linked to the device and answer
  *         to SVDM Discovery identity init message sent by port partner
  * @param  PortNum current port number
  * @param  pIdentity Pointer on @ref USBPD_DiscoveryIdentity_TypeDef structure
  * @retval USBPD status: @ref USBPD_ACK or @ref USBPD_BUSY
  */
static USBPD_StatusTypeDef USBPD_VDM_DiscoverIdentity(uint8_t PortNum, USBPD_DiscoveryIdentity_TypeDef *pIdentity)
{
/* USER CODE BEGIN USBPD_VDM_DiscoverIdentity */
  IDHeaderVDO[PortNum].d32                      = 0;
  if ((DPM_Params[PortNum].PE_SpecRevision) > USBPD_SPECIFICATION_REV2)
  {
    IDHeaderVDO[PortNum].b30.VID                  = DPM_VDM_Settings[PortNum].VDM_USB_VID_SOP;
    /* Connector Type field set to USB Type-C Receptacle when the UUT was not a cable or Captive_Cable */
    IDHeaderVDO[PortNum].b30.ConnectorType        = CONNECTOR_TYPE_C_RECEPTACLE;
    IDHeaderVDO[PortNum].b30.ModalOperation       = DPM_VDM_Settings[PortNum].VDM_ModalOperation;
    IDHeaderVDO[PortNum].b30.USBHostCapability    = DPM_VDM_Settings[PortNum].VDM_USBHostSupport;
    IDHeaderVDO[PortNum].b30.USBDevCapability     = DPM_VDM_Settings[PortNum].VDM_USBDeviceSupport;
    IDHeaderVDO[PortNum].b30.ProductTypeUFPorCP   = DPM_VDM_Settings[PortNum].VDM_ProductTypeUFPorCP;
    IDHeaderVDO[PortNum].b30.ProductTypeDFP       = DPM_VDM_Settings[PortNum].VDM_ProductTypeDFP;
  }
  else
  {
    IDHeaderVDO[PortNum].b20.VID                    = DPM_VDM_Settings[PortNum].VDM_USB_VID_SOP;
    IDHeaderVDO[PortNum].b20.ModalOperation         = DPM_VDM_Settings[PortNum].VDM_ModalOperation;
    IDHeaderVDO[PortNum].b20.USBHostCapability      = DPM_VDM_Settings[PortNum].VDM_USBHostSupport;
    IDHeaderVDO[PortNum].b20.USBDevCapability       = DPM_VDM_Settings[PortNum].VDM_USBDeviceSupport;
    /* Not allowed to set at reserved value in case of PD.2.0. Check the value possible for P2.0 */
    IDHeaderVDO[PortNum].b20.ProductTypeUFPorCP   = PRODUCT_TYPE_NOT_UFP;
    if ((PRODUCT_TYPE_HUB == DPM_VDM_Settings[PortNum].VDM_ProductTypeUFPorCP)
     || (PRODUCT_TYPE_PERIPHERAL== DPM_VDM_Settings[PortNum].VDM_ProductTypeUFPorCP)
     || (PRODUCT_TYPE_AMA == DPM_VDM_Settings[PortNum].VDM_ProductTypeUFPorCP))
    {
      IDHeaderVDO[PortNum].b20.ProductTypeUFPorCP   = DPM_VDM_Settings[PortNum].VDM_ProductTypeUFPorCP;
    }
  }
  sIdentity[PortNum].IDHeader.d32               = IDHeaderVDO[PortNum].d32;
  sIdentity[PortNum].CertStatVDO.b.XID          = DPM_VDM_Settings[PortNum].VDM_XID_SOP;
  sIdentity[PortNum].ProductVDO.b.USBProductId  = DPM_VDM_Settings[PortNum].VDM_PID_SOP;
  sIdentity[PortNum].ProductVDO.b.bcdDevice     = DPM_VDM_Settings[PortNum].VDM_bcdDevice_SOP;

  sIdentity[PortNum].CableVDO_Presence   = 0;/*!< Indicate Passive Cable VDO presence or not           */
  sIdentity[PortNum].AMA_VDO_Presence    = 0;/*!< Indicate Alternate Mode Adapter VDO presence or not  */
  sIdentity[PortNum].UFP_VDO_Presence    = 0; /*!< Indicate UFP VDO presence or not                     */
  sIdentity[PortNum].DFP_VDO_Presence    = 0; /*!< Indicate DFP VDO presence or not                     */
#if defined(USBPDCORE_VPD)
  sIdentity[PortNum].VPD_VDO_Presence    = 0; /*!< Indicate VPD VDO presence or not                     */
#endif /* USBPDCORE_VPD */

  if ((DPM_Params[PortNum].PE_SpecRevision) > USBPD_SPECIFICATION_REV2)
  {
    /* UFP VDO and DFP VDO should be filled in we support DRD */
    switch (DPM_VDM_Settings[PortNum].VDM_ProductTypeUFPorCP)
    {
    case PRODUCT_TYPE_HUB:        /*!< PDUSB Hub (UFP)                        */
    case PRODUCT_TYPE_PERIPHERAL: /*!< PDUSB Peripheral (UFP)                       */
      {
        /* Fill the UFP VDO */
        USBPD_UFPVdo_TypeDef      ufp_vdo =
        {
          .b.USB_HighestSpeed = USB2P0_ONLY,              /*!< USB Highest Speed Support           */
          .b.AlternateModes   = ALTERNATE_MODES_TBT3,     /*!< Alternate Modes based */
          .b.ConnectorType    = CONNECTOR_TYPE_C_RECEPTACLE,  /*!< Connector Type */
#if defined (USB_BILLBOARD)
          .b.DeviceCapability = DEVICE_CAPABILITY_USB2P0_BILLBOARD,/*!< Device Capability   */
#else
          .b.DeviceCapability = DEVICE_CAPABILITY_USB2P0, /*!< Device Capability   */
#endif /* USB_BILLBOARD */
          .b.UFPVDOVersion    = USBPD_VDM_VDO_UFP_VERSION_REV1P2, /*!< Version Number of the VDO (should be set to Version1.2)   */
        };
        sIdentity[PortNum].UFP_VDO_Presence     = 1;
        sIdentity[PortNum].UFP_VDO.d32          = ufp_vdo.d32;
        break;
      }
#if defined(USBPDCORE_VPD)
    case PRODUCT_TYPE_VPD:        /*!< VCONN-Powered USB Device (VPD) (UFP)   */
      {
        /* Fill the VPD VDO */
        USBPD_VPDVdo_TypeDef      vpd_vdo =
        {
          .b.ChargeThroughSupport   = VPD_CHARGE_THROUGH_NOT_SUPPORTED,  /*!< Charge Through Support           */
          .b.GroundImpedance        = 0,  /*!< Charge Through Support bit = 1b: Ground impedance through the VPD in 1 mOhm increments.
                                                                    Values less than 10 mOhm are Reserved and Shall Not be used.
                                                                    Charge Through Support bit = 0b: Reserved, Shall be set to zero */
          .b.VBUS_Impedance         = 0,  /*!< Charge Through Support bit = 1b: Vbus impedance through the VPD in 2 mOhm increments.
                                                                    Values less than 10 mOhm are Reserved and Shall Not be used.
                                                                    Charge Through Support bit = 0b: Reserved, Shall be set to zero   */
          .b.ChargeThroughCurrent   = VPD_CHARGE_THROUGH_NOT_SUPPORTED,  /*!< Charge Through Current Support            */
          .b.CableMaxVoltage        = VBUS_MAX_20V,  /*!< Cable maximum voltage                         */
          .b.VDO_Version            = USBPD_VDM_VDO_VPD_VERSION_REV1P0,  /*!< Version number  of the VPD VDO                */
          .b.FWVersion              = 0x1,  /*!< Cable FW version number (vendor defined)           */
          .b.HWVersion              = 0x1,  /*!< Cable HW version number (vendor defined)           */
        };
        sIdentity[PortNum].VPD_VDO_Presence     = 1;
        sIdentity[PortNum].VPD_VDO.d32          = vpd_vdo.d32;
        break;
      }
#endif /* USBPDCORE_VPD */
    case PRODUCT_TYPE_NOT_UFP: /*!< Undefined             */
    case PRODUCT_TYPE_PSD: /*!< PSD, e.g. power bank (UFP)             */
    default:
      break;
    }

    /* UFP VDO and DFP VDO should be filled in we support DRD */
    switch (DPM_VDM_Settings[PortNum].VDM_ProductTypeDFP)
    {
    case PRODUCT_TYPE_HUB:          /*!< PDUSB Hub (DFP)                        */
    case PRODUCT_TYPE_HOST:         /*!< PDUSB Host  (DFP)                      */
    case PRODUCT_TYPE_POWER_BRICK:  /*!< Power Brick (DFP)                      */
      {
        /* Fill the DFP VDO */
        USBPD_DFPVdo_TypeDef      dfp_vdo =
        {
          .b.PortNumber        = 0x1F,   /*!< Unique port number to identify a specific port on a multi-port device  */
          .b.ConnectorType     = CONNECTOR_TYPE_C_RECEPTACLE,   /*!< Connector Type                                                         */
          .b.HostCapability    = HOST_CAPABILITY_USB2P0,   /*!< Host Capability                                                        */
          .b.DFPVDOVersion     = USBPD_VDM_VDO_DFP_VERSION_REV1P1,   /*!< Version Number of the VDO (should be set to Version1.1)                */
        };
        sIdentity[PortNum].DFP_VDO_Presence     = 1;
        sIdentity[PortNum].DFP_VDO.d32          = dfp_vdo.d32;
        break;
      }
    case PRODUCT_TYPE_NOT_DFP:    /*!< Undefined             */
    default:
      break;
    }
  }

  *pIdentity = sIdentity[PortNum];

  return USBPD_ACK;
/* USER CODE END USBPD_VDM_DiscoverIdentity */
}

/**
  * @brief  VDM Discover SVID callback
  * @note   Function is called to retrieve SVID supported by device and answer
  *         to SVDM Discovery SVID init message sent by port partner
  * @param  PortNum        current port number
  * @param  p_SVID_Info Pointer on @ref USBPD_SVIDInfo_TypeDef structure
  * @param  pNbSVID     Pointer on number of SVID
  * @retval USBPD status  @ref USBPD_BUSY or @ref USBPD_ACK or @ref USBPD_NAK
  */
static USBPD_StatusTypeDef USBPD_VDM_DiscoverSVIDs(uint8_t PortNum, uint16_t **p_SVID_Info, uint8_t *pNbSVID)
{
/* USER CODE BEGIN USBPD_VDM_DiscoverSVIDs */
  return USBPD_NAK;
/* USER CODE END USBPD_VDM_DiscoverSVIDs */
}

/**
  * @brief  VDM Discover Mode callback (report all the modes supported by SVID)
  * @note   Function is called to report all the modes supported by selected SVID and answer
  *         to SVDM Discovery Mode init message sent by port partner
  * @param  PortNum current port number
  * @param  SVID         SVID value
  * @param  p_ModeTab    Pointer on the mode value
  * @param  NumberOfMode Number of mode available
  * @retval USBPD status
  */
static USBPD_StatusTypeDef USBPD_VDM_DiscoverModes(uint8_t PortNum, uint16_t SVID, uint32_t **p_ModeTab, uint8_t *NumberOfMode)
{
/* USER CODE BEGIN USBPD_VDM_DiscoverModes */
  return USBPD_NAK;
/* USER CODE END USBPD_VDM_DiscoverModes */
}

/**
  * @brief  VDM Mode enter callback
  * @note   Function is called to check if device can enter in the mode received for the selected SVID in the
  *         SVDM enter mode init message sent by port partner
  * @param  PortNum current port number
  * @param  SVID      SVID value
  * @param  ModeIndex Index of the mode to be entered
  * @retval USBPD status @ref USBPD_ACK/@ref USBPD_NAK
  */
static USBPD_StatusTypeDef USBPD_VDM_ModeEnter(uint8_t PortNum, uint16_t SVID, uint32_t ModeIndex)
{
/* USER CODE BEGIN USBPD_VDM_ModeEnter */
  return USBPD_NAK;
/* USER CODE END USBPD_VDM_ModeEnter */
}

/**
  * @brief  VDM Mode exit callback
  * @note   Function is called to check if device can exit from the mode received for the selected SVID in the
  *         SVDM exit mode init message sent by port partner
  * @param  PortNum current port number
  * @param  SVID      SVID value
  * @param  ModeIndex Index of the mode to be exited
  * @retval USBPD status @ref USBPD_ACK/@ref USBPD_NAK
  */
static USBPD_StatusTypeDef USBPD_VDM_ModeExit(uint8_t PortNum, uint16_t SVID, uint32_t ModeIndex)
{
/* USER CODE BEGIN USBPD_VDM_ModeExit */
  return USBPD_NAK;
/* USER CODE END USBPD_VDM_ModeExit */
}

/**
  * @brief  Send VDM Attention message callback
  * @note   Function is called when device wants to send a SVDM attention message to port partner
  *         (for instance DP status can be filled through this function)
  * @param  PortNum    current port number
  * @param  pNbData    Pointer of number of VDO to send
  * @param  pVDO       Pointer of VDO to send
  * @retval None
  */
static void USBPD_VDM_SendAttention(uint8_t PortNum, uint8_t *pNbData, uint32_t *pVDO)
{
/* USER CODE BEGIN USBPD_VDM_SendAttention */

/* USER CODE END USBPD_VDM_SendAttention */
}

/**
  * @brief  Receive VDM Attention callback
  * @note   Function is called when a SVDM attention init message has been received from port partner
  *         (for instance, save DP status data through this function)
  * @param  PortNum   current port number
  * @param  NbData    Number of received VDO
  * @param  VDO       Received VDO
  * @retval None
  */
static void USBPD_VDM_ReceiveAttention(uint8_t PortNum, uint8_t NbData, uint32_t VDO)
{
/* USER CODE BEGIN USBPD_VDM_ReceiveAttention */

/* USER CODE END USBPD_VDM_ReceiveAttention */
}

/**
  * @brief  VDM Receive Specific message callback
  * @note   Function is called to answer to a SVDM specific init message received by port partner.
  *         (for instance, retrieve DP status or DP configure data through this function)
  * @param  PortNum     Current port number
  * @param  VDMCommand  VDM command based on @ref USBPD_VDM_Command_Typedef
  * @param  pNbData     Pointer of number of received VDO and used for the answer
  * @param  pVDO        Pointer of received VDO and use for the answer
  * @retval USBPD Status
  */
static USBPD_StatusTypeDef USBPD_VDM_ReceiveSpecific(uint8_t PortNum, USBPD_VDM_Command_Typedef VDMCommand, uint8_t *pNbData, uint32_t *pVDO)
{
/* USER CODE BEGIN USBPD_VDM_ReceiveSpecific */
  return USBPD_NAK;
/* USER CODE END USBPD_VDM_ReceiveSpecific */
}

/**
  * @brief  Inform identity callback
  * @note   Function is called to save Identity information received in Discovery identity from port partner
            (answer to SVDM discovery identity sent by device)
  * @param  PortNum       current port number
  * @param  SOPType       SOP type
  * @param  CommandStatus Command status based on @ref USBPD_VDM_CommandType_Typedef
  * @param  pIdentity     Pointer on the discovery identity information based on @ref USBPD_DiscoveryIdentity_TypeDef
  * @retval None
*/
static void USBPD_VDM_InformIdentity(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, USBPD_DiscoveryIdentity_TypeDef *pIdentity)
{
/* USER CODE BEGIN USBPD_VDM_InformIdentity */
  switch(CommandStatus)
  {
  case SVDM_RESPONDER_ACK :
    break;
  case SVDM_RESPONDER_NAK :
    break;
  case SVDM_RESPONDER_BUSY :
    break;
  default :
    break;
  }
/* USER CODE END USBPD_VDM_InformIdentity */
}

/**
  * @brief  Inform SVID callback
  * @note   Function is called to save list of SVID received in Discovery SVID from port partner
            (answer to SVDM discovery SVID sent by device)
  * @param  PortNum       current port number
  * @param  SOPType       SOP type
  * @param  CommandStatus Command status based on @ref USBPD_VDM_CommandType_Typedef
  * @param  pListSVID     Pointer of list of SVID based on @ref USBPD_SVIDInfo_TypeDef
  * @retval None
  */
static void USBPD_VDM_InformSVID(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, USBPD_SVIDInfo_TypeDef *pListSVID)
{
/* USER CODE BEGIN USBPD_VDM_InformSVID */
  switch(CommandStatus)
  {
  case SVDM_RESPONDER_ACK :
    break;
  case SVDM_RESPONDER_NAK :
    break;
  case SVDM_RESPONDER_BUSY :
    break;
  default :
      break;
  }
/* USER CODE END USBPD_VDM_InformSVID */
}

/**
  * @brief  Inform Mode callback ( received in Discovery Modes ACK)
  * @note   Function is called to save list of modes linked to SVID received in Discovery mode from port partner
            (answer to SVDM discovery mode sent by device)
  * @param  PortNum         current port number
  * @param  SOPType         SOP type
  * @param  CommandStatus   Command status based on @ref USBPD_VDM_CommandType_Typedef
  * @param  pModesInfo      Pointer of Modes info based on @ref USBPD_ModeInfo_TypeDef
  * @retval None
  */
static void USBPD_VDM_InformMode(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, USBPD_ModeInfo_TypeDef *pModesInfo)
{
/* USER CODE BEGIN USBPD_VDM_InformMode */
  switch(CommandStatus)
  {
  case SVDM_RESPONDER_ACK :
    break;
  case SVDM_RESPONDER_NAK :
    break;
  case SVDM_RESPONDER_BUSY :
    break;
  default :
    break;
  }
/* USER CODE END USBPD_VDM_InformMode */
}

/**
  * @brief  Inform Mode enter callback
  * @note   Function is called to inform if port partner accepted or not to enter in the mode
  *         specified in the SVDM enter mode sent by the device
  * @param  PortNum   current port number
  * @param  SOPType       SOP type
  * @param  CommandStatus Command status based on @ref USBPD_VDM_CommandType_Typedef
  * @param  SVID      SVID ID
  * @param  ModeIndex Index of the mode to be entered
  * @retval None
  */
static void USBPD_VDM_InformModeEnter(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, uint16_t SVID, uint32_t ModeIndex)
{
/* USER CODE BEGIN USBPD_VDM_InformModeEnter */
  switch(CommandStatus)
  {
  case SVDM_RESPONDER_ACK :
    break;
  case SVDM_RESPONDER_NAK :
    break;
  case SVDM_RESPONDER_BUSY :
    /* retry in 50ms */
    break;
  default :
    break;
  }
/* USER CODE END USBPD_VDM_InformModeEnter */
}

/**
  * @brief  Inform Exit enter callback
  * @param  PortNum   current port number
  * @param  SVID      SVID ID
  * @param  ModeIndex Index of the mode to be entered
  * @retval None
  */
static void USBPD_VDM_InformModeExit(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, uint16_t SVID, uint32_t ModeIndex)
{
/* USER CODE BEGIN USBPD_VDM_InformModeExit */
  switch(CommandStatus)
  {
  case SVDM_RESPONDER_ACK :
    break;
  case SVDM_RESPONDER_NAK :
    break;
  case SVDM_RESPONDER_BUSY :
    /* retry in 50ms */
    break;
  default :
    break;
  }
/* USER CODE END USBPD_VDM_InformModeExit */
}

/**
  * @brief  VDM Send Specific message callback
  * @note   Function is called when device wants to send a SVDM specific init message to port partner
  *         (for instance DP status or DP configure can be filled through this function)
  * @param  PortNum    current port number
  * @param  SOPType    SOP type
  * @param  VDMCommand VDM command based on @ref USBPD_VDM_Command_Typedef
  * @param  pNbData    Pointer of number of VDO to send
  * @param  pVDO       Pointer of VDO to send
  * @retval None
  */
static void USBPD_VDM_SendSpecific(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_Command_Typedef VDMCommand, uint8_t *pNbData, uint32_t *pVDO)
{
/* USER CODE BEGIN USBPD_VDM_SendSpecific */

/* USER CODE END USBPD_VDM_SendSpecific */
}

/**
  * @brief  VDM Specific message callback to inform user of reception of VDM specific message
  * @note   Function is called when answer from SVDM specific init message has been received by the device
  *         (for instance, save DP status and DP configure data through this function)
  * @param  PortNum    current port number
  * @param  SOPType    SOP type
  * @param  VDMCommand VDM command based on @ref USBPD_VDM_Command_Typedef
  * @param  pNbData    Pointer of number of received VDO
  * @param  pVDO       Pointer of received VDO
  * @retval None
  */
static void USBPD_VDM_InformSpecific(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_Command_Typedef VDMCommand, uint8_t *pNbData, uint32_t *pVDO)
{
/* USER CODE BEGIN USBPD_VDM_InformSpecific */

/* USER CODE END USBPD_VDM_InformSpecific */
}

/**
  * @brief  VDM Send Unstructured message callback
  * @note   Aim of this function is to fill the UVDM message which contains 1 VDM Header + 6 VDO
  *         This callback will be called when user requests to send a UVDM message thanks
  *         to USBPD_DPM_RequestUVDMMessage function
  * @param  PortNum       current port number
  * @param  pUVDM_Header  Pointer on UVDM header based on @ref USBPD_UVDMHeader_TypeDef
  * @param  pNbData       Pointer of number of VDO to send (max size must be equal to 6)
  * @param  pVDO          Pointer of VDO to send (up to 6 x uint32_t)
  * @retval None
  */
static void USBPD_VDM_SendUVDM(uint8_t PortNum, USBPD_UVDMHeader_TypeDef *pUVDM_Header, uint8_t *pNbData, uint32_t *pVDO)
{
/* USER CODE BEGIN USBPD_VDM_SendUVDM */

/* USER CODE END USBPD_VDM_SendUVDM */
}

/**
  * @brief  Unstructured VDM  message callback to inform user of reception of UVDM message
  * @param  PortNum    current port number
  * @param  UVDM_Header UVDM header based on @ref USBPD_UVDMHeader_TypeDef
  * @param  pNbData    Pointer of number of received VDO
  * @param  pVDO       Pointer of received VDO
  * @retval USBPD Status
  */
static USBPD_StatusTypeDef USBPD_VDM_ReceiveUVDM(uint8_t PortNum, USBPD_UVDMHeader_TypeDef UVDM_Header, uint8_t *pNbData, uint32_t *pVDO)
{
/* USER CODE BEGIN USBPD_VDM_ReceiveUVDM */
  return USBPD_ERROR;
/* USER CODE END USBPD_VDM_ReceiveUVDM */
}

/* USER CODE BEGIN Private_functions */

/* USER CODE END Private_functions */

/* Exported functions ---------------------------------------------------------*/
/**
  * @brief  VDM Initialization function
  * @param  PortNum     Index of current used port
  * @retval status
  */
USBPD_StatusTypeDef USBPD_VDM_UserInit(uint8_t PortNum)
{
/* USER CODE BEGIN USBPD_VDM_UserInit */
   /* Initialize SVID supported by consumer */
  SVIDInfo[PortNum].NumSVIDs = MAX_SVID_USER;

  USBPD_PE_InitVDM_Callback(PortNum, (USBPD_VDM_Callbacks *)&vdmCallbacks);

  return USBPD_OK;
}

/**
  * @brief  VDM Reset function
  * @param  PortNum     Index of current used port
  * @retval status
  */
void USBPD_VDM_UserReset(uint8_t PortNum)
{
/* USER CODE BEGIN USBPD_VDM_UserReset */

/* USER CODE END USBPD_VDM_UserReset */
}

/* USER CODE BEGIN Exported_functions */

/* USER CODE END Exported_functions */

/**
  * @}
  */

/**
  * @}
  */

