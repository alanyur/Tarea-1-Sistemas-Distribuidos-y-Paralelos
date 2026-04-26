CXX ?= g++

CXXFLAGS ?= -std=c++17 -O3 -march=native -Wall -Wextra -Wpedantic
OMPFLAGS := -fopenmp
LDFLAGS ?=
BIN_DIR ?= bin

SEQ_NAMES := mergesort_seq kway_seq
PAR_NAMES := mergesort_par kway_par kway_ranks kway_full
SEQ_BINS := $(addprefix $(BIN_DIR)/,$(SEQ_NAMES))
PAR_BINS := $(addprefix $(BIN_DIR)/,$(PAR_NAMES))
ALL_BINS := $(SEQ_BINS) $(PAR_BINS)
LEGACY_BINS := $(SEQ_NAMES) $(PAR_NAMES)

.PHONY: all seq par kway clean help debug perf-build perf-seq perf-par perf-all \
		mergesort_seq mergesort_par kway_seq kway_par kway_ranks kway_full

all: $(ALL_BINS)

seq: $(SEQ_BINS)

par: $(PAR_BINS)

kway: kway_seq kway_par kway_ranks kway_full

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

mergesort_seq: $(BIN_DIR)/mergesort_seq

kway_seq: $(BIN_DIR)/kway_seq

mergesort_par: $(BIN_DIR)/mergesort_par

kway_par: $(BIN_DIR)/kway_par

kway_ranks: $(BIN_DIR)/kway_ranks

kway_full: $(BIN_DIR)/kway_full

$(BIN_DIR)/mergesort_seq: mergesort_seq.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/kway_seq: kway_mergesort_seq.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/mergesort_par: mergesort_par.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/kway_par: kway_mergesort_par.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/kway_ranks: kway_mergesort_ranks.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/kway_full: kway_mergesort_full.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $< -o $@ $(LDFLAGS)

# Debug build for validation and debugging
debug: CXXFLAGS := -std=c++17 -O0 -g -fno-omit-frame-pointer -Wall -Wextra -Wpedantic
debug: clean all

clean:
	rm -rf $(BIN_DIR)
	rm -f $(LEGACY_BINS)

help:
	@echo "Targets available:"
	@echo "  make all        -> compile all binaries into $(BIN_DIR)/"
	@echo "  make seq        -> compile sequential versions into $(BIN_DIR)/"
	@echo "  make par        -> compile parallel versions (OpenMP) into $(BIN_DIR)/"
	@echo "  make kway       -> compile all k-way versions into $(BIN_DIR)/"
	@echo "  make mergesort_seq | mergesort_par | kway_seq | kway_par | kway_ranks | kway_full"
	@echo "  make debug      -> clean + debug build"
	@echo "  make clean      -> remove generated $(BIN_DIR)/ folder and legacy root binaries"