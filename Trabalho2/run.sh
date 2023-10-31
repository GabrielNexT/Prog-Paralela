file_name=$1
bin_name=program.exe

if [ -z "$file_name" ]
then
  echo "Error: You must provide a .c file as a parameter"
  exit 1
fi

echo "Compiling..."
gcc $file_name -O3 -lm -lX11 -fopenmp -o $bin_name

if [ "$?" -ne "0" ]; then
  echo "Compilation error, check your code."
  exit 1
fi

for i in 1 2 4 8 12 16 32 64; do
  echo "Running with $i threads"
  export OMP_NUM_THREADS=$i
  echo "-----------------------------------"
  for j in $(seq 1 5); do
    ./$bin_name
    if [ "$?" -ne "0" ]; then
      echo "Execution error, check your code."
      exit 1
    fi
  done
  echo "-----------------------------------"
done

[ -e $bin_name ] && rm $bin_name

echo "Success!"
