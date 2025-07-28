#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <numeric>

class BigNumber {
public:
    std::string digits;
    bool isNegative;

    BigNumber(long long n = 0) {
        digits = std::to_string(std::abs(n));
        isNegative = n < 0;
    }

    BigNumber(std::string s) {
        if (s.empty() || s == "0") {
            digits = "0";
            isNegative = false;
        } else if (s[0] == '-') {
            digits = s.substr(1);
            isNegative = true;
        } else {
            digits = s;
            isNegative = false;
        }
        this->normalize();
    }

    void normalize() {
        size_t firstDigitPos = digits.find_first_not_of('0');
        if (std::string::npos != firstDigitPos) {
            digits = digits.substr(firstDigitPos);
        } else {
            digits = "0";
            isNegative = false;
        }
        if (digits == "0") {
            isNegative = false;
        }
    }
};

bool isSmallerUnsigned(const std::string& s1, const std::string& s2) {
    if (s1.length() != s2.length()) {
        return s1.length() < s2.length();
    }
    return s1 < s2;
}

BigNumber subtractUnsigned(std::string larger, std::string smaller) {
    std::string result = "";
    int len_l = larger.length();
    int len_s = smaller.length();
    std::reverse(larger.begin(), larger.end());
    std::reverse(smaller.begin(), smaller.end());
    int carry = 0;
    for (int i = 0; i < len_s; i++) {
        int sub = ((larger[i] - '0') - (smaller[i] - '0') - carry);
        if (sub < 0) {
            sub = sub + 10;
            carry = 1;
        } else {
            carry = 0;
        }
        result.push_back(sub + '0');
    }
    for (int i = len_s; i < len_l; i++) {
        int sub = ((larger[i] - '0') - carry);
        if (sub < 0) {
            sub = sub + 10;
            carry = 1;
        } else {
            carry = 0;
        }
        result.push_back(sub + '0');
    }
    std::reverse(result.begin(), result.end());
    return BigNumber(result);
}

BigNumber addUnsigned(std::string s1, std::string s2) {
    std::string str = "";
    int n1 = s1.length(), n2 = s2.length();
    std::reverse(s1.begin(), s1.end());
    std::reverse(s2.begin(), s2.end());
    int carry = 0;
    for (int i = 0; i < n1 || i < n2 || carry; i++) {
        int sum = carry + (i < n1 ? s1[i] - '0' : 0) + (i < n2 ? s2[i] - '0' : 0);
        str.push_back(sum % 10 + '0');
        carry = sum / 10;
    }
    std::reverse(str.begin(), str.end());
    return BigNumber(str);
}


BigNumber operator+(BigNumber a, BigNumber b) {
    if (a.isNegative == b.isNegative) {
        BigNumber result = addUnsigned(a.digits, b.digits);
        result.isNegative = a.isNegative;
        return result;
    }
    if (isSmallerUnsigned(a.digits, b.digits)) {
        BigNumber result = subtractUnsigned(b.digits, a.digits);
        result.isNegative = b.isNegative;
        return result;
    }
    BigNumber result = subtractUnsigned(a.digits, b.digits);
    result.isNegative = a.isNegative;
    return result;
}

BigNumber operator-(BigNumber a, BigNumber b) {
    b.isNegative = !b.isNegative;
    return a + b;
}

BigNumber operator*(BigNumber a, BigNumber b) {
    if (a.digits == "0" || b.digits == "0") return BigNumber(0);
    std::vector<int> res(a.digits.length() + b.digits.length(), 0);
    int i_n1 = 0, i_n2 = 0;
    for (int i = a.digits.length() - 1; i >= 0; i--) {
        int carry = 0;
        int n1 = a.digits[i] - '0';
        i_n2 = 0;
        for (int j = b.digits.length() - 1; j >= 0; j--) {
            int n2 = b.digits[j] - '0';
            int sum = n1 * n2 + res[i_n1 + i_n2] + carry;
            carry = sum / 10;
            res[i_n1 + i_n2] = sum % 10;
            i_n2++;
        }
        if (carry > 0) res[i_n1 + i_n2] += carry;
        i_n1++;
    }
    int i = res.size() - 1;
    while (i >= 0 && res[i] == 0) i--;
    if (i == -1) return BigNumber(0);
    std::string s = "";
    while (i >= 0) s += std::to_string(res[i--]);
    BigNumber result(s);
    result.isNegative = a.isNegative != b.isNegative;
    return result;
}

bool operator<(const BigNumber& a, const BigNumber& b){
    return isSmallerUnsigned(a.digits, b.digits);
}

struct Rational {
    BigNumber numerator;
    BigNumber denominator;

    Rational(BigNumber n = 0, BigNumber d = 1) : numerator(n), denominator(d) {
        if (d.digits == "0") throw std::runtime_error("Division by zero in Rational.");
    }
};

Rational operator+(Rational a, Rational b) {
    BigNumber newNum = (a.numerator * b.denominator) + (b.numerator * a.denominator);
    BigNumber newDen = a.denominator * b.denominator;
    return Rational(newNum, newDen);
}

BigNumber stringToBase(const std::string& val_str, int base) {
    BigNumber result(0);
    for (char c : val_str) {
        int digit;
        if (c >= '0' && c <= '9') digit = c - '0';
        else if (c >= 'a' && c <= 'f') digit = 10 + (c - 'a');
        else if (c >= 'A' && c <= 'F') digit = 10 + (c - 'A');
        else throw std::runtime_error("Invalid character for base conversion.");

        if (digit >= base) {
            throw std::runtime_error("Invalid digit for given base.");
        }
        result = (result * BigNumber(base)) + BigNumber(digit);
    }
    return result;
}

BigNumber solveLagrange(const std::vector<std::pair<BigNumber, BigNumber>>& points) {
    Rational totalSum(0);
    int k = points.size();

    for (int i = 0; i < k; ++i) {
        Rational basis(1);
        for (int j = 0; j < k; ++j) {
            if (i == j) continue;
            BigNumber neg_xj = points[j].first;
            neg_xj.isNegative = !neg_xj.isNegative;
            basis.numerator = basis.numerator * neg_xj;
            basis.denominator = basis.denominator * (points[i].first - points[j].first);
        }
        totalSum = totalSum + Rational(basis.numerator * points[i].second, basis.denominator);
    }

    if (totalSum.denominator.digits == "0") throw std::runtime_error("Denominator is zero.");
    
    BigNumber quotient(0), remainder(0);
    std::string temp_str = "";
    for(char c : totalSum.numerator.digits) {
        temp_str += c;
        BigNumber temp_dividend(temp_str);
        int count = 0;
        while(!(isSmallerUnsigned(temp_dividend.digits, totalSum.denominator.digits))) {
            temp_dividend = temp_dividend - totalSum.denominator;
            count++;
        }
        quotient = (quotient * BigNumber(10)) + BigNumber(count);
        temp_str = temp_dividend.digits == "0" ? "" : temp_dividend.digits;
    }
    remainder = BigNumber(temp_str);

    if (remainder.digits != "0") {
        throw std::runtime_error("Result is not a whole number.");
    }
    
    quotient.isNegative = totalSum.numerator.isNegative != totalSum.denominator.isNegative;
    quotient.normalize();
    return quotient;
}

void findCombinations(int offset, int k, std::vector<int>& combination, const std::vector<std::pair<BigNumber, BigNumber>>& all, std::vector<std::vector<std::pair<BigNumber, BigNumber>>>& result) {
    if (k == 0) {
        result.push_back(combination);
        return;
    }
    for (int i = offset; i <= all.size() - k; ++i) {
        combination.push_back(i);
        findCombinations(i + 1, k - 1, combination, all, result);
        combination.pop_back();
    }
}

std::string cleanJsonValue(const std::string& s) {
    size_t first = s.find_first_not_of(" \t\n\r,:}");
    size_t last = s.find_last_not_of(" \t\n\r,:}");
    std::string res = s.substr(first, last - first + 1);
    
    if (res.front() == '"') res.erase(0, 1);
    if (res.back() == '"') res.pop_back();

    return res;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::string full_input, current_line;
    while(getline(std::cin, current_line)) {
        full_input += current_line;
    }
    
    int n, k;
    try {
        size_t n_label = full_input.find("\"n\"");
        size_t k_label = full_input.find("\"k\"");
        size_t n_colon = full_input.find(':', n_label);
        size_t k_colon = full_input.find(':', k_label);
        size_t n_end = full_input.find_first_of(",}", n_colon);
        size_t k_end = full_input.find_first_of(",}", k_colon);
        n = std::stoi(full_input.substr(n_colon + 1, n_end - n_colon - 1));
        k = std::stoi(full_input.substr(k_colon + 1, k_end - k_colon - 1));
    } catch(const std::exception& e) {
        std::cerr << "Error parsing n and k values." << std::endl;
        return 1;
    }
    
    std::vector<std::pair<BigNumber, BigNumber>> valid_shares;
    
    for (int i = 1; i <= n; ++i) {
        std::string key_label = "\"" + std::to_string(i) + "\"";
        size_t share_block_start = full_input.find(key_label);
        if (share_block_start == std::string::npos) continue;

        try {
            size_t base_label = full_input.find("\"base\"", share_block_start);
            size_t value_label = full_input.find("\"value\"", share_block_start);
            size_t base_colon = full_input.find(':', base_label);
            size_t value_colon = full_input.find(':', value_label);
            size_t base_end = full_input.find_first_of(",}", base_colon);
            size_t value_end = full_input.find_first_of(",}", value_colon);

            std::string base_str = cleanJsonValue(full_input.substr(base_colon + 1, base_end - base_colon - 1));
            std::string value_str = cleanJsonValue(full_input.substr(value_colon + 1, value_end - value_colon - 1));

            valid_shares.push_back({BigNumber(i), stringToBase(value_str, std::stoi(base_str))});
        
        } catch (const std::exception& e) {
            // Ignore corrupt or invalid shares and continue.
        }
    }

    if (valid_shares.size() < k) {
        std::cerr << "Not enough valid shares to find the secret." << std::endl;
        return 1;
    }
    
    std::vector<int> combination;
    std::vector<std::vector<int>> indices_groups;
    findCombinations(0, k, combination, valid_shares, indices_groups);

    std::map<std::string, int> secret_tally;

    for (const auto& index_group : indices_groups) {
        std::vector<std::pair<BigNumber, BigNumber>> subset;
        for (int index : index_group) {
            subset.push_back(valid_shares[index]);
        }
        
        try {
            BigNumber secret = solveLagrange(subset);
            std::string secret_str = (secret.isNegative ? "-" : "") + secret.digits;
            secret_tally[secret_str]++;
        } catch (const std::exception& e) {
            // Ignore combinations that do not produce a whole number.
        }
    }

    std::string final_secret = "";
    int max_votes = 0;
    for (const auto& pair : secret_tally) {
        if (pair.second > max_votes) {
            max_votes = pair.second;
            final_secret = pair.first;
        }
    }

    std::cout << final_secret << std::endl;

    return 0;
}
