

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <raylib.h>


/// prototypes

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 320;
const int INST_PER_FRAME = 10;
const int TOTAL_INST_TO_RUN = 100000;
const int TARGET_FPS = 60;

enum operation {
    OP_0NNN,
    OP_00CN,
    OP_00E0,
    OP_00EE,
    OP_00FB,
    OP_00FC,
    OP_00FD,
    OP_00FE,
    OP_00FF,
    OP_1NNN,
    OP_2NNN,
    OP_3XNN,
    OP_4XNN,
    OP_5XY0,
    OP_6XNN,
    OP_7XNN,
    OP_8XY0,
    OP_8XY1,
    OP_8XY2,
    OP_8XY3,
    OP_8XY4,
    OP_8XY5,
    OP_8XY7,
    OP_8XY6,
    OP_8XYE,
    OP_9XY0,
    OP_ANNN,
    OP_BNNN,
    OP_CXNN,
    OP_DXYN,
    OP_EX9E,
    OP_EXA1,
    OP_FX07,
    OP_FX0A,
    OP_FX15,
    OP_FX18,
    OP_FX1E,
    OP_FX29,
    OP_FX30,
    OP_FX33,
    OP_FX55,
    OP_FX65,
    OP_FX75,
    OP_FX85,
};


struct settings {
    bool superChip;
};

struct inst {
    uint16_t raw;
    enum operation operation;
    uint16_t nnn;
    uint8_t x, y, nn, n;
    bool isValid;
};

struct state {
    uint8_t memory[4096];
    bool numpad[16];
    bool display[128][64];
    uint16_t regI, regPC;
    uint8_t regV[16], regSP, delayTimer, soundTimer;
    bool is128x64mode;
    uint8_t storage[8];
    bool storageHasChanged, displayHasChanged;
};

typedef void (*instHandler) ( struct state *, struct inst *);

void execute00CN ( struct state *state, struct inst *inst);
void execute00E0 ( struct state *state, struct inst *inst);
void execute00EE ( struct state *state, struct inst *inst);
void execute00FB ( struct state *state, struct inst *inst);
void execute00FC ( struct state *state, struct inst *inst);
void execute00FD ( struct state *state, struct inst *inst);
void execute00FE ( struct state *state, struct inst *inst);
void execute00FF ( struct state *state, struct inst *inst);
void execute0NNN ( struct state *state, struct inst *inst);
void execute1NNN ( struct state *state, struct inst *inst);
void execute2NNN ( struct state *state, struct inst *inst);
void execute3XNN ( struct state *state, struct inst *inst);
void execute4XNN ( struct state *state, struct inst *inst);
void execute5XY0 ( struct state *state, struct inst *inst);
void execute6XNN ( struct state *state, struct inst *inst);
void execute7XNN ( struct state *state, struct inst *inst);
void execute8XY0 ( struct state *state, struct inst *inst);
void execute8XY1 ( struct state *state, struct inst *inst);
void execute8XY2 ( struct state *state, struct inst *inst);
void execute8XY3 ( struct state *state, struct inst *inst);
void execute8XY4 ( struct state *state, struct inst *inst);
void execute8XY5 ( struct state *state, struct inst *inst);
void execute8XY7 ( struct state *state, struct inst *inst);
void execute8XY6 ( struct state *state, struct inst *inst);
void execute8XYE ( struct state *state, struct inst *inst);
void execute9XY0 ( struct state *state, struct inst *inst);
void executeANNN ( struct state *state, struct inst *inst);
void executeBNNN ( struct state *state, struct inst *inst);
void executeCXNN ( struct state *state, struct inst *inst);
void executeDXYN ( struct state *state, struct inst *inst);
void executeEX9E ( struct state *state, struct inst *inst);
void executeEXA1 ( struct state *state, struct inst *inst);
void executeFX07 ( struct state *state, struct inst *inst);
void executeFX0A ( struct state *state, struct inst *inst);
void executeFX15 ( struct state *state, struct inst *inst);
void executeFX18 ( struct state *state, struct inst *inst);
void executeFX1E ( struct state *state, struct inst *inst);
void executeFX29 ( struct state *state, struct inst *inst);
void executeFX30 ( struct state *state, struct inst *inst);
void executeFX33 ( struct state *state, struct inst *inst);
void executeFX55 ( struct state *state, struct inst *inst);
void executeFX65 ( struct state *state, struct inst *inst);
void executeFX75 ( struct state *state, struct inst *inst);
void executeFX85 ( struct state *state, struct inst *inst);

char* operationToEncoding( enum operation operation);
void printInst (struct inst inst);
bool instMatches( uint16_t inst, char encoding[4] );
struct inst decodeInst ( uint16_t inst );
uint8_t getNthBit(uint8_t byte, uint8_t n);
void newMemory(uint8_t *destAddr, uint8_t* rom, int romLen);
int loadFromMem( uint8_t *mem, uint16_t addr);
int storeToMem( uint8_t *mem, uint16_t addr, uint8_t val);
void loadPermanentRegisters( uint8_t *destAddr );
void savePermanentRegisters( uint8_t *regs);
void newState( struct state *destAddr);
uint16_t fetchInst( uint8_t* mem, uint16_t pc);
instHandler findHandler( enum operation operation);
void updateNumpad( bool *numpad );
void makeSound(void);
void drawDisplay( struct state *state );
void printState( struct state *state );
void printMemory( uint8_t memory[4096]);

struct {
    char encoding[5];
    enum operation operation;
    instHandler handler;

} encodingOperationMap[] = {
    {"00CN",  OP_00CN, execute00CN},
    {"00E0",  OP_00E0, execute00E0},
    {"00EE",  OP_00EE, execute00EE},
    {"00FB",  OP_00FB, execute00FB},
    {"00FC",  OP_00FC, execute00FC},
    {"00FD",  OP_00FD, execute00FD},
    {"00FE",  OP_00FE, execute00FE},
    {"00FF",  OP_00FF, execute00FF},
    {"0NNN",  OP_0NNN, execute0NNN}, // this must be the last 0... one
    {"1NNN", OP_1NNN, execute1NNN},
    {"2NNN", OP_2NNN, execute2NNN},
    {"3XNN", OP_3XNN, execute3XNN},
    {"4XNN", OP_4XNN, execute4XNN},
    {"5XY0", OP_5XY0, execute5XY0},
    {"6XNN", OP_6XNN, execute6XNN},
    {"7XNN", OP_7XNN, execute7XNN},
    {"8XY0", OP_8XY0, execute8XY0},
    {"8XY1", OP_8XY1, execute8XY1},
    {"8XY2", OP_8XY2, execute8XY2},
    {"8XY3", OP_8XY3, execute8XY3},
    {"8XY4", OP_8XY4, execute8XY4},
    {"8XY5", OP_8XY5, execute8XY5},
    {"8XY7", OP_8XY7, execute8XY7},
    {"8XY6", OP_8XY6, execute8XY6},
    {"8XYE", OP_8XYE, execute8XYE},
    {"9XY0", OP_9XY0, execute9XY0},
    {"ANNN", OP_ANNN, executeANNN},
    {"BNNN", OP_BNNN, executeBNNN},
    {"CXNN", OP_CXNN, executeCXNN},
    {"DXYN", OP_DXYN, executeDXYN},
    {"EX9E", OP_EX9E, executeEX9E},
    {"EXA1", OP_EXA1, executeEXA1},
    {"FX07", OP_FX07, executeFX07},
    {"FX0A", OP_FX0A, executeFX0A},
    {"FX15", OP_FX15, executeFX15},
    {"FX18", OP_FX18, executeFX18},
    {"FX1E", OP_FX1E, executeFX1E},
    {"FX29", OP_FX29, executeFX29},
    {"FX30", OP_FX30, executeFX30},
    {"FX33", OP_FX33, executeFX33},
    {"FX55", OP_FX55, executeFX55},
    {"FX65", OP_FX65, executeFX65},
    {"FX75", OP_FX75, executeFX75},
    {"FX85", OP_FX85, executeFX85},
};

const int ENCODING_OPERATION_MAP_LENGTH = 44;


struct settings settings = {0};




char* operationToEncoding( enum operation operation) {
    for (int i = 0; i<ENCODING_OPERATION_MAP_LENGTH; i++) {
        if (encodingOperationMap[i].operation == operation)
            return encodingOperationMap[i].encoding;
    }
    return NULL;
}

void printInst (struct inst inst) {
    printf("inst:\n");
    printf("    raw: %X\n", inst.raw);
    printf("    isValid: %d\n", inst.isValid);
    if (!inst.isValid) {
        puts("");
        return;
    }

    printf("    operation: %s\n", operationToEncoding(inst.operation) );
    printf("    nnn: %X\n", inst.nnn);
    printf("    nn: %X\n", inst.nn);
    printf("    n: %X\n", inst.n);
    printf("    x: %X\n", inst.x);
    printf("    y: %X\n", inst.y);
    puts("");
    return;
}



bool instMatches( uint16_t inst, char encoding[4] ) {
    uint8_t nibbles[4];
    nibbles[0] = (inst & 0xF000) >> 12;
    nibbles[1] = (inst & 0x0F00) >> 8;
    nibbles[2] = (inst & 0x00F0) >> 4;
    nibbles[3] = (inst & 0x000F) >> 0;

    for (int i = 0; i<4; i++) {
        if (encoding[i] >= '0' && encoding[i] <= '9') {
            if ( encoding[i] - '0' != nibbles[i]) {
                return false;
            }
        }
        else if (encoding[i] >= 'A' && encoding[i] <= 'F') {
            if ( encoding[i] - 'A' + 10 != nibbles[i]) {
                return false;
            }
        }
    }

    return true;
}


struct inst decodeInst ( uint16_t inst ) {

    bool encodingFound = false;
    int i = 0;
    for ( ; i<ENCODING_OPERATION_MAP_LENGTH; i++) {
        if ( instMatches(inst, encodingOperationMap[i].encoding)) {
            encodingFound = true;
            break;
        }
    }

    if (!encodingFound) {
        return (struct inst) { .isValid=false, .raw=inst };
    }

    return (struct inst) {
        .isValid = true,
        .raw = inst,
        .operation = encodingOperationMap[i].operation,
        .nnn = inst & 0xFFF,
        .nn = inst & 0xFF,
        .n = inst & 0xF,
        .x = (inst & 0x0F00) >> 8,
        .y = (inst & 0x00F0) >> 4
    };    
}


/*
    Settings:
    BNNN jump inst interpreted as BNNN or BXNN?
    Shifts are done in-place or copy from VY?
    Set carry if I>0xFFF on inst FX1E?
    FX0A wait until keydown or keyrelease?
    Increment I at load insts?
    Increment I at store insts?
    Display with N=0 is high-res?

*/

/*
    Unclear:
    The stored BCD is always 3 bytes? or 1-3 bytes?
    Increase I at display inst?
*/

/*
    Useful Links:
    https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
    http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    http://johnearnest.github.io/Octo/docs/SuperChip.html

*/

/*
    Computers:
    - Cosmac Vip
    - Dream 6800
    - Eti 660
    - HP48 (Chip-48)
*/

/*
    Assumptions:
    - Stack starts at address 0 and grows up to 32.
      If it overflows, we do runtime error and exit.
      SP points to the empty slot.
    - Small font is at [0x50 - 0x9F]
    - Large font is at [0x100 - 0x19F]
*/

/*
                                                    .NNN    .X..    ..Y.    ..NN    ...N
0NNN    Exec machine code                           +
00CN    Scroll down 0-15 pixels                                                     +
00E0    Clear Screen                                
00EE    Return subroutine                           
00FB    Scroll right 4 pixels
00FC    Scroll left 4 pixels
00FD    Exit interpreter
00FE    Enter 64x32 mode
00FF    Enter 128x64 mode
1NNN    Jmp                                         +
2NNN    Call subroutine                             +
3XNN    Skip if VX==NN                                      +               +
4XNN    Skip if VX!=NN                                      +               +
5XY0    Skip if VX==VY                                      +       +
6XNN    VX=NN                                               +               +
7XNN    VX+=NN (no carry)                                   +               +
8XY0    VX=VY                                               +       +
8XY1    VX|=VY                                              +       +
8XY2    VX&=VY                                              +       +
8XY3    VX^=VY                                              +       +
8XY4    VX+=VY (carry)                                      +       +
8XY5    VX-=VY (carry)                                      +       +
8XY7    VX=VY-VX (carry)                                    +       +
8XY6    VX=VY (optional); VX>>=1 (carry)                    +       +
8XYE    VX=VY (optional); VX<<=1 (carry)                    +       +
9XY0    Skip if VX!=VY                                      +       +
ANNN    I=NNN                                       +
BNNN    Jump to NNN+V0                              +
BXNN!   Jump to X*100+NN+VX                         +       +
CXNN    VX=random8bit()&NN                                  +       +
DXY0!   Display 16x16 sprites (superchip)                   +       +
DXYN    Display (...) (carry)                               +       +               +
EX9E    Skip if num[VX]==pressed                            +
EXA1    Skip if num[VX]!=pressed                            +
FX07    VX=delayTimer                                       +
FX0A    Block until keypress then put in VX                 +
FX15    delayTimer=VX                                       +
FX18    soundTimer=VX                                       +
FX1E    I+=VX (optional carry if I>0xFFF)                   +
FX29    Set I to addr where VX char is stored               +
FX30    Same as FX29, but 8x10 sprites                      +
FX33    Store VX in mem[I..I+2] as BCD                      +
FX55    mem[I..] = V0..X                                    +
FX65    V0..X = mem[I..]                                    +
FX75    storage <= V0..X (x<8)                              +
FX85    V0..X <= storage (x<8)                              +
*/

uint8_t getNthBit(uint8_t byte, uint8_t n) {
    // we define 0th bit to be LSB bit.
    return (byte>>n) & 1;
}

const int SMALL_FONT_ADDRESS = 0x50;
const int LARGE_FONT_ADDRESS = 0x100;

void newMemory(uint8_t *destAddr, uint8_t* rom, int romLen) {
    // assumes that destAddr has 4096-byte allocated space for us to fill in

    if (romLen > 0xFFFF - 0x200 + 1) {
        fprintf(stderr, "Provided rom is too large.\n");
        exit(1);
    }

    const uint8_t smallFont[16][5] = {
        { // 0
            0xF0, // 11110000
            0x90, // 10010000
            0x90, // 10010000
            0x90, // 10010000
            0xF0  // 11110000
        },
        { // 1
            0x20, // 00100000
            0x60, // 01100000
            0x20, // 00100000
            0x20, // 00100000
            0x70  // 01110000
        },
        { // 2
            0xF0, // 11110000
            0x10, // 00010000
            0xF0, // 11110000
            0x80, // 10000000
            0xF0  // 11110000
        },
        { // 3
            0xF0, // 11110000
            0x10, // 00010000
            0xF0, // 11110000
            0x10, // 00010000
            0xF0  // 11110000
        },
        { // 4
            0x90, // 10010000
            0x90, // 10010000
            0xF0, // 11110000
            0x10, // 00010000
            0x10  // 00010000
        },
        { // 5
            0xF0, // 11110000
            0x80, // 10000000
            0xF0, // 11110000
            0x10, // 00010000
            0xF0  // 11110000
        },
        { // 6
            0xF0, // 11110000
            0x80, // 10000000
            0xF0, // 11110000
            0x90, // 10010000
            0xF0  // 11110000
        },
        { // 7
            0xF0, // 11110000
            0x10, // 00010000
            0x20, // 00100000
            0x40, // 01000000
            0x40  // 01000000
        },
        { // 8
            0xF0, // 11110000
            0x90, // 10010000
            0xF0, // 11110000
            0x90, // 10010000
            0xF0  // 11110000
        },
        { // 9
            0xF0, // 11110000
            0x90, // 10010000
            0xF0, // 11110000
            0x10, // 00010000
            0xF0  // 11110000
        },
        { // A
            0xF0, // 11110000
            0x90, // 10010000
            0xF0, // 11110000
            0x90, // 10010000
            0x90  // 10010000
        },
        { // B
            0xE0, // 11100000
            0x90, // 10010000
            0xE0, // 11100000
            0x90, // 10010000
            0xE0  // 11100000
        },
        { // C
            0xF0, // 11110000
            0x80, // 10000000
            0x80, // 10000000
            0x80, // 10000000
            0xF0  // 11110000
        },
        { // D
            0xE0, // 11100000
            0x90, // 10010000
            0x90, // 10010000
            0x90, // 10010000
            0xE0  // 11100000
        },
        { // E
            0xF0, // 11110000
            0x80, // 10000000
            0xF0, // 11110000
            0x80, // 10000000
            0xF0  // 11110000
        },
        { // F
            0xF0, // 11110000
            0x80, // 10000000
            0xF0, // 11110000
            0x80, // 10000000
            0x80  // 10000000
        }
    };

    const uint8_t largeFont[16][10] = {
        { // 0
            0x3C, // 00111100
            0x42, // 01000010
            0x42, // 01000010
            0x42, // 01000010
            0x42, // 01000010
            0x42, // 01000010
            0x42, // 01000010
            0x3C, // 00111100
            0x00, // 00000000
            0x00  // 00000000
        },
        { // 1
            0x08, // 00001000
            0x18, // 00011000
            0x28, // 00101000
            0x48, // 01001000
            0x08, // 00001000
            0x08, // 00001000
            0x08, // 00001000
            0x7E, // 01111110
            0x00, // 00000000
            0x00  // 00000000
        },
        { // 2
            0x3C, // 00111100
            0x42, // 01000010
            0x02, // 00000010
            0x04, // 00000100
            0x08, // 00001000
            0x10, // 00010000
            0x20, // 00100000
            0x7E, // 01111110
            0x00, // 00000000
            0x00  // 00000000
        },
        { // 3
            0x3C, // 00111100
            0x42, // 01000010
            0x02, // 00000010
            0x1C, // 00011100
            0x02, // 00000010
            0x02, // 00000010
            0x42, // 01000010
            0x3C, // 00111100
            0x00, // 00000000
            0x00  // 00000000
        },
        { // 4
            0x04, // 00000100
            0x0C, // 00001100
            0x14, // 00010100
            0x24, // 00100100
            0x44, // 01000100
            0x7E, // 01111110
            0x04, // 00000100
            0x04, // 00000100
            0x00, // 00000000
            0x00  // 00000000
        },
        { // 5
            0x7E, // 01111110
            0x40, // 01000000
            0x40, // 01000000
            0x7C, // 01111100
            0x02, // 00000010
            0x02, // 00000010
            0x42, // 01000010
            0x3C, // 00111100
            0x00, // 00000000
            0x00  // 00000000
        },
        { // 6
            0x3C, // 00111100
            0x42, // 01000010
            0x40, // 01000000
            0x7C, // 01111100
            0x42, // 01000010
            0x42, // 01000010
            0x42, // 01000010
            0x3C, // 00111100
            0x00, // 00000000
            0x00  // 00000000
        },
        { // 7
            0x7E, // 01111110
            0x02, // 00000010
            0x04, // 00000100
            0x08, // 00001000
            0x10, // 00010000
            0x20, // 00100000
            0x40, // 01000000
            0x40, // 01000000
            0x00, // 00000000
            0x00  // 00000000
        },
        { // 8
            0x3C, // 00111100
            0x42, // 01000010
            0x42, // 01000010
            0x3C, // 00111100
            0x42, // 01000010
            0x42, // 01000010
            0x42, // 01000010
            0x3C, // 00111100
            0x00, // 00000000
            0x00  // 00000000
        },
        { // 9
            0x3C, // 00111100
            0x42, // 01000010
            0x42, // 01000010
            0x3E, // 00111110
            0x02, // 00000010
            0x02, // 00000010
            0x42, // 01000010
            0x3C, // 00111100
            0x00, // 00000000
            0x00  // 00000000
        },
        { // A
            0x18, // 00011000
            0x24, // 00100100
            0x42, // 01000010
            0x42, // 01000010
            0x7E, // 01111110
            0x42, // 01000010
            0x42, // 01000010
            0x42, // 01000010
            0x00, // 00000000
            0x00  // 00000000
        },
        { // B
            0x7C, // 01111100
            0x42, // 01000010
            0x42, // 01000010
            0x7C, // 01111100
            0x42, // 01000010
            0x42, // 01000010
            0x42, // 01000010
            0x7C, // 01111100
            0x00, // 00000000
            0x00  // 00000000
        },
        { // C
            0x3C, // 00111100
            0x42, // 01000010
            0x40, // 01000000
            0x40, // 01000000
            0x40, // 01000000
            0x40, // 01000000
            0x42, // 01000010
            0x3C, // 00111100
            0x00, // 00000000
            0x00  // 00000000
        },
        { // D
            0x78, // 01111000
            0x44, // 01000100
            0x42, // 01000010
            0x42, // 01000010
            0x42, // 01000010
            0x42, // 01000010
            0x44, // 01000100
            0x78, // 01111000
            0x00, // 00000000
            0x00  // 00000000
        },
        { // E
            0x7E, // 01111110
            0x40, // 01000000
            0x40, // 01000000
            0x7C, // 01111100
            0x40, // 01000000
            0x40, // 01000000
            0x40, // 01000000
            0x7E, // 01111110
            0x00, // 00000000
            0x00  // 00000000
        },
        { // F
            0x7E, // 01111110
            0x40, // 01000000
            0x40, // 01000000
            0x7C, // 01111100
            0x40, // 01000000
            0x40, // 01000000
            0x40, // 01000000
            0x40, // 01000000
            0x00, // 00000000
            0x00  // 00000000
        }
    };

    memcpy(&destAddr[SMALL_FONT_ADDRESS], smallFont, 5*16);
    memcpy(&destAddr[LARGE_FONT_ADDRESS], largeFont, 10*16);
    memcpy(&destAddr[0x200], rom, romLen);
    }

int loadFromMem( uint8_t *mem, uint16_t addr) {
    if (addr > 0xFFF) {
        fprintf(stderr, "Tried to load from %X\n", addr);
        exit(1); // remove later
        return -1;
    }
    return mem[addr];
}

int storeToMem( uint8_t *mem, uint16_t addr, uint8_t val) {
    if (addr > 0xFFF) {
        fprintf(stderr, "Tried to store to %X\n", addr);
        exit(1); // remove later
        return -1;
    }
    mem[addr] = val;
    return 0;
}







void execute0NNN( struct state *state, struct inst *inst) {
    // TODO
    fprintf(stderr, "0NNN not supported.\n");
    exit(1);
}



void execute00CN( struct state *state, struct inst *inst) {
    // Scroll down 0-15 pixels
    if (!settings.superChip) {
        fprintf(stderr, "00CN not supported on original CHIP-8.\n");
        exit(1);
    }

    const int n = inst->n; // scroll amount
    for (int x = 0; x<128; x++) {
        for (int y = 0; y<64-n; y++) {
            state->display[x][63-y] = state->display[x][63-y-n];
        }
    }
    for (int x = 0; x<128; x++) {
        for (int y = 0; y<n; y++) {
            state->display[x][y] = 0;
        }
    }
    state->displayHasChanged = true;
    state->regPC += 2;
}



void execute00E0( struct state *state, struct inst *inst) {
    // Clear Screen
    memset(&state->display, 0, 128*64);
    state->displayHasChanged = true;
    state->regPC += 2;
}



void execute00EE( struct state *state, struct inst *inst) {
    // Return subroutine
    if (state->regSP == 0) {
        fprintf(stderr, "Stack underflow.\n");
        exit(1);
    }

    state->regSP -= 2;
    uint8_t highNibble = state->memory[state->regSP];
    uint8_t lowNibble = state->memory[state->regSP+1];
    state->regPC = highNibble<<8 | lowNibble;
}



void execute00FB( struct state *state, struct inst *inst) {
    // Scroll right 4 pixels
    if (!settings.superChip) {
        fprintf(stderr, "00FB not supported on original CHIP-8.\n");
        exit(1);
    }

    if (state->is128x64mode) {
        for (int x = 0; x<124; x++) {
            for (int y = 0; y<64; y++) {
                state->display[127-x][y] = state->display[123-x][y];
            }
        }
        for (int x = 0; x<4; x++) {
            for (int y = 0; y<64; y++) {
                state->display[x][y] = 0;
            }
        }
    }
    else { // 64x32 mode
        for (int x = 0; x<60; x++) {
            for (int y = 0; y<32; y++) {
                state->display[63-x][y] = state->display[59-x][y];
            }
        }
        for (int x = 0; x<4; x++) {
            for (int y = 0; y<32; y++) {
                state->display[x][y] = 0;
            }
        }
    }
    state->displayHasChanged = true;
    state->regPC += 2;
}



void execute00FC( struct state *state, struct inst *inst) {
    // Scroll left 4 pixels
    if (!settings.superChip) {
        fprintf(stderr, "00FC not supported on original CHIP-8.\n");
        exit(1);
    }

    if (state->is128x64mode) {
        for (int x = 0; x<124; x++) {
            for (int y = 0; y<64; y++) {
                state->display[x][y] = state->display[x+4][y];
            }
        }
        for (int x = 0; x<4; x++) {
            for (int y = 0; y<64; y++) {
                state->display[x+124][y] = 0;
            }
        }
    }
    else { // 64x32 mode
        for (int x = 0; x<60; x++) {
            for (int y = 0; y<32; y++) {
                state->display[63-x][y] = state->display[59-x][y];
            }
        }
        for (int x = 0; x<4; x++) {
            for (int y = 0; y<32; y++) {
                state->display[x][y] = 0;
            }
        }
    }
    state->displayHasChanged = true;
    state->regPC += 2;
}



void execute00FD( struct state *state, struct inst *inst) {
    // Exit interpreter
    if (!settings.superChip) {
        fprintf(stderr, "00FD not supported on original CHIP-8.\n");
        exit(1);
    }
    
    puts("Exiting...");
    exit(0);
}



void execute00FE( struct state *state, struct inst *inst) {
    // Enter 64x32 mode
    if (!settings.superChip) {
        fprintf(stderr, "00FE not supported on original CHIP-8.\n");
        exit(1);
    }

    state->is128x64mode = false;
    memset(&state->display, 0, 128*64);
    state->displayHasChanged = true;
    state->regPC += 2;
}



void execute00FF( struct state *state, struct inst *inst) {
    // Enter 128x64 mode
    if (!settings.superChip) {
        fprintf(stderr, "00FF not supported on original CHIP-8.\n");
        exit(1);
    }

    state->is128x64mode = true;
    memset(&state->display, 0, 128*64);
    state->displayHasChanged = true;
    state->regPC += 2;
}



void execute1NNN( struct state *state, struct inst *inst) {
    // Jmp
    state->regPC = inst->nnn;
}



void execute2NNN( struct state *state, struct inst *inst) {
    // Call subroutine
    if (state->regSP >= 32) {
        fprintf(stderr, "Stack overflow (max 32 bytes).\n");
        exit(1);
    }
    const uint16_t returnAddr = state->regPC + 2;
    state->memory[state->regSP] = returnAddr >> 8;
    state->memory[state->regSP+1] = returnAddr & 0xFF;
    state->regPC = inst->nnn;
    state->regSP += 2;
}



void execute3XNN( struct state *state, struct inst *inst) {
    // Skip if VX==NN
    if (state->regV[inst->x]==inst->nn) {
        state->regPC+=2;
    }
    state->regPC += 2;
}



void execute4XNN( struct state *state, struct inst *inst) {
    // Skip if VX!=NN
    if (state->regV[inst->x]!=inst->nn) {
        state->regPC+=2;
    }
    state->regPC += 2;
}



void execute5XY0( struct state *state, struct inst *inst) {
    // Skip if VX==VY
    if (state->regV[inst->x]==state->regV[inst->y]) {
        state->regPC+=2;
    }
    state->regPC += 2;
}



void execute6XNN( struct state *state, struct inst *inst) {
    // VX=NN
    state->regV[inst->x] = inst->nn;
    state->regPC += 2;
}



void execute7XNN( struct state *state, struct inst *inst) {
    // VX+=NN (no carry)
    state->regV[inst->x] += inst->nn;
    state->regPC += 2;
}



void execute8XY0( struct state *state, struct inst *inst) {
    // VX=VY
    state->regV[inst->x] = state->regV[inst->y];
    state->regPC += 2;
}



void execute8XY1( struct state *state, struct inst *inst) {
    // VX|=VY
    state->regV[inst->x] |= state->regV[inst->y];
    state->regPC += 2;
}



void execute8XY2( struct state *state, struct inst *inst) {
    // VX&=VY
    state->regV[inst->x] &= state->regV[inst->y];
    state->regPC += 2;
}



void execute8XY3( struct state *state, struct inst *inst) {
    // VX^=VY
    state->regV[inst->x] ^= state->regV[inst->y];
    state->regPC += 2;
}



void execute8XY4( struct state *state, struct inst *inst) {
    // VX+=VY (carry)
    // TODO what happens if X=15?
    state->regV[15] = state->regV[inst->x] + state->regV[inst->y] > 255;
    state->regV[inst->x] += state->regV[inst->y];
    state->regPC += 2;
}



void execute8XY5( struct state *state, struct inst *inst) {
    // VX-=VY (carry)
    // TODO what happens if X=15?
    state->regV[15] = state->regV[inst->x] >= state->regV[inst->y];
    state->regV[inst->x] -= state->regV[inst->y];
    state->regPC += 2;
}



void execute8XY7( struct state *state, struct inst *inst) {
    // VX=VY-VX (carry)
    // TODO what happens if X=15?
    state->regV[15] = state->regV[inst->y] >= state->regV[inst->x];
    state->regV[inst->x] = state->regV[inst->y] - state->regV[inst->x];
    state->regPC += 2;
}



void execute8XY6( struct state *state, struct inst *inst) {
    // VX=VY (optional); VX>>=1 (carry)
    if (!settings.superChip) {
        state->regV[inst->x] = state->regV[inst->y];
    }
    state->regV[15] = state->regV[inst->x] & 1;
    state->regV[inst->x] >>= 1;
    state->regPC += 2;
}



void execute8XYE( struct state *state, struct inst *inst) {
    // VX=VY (optional); VX<<=1 (carry)
    if (!settings.superChip) {
        state->regV[inst->x] = state->regV[inst->y];
    }
    state->regV[15] = state->regV[inst->x] >= 128;
    state->regV[inst->x] <<= 1;
    state->regPC += 2;
}



void execute9XY0( struct state *state, struct inst *inst) {
    // Skip if VX!=VY
    if (state->regV[inst->x]!=state->regV[inst->y]) {
        state->regPC+=2;
    }
    state->regPC += 2;
}



void executeANNN( struct state *state, struct inst *inst) {
    // I=NNN
    state->regI = inst->nnn;
    state->regPC += 2;
}



void executeBNNN( struct state *state, struct inst *inst) {
    if (settings.superChip) {
        state->regPC = inst->nnn + state->regV[inst->x];
    }
    else {
        state->regPC = inst->nnn + state->regV[0];
    }
}



void executeCXNN( struct state *state, struct inst *inst) {
    // VX=random8bit()&NN
    state->regV[inst->x] = rand() & 0xFF & inst->nn;
    state->regPC += 2;
}



void executeDXYN( struct state *state, struct inst *inst) {
    // Display (...) (carry)
    
    int winW = state->is128x64mode ? 128 : 64;
    int winH = state->is128x64mode ? 64 : 32;
    
    int startX = state->regV[inst->x] % winW;
    int startY = state->regV[inst->y] % winH;
    int nBytes = inst->n;

    bool aPixelWasTurnedOff = false;

    if (settings.superChip && !inst->n) { // 16x16 sprite
        
        for (int y = 0; y<16; y++) {
            if (startY+y >= winH ) break;
            
            uint8_t rowLeftPart = loadFromMem(state->memory, state->regI + 2*y);
            uint8_t rowRightPart = loadFromMem(state->memory, state->regI + 2*y + 1);

            for (int x = 0; x<16; x++) {
                if ( startX+x >= winW ) break;

                uint8_t pixel = x<8 ? getNthBit(rowLeftPart, 7-x) : getNthBit(rowRightPart, 15-x);
                state->display[startX+x][startY+y] ^= pixel;

                // carry flag
                if (!state->display[startX+x][startY+y] && pixel) {
                    aPixelWasTurnedOff = true;
                }
            }
        }
        
    }
    else { // normal sprites 

        for (int y = 0; y<nBytes; y++) {
            if (startY+y >= winH ) break;
            
            uint8_t row = loadFromMem(state->memory, state->regI+y);

            for (int x = 0; x<8; x++) {
                if ( startX+x >= winW ) break;

                uint8_t pixel = getNthBit(row, 7-x);
                state->display[startX+x][startY+y] ^= pixel;

                // carry flag
                if (!state->display[startX+x][startY+y] && pixel) {
                    aPixelWasTurnedOff = true;
                }
            }
        }
    }

    state->regV[15] = aPixelWasTurnedOff;
    state->displayHasChanged = true;
    state->regPC += 2;

}



void executeEX9E( struct state *state, struct inst *inst) {
    // Skip if num[VX]==pressed
    if (state->regV[inst->x] > 15) {
        fprintf(stderr, "EX9E with VX=%X\n", state->regV[inst->x]);
        exit(EXIT_FAILURE);
    }
    if ( state->numpad[ state->regV[inst->x] ]) {
        state->regPC += 2;
    }
    state->regPC += 2;
}



void executeEXA1( struct state *state, struct inst *inst) {
    // Skip if num[VX]!=pressed
    if (state->regV[inst->x] > 15) {
        fprintf(stderr, "EXA1 with VX=%X\n", state->regV[inst->x]);
        exit(EXIT_FAILURE);
    }
    if ( !state->numpad[ state->regV[inst->x] ]) {
        state->regPC += 2;
    }
    state->regPC += 2;
}



void executeFX07( struct state *state, struct inst *inst) {
    // VX=delayTimer
    state->regV[inst->x] = state->delayTimer;
    state->regPC += 2;
}



void executeFX0A( struct state *state, struct inst *inst) {
    // Block until keypress then put in VX
    int pressedKey = -1; // -1 means none
    for (int i = 0; i<16; i++) {
        if ( state->numpad[i] ) {
            pressedKey = i;
            break;
        }
    }

    if (pressedKey == -1) {
        return; // return without incrementing pc. 
    }

    state->regV[inst->x] = pressedKey;
    state->regPC += 2;
}



void executeFX15( struct state *state, struct inst *inst) {
    // delayTimer=VX
    state->delayTimer = state->regV[inst->x];
    state->regPC += 2;
}



void executeFX18( struct state *state, struct inst *inst) {
    // soundTimer=VX
    state->soundTimer = state->regV[inst->x];
    state->regPC += 2;
}



void executeFX1E( struct state *state, struct inst *inst) {
   // I+=VX (optional carry if I>0xFFF) 
    state->regI += state->regV[inst->x];
    if (settings.superChip) {
        state->regV[15] = state->regI > 0xFFF; // TODO
    }
    state->regPC += 2;
}



void executeFX29( struct state *state, struct inst *inst) {
    // Set I to addr where VX char is stored
    const int digit = state->regV[inst->x];
    if (digit > 15) {
        fprintf(stderr, "FX29 supports only 0..F.\n");
        exit(1);
    }
    state->regI = SMALL_FONT_ADDRESS + 5*digit;
    state->regPC += 2;
}



void executeFX30( struct state *state, struct inst *inst) {
    // Set I to addr where VX char is stored (8x10 sprites)
    if (!settings.superChip) {
        fprintf(stderr, "FX30 not supported on original CHIP-8.\n");
        exit(1);
    }
    const int digit = state->regV[inst->x];
    if (digit > 15) { // TODO: or 10?
        fprintf(stderr, "FX30 supports only 0..F.\n");
        exit(1);
    }
    state->regI = LARGE_FONT_ADDRESS + 10*digit;
    state->regPC += 2;
}



void executeFX33( struct state *state, struct inst *inst) {
    // Store VX in mem[I..I+2] as BCD
    const uint8_t num = state->regV[inst->x];
    storeToMem( state->memory, state->regI  , num/100); // TODO need to handle mem fault
    storeToMem( state->memory, state->regI+1, (num%100) / 10);
    storeToMem( state->memory, state->regI+2, num%10);
    // TODO I+=3 or not?
    state->regPC += 2;
}



void executeFX55( struct state *state, struct inst *inst) {
    // mem[I..] = V0..X
    const int n = inst->x;

    for (int i = 0; i<=n; i++) {
        storeToMem(state->memory, state->regI+i, state->regV[i]); // TODO need to handle mem fault
    }
    if (!settings.superChip) {
        state->regI += n+1;
    }
    state->regPC += 2;
}



void executeFX65( struct state *state, struct inst *inst) {
    // V0..X = mem[I..]
    const int n = inst->x;

    for (int i = 0; i<=n; i++) {
        state->regV[i] = loadFromMem( state->memory, state->regI+i ); // TODO need to handle mem fault
    }
    if (!settings.superChip) {
        state->regI += n+1;
    }
    state->regPC += 2;
}



void executeFX75( struct state *state, struct inst *inst) {
    // storage <= V0..X (x<8)
    if (!settings.superChip) {
        fprintf(stderr, "FX75 not supported on original CHIP-8.\n");
        exit(1);
    }

    const int n = inst->x;
    if (n >= 8) {
        fprintf(stderr, "FX75 supports X up to 7.\n");
        exit(1);
    }

    for (int i = 0; i<=n; i++) {
        state->storage[i] = state->regV[i];
    }

    state->storageHasChanged = true;
    state->regPC += 2;
}



void executeFX85( struct state *state, struct inst *inst) {
    // V0..X <= storage (x<8)
    if (!settings.superChip) {
        fprintf(stderr, "FX85 not supported on original CHIP-8.\n");
        exit(1);
    }

    const int n = inst->x;
    if (n >= 8) {
        fprintf(stderr, "FX85 supports X up to 7.\n");
        exit(1);
    }

    for (int i = 0; i<=n; i++) {
        state->regV[i] = state->storage[i];
    }
    state->regPC += 2;
}






void loadPermanentRegisters( uint8_t *destAddr ) {
    // TODO later change to actual file i/o
    uint8_t empty[8] = {0,0,0,0,0,0,0,0};
    memcpy(destAddr, empty, 8);
}

void savePermanentRegisters( uint8_t *regs) {
    // TODO later change to actual file i/o
    return;
}

void newState( struct state *destAddr) {
    // assumes destAddr is pre-allocated and can hold struct state
    // this leaves memory and storage fields blank, expecting them to be filled later
    struct state state = {
        .memory={0},
        .numpad={0},
        .display={0},
        .regI=0, .regPC=0x200,
        .regV={0}, .regSP=0, .delayTimer=0, .soundTimer=0,
        .is128x64mode=false,
        .storage={0},
        .storageHasChanged=false, .displayHasChanged=false
    };

    memcpy(destAddr, &state, sizeof(state));
}


uint16_t fetchInst( uint8_t* memory, uint16_t pc) {
    // TODO align 2 here? or just take two bytes
    uint8_t instHighPart = loadFromMem(memory, pc);
    uint8_t instLowPart = loadFromMem(memory, pc+1);
    return (instHighPart<<8) | instLowPart;
}

instHandler findHandler( enum operation operation) {
    for (int i = 0; i<ENCODING_OPERATION_MAP_LENGTH; i++) {
        if ( encodingOperationMap[i].operation == operation) {
            return encodingOperationMap[i].handler;
        }
    }
    
    fprintf(stderr, "Couldn't find a handler for inst.\n");
    exit(1);
}

void updateNumpad( bool *numpad ) {
    // we're assuming the following layout for now:
    // on keyboard            corresponding values
    // -------------         ------------------------
    // 1  2  3  4               0  1  2  3
    // Q  W  E  R               4  5  6  7
    // A  S  D  F               8  9  A  B
    // Z  X  C  V               C  D  E  F

    numpad[0] = IsKeyDown(KEY_ONE);
    numpad[1] = IsKeyDown(KEY_TWO);
    numpad[2] = IsKeyDown(KEY_THREE);
    numpad[3] = IsKeyDown(KEY_FOUR);
    numpad[4] = IsKeyDown(KEY_Q);
    numpad[5] = IsKeyDown(KEY_W);
    numpad[6] = IsKeyDown(KEY_E);
    numpad[7] = IsKeyDown(KEY_R);
    numpad[8] = IsKeyDown(KEY_A);
    numpad[9] = IsKeyDown(KEY_S);
    numpad[0xA] = IsKeyDown(KEY_D);
    numpad[0xB] = IsKeyDown(KEY_F);
    numpad[0xC] = IsKeyDown(KEY_Z);
    numpad[0xD] = IsKeyDown(KEY_X);
    numpad[0xE] = IsKeyDown(KEY_C);
    numpad[0xF] = IsKeyDown(KEY_V);
}

void makeSound(void) {
    puts("Buzz!");
    // TODO change later
}

void drawDisplay( struct state *state ) {
    ClearBackground(BLACK);
    if (state->is128x64mode) {
        const int CELL_WIDTH = WINDOW_WIDTH/128;
        const int CELL_HEIGHT = WINDOW_HEIGHT/64;

        for (int x = 0; x<128; x++) {
            for (int y = 0; y<64; y++) {
                if (state->display[x][y]) {
                    DrawRectangle(x*CELL_WIDTH, y*CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, BLACK);
                }
                else {
                    DrawRectangle(x*CELL_WIDTH, y*CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, WHITE);
                }
            }
        }
    }
    else {
        const int CELL_WIDTH = WINDOW_WIDTH/64;
        const int CELL_HEIGHT = WINDOW_HEIGHT/32;

        for (int x = 0; x<64; x++) {
            for (int y = 0; y<32; y++) {
                if (state->display[x][y]) {
                    DrawRectangle(x*CELL_WIDTH, y*CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, BLACK);
                }
                else {
                    DrawRectangle(x*CELL_WIDTH, y*CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, WHITE);
                }
            }
        }
    }
}

void printState( struct state *state ) {
    printf("I:%X PC:%X SP:%X V:%X %X %X %X  %X %X %X %X  %X %X %X %X  %X %X %X %X (%d %d)\n",
    state->regI, state->regPC, state->regSP,
    state->regV[0],
    state->regV[1],
    state->regV[2],
    state->regV[3],
    state->regV[4],
    state->regV[5],
    state->regV[6],
    state->regV[7],
    state->regV[8],
    state->regV[9],
    state->regV[0xA],
    state->regV[0xB],
    state->regV[0xC],
    state->regV[0xD],
    state->regV[0xE],
    state->regV[0xF],
    state->storageHasChanged,
    state->displayHasChanged
    );
}

void printMemory( uint8_t memory[4096]) {
    const char* ANSI_COLOR_RESET = "\x1b[0m";
    const char* ANSI_COLOR_WHITE_ON_BLACK = "\x1b[37;40m";
    const char* ANSI_COLOR_BLACK_ON_WHITE = "\x1b[30;47m";
    const char* ANSI_COLOR_WHITE_ON_GREEN = "\x1b[37;42m";

    bool currentlyBlackOnWhite = true;
    for (int i = 0; i<64; i++) {
        for (int j = 0; j<64; j++) {
            if (currentlyBlackOnWhite) {
                printf("%s", ANSI_COLOR_BLACK_ON_WHITE);
            }
            else {
                printf("%s", ANSI_COLOR_WHITE_ON_BLACK);
            }

            if ( 64*i+j == 0x200) {
                printf("%s", ANSI_COLOR_WHITE_ON_GREEN);
            }

            printf("%2X", memory[64*i+j]);
            currentlyBlackOnWhite = !currentlyBlackOnWhite;
        }
        printf("%s", ANSI_COLOR_RESET);
        puts("");
        currentlyBlackOnWhite = !currentlyBlackOnWhite;
    }

    printf("%s", ANSI_COLOR_RESET);
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: chip8emulator gameFile.ch8\n");
        exit(1);
    }

    SetTraceLogLevel(LOG_WARNING);

    // loading game data from file:
    int returnedSize;
    uint8_t* gameData = LoadFileData(argv[1], &returnedSize);
    if (returnedSize > 4096-0x200+1) {
        fprintf(stderr, "Game size max 3584 bytes.\n");
        exit(1);
    }

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chip-8");
    SetTargetFPS(TARGET_FPS);

    int INST_COMPLETE = 0;
    
    struct state state;
    newState(&state);
    newMemory(state.memory, gameData, returnedSize);
    loadPermanentRegisters(state.storage);

    while( !WindowShouldClose() && INST_COMPLETE < TOTAL_INST_TO_RUN ) {

        // update numpad
        updateNumpad( state.numpad);

        if (state.delayTimer > 0) {
            state.delayTimer--;
        }
        if (state.soundTimer > 0) {
            state.soundTimer--;
            // make a sound
            makeSound();
        }

        for (int i = 0; i<INST_PER_FRAME; i++) {


            uint16_t instRaw = fetchInst(state.memory, state.regPC);
            struct inst instDecoded = decodeInst(instRaw);

            if (!instDecoded.isValid) {
                fprintf(stderr, "Encountered invalid inst.\n");
                exit(1);
            }

            instHandler handler = findHandler(instDecoded.operation);
            handler(&state, &instDecoded);

            if (state.storageHasChanged) {
                // save to perm mem
                savePermanentRegisters(state.storage);
                state.storageHasChanged = false;
            }

            INST_COMPLETE++;
        }

        BeginDrawing();

        if (state.displayHasChanged) {
            // draw
            drawDisplay(&state);
            state.displayHasChanged = false;
        }

        EndDrawing();
    }

    




    return 0;
}