# Project management tasks; see CMakeLists.txt for building the project.

VENV = venv
CONAN = . $(VENV)/bin/activate && conan
PYTHON = . $(VENV)/bin/activate && python


$(VENV)/.make-update: requirements-env.txt
	python -m venv $(VENV)
	$(PYTHON) -m pip install -U pip  # needs to be updated first
	$(PYTHON) -m pip install -U -r $<
	touch $@


.PHONY: conan
conan: conanfile.py
	$(CONAN) profile detect --exist-ok
	$(CONAN) install --build=missing --output-folder=build/debug/conan --settings=build_type=Debug .
	$(CONAN) install --build=missing --output-folder=build/release/conan --settings=build_type=Release .


.PHONY: dev
dev: $(VENV)/.make-update conan
	cmake --preset conan-debug
	cmake --preset conan-release


.PHONY: build
build: dev
	cmake --build --preset conan-debug


.PHONY: test
test: build
	. $(VENV)/bin/activate
	cd build/debug/conan && ctest --output-on-failure


.PHONY: docs
docs: conan
	cmake --build build/debug/conan --target docs
