cd $1
mkdir -p $2
./config shared --prefix=$2
make && make install 
