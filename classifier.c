#include "classifier.h"
#include "word_templates.h"

#include <avr/pgmspace.h>
#include <math.h>

uint8_t classify(const float feat[N_FEATURES])
{
    float norm_feat[N_FEATURES];

    // ============================================
    // NORMALIZE FEATURES
    // (feat - mean) / std
    // ============================================

    for (uint8_t i = 0; i < N_FEATURES; i++)
    {
        float mean = pgm_read_float(&feature_mean[i]);
        float std  = pgm_read_float(&feature_std[i]);

        norm_feat[i] = (feat[i] - mean) / std;
    }

    // ============================================
    // FIND BEST WORD
    // ============================================

    uint8_t best_word = 0;
    float best_dist = 1e30f;

    for (uint8_t w = 0; w < N_WORDS; w++)
    {
        uint8_t n_templates =
            pgm_read_byte(&n_templates_per_word[w]);

        float word_best = 1e30f;

        // compare against ALL templates
        for (uint8_t t = 0; t < n_templates; t++)
        {
            float dist = 0.0f;

            for (uint8_t i = 0; i < N_FEATURES; i++)
            {
                float tmpl =
                    pgm_read_float(
                        &word_templates[w][t][i]
                    );

                float diff = norm_feat[i] - tmpl;

                dist += diff * diff;
            }

            if (dist < word_best)
            {
                word_best = dist;
            }
        }

        if (word_best < best_dist)
        {
            best_dist = word_best;
            best_word = w;
        }
    }

    return best_word;
}
