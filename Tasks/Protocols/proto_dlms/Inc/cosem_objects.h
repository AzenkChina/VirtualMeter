/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COSEM_OBJECTS_H__
#define __COSEM_OBJECTS_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "object_template.h"

/* Exported types ------------------------------------------------------------*/
/**
  * Class Data
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetValue;
    TypeObject  SetValue;
    
} ClassData;

/**
  * Class Register
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetValue;
    TypeObject  SetValue;
    TypeObject  GetScalerUnit;
    TypeObject  SetScalerUnit;
    
    TypeObject  Reset;
    
} ClassRegister;

/**
  * Class Extended Register
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetValue;
    TypeObject  SetValue;
    TypeObject  GetScalerUnit;
    TypeObject  SetScalerUnit;
    TypeObject  GetStatus;
    TypeObject  SetStatus;
    TypeObject  GetCaptureTime;
    TypeObject  SetCaptureTime;
    
    TypeObject  Reset;
    
} ClassExtRegister;

/**
  * Class Demand
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetCurrentAverageValue;
    TypeObject  SetCurrentAverageValue;
    TypeObject  GetLastAverageValue;
    TypeObject  SetLastAverageValue;
    TypeObject  GetScalerUnit;
    TypeObject  SetScalerUnit;
    TypeObject  GetStatus;
    TypeObject  SetStatus;
    TypeObject  GetCaptureTime;
    TypeObject  SetCaptureTime;
    TypeObject  GetStartTimeCurrent;
    TypeObject  SetStartTimeCurrent;
    TypeObject  GetPeriod;
    TypeObject  SetPeriod;
    TypeObject  GetNumberOfPeriods;
    TypeObject  SetNumberOfPeriods;

    TypeObject  Reset;
    TypeObject  NextPeriod;
    
} ClassDemand;

/**
  * Class Register Activation
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetRegisterAssignment;
    TypeObject  SetRegisterAssignment;
    TypeObject  GetMaskList;
    TypeObject  SetMaskList;
    TypeObject  GetActiveMask;
    TypeObject  SetActiveMask;

    TypeObject  AddRegister;
    TypeObject  AddMask;
    TypeObject  DeleteMask;
    
} ClassRegisterActivation;

/**
  * Class Profile Generic
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetBuffer;
    TypeObject  SetBuffer;
    TypeObject  GetCaptureObjects;
    TypeObject  SetCaptureObjects;
    TypeObject  GetCapturePeriod;
    TypeObject  SetCapturePeriod;
    TypeObject  GetSortMethod;
    TypeObject  SetSortMethod;
    TypeObject  GetSortObject;
    TypeObject  SetSortObject;
    TypeObject  GetEntriesInUse;
    TypeObject  SetEntriesInUse;
    TypeObject  GetProfileEntries;
    TypeObject  SetProfileEntries;
    
    TypeObject  Reset;
    TypeObject  Capture;
    
} ClassProfileGeneric;

/**
  * Class Clock
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetTime;
    TypeObject  SetTime;
    TypeObject  GetTimeZone;
    TypeObject  SetTimeZone;
    TypeObject  GetStatus;
    TypeObject  SetStatus;
    TypeObject  GetDaylightSavingsBegin;
    TypeObject  SetDaylightSavingsBegin;
    TypeObject  GetDaylightSavingsEnd;
    TypeObject  SetDaylightSavingsEnd;
    TypeObject  GetDaylightSavingsDeviation;
    TypeObject  SetDaylightSavingsDeviation;
    TypeObject  GetDaylightSavingsEnabled;
    TypeObject  SetDaylightSavingsEnabled;
    TypeObject  GetClockBase;
    TypeObject  SetClockBase;

    TypeObject  AdjusttoQuarter;
    TypeObject  AdjusttoMeasuringPeriod;
    TypeObject  AdjusttoMinute;
    TypeObject  AdjusttoPresetTime;
    TypeObject  PresetAdjustingTime;
    TypeObject  ShiftTime;
    
} ClassClock;

/**
  * Class AssociationLN
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetObjectList;
    TypeObject  SetObjectList;
    TypeObject  GetAssociatedPartnersID;
    TypeObject  SetAssociatedPartnersID;
    TypeObject  GetApplicationContextName;
    TypeObject  SetApplicationContextName;
    TypeObject  GetXDLMSContextInfo;
    TypeObject  SetXDLMSContextInfo;
    TypeObject  GetAuthenticationMechanismName;
    TypeObject  SetAuthenticationMechanismName;
    TypeObject  GetSecret;
    TypeObject  SetSecret;
    TypeObject  GetAssociationStatus;
    TypeObject  SetAssociationStatus;
    TypeObject  GetSecuritySetupReference;
    TypeObject  SetSecuritySetupReference;
    TypeObject  GetUserList;
    TypeObject  SetUserList;
    TypeObject  GetCurrentUser;
    TypeObject  SetCurrentUser;
    
    TypeObject  ReplytoHLSAuthentication;
    TypeObject  ChangeHLSSecret;
    TypeObject  AddObject;
    TypeObject  RemoveObject;
    TypeObject  AddUser;
    TypeObject  RemoveUser;
    
} ClassAssociationLN;

/**
  * Class ImageTransfer
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetBlockSize;
    TypeObject  SetBlockSize;
    TypeObject  GetTransferredBlocksStatus;
    TypeObject  SetTransferredBlocksStatus;
    TypeObject  GetFirstNotTransferredBlockNumber;
    TypeObject  SetFirstNotTransferredBlockNumber;
    TypeObject  GetTransferEnabled;
    TypeObject  SetTransferEnabled;
    TypeObject  GetTransferStatus;
    TypeObject  SetTransferStatus;
    TypeObject  GetActivateInfo;
    TypeObject  SetActivateInfo;
    
    TypeObject  Initiate;
    TypeObject  BlockTransfer;
    TypeObject  Verify;
    TypeObject  Activate;
    
} ClassImageTransfer;

/**
  * Class SAP Assignment
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetSAPAssignmentList;
    TypeObject  SetSAPAssignmentList;
    
    TypeObject  ConnectLogicalDevice;
    
} ClassSAPAssignment;

/**
  * Class HDLC Setup
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetCommSpeed;
    TypeObject  SetCommSpeed;
    TypeObject  GetWindowSizeTransmit;
    TypeObject  SetWindowSizeTransmit;
    TypeObject  GetWindowSizeReceive;
    TypeObject  SetWindowSizeReceive;
    TypeObject  GetMaxInfoFieldLengthTransmit;
    TypeObject  SetMaxInfoFieldLengthTransmit;
    TypeObject  GetMaxInfoFieldLengthReceive;
    TypeObject  SetMaxInfoFieldLengthReceive;
    TypeObject  GetInterOctetTimeOut;
    TypeObject  SetInterOctetTimeOut;
    TypeObject  GetInactivityTimeOut;
    TypeObject  SetInactivityTimeOut;
    TypeObject  GetDeviceAddress;
    TypeObject  SetDeviceAddress;
    
} ClassHDLCSetup;

/**
  * Class Utility Tables
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetTableID;
    TypeObject  SetTableID;
    TypeObject  GetLength;
    TypeObject  SetLength;
    TypeObject  GetBuffer;
    TypeObject  SetBuffer;

} ClassUtilityTables;

/**
  * Class Push Setup
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetPushObjectList;
    TypeObject  SetPushObjectList;
    TypeObject  GetSendDestinationAndMethod;
    TypeObject  SetSendDestinationAndMethod;
    TypeObject  GetCommunicationWindow;
    TypeObject  SetCommunicationWindow;
    TypeObject  GetRandomisationStartInterval;
    TypeObject  SetRandomisationStartInterval;
    TypeObject  GetNumberofRetries;
    TypeObject  SetNumberofRetries;
    TypeObject  GetRepetitionDelay;
    TypeObject  SetRepetitionDelay;
    
    TypeObject  Push;
    
} ClassPushSetup;

/**
  * Class Register Table
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetTableCellValues;
    TypeObject  SetTableCellValues;
    TypeObject  GetTableCellDefinition;
    TypeObject  SetTableCellDefinition;
    TypeObject  GetScalerUnit;
    TypeObject  SetScalerUnit;
    
    TypeObject  Reset;
    TypeObject  Capture;

} ClassRegisterTable;

/**
  * Class Status Mapping
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetStatusWord;
    TypeObject  SetStatusWord;
    TypeObject  GetMappingTable;
    TypeObject  SetMappingTable;

} ClassStatusMapping;

/**
  * Class Security Setup
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    TypeObject  GetSecurityPolicy;
    TypeObject  SetSecurityPolicy;
    TypeObject  GetSecuritySuite;
    TypeObject  SetSecuritySuite;
    TypeObject  GetClientSystemTitle;
    TypeObject  SetClientSystemTitle;
    TypeObject  GetServerSystemTitle;
    TypeObject  SetServerSystemTitle;
    TypeObject  GetCertificates;
    TypeObject  SetCertificates;
    TypeObject  GetSecurityActivate;
    TypeObject  SetSecurityActivate;

    TypeObject  KeyTransfer;
    TypeObject  KeyAgreement;
    TypeObject  GenerateKeyPair;
    TypeObject  GenerateCertificateRequest;
    TypeObject  ImportCertificate;
    TypeObject  ExportCertificate;
    TypeObject  RemoveCertificate;
    
} ClassSecuritySetup;






/* Exported constants --------------------------------------------------------*/
#define MOTIV_GET       ((uint8_t)0x0f)
#define MOTIV_SET       ((uint8_t)0xf0)

/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern TypeObject CosemLoadAttribute(uint16_t ClassID, uint8_t Index, uint8_t Motive);
extern TypeObject CosemLoadMethod(uint16_t ClassID, uint8_t Index);

#endif /* __COSEM_OBJECTS_H__ */
