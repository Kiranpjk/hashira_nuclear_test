#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <numeric>

using namespace std;

// Custom BigInt structure to handle numbers larger than 64-bit
struct BigInt {
    string number;
    bool is_negative;

    BigInt(long long n = 0) : number(to_string(abs(n))), is_negative(n < 0) {}
    BigInt(string s) {
        if (s.empty() || s == "0") {
            number = "0";
            is_negative = false;
        } else if (s[0] == '-') {
            number = s.substr(1);
            is_negative = true;
        } else {
            number = s;
            is_negative = false;
        }
    }

    // Removes leading zeros
    void trim() {
        size_t first_digit = number.find_first_not_of('0');
        if (string::npos != first_digit) {
            number = number.substr(first_digit);
        } else {
            number = "0";
            is_negative = false;
        }
        if (number == "0") {
            is_negative = false;
        }
    }
};

// --- BigInt Comparison Operators ---
bool operator<(const BigInt& a, const BigInt& b) {
    if (a.is_negative != b.is_negative) return a.is_negative;
    if (a.is_negative) { // Both are negative
        if (a.number.length() != b.number.length()) return a.number.length() > b.number.length();
        return a.number > b.number;
    }
    // Both are positive
    if (a.number.length() != b.number.length()) return a.number.length() < b.number.length();
    return a.number < b.number;
}

bool operator==(const BigInt& a, const BigInt& b) {
    return a.number == b.number && a.is_negative == b.is_negative;
}

// --- BigInt Arithmetic Declarations ---
BigInt add(const BigInt& a, const BigInt& b);
BigInt subtract(const BigInt& a, const BigInt& b);

// --- BigInt Helper Functions for Arithmetic ---
BigInt add_unsigned(const string& s1, const string& s2) {
    string result = "";
    int i = s1.length() - 1, j = s2.length() - 1, carry = 0;
    while (i >= 0 || j >= 0 || carry) {
        int sum = carry + (i >= 0 ? s1[i--] - '0' : 0) + (j >= 0 ? s2[j--] - '0' : 0);
        result += to_string(sum % 10);
        carry = sum / 10;
    }
    reverse(result.begin(), result.end());
    return BigInt(result);
}

BigInt subtract_unsigned(string s1, string s2) {
    string result = "";
    int i = s1.length() - 1, j = s2.length() - 1, borrow = 0;
    while (i >= 0) {
        int diff = (s1[i] - '0') - (j >= 0 ? s2[j--] - '0' : 0) - borrow;
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result += to_string(diff);
        i--;
    }
    reverse(result.begin(), result.end());
    BigInt res(result);
    res.trim();
    return res;
}

// --- BigInt Main Arithmetic Functions ---
BigInt add(const BigInt& a, const BigInt& b) {
    if (a.is_negative == b.is_negative) {
        BigInt result = add_unsigned(a.number, b.number);
        result.is_negative = a.is_negative;
        return result;
    }
    if (BigInt(a.number) < BigInt(b.number)) {
        BigInt result = subtract_unsigned(b.number, a.number);
        result.is_negative = b.is_negative;
        return result;
    }
    BigInt result = subtract_unsigned(a.number, b.number);
    result.is_negative = a.is_negative;
    return result;
}

BigInt subtract(const BigInt& a, const BigInt& b) {
    BigInt neg_b = b;
    neg_b.is_negative = !b.is_negative;
    if (b.number == "0") neg_b.is_negative = false;
    return add(a, neg_b);
}

BigInt multiply(const BigInt& a, const BigInt& b) {
    if (a.number == "0" || b.number == "0") return BigInt(0);
    string s1 = a.number;
    string s2 = b.number;
    vector<int> res(s1.length() + s2.length(), 0);
    int i_n1 = 0;
    for (int i = s1.length() - 1; i >= 0; i--) {
        int carry = 0;
        int n1 = s1[i] - '0';
        int i_n2 = 0;
        for (int j = s2.length() - 1; j >= 0; j--) {
            int n2 = s2[j] - '0';
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
    if (i == -1) return BigInt(0);
    string s = "";
    while (i >= 0) s += to_string(res[i--]);
    BigInt result(s);
    result.is_negative = a.is_negative != b.is_negative;
    return result;
}

// Custom Fraction structure for precise division using BigInts
struct Fraction {
    BigInt num, den;

    Fraction(BigInt n = 0, BigInt d = 1) : num(n), den(d) {
        if (den.number == "0") throw runtime_error("Division by zero");
    }
};

// --- Fraction Arithmetic ---
Fraction add(const Fraction& a, const Fraction& b) {
    BigInt new_num = add(multiply(a.num, b.den), multiply(b.num, a.den));
    BigInt new_den = multiply(a.den, b.den);
    return Fraction(new_num, new_den);
}

// --- Core Application Logic ---

// Converts a string value in a given base to a BigInt
BigInt baseToBigInt(const string& val_str, int base) {
    BigInt result(0);
    for (char c : val_str) {
        int digit;
        if (c >= '0' && c <= '9') digit = c - '0';
        else if (c >= 'a' && c <= 'z') digit = 10 + (c - 'a');
        else if (c >= 'A' && c <= 'Z') digit = 10 + (c - 'A');
        else throw runtime_error("Invalid character in base conversion");

        if (digit >= base) {
            throw runtime_error("Invalid digit for given base");
        }

        result = multiply(result, BigInt(base));
        result = add(result, BigInt(digit));
    }
    return result;
}

// Correct Lagrange Interpolation using Fractions and BigInts
BigInt lagrangeInterpolationAtZero(const vector<pair<BigInt, BigInt>>& points) {
    Fraction total_sum(0);
    int k = points.size();

    for (int i = 0; i < k; ++i) {
        const BigInt& yi = points[i].second;
        BigInt basis_num(1);
        BigInt basis_den(1);

        for (int j = 0; j < k; ++j) {
            if (i == j) continue;
            
            BigInt neg_xj = points[j].first;
            neg_xj.is_negative = !neg_xj.is_negative;
            if (neg_xj.number == "0") neg_xj.is_negative = false;

            basis_num = multiply(basis_num, neg_xj);
            basis_den = multiply(basis_den, subtract(points[i].first, points[j].first));
        }
        
        BigInt term_num = multiply(basis_num, yi);
        total_sum = add(total_sum, Fraction(term_num, basis_den));
    }
    
    // Check if the final result is a whole number by performing division
    BigInt q(0);
    if (total_sum.den.number != "0") {
        string numStr = total_sum.num.number;
        string denStr = total_sum.den.number;

        BigInt dividend(numStr);
        BigInt divisor(denStr);

        if (BigInt(numStr) < BigInt(denStr)) { // If numerator < denominator
             if(dividend.number != "0") throw runtime_error("Result is a fraction.");
        } else {
             BigInt r(0);
             string temp = "";
             for(char c : numStr) {
                 temp += c;
                 BigInt tempBigInt(temp);
                 int count = 0;
                 while(!(tempBigInt < divisor)) {
                     tempBigInt = subtract(tempBigInt, divisor);
                     count++;
                 }
                 q = add(multiply(q, BigInt(10)), BigInt(count));
                 temp = tempBigInt.number;
             }
             r = BigInt(temp);

             if (r.number != "0") {
                throw runtime_error("Final secret is not an integer.");
             }
        }
    } else {
        throw runtime_error("Denominator is zero in final fraction.");
    }

    q.is_negative = total_sum.num.is_negative != total_sum.den.is_negative;
    return q;
}

// Generates all combinations of size k from 0..n-1
void generateCombinations(int n, int k, int start, vector<int>& current, vector<vector<int>>& result) {
    if (current.size() == k) {
        result.push_back(current);
        return;
    }
    for (int i = start; i < n; ++i) {
        current.push_back(i);
        generateCombinations(n, k, i + 1, current, result);
        current.pop_back();
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string json_content, line;
    while(getline(cin, line)) {
        json_content += line;
    }
    
    int n = stoi(json_content.substr(json_content.find("\"n\"") + 5));
    int k = stoi(json_content.substr(json_content.find("\"k\"") + 5));
    
    vector<pair<BigInt, BigInt>> all_shares;
    
    for (int i = 1; i <= n; ++i) {
        string key_to_find = "\"" + to_string(i) + "\"";
        size_t share_pos = json_content.find(key_to_find);

        if (share_pos == string::npos) continue;

        try {
            size_t base_pos = json_content.find("\"base\"", share_pos);
            size_t value_pos = json_content.find("\"value\"", share_pos);
            
            string base_str = json_content.substr(json_content.find(':', base_pos) + 1);
            base_str = base_str.substr(base_str.find_first_of("0123456789"));
            base_str = base_str.substr(0, base_str.find_first_not_of("0123456789"));
            
            string value_str = json_content.substr(json_content.find(':', value_pos) + 1);
            value_str = value_str.substr(value_str.find('\"') + 1);
            value_str = value_str.substr(0, value_str.find('\"'));
            
            // Convert and add the share
            all_shares.push_back({BigInt(i), baseToBigInt(value_str, stoi(base_str))});
        
        } catch (const runtime_error& e) {
            // This catches errors from baseToBigInt (e.g., invalid digit for base).
            // We simply ignore this corrupt share and continue to the next one.
            // cerr << "Warning: Skipping corrupt share " << i << " due to parsing error." << endl;
        }
    }

    vector<vector<int>> combinations;
    vector<int> current_combination;
    // Generate combinations from the list of VALID shares we collected
    generateCombinations(all_shares.size(), k, 0, current_combination, combinations);

    map<string, int> secret_frequencies;

    for (const auto& comb_indices : combinations) {
        vector<pair<BigInt, BigInt>> subset;
        for (int index : comb_indices) {
            subset.push_back(all_shares[index]);
        }
        
        try {
            BigInt secret = lagrangeInterpolationAtZero(subset);
            string secret_str = (secret.is_negative ? "-" : "") + secret.number;
            secret_frequencies[secret_str]++;
        } catch (const runtime_error& e) {
            // Ignore this combination as it resulted in a non-integer secret
        }
    }

    string correct_secret_str = "";
    int max_freq = 0;
    for (const auto& pair : secret_frequencies) {
        if (pair.second > max_freq) {
            max_freq = pair.second;
            correct_secret_str = pair.first;
        }
    }

    cout << correct_secret_str << endl;

    return 0;
}
