#pragma once
#include "Zhen/BasePage.h"
#include "Zhen/StrandCommandScheduler.h"
#include "Zhen/StringContent.h"

#include <Bt/BluetoothAddress.h>

class OptionContent;
class PageDeviceList : public BasePage
{

public:

    PageDeviceList();

    ~PageDeviceList();

    std::string GetPrintableString()const override;

    bool OnEvent( std::shared_ptr<Event> a_event ) override;

    void StartSearchDevice( bool a_start = true );

    void GenerateOobData();

// The functions to handle the input event from the user
private:

    void HanldePairedDeviceChanged();

    bool HanldePairWithRemoteDevice( std::string const& a_input );

    bool HanldeDeletePairedDevice( std::string const& a_input );

    // The functions to handle the input event from the user
private:

    void HanldeNewDeviceFound();

    void HandleIncomingSspRequest
        (
        BluetoothAddress    a_address,
        std::string         a_deviceName,
        uint32_t            a_passkey
        );

    void HandleIncomingPincodeRequest
        (
        BluetoothAddress    a_address,
        std::string         a_deviceName,
        bool                a_limit
        );

    void HandlePairingStateChanged();

    void HandleSearchStateChanged();

    void UpdateStatusContent();

    std::shared_ptr<OptionContent> m_content;
    std::shared_ptr<OptionContent> m_searchedDeviceContent;
    std::shared_ptr<OptionContent> m_PairedDeviceContent;
    std::shared_ptr<StringContent> m_StatusContent;
    StrandCommandScheduler m_commandScheduler;

    std::shared_ptr<BasePage> m_confirmPage;
    boost_ns::signals2::connection m_deviceFoundSignalConnection;
    boost_ns::signals2::connection m_sspPairingRequestConnection;
    boost_ns::signals2::connection m_pincodeRequestConnection;
    boost_ns::signals2::connection m_pairedDeviceChangedConnection;
    boost_ns::signals2::connection m_pairingStateChangedConnection;
    boost_ns::signals2::connection m_searchStateChangedConnection;
};

