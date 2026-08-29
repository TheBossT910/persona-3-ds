#include "demo_dialogue.hpp"

// dialogue content
Dialogue demo_yukari_kenji_argument_lines[18];
void demo_yukari_kenji_argument_init()
{
    DialogueSelection demo_yukari_kenji_argument_sel_6_0 = {
        "Vouch for Yukari", false, &demo_yukari_kenji_argument_lines[7]};
    DialogueSelection demo_yukari_kenji_argument_sel_6_1 = {
        "Side with Kenji", false, &demo_yukari_kenji_argument_lines[12]};
    DialogueSelection demo_yukari_kenji_argument_sel_6_2 = {
        "Stay out of it", false, &demo_yukari_kenji_argument_lines[15]};

    demo_yukari_kenji_argument_lines[0] = {
        "Kenji",
        "Yukari, the\xFF\x01\x07 east path\xFF\x01\xFF is closed after sundown. I've said it twice.",
        BustKenji::spNeutral,
        NULL,
        &demo_yukari_kenji_argument_lines[1],
        {}};
    demo_yukari_kenji_argument_lines[1] = {
        "Yukari",
        "And I've told you - I left my bag on the bench. It will take thirty seconds.",
        BustYukari::spAngry,
        &demo_yukari_kenji_argument_lines[0],
        &demo_yukari_kenji_argument_lines[2],
        {}};
    demo_yukari_kenji_argument_lines[2] = {"Kenji",
                                           "Rules are rules. Come back tomorrow.",
                                           BustKenji::spNeutral,
                                           &demo_yukari_kenji_argument_lines[1],
                                           &demo_yukari_kenji_argument_lines[3],
                                           {}};
    demo_yukari_kenji_argument_lines[3] = {"Akihiko",
                                           "Hey, what's going on?",
                                           BustAkihiko::spNeutral,
                                           &demo_yukari_kenji_argument_lines[2],
                                           &demo_yukari_kenji_argument_lines[4],
                                           {}};
    demo_yukari_kenji_argument_lines[4] = {"Yukari",
                                           "Kenji won't let me through to get my bag.",
                                           BustYukari::spAngry,
                                           &demo_yukari_kenji_argument_lines[3],
                                           &demo_yukari_kenji_argument_lines[5],
                                           {}};
    demo_yukari_kenji_argument_lines[5] = {"Kenji",
                                           "Akihiko, stay back. This doesn't concern you.",
                                           BustKenji::spNeutral,
                                           &demo_yukari_kenji_argument_lines[4],
                                           &demo_yukari_kenji_argument_lines[6],
                                           {}};
    demo_yukari_kenji_argument_lines[6] = {
        "Akihiko",
        "Can we work something out here?",
        BustAkihiko::spNeutral,
        &demo_yukari_kenji_argument_lines[5],
        NULL,
        {demo_yukari_kenji_argument_sel_6_0, demo_yukari_kenji_argument_sel_6_1, demo_yukari_kenji_argument_sel_6_2}};
    demo_yukari_kenji_argument_lines[7] = {"Akihiko",
                                           "C'mon, she's not going to cause any trouble.",
                                           BustAkihiko::spNeutral,
                                           &demo_yukari_kenji_argument_lines[6],
                                           &demo_yukari_kenji_argument_lines[8],
                                           {}};
    demo_yukari_kenji_argument_lines[8] = {"Kenji",
                                           "...",
                                           BustKenji::spNeutral,
                                           &demo_yukari_kenji_argument_lines[7],
                                           &demo_yukari_kenji_argument_lines[9],
                                           {}};
    demo_yukari_kenji_argument_lines[9] = {"Kenji",
                                           "Fine. Two minutes. And you're coming with her.",
                                           BustKenji::spNeutral,
                                           &demo_yukari_kenji_argument_lines[8],
                                           &demo_yukari_kenji_argument_lines[10],
                                           {}};
    demo_yukari_kenji_argument_lines[10] = {"Yukari",
                                            "Thank you.",
                                            BustYukari::spAngry,
                                            &demo_yukari_kenji_argument_lines[9],
                                            &demo_yukari_kenji_argument_lines[11],
                                            {}};
    demo_yukari_kenji_argument_lines[11] = {
        "Yukari", "...Both of you.", BustYukari::spAngry, &demo_yukari_kenji_argument_lines[10], NULL, {}};
    demo_yukari_kenji_argument_lines[12] = {"Akihiko",
                                            "He's got a point, Yukari. Come back in the morning.",
                                            BustAkihiko::spNeutral,
                                            &demo_yukari_kenji_argument_lines[11],
                                            &demo_yukari_kenji_argument_lines[13],
                                            {}};
    demo_yukari_kenji_argument_lines[13] = {"Yukari",
                                            "Are you serious right now?",
                                            BustYukari::spAngry,
                                            &demo_yukari_kenji_argument_lines[12],
                                            &demo_yukari_kenji_argument_lines[14],
                                            {}};
    demo_yukari_kenji_argument_lines[14] = {"Yukari",
                                            "Fine. Don't talk to me tonight.",
                                            BustYukari::spAngry,
                                            &demo_yukari_kenji_argument_lines[13],
                                            NULL,
                                            {}};
    demo_yukari_kenji_argument_lines[15] = {"Akihiko",
                                            "Actually - yeah, none of my business. Carry on.",
                                            BustAkihiko::spNeutral,
                                            &demo_yukari_kenji_argument_lines[14],
                                            &demo_yukari_kenji_argument_lines[16],
                                            {}};
    demo_yukari_kenji_argument_lines[16] = {"Kenji",
                                            "Smart man.",
                                            BustKenji::spNeutral,
                                            &demo_yukari_kenji_argument_lines[15],
                                            &demo_yukari_kenji_argument_lines[17],
                                            {}};
    demo_yukari_kenji_argument_lines[17] = {
        "Yukari", "Unbelievable.", BustYukari::spAngry, &demo_yukari_kenji_argument_lines[16], NULL, {}};
}
