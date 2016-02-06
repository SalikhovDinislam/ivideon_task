#!/bin/bash

CLIENT_ID=$$

MAIN_CHANNEL="led_server"
INPUT_CHANNEL="to_led_${CLIENT_ID}"
OUTPUT_CHANNEL="from_led_${CLIENT_ID}"

if [ ! -p "${MAIN_CHANNEL}" ]; then
    echo "Server is not running"
    exit 1
fi

start_client() 
{
    mkfifo "${INPUT_CHANNEL}"
    mkfifo "${OUTPUT_CHANNEL}"

    echo "${CLIENT_ID}" > "${MAIN_CHANNEL}"
    cat "${OUTPUT_CHANNEL}"&
    CAT_PID=$!
}

stop_client()
{
    kill $CAT_PID
    wait

    echo "stop" > "${INPUT_CHANNEL}"

    rm "${INPUT_CHANNEL}"
    rm "${OUTPUT_CHANNEL}"
}

show_commands_list()
{
    echo "The following commands are supported:"
    echo "  Turn on/off a LED"
    echo "      set-led-state {on,off}"
    echo "  Get LED's state"
    echo "      get-led-state"
    echo "  Change LED's color"
    echo "      set-led-color {red,green,blue}"
    echo "  Get LED's color"
    echo "      get-led-color"
    echo "  Change LED's twinkling rate"
    echo "      set-led-rate {0..5}"
    echo "  Get LED's twinkling rate"
    echo "      get-led-rate"
    echo "  Stop the client"
    echo "      stop"
}

get_commands()
{
    echo "Print your commands ('?' for help):"
    # TODO: add autocompletion
    while read LINE; do
        if [ "$LINE" = "stop" ]; then
            break;
        fi
        if [ "$LINE" = "?" ]; then
            show_commands_list
        else
            echo "$LINE" > "${INPUT_CHANNEL}"
        fi
    done
}

start_client
get_commands
stop_client
