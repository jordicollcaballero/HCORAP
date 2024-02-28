for i in TXT_10-30_4-5_NEW/*; do 
	echo $i;
	name=$(basename $i); 
	../bin/release/rl2sat -e=1 -f=dimacs -S=0 $i > WCNF_10-30_4-5_NEW/${name}_HARD.wcnf ; 
	../bin/release/rl2sat -e=1 -f=dimacs -S=1 $i > WCNF_10-30_4-5_NEW/${name}_SOFT.wcnf ;
done

