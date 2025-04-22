CC = gcc
CFLAGS = -Wall -Wextra -g -O2 # Optimization for tests, -g still useful
LDFLAGS = -lm

# --- Files for Main Application ---
MAIN_SRCS = main.c skiplist.c record.c persistence.c
MAIN_OBJS = $(MAIN_SRCS:.c=.o)
TARGET = crud_db
DB_FILENAME = crud_database.bin # Used by main app and clean target

# --- Files for Test Runner ---
TEST_SRCS = test.c skiplist.c record.c # Note: No persistence needed for tests
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_TARGET = test_runner
RESULTS_FILE = results.csv

# --- Default Target: Build the main application ---
all: $(TARGET)

# --- Rules for Main Application ---
$(TARGET): $(MAIN_OBJS)
	$(CC) $(CFLAGS) $(MAIN_OBJS) -o $(TARGET) $(LDFLAGS)

main.o: main.c skiplist.h record.h persistence.h
	$(CC) $(CFLAGS) -c main.c -o main.o

persistence.o: persistence.c persistence.h skiplist.h record.h
	$(CC) $(CFLAGS) -c persistence.c -o persistence.o

# --- Rules for Test Runner ---
# Build the test runner executable
test: $(TEST_TARGET) # Add a simple 'make test' target to build the runner

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(TEST_OBJS) -o $(TEST_TARGET) $(LDFLAGS)

test.o: test.c skiplist.h record.h
	$(CC) $(CFLAGS) -c test.c -o test.o

# --- Common Object File Rules (used by both targets) ---
skiplist.o: skiplist.c skiplist.h record.h
	$(CC) $(CFLAGS) -c skiplist.c -o skiplist.o

record.o: record.c record.h
	$(CC) $(CFLAGS) -c record.c -o record.o


# --- Test Execution Targets ---

# Define default values for N and M (can be overridden from command line)
N ?= 3000
M ?= 1000

# Run Insertion Test for N records using the test runner
test-insert: $(TEST_TARGET) # Depends on the test runner executable
	@echo "Running Insertion Test (N=$(N))..."
	./$(TEST_TARGET) --test-insert $(N) >> $(RESULTS_FILE) # Execute test_runner
	@echo "Insertion test complete. Results appended to $(RESULTS_FILE)"

# Run Search Test for M searches in a list of N records
test-search: $(TEST_TARGET)
	@echo "Running Search Test (N=$(N), M=$(M))..."
	./$(TEST_TARGET) --test-search $(N) $(M) >> $(RESULTS_FILE) # Execute test_runner
	@echo "Search test complete. Results appended to $(RESULTS_FILE)"

# Run Deletion Test for M deletions in a list of N records
test-delete: $(TEST_TARGET)
	@echo "Running Deletion Test (N=$(N), M=$(M))..."
	./$(TEST_TARGET) --test-delete $(N) $(M) >> $(RESULTS_FILE) # Execute test_runner
	@echo "Deletion test complete. Results appended to $(RESULTS_FILE)"

# Run all tests with specified N and M
test-all: clean-results test-insert test-search test-delete
	@echo "All tests complete for N=$(N), M=$(M)."
	@echo "Results are in $(RESULTS_FILE)"

# Target to clean previous results
clean-results:
	@echo "Cleaning previous results file ($(RESULTS_FILE))..."
	rm -f $(RESULTS_FILE)

# --- Clean Target ---
clean: clean-results
	# Remove objects for both main app and test runner
	rm -f $(MAIN_OBJS) $(TEST_OBJS) \
	# Remove executables
	      $(TARGET) $(TEST_TARGET) \
	# Remove other generated files
	      $(DB_FILENAME)

# Phony targets are not files
.PHONY: all clean clean-results test test-insert test-search test-delete test-all
