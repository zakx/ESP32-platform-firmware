#include "include/driver_framebuffer.h"
const uint8_t org_016pt7bBitmaps[] = {
  0x00, 0xE8, 0xA0, 0x57, 0xD5, 0xF5, 0x00, 0xFD, 0x3E, 0x5F, 0x80, 0x88,
  0x88, 0x88, 0x80, 0xF4, 0xBF, 0x2E, 0x80, 0x80, 0x6A, 0x40, 0x95, 0x80,
  0xAA, 0x80, 0x5D, 0x00, 0xC0, 0xF0, 0x80, 0x08, 0x88, 0x88, 0x00, 0xFC,
  0x63, 0x1F, 0x80, 0xF8, 0xF8, 0x7F, 0x0F, 0x80, 0xF8, 0x7E, 0x1F, 0x80,
  0x8C, 0x7E, 0x10, 0x80, 0xFC, 0x3E, 0x1F, 0x80, 0xFC, 0x3F, 0x1F, 0x80,
  0xF8, 0x42, 0x10, 0x80, 0xFC, 0x7F, 0x1F, 0x80, 0xFC, 0x7E, 0x1F, 0x80,
  0x90, 0xB0, 0x2A, 0x22, 0xF0, 0xF0, 0x88, 0xA8, 0xF8, 0x4E, 0x02, 0x00,
  0xFD, 0x6F, 0x0F, 0x80, 0xFC, 0x7F, 0x18, 0x80, 0xF4, 0x7D, 0x1F, 0x00,
  0xFC, 0x21, 0x0F, 0x80, 0xF4, 0x63, 0x1F, 0x00, 0xFC, 0x3F, 0x0F, 0x80,
  0xFC, 0x3F, 0x08, 0x00, 0xFC, 0x2F, 0x1F, 0x80, 0x8C, 0x7F, 0x18, 0x80,
  0xF9, 0x08, 0x4F, 0x80, 0x78, 0x85, 0x2F, 0x80, 0x8D, 0xB1, 0x68, 0x80,
  0x84, 0x21, 0x0F, 0x80, 0xFD, 0x6B, 0x5A, 0x80, 0xFC, 0x63, 0x18, 0x80,
  0xFC, 0x63, 0x1F, 0x80, 0xFC, 0x7F, 0x08, 0x00, 0xFC, 0x63, 0x3F, 0x80,
  0xFC, 0x7F, 0x29, 0x00, 0xFC, 0x3E, 0x1F, 0x80, 0xF9, 0x08, 0x42, 0x00,
  0x8C, 0x63, 0x1F, 0x80, 0x8C, 0x62, 0xA2, 0x00, 0xAD, 0x6B, 0x5F, 0x80,
  0x8A, 0x88, 0xA8, 0x80, 0x8C, 0x54, 0x42, 0x00, 0xF8, 0x7F, 0x0F, 0x80,
  0xEA, 0xC0, 0x82, 0x08, 0x20, 0x80, 0xD5, 0xC0, 0x54, 0xF8, 0x80, 0xF1,
  0xFF, 0x8F, 0x99, 0xF0, 0xF8, 0x8F, 0x1F, 0x99, 0xF0, 0xFF, 0x8F, 0x6B,
  0xA4, 0xF9, 0x9F, 0x10, 0x8F, 0x99, 0x90, 0xF0, 0x55, 0xC0, 0x8A, 0xF9,
  0x90, 0xF8, 0xFD, 0x63, 0x10, 0xF9, 0x99, 0xF9, 0x9F, 0xF9, 0x9F, 0x80,
  0xF9, 0x9F, 0x20, 0xF8, 0x88, 0x47, 0x1F, 0x27, 0xC8, 0x42, 0x00, 0x99,
  0x9F, 0x99, 0x97, 0x8C, 0x6B, 0xF0, 0x96, 0x69, 0x99, 0x9F, 0x10, 0x2E,
  0x8F, 0x2B, 0x22, 0xF8, 0x89, 0xA8, 0x0F, 0xE0 };

const GFXglyph org_016pt7bGlyphs[] = {
  {     0,   1,   1,   7,    0,    0 },   // 0x20 ' '
  {     1,   1,   5,   2,    0,   -4 },   // 0x21 '!'
  {     2,   3,   1,   5,    0,   -4 },   // 0x22 '"'
  {     3,   5,   5,   7,    1,   -4 },   // 0x23 '#'
  {     7,   5,   5,   7,    1,   -4 },   // 0x24 '$'
  {    11,   5,   5,   7,    1,   -4 },   // 0x25 '%'
  {    15,   5,   5,   7,    1,   -4 },   // 0x26 '&'
  {    19,   1,   1,   2,    0,   -4 },   // 0x27 '''
  {    20,   2,   5,   3,    0,   -4 },   // 0x28 '('
  {    22,   2,   5,   3,    0,   -4 },   // 0x29 ')'
  {    24,   3,   3,   5,    0,   -3 },   // 0x2A '*'
  {    26,   3,   3,   5,    0,   -3 },   // 0x2B '+'
  {    28,   1,   2,   2,    0,    0 },   // 0x2C ','
  {    29,   4,   1,   6,    0,   -2 },   // 0x2D '-'
  {    30,   1,   1,   2,    0,    0 },   // 0x2E '.'
  {    31,   5,   5,   7,    1,   -4 },   // 0x2F '/'
  {    35,   5,   5,   7,    1,   -4 },   // 0x30 '0'
  {    39,   1,   5,   2,    0,   -4 },   // 0x31 '1'
  {    40,   5,   5,   7,    1,   -4 },   // 0x32 '2'
  {    44,   5,   5,   7,    1,   -4 },   // 0x33 '3'
  {    48,   5,   5,   7,    1,   -4 },   // 0x34 '4'
  {    52,   5,   5,   7,    1,   -4 },   // 0x35 '5'
  {    56,   5,   5,   7,    1,   -4 },   // 0x36 '6'
  {    60,   5,   5,   7,    1,   -4 },   // 0x37 '7'
  {    64,   5,   5,   7,    1,   -4 },   // 0x38 '8'
  {    68,   5,   5,   7,    1,   -4 },   // 0x39 '9'
  {    72,   1,   4,   2,    0,   -3 },   // 0x3A ':'
  {    73,   1,   4,   2,    0,   -3 },   // 0x3B ';'
  {    74,   3,   5,   5,    0,   -4 },   // 0x3C '<'
  {    76,   4,   3,   6,    0,   -3 },   // 0x3D '='
  {    78,   3,   5,   5,    0,   -4 },   // 0x3E '>'
  {    80,   5,   5,   7,    1,   -4 },   // 0x3F '?'
  {    84,   5,   5,   7,    1,   -4 },   // 0x40 '@'
  {    88,   5,   5,   7,    1,   -4 },   // 0x41 'A'
  {    92,   5,   5,   7,    1,   -4 },   // 0x42 'B'
  {    96,   5,   5,   7,    1,   -4 },   // 0x43 'C'
  {   100,   5,   5,   7,    1,   -4 },   // 0x44 'D'
  {   104,   5,   5,   7,    1,   -4 },   // 0x45 'E'
  {   108,   5,   5,   7,    1,   -4 },   // 0x46 'F'
  {   112,   5,   5,   7,    1,   -4 },   // 0x47 'G'
  {   116,   5,   5,   7,    1,   -4 },   // 0x48 'H'
  {   120,   5,   5,   7,    1,   -4 },   // 0x49 'I'
  {   124,   5,   5,   7,    1,   -4 },   // 0x4A 'J'
  {   128,   5,   5,   7,    1,   -4 },   // 0x4B 'K'
  {   132,   5,   5,   7,    1,   -4 },   // 0x4C 'L'
  {   136,   5,   5,   7,    1,   -4 },   // 0x4D 'M'
  {   140,   5,   5,   7,    1,   -4 },   // 0x4E 'N'
  {   144,   5,   5,   7,    1,   -4 },   // 0x4F 'O'
  {   148,   5,   5,   7,    1,   -4 },   // 0x50 'P'
  {   152,   5,   5,   7,    1,   -4 },   // 0x51 'Q'
  {   156,   5,   5,   7,    1,   -4 },   // 0x52 'R'
  {   160,   5,   5,   7,    1,   -4 },   // 0x53 'S'
  {   164,   5,   5,   7,    1,   -4 },   // 0x54 'T'
  {   168,   5,   5,   7,    1,   -4 },   // 0x55 'U'
  {   172,   5,   5,   7,    1,   -4 },   // 0x56 'V'
  {   176,   5,   5,   7,    1,   -4 },   // 0x57 'W'
  {   180,   5,   5,   7,    1,   -4 },   // 0x58 'X'
  {   184,   5,   5,   7,    1,   -4 },   // 0x59 'Y'
  {   188,   5,   5,   7,    1,   -4 },   // 0x5A 'Z'
  {   192,   2,   5,   3,    0,   -4 },   // 0x5B '['
  {   194,   5,   5,   7,    1,   -4 },   // 0x5C '\'
  {   198,   2,   5,   3,    0,   -4 },   // 0x5D ']'
  {   200,   3,   2,   5,    0,   -4 },   // 0x5E '^'
  {   201,   5,   1,   7,    1,    1 },   // 0x5F '_'
  {   202,   1,   1,   2,    0,   -4 },   // 0x60 '`'
  {   203,   4,   4,   6,    0,   -3 },   // 0x61 'a'
  {   205,   4,   5,   6,    0,   -4 },   // 0x62 'b'
  {   208,   4,   4,   6,    0,   -3 },   // 0x63 'c'
  {   210,   4,   5,   6,    0,   -4 },   // 0x64 'd'
  {   213,   4,   4,   6,    0,   -3 },   // 0x65 'e'
  {   215,   3,   5,   5,    0,   -4 },   // 0x66 'f'
  {   217,   4,   5,   6,    0,   -3 },   // 0x67 'g'
  {   220,   4,   5,   6,    0,   -4 },   // 0x68 'h'
  {   223,   1,   4,   2,    0,   -3 },   // 0x69 'i'
  {   224,   2,   5,   3,    0,   -3 },   // 0x6A 'j'
  {   226,   4,   5,   6,    0,   -4 },   // 0x6B 'k'
  {   229,   1,   5,   2,    0,   -4 },   // 0x6C 'l'
  {   230,   5,   4,   7,    1,   -3 },   // 0x6D 'm'
  {   233,   4,   4,   6,    0,   -3 },   // 0x6E 'n'
  {   235,   4,   4,   6,    0,   -3 },   // 0x6F 'o'
  {   237,   4,   5,   6,    0,   -3 },   // 0x70 'p'
  {   240,   4,   5,   6,    0,   -3 },   // 0x71 'q'
  {   243,   4,   4,   6,    0,   -3 },   // 0x72 'r'
  {   245,   4,   4,   6,    0,   -3 },   // 0x73 's'
  {   247,   5,   5,   7,    1,   -4 },   // 0x74 't'
  {   251,   4,   4,   6,    0,   -3 },   // 0x75 'u'
  {   253,   4,   4,   6,    0,   -3 },   // 0x76 'v'
  {   255,   5,   4,   7,    1,   -3 },   // 0x77 'w'
  {   258,   4,   4,   6,    0,   -3 },   // 0x78 'x'
  {   260,   4,   5,   6,    0,   -3 },   // 0x79 'y'
  {   263,   4,   4,   6,    0,   -3 },   // 0x7A 'z'
  {   265,   3,   5,   5,    0,   -4 },   // 0x7B '{'
  {   267,   1,   5,   2,    0,   -4 },   // 0x7C '|'
  {   268,   3,   5,   5,    0,   -4 },   // 0x7D '}'
  {   270,   5,   3,   7,    1,   -3 } }; // 0x7E '~'

const GFXfont org_016pt7b = {
  (uint8_t  *)org_016pt7bBitmaps,
  (GFXglyph *)org_016pt7bGlyphs,
  0x20, 0x7E, 13 };

// Approx. 944 bytes
