#!/bin/bash

#--------------------
# YIO updater script
#--------------------
if [ -z $1 ]; then
    echo "This script requires the URL to download (probably not a good idea to run this script yourself)."
    exit 1
fi


#--------------------
# TODO: Rework based on path?
#--------------------
fbv -d 1 /usr/bin/yio-remote/images/update.png


#--------------------
# 1. Create temp location
#    - Download file and pipe to tar to unzip to temp location
#--------------------
tmpdir="/usr/bin/yio-$(date +"%Y%m%d%H%M%S")"
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
rm -rf "/usr/bin/yio-remote-backup"


#--------------------
# 5. Make a backup of the /usr/bin/remote folder (if not already exist, maybe add timestamp to folder)
#--------------------
mv "/usr/bin/yio-remote" "/usr/bin/yio-remote-backup"


#--------------------
# 6. Kill python and remote by pid
#    - todo/improvement: Only kill based on PiD to prevent killing other apps?
#--------------------
killall -9 remote
sleep 2


#--------------------
# 8. Rename the update folder to /usr/bin/yio-remote
#--------------------
mv "${tmpdir}" "/usr/bin/yio-remote"


#--------------------
# 9. Launch the remote app with the launcher bash script
#--------------------
/usr/bin/yio-remote/app-launch.sh &
