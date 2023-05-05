#!/bin/bash

function echo_error
{
    echo -n '[ERROR] : ' >&2
    for var in "$@"
    do
        echo -n "$var" >&2
    done
    echo
    exit 1
}

function echo_warn
{
    echo -n '[WARN] : '
    for var in "$@"
    do
        echo -n "$var"
    done
    echo
}

function help
{
    echo 'Options: [-d <tapname>] [-a <tapIP>] [-n <networkIP>] [-c <serverIP>] [-N] [-h]'
    echo '    -d <tapname>   : specify the name of the tap interface that will be created'
    echo '                     (default = tap50 for server and tap51 for client)'
    echo '    -a <tapIP>     : specify the IP of the tap interface that will be created'
    echo '                     (default = 10.0.8.1/24 for server and 10.0.8.2/24 for client)'
    echo '    -c <serverIP>  : run in client mode and specify the IP of the server to connect to'
    echo '                     (default = 10.0.8.1/24)'
    echo '    -n <networkIP> : specify the IP of the network which will be routed to the tap interface'
    echo '                     (default = 10.0.8.0/24)'
    echo '    -N             : create a new tap interface instead of configuring an existing one, must run as root'
    echo '    -h             : show this list'
}

if [[ $EUID -ne 0 ]]; then
    echo_error 'Script mush run as root'
fi

tap_name=0
tap_addr=0
client_mode=0
new_interface=0
srv_addr=0
network_ip=0

while getopts d:a:n:c:Nh flag
do
    case '${flag}' in
        d)
        tap_name=${OPTARG}
        ;;

        a)
        tap_addr=${OPTARG}
        ;;

        n)
        network_ip=${OPTARG}
        ;;

        c)
        client_mode=1
        srv_addr=${OPTARG}
        ;;

        N)
        new_interface=1
        ;;

        h)
        help
        exit 0
        ;;
    esac
done

if [[ $client_mode == 0 ]]; then
    echo 'Running in server mode...'

    port_forward=$(sysctl net.ipv4.ip_forward)

    if [[ $port_forward == 0 ]]; then
        echo_warn 'IP forwarding is disabled, enabling IP forwarding'
        sysctl net.ipv4.ip_forward=1 || echo_error 'Failed to enable IP forwarding'
    fi

    # echo 'Setting up iptables '
    # iptables -t nat -A POSTROUTING -j MASQUERADE || echo_error 'Failed to setup iptables'

    if [[ $tap_name == 0 ]]; then
        tap_name='tap50'
        echo_warn 'No tap interface name provided, using default ' $tap_name
    fi

    if [[ $tap_addr == 0 ]]; then
        tap_addr='10.0.8.1/24'
        echo_warn 'No tap address provided, using default ' $tap_addr
    fi
else
    echo 'Running in client mode...'

    if [[ $tap_name == 0 ]]; then
        tap_name='tap51'
        echo_warn 'No tap interface name provided, using default ' $tap_name
    fi

    if [[ $tap_addr == 0 ]]; then
        tap_addr='10.0.8.2/24'
        echo_warn 'No tap address provided, using default ' $tap_addr
    fi

    if [[ $server_addr == 0 ]]; then
        server_addr='10.0.8.1/24'
        echo_warn 'No server address provided, using default ' $server_addr
    fi
fi

if [[ $new_interface == 1 ]]; then
    ip tuntap add $tap_name mode tap || echo_error 'Failed to create tap interface, perhaps it already exists?'
    ip addr add $tap_addr dev $tap_name || echo_error 'Failed to add address to tap interface, interface may already have this address configured'
    ip link set $tap_name up || echo_error 'Failed to enable tap interface'

    echo_warn 'Created new interface ' $tap_name ' on address ' $tap_addr ' and enabled it'
fi

if [[ $network_ip == 0 ]]; then
    network_ip='10.0.8.0/24'
    echo_warn 'No network IP provided, using default ' $network_ip
fi

route add -net $network_ip $tap_name || echo_warn 'Failed to route trafic from ' $network_ip ' to ' $tap_name ', routing rule may aready exist'