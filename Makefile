default: build

configure:
	@cmake -S . -B /tmp/build . -DCMAKE_BUILD_TYPE=Release

build: configure
	@cmake --build /tmp/build

install: build
	@cmake --install /tmp/build

test:
	@cmake -S . -B /tmp/build . -DCUDD_BUILD_TESTS=ON
	@cmake --build /tmp/build
	@ctest --test-dir /tmp/build --output-on-failure

coverage:
	@cmake -S . -B /tmp/build . -DCUDD_BUILD_TESTS=ON -DCUDD_BUILD_COVERAGE=ON
	@cmake --build /tmp/build 
	@cmake --build /tmp/build --target coverage

scan:
	@rm -rf /tmp/build
	@scan-build cmake -S . -B /tmp/build -DCMAKE_BUILD_TYPE=Debug
	@scan-build -v --status-bugs cmake --build /tmp/build

clean:
	@rm -rf /tmp/build
