#!/bin/bash

GAME_NAME=$1
INSTALL_DIR=$2
DATA=$3
ICON=$4
SHORTCUT="$HOME/Desktop/$GAME_NAME.desktop"
PROFILE="$HOME/.local/share/konsole/$GAME_NAME.profile"
EXE="${INSTALL_DIR}data/bin/$GAME_NAME"
ICON_PATH="${INSTALL_DIR}data/icon/"

mkdir -p $INSTALL_DIR $ICON_PATH &&
    cp -r $DATA $INSTALL_DIR &&
    cp $ICON $ICON_PATH
chmod +x $EXE
cat >$PROFILE <<EOL
[General]
Name=$GAME_NAME
Parent=FALLBACK/
ShowTerminalSizeHint=true
TerminalCenter=true
TerminalColumns=34
TerminalRows=27

EOL
cat >${INSTALL_DIR}$GAME_NAME.desktop <<EOL
[Desktop Entry]
Name=$GAME_NAME
Exec=konsole --profile $GAME_NAME -e $EXE
Icon=${ICON_PATH}$ICON
Type=Application
Path=$INSTALL_DIR
EOL

cat >${INSTALL_DIR}uninstall.desktop <<EOL
[Desktop Entry]
Name=uninstall
Exec=rm -rf $SHORTCUT $PROFILE $INSTALL_DIR
Icon=${ICON_PATH}$ICON
Type=Application
Path=$INSTALL_DIR
EOL

chmod +x ${INSTALL_DIR}$GAME_NAME.desktop
chmod +x ${INSTALL_DIR}uninstall.desktop
ln -sf "${INSTALL_DIR}$GAME_NAME.desktop" $SHORTCUT
