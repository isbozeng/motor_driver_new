#! /bin/bash
pushd .
if [ -d ../build ]; then
    cd ../build
else
    mkdir ../build
    cd ../build
fi
rm -rf *
cmake ..
make -j8
cp *.so ../lib/
popd
make -j8
make stepLine
make disable
make setHome