#!/bin/bash

mkdir -p build
pushd build > /dev/null

cmake .. \
&& make \
&& (
	echo '---- serial_prime ----' && ./run_serial_prime > ../sample_output_04_fast.txt && echo 'written: serial_prime'
	echo '---- chat ----' && ./run_chat > ../sample_outout_chat.txt && echo 'written: chat'
	echo '---- calc ----' && ./run_calc > ../sample_outout_calc.txt && echo 'written: calc'
)

popd > /dev/null

