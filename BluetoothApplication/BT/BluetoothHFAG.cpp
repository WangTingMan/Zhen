#include "BluetoothHFAG.h"
#include "Adaptor.h"

#include <BT/StackLayer/BluetoothHFAGInterface.h>

#define HANDS_FREE_SERVICE_MODULE "HFP_HF_AG_ROLE"

namespace Bluetooth {

    BluetoothHFAG::BluetoothHFAG()
    {
        m_status = ConnectionStatus::ServiceDisconnected;
        SetModuleName( HANDS_FREE_SERVICE_MODULE );

        CallSession call_session;
        call_session.dir = CallDirection::CallIncoming;
        call_session.index = 1;
        call_session.mpty = CallType::SINGLE;
        call_session.number = "10086";
        call_session.state = CallState::INCOMING;

        //m_callSessions.push_back(call_session);
    }

    std::shared_ptr<BluetoothHFAG> BluetoothHFAG::GetMoudle()
    {
        auto hf = Adaptor::GetInstance().FindModule( HANDS_FREE_SERVICE_MODULE );
        std::shared_ptr<BluetoothHFAG> hfModule = std::dynamic_pointer_cast<BluetoothHFAG>( hf );
        return hfModule;
    }

    int BluetoothHFAG::Init()
    {
        BluetoothHFAGInterface::GetInterface().Init();
        return 0;
    }

    bool BluetoothHFAG::Connect
        (
        BluetoothAddress a_address
        )
    {
        return BluetoothHFAGInterface::GetInterface().Connect( a_address );
    }

    bool BluetoothHFAG::Disconnect
        (
        BluetoothAddress a_address
        )
    {
        return BluetoothHFAGInterface::GetInterface().disconnect( a_address );
    }

    std::vector<BluetoothAddress> BluetoothHFAG::GetConnectedDevices()
    {
        std::vector<BluetoothAddress> devices;
        return devices;
    }

    ConnectionStatus BluetoothHFAG::GetConnectionState( BluetoothAddress a_address )
    {
        return m_status;
    }

    ConnectionStatus BluetoothHFAG::GetConnectionState()const
    {
        return m_status;
    }

    bool BluetoothHFAG::DialNewNumber( std::string a_number )
    {
        return BluetoothHFAGInterface::GetInterface().DialNewNumber( a_number );
    }

    void BluetoothHFAG::HandleCindCallback( BluetoothAddress a_address )
    {
        return BluetoothHFAGInterface::GetInterface().ResponseCind( a_address, true, 0, 0, CallState::IDLE, 5, true, 0 );
    }

    void BluetoothHFAG::HandleCopsCallback( BluetoothAddress a_address )
    {
        return BluetoothHFAGInterface::GetInterface().ResponseCops( a_address, "Taiwan network");
    }

    void BluetoothHFAG::HandleCnumCallback( BluetoothAddress a_address )
    {
        return BluetoothHFAGInterface::GetInterface().ResponseCnum( a_address, "10086" );
    }

    void BluetoothHFAG::HandleClccCallback( BluetoothAddress a_address )
    {
        for (auto& ele : m_callSessions)
        {
            BluetoothHFAGInterface::GetInterface().ResponseClcc(a_address, ele.index, ele.dir,
                ele.state, CallMode::Voice, ele.mpty,
                ele.number, CallAddrType::UNKNOWN);
        }
        BluetoothHFAGInterface::GetInterface().AtResponse(true, 0, a_address);
    }

    void BluetoothHFAG::HandleDialCallback( BluetoothAddress a_address, std::string a_phone_number )
    {
        return BluetoothHFAGInterface::GetInterface().AtResponse( true, 5, a_address );
    }

    void BluetoothHFAG::HandleConnectionStatusChangedFromStack
        (
        BluetoothAddress a_address,
        ConnectionStatus a_status
        )
    {
        m_address = a_address;
        m_status = a_status;
        if (a_status == ConnectionStatus::ServiceConnected)
        {
            PhoneStateChange();
        }
    }

    void BluetoothHFAG::PhoneStateChange()
    {
        int num_active = 0;
        int num_held = 0;
        CallState call_setup_state = CallState::IDLE;
        std::string number;
        CallAddrType type = CallAddrType::UNKNOWN;
        std::string name = "Mobilde";
        for (auto& ele : m_callSessions)
        {
            if (ele.state == CallState::ACTIVE)
            {
                num_active++;
            }

            if (ele.state == CallState::HELD)
            {
                num_held++;
            }
            call_setup_state = ele.state;
            number = ele.number;
        }

        BluetoothHFAGInterface::GetInterface().PhoneStateChange(m_address, num_active, num_held,
            call_setup_state, number, type, name);
    }

}

