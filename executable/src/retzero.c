__declspec(dllimport) void KPrint(const char *fmt, ...);

int __main()
{
    KPrint("Testing!\n");

    return 0;
}
