for i in `seq 1 20`; do 
	for A in 35 40 45 50; do 
		for V in 4 5 6 7; do  
			U=30;  
			echo instance_${U}_${A}_${V}_${i}.txt
			python3 create_instances.py $U $A $V > TXT_35-50_4-7/instance_${U}_${A}_${V}_${i}.txt 2>/dev/null
		done; 
	done; 
done

