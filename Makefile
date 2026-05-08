# Project management tasks; see CMakeLists.txt for building the project.

VENV = venv
CONAN = . $(VENV)/bin/activate && conan
PYTHON = . $(VENV)/bin/activate && python3
PYTEST = $(PYTHON) -m pytest -v


$(VENV)/.make-update: requirements-env.txt
	python3 -m venv $(VENV)
	$(PYTHON) -m pip install -U pip  # needs to be updated first
	$(PYTHON) -m pip install -U -r $<
	touch $@


.PHONY: conan
conan: conanfile.py
	$(CONAN) profile detect --exist-ok
	$(CONAN) install --build=missing --settings=build_type=Debug .
	$(CONAN) install --build=missing --settings=build_type=Release .


.PHONY: dev
dev: $(VENV)/.make-update conan
	cmake --preset conan-debug
	cmake --preset conan-release


.PHONY: build
build: dev
	cmake --build --preset conan-debug


.PHONY: test-unit
test-unit: build
	ctest --test-dir build/Debug --output-on-failure


.PHONY: test-integration
test-integration: build
	$(PYTEST) tests/integration/


.PHONY: test
test: test-unit test-integration


.PHONY: docs
docs: conan
	cmake --build build/Debug --target docs
