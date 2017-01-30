/*
* This file was generated by the CommonAPI Generators.
* Used org.genivi.commonapi.dbus 3.1.5.v201601121430.
* Used org.franca.core 0.9.1.201412191134.
*
* This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
* If a copy of the MPL was not distributed with this file, You can obtain one at
* http://mozilla.org/MPL/2.0/.
*/
#include <v1/org/genivi/SotaClient.hpp>
#include <v1/org/genivi/SotaClientDBusStubAdapter.hpp>

namespace v1 {
namespace org {
namespace genivi {

std::shared_ptr<CommonAPI::DBus::DBusStubAdapter> createSotaClientDBusStubAdapter(
                   const CommonAPI::DBus::DBusAddress &_address,
                   const std::shared_ptr<CommonAPI::DBus::DBusProxyConnection> &_connection,
                   const std::shared_ptr<CommonAPI::StubBase> &_stub) {
    return std::make_shared<SotaClientDBusStubAdapter>(_address, _connection, _stub);
}

INITIALIZER(registerSotaClientDBusStubAdapter) {
    CommonAPI::DBus::Factory::get()->registerStubAdapterCreateMethod(
    	SotaClient::getInterface(), &createSotaClientDBusStubAdapter);
}

SotaClientDBusStubAdapterInternal::~SotaClientDBusStubAdapterInternal() {
    deactivateManagedInstances();
    SotaClientDBusStubAdapterHelper::deinit();
}

void SotaClientDBusStubAdapterInternal::deactivateManagedInstances() {

}

const char* SotaClientDBusStubAdapterInternal::getMethodsDBusIntrospectionXmlData() const {
    static const std::string introspectionData =
        "<method name=\"getInterfaceVersion\">\n"
            "<arg name=\"value\" type=\"uu\" direction=\"out\" />"
        "</method>\n"
        "<signal name=\"UpdateAvailable\">\n"
            "<arg name=\"updateAvailable\" type=\"(sssbts)\" />\n"
        "</signal>\n"
        "<signal name=\"DownloadComplete\">\n"
            "<arg name=\"downloadComplete\" type=\"(sss)\" />\n"
        "</signal>\n"
        "<signal name=\"InstalledSoftwareNeeded\">\n"
        "</signal>\n"
        /**
         * description: Sent by SC to start the download of an update previously announced
        	as
         *   available through an update_available() call  made from SC to
        	SWLM.
         */
        "<method name=\"initiateDownload\">\n"
            "<arg name=\"_updateId\" type=\"s\" direction=\"in\" />\n"
        "</method>\n"
        /**
         * description: Abort a download previously initiated with initiate_download().
        	Invoked by
         *   SWLM in response to an error or an explicit
        	request sent by HMI to SWLM in
         *   response to a user abort.
         */
        "<method name=\"abortDownload\">\n"
            "<arg name=\"_updateId\" type=\"s\" direction=\"in\" />\n"
        "</method>\n"
        /**
         * description: Receive an update report from SWLM with the processing result of all
        	bundled
         *   operations.
        	An update report message can either be sent in response
        	to an
         *   downloadComplete() message transmitted from SC to SWLM,
        	or be sent
         *   unsolicited by SWLM to SC
         */
        "<method name=\"updateReport\">\n"
            "<arg name=\"_updateId\" type=\"s\" direction=\"in\" />\n"
            "<arg name=\"_operationsResults\" type=\"a(sis)\" direction=\"in\" />\n"
        "</method>\n"

    ;
    return introspectionData.c_str();
}

CommonAPI::DBus::DBusGetAttributeStubDispatcher<
        ::v1::org::genivi::SotaClientStub,
        CommonAPI::Version
        > SotaClientDBusStubAdapterInternal::getSotaClientInterfaceVersionStubDispatcher(&SotaClientStub::getInterfaceVersion, "uu");




/**
 * description: Sent by SC to start the download of an update previously announced
	as
 *   available through an update_available() call  made from SC to
	SWLM.
 */
CommonAPI::DBus::DBusMethodWithReplyStubDispatcher<
    ::v1::org::genivi::SotaClientStub,
    std::tuple<std::string>,
    std::tuple<>,
    std::tuple<CommonAPI::DBus::StringDeployment>,
    std::tuple<>
    
    > SotaClientDBusStubAdapterInternal::initiateDownloadStubDispatcher(
    &SotaClientStub::initiateDownload, "",
					std::make_tuple(static_cast<CommonAPI::DBus::StringDeployment*>(nullptr)),
                	std::make_tuple());

/**
 * description: Abort a download previously initiated with initiate_download().
	Invoked by
 *   SWLM in response to an error or an explicit
	request sent by HMI to SWLM in
 *   response to a user abort.
 */
CommonAPI::DBus::DBusMethodWithReplyStubDispatcher<
    ::v1::org::genivi::SotaClientStub,
    std::tuple<std::string>,
    std::tuple<>,
    std::tuple<CommonAPI::DBus::StringDeployment>,
    std::tuple<>
    
    > SotaClientDBusStubAdapterInternal::abortDownloadStubDispatcher(
    &SotaClientStub::abortDownload, "",
					std::make_tuple(static_cast<CommonAPI::DBus::StringDeployment*>(nullptr)),
                	std::make_tuple());

/**
 * description: Receive an update report from SWLM with the processing result of all
	bundled
 *   operations.
	An update report message can either be sent in response
	to an
 *   downloadComplete() message transmitted from SC to SWLM,
	or be sent
 *   unsolicited by SWLM to SC
 */
CommonAPI::DBus::DBusMethodWithReplyStubDispatcher<
    ::v1::org::genivi::SotaClientStub,
    std::tuple<std::string, std::vector<SotaClient::OperationResult>>,
    std::tuple<>,
    std::tuple<CommonAPI::DBus::StringDeployment, CommonAPI::DBus::ArrayDeployment<SotaClient_::OperationResultDeployment_t>>,
    std::tuple<>
    
    > SotaClientDBusStubAdapterInternal::updateReportStubDispatcher(
    &SotaClientStub::updateReport, "",
					std::make_tuple(static_cast<CommonAPI::DBus::StringDeployment*>(nullptr), static_cast<CommonAPI::DBus::ArrayDeployment<SotaClient_::OperationResultDeployment_t>*>(nullptr)),
                	std::make_tuple());



void SotaClientDBusStubAdapterInternal::fireUpdateAvailableEvent(const ::v1::org::genivi::SotaClient::UpdateAvailable& updateAvailable) {
    CommonAPI::DBus::DBusStubSignalHelper<CommonAPI::DBus::DBusSerializableArguments<
    CommonAPI::Deployable<::v1::org::genivi::SotaClient::UpdateAvailable, SotaClient_::UpdateAvailableDeployment_t>
    >>::sendSignal(
            *this,
            "UpdateAvailable",
            "(sssbts)",
    CommonAPI::Deployable<::v1::org::genivi::SotaClient::UpdateAvailable, SotaClient_::UpdateAvailableDeployment_t>(updateAvailable, static_cast<SotaClient_::UpdateAvailableDeployment_t*>(nullptr))
    );
}
void SotaClientDBusStubAdapterInternal::fireDownloadCompleteEvent(const ::v1::org::genivi::SotaClient::DownloadComplete& downloadComplete) {
    CommonAPI::DBus::DBusStubSignalHelper<CommonAPI::DBus::DBusSerializableArguments<
    CommonAPI::Deployable<::v1::org::genivi::SotaClient::DownloadComplete, SotaClient_::DownloadCompleteDeployment_t>
    >>::sendSignal(
            *this,
            "DownloadComplete",
            "(sss)",
    CommonAPI::Deployable<::v1::org::genivi::SotaClient::DownloadComplete, SotaClient_::DownloadCompleteDeployment_t>(downloadComplete, static_cast<SotaClient_::DownloadCompleteDeployment_t*>(nullptr))
    );
}
void SotaClientDBusStubAdapterInternal::fireInstalledSoftwareNeededEvent() {
    CommonAPI::DBus::DBusStubSignalHelper<CommonAPI::DBus::DBusSerializableArguments<
    >>::sendSignal(
            *this,
            "InstalledSoftwareNeeded",
            ""
    );
}


const SotaClientDBusStubAdapterHelper::StubDispatcherTable& SotaClientDBusStubAdapterInternal::getStubDispatcherTable() {
    return stubDispatcherTable_;
}

const CommonAPI::DBus::StubAttributeTable& SotaClientDBusStubAdapterInternal::getStubAttributeTable() {
    return stubAttributeTable_;
}

SotaClientDBusStubAdapterInternal::SotaClientDBusStubAdapterInternal(
        const CommonAPI::DBus::DBusAddress &_address,
        const std::shared_ptr<CommonAPI::DBus::DBusProxyConnection> &_connection,
        const std::shared_ptr<CommonAPI::StubBase> &_stub)
	: CommonAPI::DBus::DBusStubAdapter(_address, _connection,false),
      SotaClientDBusStubAdapterHelper(_address, _connection, std::dynamic_pointer_cast<SotaClientStub>(_stub), false),
      stubDispatcherTable_({
            /**
             * description: Sent by SC to start the download of an update previously announced
            	as
             *   available through an update_available() call  made from SC to
            	SWLM.
             */
            { { "initiateDownload", "s" }, &org::genivi::SotaClientDBusStubAdapterInternal::initiateDownloadStubDispatcher },
            /**
             * description: Abort a download previously initiated with initiate_download().
            	Invoked by
             *   SWLM in response to an error or an explicit
            	request sent by HMI to SWLM in
             *   response to a user abort.
             */
            { { "abortDownload", "s" }, &org::genivi::SotaClientDBusStubAdapterInternal::abortDownloadStubDispatcher },
            /**
             * description: Receive an update report from SWLM with the processing result of all
            	bundled
             *   operations.
            	An update report message can either be sent in response
            	to an
             *   downloadComplete() message transmitted from SC to SWLM,
            	or be sent
             *   unsolicited by SWLM to SC
             */
            { { "updateReport", "sa(sis)" }, &org::genivi::SotaClientDBusStubAdapterInternal::updateReportStubDispatcher }
            }),
        stubAttributeTable_() {

    stubDispatcherTable_.insert({ { "getInterfaceVersion", "" }, &org::genivi::SotaClientDBusStubAdapterInternal::getSotaClientInterfaceVersionStubDispatcher });
}

bool SotaClientDBusStubAdapterInternal::hasFreedesktopProperties() {
    return false;
}

} // namespace genivi
} // namespace org
} // namespace v1