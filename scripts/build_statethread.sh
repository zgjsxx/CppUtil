cd $1
mkdir -p $2

make linux-debug
cp LINUX_$(uname -r)_DBG/libst.a $2/lib
