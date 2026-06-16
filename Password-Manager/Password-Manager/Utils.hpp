#pragma once

namespace utils {
	int gcd(int a, int b);
	int mod(int value, int module);
	int modInverse(int a, int module);
	char* readFile(const char* filePath);
	char* intToString(int a);
}