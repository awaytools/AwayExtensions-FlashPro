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


#ifndef FCM_PLUGIN_INTERFACE_H_
#define FCM_PLUGIN_INTERFACE_H_

#include "FCMPreConfig.h"
#include "FCMTypes.h"
#include "FCMErrors.h"
#include "FCMMacros.h"

#include "IFCMCalloc.h"
#include "IFCMCallback.h"
#include "IFCMList.h"
#include "IFCMClassFactory.h"
#include "IFCMPluginDictionary.h"
#include "IFCMNoAggregationUnknown.h"

#include "FCMPublicIDs.h"

#include "assert.h"

namespace FCM
{
    //Forward declare IFCMUnknown
    class IFCMUnknown;

    typedef struct {
        FCMCLSID        classID;
        FCM::U_Int32    classVersion;
        FCMIID          interfaceID;
    }    FCMClassInterfaceInfo;

    typedef FCMClassInterfaceInfo* PFCMClassInterfaceInfo;

    inline FCM::Result _Delegate(
        FCM::PVoid pv,
    FCMIID riid,
    FCM::PPVoid ppvObject,
    FCM::S_Int64 dw)
    {
        FCM::Result hRes = FCM_NO_INTERFACE;
        IFCMUnknown* p = *((IFCMUnknown**)((FCM::S_Int64)pv + dw));
        if (p != 0)
            hRes = p->QueryInterface(riid, ppvObject);
        return hRes;
    }

    typedef FCM::Result (_FCM_CREATORARGFUNC)(
    FCM::PVoid pv,
    FCMIID riid,
    FCM::PPVoid ppvObject,
    FCM::S_Int64 dw);

    class FCMPluginModule;

    typedef IFCMClassFactory* (*FactorCreatorProc)(FCMPluginModule *) ;

    
    typedef struct
    {
        FCMIID iid;
        FCM::S_Int64 offset;
        _FCM_CREATORARGFUNC* innerObjectFunc;
    }    FCMInterfaceMap;


    typedef FCMInterfaceMap* (*InterfaceMapGetProc)()  ;

    typedef struct
    {
        FCMCLSID clsid;
        FactorCreatorProc   pFactoryCreator;
        InterfaceMapGetProc pGetInterfaceTable;
        FCM::U_Int32 classVersion;
    } FLCMClassMap;

    
    


    template<typename T> //T should be derived from IFCMUnknown
    class AutoPtr
    {
    public:
       
        T* m_Ptr;
        
        AutoPtr()
        {
            this->m_Ptr=0;
        }
       
        AutoPtr(T* p)
        {
            this->m_Ptr=p;
           
            FCM_ADDREF(this->m_Ptr);
            
        }
        
        AutoPtr( const AutoPtr<T>& pObj)
        {
            this->m_Ptr=pObj.m_Ptr;
            
            FCM_ADDREF(this->m_Ptr);
        }
        
                
        template <typename Q> //Q should be derived from IFCMUnknown
        AutoPtr(Q* p)
        {
            this->m_Ptr=0;
            
            if(!IsSameUnknown(p))
                assignPtrWithQIOwnership(this->m_Ptr,p);
            
        }

        template <typename Q>
        AutoPtr( const AutoPtr<Q>& pObj)
        {
           this->m_Ptr=0;
            
           if(!IsSameUnknown(pObj.m_Ptr))
               assignPtrWithQIOwnership(this->m_Ptr,pObj.m_Ptr);
       
        }

        ~AutoPtr()
        {
            Reset();
        }
        operator T*() const
        {
            return this->m_Ptr;
        }
        
        T* operator->() const
        {
            assert(this->m_Ptr != 0);
            return (this->m_Ptr);
        }
        
        T** operator &()
        {
            //Trying to assign the new value. Check whether the object is detached.
            assert(this->m_Ptr == 0);
            return &this->m_Ptr;
        }
        operator bool() const
        {
            return (this->m_Ptr!=0);
        }
        bool operator != (T* p) const
        {
            return  (this->m_Ptr != p);
        }
        bool operator == (T* p) const
        {
            return  (this->m_Ptr == p);
        }
        void operator =(T* p)
        {
            if(this->m_Ptr != p)
            {
                assignPtrWithOwnership(this->m_Ptr,p);
            }
        }
        void operator =(const AutoPtr<T>& pObj)
        {
            if(this->m_Ptr != pObj.m_Ptr)
            {
                assignPtrWithOwnership(this->m_Ptr,pObj.m_Ptr);

            }
        }
        
        template <typename Q>
        void operator =(const AutoPtr<Q>& pObj)
        {
            if(!IsSameUnknown(pObj.m_Ptr))
            {
               assignPtrWithQIOwnership(this->m_Ptr,pObj.m_Ptr);
                
            }
        }
        template <typename Q>
        void operator =(Q* p)
        {
            if(!IsSameUnknown(p))
            {
                assignPtrWithQIOwnership(this->m_Ptr,p);
                
            }
        }
        //Detaches the pointer with release
        void Reset()
        {
            FCM_RELEASE(this->m_Ptr);
        }
        
         //Detaches the pointer without Release
        void Detach()
        {
             this->m_Ptr = 0;
        }
        
        //Attaches the pointer without AddRef
        void Attach(T* p)
        {
            Reset();
            this->m_Ptr = p;
            
        }
        
        template <typename Q>
        bool IsSameUnknown(Q* pArg)
        {
            if( (pArg==0) &&  (0==this->m_Ptr))
                return true;
            if( (pArg==0) ||  (0==this->m_Ptr))
                return false;
            
            PIFCMUnknown unkThis=0;
            this->m_Ptr->QueryInterface(IFCMUnknown::GetIID(),(FCM::PPVoid)&unkThis);
            
            PIFCMUnknown unkArg=0;
            pArg->QueryInterface(IFCMUnknown::GetIID(),(FCM::PPVoid)&unkArg);

            if(unkThis)
                unkThis->Release();
            if(unkArg)
                unkArg->Release();
            
            return (unkThis == unkArg);
            
        }
        
     private:

        void assignPtrWithOwnership ( T*& pDest,T* pSource)
        {
            Reset();
            
            FCM_ADDREF(pSource);
            
            pDest = pSource;
       }

        void assignPtrWithQIOwnership ( T*& pDest,PIFCMUnknown pSource)
        {
            Reset();
            
            if(pSource)
                pSource->QueryInterface(T::GetIID(),(PPVoid)&pDest);
            
        }
        

    };
   
    class FCMListPtr : public AutoPtr <IFCMList>
    {
    public:
           PIFCMUnknown operator [](FCM::U_Int32 index)
        {
            return this->m_Ptr->operator[](index);
        }
    };
    
      

    
    class FCMPluginModule
    {
    public:
        struct ClassNode
        {
            FLCMClassMap m_ClassMap;
            ClassNode* next;
        };

    protected:    
        ClassNode* m_firstNode;
        FCM::AutoPtr<FCM::IFCMCalloc> m_pCalloc;
        FCM::U_Int32                  m_LiveObjectCounter;
        
        FCMPluginModule()
        {
            m_firstNode=0;
            m_pCalloc=0;
            m_LiveObjectCounter=0;
        }
        virtual ~FCMPluginModule()
        {
        }

    public:
        
        FCM::U_Int32 incrementAliveCount()
        {
            return ++m_LiveObjectCounter;
        }
        FCM::U_Int32 decrementAliveCount()
        {
            assert(m_LiveObjectCounter>0);
            return --m_LiveObjectCounter;
        }

        FCM::Result init(FCM::PIFCMCallback pCallback)
        {
            AutoPtr<IFCMUnknown> pCallocUnk;
            FCM::Result res = pCallback->GetService(FCM::SRVCID_Core_Memory,pCallocUnk.m_Ptr);
            m_pCalloc=pCallocUnk;
            return res;
        }

        FCM::Result getClassInfo(FCM::PIFCMCalloc pCalloc,FCM::PFCMClassInterfaceInfo* ppClassInfo)
        {
                
            FCM::Result res= FCM_GENERAL_ERROR;
            FCM::U_Int32 pairCount=0;
            for( FCMPluginModule::ClassNode* pNode = m_firstNode; pNode != 0; pNode = pNode->next )
            {
                FCMInterfaceMap* pIntfIndex = pNode->m_ClassMap.pGetInterfaceTable();
                for(; pIntfIndex &&  (pIntfIndex->iid != FCMIID_NULL); ++pIntfIndex){
                    if(pIntfIndex->iid == IID_IFCMUnknown )
                        continue;
                    ++pairCount;
                }
            }
            PFCMClassInterfaceInfo arrClsIntfinfo = *ppClassInfo = (FCMClassInterfaceInfo *) pCalloc->Alloc((sizeof(FCMClassInterfaceInfo)*(pairCount+2)));
                    
            if(!arrClsIntfinfo)
                return FCM_MEM_NOT_AVAILABLE;

            arrClsIntfinfo[0].classID = FCMCLSID_NULL;
            arrClsIntfinfo[0].classVersion = (FCM::U_Int32)(FCM_VERSION);
            arrClsIntfinfo[0].interfaceID = FCMIID_NULL;

            arrClsIntfinfo[pairCount+1].classID = FCMCLSID_NULL;
            arrClsIntfinfo[pairCount+1].classVersion = 0;
            arrClsIntfinfo[pairCount+1].interfaceID = FCMIID_NULL;

            pairCount=0;
            for( FCMPluginModule::ClassNode* pNode = m_firstNode; pNode != 0; pNode = pNode->next )
            { 
                FCMInterfaceMap* pIntfIndex =  pNode->m_ClassMap.pGetInterfaceTable();
                for(; pIntfIndex && (pIntfIndex->iid != FCMIID_NULL); ++pIntfIndex)
                {
                    if(pIntfIndex->iid == IID_IFCMUnknown )
                        continue;
                    arrClsIntfinfo[pairCount+1].classID = pNode->m_ClassMap.clsid;
                    arrClsIntfinfo[pairCount+1].classVersion =pNode->m_ClassMap.classVersion;
                    arrClsIntfinfo[pairCount+1].interfaceID = pIntfIndex->iid;
                
                    ++pairCount;
                }
            }
            return FCM_SUCCESS;
        }

    
        FCM::Result getClassObject(FCM::PIFCMUnknown pUnkOuter,FCM::ConstRefFCMCLSID clsid, FCM::ConstRefFCMIID iid, FCM::PPVoid pAny)
        {
            using namespace FCM;
            FCM::Result res= FCM_GENERAL_ERROR;

        
            for( FCMPluginModule::ClassNode* pNode = m_firstNode; pNode != 0; pNode = pNode->next )
            {
                if( pNode->m_ClassMap.clsid == clsid) {
                    PIFCMClassFactory pf =  pNode->m_ClassMap.pFactoryCreator(this);
                    res= pf->QueryInterface(iid,pAny);
                    pf->Release();
                    return res;
                }
            }
            return FCM_CLS_NOT_DEFINED;
        }
        FCM::U_Int32 canUnloadNow(void)
        {
            return m_LiveObjectCounter;
        }
        void finalize()
        {
            if(!m_pCalloc)
                return;

            ClassNode* current=m_firstNode;
            ClassNode* prev=0;
            while(current)
            {
                prev=current;
                current= current->next;
                m_pCalloc->Free(prev);
            }

            //Release the ICalloc now. Don't delay till destructor
            m_pCalloc.Reset();
        }

    protected:

        void addClassEntry ( FCMCLSID clsid,    FactorCreatorProc pFactoryCreator,    InterfaceMapGetProc pGetInterfaceTable,    FCM::U_Int32 classVersion)
        {
            if(!m_pCalloc)
                return;

            ClassNode* newnode = (ClassNode*)m_pCalloc->Alloc(sizeof(ClassNode));
            if(!newnode)
                return;
            newnode->next=0;
            newnode->m_ClassMap.clsid = clsid;
            newnode->m_ClassMap.pFactoryCreator = pFactoryCreator;
            newnode->m_ClassMap.pGetInterfaceTable = pGetInterfaceTable;
            newnode->m_ClassMap.classVersion = classVersion;

            if(!m_firstNode)
            {
                m_firstNode=newnode;
            }
            else
            {
                ClassNode* current=m_firstNode;
                ClassNode* prev=0;
                while(current)
                {
                    prev=current;
                    current= current->next;
                }
                prev->next = newnode;                
            }
        }

    };


    class FCMObjectBase
    {
    public:
        FCM::Result InitDone()
        {
            return FCM_SUCCESS;
        }
    };

    
    template<typename IntfImpl>   
    class FCMObject : public IntfImpl,public  IFCMNoAggregationUnknown
    {
    private:
        FCM::U_Int32 mRefCount;
        PIFCMCallback m_pPIFCMCallback;
        PIFCMUnknown  m_pUnknownOuter;
        FCMPluginModule *m_pModule;

    public:            
        FCMObject(FCMPluginModule *pModule)
        {
            mRefCount=0;
            m_pPIFCMCallback=0;
            m_pUnknownOuter=0;
            m_pModule=pModule;
            assert(pModule);
            m_pModule->incrementAliveCount();    
        }
        virtual ~FCMObject()
        {
            m_pModule->decrementAliveCount();
        }

        void FCMInit(PIFCMCallback pIFCMCallback,PIFCMUnknown pUnknownOuter)
        {
            m_pPIFCMCallback=pIFCMCallback;            

            if (!pUnknownOuter) 
            {
                m_pUnknownOuter = reinterpret_cast<IFCMUnknown*>(
                    static_cast<IFCMNoAggregationUnknown*>(this));
            } 
            else 
            {
                m_pUnknownOuter = pUnknownOuter;
            }
        }

        virtual PIFCMCallback GetCallback()
        {
            return m_pPIFCMCallback;
        }
        
        virtual FCMPluginModule * GetPluginModule()
        {
            return m_pModule;
        }
        virtual FCM::U_Int32 InternalAddRef()
        {
            return AddRef();
        }
        virtual FCM::U_Int32 InternalRelease()
        {
            return Release();
        }
        virtual FCM::Result InternalQueryInterface(ConstRefFCMIID pInterfaceID, FCM::PPVoid ppvObj)
        {
            return QueryInterface(pInterfaceID,ppvObj);
        }


        FCM::Result QueryInterface (ConstRefFCMIID pInterfaceID, FCM::PPVoid ppvObj)
        {            
            return m_pUnknownOuter->QueryInterface(pInterfaceID,ppvObj);        
        }

        FCM::U_Int32 AddRef ()
        {
            return m_pUnknownOuter->AddRef();
        }
        FCM::U_Int32 Release ()
        {
            return m_pUnknownOuter->Release();                
        }

        FCM::Result MainQueryInterface (ConstRefFCMIID pInterfaceID, FCM::PPVoid ppvObj)
        {
            FCMInterfaceMap* pIntfMap = IntfImpl::GetInterfaceMap();
            FCM::Result res= FCM_NO_INTERFACE;
            while( pIntfMap && pIntfMap->iid != FCMIID_NULL)
            {
                if(pInterfaceID == pIntfMap->iid && ppvObj )
                {
                    if(pIntfMap->innerObjectFunc == _FCM_SIMPLEMAPENTRY)
                    {
                        if (pInterfaceID == IID_IFCMUnknown) {
                            *ppvObj = static_cast<IFCMNoAggregationUnknown*>(this);
                        }
                        else
                        {
                            *ppvObj = (IFCMUnknown *)((FCM::S_Int64 )this + pIntfMap->offset );
                        }
                        ( reinterpret_cast<IFCMUnknown *>(*ppvObj))->AddRef();
                        res = FCM_SUCCESS;
                        break;
                    }
                    else
                    {
                        res = pIntfMap->innerObjectFunc((FCM::PVoid)this,pInterfaceID,ppvObj,pIntfMap->offset);
                        break;
                    }
                } 

                ++pIntfMap;
            }
            return res;        
        }
        FCM::U_Int32 NoAggregationAddRef ()
        {
            mRefCount++;
            return mRefCount;
        }
        FCM::U_Int32 NoAggregationRelease ()
        {
            assert(mRefCount > 0);
            FCM::U_Int32 refCount = --mRefCount;    
            if(refCount==0)
            {
                delete this;
            }
            return refCount;
        }

    };

    template<class T>
    class IntfImpl_Traits
    {
    public:
        static  FCM::Result CreateInstance(FCMPluginModule *pModule,PIFCMCallback pCallback,PIFCMUnknown pUnkOuter,T*& out)
        { 
            FCM::Result res= FCM_SUCCESS;
            out =  new T(pModule);
            out->FCMInit(pCallback,pUnkOuter);
            res=out->InitDone();
            if(FCM_FAILURE_CODE(res))
            {
                delete out;
                out=0;
                return res;
            }
            return res;
        }    
    };

    
    template<class T>
    class FCMClassFactory:public IFCMClassFactory, public FCMObjectBase
    {
    public:
        FCMClassFactory()
        {

        }
        BEGIN_INTERFACE_MAP(FCMClassFactory,FCM_VERSION)    
            INTERFACE_ENTRY(IFCMClassFactory)    
        END_INTERFACE_MAP

        FCM::Result CreateInstance(PIFCMUnknown pUnkOuter,PIFCMCallback pCallback,ConstRefFCMIID iid, FCM::PPVoid pAny)
        {
            FCM::Result res= FCM_GENERAL_ERROR;
            if (pUnkOuter && (iid != IID_IFCMUnknown))
                return FCM_NO_AGGREGATION;

            FCMObject<T>* pNewObject =0;
            res= IntfImpl_Traits<FCMObject<T> >::CreateInstance(GetPluginModule(),pCallback,pUnkOuter,pNewObject);
            if(FCM_FAILURE_CODE(res))
                return res;

            res=pNewObject->MainQueryInterface(iid,pAny);
            if(FCM_SUCCESS_CODE(res))
            {
                return res;
            }
            else
                delete pNewObject;        
            
            return res;
        }
        static PIFCMClassFactory GetFactory( FCMPluginModule *pModule)
        {

            FCMObject<FCMClassFactory<T> >* pFact =0;
            FCM::Result res = IntfImpl_Traits<FCMObject<FCMClassFactory<T> > >::CreateInstance(pModule,0,0,pFact);
            if(FCM_FAILURE_CODE(res))
                return 0;
            FCM_ADDREF(pFact);
            return pFact;
        }

        virtual FCMPluginModule * GetPluginModule()=0;
    };


    typedef FCM::Result (*PluginBootProc)(PIFCMCallback pCallback);
        
    //Returns a null terminated array of PFCMClassInterfaceInfo; 
    //The Plugin should expose all the pairs to the host
    typedef FCM::Result (*PluginGetClassInfoProc)(PIFCMCalloc pCalloc,PFCMClassInterfaceInfo* ppClassInfo);

    //Returns a IClassFactory interface for a CLSID    
    typedef FCM::Result  (*PluginGetClassObjectProc)(PIFCMUnknown pUnkOuter,
            ConstRefFCMCLSID clsid,ConstRefFCMIID iid, FCM::PPVoid pAny);

    typedef FCM::Result (*PluginRegisterProc)(PIFCMPluginDictionary pPluginDict);

    typedef FCM::U_Int32 (*PluginCanUnloadNowProc)(void);

    typedef FCM::Result (*PluginShutdownProc)();



    typedef struct
    {
            /* Boot function will be called immediately after the DLL/framework load.
            *  Perfrom any global intitialization inside this
            */
            PluginBootProc                m_pBoot; 
            /* GetClassinfo is used to find out the different classes implemented 
            *  by the plugin. Do no perform any app specific logic here
            */
            PluginGetClassInfoProc        m_pGetClassInfo;
    
            /* GetClassObject is used to create the factory for each class implemented 
            *  by the plugin. Do no perform any app specific logic here
            */
            PluginGetClassObjectProc      m_pGetClassObject;

            /* Register function is used publish the services and capabilities of the plugin.
            *  This may not be called for plugin loading
            */
            PluginRegisterProc            m_pRegister;
            /* CanUnloadNow returns the number of live object instances from by this plugin.
            *  Just before the shutdown all the instances should be deleted and zero should be returned
            */
            PluginCanUnloadNowProc        m_pCanUnloadNow;

            /* Shutdown allows the plugin to do proper clean-up before unloading.
            */
            PluginShutdownProc            m_pShutdown;
    }
    PluginProcs;


};

#include "FCMPostConfig.h"

#endif // FCM_PLUGIN_INTERFACE_H_
