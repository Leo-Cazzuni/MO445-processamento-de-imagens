inputdir=$1;
outputdir=$2;

cmd="mkdir $outputdir";eval $cmd

a=0
for file in $inputdir*.pgm
do 
    a=$(($a+1));
    echo -e "$a:"

    filename="${file##*/}"
    dir="${fullpath:0:${#fullpath} - ${#filename}}"
    base="${filename%.[^.]*}"
    ext="${filename:${#base} + 1}"                 


    cmd="iftLabels2Borders $file $outputdir$filename"; echo -e $cmd; eval $cmd;
done 
echo "total of images processed is: $a";