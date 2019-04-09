for x in *.txt
do
        echo $x
        cat $x
        echo -e "-----------------\n";
done
unset x