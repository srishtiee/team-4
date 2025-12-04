#pragma once

#include "cpu/registers.hpp"
#include "cpu/alu.hpp"
#include "cpu/bus.hpp"
#include "cpu/memory.hpp"
#include "cpu/isa.hpp"
#include "cpu/control_unit.hpp"
#include <vector>
#include <string>
#include <iomanip>
#include <iostream>

namespace emulator {

// Main CPU Emulator class
class CPUEmulator {
private:
    cpu::GPRs gprs;
    cpu::SPRs sprs;
    cpu::Memory memory;
    cpu::ControlUnit control_unit;
    cpu::BusSystem buses;
    
    bool running;
    uint16_t program_start;
    
public:
    CPUEmulator(bool trace = false) 
        : control_unit(trace), running(false), program_start(0x0000) {}
    
    // Load program into memory
    void load_program(const std::vector<uint16_t>& program, uint16_t start_address = 0x0000) {
        program_start = start_address;
        sprs.PC = start_address;
        memory.load_program(start_address, program);
    }
    
    // Run program until halt
    void run() {
        running = true;
        while (running && !control_unit.is_halted()) {
            running = control_unit.execute_cycle(memory, gprs, sprs, buses);
        }
        // Flush any remaining output in the buffer
        std::string remaining = memory.get_output();
        if (!remaining.empty()) {
            std::cout << remaining << std::endl;
            memory.clear_output();
        }
    }
    
    // Step one instruction
    void step() {
        if (!control_unit.is_halted()) {
            control_unit.execute_cycle(memory, gprs, sprs, buses);
        }
    }
    
    // Reset CPU state
    void reset() {
        gprs = cpu::GPRs();
        sprs = cpu::SPRs();
        buses.reset();
        running = false;
        sprs.PC = program_start;
    }
    
    // Print CPU state
    void print_state() const {
        std::cout << "\n=== CPU State ===" << std::endl;
        std::cout << "Cycle: " << control_unit.get_cycle_count() << std::endl;
        gprs.print();
        std::cout << std::endl;
        sprs.print();
    }
    
    // Print GPRs
    void print_gprs() const {
        gprs.print();
    }
    
    // Print SPRs
    void print_sprs() const {
        sprs.print();
    }
    
    // Print RAM
    void print_ram(uint16_t start = 0, uint16_t length = 256) const {
        memory.print_dump(start, length);
    }
    
    // Print memory values as decimal numbers
    void print_decimal(uint16_t start = 0, uint16_t count = 10) const {
        std::cout << "\n=== Memory as Decimal Numbers ===" << std::endl;
        std::cout << "Starting address: 0x" << std::hex << std::setw(4) << std::setfill('0') 
                  << start << std::dec << std::endl;
        for (uint16_t i = 0; i < count; i++) {
            uint16_t addr = start + (i * 2);  // Each value is 2 bytes (16-bit word)
            uint16_t value = memory.read_word(addr);
            std::cout << "[" << std::setw(2) << std::setfill('0') << i << "] "
                      << "0x" << std::hex << std::setw(4) << std::setfill('0') << addr 
                      << std::dec << ": " << static_cast<int16_t>(value) << std::endl;
        }
    }
    
    // Print memory as instructions
    void print_instructions(uint16_t start, uint16_t count) const {
        memory.print_instructions(start, count);
    }
    
    // Enable/disable trace
    void enable_trace(bool enable) {
        control_unit.enable_trace(enable);
    }
    
    // Get current PC
    uint16_t get_pc() const {
        return sprs.PC;
    }
    
    // Check if halted
    bool is_halted() const {
        return control_unit.is_halted();
    }
    
    // Get cycle count
    uint64_t get_cycle_count() const {
        return control_unit.get_cycle_count();
    }
    
    // Memory dump to file
    void memory_dump(const std::string& filename, uint16_t start = 0, uint16_t length = 0xFFFF) const {
        // This would write memory to file - simplified for now
        (void)start;  // Suppress unused parameter warning
        (void)length; // Suppress unused parameter warning
        std::cout << "Memory dump to " << filename << " (not implemented)" << std::endl;
    }
    
    // Print stack contents
    void print_stack(uint16_t count = 20) const {
        std::cout << "\n=== Stack Contents ===" << std::endl;
        std::cout << "Stack Pointer (SP): 0x" << std::hex << std::setw(4) << std::setfill('0') 
                  << sprs.SP << std::dec << std::endl;
        std::cout << "Stack grows downward (from 0xFFFF)" << std::endl;
        std::cout << std::endl;
        
        uint16_t stack_top = 0xFFFF;
        uint16_t stack_bottom = (sprs.SP >= count * 2) ? (sprs.SP - count * 2) : 0xFE00;
        if (stack_bottom < 0xFE00) stack_bottom = 0xFE00;
        
        std::cout << "Address    Value (hex)  Value (dec)  Interpretation" << std::endl;
        std::cout << "---------------------------------------------------" << std::endl;
        
        for (uint16_t addr = stack_top; addr >= stack_bottom && addr >= sprs.SP; addr -= 2) {
            uint16_t value = memory.read_word(addr);
            std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << addr 
                      << "    0x" << std::setw(4) << std::setfill('0') << value
                      << "      " << std::dec << std::setw(6) << static_cast<int16_t>(value);
            
            // Try to interpret as address (if it's in code range)
            if (value >= 0x0000 && value < 0x0100) {
                std::cout << "  [Possible return addr: 0x" << std::hex << std::setw(4) 
                          << std::setfill('0') << value << std::dec << "]";
            }
            std::cout << std::endl;
        }
        std::cout << std::dec;
    }
    
    // Print stack frames (for function calls)
    void print_frames() const {
        std::cout << "\n=== Stack Frames ===" << std::endl;
        std::cout << "Stack Pointer (SP): 0x" << std::hex << std::setw(4) << std::setfill('0') 
                  << sprs.SP << std::dec << std::endl;
        std::cout << std::endl;
        
        uint16_t current_sp = sprs.SP;
        uint16_t stack_top = 0xFFFF;
        int frame_num = 1;
        
        // Walk up the stack looking for return addresses
        while (current_sp < stack_top && current_sp <= 0xFFFE) {
            uint16_t value = memory.read_word(current_sp);
            
            // Check if this looks like a return address (in code range)
            if (value >= 0x0000 && value < 0x0100) {
                std::cout << "Frame " << frame_num << ":" << std::endl;
                std::cout << "  Return Address: 0x" << std::hex << std::setw(4) 
                          << std::setfill('0') << value << std::dec << std::endl;
                std::cout << "  Frame Start: 0x" << std::hex << std::setw(4) 
                          << std::setfill('0') << current_sp << std::dec << std::endl;
                
                // Check for saved values above this frame
                if (current_sp < 0xFFFE) {
                    uint16_t next_value = memory.read_word(current_sp + 2);
                    std::cout << "  Saved Value: 0x" << std::hex << std::setw(4) 
                              << std::setfill('0') << next_value << " (" << std::dec 
                              << static_cast<int16_t>(next_value) << ")" << std::endl;
                }
                std::cout << std::endl;
                frame_num++;
            }
            
            current_sp += 2;
            if (current_sp > stack_top) break;
        }
        
        if (frame_num == 1) {
            std::cout << "No stack frames detected (no return addresses found)" << std::endl;
        }
        std::cout << std::dec;
    }
};

} // namespace emulator

