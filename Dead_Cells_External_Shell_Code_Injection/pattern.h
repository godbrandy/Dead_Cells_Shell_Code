#pragma once
#include "stdafx.h"

class Pattern
{
public:
	Pattern(const std::string& pattern, 
		const std::string& mask, 
		Game& game, 
		char* start = nullptr, 
		char* end = nullptr);
	~Pattern();

	void* MemoryScan();
	void* HeapScan();
	void* PatternMatch(const std::vector<char>& buffer);
	
private:
	const std::string pattern;
	const std::string mask;
	char* start;
	char* end;
	Game& game;
};