#!/bin/bash

# Script to create desktop shortcuts for BrickGame
INSTALL_DIR="$1"
DESKTOP_DIR="$HOME/Desktop"
ICON_SRC="./misc/brick_game.png"  # Changed from ./misc
ICON_DEST="$INSTALL_DIR/icons/brick_game.png"  # Install dir instead of SHARE_DIR

# Create necessary directories
mkdir -p "$INSTALL_DIR/icons" "$DESKTOP_DIR"  # Changed to install dir

# Copy icon to install directory
cp "$ICON_SRC" "$ICON_DEST"  # Changed destination

# Create desktop entry for BrickGameCLI
cat >"$DESKTOP_DIR/BrickGameCLI.desktop" <<EOL
[Desktop Entry]
Name=BrickGame CLI
Exec=konsole --profile BrickGameCLI -e $INSTALL_DIR/bin/BrickGameCLI
Icon=$ICON_DEST
Type=Application
Terminal=false
Categories=Game;
Comment=Classic brick game in terminal
EOL

# Create desktop entry for BrickGameDesktop
cat >"$DESKTOP_DIR/BrickGameDesktop.desktop" <<EOL
[Desktop Entry]
Name=BrickGame Desktop
Exec=$INSTALL_DIR/bin/BrickGameDesktop
Icon=$ICON_DEST
Type=Application
Terminal=false
Categories=Game;
Comment=Classic brick game with desktop interface
EOL

# Create custom konsole profile for CLI version
mkdir -p "$HOME/.local/share/konsole"
cat >"$HOME/.local/share/konsole/BrickGameCLI.profile" <<EOL
[General]
Name=BrickGameCLI
Parent=FALLBACK/
ShowTerminalSizeHint=true
TerminalCenter=true
TerminalColumns=34
TerminalRows=27
EOL

# Make desktop entries executable
chmod +x "$DESKTOP_DIR/BrickGameCLI.desktop"
chmod +x "$DESKTOP_DIR/BrickGameDesktop.desktop"
chmod +x "$INSTALL_DIR/bin/BrickGameCLI"
chmod +x "$INSTALL_DIR/bin/BrickGameDesktop"

echo "Created desktop shortcuts:"
echo "  - $DESKTOP_DIR/BrickGameCLI.desktop"
echo "  - $DESKTOP_DIR/BrickGameDesktop.desktop"
echo "Icon installed to: $ICON_DEST"