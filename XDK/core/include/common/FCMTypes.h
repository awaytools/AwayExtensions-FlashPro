/******************************************************************************
* ADOBE CONFIDENTIAL
* ___________________
*
*  Copyright [2013] Adobe Systems Incorporated
*  All Rights Reserved.
*
* NOTICE:  All information contained herein is, and remains
* the property of Adobe Systems Incorporated and its suppliers,
* if any.  The intellectual and technical concepts contained
* herein are proprietary to Adobe Systems Incorporated and its
* suppliers and are protected by all applicable intellectual 
* property laws, including trade secret and copyright laws.
* Dissemination of this information or reproduction of this material
* is strictly forbidden unless prior written permission is obtained
* from Adobe Systems Incorporated.
******************************************************************************/


#ifndef FCM_TYPES_H_
#define FCM_TYPES_H_

#include "FCMPreConfig.h"


namespace FCM
{
    
    #if defined(__GNUC__) && defined(__APPLE__)
        typedef u_int8_t                U_Int8;
        typedef int8_t                  S_Int8;
        typedef u_int16_t               U_Int16;
        typedef int16_t                 S_Int16;
        typedef u_int32_t               U_Int32;
        typedef int32_t                 S_Int32;
        typedef u_int64_t               U_Int64;
        typedef int64_t                 S_Int64;
    #else //Windows
        typedef unsigned    _int8       U_Int8;
        typedef signed      _int8       S_Int8;
        typedef unsigned    _int16      U_Int16; 
        typedef signed      _int16      S_Int16;
        typedef unsigned    _int32      U_Int32;
        typedef signed      _int32      S_Int32;
        typedef unsigned    _int64      U_Int64;
        typedef signed      _int64      S_Int64;
    #endif
        
    typedef float                   Float;
    typedef double                  Double;


    typedef U_Int32                 Result;
    typedef U_Int8                  Byte;
    typedef U_Int8                  Boolean;
    typedef void                    *PVoid;
    typedef void                    ** PPVoid;    
    typedef U_Int16*                StringRep16;
    typedef const U_Int16*          CStringRep16;
    typedef char*                   StringRep8;
    typedef const char*           CStringRep8;

    struct FCMGUID{
        FCM::U_Int32    Data1;
        FCM::U_Int16    Data2;
        FCM::U_Int16    Data3;
        FCM::Byte       Data4[8];    
    };

    typedef enum {
        kFCMVarype_Invalid = 0,     /* illegal. */
        kFCMVarype_UInt32,          /* Returns a FCM::S_Int32. */
        kFCMVarype_Float,           /* Returns a 32 bit IEEE single precision floating point number. */
        kFCMVarype_Bool,            /* FCM::Boolean */
        kFCMVarype_CString,         /* Null-terminated C String */
        kFCMVarype_Double,          /* FCM::Double */
        _kFCMVarype_Last_
    } VarType;

    struct VARIANT
    {
        VarType    m_type;
        union
        {
            U_Int32         uVal;
            Float           fVal;
            Boolean         bVal;
            StringRep16     strVal;
            Double          dVal;
            PVoid           reserved;
        }m_value;
    };
        

    __inline bool operator==(const FCMGUID& guidOne, const FCMGUID& guidOther)
    {
        return ((guidOne.Data1 == guidOther.Data1) &&
            (guidOne.Data2 == guidOther.Data2) &&
            (guidOne.Data3 == guidOther.Data3) &&
            (* (U_Int32*) guidOne.Data4 == *(U_Int32*)guidOther.Data4) &&
            (* (U_Int32*) (guidOne.Data4+4) == *( (U_Int32*) (guidOther.Data4+4)) ) );
    }

    __inline bool operator!=(const FCMGUID& guidOne, const FCMGUID& guidOther)
    {
        return !(guidOne == guidOther);
    }

    typedef FCMGUID FCMIID;
    typedef FCMGUID FCMCLSID;
    typedef FCMGUID SRVCID;



    typedef const FCMIID        ConstFCMIID;
    typedef const FCMIID        ConstFCMCLSID;
    typedef ConstFCMCLSID&      ConstRefFCMIID;
    typedef ConstFCMCLSID&      ConstRefFCMCLSID;
    
    
    typedef FCMGUID             FCMNotifyID, * PFCMNotifyID;
    typedef const FCMNotifyID   ConstFCMNotifyID;
    typedef const FCMNotifyID & ConstRefFCMNotifyID;


    const FCMIID FCMIID_NULL        = { 0, 0, 0, { 0, 0,  0,  0,  0,  0,  0,  0 } };
    const FCMCLSID FCMCLSID_NULL        = { 0, 0, 0, { 0, 0,  0,  0,  0,  0,  0,  0 } };


    template<class T>
    struct FCMIID_Traits{ static FCMIID GetIID() { return FCMIID_NULL; } }; 


}; //namespace FCM

#include "FCMPostConfig.h"

#endif // FCM_TYPES_H_

