#ifndef _REGKEY_H_
#define _REGKEY_H_

//. soyaviper

#pragma warning(disable : 4786)
#include <string>

#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// Android stub: CRegKey reads/writes a simple key=value flat file stored in
// the game data directory.  Values are stored as decimal integers or strings.
// The file path is <dataDir>/registry/<subkey>.reg
// ─────────────────────────────────────────────────────────────────────────────
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <unordered_map>
#include "../../Platform/GameAssetPath.h"

namespace leaf {
class CRegKey {
public:
    enum _HKEY { _HKEY_CLASSES_ROOT=0,_HKEY_CURRENT_CONFIG=1,
                 _HKEY_CURRENT_USER=2,_HKEY_LOCAL_MACHINE=3,_HKEY_USERS=4 };

    CRegKey(){}
    ~CRegKey(){}

    void SetKey(_HKEY, const std::string& subkey) { m_subkey = subkey; Load(); }

    bool ReadDword(const std::string& name, uint32_t& value) {
        auto it = m_data.find(name);
        if (it == m_data.end()) return false;
        value = (uint32_t)std::stoul(it->second);
        return true;
    }
    bool ReadInterget(const std::string& name, int& value) {
        auto it = m_data.find(name);
        if (it == m_data.end()) return false;
        value = std::stoi(it->second);
        return true;
    }
    bool ReadBinary(const std::string& name, uint8_t* value, uint32_t sz) {
        auto it = m_data.find(name);
        if (it == m_data.end()) return false;
        // stored as hex string
        const std::string& s = it->second;
        for (uint32_t i = 0; i < sz && i*2+1 < s.size(); i++)
        {
            char hex[3] = {s[i*2], s[i*2+1], 0};
            value[i] = (uint8_t)std::stoul(hex, nullptr, 16);
        }
        return true;
    }
    bool ReadString(const std::string& name, std::string& value) {
        auto it = m_data.find(name);
        if (it == m_data.end()) return false;
        value = it->second;
        return true;
    }
    bool WriteDword(const std::string& name, uint32_t value) {
        m_data[name] = std::to_string(value); Save(); return true;
    }
    bool WriteInterget(const std::string& name, int value) {
        m_data[name] = std::to_string(value); Save(); return true;
    }
    bool WriteBinary(const std::string& name, uint8_t* value, uint32_t sz) {
        char buf[3]; std::string s;
        for (uint32_t i=0;i<sz;i++){snprintf(buf,3,"%02x",value[i]);s+=buf;}
        m_data[name]=s; Save(); return true;
    }
    bool WriteString(const std::string& name, const std::string& value) {
        m_data[name] = value; Save(); return true;
    }
    bool DeleteKey() { m_data.clear(); Save(); return true; }
    bool DeleteValue(const std::string& name) { m_data.erase(name); Save(); return true; }

private:
    std::string m_subkey;
    std::unordered_map<std::string,std::string> m_data;

    std::string FilePath() const {
        std::string p = GameAssetPath::Resolve("registry/") + m_subkey + ".reg";
        // sanitize path separators
        for (char& c : p) if (c=='\\') c='/';
        return p;
    }
    void Load() {
        m_data.clear();
        FILE* f = fopen(FilePath().c_str(), "r");
        if (!f) return;
        char line[512];
        while (fgets(line, sizeof(line), f)) {
            char* eq = strchr(line, '=');
            if (!eq) continue;
            *eq = '\0';
            std::string key(line);
            std::string val(eq+1);
            if (!val.empty() && val.back()=='\n') val.pop_back();
            m_data[key] = val;
        }
        fclose(f);
    }
    void Save() {
        std::string path = FilePath();
        // ensure dir exists
        size_t sl = path.rfind('/');
        if (sl != std::string::npos) {
            std::string dir = path.substr(0, sl);
            std::string tmp;
            for (char c : dir+'/') { tmp+=c; if(c=='/') mkdir(tmp.c_str(),0755); }
        }
        FILE* f = fopen(path.c_str(), "w");
        if (!f) return;
        for (auto& kv : m_data) fprintf(f, "%s=%s\n", kv.first.c_str(), kv.second.c_str());
        fclose(f);
    }
};
} // namespace leaf

#else // Windows

#include <windows.h>

namespace leaf{
	class CRegKey{
	public:
		enum _HKEY{ _HKEY_CLASSES_ROOT = (LONG)(ULONG_PTR)HKEY_CLASSES_ROOT,
					_HKEY_CURRENT_CONFIG = (LONG)(ULONG_PTR)HKEY_CURRENT_CONFIG,
					_HKEY_CURRENT_USER = (LONG)(ULONG_PTR)HKEY_CURRENT_USER,
					_HKEY_LOCAL_MACHINE = (LONG)(ULONG_PTR)HKEY_LOCAL_MACHINE,
					_HKEY_USERS = (LONG)(ULONG_PTR)HKEY_USERS
		};
		CRegKey(){}
		~CRegKey(){}

		void SetKey(_HKEY hKey, const std::string& subkey) {
			m_hKey = (HKEY)hKey;
			m_subkey = subkey;
		}
		bool ReadDword(const std::string& name, DWORD& value) {
			HKEY	hKey = NULL;
			DWORD	dwDisp;
			DWORD	dwType = REG_DWORD;
			DWORD	dwSize = sizeof(DWORD);
			
			if (ERROR_SUCCESS != RegCreateKeyEx(m_hKey, m_subkey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp))
				return false;
			
			if (ERROR_SUCCESS != RegQueryValueEx(hKey, name.c_str(), NULL, &dwType, (LPBYTE)&value, &dwSize)){
				RegCloseKey(hKey); 
				return false;
			}
			
			RegCloseKey(hKey);
			return true;
		}
		bool ReadInterget(const std::string& name, int& value) {
			HKEY	hKey = NULL;
			DWORD	dwDisp;
			DWORD	dwType = REG_DWORD;
			DWORD	dwSize = sizeof(int);

			if (ERROR_SUCCESS != RegCreateKeyEx(m_hKey, m_subkey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp))
				return false;

			if (ERROR_SUCCESS != RegQueryValueEx(hKey, name.c_str(), NULL, &dwType, (LPBYTE)&value, &dwSize)) {
				RegCloseKey(hKey);
				return false;
			}

			RegCloseKey(hKey);
			return true;
		}
		bool ReadBinary(const std::string& name, BYTE* value, DWORD	dwSize) {
			HKEY	hKey = NULL;
			DWORD	dwDisp;
			DWORD	dwType = REG_NONE;

			if (ERROR_SUCCESS != RegCreateKeyEx(m_hKey, m_subkey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp))
				return false;

			if (ERROR_SUCCESS != RegQueryValueEx(hKey, name.c_str(), NULL, &dwType, (LPBYTE)&value, &dwSize)) {
				RegCloseKey(hKey);
				return false;
			}

			RegCloseKey(hKey);
			return true;
		}
		bool ReadString(const std::string& name, std::string& value) {
			char	szTempKey[256];
			HKEY	hKey = NULL;
			DWORD	dwDisp;
			DWORD	dwType = REG_EXPAND_SZ;
			DWORD	dwSize = 256;
			
			if (ERROR_SUCCESS != RegCreateKeyEx(m_hKey, m_subkey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp))
				return false;
			
			if (ERROR_SUCCESS != RegQueryValueEx(hKey, name.c_str(), NULL, &dwType, (LPBYTE )szTempKey, &dwSize)){
				RegCloseKey(hKey);
				return false;
			}
			value = szTempKey;
			RegCloseKey(hKey);
			return true;
		}
		bool WriteDword(const std::string& name, DWORD value) {
			HKEY	hKey = NULL;
			DWORD	dwDisp;
			DWORD	dwSize = sizeof(DWORD);
			
			if (ERROR_SUCCESS != RegCreateKeyEx(m_hKey, m_subkey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp))
				return false;
			
			RegSetValueEx(hKey, name.c_str(), 0L, REG_DWORD, (BYTE*)&value, dwSize);
			RegCloseKey(hKey);
			return true;
		}
		bool WriteInterget(const std::string& name, int value) {
			HKEY	hKey = NULL;
			DWORD	dwDisp;
			DWORD	dwSize = sizeof(int);

			if (ERROR_SUCCESS != RegCreateKeyEx(m_hKey, m_subkey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp))
				return false;

			RegSetValueEx(hKey, name.c_str(), 0L, REG_DWORD, (BYTE*)&value, dwSize);
			RegCloseKey(hKey);
			return true;
		}
		bool WriteBinary(const std::string& name, BYTE* value, DWORD dwSize) {
			HKEY	hKey = NULL;
			DWORD	dwDisp;

			if (ERROR_SUCCESS != RegCreateKeyEx(m_hKey, m_subkey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp))
				return false;

			RegSetValueEx(hKey, name.c_str(), 0L, REG_BINARY, value, dwSize);
			RegCloseKey(hKey);
			return true;
		}
		bool WriteString(const std::string& name, const std::string& value) {
			HKEY	hKey = NULL;
			DWORD	dwDisp;
			DWORD	dwSize = value.size();
			
			if (ERROR_SUCCESS != RegCreateKeyEx(m_hKey, m_subkey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp))
				return false;
			
			RegSetValueEx(hKey, name.c_str(), 0L, REG_SZ, (CONST BYTE*)value.c_str(), dwSize);
			RegCloseKey(hKey);
			return true;
		}
		bool DeleteKey() {
			if (ERROR_SUCCESS != RegDeleteKey(m_hKey, m_subkey.c_str()))
				return false;
			return true;
		}
		bool DeleteValue(const std::string& name) {
			HKEY	hKey = NULL;
			if (ERROR_SUCCESS != RegOpenKeyEx(m_hKey, m_subkey.c_str(), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &hKey))
				return false;

			RegDeleteValue(hKey, name.c_str());
			RegCloseKey(hKey);
			return true;
		}

	private:
		HKEY m_hKey;
		std::string m_subkey;
	};
}

#endif // __ANDROID__ / Windows

#endif /* _REGKEY_H_ */
