CXX ?= g++

CXXFLAGS ?= -std=c++17 -O3 -march=native -Wall -Wextra -Wpedantic
OMPFLAGS := -fopenmp
LDFLAGS ?=

SEQ_BINS := mergesort_seq kway_seq
PAR_BINS := mergesort_par kway_par kway_ranks kway_full
ALL_BINS := $(SEQ_BINS) $(PAR_BINS)

.PHONY: all seq par kway clean help debug perf-build perf-seq perf-par perf-all

all: $(ALL_BINS)

seq: $(SEQ_BINS)

par: $(PAR_BINS)

kway: kway_seq kway_par kway_ranks kway_full

mergesort_seq: mergesort_seq.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

kway_seq: kway_mergesort_seq.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

mergesort_par: mergesort_par.cpp
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $< -o $@ $(LDFLAGS)

kway_par: kway_mergesort_par.cpp
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $< -o $@ $(LDFLAGS)

kway_ranks: kway_mergesort_ranks.cpp
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $< -o $@ $(LDFLAGS)

kway_full: kway_mergesort_full.cpp
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $< -o $@ $(LDFLAGS)

# Debug build for validation and debugging
debug: CXXFLAGS := -std=c++17 -O0 -g -fno-omit-frame-pointer -Wall -Wextra -Wpedantic
debug: clean all

clean:
	rm -f $(ALL_BINS)

help:
	@echo "Targets available:"
	@echo "  make all        -> compile all binaries"
	@echo "  make seq        -> compile sequential versions"
	@echo "  make par        -> compile parallel versions (OpenMP)"
	@echo "  make kway       -> compile all k-way versions"
	@echo "  make debug      -> clean + debug build"
	@echo "  make clean      -> remove generated binaries"