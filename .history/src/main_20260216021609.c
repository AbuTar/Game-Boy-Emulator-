#include <stdio.h>
#include "cpu.h"
#include "memory.h"


int main() {
    CPU cpu;
    memory_init();
    cpu_init(&cpu);

    int tests_passed = 0;
    int tests_failed = 0;

    // ── Test 1: ADD A, B ──
    cpu_init(&cpu);
    cpu.A = 0x05;
    cpu.B = 0x0A;
    add_reg(&cpu, &cpu.A, cpu.B);
    if (cpu.A == 0x0F && !get_flag(&cpu, FLAG_Z) && !get_flag(&cpu, FLAG_C)) {
        printf("✓ Test 1 PASSED: ADD A,B → 0x%02X\n", cpu.A);
        tests_passed++;
    } else {
        printf("✗ Test 1 FAILED: ADD A,B → 0x%02X (expected 0x0F)\n", cpu.A);
        tests_failed++;
    }

    // ── Test 2: ADD overflow sets carry ──
    cpu_init(&cpu);
    cpu.A = 0xFF;
    cpu.B = 0x01;
    add_reg(&cpu, &cpu.A, cpu.B);
    if (cpu.A == 0x00 && get_flag(&cpu, FLAG_Z) && get_flag(&cpu, FLAG_C)) {
        printf("✓ Test 2 PASSED: ADD overflow → A=0x%02X C=1 Z=1\n", cpu.A);
        tests_passed++;
    } else {
        printf("✗ Test 2 FAILED: A=0x%02X Z=%d C=%d\n",
            cpu.A, get_flag(&cpu, FLAG_Z), get_flag(&cpu, FLAG_C));
        tests_failed++;
    }

    // ── Test 3: SUB sets zero flag ──
    cpu_init(&cpu);
    cpu.A = 0x05;
    sub_reg(&cpu, &cpu.A, 0x05);
    if (cpu.A == 0x00 && get_flag(&cpu, FLAG_Z) && get_flag(&cpu, FLAG_N)) {
        printf("✓ Test 3 PASSED: SUB sets Z flag\n");
        tests_passed++;
    } else {
        printf("✗ Test 3 FAILED: A=0x%02X Z=%d N=%d\n",
            cpu.A, get_flag(&cpu, FLAG_Z), get_flag(&cpu, FLAG_N));
        tests_failed++;
    }

    // ── Test 4: PUSH/POP roundtrip ──
    cpu_init(&cpu);
    cpu.sp = 0xFFFE;
    push(&cpu, 0x1234);
    u16 val = pop(&cpu);
    if (val == 0x1234 && cpu.sp == 0xFFFE) {
        printf("✓ Test 4 PASSED: PUSH/POP roundtrip → 0x%04X\n", val);
        tests_passed++;
    } else {
        printf("✗ Test 4 FAILED: got 0x%04X SP=0x%04X\n", val, cpu.sp);
        tests_failed++;
    }

    // ── Test 5: INC wraps and sets zero ──
    cpu_init(&cpu);
    cpu.B = 0xFF;
    inc_reg(&cpu, &cpu.B);
    if (cpu.B == 0x00 && get_flag(&cpu, FLAG_Z)) {
        printf("✓ Test 5 PASSED: INC 0xFF wraps to 0x00\n");
        tests_passed++;
    } else {
        printf("✗ Test 5 FAILED: B=0x%02X Z=%d\n", cpu.B, get_flag(&cpu, FLAG_Z));
        tests_failed++;
    }

    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed;  // Returns 0 if all passed
}