.PHONY: build
build: ## Kicks off a build
	mkdir -p build build/test
	g++ --std=c++2a -o build/match_engine main.cpp
	g++ --std=c++2a -o build/test/unittest test/unittest.cpp -lgtest -lpthread

.PHONY: clean
clean: ## Cleans the build directory
	rm -rf build/*

