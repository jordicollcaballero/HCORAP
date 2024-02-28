for i in `seq 1 20`; do 
	for A in 10 15 20 25 30; do 
		for V in 4 5; do  
			U=30;  
			echo instance_${U}_${A}_${V}_${i}.txt
			python3 create_instances.py $U $A $V > TXT_10-30_4-5_NEW/instance_${U}_${A}_${V}_${i}.txt 2>/dev/null
		done; 
	done; 
done

