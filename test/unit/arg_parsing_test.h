#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../../src/args.h"

void test_arg_parsing();
void test_min_size_parsing();

void test_arg_parsing() {
    printf("[UNIT-TEST] Running argument parsing tests...\n");

    test_min_size_parsing();

    printf("[UNIT-TEST] Passed argument parsing tests!\n");
}

bool fcmp(double a, double b) {
    return fabs(a - b) < 0.0001;
}

void test_min_size_parsing() {
    size_t bytes = 0;
    double percentage = 0;
    assert(try_parse_min_size_str("100", &bytes, &percentage));
    assert(bytes == 100);
    assert(percentage == 0);

    assert(try_parse_min_size_str("100000", &bytes, &percentage));
    assert(bytes == 100000);
    assert(percentage == 0);

    bytes = 0;
    // Empty space fails
    assert(!try_parse_min_size_str("", &bytes, &percentage));
    // Unchanged
    assert(bytes == 0);
    assert(percentage == 0);

    // Test suffix
    assert(try_parse_min_size_str("1K", &bytes, &percentage));
    assert(bytes == 1000);

    assert(try_parse_min_size_str("1KB", &bytes, &percentage));
    assert(bytes == 1000);
    // Test case insensitivity
    assert(try_parse_min_size_str("1kb", &bytes, &percentage));
    assert(bytes == 1000);

    // Test kibibytes
    assert(try_parse_min_size_str("1KiB", &bytes, &percentage));
    assert(bytes == 1024);

    // Test the rest of the suffixes
    assert(try_parse_min_size_str("1M", &bytes, &percentage));
    assert(bytes == 1000000);
    assert(try_parse_min_size_str("1G", &bytes, &percentage));
    assert(bytes == 1000000000);
    assert(try_parse_min_size_str("1T", &bytes, &percentage));
    assert(bytes == 1000000000000);

    // Test decimal numbers
    assert(try_parse_min_size_str("1.5M", &bytes, &percentage));
    assert(bytes == 1500000);
    assert(try_parse_min_size_str("1.32G", &bytes, &percentage));
    assert(bytes == 1320000000);
    // Comma separators are not allowed
    assert(!try_parse_min_size_str("1,32G", &bytes, &percentage));
    // Byte values under 1 not allowed
    assert(!try_parse_min_size_str("0.1", &bytes, &percentage));
    assert(!try_parse_min_size_str("0.0001K", &bytes, &percentage));

    // Test percentage
    bytes = 0;
    assert(try_parse_min_size_str("32%", &bytes, &percentage));
    assert(fcmp(percentage, 0.32));

    assert(try_parse_min_size_str("3.3%", &bytes, &percentage));
    assert(fcmp(percentage, 0.033));
    // Percentage over 100% not allowed
    assert(!try_parse_min_size_str("101.2%", &bytes, &percentage));
}