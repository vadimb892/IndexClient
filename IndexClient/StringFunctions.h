#pragma once
#ifndef STRINGFUNCTION_H
#define STRINGFUNCTION_H

#include <vector>
#include <string>
#include <codecvt>
#include <iostream>

std::string ws2s(const std::wstring& wstr);

std::wstring s2ws(std::string value);

#endif