rm -f smc.conf
cp smc.cs.conf smc.conf
make clean
make

echo "*****Test with 4 utilities*******"
echo "====     Testing obscure for input of length 10  ==="
./RunQIFS.sh case_studies/obscure.1.input 10

echo
echo "====     Testing grep for file size of 629 bytes  ==="
./RunQIFS.sh case_studies/grep.input 629

echo
echo "====     Testing wc for file size of 629 bytes  ==="
./RunQIFS.sh case_studies/wc.input 629

echo
echo "====     Testing csplit for file size of 629 bytes  ==="
./RunQIFS.sh case_studies/csplit.input 629


echo
echo "*****Comparison with FuzzBALL*******"
echo "====     Testing obscure* for input of length 6  ==="
./RunQIFS.sh case_studies/obscure.2.input 6

echo
echo "====     Testing strstr for input of length 5  ==="
./RunQIFS.sh case_studies/strstr.1.input 5

echo
echo "====     Testing strstr for input of length 4  ==="
./RunQIFS.sh case_studies/strstr.1.input 4

echo
echo "====     Testing match_regex for input of length 4  ==="
./RunQIFS.sh case_studies/regex.input 4


echo
echo "*****Comparison with Castro et al.*******"
echo "====     Testing Ghttpd with input of length 620   ==="
./RunQIFS.sh case_studies/ghttpd.input 620

echo
echo "====     Testing NullHTTPd with input of length 500   ==="
./RunQIFS.sh case_studies/nullhttpd.input 500

echo
echo "********Comparison with QUAIL*******"
echo "====     Testing strstr(ab) with input of length 5   ==="
./RunQIFS.sh case_studies/strstr.2.input 5

echo
echo "====     Testing strstr(ab) with input of length 7   ==="
./RunQIFS.sh case_studies/strstr.2.input 7

echo
echo "====     Testing contains(ab) with input of length 5   ==="
./RunQIFS.sh case_studies/contains.input 5

echo
echo "====     Testing contains(ab) with input of length 7   ==="
./RunQIFS.sh case_studies/contains.input 7
