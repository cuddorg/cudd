all: test

configure:
	@cmake -S . -B /tmp/build . -DCUDD_BUILD_TESTS=ON

build: configure
	@cmake --build /tmp/build

test: build
	@ctest --test-dir /tmp/build --output-on-failure

coverage:
	@cmake -S . -B /tmp/build . -DCUDD_BUILD_TESTS=ON -DCUDD_BUILD_COVERAGE=ON
	@cmake --build /tmp/build 
	@cmake --build /tmp/build --target coverage

clean:
	@rm -rf /tmp/build
