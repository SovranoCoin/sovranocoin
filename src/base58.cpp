// Copyright (c) 2014 The Bitcoin developers
// Copyright (c) 2017-2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "base58.h"

#include "hash.h"
#include "uint256.h"

#include <assert.h>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <sstream>
#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>

/** All alphanumeric characters except for "0", "I", "O", and "l" */
static const char* pszBase58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

bool DecodeBase58(const char* psz, std::vector<unsigned char>& vch)
{
    // Skip leading spaces.
    while (*psz && isspace(*psz))
        psz++;
    // Skip and count leading '1's.
    int zeroes = 0;
    int length = 0;
    while (*psz == '1') {
        zeroes++;
        psz++;
    }
    // Allocate enough space in big-endian base256 representation.
    int size = strlen(psz) * 733 /1000 + 1; // log(58) / log(256), rounded up.
    std::vector<unsigned char> b256(size);
    // Process the characters.
    while (*psz && !isspace(*psz)) {
        // Decode base58 character
        const char* ch = strchr(pszBase58, *psz);
        if (ch == NULL)
            return false;
        // Apply "b256 = b256 * 58 + ch".
        int carry = ch - pszBase58;
        int i = 0;
        for (std::vector<unsigned char>::reverse_iterator it = b256.rbegin(); (carry != 0 || i < length) && (it != b256.rend()); ++it, ++i) {
            carry += 58 * (*it);
            *it = carry % 256;
            carry /= 256;
        }
        assert(carry == 0);
        length = i;
        psz++;
    }
    // Skip trailing spaces.
    while (isspace(*psz))
        psz++;
    if (*psz != 0)
        return false;
    // Skip leading zeroes in b256.
    std::vector<unsigned char>::iterator it = b256.begin() + (size - length);
    while (it != b256.end() && *it == 0)
        it++;
    // Copy result into output vector.
    vch.reserve(zeroes + (b256.end() - it));
    vch.assign(zeroes, 0x00);
    while (it != b256.end())
        vch.push_back(*(it++));
    return true;
}

std::string DecodeBase58(const char* psz)
{
    std::vector<unsigned char> vch;
    DecodeBase58(psz, vch);
    std::stringstream ss;
    ss << std::hex;

    for (unsigned int i = 0; i < vch.size(); i++) {
        unsigned char* c = &vch[i];
        ss << std::setw(2) << std::setfill('0') << (int)c[0];
    }

    return ss.str();
}

std::string EncodeBase58(const unsigned char* pbegin, const unsigned char* pend)
{
    // Skip & count leading zeroes.
    int zeroes = 0;
    int length = 0;
    while (pbegin != pend && *pbegin == 0) {
        pbegin++;
        zeroes++;
    }
    // Allocate enough space in big-endian base58 representation.
    int size = (pend - pbegin) * 138 / 100 + 1; // log(256) / log(58), rounded up.
    std::vector<unsigned char> b58(size);
    // Process the bytes.
    while (pbegin != pend) {
        int carry = *pbegin;
        int i = 0;
        // Apply "b58 = b58 * 256 + ch".
        for (std::vector<unsigned char>::reverse_iterator it = b58.rbegin(); (carry != 0 || i < length) && (it != b58.rend()); it++, i++) {
            carry += 256 * (*it);
            *it = carry % 58;
            carry /= 58;
        }

        assert(carry == 0);
        length = i;
        pbegin++;
    }
    // Skip leading zeroes in base58 result.
    std::vector<unsigned char>::iterator it = b58.begin() + (size - length);
    while (it != b58.end() && *it == 0)
        it++;
    // Translate the result into a string.
    std::string str;
    str.reserve(zeroes + (b58.end() - it));
    str.assign(zeroes, '1');
    while (it != b58.end())
        str += pszBase58[*(it++)];
    return str;
}

std::string EncodeBase58(const std::vector<unsigned char>& vch)
{
    return EncodeBase58(&vch[0], &vch[0] + vch.size());
}

bool DecodeBase58(const std::string& str, std::vector<unsigned char>& vchRet)
{
    return DecodeBase58(str.c_str(), vchRet);
}

std::string EncodeBase58Check(const std::vector<unsigned char>& vchIn)
{
    // add 4-byte hash check to the end
    std::vector<unsigned char> vch(vchIn);
    uint256 hash = Hash(vch.begin(), vch.end());
    vch.insert(vch.end(), (unsigned char*)&hash, (unsigned char*)&hash + 4);
    return EncodeBase58(vch);
}

bool DecodeBase58Check(const char* psz, std::vector<unsigned char>& vchRet)
{
    if (!DecodeBase58(psz, vchRet) ||
        (vchRet.size() < 4)) {
        vchRet.clear();
        return false;
    }
    // re-calculate the checksum, insure it matches the included 4-byte checksum
    uint256 hash = Hash(vchRet.begin(), vchRet.end() - 4);
    if (memcmp(&hash, &vchRet[vchRet.size() - 4], 4) != 0) {
        vchRet.clear();
        return false;
    }
    vchRet.resize(vchRet.size() - 4);
    return true;
}

bool DecodeBase58Check(const std::string& str, std::vector<unsigned char>& vchRet)
{
    return DecodeBase58Check(str.c_str(), vchRet);
}

CBase58Data::CBase58Data()
{
    vchVersion.clear();
    vchData.clear();
}

void CBase58Data::SetData(const std::vector<unsigned char>& vchVersionIn, const void* pdata, size_t nSize)
{
    vchVersion = vchVersionIn;
    vchData.resize(nSize);
    if (!vchData.empty())
        memcpy(&vchData[0], pdata, nSize);
}

void CBase58Data::SetData(const std::vector<unsigned char>& vchVersionIn, const unsigned char* pbegin, const unsigned char* pend)
{
    SetData(vchVersionIn, (void*)pbegin, pend - pbegin);
}

bool CBase58Data::SetString(const char* psz, unsigned int nVersionBytes)
{
    std::vector<unsigned char> vchTemp;
    bool rc58 = DecodeBase58Check(psz, vchTemp);
    if ((!rc58) || (vchTemp.size() < nVersionBytes)) {
        vchData.clear();
        vchVersion.clear();
        return false;
    }
    vchVersion.assign(vchTemp.begin(), vchTemp.begin() + nVersionBytes);
    vchData.resize(vchTemp.size() - nVersionBytes);
    if (!vchData.empty())
        memcpy(&vchData[0], &vchTemp[nVersionBytes], vchData.size());
    memory_cleanse(&vchTemp[0], vchTemp.size());
    return true;
}

bool CBase58Data::SetString(const std::string& str)
{
    if (str.empty())
        return false;
    return SetString(str.c_str());
}

std::string CBase58Data::ToString() const
{
    std::vector<unsigned char> vch = vchVersion;
    vch.insert(vch.end(), vchData.begin(), vchData.end());
    return EncodeBase58Check(vch);
}

int CBase58Data::CompareTo(const CBase58Data& b58) const
{
    if (vchVersion < b58.vchVersion)
        return -1;
    if (vchVersion > b58.vchVersion)
        return 1;
    if (vchData < b58.vchData)
        return -1;
    if (vchData > b58.vchData)
        return 1;
    return 0;
}

namespace
{
class CBitcoinAddressVisitor : public boost::static_visitor<bool>
{
private:
    CBitcoinAddress* addr;
    CChainParams::Base58Type type;

public:
    CBitcoinAddressVisitor(CBitcoinAddress* addrIn,
            const CChainParams::Base58Type addrType = CChainParams::PUBKEY_ADDRESS) :
        addr(addrIn),
        type(addrType){};

    bool operator()(const CKeyID& id) const { return addr->Set(id, type); }
    bool operator()(const CScriptID& id) const { return addr->Set(id); }
    bool operator()(const CNoDestination& no) const { return false; }
};

} // anon namespace

bool CBitcoinAddress::Set(const CKeyID& id, const CChainParams::Base58Type addrType)
{
    SetData(Params().Base58Prefix(addrType), &id, 20);
    return true;
}

bool CBitcoinAddress::Set(const CScriptID& id)
{
    SetData(Params().Base58Prefix(CChainParams::SCRIPT_ADDRESS), &id, 20);
    return true;
}

bool CBitcoinAddress::Set(const CTxDestination& dest, const CChainParams::Base58Type addrType)
{
    return boost::apply_visitor(CBitcoinAddressVisitor(this, addrType), dest);
}

bool CBitcoinAddress::IsValid() const
{
    return IsValid(Params());
}

bool CBitcoinAddress::IsValid(const CChainParams& params) const
{
    bool fCorrectSize = vchData.size() == 20;
    bool fKnownVersion = vchVersion == params.Base58Prefix(CChainParams::PUBKEY_ADDRESS) ||
                         vchVersion == params.Base58Prefix(CChainParams::SCRIPT_ADDRESS) ||
                         vchVersion == params.Base58Prefix(CChainParams::STAKING_ADDRESS);
    return fCorrectSize && fKnownVersion;
}

CTxDestination CBitcoinAddress::Get() const
{
    if (!IsValid())
        return CNoDestination();
    uint160 id;
    memcpy(&id, &vchData[0], 20);
    if (vchVersion == Params().Base58Prefix(CChainParams::PUBKEY_ADDRESS) ||
            vchVersion == Params().Base58Prefix(CChainParams::STAKING_ADDRESS))
        return CKeyID(id);
    else if (vchVersion == Params().Base58Prefix(CChainParams::SCRIPT_ADDRESS))
        return CScriptID(id);
    else
        return CNoDestination();
}

bool CBitcoinAddress::GetKeyID(CKeyID& keyID) const
{
    if (!IsValid() ||
            (vchVersion != Params().Base58Prefix(CChainParams::PUBKEY_ADDRESS) &&
            vchVersion != Params().Base58Prefix(CChainParams::STAKING_ADDRESS)))
        return false;
    uint160 id;
    memcpy(&id, &vchData[0], 20);
    keyID = CKeyID(id);
    return true;
}

bool CBitcoinAddress::IsScript() const
{
    bool fCorrectSize = vchData.size() == 20;
    return fCorrectSize && vchVersion == Params().Base58Prefix(CChainParams::SCRIPT_ADDRESS);
}

bool CBitcoinAddress::IsStakingAddress() const
{
    bool fCorrectSize = vchData.size() == 20;
    return fCorrectSize && vchVersion == Params().Base58Prefix(CChainParams::STAKING_ADDRESS);
}

CKey DecodeSecret(const std::string& str)
{
    CKey key;
    std::vector<unsigned char> data;
    if (DecodeBase58Check(str, data)) {
        const std::vector<unsigned char>& privkey_prefix = Params().Base58Prefix(CChainParams::SECRET_KEY);
        if ((data.size() == 32 + privkey_prefix.size() || (data.size() == 33 + privkey_prefix.size() && data.back() == 1)) &&
            std::equal(privkey_prefix.begin(), privkey_prefix.end(), data.begin())) {
            bool compressed = data.size() == 33 + privkey_prefix.size();
            key.Set(data.begin() + privkey_prefix.size(), data.begin() + privkey_prefix.size() + 32, compressed);
        }
    }
    memory_cleanse(data.data(), data.size());
    return key;
}

std::string EncodeSecret(const CKey& key)
{
    assert(key.IsValid());
    std::vector<unsigned char> data = Params().Base58Prefix(CChainParams::SECRET_KEY);
    data.insert(data.end(), key.begin(), key.end());
    if (key.IsCompressed()) {
        data.push_back(1);
    }
    std::string ret = EncodeBase58Check(data);
    memory_cleanse(data.data(), data.size());
    return ret;
}

CTxDestination DestinationFor(const CKeyID& keyID, const CChainParams::Base58Type addrType)
{
    CBitcoinAddress addr(keyID, addrType);
    if (!addr.IsValid()) throw std::runtime_error("Error, trying to decode an invalid keyID");
    return addr.Get();
}

std::string EncodeDestination(const CTxDestination& dest, bool isStaking)
{
    return EncodeDestination(dest, isStaking ? CChainParams::STAKING_ADDRESS : CChainParams::PUBKEY_ADDRESS);
}

std::string EncodeDestination(const CTxDestination& dest, const CChainParams::Base58Type addrType)
{
    CBitcoinAddress addr(dest, addrType);
    if (!addr.IsValid()) return "";
    return addr.ToString();
}

CTxDestination DecodeDestination(const std::string& str)
{
    return CBitcoinAddress(str).Get();
}

CTxDestination DecodeDestination(const std::string& str, bool& isStaking)
{
    CBitcoinAddress addr(str);
    isStaking = addr.IsStakingAddress();
    return addr.Get();
}

bool IsValidDestinationString(const std::string& str, bool fStaking, const CChainParams& params)
{
    return IsValidDestinationString(str, fStaking);
}

bool IsValidDestinationString(const std::string& str)
{
    CBitcoinAddress address(str);
    return address.IsValid();
}

bool IsValidDestinationString(const std::string& str, bool isStaking)
{
    CBitcoinAddress address(str);
    return address.IsValid() && (address.IsStakingAddress() == isStaking);
}
