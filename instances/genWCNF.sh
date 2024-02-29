for i in TXT_10-35_4-5_U40/*; do 
	echo $i;
	name=$(basename $i); 
	../bin/release/rl2sat -e=1 -f=dimacs -S=0 $i > WCNF_10-35_4-5_U40/${name}_HARD.wcnf ; 
	../bin/release/rl2sat -e=1 -f=dimacs -S=1 $i > WCNF_10-35_4-5_U40/${name}_SOFT.wcnf ;
done

