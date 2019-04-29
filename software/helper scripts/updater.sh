#!/bin/bash

#--------------------
# YIO updater script
#--------------------
fi

if [ -z $1 ]; then
    echo "This script requires the URL to download (probably not a good idea to run this script yourself)."
    exit 1
fi


#--------------------
# note: Since OSX 10.11, Apple reinforces System Integrity Protection and locks down:
# - /System
# - /sbin
# - /usr (with the exception of /usr/local subdirectory)
#
# Since the developers of the yio-remote use macs, i adopted this script to work with the /usr/local/yio-remote target path.
# That is also why we require root access
#--------------------


#--------------------
# TODO: Rework based on path?
#--------------------
fbv -d 1 /usr/local/yio-remote/update.png


#--------------------
# 1. Create temp location
#    - Download file and pipe to tar to unzip to temp location
#--------------------
tmpdir="/usr/local/yio-$(date +"%Y%m%d%H%M%S")"
mkdir -p "${tmpdir}"


#--------------------
# 2. Download the URL that is given to the bash script and extract the zip file
#--------------------
curl -SL $1 | tar -xf - -C "${tmpdir}"


#--------------------
# 3. Give executable permissions to all .sh files
#--------------------
find "${tmpdir}" -type f -name "*.sh" -exec chmod 775 {} +
chmod +x ${tmpdir}/remote


#--------------------
# 4. Remove previous backups (should not be needed, unless this scripts fails somewhere, therefore not yet implemented)
#--------------------
rm -rf "/usr/local/yio-remote-backup"


#--------------------
# 5. Make a backup of the /usr/bin/remote folder (if not already exist, maybe add timestamp to folder)
#--------------------
mv "/usr/local/yio-remote" "/usr/local/yio-remote-backup"


#--------------------
# 6. Kill python and remote by pid
#    - todo/improvement: Only kill based on PiD to prevent killing other apps?
#--------------------
killall -9 python
killall -9 remote
sleep 2


#--------------------
# 8. Rename the update folder to /usr/local/yio-remote
#--------------------
mv "${tmpdir}" "/usr/local/yio-remote"


#--------------------
# 9. Launch the remote app with the launcher bash script
#--------------------
/usr/local/yio-remote/app-launch.sh &
