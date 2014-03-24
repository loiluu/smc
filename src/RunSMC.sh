#!/bin/bash
# Author : Loi Luu
if (( "$#" > 2 ))
then
    echo "Usage info:..."
    echo "./RunSMC <input_file> [length]"
    echo "for example, run as: ./RunSMC input_smc.txt 10."
exit 1
fi
echo "Calling the parser..."
python smc_parser.py "$1";
echo "Calling the counter..."
./main_smc inter_input.txt
echo "Finished. Please check the bounds in output.txt"
if (( "$#" == 2 ))
then
   ./collect_leakage output.txt "$2";
fi
rm inter_input.txt;
#remove the python generated file
rm -f *.pyc;
rm -f parsetab.py;
rm -f parser.out;
