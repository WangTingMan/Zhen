#pragma once

enum class bluetooth_interface_type
{
    unknown,
    base_type,
    a2dp_source,
    a2dp_sink,
    avrcp_target,
    gatt_client,
    gatt_server,
    hfp_client,
    hfp_ag,
    hid_host,
    pan,
    rfcomm,
    adv_scanning,
    l2cap_coc
};

class bluetooth_interface
{

public:

    bluetooth_interface
        (
        bluetooth_interface_type a_interface_type = bluetooth_interface_type::unknown
        )
        : m_interface_type( a_interface_type )
    {

    }

    virtual ~bluetooth_interface()
    {

    }

    virtual void Init() = 0;

    bluetooth_interface_type get_interface_type()const
    {
        return m_interface_type;
    }

protected:

    void set_type( bluetooth_interface_type a_interface_type )
    {
        m_interface_type = a_interface_type;
    }

private:

    bluetooth_interface_type m_interface_type = bluetooth_interface_type::unknown;
};
