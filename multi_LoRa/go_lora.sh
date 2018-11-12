#! /bin/sh

if [ $# = 6 ]
then

# test if directory gpio12 exists, if not create it
[ -d /sys/class/gpio/gpio12 ] || echo 12 > /sys/class/gpio/export

# test if file gpio12/edge exists and is writable,
# if yes: continue with commands within { }
# if not: exit
[ -w /sys/class/gpio/gpio12/edge ] && {
echo rising > /sys/class/gpio/gpio12/edge
}

echo $0
./set_rtc $1 $2 $3 $4 $5 $6

./RXcontinuous_LoRa_rpi_one_channel_logfile &


else 
echo "usage: $0 YY MM DD hh mm ss"
fi
