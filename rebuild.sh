#!/bin/bash

pushd build

cmake .. \
&& make \
&& (
	./run_02 > ../sample_output_02.txt && echo 'written: run_02'
	./run_03 > ../sample_output_03.txt && echo 'written: run_03'
	./run_04 > ../sample_output_04.txt && echo 'written: run_04'
	./run_04_fast > ../sample_output_04_fast.txt && echo 'written: run_04_fast'
)

popd

