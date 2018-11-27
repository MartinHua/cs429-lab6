types=("normal" "defrag")
for filename in ./traces/*; do
	for t in "${types[@]}"; do
		test_firstfit "$filename" $t
		sleep 1
		test_bestfit "$filename" $t
		sleep 1
	done
done
