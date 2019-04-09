for x in *
do
	echo -e "-----------------\n";
	echo $x
	echo -e "-----------------\n";
	cat $x
done
unset x
