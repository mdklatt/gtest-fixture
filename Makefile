# Project management tasks; see CMakeLists.txt for building the project.

BUILD_TYPE = Debug
BUILD_ROOT = build/$(BUILD_TYPE)
VENV = .venv
CONAN = . $(VENV)/bin/activate && conan
PYTHON = . $(VENV)/bin/activate && python


$(VENV)/.make-update: requirements-env.txt
	python -m venv $(VENV)
	$(PYTHON) -m pip install -U pip  # needs to be updated first
	$(PYTHON) -m pip install -U -r $^
	touch $@


$(BUILD_ROOT)/conan_toolchain.cmake: conanfile.txt
	$(CONAN) install --output-folder=$(BUILD_ROOT) -s build_type=$(BUILD_TYPE) -s compiler.cppstd=17 --build missing .


.PHONY: dev
dev: $(VENV)/.make-update $(BUILD_ROOT)/conan_toolchain.cmake
	cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_TOOLCHAIN_FILE=$(BUILD_ROOT)/conan_toolchain.cmake -DBUILD_TESTING=ON -DBUILD_DOCS=ON -S . -B $(BUILD_ROOT)


.PHONY: build
build:
	cmake --build $(BUILD_ROOT)


.PHONY: test
test: build
	. $(VENV)/bin/activate
	cd $(BUILD_ROOT) && ctest --output-on-failure


.PHONY: docs
docs:
	cmake --build $(BUILD_ROOT) --target docs
