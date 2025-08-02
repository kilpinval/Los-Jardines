// Secuencia típica de conmutación (6 pasos)
const unsigned char commutationTable[6] = {
    0x21, // Fase U+, V-
    0x29, // Fase U+, W-
    0x18, // Fase V+, W-
    0x0C, // Fase V+, U-
    0x14, // Fase W+, U-
    0x24  // Fase W+, V-
};
