#include "StringFunctions.h"

std::string ws2s(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
    return converterX.to_bytes(wstr);
}

std::wstring s2ws(std::string value) {
    const size_t cSize = value.size() + 1;

    std::wstring wc;
    wc.resize(cSize);

    size_t cSize1;
    mbstowcs_s(&cSize1, (wchar_t*)&wc[0], cSize, value.c_str(), cSize);

    wc.pop_back();

    return wc;
}
