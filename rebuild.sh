#!/bin/bash

mkdir -p build
pushd build > /dev/null

cmake .. \
&& make \
&& (
	echo '---- serial_prime ----' && ./run_serial_prime > ../sample_output_04_fast.txt && echo 'written: serial_prime'
	echo '---- chat ----' && ./run_chat > ../sample_outout_chat.txt && echo 'written: chat'
	echo '---- calc ----' && ./run_calc > ../sample_outout_calc.txt && echo 'written: calc'
	# echo '---- lz77 ----' && ./run_lz77 > ../sample_outout_lz77.txt && echo 'written: lz77'
	echo '---- rand_pi ----' && ./run_rand_pi > ../sample_outout_rand_pi.txt && echo 'written: rand_pi'
)

popd > /dev/null

