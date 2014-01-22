#ifndef _COOKIEHELPER_H_
#define _COOKIEHELPER_H_
#define WIN32_LEAN_AND_MEAN

#include <fstream>
#include <string>
#include <vector>
#include <string.h>
#include <assert.h>
#include <Windows.h>

using std::string;
using std::vector;
using std::ifstream;

typedef struct _CacheEntry
{
	string m_website;
	string m_fileName;
	string m_user;
	string m_localPath;
	long   m_cookieSize;    // the size of the cache entry
	int    m_hits;
	string m_lastAccess;
	string m_lastModified;
	int    m_recordNum;     // the order number in the hash section.
}CacheEntry, *LPCacheEntry;

//////////////////////////////////////////////////////////////////////////
// Head structure
struct CacheDir
{
	DWORD nFileCount;
	char sDirName[8];
};

typedef struct _MEMMAP_HEADER_SMALL
{
	char    FileSignature[28];     // ��Client UrlCache MMF Ver 5.2��
	DWORD    FileSize;             // index.dat�ļ��Ĵ�С
	DWORD    dwHashTableOffset;    // ��һ����ϣ���ƫ��,������ļ��Ŀ�ʼ��ַ
	DWORD    NumUrlInternalEntries;// δ֪
	DWORD    NumUrlEntriesAlloced; // δ֪
	DWORD    dwGarbage;            // δ֪
	LONGLONG CacheLimit;           // 8Byte
	LONGLONG CacheSize;            // 8Byte
	LONGLONG ExemptUsage;          // 8Byte
	DWORD    nDirCount;            // ��Ŀ¼����
	CacheDir DirArray[32];         // ��Ŀ¼����
	DWORD    dwHeaderData[33];     // δ֪
}MEMMAP_HEADER_SMALL, *LPMEMMAP_HEADER_SMALL;

typedef struct _MEMMAP_HEADER : _MEMMAP_HEADER_SMALL
{
	DWORD AllocationBitMap[3948];
}MEMMAP_HEADER, *LPMEMMAP_HEADER;

//////////////////////////////////////////////////////////////////////////
// Hash items
typedef struct _FILEMAP_ENTRY
{
	DWORD dwSig;      //��Ŀ��ʶ
	DWORD nBlocks;    //��Ŀռ�ö��ٸ��飨ÿ��128�ֽڣ�
}FILEMAP_ENTRY, *LPFILEMAP_ENTRY;

typedef struct _HASH_FILEMAP_ENTRY : _FILEMAP_ENTRY
{
	DWORD dwNext;     // ��һ����ϣ��ƫ�ƣ�0��ʾΪ���һ����,ƫ����index.dat�ļ���0�ֽ�Ϊ����ַ��
	DWORD nOrder;     // ����ϣ������кš���0��1��2����.
}HASH_FILEMAP_ENTRY, *LPHASH_FILEMAP_ENTRY;

typedef struct _HASH_ITEM
{
	DWORD dwHash;     //��ϣֵ��ע�����6λΪ0
	DWORD dwOffset;   //ָ���ʵ���еļ�¼���ֵ�ƫ��, ƫ����index.dat�ļ���0�ֽ�Ϊ����ַ��
}HASH_ITEM, *LPHASH_ITEM;

// LEAK is the same as URL
typedef struct _IE5_URL_FILEMAP_ENTRY : _FILEMAP_ENTRY
{
	LONGLONG LastModifiedTime;         // ����޸�ʱ��
	LONGLONG LastAccessedTime;         // ������ʱ��
	DWORD    dostExpireTime;           // ����ʱ��
	DWORD    dostPostCheckTime;

	DWORD    dwFileSize;               // Ӳ�̻����е��ļ��Ĵ�С
	DWORD    dwRedirHashItemOffset;    // ask DanpoZ
	DWORD    dwGroupOffset;
	union
	{
		DWORD  dwExemptDelta;          // for SIG_URL
		DWORD  dwNextLeak;             // for SIG_LEAK
	};

	DWORD    CopySize;                 // ��������0x60
	DWORD    UrlNameOffset;            // URL����ƫ�ơ�����ַ�Ǳ�URL��Ŀ�Ŀ�ʼ��ַ
	BYTE     DirIndex;                 // ���ڵ����ļ�������
	BYTE     bSyncState;               // automatic sync mode state
	BYTE     bVerCreate;               // ������ENTRY��CACHE�İ汾
	BYTE     bVerUpdate;               // ������ENTRY��CACHE�İ汾

	DWORD    InternalFileNameOffset;   // Ӳ�����ļ�����������Ŀ¼���ַ�����ƫ�ƣ�����ַ�Ǳ�URL��Ŀ�Ŀ�ʼ��ַ��
	DWORD    CacheEntryType;           // ��������
	DWORD    HeaderInfoOffset;         // ��WEB��������ȡ���ļ�ʱ�ķ��ص�HTTPͷ����Ϣ
	DWORD    HeaderInfoSize;           // HTTPͷ����С��ע��������Ļس����еģ�
	DWORD    FileExtensionOffset;      // should be WORD
	DWORD    dostLastSyncTime;      
	DWORD    NumAccessed;              // ��ȡ����������ʣ�
	DWORD    NumReferences;            // ���ô���
	DWORD    dostFileCreationTime;   
}IE5_URL_FILEMAP_ENTRY, *LPIE5_URL_FILEMAP_ENTRY;

class CookieHelper
{
public:
	explicit CookieHelper(void);
	~CookieHelper(void);

	const vector<CacheEntry>& getCacheEntVec() const { return m_recordsVec;}
	static const string getCookiePath(const char* name);
private:
	CookieHelper& operator=(const CookieHelper& obj);
	CookieHelper(const CookieHelper& obj);

private:
	void initializeEntriesVec();
	const string transformTimeFormat(LONGLONG dtime);
	void splitCookieURL(char* url, CacheEntry& obj);
private:
	vector<CacheEntry>	m_recordsVec;
	HANDLE				m_hFile;
	HANDLE				m_hMapping;
	LPSTR				m_startAddr;
};


#endif

