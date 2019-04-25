#!/bin/bash

mkdir -p build
pushd build

cmake .. \
&& make \
&& (
	./run_serial_prime > ../sample_output_04_fast.txt && echo 'written: serial_prime'
	./run_chat > ../sample_outout_chat.txt && echo 'written: chat'
)

popd

