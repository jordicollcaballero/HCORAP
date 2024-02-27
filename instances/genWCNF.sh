for i in TXT_35-50_4-7/*; do 
	echo $i;
	name=$(basename $i); 
	../bin/release/rl2sat -e=1 -f=dimacs -S=0 $i > WCNF_35-50_4-7/${name}_HARD.wcnf ; 
	../bin/release/rl2sat -e=1 -f=dimacs -S=1 $i > WCNF_35-50_4-7/${name}_SOFT.wcnf ;
done

