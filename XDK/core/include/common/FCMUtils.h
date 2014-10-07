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


#ifndef FCM_UTILS_H_
#define FCM_UTILS_H_

#include "FCMTypes.h"
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>

namespace FCM
{
    class FCMUtils
    {
    public:
        static std::string FCMGUIDToString(const FCMGUID & in)
        {
            std::ostringstream result;
            result.fill('0');
            result << std::hex;
            result  << std::setw(8) << ( in.Data1);
            result << "-";
            result  << std::setw(4) << ( in.Data2);
            result << "-";
            result  << std::setw(4) << ( in.Data3);
            result << "-";
            unsigned i=0;
            for(; i<2 ;++i)
            {
                result << std::setw(2) << (unsigned int) ( in.Data4[i]);
            }
            result << "-";
            for(; i< 8 ;++i)
            {
                result << std::setw(2) << (unsigned int) ( in.Data4[i]);
            }
            std::string guid_str = result.str();
            std::transform(guid_str.begin(), guid_str.end(), guid_str.begin(), ::toupper);
            return guid_str;
            
        }

        static FCMGUID FCMGUIDFromString(const std::string& inStr_)
        {
            std::string input = inStr_;
            std::string sep(1,'-');
            std::string empty;
            size_t pos = 0;
            while((pos = input.find(sep, pos)) != std::string::npos)
            {
                input.erase(pos,1);
            }
            assert(input.size() == 32);
 
            FCM::FCMGUID guid;
            
            std::stringstream(input.substr(0,8))>>std::hex>>guid.Data1;
            std::stringstream(input.substr(8,4))>>std::hex>>guid.Data2;
            std::stringstream(input.substr(12,4))>>std::hex>>guid.Data3;
            
            
            U_Int64 bytes;
            std::stringstream(input.substr(16,16))>>std::hex>>bytes;
            for(int j=7;j>=0;--j)
            {
                guid.Data4[j] = (FCM::Byte)bytes;
                bytes >>= 8;
            }

            return guid;
        }

    };
};
#endif // FCM_UTILS_H_