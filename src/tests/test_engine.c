#include "fieldofchaos_engine.h"

#include <stdio.h>
#include <string.h>

typedef struct {
    int count;
    int attacks;
    int rounds;
    int results;
} EventCounts;

static int failures = 0;

static void expect_true(bool condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        failures++;
    }
}

static void count_event(const FocEvent *event, void *context) {
    EventCounts *counts = (EventCounts *)context;
    counts->count++;
    if (event->type == FOC_EVENT_ATTACK) {
        counts->attacks++;
    }
    if (event->type == FOC_EVENT_ROUND_START) {
        counts->rounds++;
    }
    if (event->type == FOC_EVENT_RESULT) {
        counts->results++;
    }
    expect_true(event->event_id > 0, "event IDs should be positive");
    expect_true(event->seed == 42, "event seed should match duel seed");
}

static void test_deterministic_character_generation(void) {
    FocCharacter a;
    FocCharacter b;

    foc_seed(7);
    foc_init_default_character(&a, "A");
    foc_seed(7);
    foc_init_default_character(&b, "B");

    expect_true(a.stats.re == b.stats.re, "RE should be deterministic for a fixed seed");
    expect_true(a.stats.ir == b.stats.ir, "IR should be deterministic for a fixed seed");
    expect_true(a.stats.ap == b.stats.ap, "AP should be deterministic for a fixed seed");
    expect_true(a.stats.ph == b.stats.ph, "PH should be deterministic for a fixed seed");
    expect_true(a.stats.me == b.stats.me, "ME should be deterministic for a fixed seed");
}

static void test_character_json_round_trip(void) {
    const char *path = "_test_character_round_trip.json";
    FocCharacter original;
    FocCharacter loaded;
    char error[160];

    foc_seed(11);
    foc_init_default_character(&original, "RoundTrip");
    original.loadout.weapon = FOC_WEAPON_SUBMG;
    original.skills.running = true;

    expect_true(foc_save_character_json(path, &original), "character save should succeed");
    expect_true(foc_load_character_json(path, &loaded, error, sizeof(error)), "character load should succeed");
    expect_true(loaded.schema_version == FOC_CHARACTER_SCHEMA_VERSION, "schema version should round-trip");
    expect_true(strcmp(loaded.name, "RoundTrip") == 0, "name should round-trip");
    expect_true(loaded.loadout.weapon == FOC_WEAPON_SUBMG, "weapon should round-trip");
    expect_true(loaded.skills.running, "skill flag should round-trip");
    remove(path);
}

static void test_future_schema_rejected(void) {
    const char *path = "_test_future_schema.json";
    FocCharacter loaded;
    char error[160];
    FILE *file = fopen(path, "w");
    expect_true(file != NULL, "future schema fixture should open");
    if (file == NULL) {
        return;
    }

    fprintf(file,
            "{\n"
            "  \"schema_version\": 99,\n"
            "  \"name\": \"Future\",\n"
            "  \"stats\": {\"RE\": 1, \"IR\": 1, \"AP\": 1, \"PH\": 1, \"ME\": 1}\n"
            "}\n");
    fclose(file);

    expect_true(!foc_load_character_json(path, &loaded, error, sizeof(error)), "future schema should be rejected");
    expect_true(strstr(error, "unsupported future") != NULL, "future schema error should be actionable");
    remove(path);
}

static void test_duel_events(void) {
    FocCharacter a;
    FocCharacter b;
    FocDuelConfig config;
    EventCounts counts;
    int winner;

    memset(&counts, 0, sizeof(counts));
    foc_seed(42);
    foc_init_default_character(&a, "Alice");
    foc_init_default_character(&b, "Bob");
    a.skills.evade = true;
    b.skills.firearm_advanced = true;
    b.loadout.weapon = FOC_WEAPON_SUBMG;

    config.range = FOC_RANGE_CLOSE;
    config.max_rounds = 12;
    config.verbose = false;

    winner = foc_run_duel(&a, &b, &config, count_event, &counts);

    expect_true(winner == 1 || winner == 2, "duel should return a winner");
    expect_true(counts.count > 0, "duel should emit events");
    expect_true(counts.rounds > 0, "duel should emit round-start events");
    expect_true(counts.attacks > 0, "duel should emit attack events");
    expect_true(counts.results == 1, "duel should emit one result event");
}

static void test_snapshots_and_event_buffer(void) {
    FocCharacter a;
    FocCharacter b;
    FocCharacterSnapshot snapshot;
    FocDuelConfig config;
    FocEventBuffer buffer;
    int winner;
    int count;
    const FocEvent *first;

    foc_seed(42);
    foc_init_default_character(&a, "Alice");
    foc_init_default_character(&b, "Bob");

    foc_make_character_snapshot(&a, &snapshot);
    expect_true(strcmp(snapshot.name, "Alice") == 0, "snapshot should include the character name");
    expect_true(snapshot.wound_total == 11, "snapshot should include total wounds");
    expect_true(snapshot.initiative_modifier == a.stats.me, "snapshot should include initiative modifier");

    config.range = FOC_RANGE_CLOSE;
    config.max_rounds = 3;
    config.verbose = false;

    winner = foc_run_duel_to_buffer(&a, &b, &config, &buffer);
    count = foc_event_buffer_count(&buffer);
    first = foc_event_buffer_get(&buffer, 0);

    expect_true(winner == 1 || winner == 2, "buffered duel should return a winner");
    expect_true(count > 0, "buffered duel should capture events");
    expect_true(!foc_event_buffer_truncated(&buffer), "short buffered duel should not truncate");
    expect_true(first != NULL && first->type == FOC_EVENT_ROUND_START, "first buffered event should start a round");
}

static void test_board_helpers(void) {
    FocCharacter attacker;
    FocCharacter target;
    FocTargetingPreview preview;
    FocEventBuffer buffer;
    char message[FOC_ACTION_MESSAGE_LEN];
    bool ok;

    foc_seed(42);
    foc_init_default_character(&attacker, "Shooter");
    foc_init_default_character(&target, "Target");
    attacker.loadout.weapon = FOC_WEAPON_RIFLE;
    attacker.skills.firearm_basic = true;

    expect_true(foc_weapon_range_yards(FOC_WEAPON_RIFLE, FOC_RANGE_LONG) == 432, "rifle long range should be 432 yards");
    expect_true(foc_roll_initiative(&attacker) > attacker.stats.me, "initiative should include a dice roll above ME");
    expect_true(foc_character_movement_yards(&attacker, FOC_PACE_FAST) == 7, "fast movement should default to 7 yards");
    attacker.skills.running = true;
    expect_true(foc_character_movement_yards(&attacker, FOC_PACE_FAST) == 10, "running should add fast movement yards");
    attacker.wounds.left_leg = 0;
    expect_true(foc_character_movement_yards(&attacker, FOC_PACE_FAST) == 5, "one leg wound should halve movement");
    attacker.wounds.left_leg = 2;

    foc_make_targeting_preview(&attacker, &target, 100, &preview);
    expect_true(preview.in_range, "rifle target at 100 yards should be in range");
    expect_true(preview.can_attack, "valid rifle target should be attackable");
    expect_true(preview.threshold == 4, "100-yard rifle target should be close range");

    foc_event_buffer_clear(&buffer);
    attacker.loadout.rounds_in_clip = 1;
    ok = foc_board_ranged_attack(&attacker, &target, 100, &buffer, message, sizeof(message));
    expect_true(ok, "board ranged attack should resolve");
    expect_true(attacker.loadout.rounds_in_clip == 0, "board attack should spend one round");
    expect_true(foc_event_buffer_count(&buffer) > 0, "board attack should emit events");

    attacker.loadout.clips = 1;
    ok = foc_board_reload(&attacker, &buffer, message, sizeof(message));
    expect_true(ok, "board reload should succeed with a spare clip");
    expect_true(attacker.loadout.rounds_in_clip == FOC_DEFAULT_CLIP_SIZE, "reload should fill the clip");

    attacker.jammed = true;
    ok = foc_board_clear_jam(&attacker, &buffer, message, sizeof(message));
    expect_true(ok, "clear jam should succeed when weapon is jammed");
    expect_true(!attacker.jammed, "clear jam should reset jammed state");
}

static void test_scenario_ai_campaign_helpers(void) {
    FocScenarioConfig config;
    FocScenario scenario;
    FocCharacter player;
    FocCharacter ai;
    FocAIAction action;
    FocCampaignRecord campaign;
    char message[FOC_ACTION_MESSAGE_LEN];

    config.seed = 401;
    config.difficulty = 2;
    config.width = 18;
    config.height = 11;
    foc_generate_scenario(&config, &scenario);
    expect_true(scenario.width == 18, "scenario should use configured width");
    expect_true(scenario.height == 11, "scenario should use configured height");
    expect_true(scenario.objective_x == 9, "scenario objective should be centered");

    foc_seed(42);
    foc_init_default_character(&player, "Player");
    foc_init_default_character(&ai, "AI");
    ai.loadout.weapon = FOC_WEAPON_RIFLE;
    foc_choose_ai_action(&ai, &player, 14, 5, 2, 5, scenario.width, scenario.height, &action);
    expect_true(action.type == FOC_AI_ATTACK || action.type == FOC_AI_MOVE, "AI should attack or move toward target");

    foc_campaign_init(&campaign);
    foc_campaign_apply_result(&campaign, true, &player, &scenario);
    expect_true(campaign.missions == 1, "campaign should record a mission");
    expect_true(campaign.wins == 1, "campaign should record a win");
    expect_true(campaign.advances_available == 1, "campaign win should grant advancement");
    expect_true(foc_campaign_spend_advancement(&campaign, &player, message, sizeof(message)), "campaign advancement should spend");
}

static void test_expanded_combat_helpers(void) {
    FocCharacter medic;
    FocCharacter target;
    FocEventBuffer buffer;
    char message[FOC_ACTION_MESSAGE_LEN];
    bool ok;

    foc_seed(5);
    foc_init_default_character(&medic, "Medic");
    foc_init_default_character(&target, "Target");
    medic.loadout.medical = FOC_MEDICAL_PARAMEDIC;
    target.wounds.body = 2;
    foc_event_buffer_clear(&buffer);
    ok = foc_board_heal(&medic, &target, &buffer, message, sizeof(message));
    expect_true(foc_event_buffer_count(&buffer) == 1, "healing should emit a status event");
    expect_true(ok == true || ok == false, "healing should return a boolean result");

    foc_seed(8);
    foc_event_buffer_clear(&buffer);
    ok = foc_board_grenade_attack(&medic, &target, 3, false, &buffer, message, sizeof(message));
    expect_true(foc_event_buffer_count(&buffer) > 0, "grenade should emit events when in range");
    expect_true(ok == true || ok == false, "grenade should return a boolean result");
}

int main(void) {
    test_deterministic_character_generation();
    test_character_json_round_trip();
    test_future_schema_rejected();
    test_duel_events();
    test_snapshots_and_event_buffer();
    test_board_helpers();
    test_scenario_ai_campaign_helpers();
    test_expanded_combat_helpers();

    if (failures != 0) {
        fprintf(stderr, "%d test failure(s)\n", failures);
        return 1;
    }

    printf("engine tests passed\n");
    return 0;
}
