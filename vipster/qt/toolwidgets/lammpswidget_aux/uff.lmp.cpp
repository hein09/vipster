#include "uff.lmp.h"
#include "fmt/format.h"

using namespace Vipster;

std::vector<std::tuple<size_t, size_t, int>> aromaticity_criteria{
    // atomic number, coordination, delta ring-size (starting at 6)
    // boron group:
    {5,  3, +1},
    // carbon group:
    {6,  3,  0},
    {14, 3,  0},
    // nitrogen group:
    {7,  2,  0},
    {7,  3, -1},
    {15, 2,  0},
    {15, 3, -1},
    // oxygen group:
    {8,  2, -1},
    {16, 2, -1},
};

/* Types:
 * map (atom number, coordination number, aromatic) to name
 * atom number must match exactly
 * coordination matches nearest or is ignored (-1)
 * aromaticity matches exactly or is ignored (-1)
 */
const std::map<size_t, std::vector<std::tuple<int, int, std::string>>> UFF_Criteria{
    {1,   {{ 1, -1, "H_"}, {2, -1, "H_b"}}},
    {2,   {{-1, -1, "He4+4"}}},
    {3,   {{-1, -1, "Li"}}},
    {4,   {{-1, -1, "Be3+2"}}},
    {5,   {{ 4, -1, "B_3"}, {3, -1, "B_2"}}},
    {6,   {{ 4, -1, "C_3"}, {3,  1, "C_R"}, {3,  0, "C_2"}, {2, -1, "C_1"}, {1, -1, "C_1"}}},
    {7,   {{ 3,  0, "N_3"}, {3,  1, "N_R"}, {2,  1, "N_R"}, {2,  0, "N_2"}, {1, -1, "N_1"}}},
    {8,   {{ 2,  0, "O_3"}, {2,  1, "O_R"}, {1, -1, "O_2"}}},
    {9,   {{-1, -1, "F_"}}},
    {10,  {{-1, -1, "Ne4+4"}}},
    {11,  {{-1, -1, "Na"}}},
    {12,  {{-1, -1, "Mg3+2"}}},
    {13,  {{-1, -1, "Al3"}}},
    {14,  {{-1, -1, "Si3"}}},
    {15,  {{ 3, -1, "P_3+3"}, {4, -1, "P_3+5"}}},
    {16,  {{ 2,  0, "S_3+2"}, {3, -1, "S_3+4"}, {4, -1, "S_3+6"}, {2,  1, "S_R"}, {1, -1, "S_2"}}},
    {17,  {{-1, -1, "Cl"}}},
    {18,  {{-1, -1, "Ar4+4"}}},
    {19,  {{-1, -1, "K_"}}},
    {20,  {{-1, -1, "Ca6+2"}}},
    {21,  {{-1, -1, "Sc3+3"}}},
    {22,  {{ 4, -1, "Ti3+4"}, {6, -1, "Ti6+4"}}},
    {23,  {{-1, -1, "V_3+5"}}},
    {24,  {{-1, -1, "Cr6+3"}}},
    {25,  {{-1, -1, "Mn6+2"}}},
    {26,  {{ 4, -1, "Fe3+2"}, {6, -1, "Fe6+2"}}},
    {27,  {{-1, -1, "Co6+3"}}},
    {28,  {{-1, -1, "Ni4+2"}}},
    {29,  {{-1, -1, "Cu3+1"}}},
    {30,  {{-1, -1, "Zn3+2"}}},
    {31,  {{-1, -1, "Ga3+3"}}},
    {32,  {{-1, -1, "Ge3"}}},
    {33,  {{-1, -1, "As3+3"}}},
    {34,  {{-1, -1, "Se3+2"}}},
    {35,  {{-1, -1, "Br"}}},
    {36,  {{-1, -1, "Kr4+4"}}},
    {37,  {{-1, -1, "Rb"}}},
    {38,  {{-1, -1, "Sr6+2"}}},
    {39,  {{-1, -1, "Y_3+3"}}},
    {40,  {{-1, -1, "Zr3+4"}}},
    {41,  {{-1, -1, "Nb3+5"}}},
    {42,  {{ 4, -1, "Mo3+6"}, {6, -1, "Mo6+6"}}},
    {43,  {{-1, -1, "Tc6+5"}}},
    {44,  {{-1, -1, "Ru6+2"}}},
    {45,  {{-1, -1, "Rh6+3"}}},
    {46,  {{-1, -1, "Pd4+2"}}},
    {47,  {{-1, -1, "Ag1+1"}}},
    {48,  {{-1, -1, "Cd3+2"}}},
    {49,  {{-1, -1, "In3+3"}}},
    {50,  {{-1, -1, "Sn3"}}},
    {51,  {{-1, -1, "Sb3+3"}}},
    {52,  {{-1, -1, "Te3+2"}}},
    {53,  {{-1, -1, "I_"}}},
    {54,  {{-1, -1, "Xe4+4"}}},
    {55,  {{-1, -1, "Cs"}}},
    {56,  {{-1, -1, "Ba6+2"}}},
    {57,  {{-1, -1, "La3+3"}}},
    {58,  {{-1, -1, "Ce6+3"}}},
    {59,  {{-1, -1, "Pr6+3"}}},
    {60,  {{-1, -1, "Nd6+3"}}},
    {61,  {{-1, -1, "Pm6+3"}}},
    {62,  {{-1, -1, "Sm6+3"}}},
    {63,  {{-1, -1, "Eu6+3"}}},
    {64,  {{-1, -1, "Gd6+3"}}},
    {65,  {{-1, -1, "Tb6+3"}}},
    {66,  {{-1, -1, "Dy6+3"}}},
    {67,  {{-1, -1, "Ho6+3"}}},
    {68,  {{-1, -1, "Er6+3"}}},
    {69,  {{-1, -1, "Tm6+3"}}},
    {70,  {{-1, -1, "Yb6+3"}}},
    {71,  {{-1, -1, "Lu6+3"}}},
    {72,  {{-1, -1, "Hf3+4"}}},
    {73,  {{-1, -1, "Ta3+5"}}},
    {74,  {{ 3, -1, "W_3+4"}, {4, -1, "W_3+6"}, {6, -1, "W_6+6"}}},
    {75,  {{ 4, -1, "Re3+7"}, {6, -1, "Re6+5"}}},
    {76,  {{-1, -1, "Os6+6"}}},
    {77,  {{-1, -1, "Ir6+3"}}},
    {78,  {{-1, -1, "Pt4+2"}}},
    {79,  {{-1, -1, "Au4+3"}}},
    {80,  {{-1, -1, "Hg1+2"}}},
    {81,  {{-1, -1, "Tl3+3"}}},
    {82,  {{-1, -1, "Pb3"}}},
    {83,  {{-1, -1, "Bi3+3"}}},
    {84,  {{-1, -1, "Po3+2"}}},
    {85,  {{-1, -1, "At"}}},
    {86,  {{-1, -1, "Rn4+4"}}},
    {87,  {{-1, -1, "Fr"}}},
    {88,  {{-1, -1, "Ra6+2"}}},
    {89,  {{-1, -1, "Ac6+3"}}},
    {90,  {{-1, -1, "Th6+4"}}},
    {91,  {{-1, -1, "Pa6+4"}}},
    {92,  {{-1, -1, "U_6+4"}}},
    {93,  {{-1, -1, "Np6+4"}}},
    {94,  {{-1, -1, "Pu6+4"}}},
    {95,  {{-1, -1, "Am6+4"}}},
    {96,  {{-1, -1, "Cm6+3"}}},
    {97,  {{-1, -1, "Bk6+3"}}},
    {98,  {{-1, -1, "Cf6+3"}}},
    {99,  {{-1, -1, "Es6+3"}}},
    {100, {{-1, -1, "Fm6+3"}}},
    {101, {{-1, -1, "Md6+3"}}},
    {102, {{-1, -1, "No6+3"}}},
    {103, {{-1, -1, "Lw6+3"}}}
};

/* Parameters:
 * {name, {rb0/Å, th0/°, rnb0/Å, D/kcal/mol, scale/ignored, charge, V, U, Xi, Hard, Radius}}
 */
const std::map<std::string, std::tuple<double, double, double, double, double, double, double, double, double, double, double>> UFF_Parameters{
    {"H_",     {0.354,  180,    2.886,  0.044,  12,     0.712,  0,      0,      4.528,  6.9452, 0.371}},
    {"H_b",    {0.46,   83.5,   2.886,  0.044,  12,     0.712,  0,      0,      4.528,  6.9452, 0.371}},
    {"He4+4",  {0.849,  90,     2.362,  0.056,  15.24,  0.098,  0,      0,      9.66,   14.92,  1.3}},
    {"Li",     {1.336,  180,    2.451,  0.025,  12,     1.026,  0,      2,      3.006,  2.386,  1.557}},
    {"Be3+2",  {1.074,  109.47, 2.745,  0.085,  12,     1.565,  0,      2,      4.877,  4.443,  1.24}},
    {"B_3",    {0.838,  109.47, 4.083,  0.18,   12.052, 1.755,  0,      2,      5.11,   4.75,   0.822}},
    {"B_2",    {0.828,  120,    4.083,  0.18,   12.052, 1.755,  0,      2,      5.11,   4.75,   0.822}},
    {"C_3",    {0.757,  109.47, 3.851,  0.105,  12.73,  1.912,  2.119,  2,      5.343,  5.063,  0.759}},
    {"C_R",    {0.729,  120,    3.851,  0.105,  12.73,  1.912,  0,      2,      5.343,  5.063,  0.759}},
    {"C_2",    {0.732,  120,    3.851,  0.105,  12.73,  1.912,  0,      2,      5.343,  5.063,  0.759}},
    {"C_1",    {0.706,  180,    3.851,  0.105,  12.73,  1.912,  0,      2,      5.343,  5.063,  0.759}},
    {"N_3",    {0.7,    106.7,  3.66,   0.069,  13.407, 2.544,  0.45,   2,      6.899,  5.88,   0.715}},
    {"N_R",    {0.699,  120,    3.66,   0.069,  13.407, 2.544,  0,      2,      6.899,  5.88,   0.715}},
    {"N_2",    {0.685,  111.2,  3.66,   0.069,  13.407, 2.544,  0,      2,      6.899,  5.88,   0.715}},
    {"N_1",    {0.656,  180,    3.66,   0.069,  13.407, 2.544,  0,      2,      6.899,  5.88,   0.715}},
    {"O_3",    {0.658,  104.51, 3.5,    0.06,   14.085, 2.3,    0.018,  2,      8.741,  6.682,  0.669}},
    {"O_3_z",  {0.528,  146,    3.5,    0.06,   14.085, 2.3,    0.018,  2,      8.741,  6.682,  0.669}}, // not actively assigned, criterion too vague/complicated
    {"O_R",    {0.68,   110,    3.5,    0.06,   14.085, 2.3,    0,      2,      8.741,  6.682,  0.669}},
    {"O_2",    {0.634,  120,    3.5,    0.06,   14.085, 2.3,    0,      2,      8.741,  6.682,  0.669}},
    {"O_1",    {0.639,  180,    3.5,    0.06,   14.085, 2.3,    0,      2,      8.741,  6.682,  0.669}}, // not actively assigned, should only be terminal like in CO, O_2 should suffice
    {"F_",     {0.668,  180,    3.364,  0.05,   14.762, 1.735,  0,      2,      10.874, 7.474,  0.706}},
    {"Ne4+4",  {0.92,   90,     3.243,  0.042,  15.44,  0.194,  0,      2,      11.04,  10.55,  1.768}},
    {"Na",     {1.539,  180,    2.983,  0.03,   12,     1.081,  0,      1.25,   2.843,  2.296,  2.085}},
    {"Mg3+2",  {1.421,  109.47, 3.021,  0.111,  12,     1.787,  0,      1.25,   3.951,  3.693,  1.5}},
    {"Al3",    {1.244,  109.47, 4.499,  0.505,  11.278, 1.792,  0,      1.25,   4.06,   3.59,   1.201}},
    {"Si3",    {1.117,  109.47, 4.295,  0.402,  12.175, 2.323,  1.225,  1.25,   4.168,  3.487,  1.176}},
    {"P_3+3",  {1.101,  93.8,   4.147,  0.305,  13.072, 2.863,  2.4,    1.25,   5.463,  4,      1.102}},
    {"P_3+5",  {1.056,  109.47, 4.147,  0.305,  13.072, 2.863,  2.4,    1.25,   5.463,  4,      1.102}},
    {"P_3+q",  {1.056,  109.47, 4.147,  0.305,  13.072, 2.863,  2.4,    1.25,   5.463,  4,      1.102}}, // not actively assigned, duplicate of P_3+5
    {"S_3+2",  {1.064,  92.1,   4.035,  0.274,  13.969, 2.703,  0.484,  1.25,   6.928,  4.486,  1.047}},
    {"S_3+4",  {1.049,  103.2,  4.035,  0.274,  13.969, 2.703,  0.484,  1.25,   6.928,  4.486,  1.047}},
    {"S_3+6",  {1.027,  109.47, 4.035,  0.274,  13.969, 2.703,  0.484,  1.25,   6.928,  4.486,  1.047}},
    {"S_R",    {1.077,  92.2,   4.035,  0.274,  13.969, 2.703,  0,      1.25,   6.928,  4.486,  1.047}},
    {"S_2",    {0.854,  120,    4.035,  0.274,  13.969, 2.703,  0,      1.25,   6.928,  4.486,  1.047}},
    {"Cl",     {1.044,  180,    3.947,  0.227,  14.866, 2.348,  0,      1.25,   8.564,  4.946,  0.994}},
    {"Ar4+4",  {1.032,  90,     3.868,  0.185,  15.763, 0.3,    0,      1.25,   9.465,  6.355,  2.108}},
    {"K_",     {1.953,  180,    3.812,  0.035,  12,     1.165,  0,      0.7,    2.421,  1.92,   2.586}},
    {"Ca6+2",  {1.761,  90,     3.399,  0.238,  12,     2.141,  0,      0.7,    3.231,  2.88,   2}},
    {"Sc3+3",  {1.513,  109.47, 3.295,  0.019,  12,     2.592,  0,      0.7,    3.395,  3.08,   1.75}},
    {"Ti3+4",  {1.412,  109.47, 3.175,  0.017,  12,     2.659,  0,      0.7,    3.47,   3.38,   1.607}},
    {"Ti6+4",  {1.412,  90,     3.175,  0.017,  12,     2.659,  0,      0.7,    3.47,   3.38,   1.607}},
    {"V_3+5",  {1.402,  109.47, 3.144,  0.016,  12,     2.679,  0,      0.7,    3.65,   3.41,   1.47}},
    {"Cr6+3",  {1.345,  90,     3.023,  0.015,  12,     2.463,  0,      0.7,    3.415,  3.865,  1.402}},
    {"Mn6+2",  {1.382,  90,     2.961,  0.013,  12,     2.43,   0,      0.7,    3.325,  4.105,  1.533}},
    {"Fe3+2",  {1.27,   109.47, 2.912,  0.013,  12,     2.43,   0,      0.7,    3.76,   4.14,   1.393}},
    {"Fe6+2",  {1.335,  90,     2.912,  0.013,  12,     2.43,   0,      0.7,    3.76,   4.14,   1.393}},
    {"Co6+3",  {1.241,  90,     2.872,  0.014,  12,     2.43,   0,      0.7,    4.105,  4.175,  1.406}},
    {"Ni4+2",  {1.164,  90,     2.834,  0.015,  12,     2.43,   0,      0.7,    4.465,  4.205,  1.398}},
    {"Cu3+1",  {1.302,  109.47, 3.495,  0.005,  12,     1.756,  0,      0.7,    4.2,    4.22,   1.434}},
    {"Zn3+2",  {1.193,  109.47, 2.763,  0.124,  12,     1.308,  0,      0.7,    5.106,  4.285,  1.4}},
    {"Ga3+3",  {1.26,   109.47, 4.383,  0.415,  11,     1.821,  0,      0.7,    3.641,  3.16,   1.211}},
    {"Ge3",    {1.197,  109.47, 4.28,   0.379,  12,     2.789,  0.701,  0.7,    4.051,  3.438,  1.189}},
    {"As3+3",  {1.211,  92.1,   4.23,   0.309,  13,     2.864,  1.5,    0.7,    5.188,  3.809,  1.204}},
    {"Se3+2",  {1.19,   90.6,   4.205,  0.291,  14,     2.764,  0.335,  0.7,    6.428,  4.131,  1.224}},
    {"Br",     {1.192,  180,    4.189,  0.251,  15,     2.519,  0,      0.7,    7.79,   4.425,  1.141}},
    {"Kr4+4",  {1.147,  90,     4.141,  0.22,   16,     0.452,  0,      0.7,    8.505,  5.715,  2.27}},
    {"Rb",     {2.26,   180,    4.114,  0.04,   12,     1.592,  0,      0.2,    2.331,  1.846,  2.77}},
    {"Sr6+2",  {2.052,  90,     3.641,  0.235,  12,     2.449,  0,      0.2,    3.024,  2.44,   2.415}},
    {"Y_3+3",  {1.698,  109.47, 3.345,  0.072,  12,     3.257,  0,      0.2,    3.83,   2.81,   1.998}},
    {"Zr3+4",  {1.564,  109.47, 3.124,  0.069,  12,     3.667,  0,      0.2,    3.4,    3.55,   1.758}},
    {"Nb3+5",  {1.473,  109.47, 3.165,  0.059,  12,     3.618,  0,      0.2,    3.55,   3.38,   1.603}},
    {"Mo3+6",  {1.484,  109.47, 3.052,  0.056,  12,     3.4,    0,      0.2,    3.465,  3.755,  1.53}},
    {"Mo6+6",  {1.467,  90,     3.052,  0.056,  12,     3.4,    0,      0.2,    3.465,  3.755,  1.53}},
    {"Tc6+5",  {1.322,  90,     2.998,  0.048,  12,     3.4,    0,      0.2,    3.29,   3.99,   1.5}},
    {"Ru6+2",  {1.478,  90,     2.963,  0.056,  12,     3.4,    0,      0.2,    3.575,  4.015,  1.5}},
    {"Rh6+3",  {1.332,  90,     2.929,  0.053,  12,     3.5,    0,      0.2,    3.975,  4.005,  1.509}},
    {"Pd4+2",  {1.338,  90,     2.899,  0.048,  12,     3.21,   0,      0.2,    4.32,   4,      1.544}},
    {"Ag1+1",  {1.386,  180,    3.148,  0.036,  12,     1.956,  0,      0.2,    4.436,  3.134,  1.622}},
    {"Cd3+2",  {1.403,  109.47, 2.848,  0.228,  12,     1.65,   0,      0.2,    5.034,  3.957,  1.6}},
    {"In3+3",  {1.459,  109.47, 4.463,  0.599,  11,     2.07,   0,      0.2,    3.506,  2.896,  1.404}},
    {"Sn3",    {1.398,  109.47, 4.392,  0.567,  12,     2.961,  0.199,  0.2,    3.987,  3.124,  1.354}},
    {"Sb3+3",  {1.407,  91.6,   4.42,   0.449,  13,     2.704,  1.1,    0.2,    4.899,  3.342,  1.404}},
    {"Te3+2",  {1.386,  90.25,  4.47,   0.398,  14,     2.882,  0.3,    0.2,    5.816,  3.526,  1.38}},
    {"I_",     {1.382,  180,    4.5,    0.339,  15,     2.65,   0,      0.2,    6.822,  3.762,  1.333}},
    {"Xe4+4",  {1.267,  90,     4.404,  0.332,  12,     0.556,  0,      0.2,    7.595,  4.975,  2.459}},
    {"Cs",     {2.57,   180,    4.517,  0.045,  12,     1.573,  0,      0.1,    2.183,  1.711,  2.984}},
    {"Ba6+2",  {2.277,  90,     3.703,  0.364,  12,     2.727,  0,      0.1,    2.814,  2.396,  2.442}},
    {"La3+3",  {1.943,  109.47, 3.522,  0.017,  12,     3.3,    0,      0.1,    2.8355, 2.7415, 2.071}},
    {"Ce6+3",  {1.841,  90,     3.556,  0.013,  12,     3.3,    0,      0.1,    2.774,  2.692,  1.925}},
    {"Pr6+3",  {1.823,  90,     3.606,  0.01,   12,     3.3,    0,      0.1,    2.858,  2.564,  2.007}},
    {"Nd6+3",  {1.816,  90,     3.575,  0.01,   12,     3.3,    0,      0.1,    2.8685, 2.6205, 2.007}},
    {"Pm6+3",  {1.801,  90,     3.547,  0.009,  12,     3.3,    0,      0.1,    2.881,  2.673,  2}},
    {"Sm6+3",  {1.78,   90,     3.52,   0.008,  12,     3.3,    0,      0.1,    2.9115, 2.7195, 1.978}},
    {"Eu6+3",  {1.771,  90,     3.493,  0.008,  12,     3.3,    0,      0.1,    2.8785, 2.7875, 2.227}},
    {"Gd6+3",  {1.735,  90,     3.368,  0.009,  12,     3.3,    0,      0.1,    3.1665, 2.9745, 1.968}},
    {"Tb6+3",  {1.732,  90,     3.451,  0.007,  12,     3.3,    0,      0.1,    3.018,  2.834,  1.954}},
    {"Dy6+3",  {1.71,   90,     3.428,  0.007,  12,     3.3,    0,      0.1,    3.0555, 2.8715, 1.934}},
    {"Ho6+3",  {1.696,  90,     3.409,  0.007,  12,     3.416,  0,      0.1,    3.127,  2.891,  1.925}},
    {"Er6+3",  {1.673,  90,     3.391,  0.007,  12,     3.3,    0,      0.1,    3.1865, 2.9145, 1.915}},
    {"Tm6+3",  {1.66,   90,     3.374,  0.006,  12,     3.3,    0,      0.1,    3.2514, 2.9329, 2}},
    {"Yb6+3",  {1.637,  90,     3.355,  0.228,  12,     2.618,  0,      0.1,    3.2889, 2.965,  2.158}},
    {"Lu6+3",  {1.671,  90,     3.64,   0.041,  12,     3.271,  0,      0.1,    2.9629, 2.4629, 1.896}},
    {"Hf3+4",  {1.611,  109.47, 3.141,  0.072,  12,     3.921,  0,      0.1,    3.7,    3.4,    1.759}},
    {"Ta3+5",  {1.511,  109.47, 3.17,   0.081,  12,     4.075,  0,      0.1,    5.1,    2.85,   1.605}},
    {"W_3+4",  {1.526,  109.47, 3.069,  0.067,  12,     3.7,    0,      0.1,    4.63,   3.31,   1.538}},
    {"W_3+6",  {1.38,   109.47, 3.069,  0.067,  12,     3.7,    0,      0.1,    4.63,   3.31,   1.538}},
    {"W_6+6",  {1.392,  90,     3.069,  0.067,  12,     3.7,    0,      0.1,    4.63,   3.31,   1.538}},
    {"Re3+7",  {1.314,  109.47, 2.954,  0.066,  12,     3.7,    0,      0.1,    3.96,   3.92,   1.6}},
    {"Re6+5",  {1.372,  90,     2.954,  0.066,  12,     3.7,    0,      0.1,    3.96,   3.92,   1.6}},
    {"Os6+6",  {1.372,  90,     3.12,   0.037,  12,     3.7,    0,      0.1,    5.14,   3.63,   1.7}},
    {"Ir6+3",  {1.371,  90,     2.84,   0.073,  12,     3.731,  0,      0.1,    5,      4,      1.866}},
    {"Pt4+2",  {1.364,  90,     2.754,  0.08,   12,     3.382,  0,      0.1,    4.79,   4.43,   1.557}},
    {"Au4+3",  {1.262,  90,     3.293,  0.039,  12,     2.625,  0,      0.1,    4.894,  2.586,  1.618}},
    {"Hg1+2",  {1.34,   180,    2.705,  0.385,  12,     1.75,   0,      0.1,    6.27,   4.16,   1.6}},
    {"Tl3+3",  {1.518,  120,    4.347,  0.68,   11,     2.068,  0,      0.1,    3.2,    2.9,    1.53}},
    {"Pb3",    {1.459,  109.47, 4.297,  0.663,  12,     2.846,  0.1,    0.1,    3.9,    3.53,   1.444}},
    {"Bi3+3",  {1.512,  90,     4.37,   0.518,  13,     2.47,   1,      0.1,    4.69,   3.74,   1.514}},
    {"Po3+2",  {1.5,    90,     4.709,  0.325,  14,     2.33,   0.3,    0.1,    4.21,   4.21,   1.48}},
    {"At",     {1.545,  180,    4.75,   0.284,  15,     2.24,   0,      0.1,    4.75,   4.75,   1.47}},
    {"Rn4+4",  {1.42,   90,     4.765,  0.248,  16,     0.583,  0,      0.1,    5.37,   5.37,   2.2}},
    {"Fr",     {2.88,   180,    4.9,    0.05,   12,     1.847,  0,      0,      2,      2,      2.3}},
    {"Ra6+2",  {2.512,  90,     3.677,  0.404,  12,     2.92,   0,      0,      2.843,  2.434,  2.2}},
    {"Ac6+3",  {1.983,  90,     3.478,  0.033,  12,     3.9,    0,      0,      2.835,  2.835,  2.108}},
    {"Th6+4",  {1.721,  90,     3.396,  0.026,  12,     4.202,  0,      0,      3.175,  2.905,  2.018}},
    {"Pa6+4",  {1.711,  90,     3.424,  0.022,  12,     3.9,    0,      0,      2.985,  2.905,  1.8}},
    {"U_6+4",  {1.684,  90,     3.395,  0.022,  12,     3.9,    0,      0,      3.341,  2.853,  1.713}},
    {"Np6+4",  {1.666,  90,     3.424,  0.019,  12,     3.9,    0,      0,      3.549,  2.717,  1.8}},
    {"Pu6+4",  {1.657,  90,     3.424,  0.016,  12,     3.9,    0,      0,      3.243,  2.819,  1.84}},
    {"Am6+4",  {1.66,   90,     3.381,  0.014,  12,     3.9,    0,      0,      2.9895, 3.0035, 1.942}},
    {"Cm6+3",  {1.801,  90,     3.326,  0.013,  12,     3.9,    0,      0,      2.8315, 3.1895, 1.9}},
    {"Bk6+3",  {1.761,  90,     3.339,  0.013,  12,     3.9,    0,      0,      3.1935, 3.0355, 1.9}},
    {"Cf6+3",  {1.75,   90,     3.313,  0.013,  12,     3.9,    0,      0,      3.197,  3.101,  1.9}},
    {"Es6+3",  {1.724,  90,     3.299,  0.012,  12,     3.9,    0,      0,      3.333,  3.089,  1.9}},
    {"Fm6+3",  {1.712,  90,     3.286,  0.012,  12,     3.9,    0,      0,      3.4,    3.1,    1.9}},
    {"Md6+3",  {1.689,  90,     3.274,  0.011,  12,     3.9,    0,      0,      3.47,   3.11,   1.9}},
    {"No6+3",  {1.679,  90,     3.248,  0.011,  12,     3.9,    0,      0,      3.475,  3.175,  1.9}},
    {"Lw6+3",  {1.698,  90,     3.236,  0.011,  12,     3.9,    0,      0,      3.5,    3.2,    1.9}}
};

Molecule UFF_PrepareStep(const Step &s, const std::string &name){
    Molecule mol{s, name+" (UFF)"};
    auto &step = mol.getStep(0);

    // Step 1: create adjacency list from bond list, assign bond-order=1 cases
    using adjacency_node = std::vector<std::pair<size_t, size_t>>;
    auto adjacency_list = [](const Step &step)
    {
        std::vector<adjacency_node> coord(step.getNat());
        const auto& bonds = step.getBonds();
        for(size_t i=0; i<bonds.size(); ++i){
            const auto &b = bonds[i];
            coord[b.at1].emplace_back(b.at2, i);
            coord[b.at2].emplace_back(b.at1, i);
        }
        return coord;
    }(step);

    // Step 2: search for aromatic rings
    auto [aromatic_atoms, bond_aromaticity] = [](auto& al, const Step& step){
        std::set<size_t> aromatic_atoms;
        std::map<size_t, double> bond_aromaticity;
        if(al.empty()) return std::pair{std::move(aromatic_atoms), std::move(bond_aromaticity)};
        size_t target_size = 6;
        size_t current_size = 0;
        size_t cur_idx = 0;
        // store previous idx, current idx in previous adjacency list, and influence on ring size
        std::vector<std::tuple<size_t, size_t, int>> node_stack;
        // iterate over all atoms
        while(true){
            // check if the current atom could possibly be aromatic
            const auto pos = std::find_if(aromaticity_criteria.begin(), aromaticity_criteria.end(), [&](const auto& type){
                return (std::get<0>(type) == step[cur_idx].type->Z)
                    && (std::get<1>(type) == al[cur_idx].size());
            });
            /* abort-criteria:
             * - non aromatic atom
             * - aromatic atom, but size > target
             * - aromatic atom, size matches, but current atom != first atom
             * - aromatic atom, size too small, current atom already encountered
             *
             * success: aromatic atom, size matches, current atom == first atom
             * continue: aromatic atom, size too small, atom not encountered previously
             */
            if(pos != aromaticity_criteria.end()){
                if((current_size == target_size) &&
                   (cur_idx == std::get<0>(node_stack[0]))){
                    // found an aromatic ring, success
                    for(const auto &[idx, neigh, __]: node_stack){
                        // mark bond as aromatic
                        // (TODO: naive search, visited 2*target_size times, optimize?)
                        bond_aromaticity[al[idx][neigh].second] += 0.5/target_size;
                        // mark atom as aromatic
                        aromatic_atoms.insert(idx);
                    }
                }else if(current_size < target_size){
                    // too small
                    const auto tmp = std::find_if(node_stack.begin(), node_stack.end(),
                                                  [&](const auto &tuple){return std::get<0>(tuple) == cur_idx;});
                    if(tmp == node_stack.end()){
                        // is not ring, increase depth
                        current_size += 1;
                        target_size += std::get<2>(*pos);
                        node_stack.emplace_back(cur_idx, 0, std::get<2>(*pos));
                        cur_idx = al[cur_idx][0].first;
                        // skip regular index increment
                        continue;
                    }
                }
            }
            // query next atom on current or previous depth
            std::function<void(void)> getNextIdx = [&](){
                if(node_stack.empty()){
                    // next base atom
                    cur_idx++;
                }else{
                    size_t prev_idx = std::get<0>(node_stack.back());
                    size_t &neigh_idx = std::get<1>(node_stack.back());
                    int ring_change = std::get<2>(node_stack.back());
                    neigh_idx++;
                    if(neigh_idx >= al[prev_idx].size()){
                        // decrease stack depth
                        current_size -= 1;
                        target_size -= ring_change;
                        cur_idx = prev_idx;
                        node_stack.pop_back();
                        // recursively increase idx
                        getNextIdx();
                    }else{
                        // next neighbour
                        cur_idx = al[prev_idx][neigh_idx].first;
                    }
                }
            };
            getNextIdx();
            if(cur_idx >= al.size()){
                return std::pair{std::move(aromatic_atoms), std::move(bond_aromaticity)};
            }
        }
    }(adjacency_list, step);

    // Step 3: assign atom-types based on coordination and aromaticity
    for(size_t i=0; i<step.getNat(); ++i){
        auto atom = step[i];
        const auto &Z = atom.type->Z;
        auto param_pos = UFF_Parameters.find(atom.name);
        if(param_pos == UFF_Parameters.end()){
            // need to deduce atom type first
            auto element_pos = UFF_Criteria.find(Z);
            if(element_pos == UFF_Criteria.end()){
                throw Error{fmt::format("Could not deduce force-field type of atom {} of vipster type \"{}\" (Z {} is not parameterized)",
                                        i, atom.name.c_str(), Z)};
            }
            const auto& variants = element_pos->second;
            if(variants.size() == 1){
                // only one possibility
                atom.name = std::get<2>(*variants.begin());
            }else{
                // determine based on coordination and/or aromaticity
                const auto &coord = adjacency_list[i].size();
                const auto &aromatic = aromatic_atoms.find(i) != aromatic_atoms.end();
                auto variant_pos = std::find_if(variants.begin(), variants.end(), [&](const auto &tuple){
                    return (coord == std::get<0>(tuple)) // coordination must match
                        && ((std::get<1>(tuple) == -1) || std::get<1>(tuple) == aromatic); // aromaticity must match if not -1
                });
                if(variant_pos == variants.end()){
                    throw Error{fmt::format("Could not deduce force-field type of atom {} of vipster type {}"
                                            " (no parameter for coordination {} and aromaticity {})",
                                            i, atom.name.c_str(), coord, aromatic)};
                }
                atom.name = std::get<2>(*variant_pos);
            }
        }
        // clear charge
        atom.properties->charge = 0;
    }

    // Step 4: determine bond-orders
    for(size_t i=0; i<s.getNat(); ++i){
        /* applicable to:
         * B_3 (0.5 when bridged), B_2 (1!), VA: 3
         * C_1 (1, 2, 3), C_R (1, ~1.5), C_2(1, 2), VA: 4
         * N_1, N_2, N_3 (1), N_R (1, ~1.5), VA: 3
         * O_1 (3), O_2 (2), O_R (~1.5), VA: 2/3
         * P_3 (1, 2), VA: 3/5
         * S_2 (1, 2), S_R (~1.5), S_3 (1, 2), VA: 2/4/6
         */
        auto Z = s[i].type->Z;
    }
    return mol;
}

IO::Parameter UFF_PrepareParameters(const Step &s)
{
    /* TODO:
     * determine bond-order
     * needed for:
     * - r_BO (bond distance correction)
     * - D_ij (alpha in morse bond potential)
     * - V_sp2 (dihedral force constant)
     * => necessary to distinguish bond type according to atom's BO?
     */

    auto p = IO::LmpInput.makeParam();
    using coeffmap = std::map<std::string, std::string>;
    // create pair coeffs
    auto& paircoeffs = std::get<coeffmap>(p.at("Pair Coeff").first);
    for(const auto &t: s.getTypes()){
        auto pos = UFF_Parameters.find(t);
        if(pos == UFF_Parameters.end()){
            throw Error{fmt::format("UFF Parameters: {} is not a valid UFF atom type", t)};
        }
        const auto& tuple = pos->second;
        const double r2sigma = 1 / std::pow(2, 1/6);
        paircoeffs[t] = fmt::format("{} {}", std::get<3>(tuple), std::get<2>(tuple) * r2sigma);
    }
    // determine reference distances, create bond coeffs
    auto& bondcoeffs = std::get<coeffmap>(p.at("Bond Coeff").first);
    std::map<std::pair<size_t, size_t>, double> ref_dists{};
    for(const auto &b: s.getBonds()){        const std::string& name1 = s[b.at1].name;
        const std::string& name2 = s[b.at2].name;
        const std::string b_name = b.type ?
                    b.type->first :
                    fmt::format("{}-{}", std::min(name1, name2), std::max(name1, name2));
        // corrected reference distance
        double r_i = std::get<0>(UFF_Parameters.at(name1));
        double r_j = std::get<0>(UFF_Parameters.at(name2));
        // TODO:
//        double r_BO = -0.1332 * (r_i + r_j) * log(BO);
        double x_i = std::get<8>(UFF_Parameters.at(name1));
        double x_j = std::get<8>(UFF_Parameters.at(name2));
        double r_EN = r_i * r_j
                * (std::sqrt(x_i) - std::sqrt(x_j))
                * (std::sqrt(x_i) - std::sqrt(x_j))
                / (x_i * r_i + x_j * r_j);
        double r = r_i + r_j - r_EN; // + r_BO
        // store for later use
        ref_dists[{std::min(b.at1, b.at2), std::max(b.at1, b.at2)}] = r;
        // determine force constant, store parameters
        if(bondcoeffs.find(b_name) == bondcoeffs.end()){
            double k = 664.12
                     * std::get<5>(UFF_Parameters.at(name1))
                     * std::get<5>(UFF_Parameters.at(name2))
                     / (r * r * r);
            // TODO:
//            double D = BO * 70;
//            double alpha = std::sqrt(k/(2*D));
            bondcoeffs[b_name] = fmt::format("{} {}", 0.5*k, r); // harmonic
        }
    }
    auto [angles, dihedrals, impropers] = s.getTopology();
    auto& anglecoeffs = std::get<coeffmap>(p.at("Angle Coeff").first);
    for(const auto &a: angles){
        const std::string& name1 = s[a.at1].name;
        const std::string& name2 = s[a.at2].name;
        const std::string& name3 = s[a.at3].name;
        const std::string a_name = fmt::format("{}-{}-{}", std::min(name1, name3),
                                               name2, std::max(name1, name3));
        if(anglecoeffs.find(a_name) == anglecoeffs.end()){
            // force constant
            double theta0 = deg2rad * std::get<1>(UFF_Parameters.at(name2));
            double cos_theta0 = std::cos(theta0);
            double r_ij = ref_dists.at({std::min(a.at1, a.at2), std::max(a.at1, a.at2)});
            double r_jk = ref_dists.at({std::min(a.at3, a.at2), std::max(a.at3, a.at2)});
            double r_ik = r_ij * r_ij + r_jk * r_jk - 2 * r_ij * r_jk * std::cos(theta0);
            double Z_i = std::get<5>(UFF_Parameters.at(name1));
            double Z_k = std::get<5>(UFF_Parameters.at(name3));
            double k = 664.12 * Z_i * Z_k / std::pow(r_ik, 5) // beta reduced to 664.12 by eliminating the radii
                     * (3 * r_ij * r_jk * (1 - cos_theta0 * cos_theta0)
                        - r_ik * r_ik * cos_theta0);
            if(name2 == "C_1"){
                /* linear molecule: angle_style cosine
                 * other linear atom types either not parameterized or not correctly recognized (N_1?)
                 */
                anglecoeffs[a_name] = fmt::format("cosine {}", k);
            }else{
                // general case: angle_style fourier
                double c2 = 1 / (4 * std::sin(theta0) * std::sin(theta0));
                double c1 = -4 * c2 * cos_theta0;
                double c0 = c2 * (2 * cos_theta0 * cos_theta0 + 1);
                anglecoeffs[a_name] = fmt::format("fourier {} {} {} {}", k, c0, c1, c2);
            }
        }
    }
    auto& dihedcoeffs = std::get<coeffmap>(p.at("Dihedral Coeff").first);
    for(const auto &d: dihedrals){
        const std::string& name1 = s[d.at1].name;
        const std::string& name2 = s[d.at2].name;
        const std::string& name3 = s[d.at3].name;
        const std::string& name4 = s[d.at4].name;
        const std::string d_name = name1 < name4 ?
                    fmt::format("{}-{}-{}-{}", name1, name2, name3, name4)
                  : fmt::format("{}-{}-{}-{}", name4, name3, name2, name1);
        if(dihedcoeffs.find(d_name) == dihedcoeffs.end()){
            // uff formula: 0.5 * V * (1 - cos(n * phi_0) * cos(n * phi))
            // lammps formula: k * (1 + D * cos(n * phi))
            // -> k = 0.5 * V, D = -cos(n * phi_0)
            double k = 0;
            int D = 1;
            int n = 0;
            bool at2_sp3 = name2[2] == '3';
            bool at2_sp2 = name2[2] == '2' || name2[2] == 'R';
            bool at3_sp3 = name3[2] == '3';
            bool at3_sp2 = name3[2] == '2' || name3[2] == 'R';
            if(at2_sp3 && at3_sp3){
                const std::map<std::string, double> group6 = {{"O_", 2},
                                                              {"S_", 6.8},
                                                              {"Se", 6.8},
                                                              {"Te", 6.8},
                                                              {"Po", 6.8}};
                auto pos2 = group6.find(name2.substr(0,2));
                auto pos3 = group6.find(name3.substr(0,2));
                if((pos2 != group6.end()) && (pos3 != group6.end())){
                    // both atoms are of group6
                    k = 0.5 * std::sqrt(pos2->second * pos3->second);
                    n = 2;
                }else{
                    // general case
                    k = 0.5 * std::sqrt(std::get<6>(UFF_Parameters.at(name2)) *
                                        std::get<6>(UFF_Parameters.at(name3)));
                    n = 3;
                }
            }else if(at2_sp2 && at3_sp2){
                k = 2.5 * std::sqrt(std::get<7>(UFF_Parameters.at(name2))*
                                    std::get<7>(UFF_Parameters.at(name3)))
                // TODO: replace with exact expression when BO is known
                        * 3; //* (1 + 4.18 * ln(BO));
                D = -1;
                n = 2;
            }else if((at2_sp3 && at3_sp2) || (at2_sp2 && at3_sp3)){
                // sp2 + sp3
                k = 1;
                n = 3;
            }
            dihedcoeffs[d_name] = fmt::format("{} {} {}", k, D, n);
        }
    }
    auto& impropcoeffs = std::get<coeffmap>(p.at("Improper Coeff").first);
    for(const auto &i: impropers){
        const std::string& name1 = s[i.at1].name;
        std::array<std::string_view,3> names = {s[i.at2].name,
                                                s[i.at3].name,
                                                s[i.at4].name};
        std::sort(names.begin(), names.end());
        const std::string i_name = fmt::format("{}-{}-{}-{}", name1, names[0], names[1], names[2]);
        auto pos = impropcoeffs.find(i_name);
        if(pos == impropcoeffs.end()){
            int c0{}, c1{}, c2{};
            double k{};
            std::map<std::string, double> group5 = {{"P_", 84.4339},
                                                    {"As", 86.97305},
                                                    {"Sb", 87.7047},
                                                    {"Bi", 90.}};
            if(name1 == "C_2" || name1 == "C_R"){
                c0 = 1;
                c1 = -1;
                c2 = 0;
                if(names[0].substr(0,2) == "O_" || names[1].substr(0,2) == "O_" || names[2].substr(0,2) == "O_"){
                    k = 50./3.;
                }else{
                    k = 2;
                }
            }else if(auto pos = group5.find(name1.substr(0,2)); pos != group5.end()){
                const auto &omega = pos->second;
                c2 = 1;
                c1 = -4 * c2 * std::cos(omega);
                c0 = - (c1 * std::cos(omega) + c2 * std::cos(2 * omega));
                k = 22. / (3 * (c0 + c1 + c2));
            }
            impropcoeffs[i_name] = fmt::format("{} {} {} {} 1", k, c0, c1, c2);
        }
    }
    return p;
}

const ForceField UFF {
    "lj/charmm/coul/charmm 13.0 15.0",
    "harmonic",
    "hybrid fourier cosine",
    "harmonic",
    "fourier",
    {
        "special_bonds lj/coul 0.0 0.0 1.0",
        "pair_modify mix geometric",
    },
    &UFF_PrepareStep,
    &UFF_PrepareParameters
};
