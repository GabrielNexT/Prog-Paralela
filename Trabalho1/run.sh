file_name=$1
bin_name=program.exe

if [ -z "$file_name" ]
then
  echo "Error: You must provide a .c file as a parameter"
  return 1
fi

echo "Compiling..."
mpicc $file_name -O3 -Wall -lm -o $bin_name

if [ "$?" -ne "0" ]; then
  echo "Compilation error, check your code."
  return 1
fi

for i in 1 2 4 8 12 16; do
  echo "Running with $i processes\n"
  mpirun --oversubscribe -np $i $bin_name
  echo "\n"
done

[ -e $bin_name ] && rm $bin_name

echo "Success!"
