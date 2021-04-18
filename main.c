#include <assert.h>
#include <stdio.h>

#define ADC_BITS        4
#define RANGE           1000
#define NOISE_PERCENT   10

#define LEVELS          (1 << ADC_BITS)
#define THRESHOLD       (RANGE / LEVELS)
#define NOISE           ((((RANGE / LEVELS) * 100) / NOISE_PERCENT) / 100)

static unsigned int g_thresholds[LEVELS];

static void create_lookup_table(void)
{
    unsigned int current_threshold = THRESHOLD;
    for (int level = 0; level < LEVELS; level++) {
        g_thresholds[level] = current_threshold - (THRESHOLD / 2);
        current_threshold += THRESHOLD;
        printf("threshold: %d, level: %d\n", g_thresholds[level], level + 1);
    }
}

unsigned int hysteresis(unsigned int input_percent)
{
    static unsigned int discrete_level = 0;
    static unsigned int prev_input_percent = 0;

    if (input_percent >= prev_input_percent) {
        for(int level = discrete_level; level < LEVELS; level++) {
            if (input_percent >= g_thresholds[level] + NOISE) {
                discrete_level = level + 1;
            } else {
                break;
            }
        }
    } else {
        for (int level = discrete_level - 1; level >= 0; level--) {
            if (input_percent <= g_thresholds[level] - NOISE) {
                discrete_level = level;
            } else {
                break;
            }
        }
    }
    prev_input_percent = input_percent;

    return discrete_level;
 }

int main(void)
{
    assert(ADC_BITS < (sizeof(int) * 8));
    assert(LEVELS < RANGE);
    assert(NOISE < THRESHOLD);
    printf("bits: %d, levels: %d, range: %d, noise: %d%%\n", ADC_BITS, LEVELS, RANGE, NOISE_PERCENT);

    create_lookup_table();

    printf("noise: %d, threshold: %d\n", NOISE, THRESHOLD);

    for (;;) {
        unsigned int percentage_in = 0;
        printf("input: ");
        scanf("%u", &percentage_in);
        if (percentage_in > RANGE) {
            printf("Input out of range, quitting\n");
            break;
        } else {
            printf("level: %d\n", hysteresis(percentage_in));
        }
    }
    return 0;
}
