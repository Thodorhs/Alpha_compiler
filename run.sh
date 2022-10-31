cd src
if [ $# -eq 1 ]
    then
    if [ "$1" == "-h" ]
        then
        echo "formats to run:"
        echo "-h for help"
        echo "-c for clean"
        echo "your.txt to run"
    elif [ "$1" == "-c" ]
      then make clean
    else
      if [ -d "../out" ]
          then
          make all
          cd ../out
          ./targetCode ../$1
          ./alpha_vm > instructions.txt
      else
          make out
          make all
          cd ../out
          ./targetCode ../$1
          ./alpha_vm > instructions.txt
      fi
    fi
else
  echo "try -h for help"
fi