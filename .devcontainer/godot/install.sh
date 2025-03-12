#!/bin/sh
set -e

echo "Installing Godot"

VERSION=${VERSION:-4.4-stable}
echo "The provided version is: $VERSION"

cd /tmp
curl -sSL https://github.com/godotengine/godot-builds/releases/download/${VERSION}/Godot_v${VERSION}_linux.x86_64.zip --output godot.zip
unzip -q -o godot.zip
mv Godot_v${VERSION}_linux.x86_64 /usr/local/bin/godot
chmod +x /usr/local/bin/godot
