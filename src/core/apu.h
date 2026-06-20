#ifndef APU_H
#define APU_H

#include "types.h"
#include <stdbool.h>

// Output sample rate - 32768 divides the GB's 4194304 Hz clock exactly (4194304 / 32768 = 128)
#define APU_SAMPLE_RATE   48000

// Ring buffer capacity (~370ms of stereo audio at 32768 Hz)
#define APU_RING_CAPACITY 12288

// APU Components

typedef struct {

    // Channel 1 - Square wave with frequency sweep

    bool ch1_enabled;
    bool ch1_dac_enabled;
    u16  ch1_freq_timer;
    u8   ch1_duty_pos;
    u16  ch1_shadow_freq;       // Sweep's working copy of frequency
    u16  ch1_sweep_timer;
    bool ch1_sweep_enabled;
    bool ch1_sweep_negate_used; // Tracks if negate mode was used since last trigger
    u16  ch1_length_timer;
    u8   ch1_volume;
    u8   ch1_envelope_timer;
    bool ch1_envelope_running;

    // Channel 2 - Square wave

    bool ch2_enabled;
    bool ch2_dac_enabled;
    u16  ch2_freq_timer;
    u8   ch2_duty_pos;
    u16  ch2_length_timer;
    u8   ch2_volume;
    u8   ch2_envelope_timer;
    bool ch2_envelope_running;

    // Channel 3 - Wave

    bool ch3_enabled;
    bool ch3_dac_enabled;
    u16  ch3_freq_timer;
    u8   ch3_sample_index;  // 0-31, nibble index into wave RAM
    u16  ch3_length_timer;
    u8   ch3_sample_buffer; // Last fetched nibble

    // Channel 4 - Noise

    bool ch4_enabled;
    bool ch4_dac_enabled;
    u32  ch4_freq_timer;
    u16  ch4_lfsr;
    u16  ch4_length_timer;
    u8   ch4_volume;
    u8   ch4_envelope_timer;
    bool ch4_envelope_running;

    // Frame Sequencer

    u16 frame_seq_div;  // Counts T-cycles up to 8192 (512 Hz)
    u8  frame_seq_step; // 0-7

    // Master Power

    bool power_on; // NR52 bit 7

    // Resampling

    u32 sample_div_counter; // Accumulates T-cycles toward next output sample

    // Output Ring Buffer (stereo interleaved, signed 16-bit)

    s16          ring[APU_RING_CAPACITY * 2];
    volatile u32 ring_write; // Next slot to write (in sample-pairs)
    volatile u32 ring_read;  // Next slot to read  (in sample-pairs)

} APU;

// Functions

void apu_init(APU* apu);
void apu_step(APU* apu, u8 t_cycles); // Called once per instruction, same as ppu_step

// Register I/O (memory.c routes 0xFF10-0xFF3F here)

void apu_write(APU* apu, u16 address, u8 value);
u8   apu_read(APU* apu, u16 address);

// Ring buffer consumer API (used by the platform audio backend)

u32 apu_read_samples(APU* apu, s16* out, u32 max_pairs); // Returns number of stereo pairs copied
u32 apu_available_samples(const APU* apu);

#endif