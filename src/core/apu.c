#include "apu.h"
#include <string.h>


// Shadow register array for 0xFF10-0xFF3F
// Wave RAM (0xFF30-0xFF3F) is stored separately as 16 bytes (32 x 4-bit samples)
// Most registers are simple read/write - only NR52 and channel status are derived live

static u8 s_wave_ram[16];
static u8 s_reg[0x30];

// Duty cycle waveforms (8 steps per duty, 1 = high, 0 = low)
static const u8 DUTY_TABLE[4][8] = {
    {0,0,0,0,0,0,0,1}, // 12.5%
    {1,0,0,0,0,0,0,1}, // 25%
    {1,0,0,0,1,1,1,1}, // 50%
    {0,1,1,1,1,1,1,0}, // 75%
};

// Noise channel divisor lookup (NR43 low 3 bits -> divisor)
static const u16 NOISE_DIVISORS[8] = {8, 16, 32, 48, 64, 80, 96, 112};

static u16 reg_idx(u16 address){ return address - 0xFF10; }


// ---------- Trigger helpers ----------

static void trigger_ch1(APU* apu){
    apu->ch1_enabled = true;
    if (apu->ch1_length_timer == 0) apu->ch1_length_timer = 64;

    u16 freq = ((s_reg[reg_idx(0xFF14)] & 0x07) << 8) | s_reg[reg_idx(0xFF13)];
    apu->ch1_freq_timer        = (2048 - freq) * 4;
    apu->ch1_envelope_timer    = s_reg[reg_idx(0xFF12)] & 0x07;
    apu->ch1_envelope_running  = true;
    apu->ch1_volume            = (s_reg[reg_idx(0xFF12)] >> 4) & 0x0F;

    // Sweep
    apu->ch1_shadow_freq = freq;
    u8 sweep_period = (s_reg[reg_idx(0xFF10)] >> 4) & 0x07;
    apu->ch1_sweep_timer   = (sweep_period != 0) ? sweep_period : 8;
    u8 sweep_shift         = s_reg[reg_idx(0xFF10)] & 0x07;
    apu->ch1_sweep_enabled = (sweep_period != 0) || (sweep_shift != 0);
    apu->ch1_sweep_negate_used = false;

    // Immediate overflow check on trigger
    if (sweep_shift != 0){
        u16 delta    = apu->ch1_shadow_freq >> sweep_shift;
        u16 new_freq = (s_reg[reg_idx(0xFF10)] & 0x08) ? (apu->ch1_shadow_freq - delta)
                                                        : (apu->ch1_shadow_freq + delta);
        if (new_freq > 2047) apu->ch1_enabled = false;
    }

    apu->ch1_dac_enabled = (s_reg[reg_idx(0xFF12)] & 0xF8) != 0;
    if (!apu->ch1_dac_enabled) apu->ch1_enabled = false;
}

static void trigger_ch2(APU* apu){
    apu->ch2_enabled = true;
    if (apu->ch2_length_timer == 0) apu->ch2_length_timer = 64;

    u16 freq = ((s_reg[reg_idx(0xFF19)] & 0x07) << 8) | s_reg[reg_idx(0xFF18)];
    apu->ch2_freq_timer       = (2048 - freq) * 4;
    apu->ch2_envelope_timer   = s_reg[reg_idx(0xFF17)] & 0x07;
    apu->ch2_envelope_running = true;
    apu->ch2_volume           = (s_reg[reg_idx(0xFF17)] >> 4) & 0x0F;

    apu->ch2_dac_enabled = (s_reg[reg_idx(0xFF17)] & 0xF8) != 0;
    if (!apu->ch2_dac_enabled) apu->ch2_enabled = false;
}

static void trigger_ch3(APU* apu){
    apu->ch3_enabled = true;
    if (apu->ch3_length_timer == 0) apu->ch3_length_timer = 256;

    u16 freq = ((s_reg[reg_idx(0xFF1E)] & 0x07) << 8) | s_reg[reg_idx(0xFF1D)];
    apu->ch3_freq_timer   = (2048 - freq) * 2;
    apu->ch3_sample_index = 0;

    apu->ch3_dac_enabled = (s_reg[reg_idx(0xFF1A)] & 0x80) != 0;
    if (!apu->ch3_dac_enabled) apu->ch3_enabled = false;
}

static void trigger_ch4(APU* apu){
    apu->ch4_enabled = true;
    if (apu->ch4_length_timer == 0) apu->ch4_length_timer = 64;

    u8 nr43         = s_reg[reg_idx(0xFF22)];
    u8 shift        = (nr43 >> 4) & 0x0F;
    u8 divisor_code = nr43 & 0x07;
    apu->ch4_freq_timer       = ((u32)NOISE_DIVISORS[divisor_code]) << shift;
    apu->ch4_lfsr             = 0x7FFF;
    apu->ch4_envelope_timer   = s_reg[reg_idx(0xFF21)] & 0x07;
    apu->ch4_envelope_running = true;
    apu->ch4_volume           = (s_reg[reg_idx(0xFF21)] >> 4) & 0x0F;

    apu->ch4_dac_enabled = (s_reg[reg_idx(0xFF21)] & 0xF8) != 0;
    if (!apu->ch4_dac_enabled) apu->ch4_enabled = false;
}


// ---------- Lifecycle ----------

void apu_init(APU* apu){
    memset(apu, 0, sizeof(APU));
    memset(s_wave_ram, 0, sizeof(s_wave_ram));
    memset(s_reg, 0, sizeof(s_reg));

    apu->power_on  = true;
    apu->ch4_lfsr  = 0x7FFF;

    // Power-on defaults matching DMG post-boot state
    s_reg[reg_idx(0xFF24)] = 0x77; // NR50 - full volume both sides
    s_reg[reg_idx(0xFF25)] = 0xF3; // NR51 - default panning
}


// ---------- Register I/O ----------

u8 apu_read(APU* apu, u16 address){

    // Wave RAM (0xFF30-0xFF3F)
    if (address >= 0xFF30 && address <= 0xFF3F){
        return s_wave_ram[address - 0xFF30];
    }

    // NR52 - power and live channel status
    if (address == 0xFF26){
        u8 v = 0x70; // Unused bits always read as 1
        if (apu->power_on)    v |= 0x80;
        if (apu->ch1_enabled) v |= 0x01;
        if (apu->ch2_enabled) v |= 0x02;
        if (apu->ch3_enabled) v |= 0x04;
        if (apu->ch4_enabled) v |= 0x08;
        return v;
    }

    // Sound registers (0xFF10-0xFF2F)
    if (address >= 0xFF10 && address <= 0xFF2F){
        return s_reg[reg_idx(address)];
    }

    return 0xFF;
}

void apu_write(APU* apu, u16 address, u8 value){

    // Wave RAM (0xFF30-0xFF3F)
    if (address >= 0xFF30 && address <= 0xFF3F){
        s_wave_ram[address - 0xFF30] = value;
        return;
    }

    // While powered off, writes are ignored except NR52 and wave RAM
    if (!apu->power_on && address != 0xFF26){
        return;
    }

    if (address >= 0xFF10 && address <= 0xFF25){
        s_reg[reg_idx(address)] = value;
    }

    switch (address){

        // Channel 1

        case 0xFF12: // NR12 - envelope
            apu->ch1_dac_enabled = (value & 0xF8) != 0;
            if (!apu->ch1_dac_enabled) apu->ch1_enabled = false;
            break;

        case 0xFF14: // NR14 - trigger
            if (value & 0x80) trigger_ch1(apu);
            break;

        // Channel 2

        case 0xFF17: // NR22 - envelope
            apu->ch2_dac_enabled = (value & 0xF8) != 0;
            if (!apu->ch2_dac_enabled) apu->ch2_enabled = false;
            break;

        case 0xFF19: // NR24 - trigger
            if (value & 0x80) trigger_ch2(apu);
            break;

        // Channel 3

        case 0xFF1A: // NR30 - DAC enable
            apu->ch3_dac_enabled = (value & 0x80) != 0;
            if (!apu->ch3_dac_enabled) apu->ch3_enabled = false;
            break;

        case 0xFF1E: // NR34 - trigger
            if (value & 0x80) trigger_ch3(apu);
            break;

        // Channel 4

        case 0xFF21: // NR42 - envelope
            apu->ch4_dac_enabled = (value & 0xF8) != 0;
            if (!apu->ch4_dac_enabled) apu->ch4_enabled = false;
            break;

        case 0xFF23: // NR44 - trigger
            if (value & 0x80) trigger_ch4(apu);
            break;

        // Master power

        case 0xFF26: // NR52
            apu->power_on = (value & 0x80) != 0;
            if (!apu->power_on){
                // Powering off clears all registers and disables all channels
                memset(s_reg, 0, sizeof(s_reg));
                apu->ch1_enabled = apu->ch2_enabled = apu->ch3_enabled = apu->ch4_enabled = false;
                apu->ch1_dac_enabled = apu->ch2_dac_enabled = apu->ch3_dac_enabled = apu->ch4_dac_enabled = false;
            }
            break;

        default: break;
    }

    // Writing NRx1 reloads the length counter
    if (address == 0xFF11) apu->ch1_length_timer = 64  - (value & 0x3F);
    if (address == 0xFF16) apu->ch2_length_timer = 64  - (value & 0x3F);
    if (address == 0xFF1B) apu->ch3_length_timer = 256 - value;
    if (address == 0xFF20) apu->ch4_length_timer = 64  - (value & 0x3F);
}


// ---------- Frame sequencer ----------

// 512 Hz, steps 0-7
// Step   Length   Sweep   Envelope
// 0      X
// 1
// 2      X        X
// 3
// 4      X
// 5
// 6      X        X
// 7                       X

static void clock_length(APU* apu){
    if ((s_reg[reg_idx(0xFF14)] & 0x40) && apu->ch1_enabled){
        if (apu->ch1_length_timer > 0){
            apu->ch1_length_timer--;
            if (apu->ch1_length_timer == 0) apu->ch1_enabled = false;
        }
    }
    if ((s_reg[reg_idx(0xFF19)] & 0x40) && apu->ch2_enabled){
        if (apu->ch2_length_timer > 0){
            apu->ch2_length_timer--;
            if (apu->ch2_length_timer == 0) apu->ch2_enabled = false;
        }
    }
    if ((s_reg[reg_idx(0xFF1E)] & 0x40) && apu->ch3_enabled){
        if (apu->ch3_length_timer > 0){
            apu->ch3_length_timer--;
            if (apu->ch3_length_timer == 0) apu->ch3_enabled = false;
        }
    }
    if ((s_reg[reg_idx(0xFF23)] & 0x40) && apu->ch4_enabled){
        if (apu->ch4_length_timer > 0){
            apu->ch4_length_timer--;
            if (apu->ch4_length_timer == 0) apu->ch4_enabled = false;
        }
    }
}

static void clock_sweep(APU* apu){
    if (!apu->ch1_enabled) return;

    if (apu->ch1_sweep_timer > 0) apu->ch1_sweep_timer--;
    if (apu->ch1_sweep_timer != 0) return;

    u8 sweep_period = (s_reg[reg_idx(0xFF10)] >> 4) & 0x07;
    apu->ch1_sweep_timer = (sweep_period != 0) ? sweep_period : 8;

    if (!apu->ch1_sweep_enabled || sweep_period == 0) return;

    u8   sweep_shift = s_reg[reg_idx(0xFF10)] & 0x07;
    bool negate      = (s_reg[reg_idx(0xFF10)] & 0x08) != 0;
    u16  delta       = apu->ch1_shadow_freq >> sweep_shift;
    u16  new_freq;

    if (negate){
        new_freq = apu->ch1_shadow_freq - delta;
        apu->ch1_sweep_negate_used = true;
    } else {
        new_freq = apu->ch1_shadow_freq + delta;
    }

    if (new_freq > 2047){
        apu->ch1_enabled = false;
        return;
    }

    if (sweep_shift != 0){
        apu->ch1_shadow_freq       = new_freq;
        s_reg[reg_idx(0xFF13)]     = new_freq & 0xFF;
        s_reg[reg_idx(0xFF14)]     = (s_reg[reg_idx(0xFF14)] & 0xF8) | ((new_freq >> 8) & 0x07);

        // Second overflow check (hardware quirk)
        u16 delta2 = new_freq >> sweep_shift;
        u16 check  = negate ? (new_freq - delta2) : (new_freq + delta2);
        if (check > 2047) apu->ch1_enabled = false;
    }
}

static void clock_envelope_channel(bool* running, u8* volume, u8* timer, u8 nrx2){
    u8 period = nrx2 & 0x07;
    if (period == 0 || !*running) return;

    if (*timer > 0) (*timer)--;
    if (*timer != 0) return;
    *timer = period;

    bool increase = (nrx2 & 0x08) != 0;
    if (increase){
        if (*volume < 15) (*volume)++;
        else *running = false;
    } else {
        if (*volume > 0) (*volume)--;
        else *running = false;
    }
}

static void clock_envelopes(APU* apu){
    clock_envelope_channel(&apu->ch1_envelope_running, &apu->ch1_volume, &apu->ch1_envelope_timer, s_reg[reg_idx(0xFF12)]);
    clock_envelope_channel(&apu->ch2_envelope_running, &apu->ch2_volume, &apu->ch2_envelope_timer, s_reg[reg_idx(0xFF17)]);
    clock_envelope_channel(&apu->ch4_envelope_running, &apu->ch4_volume, &apu->ch4_envelope_timer, s_reg[reg_idx(0xFF21)]);
}


// ---------- Per-channel steppers ----------

static void step_ch1(APU* apu, u32 t_cycles){
    if (!apu->ch1_dac_enabled) return;
    u32 remaining = t_cycles;
    while (remaining > 0){
        if (apu->ch1_freq_timer <= remaining){
            remaining -= apu->ch1_freq_timer;
            u16 freq = ((s_reg[reg_idx(0xFF14)] & 0x07) << 8) | s_reg[reg_idx(0xFF13)];
            apu->ch1_freq_timer = (2048 - freq) * 4;
            apu->ch1_duty_pos   = (apu->ch1_duty_pos + 1) & 0x07;
        } else {
            apu->ch1_freq_timer -= remaining;
            remaining = 0;
        }
    }
}

static void step_ch2(APU* apu, u32 t_cycles){
    if (!apu->ch2_dac_enabled) return;
    u32 remaining = t_cycles;
    while (remaining > 0){
        if (apu->ch2_freq_timer <= remaining){
            remaining -= apu->ch2_freq_timer;
            u16 freq = ((s_reg[reg_idx(0xFF19)] & 0x07) << 8) | s_reg[reg_idx(0xFF18)];
            apu->ch2_freq_timer = (2048 - freq) * 4;
            apu->ch2_duty_pos   = (apu->ch2_duty_pos + 1) & 0x07;
        } else {
            apu->ch2_freq_timer -= remaining;
            remaining = 0;
        }
    }
}

static void step_ch3(APU* apu, u32 t_cycles){
    if (!apu->ch3_dac_enabled) return;
    u32 remaining = t_cycles;
    while (remaining > 0){
        if (apu->ch3_freq_timer <= remaining){
            remaining -= apu->ch3_freq_timer;
            u16 freq = ((s_reg[reg_idx(0xFF1E)] & 0x07) << 8) | s_reg[reg_idx(0xFF1D)];
            apu->ch3_freq_timer   = (2048 - freq) * 2;
            apu->ch3_sample_index = (apu->ch3_sample_index + 1) & 0x1F;
            u8 byte = s_wave_ram[apu->ch3_sample_index / 2];
            apu->ch3_sample_buffer = (apu->ch3_sample_index & 1) ? (byte & 0x0F) : (byte >> 4);
        } else {
            apu->ch3_freq_timer -= remaining;
            remaining = 0;
        }
    }
}

static void step_ch4(APU* apu, u32 t_cycles){
    if (!apu->ch4_dac_enabled) return;
    u32 remaining = t_cycles;
    while (remaining > 0){
        if (apu->ch4_freq_timer <= remaining){
            remaining -= apu->ch4_freq_timer;
            u8 nr43         = s_reg[reg_idx(0xFF22)];
            u8 shift        = (nr43 >> 4) & 0x0F;
            u8 divisor_code = nr43 & 0x07;
            apu->ch4_freq_timer = ((u32)NOISE_DIVISORS[divisor_code]) << shift;

            u16 xor_bit  = (apu->ch4_lfsr & 0x01) ^ ((apu->ch4_lfsr >> 1) & 0x01);
            apu->ch4_lfsr = (apu->ch4_lfsr >> 1) | (xor_bit << 14);
            if (nr43 & 0x08){ // Narrow mode - also feed into bit 6
                apu->ch4_lfsr = (apu->ch4_lfsr & ~0x40) | (xor_bit << 6);
            }
        } else {
            apu->ch4_freq_timer -= remaining;
            remaining = 0;
        }
    }
}


// ---------- Mixing ----------

static void push_sample(APU* apu, s16 left, s16 right){
    u32 next_write = (apu->ring_write + 1) % APU_RING_CAPACITY;
    if (next_write == apu->ring_read){
        // Buffer full - drop oldest sample to make room
        apu->ring_read = (apu->ring_read + 1) % APU_RING_CAPACITY;
    }
    apu->ring[apu->ring_write * 2 + 0] = left;
    apu->ring[apu->ring_write * 2 + 1] = right;
    apu->ring_write = next_write;
}

static void mix_and_output(APU* apu){
    if (!apu->power_on){
        push_sample(apu, 0, 0);
        return;
    }

    // Raw DAC output per channel, range -8..+7
    s32 ch1 = 0, ch2 = 0, ch3 = 0, ch4 = 0;

    if (apu->ch1_enabled && apu->ch1_dac_enabled){
        u8 duty = (s_reg[reg_idx(0xFF11)] >> 6) & 0x03;
        u8 amp  = DUTY_TABLE[duty][apu->ch1_duty_pos] ? apu->ch1_volume : 0;
        ch1 = (s32)amp - 8;
    }
    if (apu->ch2_enabled && apu->ch2_dac_enabled){
        u8 duty = (s_reg[reg_idx(0xFF16)] >> 6) & 0x03;
        u8 amp  = DUTY_TABLE[duty][apu->ch2_duty_pos] ? apu->ch2_volume : 0;
        ch2 = (s32)amp - 8;
    }
    if (apu->ch3_enabled && apu->ch3_dac_enabled){
        u8 shift_code = (s_reg[reg_idx(0xFF1C)] >> 5) & 0x03;
        u8 sample = apu->ch3_sample_buffer;
        u8 amp;
        switch (shift_code){
            case 0:  amp = 0;           break; // Mute
            case 1:  amp = sample;      break; // 100%
            case 2:  amp = sample >> 1; break; // 50%
            case 3:  amp = sample >> 2; break; // 25%
            default: amp = 0;           break;
        }
        ch3 = (s32)amp - 8;
    }
    if (apu->ch4_enabled && apu->ch4_dac_enabled){
        u8 amp = (~apu->ch4_lfsr & 0x01) ? apu->ch4_volume : 0;
        ch4 = (s32)amp - 8;
    }

    // NR51 panning
    u8  nr51      = s_reg[reg_idx(0xFF25)];
    s32 left_mix  = 0;
    s32 right_mix = 0;
    if (nr51 & 0x10) left_mix  += ch1;
    if (nr51 & 0x20) left_mix  += ch2;
    if (nr51 & 0x40) left_mix  += ch3;
    if (nr51 & 0x80) left_mix  += ch4;
    if (nr51 & 0x01) right_mix += ch1;
    if (nr51 & 0x02) right_mix += ch2;
    if (nr51 & 0x04) right_mix += ch3;
    if (nr51 & 0x08) right_mix += ch4;

    // NR50 master volume (1-8 each side)
    u8 nr50      = s_reg[reg_idx(0xFF24)];
    u8 left_vol  = ((nr50 >> 4) & 0x07) + 1;
    u8 right_vol = (nr50 & 0x07) + 1;

    // Scale to s16 range with headroom for 4 summed channels
    const s32 SCALE = 110;
    s32 left_out  = left_mix  * left_vol  * SCALE;
    s32 right_out = right_mix * right_vol * SCALE;

    if (left_out  >  32767) left_out  =  32767;
    if (left_out  < -32768) left_out  = -32768;
    if (right_out >  32767) right_out =  32767;
    if (right_out < -32768) right_out = -32768;

    push_sample(apu, (s16)left_out, (s16)right_out);
}


// ---------- Top-level step ----------

#define GB_CLOCK_HZ      4194304u
void apu_step(APU* apu, u8 t_cycles){

    // When powered off, still emit silent samples so the audio clock doesn't starve
    if (!apu->power_on){
        apu->sample_div_counter += (u32)t_cycles * APU_SAMPLE_RATE;
        while (apu->sample_div_counter >= GB_CLOCK_HZ){
            apu->sample_div_counter -= GB_CLOCK_HZ;
            push_sample(apu, 0, 0);
        }
        return;
    }

    step_ch1(apu, t_cycles);
    step_ch2(apu, t_cycles);
    step_ch3(apu, t_cycles);
    step_ch4(apu, t_cycles);

    // Frame sequencer (512 Hz = every 8192 T-cycles)
    apu->frame_seq_div += t_cycles;
    while (apu->frame_seq_div >= 8192){
        apu->frame_seq_div -= 8192;
        switch (apu->frame_seq_step){
            case 0: clock_length(apu);                    break;
            case 1:                                        break;
            case 2: clock_length(apu); clock_sweep(apu);  break;
            case 3:                                        break;
            case 4: clock_length(apu);                    break;
            case 5:                                        break;
            case 6: clock_length(apu); clock_sweep(apu);  break;
            case 7: clock_envelopes(apu);                  break;
        }
        apu->frame_seq_step = (apu->frame_seq_step + 1) & 0x07;
    }

    // Output samples at APU_SAMPLE_RATE
    apu->sample_div_counter += (u32)t_cycles * APU_SAMPLE_RATE;
    while (apu->sample_div_counter >= GB_CLOCK_HZ){
        apu->sample_div_counter -= GB_CLOCK_HZ;
        mix_and_output(apu);
    }
}


// ---------- Ring buffer consumer API ----------

u32 apu_available_samples(const APU* apu){
    u32 w = apu->ring_write;
    u32 r = apu->ring_read;
    return (w >= r) ? (w - r) : (APU_RING_CAPACITY - r + w);
}

u32 apu_read_samples(APU* apu, s16* out, u32 max_pairs){
    u32 available = apu_available_samples(apu);
    u32 to_copy   = (max_pairs < available) ? max_pairs : available;

    for (u32 i = 0; i < to_copy; i++){
        u32 idx        = (apu->ring_read + i) % APU_RING_CAPACITY;
        out[i * 2 + 0] = apu->ring[idx * 2 + 0];
        out[i * 2 + 1] = apu->ring[idx * 2 + 1];
    }
    apu->ring_read = (apu->ring_read + to_copy) % APU_RING_CAPACITY;
    return to_copy;
}