#include "stdafx.h"

Pattern::Pattern(const std::string& pattern, const std::string& mask, Game& game, char* start, char* end)
	: pattern{ pattern }, mask{ mask }, game{ game }, start{ start }, end{ end }
{
}

Pattern::~Pattern()
{
}

void* Pattern::MemoryScan()
{
    // If no module name is given, skip to the HeapScan function
    if(!game.GetModuleName().empty())
    {
        // Use the MODULEENTRY32 struct to get the module's base address and size
        MODULEENTRY32 module_entry = game.GetModule();
        char* start = (char*)module_entry.modBaseAddr;
        char* end = start + module_entry.modBaseSize;
    }

	return HeapScan();
}

void* Pattern::HeapScan()
{
    char* match{ nullptr };

    DWORD old_protection;
    std::vector<char> buffer;

    // Initialize an MBI struct and set the RegionSize to 4096 bytes (size of a page)
    MEMORY_BASIC_INFORMATION mem_info{};
    mem_info.RegionSize = 0x1000;

    // Look up the information of the first 4096 bytes starting from the 'start' pointer
    ::VirtualQueryEx(game.GetProcHandle(), start, &mem_info, sizeof(MEMORY_BASIC_INFORMATION));

    for (char* current_block = start; current_block < end; current_block += mem_info.RegionSize)
    {
        // If the function fails to retrieve any information, skip to the next block
        if (!::VirtualQueryEx(game.GetProcHandle(), current_block, &mem_info, sizeof(MEMORY_BASIC_INFORMATION)))
            continue;

        // If the memory is not commited or is not accessible, skip to the next block
        if (mem_info.State != MEM_COMMIT || mem_info.Protect == PAGE_NOACCESS)
            continue;

        buffer.resize(mem_info.RegionSize);

        // If the VirtualProtect function doesn't fail, continue with the code below, otherwise skip to the next block
        if (::VirtualProtectEx(game.GetProcHandle(), mem_info.BaseAddress, mem_info.RegionSize, PAGE_EXECUTE_READWRITE, &old_protection))
        {
            // Store the entire memory region in the buffer
            ::ReadProcessMemory(game.GetProcHandle(), mem_info.BaseAddress, buffer.data(), mem_info.RegionSize, NULL);

            // Restore the old protection of the region
            ::VirtualProtectEx(game.GetProcHandle(), mem_info.BaseAddress, mem_info.RegionSize, old_protection, &old_protection);

            // Call the PatternMatch function on the region and try to find if the pattern is inside
            char* internal_address = (char*)PatternMatch(buffer);

            // If the pattern is found, calculate its proper offset
            if (internal_address != nullptr)
            {
                // Calculate the distance between the pattern and the start of the region, then add it to the current block          
                match = current_block + (internal_address - buffer.data());
                break;
            }
        }
    }
    return (void*)match;
}

void* Pattern::PatternMatch(const std::vector<char>& buffer)
{
    for (size_t i = 0; i < buffer.size() - pattern.size(); i++)
    {
        bool found{ true };

        for (size_t j = 0; j < pattern.size(); j++)
        {
            if (mask.at(j) == 'x' && pattern.at(j) != buffer.at(i + j))
            {
                found = false;
                break;
            }
        }

        if (found)
        {
            return (void*)(buffer.data() + i);
        }
    }
    return nullptr;
}
