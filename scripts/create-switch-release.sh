#!/usr/bin/env bash

if [[ $# -eq 0 ]]; then
	echo "something is wrong.."
fi

sudo apt-get update
sudo apt-get -y install zip

version_number=`echo $1 | grep -o '[0-9]\+.[0-9]\+.[0-9]\+'`

mkdir -p switch/godot config/nx-hbmenu/fileassoc
cp godot-forwarder.nro switch/godot/godot-forwarder.nro
cp switch_release_debug.nro switch/godot/godot-${version_number}.nro
cp platform/switch/godot.cfg config/nx-hbmenu/fileassoc/godot.cfg
zip -r godot-switch.zip switch config
