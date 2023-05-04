#!/bin/bash

function echo_error
{
  echo -n '[ERROR] : ' >&2
  echo $1 >&2
  exit 1
}

# if [[ $EUID -ne 0 ]]; then
#     echo_error 'This script must be run as root'
# fi

server_mode=0
client_mode=0

while getopts s:c:t: flag
do
    case '${flag}' in
        s)
        server_mode=1
        tap_addr=${OPTARG}
        ;;

        c)
        client_mode=1
        tap_addr=${OPTARG}
        ;;

        t)
        tap_name=${OPTARG}
        ;;
    esac
done

if [ $server_mode == 1 && $client_mode == 1 ]; then
    'Cannont use both server and client arguments' >&2
fi

if [ $server_mode == 1 ]; then
    echo 'Running in server mode...'

    port_forward=$(sysctl net.ipv4.ip_forward)

    if [ $port_forward == 0 ]; then
        echo 'IP forwarding is disabled, enabling IP forwarding'
        sysctl net.ipv4.ip_forward=1 || echo_error 'Failed to enable IP forwarding'
    fi

    # echo 'Setting up iptables '
    # iptables -t nat -A POSTROUTING -j MASQUERADE || echo_error 'Failed to setup iptables'

    if [ ! -n '$tap_addr' ]; then
        $tap_addr='10.0.0.1/24'
        echo 'No tap address provided, using default 10.0.0.1/24'
    fi

    if [ -n '$tap_name' ]; then
        $tap_name='tap55'
        echo 'No tap interface name provided, using default tap55'
    fi

    ip tuntap add $tap_name mode tap || echo_error 'Failed to create tap interface'
fi

# if [ -n '$server_addr' ]; then
#     if [ -n '$client_addr' ]; then
#         echo_error 'Cannont use both server and client arguments'
#     fi

#     echo 'Running in server mode...'
    
#     echo 'Turning on port forwarding'
#     sysctl net.ipv4.ip_forward=1
# else
#     if [ -n '$client_addr' ]; then
#         echo $client_addr
#     fi
# fi