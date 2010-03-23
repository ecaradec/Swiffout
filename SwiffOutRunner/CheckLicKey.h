// TODO : directdraw is not queried
__forceinline bool CheckLicKey(char *lic, long N1, long N2)
{   
    long t;   memcpy(&t, lic, 4);
    short k1; memcpy(&k1, lic+4, 2);
    short k2; memcpy(&k2, lic+6, 2);

    long n1; memcpy(&n1, lic+8, 4);
    long n2; memcpy(&n2, lic+12, 4);

    short t1=t&0xFFFF;
    short t2=t>>16;

    n1^=(k2<<16)|k1;
    n2^=t;

    return n1==N1 && n2==N2;
}

__forceinline bool checkSwiffOutKey() {
    CRegKey r;
    r.Create(HKEY_CURRENT_USER, L"Software\\SwiffOut");  
    CLSID licence;
    r.QueryGUIDValue(L"licence",licence);
    return CheckLicKey((char*)&licence, 0x5fd7ef97, 0x97b8a7d2);
}

#define CHECKLICKEY checkSwiffOutKey()