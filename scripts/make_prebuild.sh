#! /bin/sh

echo "autogen..."
./autogen.sh || { echo "autogen failed."; exit 1; }

#echo "make ramdisk.sh"
#./scripts/create_ramdisk.sh || { echo "create ramdisk failed."; exit 1; }

echo "create flash img"
./scripts/create_flash_img.sh || { echo "create flash img failed."; exit 1; }


echo "copy to prebuild"
test -d prebuild/ || mkdir prebuild/
find prebuild/ -type f -exec rm -rf {} \;
find output/ -path "output/fs" -prune -o -type f -exec cp -rf {} prebuild \;
cp -rf docs/README.prebulid.md prebuild/README.md
