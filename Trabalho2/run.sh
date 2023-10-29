file_name=$1
bin_name=program.exe

if [ -z "$file_name" ]
then
  echo "Error: You must provide a .c file as a parameter"
  exit 1
fi

echo "Compiling..."
mpicc $file_name -O3 -Wall -lm -o $bin_name

if [ "$?" -ne "0" ]; then
  echo "Compilation error, check your code."
  exit 1
fi

for i in 1 2 4 8 12 16 32 64; do
  for j in $(seq 1 5); do
    echo "Running with $i processes ($j/5)"
    echo "-----------------------------------"
    mpirun -np $i ./$bin_name
    if [ "$?" -ne "0" ]; then
      echo "Execution error, check your code."
      exit 1
    fi
    echo "-----------------------------------"
  done
done

[ -e $bin_name ] && rm $bin_name

echo "Success!"
