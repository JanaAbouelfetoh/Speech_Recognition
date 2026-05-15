///**
// * features.c ? Memory-Optimized Real Feature Extraction
// */
//
//#include "features.h"
//#include "sram.h"
//#include "uart.h"
//#include "config.h"
//#include <math.h>
//#include <string.h>
//#include <avr/pgmspace.h>
//#include <stdint.h>
//
///* FFT buffers (512 bytes) */
//#define FFT_LEN 128
//static int16_t fft_re[FFT_LEN];
//static int16_t fft_im[FFT_LEN];
//
///* Hann window in flash */
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
///* Bit-reversal table in flash */
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
///* Twiddle factors in flash */
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
//    uint8_t half_size = 1;
//    for (uint8_t stage = 0; stage < 7; stage++) {
//        uint8_t step = half_size << 1;
//        for (uint8_t k = 0; k < half_size; k++) {
//            uint8_t tw_idx = (k * 128) / step;
//            int16_t wr = twiddle_cos(tw_idx);
//            int16_t wi = -twiddle_sin(tw_idx);
//            for (uint8_t i = k; i < FFT_LEN; i += step) {
//                uint8_t j = i + half_size;
//                int16_t tr = (int16_t)(((int32_t)wr * fft_re[j] - (int32_t)wi * fft_im[j]) >> 15);
//                int16_t ti = (int16_t)(((int32_t)wr * fft_im[j] + (int32_t)wi * fft_re[j]) >> 15);
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
//    for (uint8_t i = 0; i < FFT_LEN; i++) {
//        uint8_t raw = sram_read(sram_start + i);
//        int8_t sample = (int8_t)(raw - 128);
//        uint8_t hw = pgm_read_byte(&hann_table[i]);
//        int16_t windowed = ((int16_t)sample * hw) >> 7;
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
//void extract_features(float *feat) {
//    uint8_t i, k;
//    
//    /* First pass: find energy stats and locate speech region */
//    #define TEMP_FRAMES 50
//    float temp_energy[TEMP_FRAMES];
//    float temp_zcr[TEMP_FRAMES];
//    uint8_t n_frames = 0;
//    float max_e = 0;
//    
//    /* First pass: compute all frame energies and ZCR */
//    for (uint16_t start = SRAM_AUDIO_BASE; 
//         start + FEAT_FRAME_LEN <= SRAM_AUDIO_BASE + SRAM_AUDIO_LEN && n_frames < TEMP_FRAMES;
//         start += FEAT_HOP_LEN) {
//        
//        float energy = 0.0f;
//        uint16_t zcr = 0;
//        int8_t prev_sign = 0;
//        
//        for (i = 0; i < FEAT_FRAME_LEN; i++) {
//            uint8_t raw = sram_read(start + i);
//            float x = ((float)raw - 128.0f) / 127.0f;
//            energy += x * x;
//            
//            int8_t s = (x >= 0.0f) ? 1 : -1;
//            if (i > 0 && s != prev_sign) zcr++;
//            prev_sign = s;
//        }
//        
//        energy /= FEAT_FRAME_LEN;
//        temp_energy[n_frames] = energy;
//        temp_zcr[n_frames] = (float)zcr / FEAT_FRAME_LEN;
//        
//        if (energy > max_e) max_e = energy;
//        n_frames++;
//    }
//    
//    if (n_frames < 4) {
//        for (i = 0; i < N_FEATURES; i++) feat[i] = 0.0f;
//        return;
//    }
//    
//    /* Find active region (silence trim) */
//    float threshold = max_e * 0.02f;
//    uint8_t active_start = 0, active_end = n_frames - 1;
//    
//    for (i = 0; i < n_frames; i++) {
//        if (temp_energy[i] > threshold) {
//            active_start = (i >= 3) ? (i - 3) : 0;
//            break;
//        }
//    }
//    for (i = n_frames - 1; i > 0; i--) {
//        if (temp_energy[i] > threshold) {
//            active_end = (i + 3 < n_frames) ? (i + 3) : (n_frames - 1);
//            break;
//        }
//    }
//    
//    uint8_t nf = active_end - active_start + 1;
//    if (nf < 4) { active_start = 0; active_end = n_frames - 1; nf = n_frames; }
//    
//    /* Compute energy and ZCR statistics from active region */
//    float e_sum = 0, e_sum_sq = 0;
//    float z_sum = 0, z_sum_sq = 0;
//    
//    for (i = active_start; i <= active_end; i++) {
//        e_sum += temp_energy[i];
//        e_sum_sq += temp_energy[i] * temp_energy[i];
//        z_sum += temp_zcr[i];
//        z_sum_sq += temp_zcr[i] * temp_zcr[i];
//    }
//    
//    float e_mean = e_sum / nf;
//    float e_std = sqrtf((e_sum_sq / nf) - (e_mean * e_mean));
//    float z_mean = z_sum / nf;
//    float z_std = sqrtf((z_sum_sq / nf) - (z_mean * z_mean));
//    
//    /* Spectral features using FFT (process limited frames) */
//    float band_sum[4] = {0};
//    float centroid_sum = 0, flatness_sum = 0;
//    float bin_total[64] = {0};
//    uint8_t frames_processed = 0;
//    uint8_t max_fft_frames = (nf > 15) ? 15 : nf;
//    
//    uint16_t frame_start = SRAM_AUDIO_BASE + active_start * FEAT_HOP_LEN;
//    
//    for (uint8_t fi = 0; fi < max_fft_frames && frames_processed < 15; fi++, frame_start += FEAT_HOP_LEN) {
//        compute_frame_fft(frame_start);
//        
//        float band_e[4] = {0};
//        float total_fft_e = 0.0f;
//        
//        for (k = 0; k < 64; k++) {
//            float mag_sq = (float)get_mag_sq(k);
//            bin_total[k] += mag_sq;
//            total_fft_e += mag_sq;
//            
//            if (k < 16) band_e[0] += mag_sq;
//            else if (k < 32) band_e[1] += mag_sq;
//            else if (k < 48) band_e[2] += mag_sq;
//            else band_e[3] += mag_sq;
//        }
//        
//        float inv_total = 1.0f / (total_fft_e + 1e-8f);
//        band_sum[0] += band_e[0] * inv_total;
//        band_sum[1] += band_e[1] * inv_total;
//        band_sum[2] += band_e[2] * inv_total;
//        band_sum[3] += band_e[3] * inv_total;
//        
//        /* Spectral centroid */
//        float mag_sum = 0, weighted_sum = 0;
//        for (k = 0; k < 64; k++) {
//            float mag = sqrtf((float)get_mag_sq(k));
//            mag_sum += mag;
//            weighted_sum += (float)k * mag;
//        }
//        centroid_sum += (weighted_sum / (mag_sum + 1e-8f)) / 64.0f;
//        
//        /* Spectral flatness */
//        float log_sum = 0, arith_mean = 0;
//        for (k = 0; k < 64; k++) {
//            float mag = sqrtf((float)get_mag_sq(k)) + 1e-8f;
//            log_sum += logf(mag);
//            arith_mean += mag;
//        }
//        flatness_sum += expf(log_sum / 64.0f) / (arith_mean / 64.0f + 1e-8f);
//        
//        frames_processed++;
//    }
//    
//    float inv_frames = 1.0f / frames_processed;
//    float f4 = band_sum[0] * inv_frames;
//    float f5 = band_sum[1] * inv_frames;
//    float f6 = band_sum[2] * inv_frames;
//    float f7 = band_sum[3] * inv_frames;
//    float f8 = centroid_sum * inv_frames;
//    float f9 = flatness_sum * inv_frames;
//    
//    /* Spectral rolloff */
//    float total_energy = 0;
//    for (k = 0; k < 64; k++) total_energy += bin_total[k];
//    float cumsum = 0;
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
//    float total_e = 0;
//    for (i = active_start; i <= active_end; i++) total_e += temp_energy[i];
//    float f10 = 0;
//    if (total_e > 1e-8f) {
//        for (i = active_start; i <= active_end; i++) {
//            float p = temp_energy[i] / total_e;
//            if (p > 1e-8f) f10 -= p * logf(p);
//        }
//        f10 /= logf((float)nf);
//    }
//    
//    /* Band ratio */
//    float f11 = (f4 + f5 + 1e-8f) / (f6 + f7 + 1e-8f);
//    
//    /* Peak position */
//    uint8_t peak_idx = active_start;
//    for (i = active_start + 1; i <= active_end; i++)
//        if (temp_energy[i] > temp_energy[peak_idx]) peak_idx = i;
//    float f12 = (float)(peak_idx - active_start) / (float)(nf - 1);
//    
//    /* Attack slope */
//    uint8_t quarter = (nf / 4 < 2) ? 2 : nf / 4;
//    float sx = 0, sy = 0, sxx = 0, sxy = 0;
//    for (i = 0; i < quarter; i++) {
//        float e_val = temp_energy[active_start + i];
//        sx += i;
//        sy += e_val;
//        sxx += i * i;
//        sxy += i * e_val;
//    }
//    float denom = quarter * sxx - sx * sx;
//    float slope = (fabsf(denom) > 1e-10f) ? ((quarter * sxy - sx * sy) / denom) : 0.0f;
//    float f13 = slope / (e_mean + 1e-8f);
//    
//    /* ZCR-energy correlation */
//    float cov = 0;
//    for (i = active_start; i <= active_end; i++) {
//        float e_dev = temp_energy[i] - e_mean;
//        float z_dev = temp_zcr[i] - z_mean;
//        cov += e_dev * z_dev;
//    }
//    cov /= nf;
//    float f14 = (e_std > 1e-8f && z_std > 1e-8f) ? cov / (e_std * z_std) : 0.0f;
//    
//    /* Output all 16 features */
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
/**
 * features.c - Real Feature Extraction with Software Gain
 * =========================================================
 *
 * CHANGE FROM OLD VERSION:
 * -------------------------
 * Added software gain to compensate for weak microphone signal.
 * Your ADC reads 60-64 (range=4, centered at ~62).
 * Without gain: x = (62-128)/127 = -0.52, all frames look identical.
 * With gain:    x = (62-62)*8/127 = 0, signal deviations amplified 8x.
 *
 * All other logic is identical to your working features.c.
 * The gain is applied only at the point where raw ADC ? float sample.
 * FFT, band energies, ZCR, entropy ? all unchanged.
 *
 * Set ADC_DC_OFFSET to the center value you see in SRAM verification.
 * Set ADC_SOFT_GAIN to amplify. 8x is a good start.
 * After building the preamp circuit, set both back to defaults (128, 1).
 */

#include "features.h"
#include "sram.h"
#include "uart.h"
#include "config.h"
#include <math.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <stdint.h>

/* ?? Software gain for weak microphone ??
 * Measure the average SRAM value during silence and set DC_OFFSET to it.
 * SOFT_GAIN amplifies deviations from that center.
 * Remove these (set offset=128, gain=1) once preamp is built.          */
#define ADC_DC_OFFSET   62      /* measured center: your signal ? 62    */
#define ADC_SOFT_GAIN    8      /* amplify deviations 8x                */

/* Convert raw ADC byte ? float [-1, 1] with software gain */
static inline float adc_to_float(uint8_t raw)
{
    int16_t centered = (int16_t)raw - ADC_DC_OFFSET;
    int16_t amplified = centered * ADC_SOFT_GAIN;
    /* Clamp to int8 range [-127, 127] */
    if (amplified >  127) amplified =  127;
    if (amplified < -127) amplified = -127;
    return (float)amplified / 127.0f;
}

/* ?? FFT buffers (512 bytes, static = BSS, no stack cost) ?? */
#define FFT_LEN 128
static int16_t fft_re[FFT_LEN];
static int16_t fft_im[FFT_LEN];

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

static const int16_t cos_table[32] PROGMEM = {
    32767,32610,32138,31357,30274,28899,27246,25330,
    23170,20788,18205,15447,12540,9512,6393,3212,
    0,-3212,-6393,-9512,-12540,-15447,-18205,-20788,
    -23170,-25330,-27246,-28899,-30274,-31357,-32138,-32610
};

static inline int16_t twiddle_cos(uint8_t k)
{
    k &= 0x7F;
    if (k <  32) return  pgm_read_word(&cos_table[k]);
    if (k <  64) return -pgm_read_word(&cos_table[64 - k]);
    if (k <  96) return -pgm_read_word(&cos_table[k - 64]);
    return pgm_read_word(&cos_table[128 - k]);
}

static inline int16_t twiddle_sin(uint8_t k)
{
    return twiddle_cos((k + 32) & 0x7F);
}

static void fft_128(void)
{
    uint8_t half_size = 1;
    for (uint8_t stage = 0; stage < 7; stage++)
    {
        uint8_t step = half_size << 1;
        for (uint8_t k = 0; k < half_size; k++)
        {
            uint8_t tw_idx = (k * 128) / step;
            int16_t wr =  twiddle_cos(tw_idx);
            int16_t wi = -twiddle_sin(tw_idx);
            for (uint8_t i = k; i < FFT_LEN; i += step)
            {
                uint8_t j  = i + half_size;
                int16_t tr = (int16_t)(((int32_t)wr * fft_re[j]
                                      - (int32_t)wi * fft_im[j]) >> 15);
                int16_t ti = (int16_t)(((int32_t)wr * fft_im[j]
                                      + (int32_t)wi * fft_re[j]) >> 15);
                fft_re[j] = fft_re[i] - tr;
                fft_im[j] = fft_im[i] - ti;
                fft_re[i] = fft_re[i] + tr;
                fft_im[i] = fft_im[i] + ti;
            }
        }
        half_size = step;
    }
}

/* ?? compute_frame_fft: uses adc_to_float() for gain ?? */
static void compute_frame_fft(uint16_t sram_start)
{
    for (uint8_t i = 0; i < FFT_LEN; i++)
    {
        uint8_t raw      = sram_read(sram_start + i);
        float   x        = adc_to_float(raw);           /* ? gain here */
        int16_t sample   = (int16_t)(x * 127.0f);
        uint8_t hw       = pgm_read_byte(&hann_table[i]);
        int16_t windowed = (int16_t)((sample * (int16_t)hw) >> 7);
        uint8_t br       = pgm_read_byte(&bit_rev[i]);
        fft_re[br] = windowed;
        fft_im[br] = 0;
    }
    fft_128();
}

static inline uint32_t get_mag_sq(uint8_t bin)
{
    int32_t r  = fft_re[bin];
    int32_t im = fft_im[bin];
    return (uint32_t)(r * r + im * im);
}

/* ??????????????????????????????????????????????????????
 * extract_features()
 * All logic identical to your working version.
 * Only change: raw ADC ? float uses adc_to_float() above
 * which applies DC offset correction and soft gain.
 * ?????????????????????????????????????????????????????? */
void extract_features(float *feat)
{
    uint8_t i, k;

#define TEMP_FRAMES 50
    float temp_energy[TEMP_FRAMES];
    float temp_zcr[TEMP_FRAMES];
    uint8_t n_frames = 0;
    float max_e = 0;

    /* First pass: frame energy + ZCR */
    for (uint16_t start = SRAM_AUDIO_BASE;
         start + FEAT_FRAME_LEN <= SRAM_AUDIO_BASE + SRAM_AUDIO_LEN
         && n_frames < TEMP_FRAMES;
         start += FEAT_HOP_LEN)
    {
        float energy = 0.0f;
        uint16_t zcr = 0;
        int8_t prev_sign = 0;

        for (i = 0; i < FEAT_FRAME_LEN; i++)
        {
            uint8_t raw = sram_read(start + i);
            float x = adc_to_float(raw);              /* ? gain here  */
            energy += x * x;

            int8_t s = (x >= 0.0f) ? 1 : -1;
            if (i > 0 && s != prev_sign) zcr++;
            prev_sign = s;
        }

        energy /= FEAT_FRAME_LEN;
        temp_energy[n_frames] = energy;
        temp_zcr[n_frames]    = (float)zcr / FEAT_FRAME_LEN;
        if (energy > max_e) max_e = energy;
        n_frames++;
    }

    if (n_frames < 4)
    {
        for (i = 0; i < N_FEATURES; i++) feat[i] = 0.0f;
        return;
    }

    /* Silence trim */
    float threshold  = max_e * 0.02f;
    uint8_t active_start = 0, active_end = n_frames - 1;

    for (i = 0; i < n_frames; i++)
        if (temp_energy[i] > threshold)
        { active_start = (i >= 3) ? (i - 3) : 0; break; }

    for (i = n_frames - 1; i > 0; i--)
        if (temp_energy[i] > threshold)
        { active_end = (i + 3 < n_frames) ? (i + 3) : (n_frames - 1); break; }

    uint8_t nf = active_end - active_start + 1;
    if (nf < 4) { active_start = 0; active_end = n_frames - 1; nf = n_frames; }

    /* Energy + ZCR stats */
    float e_sum = 0, e_sum_sq = 0, z_sum = 0, z_sum_sq = 0;
    for (i = active_start; i <= active_end; i++)
    {
        e_sum    += temp_energy[i];
        e_sum_sq += temp_energy[i] * temp_energy[i];
        z_sum    += temp_zcr[i];
        z_sum_sq += temp_zcr[i] * temp_zcr[i];
    }
    float e_mean = e_sum / nf;
    float e_std  = sqrtf((e_sum_sq / nf) - (e_mean * e_mean));
    float z_mean = z_sum / nf;
    float z_std  = sqrtf((z_sum_sq / nf) - (z_mean * z_mean));

    /* Spectral features */
    float band_sum[4]  = {0, 0, 0, 0};
    float centroid_sum = 0, flatness_sum = 0;
    float bin_total[64];
    memset(bin_total, 0, sizeof(bin_total));

    uint8_t max_fft_frames  = (nf > 15) ? 15 : nf;
    uint8_t frames_processed = 0;
    uint16_t frame_start = SRAM_AUDIO_BASE + active_start * FEAT_HOP_LEN;

    for (uint8_t fi = 0; fi < max_fft_frames; fi++, frame_start += FEAT_HOP_LEN)
    {
        compute_frame_fft(frame_start);

        float band_e[4]  = {0, 0, 0, 0};
        float total_fft_e = 0;

        for (k = 0; k < 64; k++)
        {
            float mag_sq = (float)get_mag_sq(k);
            bin_total[k]  += mag_sq;
            total_fft_e   += mag_sq;
            if      (k < 16) band_e[0] += mag_sq;
            else if (k < 32) band_e[1] += mag_sq;
            else if (k < 48) band_e[2] += mag_sq;
            else             band_e[3] += mag_sq;
        }

        float inv = 1.0f / (total_fft_e + 1e-8f);
        band_sum[0] += band_e[0] * inv;
        band_sum[1] += band_e[1] * inv;
        band_sum[2] += band_e[2] * inv;
        band_sum[3] += band_e[3] * inv;

        float mag_sum = 0, weighted_sum = 0;
        for (k = 0; k < 64; k++)
        {
            float mag = sqrtf((float)get_mag_sq(k));
            mag_sum      += mag;
            weighted_sum += (float)k * mag;
        }
        centroid_sum += (weighted_sum / (mag_sum + 1e-8f)) / 64.0f;

        float log_sum = 0, arith_mean = 0;
        for (k = 0; k < 64; k++)
        {
            float mag = sqrtf((float)get_mag_sq(k)) + 1e-8f;
            log_sum    += logf(mag);
            arith_mean += mag;
        }
        flatness_sum += expf(log_sum / 64.0f) / (arith_mean / 64.0f + 1e-8f);

        frames_processed++;
    }

    float inv_f = 1.0f / (float)frames_processed;
    float f4 = band_sum[0] * inv_f;
    float f5 = band_sum[1] * inv_f;
    float f6 = band_sum[2] * inv_f;
    float f7 = band_sum[3] * inv_f;
    float f8 = centroid_sum * inv_f;
    float f9 = flatness_sum * inv_f;

    /* Rolloff */
    float total_energy = 0;
    for (k = 0; k < 64; k++) total_energy += bin_total[k];
    float cumsum = 0;
    uint8_t rolloff_bin = 63;
    for (k = 0; k < 64; k++)
    {
        cumsum += bin_total[k];
        if (cumsum >= 0.85f * total_energy) { rolloff_bin = k; break; }
    }
    float f15 = (float)rolloff_bin / 64.0f;

    /* Energy entropy */
    float total_e = 0;
    for (i = active_start; i <= active_end; i++) total_e += temp_energy[i];
    float f10 = 0;
    if (total_e > 1e-8f)
    {
        for (i = active_start; i <= active_end; i++)
        {
            float p = temp_energy[i] / total_e;
            if (p > 1e-8f) f10 -= p * logf(p);
        }
        f10 /= logf((float)nf);
    }

    float f11 = (f4 + f5 + 1e-8f) / (f6 + f7 + 1e-8f);

    uint8_t peak_idx = active_start;
    for (i = active_start + 1; i <= active_end; i++)
        if (temp_energy[i] > temp_energy[peak_idx]) peak_idx = i;
    float f12 = (float)(peak_idx - active_start) / (float)(nf > 1 ? nf - 1 : 1);

    uint8_t quarter = (nf / 4 < 2) ? 2 : nf / 4;
    float sx = 0, sy = 0, sxx = 0, sxy = 0;
    for (i = 0; i < quarter; i++)
    {
        float ev = temp_energy[active_start + i];
        sx  += i;   sy  += ev;
        sxx += (float)i * i;   sxy += (float)i * ev;
    }
    float denom = (float)quarter * sxx - sx * sx;
    float slope = (fabsf(denom) > 1e-10f)
                ? ((float)quarter * sxy - sx * sy) / denom : 0.0f;
    float f13 = slope / (e_mean + 1e-8f);

    float cov = 0;
    for (i = active_start; i <= active_end; i++)
        cov += (temp_energy[i] - e_mean) * (temp_zcr[i] - z_mean);
    cov /= nf;
    float f14 = (e_std > 1e-8f && z_std > 1e-8f)
              ? cov / (e_std * z_std) : 0.0f;

    feat[0]  = e_mean;  feat[1]  = e_std;
    feat[2]  = z_mean;  feat[3]  = z_std;
    feat[4]  = f4;      feat[5]  = f5;
    feat[6]  = f6;      feat[7]  = f7;
    feat[8]  = f8;      feat[9]  = f9;
    feat[10] = f10;     feat[11] = f11;
    feat[12] = f12;     feat[13] = f13;
    feat[14] = f14;     feat[15] = f15;
}