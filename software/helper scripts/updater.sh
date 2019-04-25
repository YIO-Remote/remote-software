#!/bin/bash
fbv -d 1 /update.png
killall -9 python
killall -9 remote
sleep 2
cp /usr/bin/remote /usr/bin/remote-backup
rm -rf /usr/bin/remote
sleep 2
curl -L $1 > /usr/bin/remote
sleep 2
chmod +x /usr/bin/remote
/usr/bin/app-launch.sh &