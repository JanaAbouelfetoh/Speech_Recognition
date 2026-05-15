//#include "features.h"
//#include "sram.h"
//#include "uart.h"
//#include "config.h"
//#include <math.h>
//#include <string.h>
//#include <avr/pgmspace.h>
//#include <stdint.h>
//
///* ============================================
//   FFT Implementation (128-point, fixed-point)
//   ============================================ */
//
//#define FFT_LEN 128
//
//static int16_t fft_re[FFT_LEN];
//static int16_t fft_im[FFT_LEN];
//
///* Hann window (128 points, Q8 format: 0-255 represents 0.0-1.0) */
//static const uint8_t hann_table[FFT_LEN] PROGMEM = {
//    0,0,1,2,4,6,9,12,16,20,24,29,34,40,45,51,
//    58,64,71,78,85,92,99,107,114,121,129,136,143,150,158,165,
//    172,179,185,192,198,204,210,216,221,226,231,235,239,243,246,249,
//    251,253,254,255,255,255,254,253,251,249,246,243,239,235,231,226,
//    221,216,210,204,198,192,185,179,172,165,158,150,143,136,129,121,
//    114,107,99,92,85,78,71,64,58,51,45,40,34,29,24,20,
//    16,12,9,6,4,2,1,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//};
//
///* Bit-reversal table for N=128 */
//static const uint8_t bit_rev[128] PROGMEM = {
//    0,64,32,96,16,80,48,112,8,72,40,104,24,88,56,120,
//    4,68,36,100,20,84,52,116,12,76,44,108,28,92,60,124,
//    2,66,34,98,18,82,50,114,10,74,42,106,26,90,58,122,
//    6,70,38,102,22,86,54,118,14,78,46,110,30,94,62,126,
//    1,65,33,97,17,81,49,113,9,73,41,105,25,89,57,121,
//    5,69,37,101,21,85,53,117,13,77,45,109,29,93,61,125,
//    3,67,35,99,19,83,51,115,11,75,43,107,27,91,59,123,
//    7,71,39,103,23,87,55,119,15,79,47,111,31,95,63,127
//};
//
///* Twiddle factors (cos/sin in Q15 format) */
//static const int16_t cos_table[32] PROGMEM = {
//    32767, 32610, 32138, 31357, 30274, 28899, 27246, 25330,
//    23170, 20788, 18205, 15447, 12540, 9512, 6393, 3212,
//    0, -3212, -6393, -9512, -12540, -15447, -18205, -20788,
//    -23170, -25330, -27246, -28899, -30274, -31357, -32138, -32610
//};
//
//static inline int16_t twiddle_cos(uint8_t k) {
//    k &= 0x7F;
//    if (k < 32) return pgm_read_word(&cos_table[k]);
//    if (k < 64) return -pgm_read_word(&cos_table[64 - k]);
//    if (k < 96) return -pgm_read_word(&cos_table[k - 64]);
//    return pgm_read_word(&cos_table[128 - k]);
//}
//
//static inline int16_t twiddle_sin(uint8_t k) {
//    return twiddle_cos((k + 32) & 0x7F);
//}
//
//static void fft_128(void) {
//    uint8_t stage, half_size, step, k, i, j;
//    half_size = 1;
//    
//    for (stage = 0; stage < 7; stage++) {
//        step = half_size << 1;
//        
//        for (k = 0; k < half_size; k++) {
//            uint8_t tw_idx = (k * 128) / step;
//            int16_t wr = twiddle_cos(tw_idx);
//            int16_t wi = -twiddle_sin(tw_idx);
//            
//            for (i = k; i < FFT_LEN; i += step) {
//                j = i + half_size;
//                
//                int16_t tr = (int16_t)(((int32_t)wr * fft_re[j] - (int32_t)wi * fft_im[j]) >> 15);
//                int16_t ti = (int16_t)(((int32_t)wr * fft_im[j] + (int32_t)wi * fft_re[j]) >> 15);
//                
//                fft_re[j] = fft_re[i] - tr;
//                fft_im[j] = fft_im[i] - ti;
//                fft_re[i] = fft_re[i] + tr;
//                fft_im[i] = fft_im[i] + ti;
//            }
//        }
//        half_size = step;
//    }
//}
//
//static void compute_frame_fft(uint16_t sram_start) {
//    uint8_t i;
//    for (i = 0; i < FFT_LEN; i++) {
//        uint8_t raw = sram_read(sram_start + i);
//        int8_t sample = (int8_t)(raw - 128);
//        uint8_t hw = pgm_read_byte(&hann_table[i]);
//        int16_t windowed = ((int16_t)sample * hw) >> 7;
//        
//        uint8_t br = pgm_read_byte(&bit_rev[i]);
//        fft_re[br] = windowed;
//        fft_im[br] = 0;
//    }
//    fft_128();
//}
//
//static inline uint32_t get_mag_sq(uint8_t bin) {
//    int32_t r = fft_re[bin];
//    int32_t im = fft_im[bin];
//    return (uint32_t)(r * r + im * im);
//}
//
///* ============================================
//   Feature Extraction - MEMORY OPTIMIZED
//   ============================================ */
//
//void extract_features(float *feat) {
//    uint8_t i, k, fi;
//    
//    /* Process frames in chunks to save memory */
//    #define MAX_FRAMES 60  /* Reduced from 124 to save memory */
//    static float frame_energy[MAX_FRAMES];
//    static float frame_zcr[MAX_FRAMES];
//    uint8_t n_frames = 0;
//    
//    /* Step 1: Compute energy and ZCR for all frames */
//    uint16_t start_addr = SRAM_AUDIO_BASE;
//    while (start_addr + FEAT_FRAME_LEN <= SRAM_AUDIO_BASE + SRAM_AUDIO_LEN && n_frames < MAX_FRAMES) {
//        
//        float energy = 0.0f;
//        uint16_t zcr = 0;
//        int8_t prev_sign = 0;
//        
//        for (i = 0; i < FEAT_FRAME_LEN; i++) {
//            uint8_t raw = sram_read(start_addr + i);
//            float x = ((float)raw - 128.0f) / 127.0f;
//            energy += x * x;
//            
//            int8_t s = (x >= 0.0f) ? 1 : -1;
//            if (i > 0 && s != prev_sign) zcr++;
//            prev_sign = s;
//        }
//        
//        frame_energy[n_frames] = energy / FEAT_FRAME_LEN;
//        frame_zcr[n_frames] = (float)zcr / FEAT_FRAME_LEN;
//        n_frames++;
//        start_addr += FEAT_HOP_LEN;
//    }
//    
//    if (n_frames == 0) {
//        for (i = 0; i < N_FEATURES; i++) feat[i] = 0.0f;
//        return;
//    }
//    
//    /* Step 2: Find active region (trim silence) */
//    float max_e = 0.0f;
//    for (i = 0; i < n_frames; i++)
//        if (frame_energy[i] > max_e) max_e = frame_energy[i];
//    
//    float threshold = max_e * 0.02f;
//    uint8_t active_start = 0, active_end = n_frames - 1;
//    
//    for (i = 0; i < n_frames; i++) {
//        if (frame_energy[i] > threshold) {
//            active_start = (i >= 2) ? (i - 2) : 0;
//            break;
//        }
//    }
//    for (i = n_frames - 1; i > 0; i--) {
//        if (frame_energy[i] > threshold) {
//            active_end = (i + 2 < n_frames) ? (i + 2) : (n_frames - 1);
//            break;
//        }
//    }
//    
//    uint8_t nf = active_end - active_start + 1;
//    float *ae = frame_energy + active_start;
//    float *az = frame_zcr + active_start;
//    
//    /* Step 3: Energy and ZCR statistics */
//    float e_mean = 0.0f, z_mean = 0.0f;
//    for (i = 0; i < nf; i++) {
//        e_mean += ae[i];
//        z_mean += az[i];
//    }
//    e_mean /= nf;
//    z_mean /= nf;
//    
//    float e_std = 0.0f, z_std = 0.0f;
//    for (i = 0; i < nf; i++) {
//        float de = ae[i] - e_mean;
//        float dz = az[i] - z_mean;
//        e_std += de * de;
//        z_std += dz * dz;
//    }
//    e_std = sqrtf(e_std / nf);
//    z_std = sqrtf(z_std / nf);
//    
//    /* Step 4: Spectral features via FFT - accumulate on the fly */
//    float band_sum[4] = {0};
//    float centroid_sum = 0.0f;
//    float flatness_sum = 0.0f;
//    float bin_total[64] = {0};  /* On stack, OK for short time */
//    
//    uint16_t frame_start = SRAM_AUDIO_BASE + active_start * FEAT_HOP_LEN;
//    
//    for (fi = 0; fi < nf && fi < 30; fi++, frame_start += FEAT_HOP_LEN) {
//        compute_frame_fft(frame_start);
//        
//        float total_fft_e = 0.0f;
//        float band_e[4] = {0};
//        
//        for (k = 0; k < 64; k++) {
//            float mag_sq = (float)get_mag_sq(k);
//            bin_total[k] += mag_sq;
//            
//            if (k < 16) band_e[0] += mag_sq;
//            else if (k < 32) band_e[1] += mag_sq;
//            else if (k < 48) band_e[2] += mag_sq;
//            else band_e[3] += mag_sq;
//            
//            total_fft_e += mag_sq;
//        }
//        
//        float inv_total = 1.0f / (total_fft_e + 1e-8f);
//        band_sum[0] += band_e[0] * inv_total;
//        band_sum[1] += band_e[1] * inv_total;
//        band_sum[2] += band_e[2] * inv_total;
//        band_sum[3] += band_e[3] * inv_total;
//        
//        /* Spectral centroid */
//        float mag_sum = 0.0f, weighted_sum = 0.0f;
//        for (k = 0; k < 64; k++) {
//            float mag = sqrtf((float)get_mag_sq(k));
//            mag_sum += mag;
//            weighted_sum += (float)k * mag;
//        }
//        centroid_sum += (weighted_sum / (mag_sum + 1e-8f)) / 64.0f;
//        
//        /* Spectral flatness */
//        float log_sum = 0.0f, arith_mean = 0.0f;
//        for (k = 0; k < 64; k++) {
//            float mag = sqrtf((float)get_mag_sq(k)) + 1e-8f;
//            log_sum += logf(mag);
//            arith_mean += mag;
//        }
//        float geom_mean = expf(log_sum / 64.0f);
//        flatness_sum += geom_mean / (arith_mean / 64.0f + 1e-8f);
//    }
//    
//    uint8_t actual_frames = (fi > 0) ? fi : 1;
//    float inv_nf = 1.0f / (float)actual_frames;
//    float f4 = band_sum[0] * inv_nf;
//    float f5 = band_sum[1] * inv_nf;
//    float f6 = band_sum[2] * inv_nf;
//    float f7 = band_sum[3] * inv_nf;
//    float f8 = centroid_sum * inv_nf;
//    float f9 = flatness_sum * inv_nf;
//    
//    /* Spectral rolloff */
//    float total_energy = 0.0f;
//    for (k = 0; k < 64; k++) total_energy += bin_total[k];
//    float cumsum = 0.0f;
//    uint8_t rolloff_bin = 63;
//    for (k = 0; k < 64; k++) {
//        cumsum += bin_total[k];
//        if (cumsum >= 0.85f * total_energy) {
//            rolloff_bin = k;
//            break;
//        }
//    }
//    float f15 = (float)rolloff_bin / 64.0f;
//    
//    /* Energy entropy */
//    float total_e = 0.0f;
//    for (i = 0; i < nf; i++) total_e += ae[i];
//    float f10 = 0.0f;
//    if (total_e > 1e-8f) {
//        for (i = 0; i < nf; i++) {
//            float p = ae[i] / total_e;
//            if (p > 1e-8f) f10 -= p * logf(p);
//        }
//        f10 /= logf((float)nf);
//    }
//    
//    /* Band ratio */
//    float f11 = (f4 + f5 + 1e-8f) / (f6 + f7 + 1e-8f);
//    
//    /* Peak position */
//    uint8_t peak_idx = 0;
//    for (i = 1; i < nf; i++)
//        if (ae[i] > ae[peak_idx]) peak_idx = i;
//    float f12 = (float)peak_idx / (float)(nf - 1);
//    
//    /* Attack slope */
//    uint8_t quarter = (nf / 4 < 2) ? 2 : nf / 4;
//    float sx = 0, sy = 0, sxx = 0, sxy = 0;
//    for (i = 0; i < quarter; i++) {
//        sx += i;
//        sy += ae[i];
//        sxx += i * i;
//        sxy += i * ae[i];
//    }
//    float denom = quarter * sxx - sx * sx;
//    float slope = (fabsf(denom) > 1e-10f) ? ((quarter * sxy - sx * sy) / denom) : 0.0f;
//    float f13 = slope / (e_mean + 1e-8f);
//    
//    /* ZCR-energy correlation */
//    float cov = 0.0f;
//    for (i = 0; i < nf; i++)
//        cov += (ae[i] - e_mean) * (az[i] - z_mean);
//    cov /= nf;
//    float f14 = (e_std > 1e-8f && z_std > 1e-8f) ? cov / (e_std * z_std) : 0.0f;
//    
//    /* Output features (16 total) */
//    feat[0] = e_mean;
//    feat[1] = e_std;
//    feat[2] = z_mean;
//    feat[3] = z_std;
//    feat[4] = f4;
//    feat[5] = f5;
//    feat[6] = f6;
//    feat[7] = f7;
//    feat[8] = f8;
//    feat[9] = f9;
//    feat[10] = f10;
//    feat[11] = f11;
//    feat[12] = f12;
//    feat[13] = f13;
//    feat[14] = f14;
//    feat[15] = f15;
//}
#include "features.h"
#include "sram.h"
#include "uart.h"
#include "config.h"
#include <math.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <stdint.h>

/* FFT buffers (512 bytes total) */
#define FFT_LEN 128
static int16_t fft_re[FFT_LEN];
static int16_t fft_im[FFT_LEN];

/* Hann window in flash */
static const uint8_t hann_table[FFT_LEN] PROGMEM = {
    0,0,1,2,4,6,9,12,16,20,24,29,34,40,45,51,
    58,64,71,78,85,92,99,107,114,121,129,136,143,150,158,165,
    172,179,185,192,198,204,210,216,221,226,231,235,239,243,246,249,
    251,253,254,255,255,255,254,253,251,249,246,243,239,235,231,226,
    221,216,210,204,198,192,185,179,172,165,158,150,143,136,129,121,
    114,107,99,92,85,78,71,64,58,51,45,40,34,29,24,20,
    16,12,9,6,4,2,1,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/* Bit-reversal table in flash */
static const uint8_t bit_rev[128] PROGMEM = {
    0,64,32,96,16,80,48,112,8,72,40,104,24,88,56,120,
    4,68,36,100,20,84,52,116,12,76,44,108,28,92,60,124,
    2,66,34,98,18,82,50,114,10,74,42,106,26,90,58,122,
    6,70,38,102,22,86,54,118,14,78,46,110,30,94,62,126,
    1,65,33,97,17,81,49,113,9,73,41,105,25,89,57,121,
    5,69,37,101,21,85,53,117,13,77,45,109,29,93,61,125,
    3,67,35,99,19,83,51,115,11,75,43,107,27,91,59,123,
    7,71,39,103,23,87,55,119,15,79,47,111,31,95,63,127
};

/* Twiddle factors in flash */
static const int16_t cos_table[32] PROGMEM = {
    32767, 32610, 32138, 31357, 30274, 28899, 27246, 25330,
    23170, 20788, 18205, 15447, 12540, 9512, 6393, 3212,
    0, -3212, -6393, -9512, -12540, -15447, -18205, -20788,
    -23170, -25330, -27246, -28899, -30274, -31357, -32138, -32610
};

static inline int16_t twiddle_cos(uint8_t k) {
    k &= 0x7F;
    if (k < 32) return pgm_read_word(&cos_table[k]);
    if (k < 64) return -pgm_read_word(&cos_table[64 - k]);
    if (k < 96) return -pgm_read_word(&cos_table[k - 64]);
    return pgm_read_word(&cos_table[128 - k]);
}

static inline int16_t twiddle_sin(uint8_t k) {
    return twiddle_cos((k + 32) & 0x7F);
}

static void fft_128(void) {
    uint8_t half_size = 1;
    for (uint8_t stage = 0; stage < 7; stage++) {
        uint8_t step = half_size << 1;
        for (uint8_t k = 0; k < half_size; k++) {
            uint8_t tw_idx = (k * 128) / step;
            int16_t wr = twiddle_cos(tw_idx);
            int16_t wi = -twiddle_sin(tw_idx);
            for (uint8_t i = k; i < FFT_LEN; i += step) {
                uint8_t j = i + half_size;
                int16_t tr = (int16_t)(((int32_t)wr * fft_re[j] - (int32_t)wi * fft_im[j]) >> 15);
                int16_t ti = (int16_t)(((int32_t)wr * fft_im[j] + (int32_t)wi * fft_re[j]) >> 15);
                fft_re[j] = fft_re[i] - tr;
                fft_im[j] = fft_im[i] - ti;
                fft_re[i] = fft_re[i] + tr;
                fft_im[i] = fft_im[i] + ti;
            }
        }
        half_size = step;
    }
}

static void compute_frame_fft(uint16_t sram_start) {
    for (uint8_t i = 0; i < FFT_LEN; i++) {
        uint8_t raw = sram_read(sram_start + i);
        int8_t sample = (int8_t)(raw - 128);
        uint8_t hw = pgm_read_byte(&hann_table[i]);
        int16_t windowed = ((int16_t)sample * hw) >> 7;
        uint8_t br = pgm_read_byte(&bit_rev[i]);
        fft_re[br] = windowed;
        fft_im[br] = 0;
    }
    fft_128();
}

static inline uint32_t get_mag_sq(uint8_t bin) {
    int32_t r = fft_re[bin];
    int32_t im = fft_im[bin];
    return (uint32_t)(r * r + im * im);
}

/* Memory-optimized feature extraction - no large static arrays */
void extract_features(float *feat) {
    uint8_t n_frames = 0;
    float e_sum = 0, e_sum_sq = 0;
    float z_sum = 0, z_sum_sq = 0;
    
    /* First pass: collect stats without storing all frames */
    uint16_t start_addr = SRAM_AUDIO_BASE;
    uint8_t valid_frames = 0;
    float max_e = 0;
    
    /* Find max energy and collect basic stats */
    while (start_addr + FEAT_FRAME_LEN <= SRAM_AUDIO_BASE + SRAM_AUDIO_LEN && valid_frames < 60) {
        float energy = 0;
        for (uint8_t i = 0; i < FEAT_FRAME_LEN; i++) {
            uint8_t raw = sram_read(start_addr + i);
            float x = ((float)raw - 128.0f) / 127.0f;
            energy += x * x;
        }
        energy /= FEAT_FRAME_LEN;
        if (energy > max_e) max_e = energy;
        e_sum += energy;
        e_sum_sq += energy * energy;
        
        start_addr += FEAT_HOP_LEN;
        valid_frames++;
    }
    
    if (valid_frames == 0) {
        for (uint8_t i = 0; i < N_FEATURES; i++) feat[i] = 0;
        return;
    }
    
    float e_mean = e_sum / valid_frames;
    float e_std = sqrtf((e_sum_sq / valid_frames) - (e_mean * e_mean));
    
    /* Simplified features for now - just energy stats */
    feat[0] = e_mean;
    feat[1] = e_std;
    feat[2] = 0.05f;  /* Placeholder ZCR mean */
    feat[3] = 0.03f;  /* Placeholder ZCR std */
    feat[4] = 0.25f;  /* Placeholder band energies */
    feat[5] = 0.25f;
    feat[6] = 0.25f;
    feat[7] = 0.25f;
    feat[8] = 0.5f;   /* Placeholder centroid */
    feat[9] = 0.5f;   /* Placeholder flatness */
    feat[10] = 0.5f;  /* Placeholder entropy */
    feat[11] = 1.0f;  /* Placeholder band ratio */
    feat[12] = 0.5f;  /* Placeholder peak pos */
    feat[13] = 0.1f;  /* Placeholder attack slope */
    feat[14] = 0.0f;  /* Placeholder correlation */
    feat[15] = 0.5f;  /* Placeholder rolloff */
}