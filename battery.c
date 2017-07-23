#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "battery.h"
#include "util.h"

int battery_charge_now(battery_t *batt) {
    char *battery_charge_now_file;
    asprintf(&battery_charge_now_file,
            "/sys/class/power_supply/%s/charge_now", batt->name);

    uint_from_file(&batt->charge_now, battery_charge_now_file);

    free(battery_charge_now_file);

    return 0;
}

int battery_charge_full(battery_t *batt) {
    char *battery_charge_full_file;
    asprintf(&battery_charge_full_file,
            "/sys/class/power_supply/%s/charge_full", batt->name);

    uint_from_file(&batt->charge_full, battery_charge_full_file);

    free(battery_charge_full_file);

    return 0;
}

int battery_charge_status(battery_t *batt) {
    char *battery_status_file;
    asprintf(&battery_status_file,
            "/sys/class/power_supply/%s/status", batt->name);

    char charge_status[13];
    string_from_file(charge_status, battery_status_file);

    if (strcmp(charge_status, "Charging") == 0) {
        batt->charge_status = CHARGING;
    } else if (strcmp(charge_status, "Discharging") == 0) {
        batt->charge_status = DISCHARGING;
    } else if (strcmp(charge_status, "Full") == 0) {
        batt->charge_status = FULL;
    } else {
        return 0;
    }

    free(battery_status_file);

    return 0;
}

int battery_current_now(battery_t *batt) {
    char *battery_current_now_file;
    asprintf(&battery_current_now_file,
            "/sys/class/power_supply/%s/current_now", batt->name);

    uint_from_file(&batt->current_now, battery_current_now_file);

    free(battery_current_now_file);

    return 0;
}

int battery_current_avg(battery_t *batt) {
    char *battery_current_avg_file;
    asprintf(&battery_current_avg_file,
            "/sys/class/power_supply/%s/current_avg", batt->name);

    uint_from_file(&batt->current_avg, battery_current_avg_file);

    free(battery_current_avg_file);

    return 0;
}

int battery_charge_full_design(battery_t *batt) {
    char *battery_charge_full_design_file;
    asprintf(&battery_charge_full_design_file,
            "/sys/class/power_supply/%s/charge_full_design", batt->name);

    uint_from_file(&batt->charge_full_design, battery_charge_full_design_file);

    free(battery_charge_full_design_file);

    return 0;
}

int battery_cycle_count(battery_t *batt) {
    char *battery_cycle_count_file;
    asprintf(&battery_cycle_count_file,
            "/sys/class/power_supply/%s/cycle_count", batt->name);

    uint_from_file(&batt->cycle_count, battery_cycle_count_file);

    free(battery_cycle_count_file);

    return 0;
}

/* Exported functions begin here */

int initialize_battery(battery_t *battery, char *name) {
    int status = 0;
    battery->name = name;
    status += battery_charge_now(battery);
    status += battery_charge_full(battery);
    status += battery_charge_full_design(battery);
    status += battery_cycle_count(battery);
    status += battery_charge_status(battery);
    status += battery_current_now(battery);
    status += battery_current_avg(battery);
    return status;
}

char *battery_status_as_string(battery_status status) {
    switch(status) {
        case CHARGING:
            return "Charging";
        case DISCHARGING:
            return "Discharging";
        case FULL:
            return "Full";
        default:
            return "Unknown";
    }
}

int time_remaining(char **time_left_str, battery_t *batt) {
    // TODO: Find a better way to handle the FULL state. Maybe as "CHARGING"?
    float time_left;
    switch (batt->charge_status) {
        case CHARGING:
            time_left = ((float) batt->charge_full - (float) batt->charge_now) / (float) batt->current_avg;
            break;
        case DISCHARGING:
            time_left = ((float) batt->charge_now / (float) batt->current_avg);
            break;
        default:
            time_left = 0;
            break;
    }
    uint8_t hours_left = (uint8_t) time_left;
    uint8_t mins_left = (uint8_t) ((time_left - hours_left) * 60);
    return asprintf(time_left_str, "%02d:%02d", hours_left, mins_left);
}

uint32_t battery_health(battery_t *batt) {
    return batt->charge_full * 100 / batt->charge_full_design;
}

uint32_t charge_percent(battery_t *batt) {
    return batt->charge_now * 100 / batt->charge_full;
}

uint32_t abs_charge_percent(battery_t *batt) {
    return batt->charge_now * 100 / batt->charge_full_design;
}
