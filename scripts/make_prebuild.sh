#! /bin/sh

echo "autogen..."
./autogen.sh || { echo "autogen failed."; exit 1; }

echo "make ramdisk.sh"
./scripts/create_ramdisk.sh || { echo "create ramdisk failed."; exit 1; }

echo "make fs tar"
tar -vcf output/fs.tar --transform 's|^output/||' output/fs  || { echo "tar failed."; exit 1; }

echo "copy to prebuild"
find prebuild/ -type f -exec rm -rf {} \;
find output/ -path "output/fs" -prune -o -type f -exec cp -rf {} prebuild \;
