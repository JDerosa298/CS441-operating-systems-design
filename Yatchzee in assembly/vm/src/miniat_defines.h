#ifndef _MINIAT_PRIV_DEFINES_H_
#define _MINIAT_PRIV_DEFINES_H_

#include <time.h>

#define M_HEX_1       0x1

/* Start Instructions */
/* Start Arithmetic Instructions */
#define M_ADD       0x0 //!< Addition opcode
#define M_SUB       0x1 //!< Subtraction opcode
#define M_MULT      0x2 //!< Multiplication opcode
#define M_DIV       0x3 //!< Division opcode
#define M_MOD       0x4 //!< Modulus opcode
#define M_AND       0x5 //!< Bitwise 'and' opcode
#define M_OR        0x6 //!< Bitwise 'or' opcode
#define M_EXOR      0x7 //!< Bitwise 'exclusive or' opcode
#define M_SHL       0x8 //!< Left shift opcode
#define M_SHR       0x9 //!< Right shift opcode
/* End Arithmetic Instructions */

/* Start Logic Instructions */
#define M_LOAD      0x10 //!< Load opcode
#define M_STORE     0x11 //!< Store opcode
#define M_RLOAD     0x12 //!< Register load opcode
#define M_RSTORE    0x13 //!< Register store opcode
/* End Logic Instructions */

/* Start Control Instructions */
#define M_BRAE      0x16 //!< Branch 'equal' opcode
#define M_BRANE     0x17 //!< Branch 'not equal' opcode
#define M_BRAL      0x18 //!< Branch 'less than' opcode
#define M_BRALE     0x19 //!< Branch 'less than or equal to' opcode
#define M_BRAG      0x1A //!< Branch 'greater than' opcode
#define M_BRAGE     0x1B //!< Branch 'greater than or equal to' opcode
/* End Control Instructions */

/* Start Interrupt Instructions */
#define M_INT       0x1C //!< Interrupt opcode
#define M_IRET      0x1D //!< Interrupt return opcode
/* End Interrupt Instructions */
/* End Instructions */

/* Start Memory Layout */
#define M_RAM_INIT_FLAGWORD         0xDEADBEEF //!< Ram initialization value
#define M_FLASH_INIT_FLAGWORD       0xDEADBEEF //!< Flash initialization value

#define M_RESERVED_ONE_START        0x00000000 //!< Start Reserved section 1
#define M_RESERVED_ONE_END          0x000001FF //!< End Reserved section 1

#define M_RAM_START                 0x00000200 //!< Start RAM
#define M_RAM_END                   0x000019FF //!< End RAM

#define M_RESERVED_TWO_START        0x00001A00 //!< Start Reserved section 2
#define M_RESERVED_TWO_END          0x00001AD9 //!< End Reserved section 2

/*Start Interrupt Vector Table*/
#define M_IVT_START                 0x00001ADA //!< Starting address of Interrupt Vector Table
/* Start Internal Interrupts */
#define M_IVT_INT_RESET             0x00001ADA //!< Reset interrupt vector address
#define M_IVT_ILLEGAL_ADDRESS       0x00001ADB //!< Illegal address interrupt vector address
#define M_IVT_ILLEGAL_INSTRUCTION   0x00001ADC //!< Illegal instruction interrupt vector address
#define M_IVT_BUS_ERROR             0x00001ADD //!< Bus error interrupt vector address
#define M_IVT_DIVIDE_BY_ZERO        0x00001ADE //!< Divide by zero interrupt vector address
#define M_IVT_WATCHDOG              0x00001ADF //!< Watchdog interrupt vector address
#define M_IVT_TIMER0                0x00001AE0 //!< Timer 0 interrupt vector address
#define M_IVT_TIMER1                0x00001AE1 //!< Timer 1 interrupt vector address
#define M_IVT_TIMER2                0x00001AE2 //!< Timer 2 interrupt vector address
#define M_IVT_TIMER3                0x00001AE3 //!< Timer 3 interrupt vector address
#define M_IVT_OVERFLOW              0x00001AE4 //!< Arithmetic overflow interrupt vector address
/* End Internal Interrupts */
#define M_IVT_RSVD_INT_START        0x00001B1A //!< Start reserved section 3
#define M_IVT_RSVD_INT_END          0x00001AF9 //!< End reserved section 3
#define M_IVT_XINT0                 0x00001AFA //!< External interrupt 0 interrupt vector address
#define M_IVT_XINT1                 0x00001AFB //!< External interrupt 1 interrupt vector address
#define M_IVT_XINT2                 0x00001AFC //!< External interrupt 2 interrupt vector address
#define M_IVT_XINT3                 0x00001AFD //!< External interrupt 3 interrupt vector address
#define M_IVT_XINT4                 0x00001AFE //!< External interrupt 4 interrupt vector address
#define M_IVT_XINT5                 0x00001AFF //!< External interrupt 5 interrupt vector address
#define M_IVT_XINT6                 0x00001B00 //!< External interrupt 6 interrupt vector address
#define M_IVT_XINT7                 0x00001B01 //!< External interrupt 7 interrupt vector address
#define M_IVT_XINT8                 0x00001B02 //!< External interrupt 8 interrupt vector address
#define M_IVT_XINT9                 0x00001B03 //!< External interrupt 9 interrupt vector address
#define M_IVT_XINT10                0x00001B04 //!< External interrupt 10 interrupt vector address
#define M_IVT_XINT11                0x00001B05 //!< External interrupt 11 interrupt vector address
#define M_IVT_XINT12                0x00001B06 //!< External interrupt 12 interrupt vector address
#define M_IVT_XINT13                0x00001B07 //!< External interrupt 13 interrupt vector address
#define M_IVT_XINT14                0x00001B08 //!< External interrupt 14 interrupt vector address
#define M_IVT_XINT15                0x00001B09 //!< External interrupt 15 interrupt vector address
/* End External Interrupts */
#define M_IVT_RSVD_XINT_START       0x00001B0A //!< Start reserved section 4 address
#define M_IVT_RSVD_XINT_END         0x00001B19 //!< End reserved section 4 address
#define M_IVT_END                   0x00001B19 //!< Ending address of the Interrupt vector table address
/* End Interrupt Vector Table */

/* Start General Purpose IO */
#define M_GPIO_START                0x00001B1A //!< General purpose IO register start address
#define M_GPIO_PORT_A               0x00001B1A //!< General purpose IO register A address
#define M_GPIO_PORT_B               0x00001B1B //!< General purpose IO register B address
#define M_GPIO_PORT_C               0x00001B1C //!< General purpose IO register C address
#define M_GPIO_PORT_D               0x00001B1D //!< General purpose IO register D address
#define M_GPIO_END                  0x00001B1D //!< General purpose IO register end address
/* End General Purpose IO */

/* Start Control / Status Registers */
#define M_CSR_START                 0x00001B1E //!< Control status register start address
#define M_CSR_SYSTEM_REGISTER       0x00001B1E //!< Control status register system register address
#define M_CSR_WATCHDOG_COMPARE      0x00001B1F //!< Control status register watchdog compare address
#define M_CSR_WATCHDOG_CONTROL      0x00001B20 //!< Control status register watchdog control address
#define M_CSR_TIMER_CONTROL         0x00001B21 //!< Control status register timer control address
#define M_CSR_TIMER0_COMPARE        0x00001B22 //!< Control status register timer 0 compare address
#define M_CSR_TIMER1_COMPARE        0x00001B23 //!< Control status register timer 1 compare address
#define M_CSR_TIMER2_COMPARE        0x00001B24 //!< Control status register timer 2 compare address
#define M_CSR_TIMER3_COMPARE        0x00001B25 //!< Control status register timer 3 compare address
#define M_CSR_TIMER0_COUNT          0x00001B26 //!< Control status register timer 0 count address
#define M_CSR_TIMER1_COUNT          0x00001B27 //!< Control status register timer 1 count address
#define M_CSR_TIMER2_COUNT          0x00001B28 //!< Control status register timer 2 count address
#define M_CSR_TIMER3_COUNT          0x00001B29 //!< Control status register timer 3 count address
#define M_CSR_TIMER_IO              0x00001B2A //!< Control status register timer IO address
#define M_CSR_RSVD_START            0x00001B2B //!< Start reserved section 5 address
#define M_CSR_RSVD_END              0x00001F15 //!< End reserved section 5 address
/* Start GPIO Port Directions */
#define M_CSR_PORT_A_DIRECTIONS     0x00001F16 //!< Control status register gpio port A direction address
#define M_CSR_PORT_B_DIRECTIONS     0x00001F17 //!< Control status register gpio port B direction address
#define M_CSR_PORT_C_DIRECTIONS     0x00001F18 //!< Control status register gpio port C direction address
#define M_CSR_PORT_D_DIRECTIONS     0x00001F19 //!< Control status register gpio port D direction address
/* End GPIO Port Directions */
#define M_CSR_INT_ENABLE_LOW        0x00001F1A //!< Control status register interrupt enable low address
#define M_CSR_INT_ENABLE_HIGH       0x00001F1B //!< Control status register interrupt enable high address
#define M_CSR_INT_FLAG_LOW          0x00001F1C //!< Control status register interrupt flag low address
#define M_CSR_INT_FLAG_HIGH         0x00001F1D //!< Control status register interrupt flag high address
#define M_CSR_END                   0x00001F1D //!< Control status register ending address
/* End Control / Status Registers */

/* Start Identification */
#define M_ID_START                  0x00001F1E //!< Virtual machine ID starting address
#define M_ID_END                    0x00001FFF //!< Virtual machine ID ending address
/* End Identification */

/* Start Flash */
#define M_FLASH_START               0x00002000 //!< Starting address of flash
#define M_FLASH_END                 0x00003FFF //!< Ending address of flash
/* End Flash */

/* Start Peripheral */
#define M_PERIPHERAL_START          0x00004000 //!< Staring address of peripheral memory space (Think bus)
#define M_PERIPHERAL_END            0x0000FFFF //!< Ending address of peripheral memory space (Think bus)
/* End Peripheral */
/* End Memory Layout */

/* Start Register File Layout */
#define M_REGISTER_ZERO             0   //!< The "zero's" register, always returns a 0
#define M_REGISTER_PREDICATE        1   //!<
#define M_REGISTER_RSP              253 //!<
#define M_REGISTER_SP               254 //!<
#define M_REGISTER_PC               255 //!< The public program counter, position 0 of the pc queue
#define M_REGISTER_PC_ONE           255 //!< The program counter position 1 of the pc queue
#define M_REGISTER_PC_TWO           256 //!< The program counter position 2 of the pc queue
#define M_REGISTER_PC_THREE         257 //!< The program counter position 3 of the pc queue
#define M_PC_QUEUE_SIZE             4   //!< The length of the pc queue
/* End Register File Layout */

/* Start CSR System Register Layout */
#define M_CSR_SYSTEM_REGISTER_INT_BIT               0
#define M_CSR_SYSTEM_REGISTER_SATURATION_BIT        1
#define M_CSR_SYSTEM_REGISTER_UNSIGNED_BIT          2
/* End CSR System Register Layout */

/* Start Sizes */
/* Start Instruction Sizes */
#define M_INSTRUCTION_SIZE_DWORDS   1 //!< Expected number of double words (64 bits) a MiniAT instruction should be
#define M_INSTRUCTION_SIZE_WORDS    2 //!< Expected number of words (32 bits) a MiniAT instruction should be
#define M_INSTRUCTION_SIZE_WYDES    4 //!< Expected number of wydes (16 bits) a MiniAT instruction should be
#define M_INSTRUCTION_SIZE_BYTES    8 //!< Expected number of bytes (8 bits) a MiniAT instruction should be
/* End Instruction Sizes */
/* Start memory section sizes */
#define M_MEMORY_SIZE               (M_PERIPHERAL_END - M_RESERVED_ONE_START + 1) //!< Size of the memory range
#define M_RAM_SIZE                  (M_RAM_END - M_RAM_START + 1) //!< Size of ram
#define M_RESERVED_TWO_SIZE         (M_RESERVED_TWO_END - M_RESERVED_TWO_START + 1) //!< Size of reserved space 2 block
#define M_IVT_SIZE                  (M_IVT_END - M_IVT_START + 1) //!< Size of the IVT
#define M_IVT_RSVD_INT_SIZE         (M_IVT_RSVD_INT_END - M_IVT_RSVD_INT_START + 1) //!< Size of the reserved space in the IVT for INTS
#define M_IVT_RSVD_XINT_SIZE        (M_IVT_RSVD_XINT_END - M_IVT_RSVD_XINT_START + 1) //!< Size of the reserved space in the IVT for XINTs
#define M_GPIO_SIZE                 (M_GPIO_END - M_GPIO_START + 1) //!< Size of the GPIO ports in memory addresses
#define M_CSR_SIZE                  (M_CSR_END - M_CSR_START + 1) //!< Size of the CSR in memory addresses
#define M_CSR_RSVD_SIZE             (M_CSR_RSVD_END - M_CSR_RSVD_START + 1) //!< Size of the CSR register(s) in memory addresses

#define M_MID_SIZE                  (M_MID_END - M_MID_START + 1) //TODO Not sure what this is
#define M_FLASH_SIZE                (M_FLASH_END - M_FLASH_START + 1) //!< Size of flash address space in memory addresses*/
#define M_FLASH_SIZE_IN_BYTES       (M_FLASH_SIZE * sizeof(m_uword))

#define M_PERIPHERAL_SIZE           (M_PERIPHERAL_END - M_PERIPHERAL_START + 1) //!< Size of the peripheral address space in memory addresses */

/* End Memory Section Sizes */
#define M_OPCODE_SIZE               (M_OPCODE_END - M_OPCODE_START + 1)
#define M_IMMEDIATE_SIZE            (M_IMMEDIATE_END - M_IMMEDIATE_START + 1)
#define M_PIPELINE_SIZE             4 //!< Size of the pipeline, ie number of stages
#define M_HAZARD_QUEUE_SIZE         (M_PIPELINE_SIZE - 1) //!< Size of the pipeline hazards queue
#define M_IVBT_SIZE                 (M_IVT_SIZE + 1) //!< Size of the interrupt backup table
#define M_IVBT_PC 						64 //!< Location of the PC in the IVBT

/* End Sizes */

/* Start Interrupt Numbers */
/* Start Internal Interrupt Numbers */
#define M_INT_RESET                 0  //!< Reset interrupt number
#define M_INT_ILLEGAL_ADDRESS       1  //!< Illegal address interrupt number
#define M_INT_ILLEGAL_INSTRUCTION   2  //!< Illegal instruction interrupt number
#define M_INT_BUS_ERROR             3  //!< Bus error interrupt number
#define M_INT_DIVIDE_BY_ZERO        4  //!< Divide by zero interrupt number
#define M_INT_WATCHDOG              5  //!< Watchdog interrupt number
#define M_INT_TIMER0                6  //!< Timer 0 interrupt number
#define M_INT_TIMER1                7  //!< Timer 1 interrupt number
#define M_INT_TIMER2                8  //!< Timer 2 interrupt number
#define M_INT_TIMER3                9  //!< Timer 3 interrupt number
#define M_INT_OVERFLOW              10 //!< Timer 4 interrupt number
/* End Internal Interrupt Numbers */

/* Start External Interrupt Numbers */
#define M_XINT0                     32 //!< External interrupt 0 number
#define M_XINT1                     33 //!< External interrupt 1 number
#define M_XINT2                     34 //!< External interrupt 2 number
#define M_XINT3                     35 //!< External interrupt 3 number
#define M_XINT4                     36 //!< External interrupt 4 number
#define M_XINT5                     37 //!< External interrupt 5 number
#define M_XINT6                     38 //!< External interrupt 6 number
#define M_XINT7                     39 //!< External interrupt 7 number
#define M_XINT8                     40 //!< External interrupt 8 number
#define M_XINT9                     41 //!< External interrupt 9 number
#define M_XINT10                    42 //!< External interrupt 10 number
#define M_XINT11                    43 //!< External interrupt 11 number
#define M_XINT12                    44 //!< External interrupt 12 number
#define M_XINT13                    45 //!< External interrupt 13 number
#define M_XINT14                    46 //!< External interrupt 14 number
#define M_XINT15                    47 //!< External interrupt 15 number
/* End External Interrupt Numbers */
/* End Interrupt Numbers */

/* Start Resource Quantities */
#define M_NUM_REGISTERS             256 //!< Number of registers
#define M_NUM_OF_TIMERS             4   //!< Number of timers
#define M_NUM_OF_IFRS               2   //!< Number of interrupt flag register(s)
/* End Resource Quantities */

#define M_FILE_ID 0xBADCC0DE         //!< File ID code
#define M_BLOCK_SEPERATOR 0xDEADBEEF //!< Block separator code
/* End File Format */

/* Start Miscelaneous */
/**
 * Returns the number of bits in a data type
 * @param TYPE
 *  The type or data to check
 * @return
 *  The number of bits in the data passed through TYPE
 */
#define m_bits_in_data(TYPE)   (sizeof(TYPE) * 8)
#define M_SYSTEM_BIG_ENDIAN    0
#define M_SYSTEM_LITTLE_ENDIAN 1
#define M_BUS_STROBE_DURATION  2
/* End Miscelaneous */
#endif
