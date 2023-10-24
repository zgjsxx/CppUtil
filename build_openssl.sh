cd $1
mkdir -p $2
echo $2/lib/libssl.so
if [ ! -f $2/lib/libssl.so ];then
    ./config --prefix=$2
    make && make install 
fi
