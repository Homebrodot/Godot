version_number=$(python -c "import version; print(f'{version.major}.{version.minor}.{version.patch}')")
template_version=$(python -c "import version; print(f'{version.major}.{version.minor}.{version.patch}-{version.status}')")

godot_dir=`pwd`

tmpdir=$(mktemp -d)
pushd $tmpdir

# repack Windows editor to include build_romfs and nxlink
mkdir godot-switch-v${version_number}-windows
pushd godot-switch-v${version_number}-windows
cp ${godot_dir}/windows-editor/godot.windows.opt.tools.64.exe .
wget https://9net.org/tools/nxlink.exe
wget https://9net.org/tools/build_romfs.exe
popd
zip -r godot-v${version_number}-windows-switch-support.zip godot-switch-v${version_number}-windows

# package macOS editor (giant todo)
#mac_tmpdir=`ssh 172.31.1.123 mktemp -d`
#scp macos-editor.zip 172.31.1.123:${mac_tmpdir}/macos-editor.zip
#scp -r ${godot_dir}/misc/dist/osx_tools.app 172.31.1.123:${mac_tmpdir}/'Godot for Switch.app'
#scp ${godot_dir}/platform/switch/Info.plist 172.31.1.123:${mac_tmpdir}/'Godot for Switch.app/Info.plist'
#
##ssh 172.31.1.123 bash << EOF
#	pushd ${mac_tmpdir}
#	unzip macos-editor.zip
#	strip godot.osx.opt.tools.64
#
#	mkdir -p "Godot for Switch".app/Contents/MacOS
#	cp godot.osx.opt.tools.64 "Godot for Switch".app/Contents/MacOS/Godot
#	chmod +x "Godot for Switch".app/Contents/MacOS/Godot

#	/usr/local/bin/create-dmg godot-v${version_number}-mac-switch-support.dmg "Godot for Switch".app
#	popd
#EOF
#scp 172.31.1.123:${mac_tmpdir}/godot-v${version_number}-mac-switch-support.dmg ./
#ssh 172.31.1.123 rm -r ${mac_tmpdir}

# repack Linux editor, also include build_romfs and nxlink
mkdir godot-switch-v${version_number}-linux
pushd godot-switch-v${version_number}-linux
cp -r ${godot_dir}/linux-editor-mono/* .
chmod +x godot.x11.opt.tools.64
wget https://9net.org/tools/nxlink-linux -O nxlink
wget https://9net.org/tools/build_romfs-linux -O build_romfs
chmod +x nxlink build_romfs
popd
tar cz godot-switch-v${version_number}-linux > godot-v${version_number}-linux-switch-support.tar.gz

# repack Switch template

mkdir templates
cp ${godot_dir}/switch-template/switch_release.nro templates/switch_release.nro
cp ${godot_dir}/platform/switch/romfs/applet_splash.rgba.gz templates/switch_applet_splash.rgba.gz
echo $template_version > templates/version.txt
zip -r godot_template-v${version_number}-switch_only.tpz templates

mkdir -p switch/godot config/nx-hbmenu/fileassoc
# todo: always get latest
wget https://github.com/Stary2001/godot-forwarder/releases/download/v0.2/godot-forwarder.nro -O switch/godot/godot-forwarder.nro
cp ${godot_dir}/switch-template/switch_release.nro switch/godot/godot-${version_number}.nro
cp ${godot_dir}/platform/switch/godot.cfg config/nx-hbmenu/fileassoc/godot.cfg
zip -r godot-v${version_number}-switch.zip switch config

mv godot.switch.opt.64 "godot-switch-v${version_number}-DEBUG_ONLY.elf"

# clean up
rm -r godot-switch-v${version_number}-windows godot-switch-v${version_number}-linux
rm -r switch templates config
popd

mkdir artifacts
cp $tmpdir/godot-v${version_number}-windows-switch-support.zip $tmpdir/godot-v${version_number}-linux-switch-support.tar.gz $tmpdir/godot_template-v${version_number}-switch_only.tpz $tmpdir/godot-v${version_number}-switch.zip artifacts/
