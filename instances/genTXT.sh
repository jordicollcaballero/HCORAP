for i in `seq 1 50`; do 
	for A in 10 15 20 25; do 
		for V in 4 5; do  
			for U in 30 40; do  
				echo instance_${U}_${A}_${V}_${i}.txt
				python3 create_instances.py $U $A $V > TXT_10-25_4-5_U${U}/instance_${U}_${A}_${V}_${i}.txt 2>/dev/null
			done
		done; 
	done; 
done

