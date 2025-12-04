#pragma once

#include <cstdint>
#include <string>
#include <map>

namespace cpu {

// Instruction Set Architecture Definition

// Instruction format: 16-bit
// Format: [OPCODE:4][RD:3][RS1:3][RS2:3][IMM:3] or [OPCODE:4][RD:3][RS1:3][IMM:6]
// For immediate instructions, IMM is 6 bits (signed -32 to 31)
// For jump instructions (JMP, JZ, JNZ), RD[11:9] + IMM[5:0] = 9 bits (signed -256 to 255)
// For register instructions, RS2 is used and IMM is ignored

// Opcodes (4 bits = 16 possible instructions)
enum class Opcode : uint8_t {
    NOP = 0x0,   // No operation
    ADD = 0x1,   // Add: RD = RS1 + RS2
    SUB = 0x2,   // Subtract: RD = RS1 - RS2
    AND = 0x3,   // Bitwise AND: RD = RS1 & RS2
    OR  = 0x4,   // Bitwise OR: RD = RS1 | RS2
    XOR = 0x5,   // Bitwise XOR: RD = RS1 ^ RS2
    NOT = 0x6,   // Bitwise NOT: RD = ~RS1
    SHL = 0x7,   // Shift left: RD = RS1 << IMM
    SHR = 0x8,   // Shift right: RD = RS1 >> IMM
    LD  = 0x9,   // Load: RD = MEM[RS1 + IMM]
    ST  = 0xA,   // Store: MEM[RS1 + IMM] = RD
    LDI = 0xB,   // Load immediate: RD = IMM (sign extended)
    JMP = 0xC,   // Jump: PC = RS1 + IMM
    JZ  = 0xD,   // Jump if zero: if (Z flag) PC = RS1 + IMM
    JNZ = 0xE,   // Jump if not zero: if (!Z flag) PC = RS1 + IMM
    HLT = 0xF    // Halt
};

// Addressing modes
enum class AddressingMode {
    REGISTER,    // Register-register operation
    IMMEDIATE,   // Immediate value
    INDIRECT,    // Memory indirect (for LD/ST)
    DIRECT       // Direct address
};

// Instruction structure
struct Instruction {
    Opcode opcode;
    uint8_t rd;      // Destination register (0-7)
    uint8_t rs1;     // Source register 1 (0-7)
    uint8_t rs2;     // Source register 2 (0-7) or unused
    int8_t imm;      // Immediate value (-32 to 31) or unused
    int16_t jump_imm; // Extended immediate for jump instructions (-256 to 255), only valid for JMP/JZ/JNZ
    bool is_immediate;
    
    // Encode instruction to 16-bit word
    uint16_t encode() const {
        uint16_t word = 0;
        word |= (static_cast<uint8_t>(opcode) & 0x0F) << 12;
        word |= (rd & 0x07) << 9;
        word |= (rs1 & 0x07) << 6;
        
        if (is_immediate) {
            // Check if this is a jump instruction (uses extended 9-bit immediate)
            if (opcode == Opcode::JMP || opcode == Opcode::JZ || opcode == Opcode::JNZ) {
                // Jump instructions: use RD[11:9] + IMM[5:0] for 9-bit signed immediate
                // Use jump_imm field (set by assembler) which contains the full 9-bit value
                int16_t extended_imm = jump_imm;
                // Pack 9-bit signed value: upper 3 bits in RD field, lower 6 bits in IMM field
                uint16_t imm_upper = (static_cast<uint16_t>(extended_imm) >> 6) & 0x07;  // Upper 3 bits
                uint16_t imm_lower = static_cast<uint16_t>(extended_imm) & 0x3F;         // Lower 6 bits
                word |= imm_upper << 9;  // Store in RD field
                word |= imm_lower;        // Store in IMM field
            } else {
                // Other immediate instructions: 6-bit immediate
                word |= (static_cast<uint8_t>(imm) & 0x3F);
            }
        } else {
            // Register format: use RS2
            word |= (rs2 & 0x07) << 3;
        }
        
        return word;
    }
    
    // Decode 16-bit word to instruction
    static Instruction decode(uint16_t word) {
        Instruction instr;
        instr.opcode = static_cast<Opcode>((word >> 12) & 0x0F);
        instr.rd = (word >> 9) & 0x07;
        instr.rs1 = (word >> 6) & 0x07;
        
        // Determine if immediate or register based on opcode
        instr.is_immediate = (instr.opcode == Opcode::LDI || 
                             instr.opcode == Opcode::LD || 
                             instr.opcode == Opcode::ST ||
                             instr.opcode == Opcode::JMP ||
                             instr.opcode == Opcode::JZ ||
                             instr.opcode == Opcode::JNZ ||
                             instr.opcode == Opcode::SHL ||
                             instr.opcode == Opcode::SHR);
        
        if (instr.is_immediate) {
            // Check if this is a jump instruction (uses extended 9-bit immediate)
            if (instr.opcode == Opcode::JMP || instr.opcode == Opcode::JZ || instr.opcode == Opcode::JNZ) {
                // Jump instructions: extract 9-bit signed immediate from RD[11:9] + IMM[5:0]
                uint16_t imm_upper = (word >> 9) & 0x07;  // Upper 3 bits from RD field
                uint16_t imm_lower = word & 0x3F;         // Lower 6 bits from IMM field
                uint16_t extended_imm_raw = (imm_upper << 6) | imm_lower;
                // Sign-extend 9-bit value to 16-bit
                int16_t extended_imm = extended_imm_raw;
                if (extended_imm & 0x100) {  // Sign bit (bit 8)
                    extended_imm |= 0xFE00;  // Sign extend to 16 bits (-256 to -1)
                }
                // Store the full 9-bit value in jump_imm field
                instr.jump_imm = extended_imm;
                // Also store lower 8 bits in imm for compatibility
                instr.imm = static_cast<int8_t>(extended_imm & 0xFF);
                instr.rd = 0;  // RD field was used for immediate, so set to 0
            } else {
                // Other immediate instructions: 6-bit immediate
                int8_t imm_raw = word & 0x3F;
                if (imm_raw & 0x20) {  // Sign bit
                    instr.imm = imm_raw | 0xC0;  // Sign extend
                } else {
                    instr.imm = imm_raw;
                }
                instr.jump_imm = static_cast<int16_t>(instr.imm);  // For non-jump instructions, jump_imm = imm
            }
            instr.rs2 = 0;
        } else {
            instr.rs2 = (word >> 3) & 0x07;
            instr.imm = 0;
            instr.jump_imm = 0;
        }
        
        return instr;
    }
    
    // Get extended immediate for jump instructions (9-bit signed)
    // Returns the 9-bit signed immediate value for JMP/JZ/JNZ instructions
    int16_t get_jump_immediate() const {
        if (opcode == Opcode::JMP || opcode == Opcode::JZ || opcode == Opcode::JNZ) {
            return jump_imm;  // Return the stored 9-bit value
        }
        return static_cast<int16_t>(imm);  // For other instructions, return 6-bit immediate
    }
    
    // Get instruction mnemonic
    std::string mnemonic() const {
        static const std::map<Opcode, std::string> opcode_names = {
            {Opcode::NOP, "NOP"}, {Opcode::ADD, "ADD"}, {Opcode::SUB, "SUB"},
            {Opcode::AND, "AND"}, {Opcode::OR, "OR"}, {Opcode::XOR, "XOR"},
            {Opcode::NOT, "NOT"}, {Opcode::SHL, "SHL"}, {Opcode::SHR, "SHR"},
            {Opcode::LD, "LD"}, {Opcode::ST, "ST"}, {Opcode::LDI, "LDI"},
            {Opcode::JMP, "JMP"}, {Opcode::JZ, "JZ"}, {Opcode::JNZ, "JNZ"},
            {Opcode::HLT, "HLT"}
        };
        
        std::string name = opcode_names.at(opcode);
        
        if (opcode == Opcode::NOP || opcode == Opcode::HLT) {
            return name;
        } else if (opcode == Opcode::NOT) {
            return name + " R" + std::to_string(rd) + ", R" + std::to_string(rs1);
        } else if (is_immediate) {
            return name + " R" + std::to_string(rd) + ", R" + std::to_string(rs1) + ", #" + std::to_string(static_cast<int>(imm));
        } else {
            return name + " R" + std::to_string(rd) + ", R" + std::to_string(rs1) + ", R" + std::to_string(rs2);
        }
    }
};

} // namespace cpu

