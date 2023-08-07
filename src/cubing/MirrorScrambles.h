#pragma once
#include <string>
#include <regex>

// this code is to be extensively tested
namespace testing {

// if m is UFM move to be mirrored, returns mirrored, else returns same m
std::string mirrorUfmMoveLr(std::string m) {
    std::regex pattern;

    pattern = std::regex("^[234]?[UFDBufdbSMEsmexyz]w?2$");
    if (std::regex_match(m, pattern))
        return m;
    if (m == "l2")
        return "r2";
    if (m == "r2")
        return "l2";
    pattern = std::regex("^[234]?Lw?2$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("L"), "R");
    pattern = std::regex("^[234]?Rw?2$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("R"), "L");

    pattern = std::regex("^[234]?Lw?$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("L"), "R") + "'";
    pattern = std::regex("^[234]?Rw?$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("R"), "L") + "'";

    pattern = std::regex("^[234]?Lw?'$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("L"), "R").substr(0, m.length() - 1);
    pattern = std::regex("^[234]?Rw?'$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("R"), "L").substr(0, m.length() - 1);
    if (m == "l'")
        return "r";
    if (m == "l")
        return "r'";
    if (m == "r'")
        return "l";
    if (m == "r")
        return "l'";

    pattern = std::regex("^[xmM]'?$");
    if (std::regex_match(m, pattern))
        return m;

    pattern = std::regex("^[234]?[UFDBufdbSMEsmexyz]w?$");
    if (std::regex_match(m, pattern))
        return m + "'";
    pattern = std::regex("^[234]?[UFDBufdbSMEsmexyz]w?'$");
    if (std::regex_match(m, pattern))
        return m.substr(0, m.length() - 1);

    return m;
}

std::string mirrorUfmMoveUd(std::string m) {
    std::regex pattern;

    pattern = std::regex("^[234]?[LRFBlrfbSMEsmexyz]w?2$");
    if (std::regex_match(m, pattern))
        return m;
    if (m == "u2")
        return "d2";
    if (m == "d2")
        return "u2";
    pattern = std::regex("^[234]?Uw?2$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("U"), "D");
    pattern = std::regex("^[234]?Dw?2$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("D"), "U");

    pattern = std::regex("^[234]?Uw?$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("U"), "D") + "'";
    pattern = std::regex("^[234]?Dw?$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("D"), "U") + "'";

    pattern = std::regex("^[234]?Uw?'$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("U"), "D").substr(0, m.length() - 1);
    pattern = std::regex("^[234]?Dw?'$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("D"), "U").substr(0, m.length() - 1);
    if (m == "u'")
        return "d";
    if (m == "u")
        return "d'";
    if (m == "d'")
        return "u";
    if (m == "d")
        return "u'";

    pattern = std::regex("^[yEe]'?$");
    if (std::regex_match(m, pattern))
        return m;

    pattern = std::regex("^[234]?[LFRBlfrbSMEsmexyz]w?$");
    if (std::regex_match(m, pattern))
        return m + "'";
    pattern = std::regex("^[234]?[LFRBlfrbSMEsmexyz]w?'$");
    if (std::regex_match(m, pattern))
        return m.substr(0, m.length() - 1);

    return m;
}

std::string mirrorUfmMoveFb(std::string m) {
    std::regex pattern;

    pattern = std::regex("^[234]?[LRUDlrudSMEsmexyz]w?2$");
    if (std::regex_match(m, pattern))
        return m;
    if (m == "f2")
        return "b2";
    if (m == "b2")
        return "f2";
    pattern = std::regex("^[234]?Fw?2$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("F"), "B");
    pattern = std::regex("^[234]?Bw?2$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("B"), "F");

    pattern = std::regex("^[234]?Fw?$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("F"), "B") + "'";
    pattern = std::regex("^[234]?Bw?$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("B"), "F") + "'";

    pattern = std::regex("^[234]?Fw?'$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("F"), "B").substr(0, m.length() - 1);
    pattern = std::regex("^[234]?Bw?'$");
    if (std::regex_match(m, pattern))
        return std::regex_replace(m, std::regex("B"), "F").substr(0, m.length() - 1);
    if (m == "f'")
        return "b";
    if (m == "f")
        return "b'";
    if (m == "b'")
        return "f";
    if (m == "b")
        return "f'";

    pattern = std::regex("^[Ssz]'?$");
    if (std::regex_match(m, pattern))
        return m;

    pattern = std::regex("^[234]?[LURDlurdSMEsmexyz]w?$");
    if (std::regex_match(m, pattern))
        return m + "'";
    pattern = std::regex("^[234]?[LURDlurdSMEsmexyz]w?'$");
    if (std::regex_match(m, pattern))
        return m.substr(0, m.length() - 1);

    return m;
}

}