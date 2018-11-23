#pragma once

#include "animation_config.h"
#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if FASTLED_SUPPORTED

    void animation_init(void);
    void animation_load(bool clear);
    void animation_save(void);
    void animation_reset(void);
    void animation_start(uint8_t strand);
    void animation_stop(uint8_t strand);
    void animation_loop(void);

    uint16_t get_real_fastled_fps(void);

    void load_strand_config(uint8_t strand);
    void save_strand_config(uint8_t strand);

    void set_strand_animation_running(uint8_t strand, bool running);
    bool is_strand_animation_running(uint8_t strand);

    uint8_t get_animation_index(uint8_t strand);
    void set_animation_by_index(uint8_t strand, uint8_t animation);
    void run_animation_by_index(uint8_t strand, uint8_t animation);

    uint8_t run_next_animation(uint8_t strand);
    uint8_t run_previous_animation(uint8_t strand);

    void set_strand_fps(uint8_t strand, uint8_t fps);
    uint8_t get_strand_fps(uint8_t strand);

    void set_strand_color(uint8_t strand, uint8_t cnr, uint8_t hsv[3]);
    void get_strand_color(uint8_t strand, uint8_t cnr, uint8_t hsv[3]);

    void set_strand_sensor_index(uint8_t strand, uint8_t sensor_id, uint8_t sensor);
    uint8_t get_strand_sensor_index(uint8_t strand, uint8_t sensor_id);

    void set_strand_autoplay(uint8_t strand, bool autoplay);
    bool get_strand_autoplay(uint8_t strand);

    void set_strand_autoplay_delay_time(uint8_t strand, uint16_t autoplay_delay_secs);
    uint16_t get_strand_autoplay_delay_time(uint8_t strand);

    void set_animation_fps(uint8_t strand, uint8_t animation, uint8_t fps);
    uint8_t get_animation_fps(uint8_t strand, uint8_t animation);

    void set_animation_sensor_index(uint8_t strand, uint8_t animation, uint8_t sensor_id, uint8_t sensor);
    uint8_t get_animation_sensor_index(uint8_t strand, uint8_t animation, uint8_t sensor_id);

    void set_animation_color(uint8_t strand, uint8_t animation, uint8_t cnr, uint8_t hsv[3]);
    void get_animation_color(uint8_t strand, uint8_t animation, uint8_t cnr, uint8_t hsv[3]);

    void set_strand_option_0(uint8_t strand, uint8_t option);
    uint8_t get_strand_option_0(uint8_t strand);

    void set_strand_option_1(uint8_t strand, uint8_t option);
    uint8_t get_strand_option_1(uint8_t strand);

    void set_animation_option(uint8_t strand, uint8_t animation, uint8_t option_id, uint8_t option);
    uint8_t get_animation_option(uint8_t strand, uint8_t animation, uint8_t option_id);

    void set_strand_autorun(uint8_t strand, bool on);
    bool get_strand_autorun(uint8_t strand);

    void set_strand_autorun_sensor_delta(uint8_t strand, int16_t delta);
    int16_t get_strand_autorun_sensor_delta(uint8_t strand);

    bool was_strand_autostarted(uint8_t strand);
    uint16_t get_strand_sensor_delta(uint8_t strand);

    void set_strands_global_brightness(uint8_t brightness);
    uint8_t get_strands_global_brightness(void);

    void ani_led_strand_reset_strand(uint8_t strand);

#else
#define animation_init()                                                                                                                             \
    do                                                                                                                                               \
    {                                                                                                                                                \
    } while (0)
#define animation_loop()                                                                                                                             \
    do                                                                                                                                               \
    {                                                                                                                                                \
    } while (0)
#endif

#ifdef __cplusplus
}
#endif
