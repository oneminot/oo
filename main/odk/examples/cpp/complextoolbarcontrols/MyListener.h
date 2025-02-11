/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/

#ifndef _MyListener_HXX
#define _MyListener_HXX

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/task/XJob.hpp>
#include <com/sun/star/document/XEventListener.hpp>
#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase2.hxx>

#define MYLISTENER_IMPLEMENTATIONNAME  "vnd.My.impl.NewDocListener"
#define MYLISTENER_SERVICENAME         "vnd.My.NewDocListener"

namespace css = ::com::sun::star;

/*---------------------------------------------------
 * Registriert sich in der Office Konfiguration als Job.
 * Dieser wird dann f�r alle neu ge�ffneten Dokumente automatisch
 * gerufen. Man bekommt eine Reference auf das ge�ffnete Dokument
 * �berreicht und kann dann pr�fen, ob es ein unterst�tztes Format
 * hat. (Wir interessieren uns ja schlie�lich nur f�r Writer/Calc Dokumente.)
 *
 * @see CalcListener
 * @see WriterListener
 */
class MyListener : public cppu::WeakImplHelper2< css::task::XJob         ,
                                                 css::lang::XServiceInfo >
{
    private:
        css::uno::Reference< css::lang::XMultiServiceFactory > m_xSMGR;

    public:
                 MyListener(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR);
        virtual ~MyListener();

	// XJob
	virtual css::uno::Any SAL_CALL execute(const css::uno::Sequence< css::beans::NamedValue >& lArguments)
		throw (css::lang::IllegalArgumentException,
			   css::uno::Exception,
			   css::uno::RuntimeException);

	// XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName()
        throw (css::uno::RuntimeException);

    virtual sal_Bool SAL_CALL supportsService(const ::rtl::OUString& sServiceName)
        throw (css::uno::RuntimeException);

    virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames()
        throw (css::uno::RuntimeException);

    public:

    static css::uno::Reference< css::uno::XInterface > st_createInstance(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR);
};

class CalcListener : public cppu::WeakImplHelper1< css::document::XEventListener >
{
    private:
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > mxMSF;

    public:
        CalcListener(const css::uno::Reference< css::lang::XMultiServiceFactory > &rxMSF)
            : mxMSF( rxMSF )
        {}

        virtual ~CalcListener()
        {}

    // document.XEventListener
    virtual void SAL_CALL notifyEvent(const css::document::EventObject& aEvent)
        throw (css::uno::RuntimeException);
	virtual void SAL_CALL disposing(const css::lang::EventObject& aEvent)
        throw (css::uno::RuntimeException);
};

class WriterListener : public cppu::WeakImplHelper1< css::document::XEventListener >
{
    private:
        css::uno::Reference< css::lang::XMultiServiceFactory > mxMSF;

    public:
        WriterListener(const css::uno::Reference< css::lang::XMultiServiceFactory >& rxMSF);

        virtual ~WriterListener()
        {}

        // document.XEventListener
    virtual void SAL_CALL notifyEvent(const css::document::EventObject& aEvent)
        throw (css::uno::RuntimeException);
	virtual void SAL_CALL disposing(const css::lang::EventObject& aEvent)
        throw (css::uno::RuntimeException);
};

#endif // _MyListener_HXX
